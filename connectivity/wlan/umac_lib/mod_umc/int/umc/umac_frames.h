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
 * \file umac_frames.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_frames.h
 * \brief
 * This module handles the frame creation and conversion.
 * \ingroup Upper_MAC_Core
 * \date 15/02/08 16:58
 */

#ifndef _UMAC_FRAMES_H
#define _UMAC_FRAMES_H

/******************************************************************************
	      INCLUDE FILES
******************************************************************************/
#include "stddefs.h"
#include "umac_messages.h"
#include "umac_if.h"

#define WFM_UMAC_PROTOCOL_VERSION     0

#if P2P_MINIAP_SUPPORT
#define UMAC_HI_MSG_ID			0x003F
#define UMAC_HI_MSG_LINK_ID		0x03C0
#define UMAC_HI_MSG_TYPE		0x0C00
#define UMAC_UPDATE_IE_FLAG_BCN		0x0001
#define UMAC_UPDATE_IE_FLAG_PRB_RESP	0x0002
#define UMAC_UPDATE_IE_FLAG_PRB_REQ	0x0004
#define UMAC_P2P_ENABLED		1
#define UMAC_CONTINUOUS_FIND		0xFFFF
/* Rx Desc + 802.11 Hdr + Encr Hdr +SNAP Hdr */
#define BC_MC_BUFFER_SIZE		(20 + 26 + 18 + 8)
#define TX_DESC_QOS_HDR_SIZE            (24/*Tx Desc*/ + 26/* 802.11 hdr*/)
#endif	/* P2P_MINIAP_SUPPORT */

/*
  Redefining a macro for MAC address size here
  to avoid the inclusion of umac_messages.h in
  this header
*/
#define WFM_UMAC_MAC_ADDRESS_SIZE	6
#define WFM_UMAC_MAX_FRAME_BODY_SIZE	2312

#define WFM_UMAC_ARP_REPLY_SIZE		28

#if P2P_MINIAP_SUPPORT
/*Frame Control bits*/
#define UMAC_TO_DS			0x0100
#define UMAC_FROM_DS			0x0200
#define UMAC_PS				0x1000
#define UMAC_RETRY			0x0800
#define UMAC_MC_BC_FRAME		0x80000000
#define UMAC_FRAME_SUBTYE_MASK         0x00F0
#endif	/* P2P_MINIAP_SUPPORT */

/*Frame Types*/
#define WFM_UMAC_MGMT			0x00
#define WFM_UMAC_CTRL			0x01
#define WFM_UMAC_DATA			0x02

/*
  Sub Types for Management Frames
*/
#define WFM_UMAC_MGMT_ASSOC_REQ		0x00
#define WFM_UMAC_MGMT_ASSOC_RSP		0x01
#define WFM_UMAC_MGMT_REASSOC_REQ	0x02
#define WFM_UMAC_MGMT_REASSOC_RSP	0x03
#define WFM_UMAC_MGMT_PROB_REQ		0x04
#define WFM_UMAC_MGMT_PROB_RSP		0x05
#define WFM_UMAC_MGMT_BEACON		0x08
#define WFM_UMAC_MGMT_ATIM		0x09
#define WFM_UMAC_MGMT_DISASSOC		0x0A
#define WFM_UMAC_MGMT_AUTH		0x0B
#define WFM_UMAC_MGMT_DEAUTH		0x0C
#define WFM_UMAC_MGMT_ACTION		0x0D

/*Data frame sub-types*/
#define WFM_UMAC_SUB_DATA			0x00
#define WFM_UMAC_SUB_DATA_CF_ACK		0x01
#define WFM_UMAC_SUB_DATA_CF_POLL		0x02
#define WFM_UMAC_SUB_DATA_CF_ACK_CF_POLL	0x03
#define WFM_UMAC_SUB_DATA_NULL			0x04
#define WFM_UMAC_SUB_NULL_CF_ACK		0x05
#define WFM_UMAC_SUB_NULL_CF_POLL		0x06
#define WFM_UMAC_SUB_NULL_CF_ACK_CF_POLL	0x07
#define WFM_UMAC_SUB_DATA_QOS			0x08
#define WFM_UMAC_SUB_DATA_QOS_CF_ACK		0x09
#define WFM_UMAC_SUB_DATA_QOS_CF_POLL		0x0A
#define WFM_UMAC_SUB_DATA_QOS_CF_ACK_CF_POLL	0x0B
#define WFM_UMAC_SUB_NULL_QOS			0x0C
#define WFM_UMAC_SUB_NULL_QOS_CF_POLL		0x0E
#define WFM_UMAC_SUB_NULL_QOS_CF_ACK_CF_POLL	0x0F

/* Action Frames Category Codes */
#if DOT11K_SUPPORT
#define UMAC_CATEGORY_RADIO_MEASUREMENT		5
#endif	/* DOT11K_SUPPORT */
#define UMAC_CATEGORY_FT_AUTH			6
#define UMAC_CATEGORY_SA_QUERY			8

#if DOT11K_SUPPORT
/*Action Type that comes in measurement Req frame*/
#define UMAC_ACTION_TYPE_RADIO_MEASUREMENT_REQ		0
#define UMAC_ACTION_TYPE_RADIO_MEASUREMENT_REPORT	1
#define UMAC_ACTION_TYPE_LINK_MEASUREMENT_REQ		2
#define UMAC_ACTION_TYPE_LINK_MEASUREMENT_REPORT	3
#define UMAC_ACTION_TYPE_NEIGHBOR_REPORT_REQ		4
#define UMAC_ACTION_TYPE_NEIGHBOR_REPORT_RESP		5
#endif	/* DOT11K_SUPPORT */

/*Action types that comes in Fast Roaming*/
#define UMAC_ACTION_TYPE_FT_AUTH_RESP			2

/*Action types that comes in Managment Frame Protection*/
#define UMAC_ACTION_TYPE_SA_QUERY_REQ			0
#define UMAC_ACTION_TYPE_SA_QUERY_RESP			1

#if DOT11K_SUPPORT
/*Measurement Report Mode*/
#define MEAS_REPORT_MODE_LATE				(1<<0)
#define MEAS_REPORT_MODE_INCAPABLE			(1<<1)
#define MEAS_REPORT_MODE_REFUSED			(1<<2)

/*Mask Definition */
#define UMAC_DURATION_MANDATORY_MASK			(1<<4)

#endif	/* DOT11K_SUPPORT */

/*
  Sub Types for Control Frames
*/

#define WFM_UMAC_CNTRL_BLK_ACK_REQ	0x08
#define WFM_UMAC_CNTRL_BLK_ACK		0x09
#define WFM_UMAC_CNTRL_PS_POLL		0x0A
#define WFM_UMAC_CNTRL_RTS		0x0B
#define WFM_UMAC_CNTRL_CTS		0x0C
#define WFM_UMAC_CNTRL_ACK		0x0D
#define WFM_UMAC_CNTRL_CF_END		0x0E
#define WFM_UMAC_CNTRL_CF_END_CF_ACK	0x0F

/* ToDs field in frame control */
#define WFM_UMAC_TO_DS_FRAME		1
#define WFM_UMAC_NOT_TO_DS_FRAME	0

/* Protected frame field in frame control */
#define WFM_UMAC_PROTECTED_FRAME	1
#define WFM_UMAC_NOT_PROTECTED_FRAME	0

/*
  Rates Encoded as given in the Table in Section 10.4.4.2
  of IEEE-802.11 2007 Spec
*/
#define WFM_802_11_DSSS_RATE_1_MBPS	0x02
#define WFM_802_11_DSSS_RATE_2_MBPS	0x04
#define WFM_802_11_DSSS_RATE_5_5_MBPS	0x0B
#define WFM_802_11_DSSS_RATE_11_MBPS	0x16

#define WFM_802_11_PBCC_RATE_22_MBPS	0x2C	/* not supported by wbf */
#define WFM_802_11_PBCC_RATE_33_MBPS	0x42	/* not supported by wbf */

#define WFM_802_11_OFDM_RATE_6_MBPS	0x0C
#define WFM_802_11_OFDM_RATE_9_MBPS	0x12
#define WFM_802_11_OFDM_RATE_12_MBPS	0x18
#define WFM_802_11_OFDM_RATE_18_MBPS	0x24
#define WFM_802_11_OFDM_RATE_24_MBPS	0x30
#define WFM_802_11_OFDM_RATE_36_MBPS	0x48
#define WFM_802_11_OFDM_RATE_48_MBPS	0x60
#define WFM_802_11_OFDM_RATE_54_MBPS	0x6C

#define WFM_802_11_HT_RATE_6_5_MBPS	0x0D
#define WFM_802_11_HT_RATE_13_MBPS	0x1A
#define WFM_802_11_HT_RATE_19_5_MBPS	0x27
#define WFM_802_11_HT_RATE_26_MBPS	0x34
#define WFM_802_11_HT_RATE_39_MBPS	0x4E
#define WFM_802_11_HT_RATE_52_MBPS	0x68
#define WFM_802_11_HT_RATE_58_5_MBPS	0x75
#define WFM_802_11_HT_RATE_65_MBPS	0x82

#define RATE_DIFFERENCE			10

#define WFM_CONVERT_CHANNEL_NO_TO_FREQ_KHZ(ChannelNo) ((2407 + (ChannelNo * 5))*1000)

#define WFM_UMAC_80211_FORMAT_SUPPORTED_RATESET_ABG	\
	{						\
		(WFM_802_11_DSSS_RATE_1_MBPS),		\
		(WFM_802_11_DSSS_RATE_2_MBPS),		\
		(WFM_802_11_DSSS_RATE_5_5_MBPS),	\
		(WFM_802_11_DSSS_RATE_11_MBPS),		\
		(WFM_802_11_PBCC_RATE_22_MBPS),		\
		(WFM_802_11_PBCC_RATE_33_MBPS),		\
		(WFM_802_11_OFDM_RATE_6_MBPS),		\
		(WFM_802_11_OFDM_RATE_9_MBPS),		\
		(WFM_802_11_OFDM_RATE_12_MBPS),		\
		(WFM_802_11_OFDM_RATE_18_MBPS),		\
		(WFM_802_11_OFDM_RATE_24_MBPS),		\
		(WFM_802_11_OFDM_RATE_36_MBPS),		\
		(WFM_802_11_OFDM_RATE_48_MBPS),		\
		(WFM_802_11_OFDM_RATE_54_MBPS)		\
	}

#define WFM_UMAC_80211_FORMAT_SUPPORTED_RATESET_ABGN	\
	{						\
		(WFM_802_11_DSSS_RATE_1_MBPS),		\
		(WFM_802_11_DSSS_RATE_2_MBPS),		\
		(WFM_802_11_DSSS_RATE_5_5_MBPS),	\
		(WFM_802_11_DSSS_RATE_11_MBPS),		\
		(WFM_802_11_PBCC_RATE_22_MBPS),		\
		(WFM_802_11_PBCC_RATE_33_MBPS),		\
		(WFM_802_11_OFDM_RATE_6_MBPS),		\
		(WFM_802_11_OFDM_RATE_9_MBPS),		\
		(WFM_802_11_OFDM_RATE_12_MBPS),		\
		(WFM_802_11_OFDM_RATE_18_MBPS),		\
		(WFM_802_11_OFDM_RATE_24_MBPS),		\
		(WFM_802_11_OFDM_RATE_36_MBPS),		\
		(WFM_802_11_OFDM_RATE_48_MBPS),		\
		(WFM_802_11_OFDM_RATE_54_MBPS),		\
		(WFM_802_11_HT_RATE_6_5_MBPS),		\
		(WFM_802_11_HT_RATE_13_MBPS),		\
		(WFM_802_11_HT_RATE_19_5_MBPS),		\
		(WFM_802_11_HT_RATE_26_MBPS),		\
		(WFM_802_11_HT_RATE_39_MBPS),		\
		(WFM_802_11_HT_RATE_52_MBPS),		\
		(WFM_802_11_HT_RATE_58_5_MBPS),		\
		(WFM_802_11_HT_RATE_65_MBPS)		\
	}

#define BASIC_RATE(rate)  ((rate) | 0x80)

/******************************************************************************
			  REASON CODES
******************************************************************************/

/*
  Following section defines the different reason codes to be used with
  management frames of type Disassociation,DeAuthentication,DELTS,DELBA
  or DLS.

  Refer section 7.3.1.7 In IEEE Standard for 802.11 - 2007
*/

/*Unspecified reason*/
#define UMAC_REASON_CODE_1 1

/*Previous authentication no longer valid*/
#define UMAC_REASON_CODE_2 2

/*
  Deauthenticated because sending STA is leaving (or has left) IBSS or ESS
*/
#define UMAC_REASON_CODE_3 3

/*4 Disassociated due to inactivity*/
#define UMAC_REASON_CODE_4 4

/* 5 Disassociated because AP is unable to handle all currently
   associated STAs
*/
#define UMAC_REASON_CODE_5 5

/*6 Class 2 frame received from nonauthenticated STA*/
#define UMAC_REASON_CODE_6 6

/*7 Class 3 frame received from nonassociated STA*/
#define UMAC_REASON_CODE_7 7

/* 8 Disassociated because sending STA is leaving (or
   has left) BSS
*/
#define UMAC_REASON_CODE_8 8

/*  9 STA requesting (re)association is not authenticated
    with responding STA
*/
#define UMAC_REASON_CODE_9 9
/*  10 Disassociated because the information in the Power
    Capability element is unacceptable
*/
#define UMAC_REASON_CODE_10 10

/*    11 Disassociated because the information in the
      Supported Channels element is unacceptable
*/
#define UMAC_REASON_CODE_11 11

/*  13  Invalid information element, i.e., an information
    element defined in this standard for
    which the content does not meet the specifications
    in Clause 7
*/
#define UMAC_REASON_CODE_13 13

/*14 Message integrity code (MIC) failure*/
#define UMAC_REASON_CODE_14 14

/*15 4-Way Handshake timeout*/
#define UMAC_REASON_CODE_15 15

/*16 Group Key Handshake timeout*/
#define UMAC_REASON_CODE_16 16

/*  17 Information element in 4-Way Handshake different from
    (Re)Association Request/Probe
    Response/Beacon frame
*/
#define UMAC_REASON_CODE_17 17

/*18 Invalid group cipher*/
#define UMAC_REASON_CODE_18 18

/*19 Invalid pairwise cipher*/
#define UMAC_REASON_CODE_19 19

/*20 Invalid AKMP*/
#define UMAC_REASON_CODE_20 20

/*21 Unsupported RSN information element version*/
#define UMAC_REASON_CODE_21 21

/*22 Invalid RSN information element capabilities*/
#define UMAC_REASON_CODE_22 22

/*23 IEEE 802.1X authentication failed*/
#define UMAC_REASON_CODE_23 23

/*24 Cipher suite rejected because of the security policy*/
#define UMAC_REASON_CODE_24 24

/*32 Disassociated for unspecified, QoS-related reason*/
#define UMAC_REASON_CODE_32 32

/*33 Disassociated because QoS AP lacks sufficient bandwidth for this QoS STA*/
#define UMAC_REASON_CODE_33 33

/*  34 Disassociated because excessive number of frames need to be
    acknowledged, but are not acknowledged due to AP transmissions
    and/or poor channel conditions
*/

#define UMAC_REASON_CODE_34 34

/*  35 Disassociated because STA is transmitting outside the
    limits of its TXOPs
*/
#define UMAC_REASON_CODE_35 35

/*36 Requested from peer STA as the STA is leaving the BSS (or resetting)*/
#define UMAC_REASON_CODE_36 36

/*37 Requested from peer STA as it does not want to use the mechanism*/
#define UMAC_REASON_CODE_37 37

/*  38 Requested from peer STA as the STA received frames using the
    mechanism for which a setup is required
*/
#define UMAC_REASON_CODE_38 38

/*39 Requested from peer STA due to timeout*/
#define UMAC_REASON_CODE_39 39

/*45 Peer STA does not support the requested cipher suite*/
#define UMAC_REASON_CODE_45 45

/******************************************************************************
	      STATUS CODES
******************************************************************************/

#define UMAC_STATUS_CODE_SUCCESS 0
#define UMAC_STATUS_CODE_FAILURE 1

/*0  : Successful*/
#define UMAC_STATUS_CODE_0  0

/*1  : Unspecified failure*/
#define UMAC_STATUS_CODE_1  1

/*
  Cannot support all requested capabilities in the Capability
  Information field

*/
#define UMAC_STATUS_CODE_10	10

/*
  Reassociation denied due to inability to confirm that
  association exists
*/

#define UMAC_STATUS_CODE_11	11

/*
  Association denied due to reason outside the scope of this standard
*/
#define UMAC_STATUS_CODE_12	12

/*
  Responding STA does not support the specified authentication algorithm
*/
#define UMAC_STATUS_CODE_13	13
/*
  Received an Authentication frame with authentication transaction sequence
  number out of expected sequence
*/

#define UMAC_STATUS_CODE_14	14

/*
  Authentication rejected because of challenge failure
*/
#define UMAC_STATUS_CODE_15	15

/*
  Authentication rejected due to timeout waiting for next frame in
  sequence
*/
#define UMAC_STATUS_CODE_16	16
/*
  Association denied because AP is unable to handle additional associated STAs
*/
#define UMAC_STATUS_CODE_17	17

/*
  Association denied due to requesting STA not supporting all of the data
  rates in the BSSBasicRateSet parameter
*/
#define UMAC_STATUS_CODE_18	18

/*
  Association denied due to requesting STA not supporting the short
  preamble option
*/
#define UMAC_STATUS_CODE_19	19

/*
  Association denied due to requesting STA not supporting the PBCC
  modulation option
*/
#define UMAC_STATUS_CODE_20	20

/*
  Association denied due to requesting STA not supporting the Channel
  Agility option
*/
#define UMAC_STATUS_CODE_21	21

/*
  Association request rejected because Spectrum Management
  capability is required
*/
#define UMAC_STATUS_CODE_22	22

/*
  Association request rejected because the information in
  the Power Capability element is unacceptable
*/
#define UMAC_STATUS_CODE_23	23

/*
  Association request rejected because the information in the
  Supported Channels element is unacceptable
*/
#define UMAC_STATUS_CODE_24	24

/*
  Association denied due to requesting STA not supporting the
  Short Slot Time option
*/
#define UMAC_STATUS_CODE_25	25

/*
  Association denied due to requesting STA not supporting
  the DSSS-OFDM option
*/
#define UMAC_STATUS_CODE_26	26

/*30 : Association request rejected temporarily; try again later*/
#define UMAC_STATUS_CODE_30	30

/******************************************************************************
	      INFORMATION ELEMENTS ID LIST
******************************************************************************/

/*
  Information elements Element Id definitions

*/

#define UMAC_IE_ELEMENT_SSID			0
#define UMAC_IE_ELEMENT_SUPPORTED_RATES		1
#define UMAC_IE_ELEMENT_DS_PARAM		3
#define UMAC_IE_ELEMENT_TIM			5
#define UMAC_IE_ELEMENT_IBSS_PARAM_SET		6
#define UMAC_IE_ELEMENT_COUNTRY			7
#define UMAC_IE_ELEMENT_EDCA_PARAMETERS		12
#define UMAC_IE_ELEMENT_CHALLENGE_TEXT		16
#define UMAC_IE_ELEMENT_POWER_CAPABILITY	33
#define UMAC_IE_ELEMENT_SUPPORTED_CHANNELS	36
#define UMAC_IE_ELEMENT_CHANNEL_SWITCH		37
#define UMAC_IE_ELEMENT_ERP_INFORMATION		42
#define UMAC_IE_ELEMENT_HT_CAPABILITIES		45
#define UMAC_IE_ELEMENT_QOS_CAPABILITY		46
#define UMAC_IE_ELEMENT_RSN			48
#define UMAC_IE_ELEMENT_EXTENDED_SUPPORTED_RATES	50
#define UMAC_IE_ELEMENT_MOBILITY_DOMAIN		54
#define UMAC_IE_ELEMENT_FAST_BSS_TRANSITION	55
#define UMAC_IE_ELEMENT_TIMEOUT			56
#define UMAC_IE_ELEMENT_RIC_DATA		57
#define UMAC_IE_ELEMENT_HT_OPERATION		61
#if WAPI_SUPPORT
#define UMAC_IE_ELEMENT_WAPI			68
#endif	/* WAPI_SUPPORT */

#if DOT11K_SUPPORT
#define UMAC_IE_TPC_REPORT			35
#define UMAC_IE_ELEMENT_MEASUREMENT_REQ		38
#define UMAC_IE_ELEMENT_MEASUREMENT_REPORT	39
#define UMAC_IE_ELEMENT_NEIGHBOR_REPORT		52
#define UMAC_IE_ELEMENT_RRM_ENABLED_CAPABILITIES	70
#define UMAC_IE_ELEMENT_POWER_CAPABILITY	33
#endif	/* DOT11K_SUPPORT */

#define UMAC_IE_ELEMENT_VENDOR_SPECIFIC		221

/*
  802.11 defines that there can be only 8 rates in supported rates. rest
  have to be in extended rates
*/
#define WFM_MAX_ELEMENTS_IN_SUPPORTED_RATES	8

/* 802.11 defines that there can be 255 rates in extended supported rates */
#define WFM_MAX_ELEMENTS_IN_EXTENDED_SUPPORTED_RATES 255

#if P2P_MINIAP_SUPPORT
#define UMAC_QOS_INFO_STA_UAPSD_MASK		0x0F
#define UMAC_QOS_INFO_STA_UAPSD_NONE		0x00
#define UMAC_QOS_INFO_STA_UAPSD_AC_BK		0x04
#define UMAC_QOS_INFO_STA_UAPSD_AC_BE		0x08
#define UMAC_QOS_INFO_STA_UAPSD_AC_VI		0x02
#define UMAC_QOS_INFO_STA_UAPSD_AC_VO		0x01
#define UMAC_QOS_INFO_STA_UAPSD_SP_LEN		0x60
#define UMAC_QOS_INFO_AP_EOSP			0x0010
#define UMAC_FC_PWR_MGMT			0x1000
#define UMAC_FC_MORE_DATA			0x2000
#define UMAC_FC_PRIVACY				0x4000
#define UMAC_ROLE_P2P_CLIENT			0
#define UMAC_ROLE_P2P_GO			1
#define UMAC_P2P_PERSISTENT_GROUP		0x01
#define UMAC_P2P_IE_ELEMENT_ID			0xDD
#define UMAC_PUBLIC_ACTN_FRM_CTGRY		0x04
#define UMAC_GENERAL_ACTN_FRM_CTGRY		0x7F
#define UMAC_P2P_OUI_TYPE			0x09
#define UMAC_P2P_OUI				{0x50, 0x6F, 0x9A}

#define D11_P2P_ACTION_TYPE_GO_NEG_REQ			0
#define D11_P2P_ACTION_TYPE_GO_NEG_RESP			1
#define D11_P2P_ACTION_TYPE_GO_NEG_CNF			2
#define D11_P2P_ACTION_TYPE_P2P_INVTN_REQ		3
#define D11_P2P_ACTION_TYPE_P2P_INVTN_RESP		4
#define D11_P2P_ACTION_TYPE_DEV_DISCVRBLTY_REQ		5
#define D11_P2P_ACTION_TYPE_DEV_DISCVRBLTY_RESP		6
#define D11_P2P_ACTION_TYPE_PROVSN_DISCOVER_REQ		7
#define D11_P2P_ACTION_TYPE_PROVSN_DISCOVER_RESP	8

/*
    Attribute Id definitions for P2P IE
*/
#define UMAC_P2P_IE_ATTRIB_STATUS			0
#define UMAC_P2P_IE_ATTRIB_M_REASON_CODE		1
#define UMAC_P2P_IE_ATTRIB_CAPABILITY			2
#define UMAC_P2P_IE_ATTRIB_DEVICE_ID			3
#define UMAC_P2P_IE_ATTRIB_GO_INTENT			4
#define UMAC_P2P_IE_ATTRIB_CONF_TIMEOUT			5
#define UMAC_P2P_IE_ATTRIB_LISTEN_CHANNEL		6
#define UMAC_P2P_IE_ATTRIB_GROUP_BSSID			7
#define UMAC_P2P_IE_ATTRIB_EXT_LISTEN_TIME		8
#define UMAC_P2P_IE_ATTRIB_INTND_IF_ADDR		9
#define UMAC_P2P_IE_ATTRIB_MANAGEABILITY		10
#define UMAC_P2P_IE_ATTRIB_CHANNEL_LIST			11
#define UMAC_P2P_IE_ATTRIB_NOA				12
#define UMAC_P2P_IE_ATTRIB_DEVICE_INFO			13
#define UMAC_P2P_IE_ATTRIB_GROUP_INFO			14
#define UMAC_P2P_IE_ATTRIB_GROUP_ID			15
#define UMAC_P2P_IE_ATTRIB_INTERFACE			16
#define UMAC_P2P_IE_ATTRIB_OP_CHANNEL			17
#define UMAC_P2P_IE_ATTRIB_INVTN_FLAGS			18

#define UMAC_P2P_IE_ATTRIB_LENGTH			2
#define UMAC_P2P_ATTRIB_STATUS_LENGTH			1
#define UMAC_P2P_ATTRIB_GO_INTENT_LENGTH		1
#define UMAC_P2P_ATTRIB_INTD_IF_ADDR_LENGTH		6
#define UMAC_P2P_ATTRIB_CAPABILITY_LENGTH		2
#define UMAC_P2P_ATTRIB_CHANNEL_LENGTH			2
#define UMAC_P2P_ATTRIB_CONF_TO_LENGTH			2
#define UMAC_P2P_ATTRIB_CONFIG_METHOD_LENGTH		2
#define UMAC_P2P_ATTRIB_PRIM_DEV_TYP_LENGTH		8
#define UMAC_P2P_ATTRIB_SEC_DEV_TYP_LENGTH		8

#define UMAC_P2P_ATTRIB_INVTN_FLAG_LENGTH		1

#define UMAC_P2P_TIE_BREAKER				0x01
#define UMAC_P2P_GO_INTENT				0xFE

#define UMAC_WSC_VERSION				0x10
/* IE Types */
#define UMAC_WSC_CONFIG_METHODS				0x1008
#define UMAC_WSC_DEV_NAME				0x1011
#define UMAC_WSC_DEV_PASSWD				0x1012
#define UMAC_WSC_VER					0x104A
#define UMAC_WSC_PRIM_DEV_TYPE				0x1054
#define UMAC_WSC_SEC_DEV_TYPE_LIST			0x1055
#define UMAC_WSC_PRIM_DEV_CAT_ID			10
#define UMAC_WSC_PRIM_DEV_SUB_CAT_ID			2
#define UMAC_WSC_VER_IE_LENGTH				1
#define UMAC_WSC_DEV_PASSWD_IE_LENGTH			2
#define UMAC_WSC_PRIM_DEV_TYP_IE_LENGTH			8
#define UMAC_WSC_SEC_DEV_TYP_IE_LENGTH			128
#define UMAC_WSC_CONFIG_METHOD_IE_LENGTH		2
#define UMAC_WSC_DEV_NAME_IE_LENGTH			32

#define UMAC_REG_CLASS_FCC_2G				12
#define UMAC_REG_CLASS_CEPT_2G				4
#define UMAC_REG_CLASS_MPHPT_2G_1_13			30
#define UMAC_REG_CLASS_MPHPT_2G_14			31

#define FCC_2G_CHANNELS		{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11}

#define CEPT_2G_CHANNELS	{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}

#define MPHPT_2G_1_13_CHANNELS	{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}

#define MPHPT_2G_14_CHANNELS	{14}

#endif	/* P2P_MINIAP_SUPPORT */

#define WMM_OUI		{0x00, 0x50, 0xF2}
#define P2P_OUI		{0x50, 0x6F, 0x9A}

#define WMM_IE_ELEMENT_TYPE	0x02

#define WMM_PARAM_ELT_LENGTH	24
#define WMM_PARAM_ELT_VER	1

/*WMM Sub-Types*/
#define WFM_IE_INFORMATION_ELEMENT_SUB_TYPE	0x00
#define WFM_IE_PARAMETER_ELEMENT_SUB_TYPE	0x01
#define WFM_IE_TSPEC_ELEMENT_SUB_TYPE		0x02

#define UMAC_OPEN_SYSTEM_AUTH			0
#define UMAC_SHARED_KEY_AUTH			1
#define UMAC_FT_AUTH				2

#define WPA_OUI					{0x00, 0x50, 0xF2}

/*WPA Sub-Types*/
#define WPA_IE_PARAMETER_ELEMENT_SUB_TYPE	0x01

#define UMAC_CONVERT_CHANNEL_2_FREQ(n) (((2407 + 5*n))*1000)

/* ERP Information */
#define WFM_UMAC_ERP_IE_NON_ERP_PRESENT		(1<<0)	/* 1 */
#define WFM_UMAC_ERP_IE_USE_PROTECTION		(1<<1)	/* 2 */
#define WFM_UMAC_ERP_IE_BARKER_PREAMBLE_MODE	(1<<2)	/* 4 */

/* Return status of the functions WFM_UMAC_ProcessBeaconOrProbRspFrame()
   WFM_UMAC_ProcInternalScanRsp()
*/
#define WFM_BEACON_PROBRSP_LIST_UPDATED			0
#define WFM_BEACON_PROBRSP_ERROR			1
#define WFM_BEACON_PROBRSP_INTERNAL_SCAN_ONGOING	2
#define WFM_BEACON_PROBRSP_INTERNAL_SCAN_GOT_RESULT	3

#define MAXIMUM_NUMBER_OF_NEIGHBORING_BSS		8

#define UMAC_SC_REQ_INTERNAL				0
#define UMAC_SC_REQ_EXTERNAL				1

typedef struct WFM_BEACON_CAPABILITIY_S {
	uint16 ESS : 1;			/* Bit 00 */
	uint16 IBSS : 1;		/* Bit 01 */
	uint16 CF_Pollable : 1;		/* Bit 02 */
	uint16 CF_PollRequest : 1;	/* Bit 03 */
	uint16 Privacy : 1;		/* Bit 04 */
	uint16 ShortPreamble : 1;	/* Bit 05 */
	uint16 PBCC : 1;		/* Bit 06 */
	uint16 ChannelAgility : 1;	/* Bit 07 */
	uint16 SpectrumMgmt : 1;	/* Bit 08 */
	uint16 QoS : 1;			/* Bit 09 */
	uint16 ShortSlotTime : 1;	/* Bit 10 */
	uint16 APSD : 1;		/* Bit 11 */
	uint16 Reserved : 1;		/* Bit 12 */
	uint16 DSSS_OFDM : 1;		/* Bit 13 */
	uint16 DelayedBlockAck : 1;	/* Bit 14 */
	uint16 ImmdediateBlockAck : 1;	/* Bit 15 */
} WFM_BEACON_CAPABILITY;

/*
  This data structure defines the 802.11 Frame header.
*/

/*All structures are properly packed (need pack 1) */
typedef struct WFM_UMAC_FRAME_HDR_S {
	uint16 FrameControl;
	uint16 Duration_Id;
	uint8 Address1[WFM_UMAC_MAC_ADDRESS_SIZE];	/* DA */
	uint8 Address2[WFM_UMAC_MAC_ADDRESS_SIZE];	/* SA */
	uint8 Address3[WFM_UMAC_MAC_ADDRESS_SIZE];	/* BSSID */
	uint16 SeqCntrl;
} WFM_UMAC_FRAME_HDR;

typedef struct WFM_UMAC_AMSDU_SUBFRM_HDR_S {
	uint8 DestAddr[6];
	uint8 SrcAddr[6];
	uint16 Length;
} WFM_UMAC_AMSDU_SUBFRM_HDR;

/*
  802.11 data frame
  Note: In ourcase ToDS and FromDS wont be 1 together as we are not the AP.
  so comenting the Address4 Field.
*/

typedef struct WFM_UMAC_DATA_FRAME_S {
	WFM_UMAC_FRAME_HDR dot11hdr;
	uint8 FrameBody[WFM_UMAC_MAX_FRAME_BODY_SIZE];
} WFM_UMAC_DATA_FRAME;

typedef struct WFM_UMAC_QOS_DATA_FRAME_S {
	WFM_UMAC_FRAME_HDR dot11hdr;	/* 24 bytes - 4 byte aligned */
	uint16 QoSCntrl;
	uint8 FrameBody[WFM_UMAC_MAX_FRAME_BODY_SIZE];
} WFM_UMAC_QOS_DATA_FRAME;

/*
  802.11 Management Frame
*/
typedef struct WFM_UMAC_MGMT_FRAME_S {
	WFM_UMAC_FRAME_HDR dot11hdr;
	uint8 FrameBody[WFM_UMAC_MAX_FRAME_BODY_SIZE];
} WFM_UMAC_MGMT_FRAME;

/*
  802.11 Ps Poll Control Frame
*/
typedef struct WFM_UMAC_CTRL_PS_POLL_FRAME_S {
	uint16 FrameControl;
	uint16 Aid;
	uint8 RxAddr[WFM_UMAC_MAC_ADDRESS_SIZE];	/* BSSID (RA) */
	uint8 TrAddr[WFM_UMAC_MAC_ADDRESS_SIZE];	/* TA */
} WFM_UMAC_CTRL_PS_POLL_FRAME;

/******************************************************************************
	  802.11 MANAGEMENT FRAME BODY DEFENITIONS
******************************************************************************/

typedef struct WFM_UMAC_DISASSOC_REQ_BODY_S {
	uint16 ReasonCode;
	uint16 padding;
} WFM_UMAC_DISASSOC_REQ_BODY;

typedef struct WFM_UMAC_ASSOC_REQ_BODY_S {
	uint16 Capability;
	uint16 ListenInterval;
	/*
	   Required for Reassoc only
	   ** AP Mac Address (for reassoc only) **
	   Following information elements follow in the specified order
	   1] SSID
	   2] Supported Rates
	   3] Extended Supported Rates
	   4] Power Capability
	   5] Supported Channels
	   6] RSN
	   7] Qos Capability
	 */
} WFM_UMAC_ASSOC_REQ_BODY;

typedef struct WFM_UMAC_ASSOC_RSP_BODY_S {
	uint16 Capability;
	uint16 StatusCode;
	uint16 AID;
	uint8 IeData[1];
	/* two MSB bits are always 1 */
	/*
	   Following IE elements follows in the order
	   1] Supported Rates
	   2] Extended supported Rates
	   3] ECDA Param Set
	 */
} WFM_UMAC_ASSOC_RSP_BODY;

typedef struct WFM_UMAC_AUTH_FRAME_BODY_S {
	uint16 AuthAlgorithm;
	uint16 TransactionSeqNumber;
	uint16 StatusCode;
	uint8 IEs[2];
} WFM_UMAC_AUTH_FRAME_BODY;

typedef struct WFM_UMAC_DEAUTH_FRAME_BODY_S {
	uint16 ReasonCode;
	uint16 padding;
} WFM_UMAC_DEAUTH_FRAME_BODY;

typedef struct WFM_UMAC_BEACON_FRAME_BODY_S {
	uint64 TimeStamp;
	uint16 BeaconInterval;
	uint16 Capability;
	uint8 IEElements[4];
} WFM_UMAC_BEACON_FRAME_BODY;

#if FT_SUPPORT
typedef struct WFM_UMAC_FT_REQ_ACTION_BODY_S {
	uint8 Category;
	uint8 Action;
	uint8 StaMac[6];
	uint8 TargetAPMac[6];
	uint8 IEs[2];
} WFM_UMAC_FT_REQ_ACTION_BODY;

typedef struct WFM_UMAC_FT_RESP_ACTION_BODY_S {
	uint8 Category;
	uint8 Action;
	uint8 StaMac[6];
	uint8 TargetAPMac[6];
	uint16 Status;
	uint8 IEs[4];
} WFM_UMAC_FT_RESP_ACTION_BODY;

#endif	/* FT_SUPPORT */

typedef struct UMAC_SA_QUERY_FRAME_BODY_S {
	uint8 Category;
	uint8 Action;
	uint16 TransactionId;
} UMAC_SA_QUERY_FRAME_BODY;

typedef struct WFM_OUI_INFO_S {
	uint8 Oui[3];
	uint8 OuiType;
	uint8 OuiSubType;
} WFM_OUI_INFO;

typedef struct WFM_UMAC_ACTION_FRAME_BODY_S {
	uint8 category;
	uint8 actionField;
	uint8 Reserved[2];
} WFM_UMAC_ACTION_FRAME_BODY;

#if DOT11K_SUPPORT
/*Radion Measurement Request Data structures*/
typedef struct CHANNEL_LOAD_REQ_S {
	uint8 regulatoryClass;
	uint8 channelNum;
	uint16 randomizationInterval;
	uint16 measurementDuration;
	uint16  Reserved;
} CHANNEL_LOAD_REQ;

typedef struct NOISE_HISTOGRAM_REQ_S {
	uint8 regulatoryClass;
	uint8 channelNum;
	uint16 randomizationInterval;
	uint16 measurementDuration;
	uint16 Reserved;
} NOISE_HISTOGRAM_REQ;

typedef struct BEACON_REQ_S {
	uint8 regulatoryClass;
	uint8 channelNum;
	uint16 randomizationInterval;
	uint16 measurementDuration;
	uint8 measurementMode;
	uint8 bssid[6];
	uint8 SubElID;
	uint8 SubElLen;
	uint8 SubEldata[1];
} BEACON_REQ;

typedef struct FRAME_REQ_S {
	uint8 regulatoryClass;
	uint8 channelNum;
	uint16 randomizationInterval;
	uint16 measurementDuration;
	uint8 frameReqType;
	uint8 macAddr[6];
	uint8 Reserved[3];
} FRAME_REQ;

typedef struct STA_STATISTICS_REQ_S {
	uint8 peerMacAddr[6];
	uint16 randomizationInterval;
	uint16 measurementDuration;
	uint8 groupIdentity;
	uint8 Reserved;
} STA_STATISTICS_REQ;

typedef struct LCI_INFORMATION_REQ_S {
	uint8 locSubject;
	uint8 latitudeRequestedResolution;
	uint8 longitudeRequestedResolution;
	uint8 altitudeRequestedResolution;
} LCI_INFORMATION_REQ;

typedef struct TRANSMIT_STREAM_CATEGORY_MEASUREMENT_REQ_S {
	uint16 randomizationInterval;
	uint16 measurementDuration;
	uint8 peerSTAAddress[6];
	uint8 trafficIdentifier;
	uint8 binRange;
} TRANSMIT_STREAM_CATEGORY_MEASUREMENT_REQ;

typedef struct LINK_MEASUREMENT_REQUEST_S {
	uint8 category;
	uint8 action;
	uint8 dialogToken;
	uint8 transmitPowerUsed;
	uint8 MaxTxPower;
	uint8 Reserved[3];
} LINK_MEASUREMENT_REQUEST;

typedef struct NEIGHBOR_REPORT_REQ_S {
	uint8 category;
	uint8 action;
	uint8 dialogToken;
	uint8 OpSubElt[1];
} NEIGHBOR_REPORT_REQ;

typedef union UMAC_MEASUREMENT_ELEMENTS_REQ_S {
	CHANNEL_LOAD_REQ channelLoadReq;
	NOISE_HISTOGRAM_REQ noiseHistogramReq;
	BEACON_REQ beaconReq;
	FRAME_REQ frameReq;
	STA_STATISTICS_REQ staStatisticsReq;
	LCI_INFORMATION_REQ lciInformationReq;
	TRANSMIT_STREAM_CATEGORY_MEASUREMENT_REQ transmitStreamReq;
} UMAC_MEASUREMENT_ELEMENTS_REQ;

typedef struct MEASUREMENT_REQUEST_ELEMENT_IE_S {
	uint8 measurementToken;
	uint8 measurementRequestMode;
	uint8 measurementType;
	/*
	  This variable is used for Enable bit 1,Incapable and Refused.
	  So request is sent only once.
	*/
	uint8 reqProcessedOnce;
	uint8 measurementElementReq[256];
} MEASUREMENT_REQUEST_ELEMENT_IE;
/*Radio Measurement Request Data structures end here*/

typedef struct UMAC_RADIO_MEAS_REPORT_BODY_S {
	uint8 category;
	uint8 action;
	uint8 dialogToken;
	/*Measurement Report Element */
	uint8 ElementID;
	uint8 length;
	uint8 measurementToken;
	uint8 measurementReportMode;
	uint8 measurementType;
	/*Measurement Report */
} UMAC_RADIO_MEAS_REPORT_BODY;

typedef struct UMAC_LINK_MEAS_REPORT_BODY_S {
	uint8 category;
	uint8 action;
	uint8 dialogToken;
	uint8 reserved;
} UMAC_LINK_MEAS_REPORT_BODY;

typedef struct UMAC_FRAME_REPORT_ENTRY_S {
	uint32 rcpi;
	uint16 FrameCount;
	uint8 lastRsni;
	uint8 lastRcpi;
	uint8 averageRcpi;
	uint8 lastAverageRcpi;
	uint8 Reserved[2];
} UMAC_FRAME_REPORT_ENTRY;

typedef struct TANSMIT_STREAM_RESULTS_S {
	uint8 actualMeasStartTime[8];
	uint32 txMsduCount;
	uint32 msduDiscardedCount;
	uint32 msduFailedCount;
	uint32 msduMultipleRetryCount;
	uint64 queueDelay;
	uint64 transmitDelay;
	uint32 bin0;
	uint32 bin1;
	uint32 bin2;
	uint32 bin3;
	uint32 bin4;
	uint32 bin5;
	uint32 CurrentTime[64];
	uint32 totalPackets;
	uint64 tsfTimer;
} TANSMIT_STREAM_RESULTS;

typedef enum REPORT_OPTIONS_E {
	NORMAL = 0,
	INCAPABLE,
	REFUSED,
	BEACON_TABLE
} REPORT_OPTIONS;

#endif	/* DOT11K_SUPPORT */

typedef struct UMAC_ACTION_FRAME_BODY_S {
	uint8 category;
	uint8 actionField;
	WFM_OUI_INFO OuiInfo;
	uint8 dialogToken;
} UMAC_ACTION_FRAME_BODY;

#if P2P_MINIAP_SUPPORT

typedef struct WFM_UMAC_PRB_RESP_FRAME_BODY_S {
	uint64 TimeStamp;
	uint16 BeaconInterval;
	uint16 Capability;
	uint8 IEElements[4];
} WFM_UMAC_PRB_RESP_FRAME_BODY;

typedef struct UMAC_TIM_IE_S {
	uint8 elementID;
	uint8 length;
	uint8 DTIMcount;
	uint8 DTIMperiod;
	uint8 bitmapCTRL;
	uint8 bitmap[3];	/* Partial virtual bitmap */
} UMAC_TIM_IE;

typedef struct UMAC_P2P_OUI_INFO_S {
	uint8 Oui[3];
	uint8 OuiType;
} UMAC_P2P_OUI_INFO;

typedef struct UMAC_SUSPEND_RESUME_TX_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Flags;
} UMAC_SUSPEND_RESUME_TX_IND;

typedef struct WFM_UMAC_ERP_IE_S {
	uint8 elementID;
	uint8 length;
	uint8 ErpInfo;
	uint8 reserved;
} WFM_UMAC_ERP_IE;

#endif	/* P2P_MINIAP_SUPPORT */

/* HT Extended Capabilities */
typedef PACKED union WFM_HT_EXTENDED_CAP_PACKED_U {
	PACKED struct {
		uint16 PCO:1;			/* b0 */
		uint16 PCOTransitionTime:2;	/* b1-b2 */
		uint16 Reserved1:5;		/* b3-b7 */
		uint16 MCSFeedback:2;		/* b8-b9 */
		uint16 HTCSupport:1;		/* b10 */
		uint16 RDResponder:1;		/* b11 */
		uint16 Reserved2:4;		/* b12-b15 */
	} GCC_PACKED s;
	uint16 All;
} GCC_PACKED WFM_HT_EXTENDED_CAP_PACKED;

/* HT Capabilities IE */
#pragma pack(push, 1)
typedef PACKED struct WFM_HT_CAPABILITIES_IE_IN_FRAME_PACKED_S {
	/* in 802.11 frame endianness (little endian) */
	uint16 CapabilitiesInfo;
	uint8 AMPDU_Params;
	WFM_HT_SUPPORTED_MCS_SET_PACKED SupportedMCSSet;
	/* in 802.11 frame endianness (little endian) */
	WFM_HT_EXTENDED_CAP_PACKED HTExtendedCapabilities;
	/* in 802.11 frame endianness (little endian) */
	uint32 TxBeamFormingCapabilities;
	uint8 ASEL_Capabilities;
} GCC_PACKED WFM_HT_CAPABILITIES_IE_IN_FRAME_PACKED;

/* HT Operation IE */
typedef PACKED struct WFM_HT_OPERATION_IE_IN_FRAME_PACKED_S {
	uint8 PrimaryChannel;				/* 1 byte */
	WFM_HT_OPERATION_IE_PARAM1_PACKED HTInfoParam1;	/* 1 byte */
	WFM_HT_OPERATION_IE_PARAM2_PACKED HTInfoParam2;	/* 2 bytes */
	WFM_HT_OPERATION_IE_PARAM3_PACKED HTInfoParam3;	/* 2 bytes */
	WFM_HT_SUPPORTED_MCS_SET_PACKED BasicMCSSet;	/* 16 bytes */
} GCC_PACKED WFM_HT_OPERATION_IE_IN_FRAME_PACKED;

typedef struct WFM_WMM_INFO_ELEMENT_IN_FRAME_S {
	uint8 OUI[3];
	uint8 OUIType;
	uint8 OUISubType;
	uint8 Version;
	uint8 QoSInfo;
} WFM_WMM_INFO_ELEMENT_IN_FRAME;
#pragma pack(pop)

typedef PACKED struct WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED_S {

	uint8 QoSInfo;
	uint8 Reserved;
	WFM_WMM_AC_PARAM_PACKED Ac_BestEffort;
	WFM_WMM_AC_PARAM_PACKED Ac_BackGround;
	WFM_WMM_AC_PARAM_PACKED Ac_Video;
	WFM_WMM_AC_PARAM_PACKED Ac_Voice;

} GCC_PACKED WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED;

typedef struct UMAC_SWITCH_CHANNEL_REQ_S {
	uint8 ChannelMode;
	uint8 ChannelSwitchCount;
	uint16 NewChannelNumber;
	/* The follwoing fields are added if mode is "Enhanced" */
	uint16  Flags;
	uint8   Band;
	uint8   NumTxSlotsReserved;
	uint32  BasicRateSet;
	uint16  DurationInKus;
	uint16  ForeignBssDtimPeriodInKus;
	uint32  ForeignBssTsfDiffLow;
	uint32  ForeignBssTsfDiffHigh;
} UMAC_SWITCH_CHANNEL_REQ;

typedef struct IeElement_S {
	uint8 Id;
	uint8 size;
	uint8 *info;
} IeElement;

#if P2P_MINIAP_SUPPORT
typedef struct P2P_IE_ELEMENT_S {
	uint8 id;
	uint16 size;
	uint8 *pInfo;
} P2P_IE_ELEMENT;
#endif	/* P2P_MINIAP_SUPPORT */

typedef struct WPS_IeElement_S {
	uint16 Id;
	uint16 size;
	uint8 *info;
} WPS_IeElement;

typedef struct HT_OPERATION_IE_S {
	uint8 WriteGreenFieldMode;
	uint8 GreenFieldMode;
	uint8 WriteErpProtection;
	uint8 ErpProtection;
	uint32 SetHtProtection;
	uint8 WriteHtProtection;
	uint8 Reserved[3];
} HT_OPERATION_IE;

typedef struct UMAC_RUNTIME_S {
	uint8 WriteUpdateJoinMode;
	uint8 PreambleType;	/*WriteUpdateJoinMode */
	uint8 UpdatePsMode;
	uint8 WriteSloteTime;
	uint32 SlotTime;	/*WriteSloteTime */
	uint8 IsBssParam;
	uint8 u8DTim;
	uint8 WriteEdcaParam;
	uint8 IsChSwitchAnnounce;
	uint8 ChSwitchMode;
	uint8 ChSwitchCount;
	uint8 NewChNum;
	uint8 BeaconWakeupPeriod;
	uint8 updateNoaAttrib;
	uint8 resv[3];
	UMAC_SWITCH_CHANNEL_REQ SwitchChReq;
	WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED EdcaParam;
	HT_OPERATION_IE HtOperationIe;
} UMAC_RUNTIME;

/* Preferred Channel Information */
typedef struct UMAC_PREFERRED_CHANNEL_INFO_S {
	uint8 CountryCode;
	uint8 NoOfTriplets;
	uint8 FirstChannel;
	uint8 NoOfChannels;
	uint8 TxPower;
	uint8 ChannelOffsetIndex;
} UMAC_PREFERRED_CHANNEL_INFO;

typedef struct WFM_UMAC_ARP_REPLY_S {
	uint16 HwType;
	uint16 ProtocolType;
	uint8  HwAddLen;
	uint8  ProtocolAddLen;
	uint16 Operation;
	uint8  MacAddSender[6];
	uint8  IpAddSender[4];
	uint8  MacAddTarget[6];
	uint8  IpAddTarger[4];
} WFM_UMAC_ARP_REPLY;

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
				  uint8 Encrypt
				  );

/******************************************************************************
 * NAME:	WFM_UMAC_CreateDot11FrameHdr_MoreGeneral
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is for inserting dot11 header information to the packet.
 * \param *dot11Hdr	    - Pointer to dot11 frame header.
 * \param Type		    - Type of the packet.
 * \param SubType	    - Sub type of the packet.
 * \param ToDs		    - Value of ToDs (0 or 1)
 * \param EncryptedPacket   - Value 0 for encrypted packet else 1
 * \param *ToDsBssid_Da_A1  - BSSID.
 * \param Sa_A2		    - Sa_A2
 * \param *ToDsDa_Bssid_A3  -DA Bssid
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_CreateDot11FrameHdr_MoreGeneral(WFM_UMAC_FRAME_HDR *dot11Hdr,
					      uint8 Type,
					      uint8 SubType,
					      uint8 ToDs,
					      uint8 EncryptedPacket,
					      uint8 *ToDsBssid_Da_A1,
					      uint8 *Sa_A2,
					      uint8 *ToDsDa_Bssid_A3);
#if FT_SUPPORT
/******************************************************************************
 * NAME:	WFM_UMAC_CreateFtActionReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an association request frame body.
 * \param UmacHandle		- Handle to UMAC Instance.
 * \param *pFrameBody		- a pointer to the buffer where association
 *                                frame body needs to be constructed.
 * \param *TargetApMac		- points to 6 octet MAC address of target AP.
 * \returns uint16		Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateFtActionReq(UMAC_HANDLE UmacHandle,
				  uint8 *pFrameBody,
				  uint8 *TargetApMac);
#endif	/* FT_SUPPORT */

/******************************************************************************
 * NAME:	WFM_UMAC_CreateAssocReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an association request frame body.
 * \param UmacHandle		   - Handle to UMAC Instance.
 * \param *pFrameBody		   - a pointer to the buffer where association
 *				     frame body needs to be constructed.
 * \param SsidLength		   - SSID length
 * \param *Ssid			   - SSID of the device/ network to be
 *				     connected with.
 * \param *ReAssoc_MacAddressOfAP  - Mac Address of AP in case of reassociation
 *				     (null otherwise)
 * \returns uint16		   Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateAssocReq(UMAC_HANDLE UmacHandle,
			       uint8 *pFrameBody,
			       uint32 SsidLength,
			       uint8 *Ssid,
			       uint8 *ReAssoc_MacAddressOfAP);

#if P2P_MINIAP_SUPPORT

/******************************************************************************
 * NAME:WFM_UMAC_CreateAssocRsp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an association response frame body.
 * \param UmacHandle	 - Handle to UMAC Instance.
 * \param *pFrameBody	 - A pointer to the buffer where association response
 *			   frame body needs to be constructed
 * \param   AID		 - Association ID of the STA
 * \param  status	 - Status to be filled in association response frame
 * \returns uint16	 Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateAssocRsp(UMAC_HANDLE UmacHandle,
			       uint8 *pFrameBody,
			       uint8 AID,
			       uint16 status);

/******************************************************************************
* NAME:	 UMAC_PutVendorIE
******************************************************************************/
/**
* \brief
* This function puts the vendor IE in Beacon/probeResponse.
* \param UmacHandle - Handle to the UMAC Instance
* \param ptr - Pointer where IE is put
* \return pointer to the end of vendor IE
******************************************************************************/
uint8 *UMAC_PutVendorIE(UMAC_HANDLE UmacHandle, uint8 *ptr, uint8 subType);

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
uint8 *UMAC_AddNoAAttribute(UMAC_HANDLE UmacHandle, uint8 *ptr, uint8 *p2pIE);

/******************************************************************************
* NAME:	 UMAC_NoAPresent
******************************************************************************/
/**
* \brief
* This function puts the vendor IE and NoA in Beacon.
* \param UmacHandle - Handle to the UMAC Instance
* \return TRUE if non-zero value of NoA is present
******************************************************************************/
uint8 UMAC_NoAPresent(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:WFM_UMAC_CreateGoNegoReqResp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates GO Negotiation request/response body
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param *pFrameBody	- A pointer to the buffer where GO negotiation
 *			  confirmation frame body needs to be constructed
 * \param  isReq	- 1: Request, 0: Response
 * \param  status	- Status to be filled in Go Negotiation response frame.
 *			  Ignored in case of creating Go negotiation request.
 * \returns uint16	 Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateGoNegoReqResp(UMAC_HANDLE UmacHandle,
				    uint8 *pFrameBody,
				    uint8 isReq,
				    uint8 status);

/******************************************************************************
 * NAME:WFM_UMAC_CreateGoNegoCnf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an association response frame body.
 * \param UmacHandle      - Handle to UMAC Instance.
 * \param *pFrameBody     - A pointer to the buffer where GO negotiation
 *			    confirmation frame body needs to be constructed
 * \param  status	  - Status to be filled in the confirmation frame
 * \returns uint16	  Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateGoNegoCnf(UMAC_HANDLE UmacHandle,
				uint8 *pFrameBody,
				uint8 status);

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
			   Ignored in case of creating Invitation request.
 * \returns uint16	 Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateInvtnReqResp(UMAC_HANDLE UmacHandle,
				   uint8 *pFrameBody,
				   uint8 isReq,
				   uint8 status);

#endif	/* P2P_MINIAP_SUPPORT */

/******************************************************************************
 * NAME:	WFM_UMAC_CreateDisAssocReq
 *----------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an DisAssociation request frame body.
 * \param UmacHandle  - Handle to UMAC Instance.
 * \param *pFrameBody - Pointer to the buffer where the frame body
 *			needs to be constructed.
 * \returns uint16    - Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateDisAssocReq(
					 UMAC_HANDLE UmacHandle,
					 uint8 *pFrameBody);

/******************************************************************************
 * NAME:	WFM_UMAC_CreateAuthFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an authentication request frame body.
 * \param UmacHandle	 - Handle to UMAC Instance.
 * \param *pFrameBody	 - a pointer to the buffer where authentication frame
 *                         body needs to be constructed.
 * \param AuthAlgo	 - Authentication algorithm to be used.
 * \param TransactionNo	 - Authentication Transaction Number.
 * \param pRxAuthFrame   - Received Auth frame.
 * \param status	 - Status to be filled in auth frame
 * \returns uint16	 -  Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateAuthFrame(UMAC_HANDLE UmacHandle,
				uint8 *pFrameBody,
				uint16 AuthAlgo,
				uint16 TransactionNo,
				WFM_UMAC_AUTH_FRAME_BODY *pRxAuthFrame,
				uint16 status);

/******************************************************************************
 * NAME:	WFM_UMAC_CreateDeAuthFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates an DeAuthentication request frame body.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param pFrameBody    - a pointer to the buffer where
 *			  authentication frame body needs to be constructed.
 * \returns uint16      - Length of the frame created.
 *****************************************************************************/
uint16 WFM_UMAC_CreateDeAuthFrame(
					 UMAC_HANDLE UmacHandle,
					 uint8 *pFrameBody);

/******************************************************************************
 * NAME:	WFM_UMAC_CreateProbeReqTemplateFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function creates a Probe Request Template Frame.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pFrameBody   - a pointer to the buffer where authentication frame
 *                        body needs to be constructed.
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
					    uint8 Type);
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
				   uint16 TransactionId);
#endif	/* MGMT_FRAME_PROTECTION */

/******************************************************************************
 * NAME:	WFM_UMAC_ProcessBeaconOrProbRspFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This processes incoming beacon and prob response frames.
 * \param UmacHandle	     - Handle to UMAC Instance.
 * \param *pLmacRxDescriptor - Incoming RxDescriptor from LMAC.
 * \param IsBeacon	     - Whether the incoming frame is a beacon or Prob
 *                             Response
 * \returns uint16
 * WFM_BEACON_PROBRSP_LIST_UPDATED	       - If its a new beacon and its
 *					        inserted to list
 * WFM_BEACON_PROBRSP_ERROR		       - Some error occured
 * WFM_BEACON_PROBRSP_INTERNAL_SCAN_ONGOING    - Internal Scan was not able to
 *					         find the device.
 * WFM_BEACON_PROBRSP_INTERNAL_SCAN_GOT_RESULT - Internal Scan finished.
 *****************************************************************************/
uint16 WFM_UMAC_ProcessBeaconOrProbRspFrame(UMAC_HANDLE UmacHandle,
					    void *pLmacRxDescriptor,
					    uint8 IsBeacon);

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
					   IeElement *pIelement);

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
					 P2P_IE_ELEMENT *pIeElement);
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
					 uint16 IELength);

/******************************************************************************
 * NAME:	WFM_UMAC_Cvt_80211abgFormat_Rate_To_WfmFormat_Rate
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts 802.11 formatted rate into WFM format rate.
 * It returns basic/supported rates in WFM format.
 * \param NumRates	        - Number of rates in pu8SupportedRatesRemote.
 * \param *pu8_80211FormatRate  - Supported Rates in 802.11 formate
 *                                (in 500Kbps) with msb set for basic rate.
 * \param *pu32SupportedRateSet - Supported Rate in WFM format
 *                                (supported by us).
 * \param *pu32BasicRateSet	- Basic Supported Rate in WFM format.
 * \returns none.
 *********************************************************************************/
void WFM_UMAC_Cvt_80211abgFormat_Rate_To_WfmFormat_Rate(uint32 NumRates,
							uint8 *pu8_80211FormatRate,
							uint32 *pu32SupportedRateSet,
							uint32 *pu32BasicRateSet);

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
							  uint32 *pu32RemainingBasicRateSet);

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
									uint32 *pu32BasicRateSet);

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
						      uint32 *pu32WfmFormatRate);

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
									    WFM_HT_SUPPORTED_MCS_SET_PACKED *p802_11nFormatRate);

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
					   uint32 *pWfmBasicSuppRate_abgn);

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
				     uint8 use_greenfield);

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
			      IeElement *pIelement);

/******************************************************************************
 * NAME:	WFM_ProcVendorSpecificIeElementsWPA
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function retrives the given WPA IE element from the given beacon/prob
 * response.
 * \param *IeElementStart   -  Start of IE lements in the beacon or prob rsp.
 * \param IELength	    -  Total length of the IE elements.
 * \param pOuiInfo	    -  Pointer to OUI which we are looking for
 * \param *pIelement	    -  memory to which IE elements to be copied.
 * \returns uint8 TRUE (IE found), FALSE (IE not found).
 *****************************************************************************/
void WFM_ProcVendorSpecificIeElementsWPA(uint8 *IeElementStart,
						uint16 IELength,
						WFM_OUI_INFO *pOuiInfo,
						IeElement *pIelement);

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
uint32 UMAC_ProcProbeReqCheckCCKRates(UMAC_HANDLE UmacHandle,
				      void *pMsg);

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
			 WPS_IeElement *pAttrib);

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
			    UMAC_RUNTIME *pUmacRuntime);

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
					      UMAC_RUNTIME *pUmacRunTime);

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
					   HT_OPERATION_IE *pHtOpIe);

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
					   HT_OPERATION_IE *pHtOpIe);

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
				     uint16 LengthOfIEElements);

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
				    WFM_BSS_CACHE_INFO_IND *pBeaconFrameBody);

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
					   IeElement *pCountryInfoIe);

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
					     uint8 *pRegionCode);

/******************************************************************************
* NAME:	 UMAC_PutCountryIE
******************************************************************************/
/**
* \brief
* This function puts the country IE.
* \param UmacHandle - Handle to the UMAC Instance
* \param ptr - Pointer where IE is put
* \return pointer
******************************************************************************/
uint8 *UMAC_PutCountryIE(UMAC_HANDLE UmacHandle, uint8 *ptr);


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
							 uint8 PhyBand);

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
uint8 WFM_IsChannelNotSupported(uint8 ChannelNo, uint8 ChannelIndex);

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
uint8 WFM_GetPMKIDIndex(UMAC_HANDLE UmacHandle, uint8 *pBSSID, uint8 *pIndex);


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
				 sint32 *PowerLevel);


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
						uint16 regDomainLen);

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
				      uint32 *txPowerLevel);

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
WFM_STATUS_CODE UMAC_SendChannelReport(UMAC_HANDLE UmacHandle);

/******************************************************************************
* NAME:	 UMAC_SendNoiseHistReport
******************************************************************************/
/**
* \brief
* This function sends the noise histogram report to AP.
* \param UmacHandle - Handle to UMAC Instance
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE UMAC_SendNoiseHistReport(UMAC_HANDLE UmacHandle);

/******************************************************************************
* NAME:	 UMAC_SendBeaconReport
******************************************************************************/
/**
* \brief
* This function sends the beacon report to AP.
* \param UmacHandle - Handle to UMAC Instance
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE UMAC_SendBeaconReport(UMAC_HANDLE UmacHandle);

/******************************************************************************
* NAME:	 UMAC_SendStatisticsReport
******************************************************************************/
/**
* \brief
* This function sends the Statistics report to AP.
* \param UmacHandle - Handle to UMAC Instance
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE UMAC_SendStatisticsReport(UMAC_HANDLE UmacHandle);

/******************************************************************************
* NAME:	 UMAC_SendLCIReport
******************************************************************************/
/**
* \brief
* This function sends the LCI report to AP.
* \param UmacHandle - Handle to UMAC Instance
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE UMAC_SendLCIReport(UMAC_HANDLE UmacHandle);

/******************************************************************************
* NAME:	 UMAC_SendTxStreamReport
******************************************************************************/
/**
* \brief
* This function sends the Link Measurement report to AP.
* \param UmacHandle - Handle to UMAC Instance
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE UMAC_SendTxStreamReport(UMAC_HANDLE UmacHandle);

/******************************************************************************
* NAME:	 UMAC_SendLinkMeasurementReport
******************************************************************************/
/**
* \brief
* This function sends the Link Measurement report to AP.
* \param UmacHandle - Handle to UMAC Instance
* \return WFM_STATUS_CODE
******************************************************************************/
WFM_STATUS_CODE UMAC_SendLinkMeasurementReport(UMAC_HANDLE UmacHandle);

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
uint32 UMAC_Div_64(uint64 *pVal, uint32 base);

#endif	/* DOT11K_SUPPORT */

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
														uint32 *pNumOfChannels);
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
				     uint8 IsBeacon);

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
uint32 WFM_UMAC_DeAuthSTAUnicast(UMAC_HANDLE UmacHandle, uint8 *StaMacAddress);

#endif	/* _UMAC_FRAMES_H */
