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
 * \file umac_mib.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_mib.c
 * \brief
 * This module implements read/write MIB functionality.
 * \ingroup Upper_MAC_Core
 * \date 27-Aug-2008
 */

/******************************************************************************
			    INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "section_wfm.h"
#include "wsm_api.h"
#include "umac_dev_conf.h"
#include "umac_dbg.h"
#include "umac_mib.h"
#include "umac_sm.h"
#include "umac_messages.h"
#include "umac_frames.h"
#include "umac_ll_if.h"
#include "umac_data_handler.h"

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_TemplateFrame_ProbeReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes probe request template frame to lmac.
 * \param UmacHandle -   Handle to the UMAC Instance.
 * \param PhyBand    -   The phyBand to be used.
 * \param Type       -   0 : SCAN and 1 : FIND
 * \returns WFM_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_TemplateFrame_ProbeReq(UMAC_HANDLE UmacHandle,
						uint8 PhyBand,
						uint8 Type)
{
	MIB_PARAMETERS *pMibParams;
	WSM_MIB_TEMPLATE_FRAME *pTemplateFrame;
	WFM_UMAC_MGMT_FRAME *pProbeReqFrame;
	uint16 FrameBodyLen;
	uint32 status = WFM_STATUS_SUCCESS;
	uint8 BssidDestn[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);
	/* Prepare pointers */
	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pTemplateFrame = (WSM_MIB_TEMPLATE_FRAME *) pMibParams->MibData;
	pProbeReqFrame = (WFM_UMAC_MGMT_FRAME *) pTemplateFrame->Frame;

	/* 1. Prepare Lowest layer */

	/* Create header */
	WFM_UMAC_CreateDot11FrameHdr_MoreGeneral(
						&pProbeReqFrame->dot11hdr,
						WFM_UMAC_MGMT,
						WFM_UMAC_MGMT_PROB_REQ,
						WFM_UMAC_NOT_TO_DS_FRAME,
						WFM_UMAC_NOT_PROTECTED_FRAME,
						BssidDestn, /* A1 is broadcast MAC address */
						pUmacInstance->MacAddress, /* A2 is our MAC address (it will be overwritten by WSM) */
						BssidDestn /* A3 is broadcast MAC address */
						);

	/* Now fill up body */
	FrameBodyLen = WFM_UMAC_CreateProbeReqTemplateFrame(
						UmacHandle,
						pProbeReqFrame->FrameBody,
						0,
						NULL,
						PhyBand,
						Type
						);
	/* 2. Prepare Middle layer */

	/*Set the template frame here */
	pTemplateFrame->FrameType = WSM_TMPLT_PRBREQ;	/*Probe Request */

	/* tbd: we can set bit7 to tell it to transmit in greenfield mode */
	/* this parameter will be overwritten when scan request is sent */
	pTemplateFrame->Rate = 0x00;

	pTemplateFrame->FrameLength = sizeof(pProbeReqFrame->dot11hdr) + FrameBodyLen;	/* dont include padding */

	LOG_BYTES(DBG_WFM_SCAN, "ProbeReqTemplate: ", (uint8 *) pProbeReqFrame, pTemplateFrame->FrameLength, pTemplateFrame->FrameLength);

	/* 3. Prepare Topmost layer */
	pMibParams->MibId = WSM_MIB_ID_TEMPLATE_FRAME;
	pMibParams->Length = SIZE_PADDED_TO_MAKE_4_BYTES_ALIGNED(pTemplateFrame->FrameLength + (sizeof(WSM_MIB_TEMPLATE_FRAME) - WSM_MAX_TEMPLATE_FRAME_SIZE));	/* size has to be 4 byte aligned */

	/* write mib for writing template frame */
	/*UMAC_LL_REQ_WRITE_MIB(UmacHandle,pMibParams)  ; */

	/*tbd: change is when wsm supports UMAC_LL_REQ_WRITE_MIB() for programming template frame */
	{
		TEMPLATE_FRAME LmacTemplateFrame;

		/* Copy Host Template Frame to LMAC Template Frame */
		LmacTemplateFrame.FrameLength = pTemplateFrame->FrameLength;
		LmacTemplateFrame.FrameType = pTemplateFrame->FrameType;
		LmacTemplateFrame.Rate = pTemplateFrame->Rate;
		LmacTemplateFrame.pFrame = (uint8 *) &(pTemplateFrame->Frame);

		/* Send Template Frame to LMAC. */
		UMAC_LL_REQ_SET_TEMPLATE_FRAME(
						UmacHandle,
						&LmacTemplateFrame,
						pMibParams
						);
	}

	return status;
} /* end WFM_UMAC_WriteMib_TemplateFrame_ProbeReq() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_RtsThreshold
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes Rts Threshold to lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param u32RtsThreshold   -   Value of Rts Threshold to use.
 * \returns WFM_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_RtsThreshold(UMAC_HANDLE UmacHandle,
				      uint32 u32RtsThreshold)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	OS_ASSERT(pUmacInstance);

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	pMibParams->MibId = WSM_MIB_ID_DOT11_RTS_THRESHOLD;
	pMibParams->Length = sizeof(uint32);
	*(uint32 *) pMibParams->MibData = u32RtsThreshold;

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	return status;
} /* end WFM_UMAC_WriteMib_RtsThreshold() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_NonErpProtection
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function enables/disables non-erp protection in lmac
 * \param UmacHandle		    -   Handle to the UMAC Instance.
 * \param u32UseNonErpProtection    -   TRUE (1) (use cts/rts),
 *				        FALSE (0) (dont use cts/rts).
 * \returns WFM_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_NonErpProtection(UMAC_HANDLE UmacHandle,
					  uint32 u32UseNonErpProtection)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	OS_ASSERT(pUmacInstance);

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	pMibParams->MibId = WSM_MIB_ID_NON_ERP_PROTECTION;
	pMibParams->Length = sizeof(uint32);
	/* Value true (1) /false (0) */
	*(uint32 *) pMibParams->MibData = u32UseNonErpProtection;

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	LOG_DATA(DBG_WFM_UMAC, "ERP Prot=%d\n", u32UseNonErpProtection);

	return status;
} /* end WFM_UMAC_WriteMib_NonErpProtection() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_SetHTProtection
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets the SetHtProtection MIB in lmac
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param u32setHTProtection	-   contains the parameters related to HT
 *                                  Protection
 * \returns WFM_STATUS_CODE	    A value of 0 indicates a success.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_SetHTProtection(UMAC_HANDLE UmacHandle,
					 uint32 u32setHTProtection)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	OS_ASSERT(pUmacInstance);

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	pMibParams->MibId = WSM_MIB_ID_SET_HT_PROTECTION;
	pMibParams->Length = sizeof(uint32);
	/* Value true (1) /false (0) */
	*(uint32 *) pMibParams->MibData = u32setHTProtection;

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_BlockAckPolicy
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets block ack policy for tx/rx directions in lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param u8BATxTidPolicy	-   Value of 0x00-0xff.
 * \param u8BARxTidPolicy       -   Value of 0x00-0xff.
 * \returns WFM_STATUS_CODE         A value of 0 indicates a success.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_BlockAckPolicy(UMAC_HANDLE UmacHandle,
					uint8 u8BATxTidPolicy,
					uint8 u8BARxTidPolicy)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	OS_ASSERT(pUmacInstance);

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	pMibParams->MibId = WSM_MIB_ID_BLOCK_ACK_POLICY;
	pMibParams->Length = sizeof(uint32);
	pMibParams->MibData[0] = u8BATxTidPolicy;
	pMibParams->MibData[1] = 0x00;	/* Reserved */
	pMibParams->MibData[2] = u8BARxTidPolicy;
	pMibParams->MibData[3] = 0x00;	/* Reserved */

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	return status;

} /* end WFM_UMAC_WriteMib_BlockAckPolicy() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_BeaconFilterTable
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes to beacon filter table.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param SetBcnFilterTable	-   Set or reset beacon filter table, 1 or 0.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_BeaconFilterTable(UMAC_HANDLE UmacHandle,
					   uint8 SetBcnFilterTable)
{

	MIB_PARAMETERS *pMibParams;
	WSM_MIB_BCN_FILTER_TABLE *pBcnFilterTbl;
	uint8 p2pOui[3] = {0x50, 0x6F, 0x9A};
	uint8 IeIdList[] = {
		UMAC_IE_ELEMENT_SSID,
		UMAC_IE_ELEMENT_SUPPORTED_RATES,
		UMAC_IE_ELEMENT_EXTENDED_SUPPORTED_RATES,
		UMAC_IE_ELEMENT_POWER_CAPABILITY,
		UMAC_IE_ELEMENT_SUPPORTED_CHANNELS,
		UMAC_IE_ELEMENT_CHANNEL_SWITCH,
		UMAC_IE_ELEMENT_ERP_INFORMATION,
		UMAC_IE_ELEMENT_RSN,
		UMAC_IE_ELEMENT_QOS_CAPABILITY,
		UMAC_IE_ELEMENT_HT_CAPABILITIES,
		UMAC_IE_ELEMENT_HT_OPERATION,
		UMAC_IE_ELEMENT_VENDOR_SPECIFIC
	};
	int Idx;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;

	OS_ASSERT(pUmacInstance);

	/* Prepare pointers */
	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	pBcnFilterTbl = (WSM_MIB_BCN_FILTER_TABLE *) &pMibParams->MibData[0];

	/* Prepare Bottom layer */
	if (SetBcnFilterTable == TRUE) {
		/*to program a new beacon filter table */
		pBcnFilterTbl->NumOfInfoElmts = sizeof(IeIdList);
		for (Idx = 0; Idx < sizeof(IeIdList); Idx++) {
			pBcnFilterTbl->IeTable[Idx].IeId = IeIdList[Idx];
			pBcnFilterTbl->IeTable[Idx].ActionFlags = (IE_F_CHANGED | IE_F_NO_LONGER_PRESENT | IE_F_APPEARED);
			if (pBcnFilterTbl->IeTable[Idx].IeId == UMAC_IE_ELEMENT_VENDOR_SPECIFIC) {
				OS_MemoryCopy(&pBcnFilterTbl->IeTable[Idx].Oui,
					      &p2pOui,
					      3);
			}
		}
	} else {
		/*to clear wsm's existing beacon filter table */
		pBcnFilterTbl->NumOfInfoElmts = 0;
	}

	/* Prepare Top layer */
	pMibParams->MibId = WSM_MIB_ID_BEACON_FILTER_TABLE;
	pMibParams->Length = (uint16) (sizeof(pBcnFilterTbl->NumOfInfoElmts)
				       + (pBcnFilterTbl->NumOfInfoElmts * sizeof(WSM_IE_TABLE)));

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	return status;

} /* end - WFM_UMAC_WriteMib_BeaconFilterTable() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_BeaconFilterEnable
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function enables/disables beacon filtering.
 * \param UmacHandle	    - Handle to the UMAC Instance.
 * \param SetBcnFilterTable - Set or reset beacon filter table, 1 or 0.
 * \param u32BeaconCount    - Nth beacon sent to host when u32BeaconFilterEnable
 *			      is 0
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_BeaconFilterEnable(UMAC_HANDLE UmacHandle,
					    uint32 u32BeaconFilterEnable,
					    uint32 u32BeaconCount)
{
	MIB_PARAMETERS *pMibParams;
	WSM_MIB_BCN_FILTER_ENABLE *pBcnFilterEn;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;

	OS_ASSERT(pUmacInstance);

	/* Prepare pointers */
	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pBcnFilterEn = (WSM_MIB_BCN_FILTER_ENABLE *) pMibParams->MibData;

	/* Prepare Bottom layer */
	pBcnFilterEn->Enable = u32BeaconFilterEnable;
	pBcnFilterEn->BcnCount = u32BeaconCount;

	/* Prepare Top layer */
	pMibParams->MibId = WSM_MIB_ID_BEACON_FILTER_ENABLE;
	pMibParams->Length = sizeof(WSM_MIB_BCN_FILTER_ENABLE);

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	return status;

} /* end - WFM_UMAC_WriteMib_BeaconFilterTable() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_RemoveKey
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function deletes a key from WSM.
 * \param UmacHandle			-   Handle to the UMAC Instance.
 * \param u8KeyEntryIndex		-   Key number to be deleted.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_RemoveKey(UMAC_HANDLE UmacHandle,
				   uint8 u8KeyEntryIndex)
{
	/*/WFM_UMAC_INSTANCE  *pUmacInstance  = (WFM_UMAC_INSTANCE *)UmacHandle ; */
	uint32 status = WFM_STATUS_SUCCESS;

	UMAC_LL_REQ_REMOVE_KEY_ENTRY(
					UmacHandle,
					u8KeyEntryIndex,
					DEFAULT_LINK_ID
					);

	return status;
} /* end WFM_UMAC_WriteMib_RemoveKey() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_SlotTime
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs Slot Time in Lmac.
 * \param UmacHandle	   -   Handle to the UMAC Instance.
 * \param u32SlotTime	   -   Slot Time value in microseconds.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_SlotTime(UMAC_HANDLE UmacHandle, uint32 u32SlotTime)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	OS_ASSERT(pUmacInstance);

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_DOT11_SLOT_TIME;
	pMibParams->Length = sizeof(uint32);
	*(uint32 *) pMibParams->MibData = u32SlotTime;

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	LOG_DATA(DBG_WFM_UMAC, "WFM_UMAC_WriteMib_SlotTime(): Slot Time =%d\n", u32SlotTime);

	return status;

} /* end WFM_UMAC_WriteMib_SlotTime() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UpdateJoinedMode_All
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes Mib for Joining.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param Flags			-   Flags.
 * \param PreambleType		-   Preamble Type.
 * \param MixedOrGreenfieldType	-   MixedOrGreenfieldType.
 * \param MpduStartSpacing      -   MpduStartSpacing.
 * \param BasicRateSet		-   BasicRateSet.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UpdateJoinedMode_All(UMAC_HANDLE UmacHandle,
					      uint8 Flags,
					      uint8 PreambleType,
					      uint8 MixedOrGreenfieldType,
					      uint8 MpduStartSpacing,
					      uint32 BasicRateSet)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;

	MIB_PARAMETERS *pMibParams;
	WSM_MIB_SET_ASSOCIATION_MODE *pUpdateJoinMode;

	OS_ASSERT(pUmacInstance);

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	LOG_DATA(DBG_WFM_UMAC | DBG_WFM_CURRENT_AK, "WFM_UMAC_WriteMib_UpdateJoinedMode_All(): Flag=0x%x\n", Flags);

	LOG_DATA3(DBG_WFM_UMAC | DBG_WFM_CURRENT_AK, "--PreambleType =%d, MixedGreenfield=%d, BasicRateSet=0x%x\n", PreambleType, MixedOrGreenfieldType, BasicRateSet);

	/* Prepare pointers */
	pUpdateJoinMode = (WSM_MIB_SET_ASSOCIATION_MODE *) pMibParams->MibData;

	/* Prepare Bottom layer */
	pUpdateJoinMode->Flags = Flags;
	pUpdateJoinMode->PreambleType = PreambleType;
	pUpdateJoinMode->MixedOrGreenfieldType = MixedOrGreenfieldType;
	pUpdateJoinMode->BasicRateSet = BasicRateSet;

	/* Prepare Top layer */
	pMibParams->MibId = WSM_MIB_ID_SET_ASSOCIATION_MODE;
	pMibParams->Length = sizeof(WSM_MIB_SET_ASSOCIATION_MODE);

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	return status;

} /* end WFM_UMAC_WriteMib_UpdateJoinedMode_All() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UpdateJoinedMode_PreambleType
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets preamble type after joining.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param u8PreambleType	-   Preamble Type to be used.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UpdateJoinedMode_PreambleType(UMAC_HANDLE UmacHandle,
						       uint8 u8PreambleType)
{
	return WFM_UMAC_WriteMib_UpdateJoinedMode_All(
				UmacHandle,
				SAM_F__PREAMBLE_ENABLE,	/* Flags */
				u8PreambleType,	/* PreambleType */
				0, /* MixedOrGreenfieldType - not used */
				0, /* MpduStartSpacing - not used */
				0  /* BasicRateSet - not used */
				);
} /* end WFM_UMAC_WriteMib_UpdateJoinedMode_PreambleType() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UpdateJoinedMode_MixedOrGreenfieldMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets whether to use mixed mode or greenfield mode
 * for auto responses/retried frames after join.
 * \param UmacHandle		  -   Handle to the UMAC Instance.
 * \param u8MixedOrGreenfieldMode -   Use mixed mode or greenfield mode
 *				      (WSM_UJM_MIXED_MODE => Mixed,
 *				      WSM_UJM_GREENFIELD_MODE => greenfield).
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UpdateJoinedMode_MixedOrGreenfieldMode(UMAC_HANDLE UmacHandle,
								uint8 u8MixedOrGreenfieldMode)
{
	return WFM_UMAC_WriteMib_UpdateJoinedMode_All(
				UmacHandle,
				SAM_F__MIXED_GREEN_ENABLE, /* Flags */
				0, /* PreambleType - not used */
				u8MixedOrGreenfieldMode, /* MixedOrGreenfieldType */
				0, /* MpduStartSpacing - not used */
				0  /* BasicRateSet - not used */
				);
} /* end WFM_UMAC_WriteMib_UpdateJoinedMode_MixedOrGreenfieldMode() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UpdateJoinedMode_MpduStartSpacing
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function updates MPDU spacing after join.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param u8MpduStartSpacing	-   MPDU spacing to be used.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UpdateJoinedMode_MpduStartSpacing(UMAC_HANDLE UmacHandle,
							   uint8 u8MpduStartSpacing)
{
	return WFM_UMAC_WriteMib_UpdateJoinedMode_All(
				UmacHandle,
				SAM_F__MPDUSTARTSPACING_ENABLE,	/* Flags */
				0, /* PreambleType - not used */
				0, /* MixedOrGreenfieldType - not used */
				u8MpduStartSpacing, /* MpduStartSpacing */
				0  /* BasicRateSet - not used */
				);
} /* end WFM_UMAC_WriteMib_UpdateJoinedMode_MpduStartSpacing() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UpdateJoinedMode_BasicRateSet
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function changes basic rate set to be used after joining.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param u8BasicRateSet	-   Use the specified basic rate set.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UpdateJoinedMode_BasicRateSet(UMAC_HANDLE UmacHandle,
						       uint32 u8BasicRateSet)
{

	return WFM_UMAC_WriteMib_UpdateJoinedMode_All(
				UmacHandle,
				SAM_F__BASICRATESET_ENABLE, /* Flags */
				0, /* PreambleType - not used */
				0, /* MixedOrGreenfieldType - not used */
				0, /* MpduStartSpacing - not used */
				u8BasicRateSet /* BasicRateSet */
				);

} /* end WFM_UMAC_WriteMib_UpdateJoinedMode_BasicRateSet() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_WepDefaultKeyId
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs default KeyId for WEP into WSM.
 * \param UmacHandle	    -   Handle to the UMAC Instance.
 * \param pDefKeyId	    -	Default KeyId to be set.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_WepDefaultKeyId(UMAC_HANDLE UmacHandle,
					 uint8 *pDefKeyId)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;
	WSM_MIB_WEP_DEFAULT_KEY_ID *pWepDefKeyId;

	OS_ASSERT(pUmacInstance);

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	/* Prepare pointers */
	pWepDefKeyId = (WSM_MIB_WEP_DEFAULT_KEY_ID *) pMibParams->MibData;

	/* Prepare Bottom layer */
	OS_MemoryCopy(pWepDefKeyId, pDefKeyId, sizeof(WSM_MIB_WEP_DEFAULT_KEY_ID));

	LOG_DATA(DBG_WFM_ENCRYPTION, "WFM_UMAC_WriteMib_WepDefaultKeyId(): DefKeyId=0x%x\n", pWepDefKeyId->WepDefaultKeyId);

	/* Prepare Top layer */
	pMibParams->MibId = WSM_MIB_ID_DOT11_WEP_DEFAULT_KEY_ID;
	pMibParams->Length = sizeof(WSM_MIB_WEP_DEFAULT_KEY_ID);

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	return status;
} /* end WFM_UMAC_WriteMib_WepDefaultKeyId() */

/******************************************************************************
 * NAME:	WFM_UMAC_ReadMib_MacAddress
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function enables/disables non-erp protection in lmac.
 * \param UmacHandle	    -   Handle to the UMAC Instance.
 * \param *pu8MacAddress    -	MAC Address to be returned from LMAC.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_ReadMib_MacAddress(UMAC_HANDLE UmacHandle, uint8 *pu8MacAddress)
{

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;

	WFM_READ_MIB_PARAMETERS *pMibParams;

	OS_ASSERT(pUmacInstance);

	pMibParams = (WFM_READ_MIB_PARAMETERS *)
	    UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	pMibParams->MibId = WSM_MIB_ID_DOT11_STATION_ID;
	pMibParams->Reserved = 0x00;
	UMAC_LL_REQ_READ_MIB(UmacHandle, (MIB_PARAMETERS *) pMibParams);

	return status;
} /* end WFM_UMAC_ReadMib_MacAddress() */

/******************************************************************************
 * NAME:	UMAC_Read_TSF_Timer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function reads the TSF timer
 * \param UmacHandle	    -   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 UMAC_Read_TSF_Timer(UMAC_HANDLE UmacHandle)
{

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;

	WFM_READ_MIB_PARAMETERS *pMibParams;

	OS_ASSERT(pUmacInstance);

	pMibParams = (WFM_READ_MIB_PARAMETERS *)
	    UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	pMibParams->MibId = WSM_MIB_ID_TSF_COUNTER;
	pMibParams->Reserved = 0x00;
	UMAC_LL_REQ_READ_MIB(UmacHandle, (MIB_PARAMETERS *) pMibParams);

	return status;
} /* end WFM_UMAC_ReadMib_MacAddress() */

/******************************************************************************
 * NAME:	WFM_UMAC_SendReqToWsm_TxQueueParams
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets Tx Queue parameters into wsm for a queue id.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param QueueId	-	Queue Id to be programmed.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_SendReqToWsm_TxQueueParams(UMAC_HANDLE UmacHandle,
					   uint8 QueueId)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	QUEUE_PARAMS *pQueueParams = (QUEUE_PARAMS *)
	    UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	OS_ASSERT(pUmacInstance);

	OS_ASSERT(QueueId < WFM_UMAC_NUM_TX_QUEUES);

	pQueueParams->QueueId = QueueId;
	pQueueParams->AllowedMediumTime = 0;
	pQueueParams->AckPolicy = pUmacInstance->TxQueueParam[QueueId].AckPolicy;
	pQueueParams->MaxTransmitLifetime = pUmacInstance->TxQueueParam[QueueId].MaxTransmitLifetime;

	/* Send the Queue parameter set to LMAC */
	UMAC_LL_REQ_SET_TX_QUEUE_PARAMS(UmacHandle, pQueueParams);

	return status;
} /* end - WFM_UMAC_SendReqToWsm_TxQueueParams() */

/******************************************************************************
 * NAME:	WFM_UMAC_SendReqToWsm_TxQueueParams_All
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets all Tx Queue parameters into wsm.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_SendReqToWsm_TxQueueParams_All(UMAC_HANDLE UmacHandle)
{
	uint16 QueueId;
	uint32 status;

	for (QueueId = 0; QueueId < WFM_UMAC_NUM_TX_QUEUES; QueueId++) {
		status = WFM_UMAC_SendReqToWsm_TxQueueParams(
								UmacHandle,
								(uint8) QueueId
								);
		if (status)
			break;
	} /* for(QueueId=0; QueueId<WFM_UMAC_NUM_TX_QUEUES; QueueId++)*/

	return status;
} /* end - WFM_UMAC_SendReqToWsm_TxQueueParams_All() */

/******************************************************************************
 * NAME:	WFM_UMAC_SendReqToWsm_SetBssParams
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets Bss Parameters.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param Falgs		-   Flags to indicate which mode firmware has to use.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_SendReqToWsm_SetBssParams(UMAC_HANDLE UmacHandle, uint8 Flags)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	BSS_PARAMETERS *pBssParameters = (BSS_PARAMETERS *)
	    UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	OS_ASSERT(pUmacInstance);

	pBssParameters->Falgs = Flags;

	if (pUmacInstance->gVars.p.BeaconLostCount.BeaconLostCount)
		pBssParameters->BeaconLostCount = pUmacInstance->gVars.p.BeaconLostCount.BeaconLostCount;
	else
		pBssParameters->BeaconLostCount = WFM_DEFAULT_BECON_LOST_COUNT;
	/* Reset last 2 MSB bits of Association ID */
	pBssParameters->AID = pUmacInstance->AssociationId & 0x3FFF;

	pBssParameters->OperationalRateSet = pUmacInstance->HtOfdmRates
						|pUmacInstance->ErpOfdmRates
						|pUmacInstance->ErpDssCckRates;

	LOG_DATA(DBG_WFM_UMAC, "OperationalRateSet=0x%x\n", pBssParameters->OperationalRateSet);

	UMAC_LL_REQ_SYN_BSS_PARAMS(UmacHandle, pBssParameters);

	return status;
} /* end WFM_UMAC_SendReqToWsm_SetBssParams() */

/******************************************************************************
 * NAME:	WFM_UMAC_SendReqToWsm_SetPsMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets PS mode in WSM.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param u8PsMode	-   Ps Mode (WFM_PS_MODE_DISABLED/WFM_PS_MODE_ENABLED).
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_SendReqToWsm_SetPsMode(UMAC_HANDLE UmacHandle,
				       UMAC_POWER_MODE *pPsMode)
{
	uint32 status = WFM_STATUS_SUCCESS;

	UMAC_LL_REQ_PS_POWER_MGMT_MODE(UmacHandle, pPsMode);

	return status;
} /* end WFM_UMAC_SendReqToWsm_SetPsMode() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_TemplateFrame_NullData
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes null data template frame to lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param IsQoS		-   1 -> QoS NULL Frame , 0 -> Non-QoS NULL Frame.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_TemplateFrame_NullData(UMAC_HANDLE UmacHandle,
						uint8 IsQoS)
{
	MIB_PARAMETERS *pMibParams;
	WSM_MIB_TEMPLATE_FRAME *pTemplateFrame;
	WFM_UMAC_DATA_FRAME *pDataFrame;
	WFM_UMAC_QOS_DATA_FRAME *pQoSDataFrame;
	uint16 FrameBodyLen;
	uint8 SubType;
	uint8 FrameType;

	uint32 status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);
	/* Prepare pointers */
	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pTemplateFrame = (WSM_MIB_TEMPLATE_FRAME *) pMibParams->MibData;
	pDataFrame = (WFM_UMAC_DATA_FRAME *) pTemplateFrame->Frame;

	if (IsQoS) {
		SubType = WFM_UMAC_SUB_NULL_QOS;
		FrameType = WSM_TMPLT_QOSNUL;

	}			/*if(IsQoS) */
	else {
		SubType = WFM_UMAC_SUB_DATA_NULL;
		FrameType = WSM_TMPLT_NULL;
	}

	/* 1. Prepare Lowest layer */

	/* Create header */
	WFM_UMAC_CreateDot11FrameHdr(
				UmacHandle,
				WFM_UMAC_DATA,
				SubType,
				&pDataFrame->dot11hdr,
				pUmacInstance->RemoteMacAdd,
				pUmacInstance->MacAddress,
				0,
				0
				);

	/* Now fill up body */
	FrameBodyLen = 0;

	/* 2. Prepare Middle layer */

	/*Set the template frame here */
	pTemplateFrame->FrameType = FrameType;	/* Null Data */

	/* TBD */
	pTemplateFrame->Rate = 0xFF;
	/* dont include padding */
	pTemplateFrame->FrameLength = sizeof(pDataFrame->dot11hdr) + FrameBodyLen;

	if (IsQoS) {
		pQoSDataFrame = (WFM_UMAC_QOS_DATA_FRAME *) pDataFrame;
		pQoSDataFrame->QoSCntrl = 0;
		pTemplateFrame->FrameLength += sizeof(pQoSDataFrame->QoSCntrl);
	} /* if(IsQoS) */

	/* 3. Prepare Topmost layer */
	pMibParams->MibId = WSM_MIB_ID_TEMPLATE_FRAME;
	pMibParams->Length = SIZE_PADDED_TO_MAKE_4_BYTES_ALIGNED(pTemplateFrame->FrameLength + (sizeof(WSM_MIB_TEMPLATE_FRAME) - WSM_MAX_TEMPLATE_FRAME_SIZE)
	    ); /* size has to be 4 byte aligned */

	/* write mib for writing template frame */
	/*UMAC_LL_REQ_WRITE_MIB(UmacHandle,pMibParams)  ; */

	/*tbd: change is when wsm supports UMAC_LL_REQ_WRITE_MIB() for programming template frame */
	{
		TEMPLATE_FRAME LmacTemplateFrame;

		/* Copy Host Template Frame to LMAC Template Frame */
		LmacTemplateFrame.FrameLength = pTemplateFrame->FrameLength;
		LmacTemplateFrame.FrameType = pTemplateFrame->FrameType;
		LmacTemplateFrame.Rate = pTemplateFrame->Rate;
		LmacTemplateFrame.pFrame = (uint8 *) &(pTemplateFrame->Frame);

		/* Send Template Frame to LMAC. */
		UMAC_LL_REQ_SET_TEMPLATE_FRAME(
						UmacHandle,
						&LmacTemplateFrame,
						pMibParams
						);
	}
	return status;
} /* end WFM_UMAC_WriteMib_TemplateFrame_NullData() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_TemplateFrame_PsPoll
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes PsPoll template frame to lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_TemplateFrame_PsPoll(UMAC_HANDLE UmacHandle)
{
	MIB_PARAMETERS *pMibParams;
	WSM_MIB_TEMPLATE_FRAME *pTemplateFrame;
	WFM_UMAC_CTRL_PS_POLL_FRAME *pPsPollFrame;
	uint16 FCtrl;
	uint32 status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);
	/* Prepare pointers */
	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pTemplateFrame = (WSM_MIB_TEMPLATE_FRAME *) pMibParams->MibData;
	pPsPollFrame = (WFM_UMAC_CTRL_PS_POLL_FRAME *) pTemplateFrame->Frame;

	/* 1. Prepare Lowest layer */

	/* Create header */
	FCtrl = ((WFM_UMAC_CTRL & 0x0003) << 2);	/*Type */
	FCtrl |= ((WFM_UMAC_CNTRL_PS_POLL & 0x000F) << 4);	/*SubType */

	pPsPollFrame->FrameControl = umc_cpu_to_le16(FCtrl);

	/*For PS-Poll frame last 2 msb bit should set to 1 */
	pPsPollFrame->Aid = pUmacInstance->AssociationId | 0xC000;

	D0_ADDR_COPY(pPsPollFrame->RxAddr, pUmacInstance->RemoteMacAdd);
	D0_ADDR_COPY(pPsPollFrame->TrAddr, pUmacInstance->MacAddress);

	/* 2. Prepare Middle layer */

	/*Set the template frame here */
	pTemplateFrame->FrameType = WSM_TMPLT_PSPOLL;	/* Ps Poll */

	/* TBD */
	pTemplateFrame->Rate = 0xFF;
	/* dont include padding */
	pTemplateFrame->FrameLength = sizeof(WFM_UMAC_CTRL_PS_POLL_FRAME);

	/* 3. Prepare Topmost layer */
	pMibParams->MibId = WSM_MIB_ID_TEMPLATE_FRAME;
	pMibParams->Length = SIZE_PADDED_TO_MAKE_4_BYTES_ALIGNED(pTemplateFrame->FrameLength + (sizeof(WSM_MIB_TEMPLATE_FRAME) - WSM_MAX_TEMPLATE_FRAME_SIZE));	/* size has to be 4 byte aligned */

	/* write mib for writing template frame */
	/*UMAC_LL_REQ_WRITE_MIB(UmacHandle,pMibParams)  ; */

	/*tbd: change is when wsm supports UMAC_LL_REQ_WRITE_MIB() for programming template frame */
	{
		TEMPLATE_FRAME LmacTemplateFrame;

		/* Copy Host Template Frame to LMAC Template Frame */
		LmacTemplateFrame.FrameLength = pTemplateFrame->FrameLength;
		LmacTemplateFrame.FrameType = pTemplateFrame->FrameType;
		LmacTemplateFrame.Rate = pTemplateFrame->Rate;
		LmacTemplateFrame.pFrame = (uint8 *) &(pTemplateFrame->Frame);

		/* Send Template Frame to LMAC. */
		UMAC_LL_REQ_SET_TEMPLATE_FRAME(
						UmacHandle,
						&LmacTemplateFrame,
						pMibParams
						);
	}
	return status;
} /* end WFM_UMAC_WriteMib_TemplateFrame_PsPoll() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_EPTAConfigData
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function updates EPTA config data to LMAC.
 * \param UmacHandle	- Handle to the UMAC Instance.
 * \param *pconfigData	- Pointer to the EPTA config data.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_EPTAConfigData(UMAC_HANDLE UmacHandle,
					WFM_UPDATE_EPTA_CONFIG_DATA *pconfigData)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	OS_ASSERT(pUmacInstance);

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	pMibParams->MibId = WSM_MIB_ID_UPDATE_EPTA_CONFIG_DATA;
	pMibParams->Length = sizeof(WFM_UPDATE_EPTA_CONFIG_DATA);
	OS_MemoryCopy(
			pMibParams->MibData,
			(uint8 *) pconfigData,
			sizeof(WFM_UPDATE_EPTA_CONFIG_DATA)
			);

	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	return status;

} /* end WFM_UMAC_WriteMib_EPTAConfigData() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_SetAutoCalibrationMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets auto calibration mode to lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param autoCalMode	-   Value of Auto calibration to use.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_SetAutoCalibrationMode(UMAC_HANDLE UmacHandle,
						uint32 autoCalMode)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	OS_ASSERT(pUmacInstance);

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	pMibParams->MibId = WSM_MIB_ID_SET_AUTO_CALIBRATION_MODE;
	pMibParams->Length = sizeof(uint32);
	*(uint32 *) pMibParams->MibData = autoCalMode;

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	return status;
} /* end WFM_UMAC_WriteMib_SetAutoCalibrationMode() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_PowerLevel
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs Power Level in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param powerLevel	-   Power Level value in 0.1 dBm.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_PowerLevel(UMAC_HANDLE UmacHandle, sint32 powerLevel)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	OS_ASSERT(pUmacInstance);

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_DOT11_CURRENT_TX_POWER_LEVEL;
	pMibParams->Length = sizeof(sint32);
	*(uint32 *) pMibParams->MibData = powerLevel;

	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	LOG_DATA(DBG_WFM_UMAC, "WFM_UMAC_WriteMib_PowerLevel(): Power Level =%d\n", powerLevel);

	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_TxRetryRatePolicy
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs rate retry policy in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param policyIndex	-   Index of the policy.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_TxRetryRatePolicy(UMAC_HANDLE UmacHandle,
					   uint8 policyIndex)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;
	WSM_MIB_SET_TX_RATE_RETRY_POLICY *pSetTxRateRetryPolicy;
	WSM_MIB_TX_RATE_RETRY_POLICY *pTxRateRetryPolicy;

	OS_ASSERT(pUmacInstance);

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_SET_TX_RATE_RETRY_POLICY;
	pMibParams->Length = sizeof(WSM_MIB_SET_TX_RATE_RETRY_POLICY);

	pSetTxRateRetryPolicy = (WSM_MIB_SET_TX_RATE_RETRY_POLICY *) &pMibParams->MibData;

	pSetTxRateRetryPolicy->NumTxRatePolicies = 1;

	pTxRateRetryPolicy = &pSetTxRateRetryPolicy->TxRateRetryPolicy;

	/* Management frames */
	pTxRateRetryPolicy->PolicyIndex = policyIndex;
	pTxRateRetryPolicy->ShortRetryCount = 7;
	pTxRateRetryPolicy->LongRetryCount = 4;

	pTxRateRetryPolicy->PolicyFlags = 0x00;
	pTxRateRetryPolicy->RateRecoveryCount = 0;

	pTxRateRetryPolicy->RateCountIndices_07_00 = 0;
	pTxRateRetryPolicy->RateCountIndices_15_08 = 0;
	pTxRateRetryPolicy->RateCountIndices_23_16 = 0;
	WFM_UMAC_UpdateRatePolicy(UmacHandle, pTxRateRetryPolicy, policyIndex);

	LOG_DATA3(DBG_WFM_UMAC, "\n RateCountIndices_07_00=0x%x, RateCountIndices_15_08=0x%x, RateCountIndices_23_16=0x%x\n", pTxRateRetryPolicy->RateCountIndices_07_00, pTxRateRetryPolicy->RateCountIndices_15_08, pTxRateRetryPolicy->RateCountIndices_23_16);

	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_UpdateRatePolicy
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function updates WSM_MIB_TX_RATE_RETRY_POLICY structure.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param *pTxRateRetryPolicy	-   Pointer to the Rate retry policy.
 * \returns none.
 *****************************************************************************/

void WFM_UMAC_UpdateRatePolicy(UMAC_HANDLE UmacHandle,
			       WSM_MIB_TX_RATE_RETRY_POLICY *pTxRateRetryPolicy,
			       uint8 linkId)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 prevIndex = RATE_INDEX_N_65M + 1;
	uint32 currIndex;

	currIndex = WFM_UMAC_GET_HIGHEST_RATE_INDEX(pUmacInstance->TxRetryRateBitMap[linkId]);
	while (currIndex != prevIndex) {
		prevIndex = currIndex;

		switch (currIndex) {
		case 0:
			pTxRateRetryPolicy->RateCountIndices_07_00 |= 0x00000001;
			break;
		case 1:
			pTxRateRetryPolicy->RateCountIndices_07_00 |= 0x00000010;
			break;
		case 2:
			pTxRateRetryPolicy->RateCountIndices_07_00 |= 0x00000100;
			break;
		case 3:
			pTxRateRetryPolicy->RateCountIndices_07_00 |= 0x00001000;
			break;
		case 4:
			pTxRateRetryPolicy->RateCountIndices_07_00 |= 0x00010000;
			break;
		case 5:
			pTxRateRetryPolicy->RateCountIndices_07_00 |= 0x00100000;
			break;
		case 6:
			pTxRateRetryPolicy->RateCountIndices_07_00 |= 0x01000000;
			break;
		case 7:
			pTxRateRetryPolicy->RateCountIndices_07_00 |= 0x10000000;
			break;

		case 8:
			pTxRateRetryPolicy->RateCountIndices_15_08 |= 0x00000001;
			break;
		case 9:
			pTxRateRetryPolicy->RateCountIndices_15_08 |= 0x00000010;
			break;
		case 10:
			pTxRateRetryPolicy->RateCountIndices_15_08 |= 0x00000100;
			break;
		case 11:
			pTxRateRetryPolicy->RateCountIndices_15_08 |= 0x00001000;
			break;
		case 12:
			pTxRateRetryPolicy->RateCountIndices_15_08 |= 0x00010000;
			break;
		case 13:
			pTxRateRetryPolicy->RateCountIndices_15_08 |= 0x00100000;
			break;
		case 14:
			pTxRateRetryPolicy->RateCountIndices_15_08 |= 0x01000000;
			break;
		case 15:
			pTxRateRetryPolicy->RateCountIndices_15_08 |= 0x10000000;
			break;

		case 16:
			pTxRateRetryPolicy->RateCountIndices_23_16 |= 0x00000001;
			break;
		case 17:
			pTxRateRetryPolicy->RateCountIndices_23_16 |= 0x00000010;
			break;
		case 18:
			pTxRateRetryPolicy->RateCountIndices_23_16 |= 0x00000100;
			break;
		case 19:
			pTxRateRetryPolicy->RateCountIndices_23_16 |= 0x00001000;
			break;
		case 20:
			pTxRateRetryPolicy->RateCountIndices_23_16 |= 0x00010000;
			break;
		case 21:
			pTxRateRetryPolicy->RateCountIndices_23_16 |= 0x00100000;
			break;
		case 22:
			pTxRateRetryPolicy->RateCountIndices_23_16 |= 0x01000000;
			break;
		case 23:
			pTxRateRetryPolicy->RateCountIndices_23_16 |= 0x10000000;
			break;

		}

		/* Only Host controlled policy */
		currIndex = WFM_UMAC_GET_NEXT_LOWER_RATE_INDEX(pUmacInstance->TxRetryRateBitMap[linkId], currIndex);
	}
}

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_BeaconWakeUpPeriod
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function set DTIM perod and listen interval.
 * \param UmacHandle	    - Handle to the UMAC Instance.
 * \param NumBeaconPeriods  - DTIM period recived from TIM information element.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_BeaconWakeUpPeriod(UMAC_HANDLE UmacHandle,
					    uint8 NumBeaconPeriods)
{
	MIB_PARAMETERS *pMibParams;
	WSM_MIB_BEACON_WAKEUP_PERIOD *pBcnWakeUpPeriod;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;

	OS_ASSERT(pUmacInstance);

	/* Prepare pointers */
	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pBcnWakeUpPeriod = (WSM_MIB_BEACON_WAKEUP_PERIOD *) pMibParams->MibData;

	/* Prepare Bottom layer */
	pBcnWakeUpPeriod->NumBeaconPeriods = NumBeaconPeriods;
	pBcnWakeUpPeriod->ListenInterval = 0;

	/* Prepare Top layer */
	pMibParams->MibId = WSM_MIB_ID_BEACON_WAKEUP_PERIOD;
	pMibParams->Length = sizeof(WSM_MIB_BEACON_WAKEUP_PERIOD);

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	return status;
} /* end - WFM_UMAC_WriteMib_BeaconWakeUpPeriod() */

/******************************************************************************
 * NAME:WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes Beacon/Probe response template frame to lmac.
 * \param UmacHandle    -   Handle to the UMAC Instance.
 * \param FrameType     -   1: Beacon, 5: Probe response
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp(UMAC_HANDLE UmacHandle,
							 uint8 FrameType)
{
	MIB_PARAMETERS *pMibParams;
	WSM_MIB_TEMPLATE_FRAME *pTemplateFrame;
	WFM_UMAC_MGMT_FRAME *pMgmtFrame;
	WFM_UMAC_BEACON_FRAME_BODY *pBeaconFrame;
	uint32 status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 *ptr;
	uint8 subType;
	uint8 broadcastaddr[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	/* default is ext rate not present */
	uint32 u32RemainingSupportedRateSet = 0;
	uint32 u32RemainingBasicRateSet = 0;
	uint32 u32OurAllBasicSupportedRatesAbgn;
	uint8 ERPinfoElement = 0; /* No Protection */

	OS_ASSERT(pUmacInstance);
	LOG_DATA(DBG_WFM_UMAC, "WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp():FrameType =%d\n", FrameType);
	/* Prepare pointers */
	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pTemplateFrame = (WSM_MIB_TEMPLATE_FRAME *) pMibParams->MibData;
	pMgmtFrame = (WFM_UMAC_MGMT_FRAME *) pTemplateFrame->Frame;
	pBeaconFrame = (WFM_UMAC_BEACON_FRAME_BODY *) pMgmtFrame->FrameBody;

	if (WSM_TMPLT_BCN == FrameType) {
		subType = WFM_UMAC_MGMT_BEACON;
	} else {
		/* Assuming it is probe response if not beacon */
		subType = WFM_UMAC_MGMT_PROB_RSP;
	}

	/*1. Create header */
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_MGMT,
					subType,
					&pMgmtFrame->dot11hdr,
					broadcastaddr,
					pUmacInstance->MacAddress,
					0,
					0
					);

	OS_MemoryCopy(
			pMgmtFrame->dot11hdr.Address3,
			pUmacInstance->RemoteMacAdd,
			WFM_MAC_ADDRESS_SIZE
			);
#if P2P_MINIAP_SUPPORT
	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP)
		pBeaconFrame->Capability = pUmacInstance->dot11BssConf.capabilityInfo | WFM_UMAC_CAPABILITY_ESS;
	else
#endif	/* P2P_MINIAP_SUPPORT */
		pBeaconFrame->Capability = pUmacInstance->CapabilitiesReq;

	pTemplateFrame->FrameType = FrameType;

	/* Leave the time stamp field. To be updated by firmware */

	pBeaconFrame->BeaconInterval = (uint16) pUmacInstance->beaconInterval;

	if (TRUE == pUmacInstance->enableWMM)
		pBeaconFrame->Capability |= WFM_UMAC_CAPABILITY_QOS;

	if (TRUE == pUmacInstance->enableWEP)
		pBeaconFrame->Capability |= WFM_UMAC_CAPABILITY_PRIVACY;

	ptr = pBeaconFrame->IEElements;

	if ((subType == WFM_UMAC_MGMT_BEACON) && (pUmacInstance->gVars.p.HiddenAPFlag == TRUE)) {
		ptr = WFM_UMAC_put_IE(
				ptr,
				UMAC_IE_ELEMENT_SSID,
				0,
				0
				);
	} else {
		ptr = WFM_UMAC_put_IE(
				ptr,
				UMAC_IE_ELEMENT_SSID,
				pUmacInstance->SsidLength,
				pUmacInstance->Ssid
				);
	}

	/* Supported Rates */
	*(ptr++) = UMAC_IE_ELEMENT_SUPPORTED_RATES;

	if (pUmacInstance->AllBasicSupportedRates_WithAP_Abgn)
		ptr = ptr + 1 + WFM_UMAC_Cvt_WfmFormat_Rate_To_80211abgFormat_Rate(pUmacInstance->AllSupportedRates_WithAP_Abgn, pUmacInstance->AllBasicSupportedRates_WithAP_Abgn, WFM_MAX_ELEMENTS_IN_SUPPORTED_RATES, ptr + 1, ptr, &u32RemainingSupportedRateSet, &u32RemainingBasicRateSet);

	/* Add DSconfig only when network is 11b or 11g */
	if (NETWORK_TYPE_11B == pUmacInstance->networkTypeInUse || NETWORK_TYPE_11BG == pUmacInstance->networkTypeInUse) {
		ptr = WFM_UMAC_put_IE(
				ptr,
				UMAC_IE_ELEMENT_DS_PARAM,
				1,
				(uint8 *) &(pUmacInstance->Channelnumber)
				);
	}

	if (WFM_802_IBSS == pUmacInstance->dot11InfraStructureMode) {
		ptr = WFM_UMAC_put_IE(
				ptr,
				UMAC_IE_ELEMENT_IBSS_PARAM_SET,
				sizeof(pUmacInstance->atimWinSize),
				(uint8 *) &(pUmacInstance->atimWinSize)
				);
	}
#if P2P_MINIAP_SUPPORT
	if ((pUmacInstance->operatingMode >= OPERATING_MODE_AP) && (WSM_TMPLT_BCN == FrameType)) {
		pUmacInstance->TIMparams.length = 4;	/* Minimum Length */
		ptr = WFM_UMAC_put_IE(
				ptr,
				UMAC_IE_ELEMENT_TIM,
				pUmacInstance->TIMparams.length,
				(uint8 *) &(pUmacInstance->TIMparams.DTIMcount)
				);
	}

	if (pUmacInstance->gVars.p.InfoHost.countryStringAvl) {
		/*Put the country Info IE*/
		ptr = UMAC_PutCountryIE(UmacHandle, ptr);
	}
#endif	/* P2P_MINIAP_SUPPORT */

	if ((NETWORK_TYPE_11BG == pUmacInstance->networkTypeInUse || NETWORK_TYPE_11BGN == pUmacInstance->networkTypeInUse) && (u32RemainingSupportedRateSet || u32RemainingBasicRateSet)
	    ) {
		pUmacInstance->ErpInfoPrevious = pUmacInstance->ErpInformation;
		ptr = WFM_UMAC_put_IE(
					ptr,
					UMAC_IE_ELEMENT_ERP_INFORMATION,
					1,
					&pUmacInstance->ErpInformation
					);

		pUmacInstance->Flags |= WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_CTSRTS_PROTECTION;

		/* Extended Rate Set. */
		if (pUmacInstance->AllSupportedRates_WithAP_Abgn) {
			*(ptr++) = UMAC_IE_ELEMENT_EXTENDED_SUPPORTED_RATES;
			ptr = ptr + 1 + WFM_UMAC_Cvt_WfmFormat_Rate_To_80211abgFormat_Rate(u32RemainingSupportedRateSet, u32RemainingBasicRateSet, WFM_MAX_ELEMENTS_IN_EXTENDED_SUPPORTED_RATES, ptr + 1, ptr, &u32RemainingSupportedRateSet, &u32RemainingBasicRateSet);
		}
	}

	if (pUmacInstance->Band) {
		/* all our basic supported rates & rates for 5 ghz band */
		u32OurAllBasicSupportedRatesAbgn = pUmacInstance->OurAllBasicSupportedRates_Abgn
		    & UMAC_PHY_BAND_5G_SUPPORTED_RATES;
	} else {
		/* all our basic supported rates & rates for 2.4 ghz band */
		u32OurAllBasicSupportedRatesAbgn = pUmacInstance->OurAllBasicSupportedRates_Abgn
		    & UMAC_PHY_BAND_2_4G_SUPPORTED_RATES;
	}

    /*************************************************************************
			       VENDOR SPECIFIC IEs
    *************************************************************************/
#if P2P_MINIAP_SUPPORT
    /*************************************************************************
					WMM
    *************************************************************************/
	if ((pUmacInstance->operatingMode >= OPERATING_MODE_AP) && (pUmacInstance->dot11BssConf.vdrSpecificCapInfo & WFM_BSS_INFO_FLAGS_BSS_WMM_CAP)) {
		*ptr++ = UMAC_IE_ELEMENT_VENDOR_SPECIFIC;

		*ptr++ = WMM_PARAM_ELT_LENGTH;

		/* OUI */
		*ptr++ = 0x00;
		*ptr++ = 0x50;
		*ptr++ = 0xF2;

		/* OUI  Type */
		*ptr++ = WMM_IE_ELEMENT_TYPE;

		/* OUI  Subype */
		*ptr++ = WFM_IE_PARAMETER_ELEMENT_SUB_TYPE;

		*ptr++ = WMM_PARAM_ELT_VER;

		OS_MemoryCopy(
			ptr,
			&pUmacInstance->gVars.WmmParamElement.QoSInfo,
			sizeof(WFM_WMM_AC_PARAM_PACKED) * WFM_NUM_ACS + 2
			);

		ptr += sizeof(WFM_WMM_AC_PARAM_PACKED) * WFM_NUM_ACS + 2;

	}
    /*************************************************************************
					WPA
    *************************************************************************/
	if ((pUmacInstance->operatingMode >= OPERATING_MODE_AP) && (pUmacInstance->dot11BssConf.vdrSpecificCapInfo & WFM_BSS_INFO_FLAGS_SUPP_WPA)) {
		*ptr++ = UMAC_IE_ELEMENT_VENDOR_SPECIFIC;

		*ptr++ = 24;	/* Length */

		*ptr++ = 0x00;	/* VENDOR_OUI */
		*ptr++ = 0x50;
		*ptr++ = 0xF2;

		*ptr++ = WPA_IE_PARAMETER_ELEMENT_SUB_TYPE;

		*ptr++ = 0x01;	/* Version */
		*ptr++ = 0x00;

		*ptr++ = 0x00;	/* Group Cipher */
		*ptr++ = 0x50;
		*ptr++ = 0xF2;
		*ptr++ = CIPHER_TYPE_TKIP;

		*ptr++ = 0x01;	/* Pairwise cipher count */
		*ptr++ = 0x00;

		*ptr++ = 0x00;	/* Pairwise Cipher TKIP list */
		*ptr++ = 0x50;
		*ptr++ = 0xF2;
		*ptr++ = CIPHER_TYPE_TKIP;

		*ptr++ = 0x01;	/* AKM Suite count */
		*ptr++ = 0x00;

		*ptr++ = 0x00;	/* AKM suite TKIP list */
		*ptr++ = 0x50;
		*ptr++ = 0xF2;
		if (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA) {
			*ptr++ = 0x01;
		} else {
			/*WPA-PSK */
			*ptr++ = 0x02;
		}

		*ptr++ = 0x18;	/* RSN Capabilities */
		*ptr++ = 0x00;

		pBeaconFrame->Capability |= WFM_UMAC_CAPABILITY_PRIVACY;
	}

    /*************************************************************************
					RSN
    *************************************************************************/
	if ((pUmacInstance->operatingMode >= OPERATING_MODE_AP) && (pUmacInstance->dot11BssConf.vdrSpecificCapInfo & WFM_BSS_INFO_FLAGS_SUPP_WPA2)) {
		*ptr++ = UMAC_IE_ELEMENT_RSN;

		*ptr++ = 0x14;	/* Length */

		*ptr++ = 1;	/* Version 2 Octets */
		*ptr++ = 0;

		*ptr++ = 0x00;	/* CCMP as group Cipher */
		*ptr++ = 0x0F;
		*ptr++ = 0xAC;
		*ptr++ = 0x04;

		*ptr++ = 0x01;	/* Pairwise Cipher suite count */
		*ptr++ = 0x00;

		*ptr++ = 0x00;	/* CCMP as pairwise Cipher */
		*ptr++ = 0x0F;
		*ptr++ = 0xAC;
		*ptr++ = 0x04;

		*ptr++ = 0x01;	/* Authentication count */
		*ptr++ = 0x00;

		*ptr++ = 0x00;	/* 802.1X Authentication */
		*ptr++ = 0x0F;
		*ptr++ = 0xAC;
		if (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost == WFM_AUTH_MODE_WPA_2) {
			*ptr++ = 0x01;
		} else {
			/*WPA2-PSK */
			*ptr++ = 0x02;
		}

		/* RSN Capabilities */
		if (pUmacInstance->dot11BssConf.vdrSpecificCapInfo & WFM_BSS_INFO_FLAGS_BSS_WMM_CAP)
			*ptr++ = 0x0C;	/* 16 replay counters per PTKSA. 1 replay counter per GTKSA */
		else
			*ptr++ = 0x00;	/* 1 replay counters per PTKSA. 1 replay counter per GTKSA */

		*ptr++ = 0x00;

		pBeaconFrame->Capability |= WFM_UMAC_CAPABILITY_PRIVACY;
	}

    /*************************************************************************
		 P2P Attributes
    *************************************************************************/
#if 0
	if (pUmacInstance->operatingMode == OPERATING_MODE_P2P_DEVICE) {
		/* P2P Capability */
		UMAC_INSERT_ATTRIB(ptr, UMAC_P2P_IE_ATTRIB_CAPABILITY, UMAC_P2P_ATTRIB_CAPABILITY_LENGTH);
		*ptr++ = UMAC_DC_CLIENT_DISCVRBLTY | UMAC_DC_DEVICE_LIMIT | UMAC_DC_INVTN_PROCEDURE;
		*ptr++ = (pUmacInstance->p2pGrpFormtn.persistentGroup) << 1 | UMAC_GC_INTRA_BSS_DIST;

		if (WSM_TMPLT_BCN == FrameType) {
			/* P2P Device Id: present only in beacon */
			UMAC_INSERT_ATTRIB(ptr, UMAC_P2P_IE_ATTRIB_DEVICE_ID, WFM_MAC_ADDRESS_SIZE);
			OS_MemoryCopy(ptr, pUmacInstance->MacAddress, WFM_MAC_ADDRESS_SIZE);
			ptr += WFM_MAC_ADDRESS_SIZE;
		} /* if(WSM_TMPLT_BCN == FrameType) */

		if (WSM_TMPLT_PRBRES == FrameType) {
			/* P2P Device Info: present only in probe response */
			uint16 length;
			length = (uint16) (WFM_MAC_ADDRESS_SIZE + UMAC_WSC_CONFIG_METHOD_IE_LENGTH + UMAC_WSC_PRIM_DEV_TYP_IE_LENGTH + 1	/*Number of sec dev = 0 */
					   + pUmacInstance->p2pStartGrpFormatn.devNameLen);

			UMAC_INSERT_WSC_IE(ptr, UMAC_P2P_IE_ATTRIB_DEVICE_INFO, length);
			OS_MemoryCopy(ptr, pUmacInstance->MacAddress, WFM_MAC_ADDRESS_SIZE);
			ptr += WFM_MAC_ADDRESS_SIZE;

			*ptr++ = pUmacInstance->wscIE.configMethod[1];
			*ptr++ = pUmacInstance->wscIE.configMethod[0];

			/* Check endian-ness */
			OS_MemoryCopy(ptr, pUmacInstance->wscIE.primDevType, UMAC_WSC_PRIM_DEV_TYP_IE_LENGTH);
			ptr += UMAC_WSC_PRIM_DEV_TYP_IE_LENGTH;

			/* Number of secondary device types are 0 */
			*ptr++ = 0;

			UMAC_INSERT_WSC_IE(ptr, UMAC_WSC_DEV_NAME, (uint16) pUmacInstance->p2pStartGrpFormatn.devNameLen);
			OS_MemoryCopy(ptr, pUmacInstance->p2pStartGrpFormatn.devName, pUmacInstance->p2pStartGrpFormatn.devNameLen);
			ptr += pUmacInstance->p2pStartGrpFormatn.devNameLen;

		} /* if(WSM_TMPLT_PRBRES == FrameType) */

	} /* if(pUmacInstance->operatingMode == OPERATING_MODE_P2P_DEVICE) */
#endif
#endif	/* P2P_MINIAP_SUPPORT */
    /*************************************************************************
		 VENDOR SPECIFIC IEs
    *************************************************************************/
	if (subType == WFM_UMAC_MGMT_BEACON) {
		if ((pUmacInstance->VendorIe.beaconIElength) && (pUmacInstance->VendorIe.pBeaconIE)) {
			/*OS_MemoryCopy(
					ptr,
					pUmacInstance->VendorIe.pBeaconIE,
					pUmacInstance->VendorIe.beaconIElength
					);

			ptr += pUmacInstance->VendorIe.beaconIElength;*/
			ptr = UMAC_PutVendorIE(UmacHandle, ptr, subType);
		}
	} else if (subType == WFM_UMAC_MGMT_PROB_RSP) {
		if ((pUmacInstance->VendorIe.probeRespIElength) && (pUmacInstance->VendorIe.pProbeRespIE)) {
			/*OS_MemoryCopy(
					ptr,
					pUmacInstance->VendorIe.pProbeRespIE,
					pUmacInstance->VendorIe.probeRespIElength
					);

			ptr += pUmacInstance->VendorIe.probeRespIElength;*/
			ptr = UMAC_PutVendorIE(UmacHandle, ptr, subType);
		}
	}
#if 0
	if ((pUmacInstance->VendorIe.respIElength) && (pUmacInstance->VendorIe.pRespIE)) {
		OS_MemoryCopy(
				ptr,
				pUmacInstance->VendorIe.pRespIE,
				pUmacInstance->VendorIe.respIElength
				);

		ptr += pUmacInstance->VendorIe.respIElength;
	}
#endif
#if P2P_MINIAP_SUPPORT
	pUmacInstance->dot11BssConf.capabilityInfo = pBeaconFrame->Capability;
#endif	/* P2P_MINIAP_SUPPORT */
	pTemplateFrame->Rate = (uint8) WFM_UMAC_GET_LOWEST_RATE_INDEX(u32OurAllBasicSupportedRatesAbgn);

	pTemplateFrame->FrameLength = ptr - (uint8 *) pMgmtFrame;

	/* 3. Prepare Topmost layer */
	pMibParams->MibId = WSM_MIB_ID_TEMPLATE_FRAME;
	pMibParams->Length = SIZE_PADDED_TO_MAKE_4_BYTES_ALIGNED(pTemplateFrame->FrameLength + (sizeof(WSM_MIB_TEMPLATE_FRAME) - WSM_MAX_TEMPLATE_FRAME_SIZE));	/* size has to be 4 byte aligned */

	{
		TEMPLATE_FRAME LmacTemplateFrame;

		/* Copy Host Template Frame to LMAC Template Frame */
		LmacTemplateFrame.FrameLength = pTemplateFrame->FrameLength;
		LmacTemplateFrame.FrameType = pTemplateFrame->FrameType;
		LmacTemplateFrame.Rate = pTemplateFrame->Rate;
		LmacTemplateFrame.pFrame = (uint8 *) &(pTemplateFrame->Frame);

		/* Send Template Frame to LMAC. */
		UMAC_LL_REQ_SET_TEMPLATE_FRAME(
						UmacHandle,
						&LmacTemplateFrame,
						pMibParams
						);
	}
	return status;
} /* end WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp() */

/******************************************************************************
 * NAME:WFM_UMAC_put_IE
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function inserts information element at the given position.
 * \param *P	  -   Position Pointer.
 * \param eid	  -   Element Id to be inserted.
 * \param size	  -   Size of the information element.
 * \param *d	  -   Information to be copied.
 * \returns Updated position pointer.
 *****************************************************************************/
uint8 *WFM_UMAC_put_IE(uint8 *p, uint8 eid, int size, uint8 * d)
{
	*p++ = eid;
	*p++ = (uint8) size;
	OS_MemoryCopy(p, d, size);
	return p + size;
}

/******************************************************************************
 * NAME:WFM_UMAC_ReadMib_CounterTable
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function send request to LMAC, to get statistics counter table .
 * \param UmacHandle    -   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_ReadMib_CounterTable(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	WFM_READ_MIB_PARAMETERS *pMibParams;

	OS_ASSERT(pUmacInstance);

	pMibParams = (WFM_READ_MIB_PARAMETERS *)
	    UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	pMibParams->MibId = WSM_MIB_ID_COUNTERS_TABLE;
	pMibParams->Reserved = 0x00;
	UMAC_LL_REQ_READ_MIB(UmacHandle, (MIB_PARAMETERS *) pMibParams);

	return status;

} /* end WFM_UMAC_ReadMib_CounterTable() */

/******************************************************************************
 * NAME:WFM_UMAC_WriteMib_OperationalPwrMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs operational power mode in Lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param OprPerMode		-   Operational power mode.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_OperationalPwrMode(UMAC_HANDLE UmacHandle,
					    uint32 OprPerMode)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	OS_ASSERT(pUmacInstance);

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_OPERATIONAL_POWER_MODE;
	pMibParams->Length = sizeof(uint32);
	*(uint32 *) pMibParams->MibData = OprPerMode;

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	LOG_DATA(DBG_WFM_UMAC, "WFM_UMAC_WriteMib_OperationalPwrMode():		\
					Operational Power Mode =%d\n", OprPerMode);

	return status;

} /* end WFM_UMAC_WriteMib_OperationalPwrMode() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_HostMsgTypeFilter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs Host message type filter in Lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_HostMsgTypeFilter(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;
	WSM_MIB_SET_HOST_MSG_TYPE_FILTER *pHostMsgTypeFilter;

	OS_ASSERT(pUmacInstance);

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pHostMsgTypeFilter = (WSM_MIB_SET_HOST_MSG_TYPE_FILTER *) pMibParams->MibData;

	/*Filter Mask for Data frames */

	pHostMsgTypeFilter->FrameCtrlFilterMask = 0x004C;
	pHostMsgTypeFilter->FrameCtrlFilterValue = 0x0008;

	pMibParams->MibId = WSM_MIB_ID_SET_HOST_MSG_TYPE_FILTER;
	pMibParams->Length = sizeof(WSM_MIB_SET_HOST_MSG_TYPE_FILTER);

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	return status;

} /* end WFM_UMAC_WriteMib_HostMsgTypeFilter() */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_SetUAPSDInformation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the UAPSD information in Lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_SetUAPSDInformation(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_SET_UAPSD_INFORMATION;
	pMibParams->Length = sizeof(WSM_MIB_SET_UAPSD_INFORMATION);
	OS_MemoryCopy(
			pMibParams->MibData,
			(uint8 *) &(pUmacInstance->gVars.p.setUapsdInfo),
			sizeof(WFM_OID_802_11_SET_UAPSD)
			);

	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_SetRcpiRssiThreshold
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the RcpiRssiThreshold information in Lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_SetRcpiRssiThreshold(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_RCPI_RSSI_THRESHOLD;
	pMibParams->Length = sizeof(WSM_MIB_RCPI_RSSI_THRESHOLD);
	OS_MemoryCopy(
			pMibParams->MibData,
			(uint8 *) &(pUmacInstance->gVars.p.RcpiRssiThreshold),
			sizeof(WFM_RCPI_RSSI_THRESHOLD)
			);
	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_EtherTypeDataFrameFilter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the Ether Type Data Frame Filter in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pFilterMsg	-   Filter Message.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_EtherTypeDataFrameFilter(UMAC_HANDLE UmacHandle,
						  uint8 *pFilterMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_SET_ETHERTYPE_DATAFRAME_FILTER;
	pMibParams->Length = sizeof(WSM_MIB_ETHERTYPE_DATA_FRAME_FILTER_SET);
	OS_MemoryCopy(
			pMibParams->MibData,
			pFilterMsg,
			sizeof(WSM_MIB_ETHERTYPE_DATA_FRAME_FILTER_SET)
			);
	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UdpPortDataFrameFilter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the UDP Port Data Frame Filter in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pFilterMsg	-   Filter Message.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UdpPortDataFrameFilter(UMAC_HANDLE UmacHandle,
						uint8 *pFilterMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_SET_UDPPORT_DATAFRAME_FILTER;
	pMibParams->Length = sizeof(WSM_MIB_UDPPORTS_DATA_FRAME_FILTER_SET);
	OS_MemoryCopy(
			pMibParams->MibData,
			pFilterMsg,
			sizeof(WSM_MIB_UDPPORTS_DATA_FRAME_FILTER_SET)
			);

	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_MagicDataFrameFilter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the MAgic Data Frame Filter in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pFilterMsg	-   Filter Message.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_MagicDataFrameFilter(UMAC_HANDLE UmacHandle,
					      uint8 *pFilterMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_SET_MAGIC_DATAFRAME_FILTER;
	pMibParams->Length = sizeof(WSM_MIB_MAGIC_DATA_FRAME_FILTER);
	OS_MemoryCopy(
			pMibParams->MibData,
			pFilterMsg,
			sizeof(WSM_MIB_MAGIC_DATA_FRAME_FILTER)
			);

	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_OverrideInternalTxRate
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the MIB OverrideInternalTxRate in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param TxRate	-   TxRate for internal frames.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_OverrideInternalTxRate(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;
	WSM_MIB_OVERRIDE_INT_RATE *pInternalTxRate;
	uint8 IntTxRate, NonErpInternalTxRtae;

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_OVERRIDE_INTERNAL_TX_RATE;
	pMibParams->Length = sizeof(WSM_MIB_OVERRIDE_INT_RATE);

	pInternalTxRate = (WSM_MIB_OVERRIDE_INT_RATE *) (pMibParams->MibData);

	if (pUmacInstance->operatingMode == OPERATING_MODE_AP) {
		IntTxRate = (uint8)WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);
		NonErpInternalTxRtae = (uint8)WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);
	} else if (pUmacInstance->operatingMode == OPERATING_MODE_STA) {
		if (pUmacInstance->ErpOfdmRates)
			IntTxRate = (uint8)WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->ErpOfdmRates & WFM_ERP_OFDM_RATES_MANDATORY);
		else
			IntTxRate = (uint8)WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);

		NonErpInternalTxRtae = (uint8)WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->AllBasicSupportedRates_WithAP_Abgn);
	} else { /* P2P Modes */
		IntTxRate = (uint8)WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_ERP_OFDM_RATES_SUPPORTED);
		NonErpInternalTxRtae = (uint8)WFM_UMAC_GET_LOWEST_RATE_INDEX(pUmacInstance->AllSupportedRates_WithAP_Abgn & WFM_ERP_OFDM_RATES_SUPPORTED);
	}

	OS_MemoryReset(pInternalTxRate, sizeof(WSM_MIB_OVERRIDE_INT_RATE));
	pInternalTxRate->InternalTxRate = IntTxRate;
	pInternalTxRate->NonErpInternalTxRate = NonErpInternalTxRtae;

	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_ArpIpAddrFilter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the Arp IP Address Filter in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pFilterMsg	-   Filter Message.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_ArpIpAddrFilter(UMAC_HANDLE UmacHandle,
					 uint8 *pFilterMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_ARP_IP_ADDRESSES_TABLE;
	pMibParams->Length = sizeof(WSM_MIB_ARP_IP_ADDR_TABLE);
	OS_MemoryCopy(
			pMibParams->MibData,
			pFilterMsg,
			sizeof(WSM_MIB_ARP_IP_ADDR_TABLE)
			);

	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_ArpKeepAlivePeriod
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the Arp Keep Alive Period in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pFilterMsg	-   Filter Message.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_ArpKeepAlivePeriod(UMAC_HANDLE UmacHandle,
					 uint8 *pFilterMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_ARP_KEEP_ALIVE_PERIOD;
	pMibParams->Length = sizeof(WSM_MIB_ARP_KEEP_ALIVE_PERIOD);
	OS_MemoryCopy(
			pMibParams->MibData,
			pFilterMsg,
			sizeof(WSM_MIB_ARP_KEEP_ALIVE_PERIOD)
			);

	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
	return status;
}

#if MGMT_FRAME_PROTECTION
/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_ProtectedMgmtPolicy
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the Protected MGMT Policy in Lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_ProtectedMgmtPolicy(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;
	uint32 protectMgmtPolicy = 0;

	if (((pUmacInstance->hostMfpPolicy == UMAC_MFP_DISABLE)
	     && ((pUmacInstance->hostMfpPolicy == UMAC_MFP_DISABLE)
		 || (pUmacInstance->bssInfoMfpPolicy == UMAC_MFP_CAPABLE)))
	    || ((pUmacInstance->hostMfpPolicy == UMAC_MFP_CAPABLE)
		&& (pUmacInstance->bssInfoMfpPolicy == UMAC_MFP_DISABLE))) {
		pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm = FALSE;
		protectMgmtPolicy = UMAC_PROT_MGMT_UNPROT_ALLOWED;
	} else {
		pUmacInstance->protectMgmtFrame.protectRobustMgmtFrm = TRUE;
		protectMgmtPolicy = UMAC_PROT_MGMT_ENABLE;
	}

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	/* Following Commented code will be used later */
#if 0
	pMibParams->MibId = WSM_MIB_ID_PROTECTED_MGMT_POLICY;
	pMibParams->Length = sizeof(uint32);
	OS_MemoryCopy(
			pMibParams->MibData,
			&protectMgmtPolicy,
			sizeof(uint32)
			);
	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
#else
	status = WFM_STATUS_FAILURE;
#endif

	return status;
}
#endif	/* MGMT_FRAME_PROTECTION */
/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UseMultiTxCnfMsg
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function enable/disable Multiple Tx Confirmation Message.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UseMultiTxCnfMsg(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_USE_MULTI_TX_CONF_MSG;
	pMibParams->Length = sizeof(uint32);
	OS_MemoryCopy(
			pMibParams->MibData,
			&pUmacInstance->gVars.p.UseMultiTxCnfMsg,
			sizeof(uint32)
			);

	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_MulticastAddrFilter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the Multicast Address Filter in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pFilterMsg	-   Filter Message.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_MulticastAddrFilter(UMAC_HANDLE UmacHandle,
					     uint8 *pFilterMsg)
{
	uint32 status = WFM_STATUS_SUCCESS;
	UMAC_MULTICAST_ADDR_FILTER *pMultiCastFilter = (UMAC_MULTICAST_ADDR_FILTER *) pFilterMsg;
	MIB_PARAMETERS *pMibParams;

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_DOT11_GROUP_ADDRESSES_TABLE;

	pMibParams->Length = (uint16) (8 + SIZE_PADDED_TO_MAKE_4_BYTES_ALIGNED(pMultiCastFilter->numOfAddresses * sizeof(UMAC_MAC_ADDRESS)));
	OS_MemoryCopy(pMibParams->MibData, pFilterMsg, pMibParams->Length);
	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_KeepAlivePeriod
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function cnfigure Keep alive period in WSM firmware.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_KeepAlivePeriod(UMAC_HANDLE UmacHandle)
{

	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_KEEP_ALIVE_PERIOD;
	pMibParams->Length = sizeof(WSM_MIB_KEEP_ALIVE_PERIOD);
	OS_MemoryCopy(
			pMibParams->MibData,
			&pUmacInstance->gVars.p.keepAlivePeriod,
			sizeof(WSM_MIB_KEEP_ALIVE_PERIOD)
			);

	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_SendReqToWsm_SwitchChReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function send request for switch channel, as AP has moved to other
 * channel.
 * \param UmacHandle	-	Handle to the UMAC Instance.
 * \param pSwitchChReq	-	Switch Channel Request.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_SendReqToWsm_SwitchChReq(UMAC_HANDLE UmacHandle,
					 uint8 *pSwitchChReq)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	UMAC_SWITCH_CHANNEL_REQ *pSwitchChannel = (UMAC_SWITCH_CHANNEL_REQ *)
	    UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	OS_ASSERT(pUmacInstance);

	OS_MemoryCopy((uint8 *)pSwitchChannel, pSwitchChReq, sizeof(UMAC_SWITCH_CHANNEL_REQ));
#if 0
	pSwitchChannel->ChannelMode = ChSwitchMode;
	pSwitchChannel->ChannelSwitchCount = ChSwitchCount;
	pSwitchChannel->NewChannelNumber = (uint16) NewChNum;
#endif
	pSwitchChannel->NewChannelNumber = (pSwitchChannel->NewChannelNumber | (pUmacInstance->Channelnumber & 0xFF00));

	/* pUmacInstance->Channelnumber = pSwitchChannel->NewChannelNumber; */

	LOG_DATA(DBG_WFM_UMAC, "NewChannelNumber=0x%x\n", pSwitchChannel->NewChannelNumber);

	UMAC_LL_REQ_SWITCH_CHANNEL(UmacHandle, pSwitchChannel);

	return status;
} /* end WFM_UMAC_SendReqToWsm_SwitchChReq() */

#if FT_SUPPORT
/******************************************************************************
 * NAME:	WFM_UMAC_SendReqToWsm_JoinReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function send request to WSM for Joining a BSS
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pBssInfo	-   Pointer to the BSS to join.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_SendReqToWsm_JoinReq(UMAC_HANDLE UmacHandle,
				     WFM_BSS_INFO *pBssInfo)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	JOIN_PARAMETERS *pJoinParams = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 dummy;

	OS_ASSERT(pUmacInstance);

	OS_ASSERT(!pUmacInstance->gVars.pJoinTxDesc);

	pUmacInstance->gVars.pJoinTxDesc = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pUmacInstance->gVars.pJoinTxDesc);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pUmacInstance->gVars.pJoinTxDesc, UMAC_JOIN_REQ);

	WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pJoinTxDesc);

	pJoinParams = (JOIN_PARAMETERS *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pJoinTxDesc);
	CHECK_POINTER_IS_VALID(pJoin);

	pJoinParams->Mode = WFM_802_INFRASTRUCTURE;
	pJoinParams->AtimWindow = 0;
	pJoinParams->ProbeForJoin = 0;
	pJoinParams->SSIDLength = pUmacInstance->SsidLength;
	OS_MemoryCopy(
			&pJoinParams->SSID[0],
			&pUmacInstance->Ssid[0],
			pUmacInstance->SsidLength
			);

	if (pBssInfo == NULL) {
		pJoinParams->ChannelNumber = pUmacInstance->Channelnumber;

		D0_ADDR_COPY(&pJoinParams->BSSID[0], &pUmacInstance->RemoteMacAdd[0]);

		if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_BARKER_SHORT_PREAMBLE)
			pJoinParams->PreambleType = WSM_PREAMBLE_SHORT;
		else
			pJoinParams->PreambleType = WSM_PREAMBLE_LONG;

		pJoinParams->DTIMPeriod = pUmacInstance->DTIMPeriod & ~WFM_DTIM_PERIOD_FLAG_UNKNOWN;
		pJoinParams->Flags = 0;
		pJoinParams->BeaconInterval = pUmacInstance->beaconInterval;
		pJoinParams->BasicRateSet = pUmacInstance->AllBasicSupportedRates_WithAP_Abgn;
	} else {
		pJoinParams->ChannelNumber = (uint16) pBssInfo->ChannelNumber;
		D0_ADDR_COPY(&pJoinParams->BSSID[0], &pBssInfo->MacAddress[0]);
		pJoinParams->PreambleType = WSM_PREAMBLE_LONG;
		pJoinParams->DTIMPeriod = pUmacInstance->DTIMPeriod & ~WFM_DTIM_PERIOD_FLAG_UNKNOWN;
		pJoinParams->Flags = 0;
		pJoinParams->BeaconInterval = pBssInfo->BeaconPeriod;

		WFM_UMAC_Cvt_80211abgFormat_Rate_To_WfmFormat_Rate(pBssInfo->NumRates, (uint8 *) pBssInfo->SupportedRates, &dummy, &pJoinParams->BasicRateSet);
	}
	if (pJoinParams->ChannelNumber > 14)
		pJoinParams->Band = PHY_BAND_5G;
	else
		pJoinParams->Band = PHY_BAND_2G;

	Result = UMAC_LL_REQ_JOIN(UmacHandle, pJoinParams);

	if (Result != WFM_STATUS_SUCCESS) {
		UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pJoinTxDesc);

		WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pJoinTxDesc);

		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pUmacInstance->gVars.pJoinTxDesc
					);

		pUmacInstance->gVars.pJoinTxDesc = NULL;
		pUmacInstance->FtFlags &= ~UMAC_FT_FLAG_AUTH_OVER_AIR_BIT;
		OS_MemoryReset(
				&pUmacInstance->FtTargetBss.MacAddress[0],
				WFM_MAC_ADDRESS_SIZE
				);
	} /* if(Result != WFM_STATUS_SUCCESS) */

	return Result;
}
#endif	/* FT_SUPPORT */
#if DOT11K_SUPPORT
/******************************************************************************
 * NAME:	UMAC_SendReqToWsm_ChannelLoadReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends channel load request to WSM
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 UMAC_SendReqToWsm_ChannelLoadReq(UMAC_HANDLE UmacHandle)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WSM_HI_START_MEASUREMENT_REQ *pMeasurementReqParams = NULL;
	CHANNEL_LOAD_REQ *pChannelLoadReq = NULL;

	OS_ASSERT(!pUmacInstance->gVars.pMeasurementTxDesc);

	pUmacInstance->gVars.pMeasurementTxDesc = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pUmacInstance->gVars.pMeasurementTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pMeasurementTxDesc);

	pMeasurementReqParams = (WSM_HI_START_MEASUREMENT_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pMeasurementTxDesc);
	CHECK_POINTER_IS_VALID(pMeasurementReqParams);
	/*Now fill the params */

	pChannelLoadReq = (CHANNEL_LOAD_REQ *) &pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq;
	/*Fill the transmit power level based on regulatory class */
	pMeasurementReqParams->TxPowerLevel = pUmacInstance->dot11k_Meas_Params.txPowerLevel;
	pMeasurementReqParams->DurationMandatory = ((pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementRequestMode & UMAC_DURATION_MANDATORY_MASK) >> 4);
	/*Don't take measurementType from tructure as WSM API is having different measurement Types */
	pMeasurementReqParams->MeasurementType = WSM_MEASURE_TYPE_CHANNEL_LOAD;
	pMeasurementReqParams->MeasurementRequestLength = sizeof(WSM_MEAS_CHANNEL_LOAD_PARAMS);
	pMeasurementReqParams->MeasurementRequest.ChannelLoadParams.ChannelLoadCCA = 1;
	pMeasurementReqParams->MeasurementRequest.ChannelLoadParams.ChannelNum = pChannelLoadReq->channelNum;
	pMeasurementReqParams->MeasurementRequest.ChannelLoadParams.RandomInterval = pChannelLoadReq->randomizationInterval;
	pMeasurementReqParams->MeasurementRequest.ChannelLoadParams.MeasurementDuration = pChannelLoadReq->measurementDuration;
	/*measurement start time is not valid for Channel load*/
	pMeasurementReqParams->MeasurementRequest.ChannelLoadParams.MeasurementStartTime = 0;
	/*Fill the MsgLen */
	pMeasurementReqParams->MsgLen = (sizeof(WSM_HI_START_MEASUREMENT_REQ) - sizeof(WSM_MEAS_REQUEST) + sizeof(WSM_MEAS_CHANNEL_LOAD_PARAMS));

	/*Send the request to DIL Layer */
	Result = UMAC_LL_REQ_MEASUEREMENT(UmacHandle, pMeasurementReqParams);
	if (Result != WFM_STATUS_SUCCESS) {
		UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pMeasurementTxDesc);
		WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pMeasurementTxDesc);
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, pUmacInstance->gVars.pMeasurementTxDesc);
		pUmacInstance->gVars.pMeasurementTxDesc = NULL;
	}			/*if(Result != WFM_STATUS_SUCCESS) */
	return Result;
}

/******************************************************************************
 * NAME:	UMAC_SendReqToWsm_NoiseHistogramReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends Noise histogram request to WSM
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 UMAC_SendReqToWsm_NoiseHistogramReq(UMAC_HANDLE UmacHandle)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WSM_HI_START_MEASUREMENT_REQ *pMeasurementReqParams = NULL;
	NOISE_HISTOGRAM_REQ *pNoiseHistReq = NULL;

	OS_ASSERT(!pUmacInstance->gVars.pMeasurementTxDesc);

	pUmacInstance->gVars.pMeasurementTxDesc = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pUmacInstance->gVars.pMeasurementTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pMeasurementTxDesc);

	pMeasurementReqParams = (WSM_HI_START_MEASUREMENT_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pMeasurementTxDesc);
	CHECK_POINTER_IS_VALID(pMeasurementReqParams);
	/*Now fill the params */

	pNoiseHistReq = (NOISE_HISTOGRAM_REQ *) &pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq;
	/*Fill the transmit power level based on regulatory class */
	pMeasurementReqParams->TxPowerLevel = pUmacInstance->dot11k_Meas_Params.txPowerLevel;
	pMeasurementReqParams->DurationMandatory = ((pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementRequestMode & UMAC_DURATION_MANDATORY_MASK) >> 4);
	pMeasurementReqParams->MeasurementType = WSM_MEASURE_TYPE_NOISE_HISTOGRAM;

	pMeasurementReqParams->MeasurementRequestLength = sizeof(WSM_MEAS_NOISE_HISTOGRAM_PARAMS);

	/*TBC Channel Offset based on channel */
	pMeasurementReqParams->MeasurementRequest.NoisHistogramParams.IpiRpi = 1;
	pMeasurementReqParams->MeasurementRequest.NoisHistogramParams.ChannelNum = pNoiseHistReq->channelNum;
	pMeasurementReqParams->MeasurementRequest.NoisHistogramParams.RandomInterval = pNoiseHistReq->randomizationInterval;
	pMeasurementReqParams->MeasurementRequest.NoisHistogramParams.MeasurementDuration = pNoiseHistReq->measurementDuration;
	pMeasurementReqParams->MeasurementRequest.NoisHistogramParams.MeasurementStartTime = 0;
	/*Fill the MsgLen */
	pMeasurementReqParams->MsgLen = (sizeof(WSM_HI_START_MEASUREMENT_REQ) - sizeof(WSM_MEAS_REQUEST) + sizeof(WSM_MEAS_NOISE_HISTOGRAM_PARAMS));

	/*Send the request to DIL Layer */
	Result = UMAC_LL_REQ_MEASUEREMENT(UmacHandle, pMeasurementReqParams);
	if (Result != WFM_STATUS_SUCCESS) {
		UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pMeasurementTxDesc);
		WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pMeasurementTxDesc);
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, pUmacInstance->gVars.pMeasurementTxDesc);
		pUmacInstance->gVars.pMeasurementTxDesc = NULL;
	}			/*if(Result != WFM_STATUS_SUCCESS) */
	return Result;
}

/******************************************************************************
 * NAME:	UMAC_SendReqToWsm_BeaconReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends beacon request to WSM
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 UMAC_SendReqToWsm_BeaconReq(UMAC_HANDLE UmacHandle)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WSM_HI_START_MEASUREMENT_REQ *pMeasurementReqParams = NULL;
	BEACON_REQ *pBeaconReq = NULL;

	OS_ASSERT(!pUmacInstance->gVars.pMeasurementTxDesc);

	pUmacInstance->gVars.pMeasurementTxDesc = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pUmacInstance->gVars.pMeasurementTxDesc);

	pMeasurementReqParams = (WSM_HI_START_MEASUREMENT_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pMeasurementTxDesc);
	CHECK_POINTER_IS_VALID(pMeasurementReqParams);
	/*Now fill the params */

	pBeaconReq = (BEACON_REQ *) &pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq;
	/*Fill the transmit power level based on regulatory class */
	pMeasurementReqParams->TxPowerLevel = 0; /* pBeaconReq->txPowerLevel; */
	pMeasurementReqParams->DurationMandatory = ((pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementRequestMode & UMAC_DURATION_MANDATORY_MASK) >> 4);
	pMeasurementReqParams->MeasurementType = WSM_MEASURE_TYPE_BEACON;
	pMeasurementReqParams->MeasurementRequestLength = sizeof(WSM_MEAS_BEACON_PARAMS);

	/*Fill the beacon Request */
	/*pMeasurementReqParams->MeasurementRequest.BeaconParams.RegulatoryClass = pBeaconReq->regulatoryClass;*/
	/*pMeasurementReqParams->MeasurementRequest.BeaconParams.MeasurementMode = pBeaconReq->measurementMode;*/
	/*pMeasurementReqParams->MeasurementRequest.BeaconParams.ChannelNum = pBeaconReq->channelNum + pBeaconReq->channelOffset;*/
	pMeasurementReqParams->MeasurementRequest.BeaconParams.RandomInterval = pBeaconReq->randomizationInterval;
	/*pMeasurementReqParams->MeasurementRequest.BeaconParams.MeasurementDuration = pBeaconReq->measurementDuration;*/
	/*OS_MemoryCopy(&pMeasurementReqParams->MeasurementRequest.BeaconParams.Bssid, &pBeaconReq->bssid, 6);*/

	/*Fill the MsgLen */
	pMeasurementReqParams->MsgLen = (sizeof(WSM_HI_START_MEASUREMENT_REQ) - sizeof(WSM_MEAS_REQUEST) + sizeof(WSM_MEAS_BEACON_PARAMS));

	/*Send the request to DIL Layer */
	Result = UMAC_LL_REQ_MEASUEREMENT(UmacHandle, pMeasurementReqParams);
	if (Result != WFM_STATUS_SUCCESS) {
		UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pMeasurementTxDesc);
		WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pMeasurementTxDesc);
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, pUmacInstance->gVars.pMeasurementTxDesc);
		pUmacInstance->gVars.pMeasurementTxDesc = NULL;
	} /* if(Result != WFM_STATUS_SUCCESS) */

	return Result;
}

/******************************************************************************
 * NAME:	UMAC_SendReqToWsm_StatisticsReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends statistics request to WSM
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 UMAC_SendReqToWsm_StatisticsReq(UMAC_HANDLE UmacHandle)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WSM_HI_START_MEASUREMENT_REQ *pMeasurementReqParams = NULL;
	STA_STATISTICS_REQ *pStatsReq = NULL;

	OS_ASSERT(!pUmacInstance->gVars.pMeasurementTxDesc);

	pUmacInstance->gVars.pMeasurementTxDesc = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pUmacInstance->gVars.pMeasurementTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pMeasurementTxDesc);

	pMeasurementReqParams = (WSM_HI_START_MEASUREMENT_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pMeasurementTxDesc);
	CHECK_POINTER_IS_VALID(pMeasurementReqParams);
	/*Now fill the params */

	pStatsReq = (STA_STATISTICS_REQ *) &pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementElementReq;
	/*Fill the transmit power level based on regulatory class */
	/*No regulatory class in STATS req.So provide default power level*/
	pMeasurementReqParams->TxPowerLevel = WFM_UMAC_DEFAULT_POWER_LEVEL;
	pMeasurementReqParams->DurationMandatory = ((pUmacInstance->dot11k_Meas_Params.measurementReqElementsIE[pUmacInstance->dot11k_Meas_Params.CurrentArrayIndex].measurementRequestMode & UMAC_DURATION_MANDATORY_MASK) >> 4);
	pMeasurementReqParams->MeasurementType = WSM_MEASURE_TYPE_STAT_STATISTICS;
	pMeasurementReqParams->MeasurementRequestLength = sizeof(WSM_MEAS_STA_STATS_PARAMS);

	/*Fill the Statistics Request */
	OS_MemoryCopy(&pMeasurementReqParams->MeasurementRequest.StaStatsParams.PeerMacAddress, &pStatsReq->peerMacAddr, 6);
	pMeasurementReqParams->MeasurementRequest.StaStatsParams.RandomInterval = pStatsReq->randomizationInterval;
	pMeasurementReqParams->MeasurementRequest.StaStatsParams.MeasurementDuration = pStatsReq->measurementDuration;
	pMeasurementReqParams->MeasurementRequest.StaStatsParams.GroupId = pStatsReq->groupIdentity;

	/*Fill the MsgLen */
	pMeasurementReqParams->MsgLen = (sizeof(WSM_HI_START_MEASUREMENT_REQ) - sizeof(WSM_MEAS_REQUEST) + sizeof(WSM_MEAS_STA_STATS_PARAMS));

	/*Send the request to DIL Layer */
	Result = UMAC_LL_REQ_MEASUEREMENT(UmacHandle, pMeasurementReqParams);
	if (Result != WFM_STATUS_SUCCESS) {
		UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pMeasurementTxDesc);
		WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pMeasurementTxDesc);
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, pUmacInstance->gVars.pMeasurementTxDesc);
		pUmacInstance->gVars.pMeasurementTxDesc = NULL;
	} /*if(Result != WFM_STATUS_SUCCESS) */

	return Result;
}

/******************************************************************************
 * NAME:	UMAC_SendReqToWsm_LinkMeasReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends link measurement request to WSM
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 UMAC_SendReqToWsm_LinkMeasReq(UMAC_HANDLE UmacHandle)
{
	uint32 Result = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	WSM_HI_START_MEASUREMENT_REQ *pMeasurementReqParams = NULL;

	LINK_MEASUREMENT_REQUEST *pLinkMeasReq = NULL;
	OS_ASSERT(!pUmacInstance->gVars.pMeasurementTxDesc);

	pUmacInstance->gVars.pMeasurementTxDesc = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pUmacInstance->gVars.pMeasurementTxDesc);

	WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pMeasurementTxDesc);

	pMeasurementReqParams = (WSM_HI_START_MEASUREMENT_REQ *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pMeasurementTxDesc);
	CHECK_POINTER_IS_VALID(pMeasurementReqParams);
	/*Now fill the params */

	pLinkMeasReq = (LINK_MEASUREMENT_REQUEST *) &pUmacInstance->dot11k_Meas_Params.LinkMeasuremetReq;

	/*Fill the transmit power level based on regulatory class */
	/*No regulatory class in STATS req.So provide default power level*/
	pMeasurementReqParams->TxPowerLevel = WFM_UMAC_DEFAULT_POWER_LEVEL;
	pMeasurementReqParams->DurationMandatory = 0;
	pMeasurementReqParams->MeasurementType = WSM_MEASURE_TYPE_LINK_MEASUREMENT;
	pMeasurementReqParams->MeasurementRequestLength = sizeof(WSM_MEAS_LINK_MEASUREMENT_PARAMS);
	/*Fill the MsgLen */
	pMeasurementReqParams->MsgLen = (sizeof(WSM_HI_START_MEASUREMENT_REQ) - sizeof(WSM_MEAS_REQUEST) + sizeof(WSM_MEAS_LINK_MEASUREMENT_PARAMS));

	/*Send the request to DIL Layer */
	Result = UMAC_LL_REQ_MEASUEREMENT(UmacHandle, pMeasurementReqParams);
	if (Result != WFM_STATUS_SUCCESS) {
		UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pMeasurementTxDesc);
		WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pMeasurementTxDesc);
		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(UmacHandle, pUmacInstance->gVars.pMeasurementTxDesc);
		pUmacInstance->gVars.pMeasurementTxDesc = NULL;
	} /* if(Result != WFM_STATUS_SUCCESS) */

	return Result;
}

#endif	/*DOT11K_SUPPORT */

#if P2P_MINIAP_SUPPORT
/******************************************************************************
 * NAME:WFM_UMAC_MAP_LINK_REQUEST
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function send link mapping request for the new station
 * \param UmacHandle    -   Handle to the UMAC Instance.
 * \param pMacAddress   -   MAC address of the station
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_MAP_LINK_REQUEST(UMAC_HANDLE UmacHandle, uint8 *pMacAddress)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	WSM_HI_MAP_LINK_REQ *pMapLink = (WSM_HI_MAP_LINK_REQ *)
	    UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	OS_ASSERT(pUmacInstance);

	OS_MemoryCopy(pMapLink->MacAddr, pMacAddress, WFM_MAC_ADDRESS_SIZE);

	/*UMAC_LL_REQ_MAP_LINK( UmacHandle,pMapLink)  ; */

	return status;
} /* end WFM_UMAC_MAP_LINK_REQUEST() */

/******************************************************************************
 * NAME:WFM_UMAC_WriteMib_FindInfo
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the Find information in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_FindInfo(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;
	WSM_MIB_P2P_FIND_INFO *pFindInfo;

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pFindInfo = (WSM_MIB_P2P_FIND_INFO *) &pMibParams->MibData[0];
	pMibParams->MibId = WSM_MIB_ID_P2P_FIND_INFO;
	pMibParams->Length = sizeof(WSM_MIB_P2P_FIND_INFO)
	    + pUmacInstance->p2pDevDiscvry.bssListScan.NumberOfChannels_2_4Ghz * sizeof(WSM_SCAN_CHANNEL_DEF);
	OS_MemoryCopy(
		(uint8 *) pFindInfo + 2, /* sizeof(band) + sizeof(Reserved) */
		(uint8 *) &(pUmacInstance->p2pDevDiscvry),
		sizeof(WSM_MIB_P2P_FIND_INFO) - 1 /* sizeof(NumChannels) */
		);

	if (pUmacInstance->p2pDevDiscvry.bssListScan.Flags & WFM_BSS_LIST_SCAN_2_4G_BAND)
		pFindInfo->Band = 0;
	else if (pUmacInstance->p2pDevDiscvry.bssListScan.Flags & WFM_BSS_LIST_SCAN_5G_BAND)
		pFindInfo->Band = 1;
	pFindInfo->NumProbs = pUmacInstance->p2pDevDiscvry.bssListScan.NumOfProbeRequests;
	pFindInfo->NumChannels = pUmacInstance->socialChannels.numSocChannels;

	/* Currently only for 2.4G */
	OS_MemoryCopy(
		(uint8 *) &pFindInfo->NumChannels + 1,
		pUmacInstance->socialChannels.socChannels,
		pUmacInstance->socialChannels.numSocChannels * sizeof(WSM_SCAN_CHANNEL_DEF)
		);

	if (pUmacInstance->UmacPreviousState == UMAC_GROUP_FORMING)
		pFindInfo->FindTimeOut = 20; /* Hard coding the time to 2 sec */
	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
	return status;
} /* WFM_UMAC_WriteMib_FindInfo() */

/******************************************************************************
 * NAME:WFM_UMAC_Get_Group_TSC
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function gets the Group TSC value from the LMAC for multicast/broadcast
 * frames.
 * \param UmacHandle    -   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_Get_Group_TSC(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	WFM_READ_MIB_PARAMETERS *pMibParams;

	OS_ASSERT(pUmacInstance);

	pMibParams = (WFM_READ_MIB_PARAMETERS *)
	    UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	pMibParams->MibId = WSM_MIB_ID_GRP_SEQ_COUNTER;
	pMibParams->Reserved = 0x00;
	UMAC_LL_REQ_READ_MIB(UmacHandle, (MIB_PARAMETERS *) pMibParams);

	return status;

} /* end WFM_UMAC_Get_Group_TSC() */
/******************************************************************************
 * NAME:WFM_UMAC_WriteMib_P2P_PSMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes the MIB for P2P PS mode to firmware.
 * \param UmacHandle    -   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_P2P_PSMode(UMAC_HANDLE UmacHandle)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;
	MIB_PARAMETERS *pMibParams;

	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pMibParams->MibId = WSM_MIB_ID_P2P_PS_MODE_INFO;
	pMibParams->Length = sizeof(WSM_MIB_P2P_PS_MODE_INFO);
	OS_MemoryCopy(
			pMibParams->MibData,
			(uint8 *) &(pUmacInstance->gVars.p.p2pPsMode),
			sizeof(WSM_MIB_P2P_PS_MODE_INFO)
			);
	status = UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);
	return status;
}

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_TemplateFrame_ArpReply
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes ARP Reply Template Frame.
 * \param UmacHandle	-   Handle to the UMAC Instance.
  * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_TemplateFrame_ArpReply(UMAC_HANDLE UmacHandle)
{
	MIB_PARAMETERS *pMibParams;
	WSM_MIB_TEMPLATE_FRAME *pTemplateFrame;
	WFM_UMAC_DATA_FRAME *pDataFrame;
	WFM_UMAC_QOS_DATA_FRAME *pQoSDataFrame;
	uint16 FrameBodyLen = 0;
	uint8 Encrypt = 0;
	uint8 EncHdrsize = 0;
	uint8 EncTagSize = 0;
	uint8 SubType = WFM_UMAC_SUB_DATA;
	WFM_UMAC_LLC_SNAP_HDR *pLLC_snap_hdr;
	WFM_UMAC_ARP_REPLY *pArpReply;
	uint16 EncryptionType = WFM_UMAC_NO_ENC_SUPPORTED;

	uint32 status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);
	/* Prepare pointers */
	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pTemplateFrame = (WSM_MIB_TEMPLATE_FRAME *) pMibParams->MibData;
	pDataFrame = (WFM_UMAC_DATA_FRAME *) pTemplateFrame->Frame;
	pQoSDataFrame = (WFM_UMAC_QOS_DATA_FRAME *) pTemplateFrame->Frame;

	if ((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION) ||
		(pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION)
	    ) {
		/*This link supports QoS, so insert QoS Control Field */

		SubType = WFM_UMAC_SUB_DATA_QOS;

		/*Adding the size of QoS field too to the length */
		FrameBodyLen += 2;

		/* Setting Ack policy to 00 [ normal ack ]
		   Setting bit-4 ESOP
		   Setting QueueSize to 0xff [which indicates Unkown or
		   unspecified value as per the spec]
		 */
		pQoSDataFrame->QoSCntrl = 0xff10;

		/*
		  As per WMM specification, bits 7 - 15 has are
		  reserved and has to be zero.Event bit 3 has to
		  be zero. Setting ESOP to zero by default
		*/
		pQoSDataFrame->QoSCntrl = pQoSDataFrame->QoSCntrl & 0x00E7;
	}

	/* Decide the encryption type */
	switch (pUmacInstance->gVars.p.InfoHost.AuthenticationModeHost) {
	case WFM_AUTH_MODE_OPEN:
	case WFM_AUTH_MODE_SHARED:
		if ((pUmacInstance->CapabilitiesRsp & WFM_UMAC_CAPABILITY_PRIVACY) == WFM_UMAC_CAPABILITY_PRIVACY)
			EncryptionType = WFM_UMAC_WEP_ENC_SUPPORTED;
		break;

	case WFM_AUTH_MODE_WPA:
	case WFM_AUTH_MODE_WPA_PSK:
	case WFM_AUTH_MODE_WPA_NONE:
		EncryptionType = WFM_UMAC_TKIP_ENC_SUPPORTED;
		break;

	case WFM_AUTH_MODE_WPA_2:
	case WFM_AUTH_MODE_WPA_2_PSK:
	case WFM_AUTH_MODE_WPA_2_FT:
	case WFM_AUTH_MODE_WPA_2_FT_PSK:
	case WFM_AUTH_MODE_WPA_2_SH256:
	case WFM_AUTH_MODE_WPA_2_SH256_PSK:
		EncryptionType = WFM_UMAC_AES_ENC_SUPPORTED;
		break;

	case WFM_AUTH_MODE_WAPI:
		EncryptionType = WFM_UMAC_WAPI_ENC_SUPPORTED;
		break;

	default:
		EncryptionType = WFM_UMAC_NO_ENC_SUPPORTED;
	}

	/*protect frame when encryption type is non-zero */
	if (EncryptionType) {
		Encrypt = 1;
		switch (EncryptionType) {
		case WFM_UMAC_WEP_ENC_SUPPORTED:
			EncHdrsize = 4;
			EncTagSize = 4;
			break;

		case WFM_UMAC_TKIP_ENC_SUPPORTED:
			EncHdrsize = 8;
			EncTagSize = 12;
			break;

		case WFM_UMAC_AES_ENC_SUPPORTED:
			EncHdrsize = 8;
			EncTagSize = 8;
			break;
#if WAPI_SUPPORT
		case WFM_UMAC_WAPI_ENC_SUPPORTED:
			EncHdrsize = 18;
			EncTagSize = 16;
			break;
#endif	/* WAPI_SUPPORT */
		} /* switch(EncryptionType) */
	}

	OS_MemoryReset(((uint8 *)pDataFrame + sizeof(pDataFrame->dot11hdr) + FrameBodyLen), EncHdrsize);
	/* Include the Encryption header size in Frame body length */
	FrameBodyLen += EncHdrsize;

	pLLC_snap_hdr = (WFM_UMAC_LLC_SNAP_HDR *)((uint8*)pDataFrame + sizeof(pDataFrame->dot11hdr) + FrameBodyLen);
	pLLC_snap_hdr->DSAP = 0xAA;
	pLLC_snap_hdr->SSAP = 0xAA;
	pLLC_snap_hdr->Control = 0x03;
	pLLC_snap_hdr->VendorCode[0] = 0x00;
	pLLC_snap_hdr->VendorCode[1] = 0x00;
	pLLC_snap_hdr->VendorCode[2] = 0x00;
	pLLC_snap_hdr->LocalCode = umc_cpu_to_be16(0x806);
	FrameBodyLen += sizeof(WFM_UMAC_LLC_SNAP_HDR);

	/* Create header */
	WFM_UMAC_CreateDot11FrameHdr(
				UmacHandle,
				WFM_UMAC_DATA,
				SubType,
				&pDataFrame->dot11hdr,
				pUmacInstance->RemoteMacAdd,
				pUmacInstance->MacAddress,
				0,
				Encrypt
				);

	/* Now fill up body */
	pArpReply = (WFM_UMAC_ARP_REPLY *)((uint8*)pDataFrame + sizeof(pDataFrame->dot11hdr) + FrameBodyLen);
	OS_MemoryReset((uint8 *)pArpReply, (WFM_UMAC_ARP_REPLY_SIZE + EncTagSize));
	pArpReply->HwType = umc_cpu_to_be16(0x0001);
	pArpReply->ProtocolType = umc_cpu_to_be16(0x0800);
	pArpReply->HwAddLen = WFM_MAC_ADDRESS_SIZE;
	pArpReply->ProtocolAddLen = WFM_IP_ADDRESS_SIZE;
	pArpReply->Operation = umc_cpu_to_be16(0x0002);

	FrameBodyLen += WFM_UMAC_ARP_REPLY_SIZE;

	/* Include the Encryption tag size in Frame body length */
	FrameBodyLen += EncTagSize;

	/*Set the template frame type here */
	pTemplateFrame->FrameType = WSM_TMPLT_ARP;	/* ARP Data */

	/* TBD */
	pTemplateFrame->Rate = 0xFF;
	/* dont include padding */
	pTemplateFrame->FrameLength = sizeof(pDataFrame->dot11hdr) + FrameBodyLen;

	/* 3. Prepare Topmost layer */
	pMibParams->MibId = WSM_MIB_ID_TEMPLATE_FRAME;
	pMibParams->Length = SIZE_PADDED_TO_MAKE_4_BYTES_ALIGNED(pTemplateFrame->FrameLength + (sizeof(WSM_MIB_TEMPLATE_FRAME) - WSM_MAX_TEMPLATE_FRAME_SIZE)
	    ); /* size has to be 4 byte aligned */

	/*tbd: change is when wsm supports UMAC_LL_REQ_WRITE_MIB() for programming template frame */
	{
		TEMPLATE_FRAME LmacTemplateFrame;

		/* Copy Host Template Frame to LMAC Template Frame */
		LmacTemplateFrame.FrameLength = pTemplateFrame->FrameLength;
		LmacTemplateFrame.FrameType = pTemplateFrame->FrameType;
		LmacTemplateFrame.Rate = pTemplateFrame->Rate;
		LmacTemplateFrame.pFrame = (uint8 *) &(pTemplateFrame->Frame);

		/* Send Template Frame to LMAC. */
		UMAC_LL_REQ_SET_TEMPLATE_FRAME(
						UmacHandle,
						&LmacTemplateFrame,
						pMibParams
						);
	}
	return status;
} /* end WFM_UMAC_WriteMib_TemplateFrame_NullData() */

/******************************************************************************
 * NAME:WFM_UMAC_WriteMib_DisableBSSIDFiltering
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function enable/disable BSSID filtering from firmware.
 * \param UmacHandle    -   Handle to the UMAC Instance.
 * \param DisableBSSIDFilter - 1 : Disable BSSID Filtering
 *                             0 : Enable BSSID Filtering
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_DisableBSSIDFiltering(UMAC_HANDLE UmacHandle,
					    uint8 DisableBSSIDFilter)
{
	MIB_PARAMETERS *pMibParams;
	WSM_MIB_DISABLE_BSSID_FILTER *pDisableBssidFilter;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint32 status = WFM_STATUS_SUCCESS;

	OS_ASSERT(pUmacInstance);

	/* Prepare pointers */
	pMibParams = (MIB_PARAMETERS *) UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);
	pDisableBssidFilter = (WSM_MIB_DISABLE_BSSID_FILTER *) pMibParams->MibData;

	OS_MemoryReset(pDisableBssidFilter, sizeof(*pDisableBssidFilter));
	/* Prepare Bottom layer */
	pDisableBssidFilter->Filter = DisableBSSIDFilter;

	/* Prepare Top layer */
	pMibParams->MibId = WSM_MIB_ID_DISABLE_BSSID_FILTER;
	pMibParams->Length = sizeof(WSM_MIB_DISABLE_BSSID_FILTER);

	UMAC_LL_REQ_WRITE_MIB(UmacHandle, pMibParams);

	return status;

}
#if 0
uint32 WFM_UMAC_SendReqToWsm_JoinReqAll(UMAC_HANDLE UmacHandle,
				     uint8 Mode,
				     uint8 Band,
				     uint16 Channel,
				     uint8 Bssid[],
				     uint16 AtimWindow,
				     uint8 PreambleType,
				     uint8 ProbeForJoin,
				     uint8 Flags,
				     uint32 SSIDLen,
				     uint8  SSID[],
				     uint32 BeaconInterval,
				     uint32 BasicRateSet
				     )
{
	uint32 Result = WFM_STATUS_SUCCESS;
	JOIN_PARAMETERS *pJoinParams = NULL;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_ASSERT(pUmacInstance);

	OS_ASSERT(!pUmacInstance->gVars.pJoinTxDesc);

	pUmacInstance->gVars.pJoinTxDesc = (UMAC_TX_DESC *) UMAC_LL_ALLOCATE_INTERNAL_FRAME_BUFF(UmacHandle);

	OS_ASSERT(pUmacInstance->gVars.pJoinTxDesc);

	UMAC_MEM_STAMP_INTERNAL_BUFFER(pUmacInstance->gVars.pJoinTxDesc, UMAC_JOIN_REQ);

	WFM_UMAC_DBG_STORE_TX_DESC(pUmacInstance->gVars.pJoinTxDesc);

	pJoinParams = (JOIN_PARAMETERS *) TX_DESC_GET_DOT11FRAME(pUmacInstance->gVars.pJoinTxDesc);
	CHECK_POINTER_IS_VALID(pJoinParams);

	pJoinParams->Mode = Mode;
	pJoinParams->Band = Band;
	pJoinParams->ChannelNumber = Channel;
	OS_MemoryCopy(pJoinParams->BSSID, Bssid, 6);
	pJoinParams->AtimWindow = AtimWindow;
	pJoinParams->PreambleType = PreambleType;
	pJoinParams->ProbeForJoin = ProbeForJoin;
	pJoinParams->Flags = Flags;
	pJoinParams->SSIDLength = SSIDLen;
	OS_MemoryCopy(pJoinParams->SSID, SSID, SSIDLen);
	pJoinParams->BeaconInterval = BeaconInterval;
	pJoinParams->BasicRateSet = BasicRateSet;

	Result = UMAC_LL_REQ_JOIN(UmacHandle, pJoinParams);

	if (Result != WFM_STATUS_SUCCESS) {
		UMAC_MEM_REMOVE_STAMP(pUmacInstance->gVars.pJoinTxDesc);

		WFM_UMAC_DBG_REMOVE_TX_DESC(pUmacInstance->gVars.pJoinTxDesc);

		UMAC_LL_RELEASE_INTERNAL_FRAME_BUFF(
					UmacHandle,
					pUmacInstance->gVars.pJoinTxDesc
					);

		pUmacInstance->gVars.pJoinTxDesc = NULL;

	} /* if(Result != WFM_STATUS_SUCCESS) */

	return Result;
}
#endif
uint32 WFM_UMAC_SendReqToWsm_StartReq(UMAC_HANDLE UmacHandle,
				      uint8 Mode,
				      uint8 Band,
				      uint16 Channel,
				      uint32 CTWindow,
				      uint32 BeaconInterval,
				      uint8 DTIM,
				      uint8 PreambleType,
				      uint8 ProbeDelay,
				      uint8 SSIDLength,
				      uint8 SSID[],
				      uint32 BasicRate)
{
	WSM_HI_START_REQ *pStartReq;
	uint32 status = WFM_STATUS_SUCCESS;

	pStartReq = (WSM_HI_START_REQ *)UMAC_MIB_ALLOCATE_BUFFER(UmacHandle);

	pStartReq->Mode = Mode;
	pStartReq->Band = Band;
	pStartReq->ChannelNumber = Channel;
	pStartReq->CTWindow = CTWindow;
	pStartReq->BeaconInterval = BeaconInterval;
	pStartReq->DTIMPeriod = DTIM;
	pStartReq->PreambleType = PreambleType;
	pStartReq->ProbeDelay = ProbeDelay;
	pStartReq->SsidLength = SSIDLength;
	if (SSIDLength)
		OS_MemoryCopy(pStartReq->Ssid, SSID, SSIDLength);
	pStartReq->BasicRateSet = BasicRate;

	UMAC_LL_REQ_START(UmacHandle, pStartReq);

	return status;
}

uint32 UMAC_SendReqToWsm_Reset(UMAC_HANDLE UmacHandle,
			       uint32 Flags,
			       uint8 linkId)
{
	uint32 status = WFM_STATUS_SUCCESS;
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	pUmacInstance->ResetReaminch = 1;
	UMAC_LL_REQ_RESET_JOIN(UmacHandle, 0x00, DEFAULT_LINK_ID);
	return status;
}
/******************************************************************************
 * NAME:	UMAC_ScanReqACS
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
uint32 UMAC_ScanReqACS(UMAC_HANDLE UmacHandle, void *pMsg)
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

	OS_ASSERT(pUmacInstance);

	LOG_EVENT(DBG_WFM_UMAC | DBG_WFM_SCAN, "UMAC_ScanReqACS()\n");

	OS_ASSERT(pMsg);

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
					pScanParams->NumOfChannels = (uint8) pWfmListScan->NumberOfChannels_2_4Ghz;
					for (Count = 0; Count < pScanParams->NumOfChannels; Count++) {
						pScanParams->Channels[Count].ChannelNum = pWfmListScan->Channels[Count].ChannelNum;
						pScanParams->Channels[Count].MinChannelTime = pWfmListScan->Channels[Count].MinChannelTime ? pWfmListScan->Channels[Count].MinChannelTime : WFM_UMAC_PASSIVE_SCAN_MIN_CHANNEL_TIME;
						pScanParams->Channels[Count].MaxChannelTime = pWfmListScan->Channels[Count].MaxChannelTime ? pWfmListScan->Channels[Count].MaxChannelTime : WFM_UMAC_PASSIVE_SCAN_MAX_CHANNEL_TIME;
						pScanParams->Channels[Count].TxPowerLevel = pWfmListScan->Channels[Count].TxPowerLevel ? pWfmListScan->Channels[Count].TxPowerLevel : PowerLevel;
					}

			} else {
				/*5Ghz */
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
		UMAC_SET_STATE(UmacHandle, UMAC_INITIALIZED);

	} else {
		Result = WFM_STATUS_REQ_REJECTED;
		LOG_EVENT(DBG_WFM_UMAC, "***ERROR: WFM_STATUS_REQ_REJECTED!\n\n");
	}

	return Result;

}
#endif	/*P2P_MINIAP_SUPPORT */
