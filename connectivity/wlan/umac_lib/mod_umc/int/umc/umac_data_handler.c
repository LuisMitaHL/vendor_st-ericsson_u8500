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
 * \file umac_data_handler.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_data_handler.c
 * \brief
 * This file implements the functions to handle ethernet to WLAN and WLAN to
 * ethernet frame conversions.
 * \ingroup Upper_MAC_Core
 * \date 25/02/08 10:38
 */

/******************************************************************************
			      INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "section_wfm.h"

#include "d0_defs.h"

#include "umac_dev_conf.h"
#include "umac_data_handler.h"
#include "umac_frames.h"

#include "umac_sm.h"

/******************************************************************************
			LOCAL DECLARATIONS
******************************************************************************/

typedef struct WFM_UMAC_802_3_FRAME_HDR_S {
	uint8 DestinationAddress[6];
	uint8 SourceAddress[6];
	uint16 EthernetType;
} WFM_UMAC_802_3_FRAME_HDR;

/*
  There are four types of Ethernet frames
  1] Ethernet - II Frames

  6 bytes       6 bytes       2 bytes	 Up to 1500 bytes
  +-------------+-------------+-------------+-------------------------+
  | Destination |   Source    |    E-type   | Network Protocol Packet |
  | MAC Address | MAC Address | (IPX: 8137) |			 |
  +-------------+-------------+-------------+-------------------------+

  2] Ethernet 802.3 Frames ("Raw" 802.3)

  6 bytes       6 bytes       2 bytes	 Up to 1500 bytes
  +-------------+-------------+--------------+------------------------+
  | Destination |   Source    | Total packet |       IPX Packet       |
  | MAC Address | MAC Address |    length    | first two bytes: FF,FF |
  +-------------+-------------+--------------+------------------------+

  3] Ethernet 802.2 (SAP) Frames

  6 b    6 b    2 b      1 b     1 b    1 b     Up to 1497 bytes
  +------+------+--------+------+------+--------+---------------------+
  | Dest | Src  | length | DSAP | SSAP | Control|   Network Packet    |
  | Addr | Addr |	| (E0) | (E0) |  (03)  |		     |
  +------+------+--------+------+------+--------+---------------------+

  4] Ethernet SNAP Frames

  6 b  6 b  2 b 1 byte 1 byte 1 byte     5 bytes     Up to 1492 bytes
  +----+----+---+------+------+------+---------------+----------------+
  |Dst |Src |len| DSAP | SSAP | Ctrl |    SNAP ID    | Network Packet |
  |Addr|Addr|   | 0xAA | 0xAA | 0x03 | (0,0,0,81,37) |		|
  +----+----+---+------+------+------+---------------+----------------+

  To identify a packet as 802.3 raw in a mixed environment, receiving stations
  must first determine whether the value in the length field is less than 1518
  bytes (to distinguish it from an Ethernet II frame type).

  If so, they check the value of the next two bytes. If it is FF-FF,
  they know it is an IPX packet because FF-FF isn't currently used in 802.2
  DSAP and SSAP fields.

  Algorithm
  if ( (Length_Type <= 1518) && (First Two Bytes == FFFF) )
  {
  Process Raw 802.3
  }
  else ( (Length_Type > 1518) )
  {
  Ethernet II
  }
  else if ( (Length_Type <= 1518) && (First Two Bytes == FFFF) )

*/

/******************************************************************************
 * NAME:	WFM_UMAC_Data_Ethernet_2_Dot11
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts Ethernet frames coming from the host to 802.11 Frames.
 * This function removes ethernet header and prepend 802.11 header and LLC-SNAP
 * header before the payload.
 *
 * If ethernet type/length filed is less than 1518 [Its length field] its an
 * 802.3  frame and there should be a LLC-SNAP header With it. Its illegal not
 * to have an LLC-SNAP header with 802.3 frame.
 *
 * If the ethernet/Length field is greater than 1518[Its type field] it's
 * Ethernet frame, so there may or maynot be a SNAP header.So we have to check
 * whether there's a SNAP header present and if not attach one.
 *
 * In any case , the outgoing 802.11 [WLAN] frame has to have a SNAP header.
 * \param UmacHandle   - Handle to the UMAC Instance.
 * \param *pTxDataReq  - Pointer to the Tx Data Structure.
 * \param *pFrameInfo  - Information about the frame like TID [IN], Encryption
 *		         Tag Size [OUT] and Offset Size [OUT]
 * \returns uint8*  A pointer to the start of the 802.11 header.
 *****************************************************************************/
uint8 *WFM_UMAC_Data_Ethernet_2_Dot11(UMAC_HANDLE UmacHandle,
				      UMAC_TX_DATA *pTxDataReq,
				      UMAC_DOT11_FRAME_INFO *pFrameInfo,
				      uint8 linkId)
{
	WFM_UMAC_802_3_FRAME_HDR *p802_3FrameHdr;
	WFM_UMAC_FRAME_HDR *pDot11Header;
	UMAC_TX_DATA_HDR UmcTxHdr;
	uint8 *pMemPtr;
	uint16 EtherType;
	uint8 SubType = WFM_UMAC_SUB_DATA;
	uint8 EncHdrsize = 0;
	uint8 EncTagSize = 0;
	uint16 Dot11HeaderOffset = sizeof(WFM_UMAC_FRAME_HDR);
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint8 Encrypt = 0;
	uint32	align_offset;
	uint8  EnableQoS = TRUE;

	OS_ASSERT(pUmacInstance);

	CHECK_POINTER_IS_VALID(pLength);
	CHECK_POINTER_IS_VALID(pFrameInfo);

	/*
	   Note: In this version we are handling only ethernet - II frames
	 */

	p802_3FrameHdr = (WFM_UMAC_802_3_FRAME_HDR *) pTxDataReq->pEtherHeader;

	/*
	   Backing up a copy of the ethernet header as we are going to over
	   write this memory with 802.11  and SNAP header.
	   copiying it manualy to speed up Tx
	 */

	EtherType = p802_3FrameHdr->EthernetType;
	UmcTxHdr.dot11hdr.SeqCntrl = 0;

	if (pUmacInstance->operatingMode >= OPERATING_MODE_AP)
		EnableQoS = pUmacInstance->sta[linkId].isWMMEnabled;

	if (((pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION) ||
		(pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION))
#if P2P_MINIAP_SUPPORT
		&& (EnableQoS)
#endif	/* P2P_MINIAP_SUPPORT */
	    ) {
		/*This link supports QoS, so insert QoS Control Field */

		SubType = WFM_UMAC_SUB_DATA_QOS;

		pMemPtr = (uint8 *) &UmcTxHdr.EncHdr[0];

		Dot11HeaderOffset += 2;

		/*Adding the size of QoS field too to the length */

		/* Setting Ack policy to 00 [ normal ack ]
		   Setting bit-4 ESOP
		   Setting QueueSize to 0xff [which indicates Unkown or
		   unspecified value as per the spec]
		 */

		UmcTxHdr.QoSCntrl = 0xff10;

		if (pUmacInstance->Flags & WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION) {
			/*
			   As per WMM specification, bits 7 - 15 has are
			   reserved and has to be zero.Event bit 3 has to
			   be zero. Setting ESOP to zero by default
			 */

			UmcTxHdr.QoSCntrl = UmcTxHdr.QoSCntrl & 0x00E7;

			/*Assigning User Priority */
			pFrameInfo->tid = ((pFrameInfo->tid) & 0x07);

		} else {
			/*Assigning TID */
			pFrameInfo->tid = ((pFrameInfo->tid) & 0x0F);
		}

		UmcTxHdr.QoSCntrl = (uint16) (UmcTxHdr.QoSCntrl | (pFrameInfo->tid));
	} else
		pMemPtr = (uint8 *) &UmcTxHdr.QoSCntrl;

	if (pUmacInstance->operatingMode <= OPERATING_MODE_P2P_CLIENT) {
		if (pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[0] && (umc_be16_to_cpu(EtherType) != UMAC_WAI_AUTH_ETHER_TYPE))
			Encrypt = 1;
	} else {
		if (linkId && pUmacInstance->gVars.p.InfoHost.EncKeyAvailable[linkId])
			Encrypt = 1;
		else if ((linkId == 0) && pUmacInstance->gVars.p.InfoHost.GroupKeyInstalled)
			Encrypt = 1;
	}

	/*protect frame only when key is available */
	if (Encrypt) {
		switch (pUmacInstance->gVars.p.InfoHost.EncryptionType) {
		case WFM_UMAC_WEP_ENC_SUPPORTED:
			EncHdrsize = 4;
			EncTagSize = 4;

			LOG_DATA(DBG_WFM_UMAC | DBG_WFM_TX | DBG_WFM_ENCRYPTION, "WEP-TX, EncType=%d\n", pUmacInstance->gVars.p.InfoHost.EncryptionType);

			break;

		case WFM_UMAC_TKIP_ENC_SUPPORTED:
			EncHdrsize = 8;
			EncTagSize = 12;

			LOG_DATA(DBG_WFM_UMAC | DBG_WFM_TX | DBG_WFM_ENCRYPTION, "TKIP-TX, EncType=%d\n", pUmacInstance->gVars.p.InfoHost.EncryptionType);

			break;

		case WFM_UMAC_AES_ENC_SUPPORTED:
			EncHdrsize = 8;
			EncTagSize = 8;

			LOG_DATA(DBG_WFM_UMAC | DBG_WFM_TX | DBG_WFM_ENCRYPTION, "AES-TX, EncType=%d\n", pUmacInstance->gVars.p.InfoHost.EncryptionType);

			break;
#if WAPI_SUPPORT
		case WFM_UMAC_WAPI_ENC_SUPPORTED:
			EncHdrsize = 18;
			EncTagSize = 16;

			LOG_DATA(DBG_WFM_UMAC | DBG_WFM_TX | DBG_WFM_ENCRYPTION, "WAPI-TX, EncType=%d\n", pUmacInstance->gVars.p.InfoHost.EncryptionType);

			break;
#endif	/* WAPI_SUPPORT */
		default:
			EncHdrsize = 0;
			EncTagSize = 0;

			LOG_DATA(DBG_WFM_UMAC | DBG_WFM_TX | DBG_WFM_ERROR | DBG_WFM_ENCRYPTION, "ERROR: Invalid Encryption mode (%d)!\n", pUmacInstance->gVars.p.InfoHost.EncryptionType);

		} /* switch(pUmacInstance->gVars.p.InfoHost.EncryptionType) */
	}

	/*
	   If encryption is enabled, leave space for IV immediatly after 802.11
	   header.
	 */

	Dot11HeaderOffset += EncHdrsize;

	pMemPtr += EncHdrsize;

	/*Inserting LLC - SNAP header */

	if (umc_be16_to_cpu(EtherType) >= 1518) {
		/*
		   Its an Ethernet-II Frame
		*/

		/* Copying first 8 bytes of ethernet payload to stack */
		OS_MemoryCopy(
				(void *)&UmcTxHdr.SnapHdr,
				(void *)pTxDataReq->pEtherPayLoad,
				sizeof(WFM_UMAC_LLC_SNAP_HDR)
				);

		/* Check for SNAP Header - 0xAA 0xAA 0x03 */
		if ((UmcTxHdr.SnapHdr.DSAP != 0xAA)
		    || (UmcTxHdr.SnapHdr.SSAP != 0xAA)
		    || (UmcTxHdr.SnapHdr.Control != 0x03)
		    ) {
			WFM_UMAC_LLC_SNAP_HDR *pLLC_snap_hdr;

			/* LLC -SNAP header is not present, so add one */

			pLLC_snap_hdr = (WFM_UMAC_LLC_SNAP_HDR *) pMemPtr;

			pLLC_snap_hdr->DSAP = 0xAA;
			pLLC_snap_hdr->SSAP = 0xAA;
			pLLC_snap_hdr->Control = 0x03;

			/*
			   The first 3 bytes of the SNAP header is the vendor
			   code, generally the same as the first three bytes
			   of the source address although it is sometimes set
			   to zero.
			*/

			pLLC_snap_hdr->VendorCode[0] = 0;
			pLLC_snap_hdr->VendorCode[1] = 0;
			pLLC_snap_hdr->VendorCode[2] = 0;

			pLLC_snap_hdr->LocalCode = EtherType;

			/*
			   There's no SNAP header, so add one, so reserve space
			   for SNAP header
			*/

			Dot11HeaderOffset += sizeof(WFM_UMAC_LLC_SNAP_HDR);

		}
	} /* if ( umc_be16_to_cpu( EtherType )  >= 1518 ) */

	/*Inserting 802.11 Header */
	WFM_UMAC_CreateDot11FrameHdr(
					UmacHandle,
					WFM_UMAC_DATA,
					SubType,
					&UmcTxHdr.dot11hdr,
					p802_3FrameHdr->DestinationAddress,
					p802_3FrameHdr->SourceAddress,
					EtherType,
					Encrypt
					);

	/*
	   We are overwriting the memory occupied by Ethernet Header,
	   so length should be adjusted to compensate that
	 */
	Dot11HeaderOffset -= WFM_ETH_FRAME_HDR_SIZE;

	/*Updating the output parameter with the Encryption Tag Size */
	pFrameInfo->EncTagSize = EncTagSize;

	pFrameInfo->OffsetLength = Dot11HeaderOffset;

	/*pDot11Header = (WFM_UMAC_FRAME_HDR *) ((uint8 *) pTxDataReq->pUlHdr + (sizeof(UMAC_TX_REQ_HDR) - SIZE_PADDED_TO_MAKE_4_BYTES_ALIGNED(Dot11HeaderOffset))); */

	pDot11Header = (WFM_UMAC_FRAME_HDR *) ((uint8 *) pTxDataReq->pUlHdr + (sizeof(UMAC_TX_REQ_HDR) - Dot11HeaderOffset));

	OS_MemoryCopy(
		(void *)pDot11Header,
		(void *)&UmcTxHdr.dot11hdr, Dot11HeaderOffset
		);

	OS_MemoryCopy(
		(void *)pTxDataReq->pEtherHeader,
		(void *)((uint8 *) &UmcTxHdr.dot11hdr + Dot11HeaderOffset),
		WFM_ETH_FRAME_HDR_SIZE
		);

	/*
	 * The start of the HI message should be 4 byte aligned to facilitate
	 * DMA. The size of UMAC_TX_DESC is already multiple of 4 byte. So we
	 * check if the start of dot11hdr is 4 byte aligned. In case it is only
	 * 2 byte aligned we leave a 2 byte gap between UMAC_TX_DESC and
	 * dot11hdr and set the flag in UMAC_TX_DESC to indicate the gap to FW.
	 * Note: we assume the address will always be even number.
	 */
	align_offset = (uint32)pDot11Header & 0x03;

	if (align_offset == 2) {
		pFrameInfo->OffsetLength += 2;
		pDot11Header = (WFM_UMAC_FRAME_HDR *)((uint8 *)pDot11Header - 2);
		pFrameInfo->FrmCtrlOffsetFalg = TRUE;
	}


	return (uint8 *) pDot11Header;

} /* end WFM_UMAC_Convert_Ethernet_2_Dot11() */

/******************************************************************************
 * NAME:    WFM_UMAC_Data_Dot11_2_Ethernet
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts a 802.11 frame into a 802.3 frame. It can only
 * handle frames which doesn't contain A-MSDU.
 * \param *pFrame	 - Pointer to the start of dot11 frame.
 * \param FrameLength    - Length Of the Dot11 Frame.
 * \param *TotalLength   - Length[OUT] of the converted 802.3 frame.
 * \param HdrSize	 - Size of 802.11 header including encryption header .
 * \returns uint8*  returns a pointer to the start of the constructed frame.
 *****************************************************************************/
uint8 *WFM_UMAC_Data_Dot11_2_Ethernet(uint8 *pFrame,
				      uint16 FrameLength,
				      uint16 *TotalLength,
				      uint16 HdrSize)
{
	WFM_UMAC_DATA_FRAME *pUmacDataFrame = NULL;
	uint32 AddrDiff = 0;
	uint16 *pFrameCtrl = (uint16 *) pFrame;
	uint16 Len;
	uint8 *pLenPtr;
	WFM_UMAC_LLC_SNAP_HDR *pLLC_snap_hdr;
	WFM_UMAC_FRAME_HDR UmacFrameHdr;
	WFM_UMAC_LLC_SNAP_HDR LLC_snap_hdr;

	/* Ethernet frame to be transmitted to host */
	WFM_UMAC_802_3_FRAME_HDR *p802_3FrameHdr = NULL;

	CHECK_POINTER_IS_VALID(pFrame);
	OS_ASSERT(TotalLength);

	pUmacDataFrame = (WFM_UMAC_DATA_FRAME *) pFrame;

	/*
	 * Backup the WLAN header info. for future use, doing it manualy
	 * to speed up Rx
	 */

	UmacFrameHdr.FrameControl = pUmacDataFrame->dot11hdr.FrameControl;
	UmacFrameHdr.Duration_Id = pUmacDataFrame->dot11hdr.Duration_Id;
	D0_ADDR_COPY(UmacFrameHdr.Address1, pUmacDataFrame->dot11hdr.Address1);
	D0_ADDR_COPY(UmacFrameHdr.Address2, pUmacDataFrame->dot11hdr.Address2);
	D0_ADDR_COPY(UmacFrameHdr.Address3, pUmacDataFrame->dot11hdr.Address3);

	UmacFrameHdr.SeqCntrl = pUmacDataFrame->dot11hdr.SeqCntrl;

	/* Back up LLC , SNAP header */

	pLLC_snap_hdr = (WFM_UMAC_LLC_SNAP_HDR *) (pFrame + HdrSize);

	LLC_snap_hdr.DSAP = pLLC_snap_hdr->DSAP;
	LLC_snap_hdr.SSAP = pLLC_snap_hdr->SSAP;
	LLC_snap_hdr.Control = pLLC_snap_hdr->Control;
	LLC_snap_hdr.VendorCode[0] = pLLC_snap_hdr->VendorCode[0];
	LLC_snap_hdr.VendorCode[1] = pLLC_snap_hdr->VendorCode[1];
	LLC_snap_hdr.VendorCode[2] = pLLC_snap_hdr->VendorCode[2];
	LLC_snap_hdr.LocalCode = pLLC_snap_hdr->LocalCode;

	/* pFrame  = (uint8 *)pFrame + HdrSize  ; */

	/*
	 * --------------------------------------------
	 * |802.11 Header  | LLC- SNAP Header | Payload |
	 * --------------------------------------------
	 */

	if ((LLC_snap_hdr.DSAP == 0xAA) && (LLC_snap_hdr.SSAP == 0xAA)
	    && (LLC_snap_hdr.Control == 0x03)
	    && (LLC_snap_hdr.VendorCode[0] == 0x00)
	    && (LLC_snap_hdr.VendorCode[1] == 0x00)
	    && (LLC_snap_hdr.VendorCode[2] == 0x00)) {
		/*Its an Ethernet packet */

		/*
		 * Get the memory location where ethernet header should be
		 * inserted
		 * The pointer p802_3FrameHdr should be pointed to a location
		 * lesser than pUmacDataFrame->FrameBody
		 */

		AddrDiff = (uint32) (sizeof(WFM_UMAC_LLC_SNAP_HDR)
				     + HdrSize) - sizeof(WFM_UMAC_802_3_FRAME_HDR);

		p802_3FrameHdr = (WFM_UMAC_802_3_FRAME_HDR *) (uint8 *)
		    (pFrame + AddrDiff);

		/*
		 * Now we should extract the ethernet type/ Length field from
		 * SNAP header
		 */

		p802_3FrameHdr->EthernetType = LLC_snap_hdr.LocalCode;

	} else {
		/*Its not an ethernet packet, so pass it to the Host as below
		   ---------------------------------------------------------------------------
		   | Destination Address |Source Address|Length  | LLC- SNAP Header | Payload |
		   ----------------------------------------------------------------------------
		 */

		AddrDiff = (uint32) (HdrSize - sizeof(WFM_UMAC_802_3_FRAME_HDR));

		p802_3FrameHdr = (WFM_UMAC_802_3_FRAME_HDR *) (pFrame + AddrDiff);

		/*
		 * Length of the MAC Payload
		 * In the case of Non-QoS frames direct assignment wont work as EthernetType
		 * [In this case Length] is not in 4 byte boundary.
		 */

		Len = (uint16) (FrameLength - AddrDiff - sizeof(WFM_UMAC_802_3_FRAME_HDR));

		pLenPtr = (uint8 *) &p802_3FrameHdr->EthernetType;

		/*8 bit MSB should be in the first octet Section 10.4 Standard 802-2001 */
		*pLenPtr = (uint8) ((Len >> 8) & 0xFF);
		/*8 bit LSB should be in the second octet */
		*(pLenPtr + 1) = (uint8) (Len & 0xFF);
	}

	*TotalLength = (uint16) (FrameLength - AddrDiff);

#if P2P_MINIAP_SUPPORT
	if (IS_BROADCAST_MAC_ADDRESS(UmacFrameHdr.Address3) || IS_MULTICAST_MAC_ADDRESS(UmacFrameHdr.Address3)) {
		/*Possible only if a broadcast frame is received by the mini AP */
		D0_ADDR_COPY(
				p802_3FrameHdr->DestinationAddress,
				UmacFrameHdr.Address3
				);
	} else {
#endif	/*P2P_MINIAP_SUPPORT */

		D0_ADDR_COPY(
				p802_3FrameHdr->DestinationAddress,
				UmacFrameHdr.Address1
				);

#if P2P_MINIAP_SUPPORT
	}
#endif	/*P2P_MINIAP_SUPPORT */

	if ((*pFrameCtrl) & 0x0200) {
		/*
		  If FromDs bit is set Source Address has to be taken from A3
		*/
		D0_ADDR_COPY(
				p802_3FrameHdr->SourceAddress,
				UmacFrameHdr.Address3
				);
	} else {
		/*
		  If FromDs bit is not set, Source Address should be taken
		  from A2
		*/
		D0_ADDR_COPY(
				p802_3FrameHdr->SourceAddress,
				UmacFrameHdr.Address2
				);
	}

	return (uint8 *) p802_3FrameHdr;

} /* end WFM_UMAC_Data_Dot11_2_Ethernet() */

/******************************************************************************
 * NAME:    WFM_UMAC_Parse_Dot11_Header
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function parses a 802.11 frame header to find out the size of the
 * 802.11 header, presence of A-MSDU and the number of MSDU in the frame in case
 * A-MSDU is present.
 * handle frames which doesn't contain A-MSDU.
 * \param *pFrame	 - Pointer to the start of dot11 frame.
 * \param Length         - Length of the Dot11 Frame.
 * \param EncHdrSize     - Size of the encryption header.
 * \param EncTagSize     - Size of the encryption trailer.
 * \param *pHdrLen	 - Size[OUT] of the 802.11 header including encryption
 *			   header.
 * \param *pAmsdu	 - Indicates presence of AMSDU [OUT].
 * \returns uint16  returns Number of octets in the payload.
 *****************************************************************************/
uint16 WFM_UMAC_Parse_Dot11_Header(uint8 *pFrame,
				   uint16 Length,
				   uint8 EncHdrSize,
				   uint8 EncTagSize,
				   uint16 *pHdrLen,
				   uint8 *pAmsdu)
{
	uint16 *pFrameCtrl = (uint16 *) pFrame;
	uint16 *pQoSCtrl;
	WFM_UMAC_DATA_FRAME *pUmacDataFrame;
	uint8 SubType;
	uint16 DataLen = 0;
	uint16 hdrLen = sizeof(WFM_UMAC_FRAME_HDR) + EncHdrSize;

	*pAmsdu = 0;

	pUmacDataFrame = (WFM_UMAC_DATA_FRAME *) pFrame;

	SubType = (uint8) (((*pFrameCtrl) >> 4) & 0x000F);

	if (!(SubType & WFM_UMAC_SUB_DATA_NULL)) {
		/*Its not NULL Data , so process it further. */
		if (SubType & WFM_UMAC_SUB_DATA_QOS) {
			/*This is QoS data */
			hdrLen += 2;

			/*Extracting QoS Ctrl info. for the current packet */
			pQoSCtrl = (uint16 *) &pUmacDataFrame->FrameBody[0];

			if (*pFrameCtrl & 0x8000) {
				/*
				  Order bit is set in Qos frame,
				  so HT ctrl present
				*/
				hdrLen += 4;
			}

			if (*pQoSCtrl & (1 << 7)) {
				/* Check for presence of AMSDU */
				*pAmsdu = 1;
			}
		}
		DataLen = Length - (hdrLen + EncTagSize);
	}

	*pHdrLen = hdrLen;
	return DataLen;
}

/******************************************************************************
 * NAME:    WFM_UMAC_Data_Dot11Amsdu_Count_Subfrm
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function counts the number of subframes present in an A-MSDU.
 * \param *pData	  - Pointer to the start of payload of the dot11 frame.
 * \param Length	  - Length of the payload of the dot11 frame.
 * \returns uint16  returns Number of subframe in the A-MSDU payload.
 *****************************************************************************/
uint16 WFM_UMAC_Data_Dot11Amsdu_Count_Subfrm(uint8 *pData, uint16 DataLen)
{
	uint16 count = 0;
	uint16 Length, SubFrmLen;
	WFM_UMAC_AMSDU_SUBFRM_HDR *pAmsduSubFrm;

	while (1) {
		if (DataLen <= (sizeof(WFM_UMAC_AMSDU_SUBFRM_HDR)
				+ sizeof(WFM_UMAC_LLC_SNAP_HDR))) {
			break;
		}
		pAmsduSubFrm = (WFM_UMAC_AMSDU_SUBFRM_HDR *) pData;

		Length = (pAmsduSubFrm->Length << 8) | (pAmsduSubFrm->Length >> 8);

		if (Length < sizeof(WFM_UMAC_LLC_SNAP_HDR) || (Length > 2304)
		    || (Length + sizeof(WFM_UMAC_AMSDU_SUBFRM_HDR) > DataLen)) {
			break;
		}

		count++;

		SubFrmLen = sizeof(WFM_UMAC_AMSDU_SUBFRM_HDR) + Length;

		if ((SubFrmLen + sizeof(WFM_UMAC_AMSDU_SUBFRM_HDR)) < DataLen)
			SubFrmLen = (SubFrmLen + 3) & ~(uint16)0x3;
		else
			break;

		pData += SubFrmLen;
		DataLen -= SubFrmLen;
	}

	return count;
}

/******************************************************************************
 * NAME:    WFM_UMAC_Data_Dot11AmsduSubFrm_2_Dot3Frm
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts an A-MSDU sub-frame into a 802.3 frame.
 * \param *pFrame	- Pointer[IN] to the start of an AMSDU sub-frame.
 * \param PayLoadLen    - Length[IN] of the 802.11 payload (MSDU).
 * \param *pDot3FrmLen  - Length[OUT] of the constructed 802.3 frame.
 * \param *pSubFrmLen   - Length[OUT] of the A-MSDU subframe including padding.
 * \returns uint8*  returns a pointer to the start of the 802.3 frame.
 *****************************************************************************/
uint8 *WFM_UMAC_Data_Dot11AmsduSubFrm_2_Dot3Frm(uint8 *pFrame,
						uint16 PayLoadLen,
						uint16 *pDot3FrmLen,
						uint16 *pSubFrmLen)
{
	WFM_UMAC_AMSDU_SUBFRM_HDR *pAmsduSubFrm;
	WFM_UMAC_LLC_SNAP_HDR *pLLcSnap;
	uint16 Length;
	WFM_UMAC_802_3_FRAME_HDR *pDot3Frame = NULL;

	*pDot3FrmLen = *pSubFrmLen = 0;

	pAmsduSubFrm = (WFM_UMAC_AMSDU_SUBFRM_HDR *) pFrame;
	pLLcSnap = (WFM_UMAC_LLC_SNAP_HDR *) (pAmsduSubFrm + 1);

	Length = ((pAmsduSubFrm->Length << 8) | (pAmsduSubFrm->Length >> 8));
	Length += sizeof(WFM_UMAC_AMSDU_SUBFRM_HDR);

	*pSubFrmLen = (Length + 3) & ~(uint16)0x3;

	/*
	 * Check if ethernet frame is encapsulated.
	 * For non ethernet frame, AMSDU subframes
	 * maps to 802.3 frames. So no change is needed.
	 */

	if ((pLLcSnap->DSAP == 0xAA)
	    && (pLLcSnap->SSAP == 0xAA)
	    && (pLLcSnap->Control == 0x03)
	    && (pLLcSnap->VendorCode[0] == 0x00)
	    && (pLLcSnap->VendorCode[1] == 0x00)
	    && (pLLcSnap->VendorCode[2] == 0x00)) {
		/*
		 * It is an ethernet frame
		 */
		pFrame += sizeof(WFM_UMAC_LLC_SNAP_HDR);
		Length -= sizeof(WFM_UMAC_LLC_SNAP_HDR);
		pDot3Frame = (WFM_UMAC_802_3_FRAME_HDR *) pFrame;
		pDot3Frame->EthernetType = pLLcSnap->LocalCode;
		D0_ADDR_COPY(
				pDot3Frame->SourceAddress,
				pAmsduSubFrm->SrcAddr
				);

		D0_ADDR_COPY(
				pDot3Frame->DestinationAddress,
				pAmsduSubFrm->DestAddr
				);
	}

	*pDot3FrmLen = Length;

	return pFrame;
}

#if P2P_MINIAP_SUPPORT
/******************************************************************************
 * NAME:    UMAC_ModifyDot11Header
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function changes the 802.11 header of the frame which has come from one
 * associated STA and has to be transmitted to another associated STA
 * \param UmacHandle	 - Handle to UMAC instance
 * \param *pdot11Hdr	 - Pointer to the start of 802.11 frame.
 * \param length	     - Length of the Dot11 Frame.
 * \returns none.
 *****************************************************************************/
void UMAC_ModifyDot11Header(UMAC_HANDLE UmacHandle,
			    WFM_UMAC_FRAME_HDR *pdot11Hdr,
			    uint16 length,
			    uint8 headerOffset)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;

	OS_MemoryCopy(
			pdot11Hdr->Address1,
			pdot11Hdr->Address3,
			WFM_MAC_ADDRESS_SIZE
			);

	OS_MemoryCopy(
			pdot11Hdr->Address3,
			pdot11Hdr->Address2,
			WFM_MAC_ADDRESS_SIZE
			);

	OS_MemoryCopy(
			pdot11Hdr->Address2,
			pUmacInstance->MacAddress,
			WFM_MAC_ADDRESS_SIZE
			);

	pdot11Hdr->FrameControl |= UMAC_FROM_DS;
	pdot11Hdr->FrameControl &= ~UMAC_TO_DS;
	pdot11Hdr->FrameControl &= ~UMAC_PS;
	pdot11Hdr->FrameControl &= ~UMAC_RETRY;

	if (headerOffset) {
		pdot11Hdr->FrameControl &= ~UMAC_FRAME_SUBTYE_MASK;
		pdot11Hdr->FrameControl |= WFM_UMAC_SUB_DATA;
	}
}

#endif	/*P2P_MINIAP_SUPPORT */
