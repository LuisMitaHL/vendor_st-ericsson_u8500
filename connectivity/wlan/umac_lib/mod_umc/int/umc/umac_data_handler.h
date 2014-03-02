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
 * \file umac_data_handler.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_data_handler.h
 * \brief
 * This file implements the functions to handle ethernet
 * to WLAN and WLAN to ethernet frame conversions.
 * \ingroup Upper_MAC_Core
 * \date 25/02/08 10:38
 */

#ifndef _UMAC_DATA_HANDLER_H
#define _UMAC_DATA_HANDLER_H

/******************************************************************************
				  INCLUDE FILES
******************************************************************************/
#include "stddefs.h"
#include "umac_if.h"
#include "umac_frames.h"

/******************************************************************************
				  CONSTANTS
*****************************************************************************/

/*802.1D Priorities*/
#define WFM_802_1D_UP_BE	0
#define WFM_802_1D_UP_BK	1
#define WFM_802_1D_UP_RESERVED  2
#define WFM_802_1D_UP_EE	3
#define WFM_802_1D_UP_CL	4
#define WFM_802_1D_UP_VI	5
#define WFM_802_1D_UP_VO	6
#define WFM_802_1D_UP_NC	7

/*802.11 Access Categories, in terms of priority*/
#define WFM_AC_BK	       0
#define WFM_AC_BE	       1
#define WFM_AC_VI	       2
#define WFM_AC_VO	       3

/*802.11 Access Category Index, in terms of priority*/
#define WFM_ACI_BE	      0
#define WFM_ACI_BK	      1
#define WFM_ACI_VI	      2
#define WFM_ACI_VO	      3

/* Values for handling Ethernet and QoS Frame combination handling */
#define ETHERNET_QOS		0x00
#define ETHERNET_NONQOS		0x01
#define NONETHERNET_QOS		0x02
#define NONETHERNET_NONQOS	0x03

/******************************************************************************
			LOCAL DECLARATIONS
******************************************************************************/

typedef struct WFM_UMAC_LLC_SNAP_HDR_S {
	uint8 DSAP;
	uint8 SSAP;
	uint8 Control;
	uint8 VendorCode[3];
	uint16 LocalCode;
} WFM_UMAC_LLC_SNAP_HDR;

typedef struct UMAC_TX_DATA_HDR_S {
	WFM_UMAC_FRAME_HDR dot11hdr;	/* 4 byte aligned */
	uint16 QoSCntrl;
	uint8 EncHdr[18];
	WFM_UMAC_LLC_SNAP_HDR SnapHdr;

} UMAC_TX_DATA_HDR;

typedef struct UMAC_DOT11_FRAME_INFO_S {
	uint32 tid;			/*IN */
	uint32 EncTagSize;		/*OUT */
	uint32 OffsetLength;		/*OUT */
	uint32 FrmCtrlOffsetFalg;	/*OUT */

} UMAC_DOT11_FRAME_INFO;

/* EtherType Values */
#define	UMAC_EAPOL_ETHER_TYPE	0x8E88

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
 *                       Tag Size [OUT] and Offset Size [OUT]
 * \param linkId       - LinkId of the associated STA(in case of P2P GO/mini
 *                       AP mode).
 * \returns uint8*  A pointer to the start of the 802.11 header.
 *****************************************************************************/
uint8 *WFM_UMAC_Data_Ethernet_2_Dot11(UMAC_HANDLE UmacHandle,
				      UMAC_TX_DATA *pTxDataReq,
				      UMAC_DOT11_FRAME_INFO *pFrameInfo,
				      uint8 linkId);

/******************************************************************************
 * NAME:    WFM_UMAC_Data_Dot11_2_Ethernet
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts a 802.11 frame into a 802.3 frame. It can only
 * handle frames which doesn't contain A-MSDU.
 * \param *pFrame	  - Pointer to the start of dot11 frame.
 * \param FrameLength     - Length Of the Dot11 Frame.
 * \param *TotalLength    - Length[OUT] of the converted 802.3 frame.
 * \param HdrSize	  - Size of 802.11 header including encryption header .
 * \returns uint8*  returns a pointer to the start of the constructed frame.
 *****************************************************************************/
uint8 *WFM_UMAC_Data_Dot11_2_Ethernet(uint8 *pFrame,
				      uint16 FrameLength,
				      uint16 *TotalLength,
				      uint16 HdrSize);

/******************************************************************************
 * NAME:    WFM_UMAC_Parse_Dot11_Header
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function parses a 802.11 frame header to find out the size of the
 * 802.11 header, presence of A-MSDU and data payload length.
 * handle frames which doesn't contain A-MSDU.
 * \param *pFrame	  - Pointer to the start of dot11 frame.
 * \param Length	  - Length of the Dot11 Frame.
 * \param EncHdrSize      - Size of the encryption header.
 * \param EncTagSize      - Size of the encryption trailer.
 * \param *pHdrLen	  - Size[OUT] of the 802.11 header including encryption
 *			    header.
 * \param *pAmsdu	  - Indicates presence of AMSDU [OUT].
 * \returns uint16  returns Number of octets in the payload.
 *****************************************************************************/
uint16 WFM_UMAC_Parse_Dot11_Header(uint8 *pFrame,
				   uint16 Length,
				   uint8 EncHdrSize,
				   uint8 EncTagSize,
				   uint16 *pHdrLen,
				   uint8 *pAmsdu);

/******************************************************************************
 * NAME:    WFM_UMAC_Data_Dot11Amsdu_Count_Subfrm
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function counts the number of subframes present in an A-MSDU.
 * \param *pData	  - Pointer to the start of payload of the dot11 frame.
 * \param Length	  - Length of the payload of the dot11 frame.
 * \returns uint8  returns Number of subframe in the A-MSDU payload.
 *****************************************************************************/
uint16 WFM_UMAC_Data_Dot11Amsdu_Count_Subfrm(uint8 *pData, uint16 DataLen);

/******************************************************************************
 * NAME:    WFM_UMAC_Data_Dot11AmsduSubFrm_2_Dot3Frm
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts an A-MSDU sub-frame into a 802.3 frame.
 * \param *pFrame	 - Pointer[IN] to the start of an AMSDU sub-frame.
 * \param PayLoadLen     - Length[IN] of the 802.11 payload (MSDU).
 * \param *pDot3FrmLen   - Length[OUT] of the constructed 802.3 frame.
 * \param *pSubFrmLen    - Length[OUT] of the A-MSDU subframe including padding.
 * \returns uint8*  returns a pointer to the start of the 802.3 frame.
 *****************************************************************************/
uint8 *WFM_UMAC_Data_Dot11AmsduSubFrm_2_Dot3Frm(uint8 *pFrame,
						uint16 PayLoadLen,
						uint16 *pDot3FrmLen,
						uint16 *pSubFrmLen);

#if P2P_MINIAP_SUPPORT
/******************************************************************************
 * NAME:    UMAC_ModifyDot11Header
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function changes the 802.11 header of the frame which has come from
 * one associated STA and has to be transmitted to another associated STA
 * \param UmacHandle		- Handle to UMAC instance
 * \param *pRxDescriptor	- Pointer to the start of dot11 frame.
 * \param length		- Length of the Dot11 Frame.
 * \returns none.
 *********************************************************************************/
void UMAC_ModifyDot11Header(UMAC_HANDLE UmacHandle,
			    WFM_UMAC_FRAME_HDR *pRxDescriptor,
			    uint16 length,
			    uint8 headerOffset);
#endif	/* P2P_MINIAP_SUPPORT */

#endif	/* _UMAC_DATA_HANDLER_H */
