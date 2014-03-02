/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
* \file
* \ingroup GLOBAL
*
* \brief IEEE 802.11 definitions
*/

#ifndef _D11_DEFS_H
#define _D11_DEFS_H

/***********************************************************************/
/***			Include Files			    ***/
/***********************************************************************/
#include "d0_defs.h"

/* -- Useful bit definitions    */

#ifndef BIT

#define BIT(n) (1 << n)
#define BIT0   BIT(0)
#define BIT1   BIT(1)
#define BIT2   BIT(2)
#define BIT3   BIT(3)
#define BIT4   BIT(4)
#define BIT5   BIT(5)
#define BIT6   BIT(6)
#define BIT7   BIT(7)
#define BIT8   BIT(8)
#define BIT9   BIT(9)
#define BIT10  BIT(10)
#define BIT11  BIT(11)
#define BIT12  BIT(12)
#define BIT13  BIT(13)
#define BIT14  BIT(14)
#define BIT15  BIT(15)

#endif

#ifndef ENABLE_11W
#define ENABLE_11W 0
#endif

/***********************************************************************/
/***			Data Types and Constants		 ***/
/***********************************************************************/
/* This protocal version is 0. If it is changed, then the frame format
   will be different. To make sure the protocal version is checked against 0,
   we expand D11 type mask from 0x0C to 0x0F.
   Alternatively, we have to define D11_P0_TYPE_MASK to 0x0F.
   But who will use it? If we assume D11 means D11 P0, then it is OK.
*/
#define D11_TYPE_MASK	   0x0F	/* for all protocal versions, 0x0c */
#define D11_TYPE_MASK_ALLVER    0x0C
#define D11_PROT_VER	    0x00
#define D11_PROT_MASK	   0x03
#define D11_MGMT_TYPE	   0x00
#define D11_CNTL_TYPE	   0x04
#define D11_DATA_TYPE	   0x08
#define D11_RESERVED_TYPE       0x0c
#define D11_SUB_TYPE_ONLY_MASK  0xF0
#define D11_SUB_TYPE_MASK       (D11_SUB_TYPE_ONLY_MASK | D11_TYPE_MASK)

#define D11_GET_TYPE(__x)       ((__x) & D11_TYPE_MASK)
#define D11_GET_SUB_TYPE(__x)   ((__x) & D11_SUB_TYPE_MASK)
#define D11_IS_MGMT(__x)	(D11_GET_TYPE(__x) == D11_MGMT_TYPE)
#define D11_IS_CNTL(__x)	(D11_GET_TYPE(__x) == D11_CNTL_TYPE)
#define D11_IS_DATA(__x)	(D11_GET_TYPE(__x) & D11_DATA_TYPE)

#define D11_SUB_MGMT_ASRQ       (0x00 + D11_MGMT_TYPE)
#define D11_SUB_MGMT_ASRSP      (0x10 + D11_MGMT_TYPE)
#define D11_SUB_MGMT_RSRQ       (0x20 + D11_MGMT_TYPE)
#define D11_SUB_MGMT_RSRSP      (0x30 + D11_MGMT_TYPE)
#define D11_SUB_MGMT_PBRQ       (0x40 + D11_MGMT_TYPE)
#define D11_SUB_MGMT_PBRSP      (0x50 + D11_MGMT_TYPE)
#define D11_SUB_MGMT_BCN	(0x80 + D11_MGMT_TYPE)
#define D11_SUB_MGMT_ATIM       (0x90 + D11_MGMT_TYPE)
#define D11_SUB_MGMT_DAS	(0xa0 + D11_MGMT_TYPE)
#define D11_SUB_MGMT_AUTH       (0xb0 + D11_MGMT_TYPE)
#define D11_SUB_MGMT_DAUTH      (0xc0 + D11_MGMT_TYPE)
#define D11_SUB_MGMT_ACTION     (0xd0 + D11_MGMT_TYPE)

#define D11_SUB_CNTL_WRAPPER    (0x70 + D11_CNTL_TYPE)
#define D11_SUB_CNTL_BLKACK_REQ (0x80 + D11_CNTL_TYPE)
#define D11_SUB_CNTL_BLKACK     (0x90 + D11_CNTL_TYPE)
#define D11_SUB_CNTL_PSPOLL     (0xa0 + D11_CNTL_TYPE)
#define D11_SUB_CNTL_RTS	(0xb0 + D11_CNTL_TYPE)
#define D11_SUB_CNTL_CTS	(0xc0 + D11_CNTL_TYPE)
#define D11_SUB_CNTL_ACK	(0xd0 + D11_CNTL_TYPE)
#define D11_SUB_CNTL_CFEND      (0xe0 + D11_CNTL_TYPE)
#define D11_SUB_CNTL_CFEACK     (0xf0 + D11_CNTL_TYPE)

#define D11_SUB_DATA	    (0x00 + D11_DATA_TYPE)
#define D11_SUB_DATA_CFACK      (0x10 + D11_DATA_TYPE)
#define D11_SUB_DATA_CFPOLL     (0x20 + D11_DATA_TYPE)
#define D11_SUB_DATA_CFAKPL     (0x30 + D11_DATA_TYPE)
#define D11_SUB_DNUL	    (0x40 + D11_DATA_TYPE)
#define D11_SUB_DNUL_CFACK      (0x50 + D11_DATA_TYPE)
#define D11_SUB_DNUL_CFPOLL     (0x60 + D11_DATA_TYPE)
#define D11_SUB_DNUL_CFAKPL     (0x70 + D11_DATA_TYPE)

/*QOS Changes - START */
#define D11_SUB_QDATA_BIT       (0x80)
#define D11_SUB_QTYPE_MASK      (D11_SUB_QDATA_BIT | D11_TYPE_MASK)
#define D11_SUB_QTYPE_MASK_ALLVER (D11_SUB_QDATA_BIT | D11_TYPE_MASK_ALLVER)
#define D11_SUB_QTYPE_VAL       (D11_SUB_QDATA_BIT | D11_DATA_TYPE)
#define D11_SUB_QDATA	   (0x80 + D11_DATA_TYPE)
#define D11_SUB_QDATA_CFACK     (0x90 + D11_DATA_TYPE)
#define D11_SUB_QDATA_CFPOLL    (0xA0 + D11_DATA_TYPE)
#define D11_SUB_QDATA_CFAKPL    (0xB0 + D11_DATA_TYPE)
#define D11_SUB_QDNUL	   (0xC0 + D11_DATA_TYPE)
#define D11_SUB_QDNUL_CFACK     (0xD0 + D11_DATA_TYPE)
#define D11_SUB_QDNUL_CFPOLL    (0xE0 + D11_DATA_TYPE)
#define D11_SUB_QDNUL_CFAKPL    (0xF0 + D11_DATA_TYPE)
/*QOS Changes - END */

/* Bit usage in data frames */
#define D11_NULL_BIT	    0x40
#define D11_CFPOLL_BIT	  0x20
#define D11_CFACK_BIT	   0x10
#define D11_DATA_BIT	    0x00

/* Remaining bits in frame control */
#define D11_TO_DS	       0x0100
#define D11_FROM_DS	     0x0200
#define D11_DS_DS	       0x0300
#define D11_MORE_FRAG	   0x0400
#define D11_RETRY	       0x0800
#define D11_PWR_MGMT	    0x1000
#define D11_MORE_DATA	   0x2000
#define D11_PRIVACY	     0x4000
#define D11_ORDER	       0x8000

/*A4UPD: Masks & Bitmap for FromDs ToDS bits*/
#define D11_WDS_FRM_IDENTIFIER  0x03
#define D11_FROM_DS_TO_DS_MASK  0x03

/* action frame categories */
#define D11_CATEGORY_11H_SPECTRUM_MGMT      0
#define D11_CATEGORY_11E_QOS		1
#define D11_CATEGORY_BLOCK_ACK	      3
#define D11_CATEGORY_PUBLIC		 4
#define D11_CATEGORY_11K_RADIO_MEASUREMENT  5
#define D11_CATEGORY_HT		     7
#define D11_CATEGORY_WME_QOS	       17
#define D11_CATEGORY_VENDOR_SPECIFIC      127

/********************************************************************/
/* -- IEEE802.11e and WME stuff				     */
/********************************************************************/

/* -- QoS action IDs	  */
#define D11_ACTION_ADDTS_REQ    0
#define D11_ACTION_ADDTS_RESP   1
#define D11_ACTION_DELTS_REQ    2
#define D11_ACTION_SCHEDULE     3

/* -- Block Ack action frame IDs */
#define D11_ACTION_ADDBA_REQ   0
#define D11_ACTION_ADDBA_RESP  1
#define D11_ACTION_DELBA_REQ   2

/* -- Fields in BlockAck Parameter Set (IEEE802.11e draft 9) */
#define D11_BLOCKACK_PS_AMSDU_UNSUPPORTED 0
#define D11_BLOCKACK_PS_AMSDU_SUPPORTED   1

#define D11_BLOCKACK_PS_IMMEDIATE_SHIFT  1
#define D11_BLOCKACK_PS_IMMEDIATE_MASK   0x0001
#define D11_BLOCKACK_PS_DELAYED	  0
#define D11_BLOCKACK_PS_IMMEDIATE	1

#define D11_BLOCKACK_PS_TID_SHIFT	2
#define D11_BLOCKACK_PS_TID_MASK	 0x000F

#define D11_BLOCKACK_PS_BUFFERSIZE_SHIFT 6
#define D11_BLOCKACK_PS_BUFFERSIZE_MASK  0x03FF

#define D11_BACONTROL_COMPRESSED	 4
#define D11_BACONTROL_TID_SHIFT	  12
#define D11_BACONTROL_TID_MASK	   0x000F

#define D11_DELBA_TID_SHIFT	      D11_BACONTROL_TID_SHIFT
#define D11_DELBA_TID_MASK	       D11_BACONTROL_TID_MASK

#define D11_DELBA_INITIATOR_ORIGINATOR   BIT(11)
#define D11_DELBA_INITIATOR_REIPIENT     0

/* QoS field bit encodings */
#define D11_TID_MASK	    0x000F
#define D11_EOSP		0x0010
#define D11_STA_QSIZE	   0x0010
#define D11_ACK_POLICY_MASK     0x0060
#define D11_ACK_POLICY_BIT_OFFSET 5
#define D11_Q_RESERVED	  0x0080
#define D11_TXOP_MASK	   0xFF00
#define D11_TXOP_SHIFT	  8
#define D11_QSIZE_MASK	  0xFF00
#define D11_QSIZE_SHIFT	 8

/* To retrieve TID from QoS Control field */
#define D11_GET_TID(__x)   ((__x) & D11_TID_MASK)

/* ACK Policy bits */
#define D11_ACK_POLICY_NORMAL   0x0000
#define D11_ACK_POLICY_NO_ACK   0x0020
#define D11_ACK_POLICY_NO_EXP   0x0040	/* for use with poll */
#define D11_ACK_POLICY_BLK_ACK  0x0060	/* Block Acknowledgement, ack later */

/* QoS Control Field - Ack Policy Bits */
#define D11_QOS_CTRL_ACKPOL_B5  0x20	/* Ack Policy bit 5 in QoS Ctrl Field */
#define D11_QOS_CTRL_ACKPOL_B6  0x40	/* Ack Policy bit 6 in QoS Ctrl Field */

/* TID bits - bit 7 says TS rather than UP */
#define D11_TSID		0x80

/* TID in BAR control foBlock Ack is in bits 12-15 */
#define D11_BAR_TID_MASK	0x7000
#define D11_BAR_TID_SHIFT       12

/* STA QoS Info Field bits */
#define D11_GET_UAPSD(__x)      ((__x) & D11_QOS_INFO_STA_UAPSD_MASK)

/* AP QoS Info Field bits */
#define D11_UAPSD_SUPPORTED	     0x80
#define D11_PARAMETER_SETCOUNT_MASK     0x0F

/*#define D11_MARK_UAPSD_FLAG(AC) (1 << ((D11_NUM_ACS-1)-AC))       */ /* OBSOLETE */

#define D11_QOS_INFO_STA_UAPSD_MASK     0x0F
#define D11_QOS_INFO_STA_UAPSD_NONE     0x00
#define D11_QOS_INFO_STA_UAPSD_AC_BK    0x04
#define D11_QOS_INFO_STA_UAPSD_AC_BE    0x08
#define D11_QOS_INFO_STA_UAPSD_AC_VI    0x02
#define D11_QOS_INFO_STA_UAPSD_AC_VO    0x01

#define D11_GET_MAXSP_LENGTH(__x)   ((__x) & D11_QOS_INFO_STA_MAXSP_LEN_MASK)

#define D11_QOS_INFO_STA_MAXSP_LEN_MASK      0x60
#define D11_QOS_INFO_STA_MAXSP_LEN_ALLFRMS   0x00
#define D11_QOS_INFO_STA_MAXSP_LEN_2FRMS     0x20
#define D11_QOS_INFO_STA_MAXSP_LEN_4FRMS     0x40
#define D11_QOS_INFO_STA_MAXSP_LEN_6FRMS     0x60

/********************************************************************/
/* -- IEEE802.11h stuff					     */
/********************************************************************/

/* -- Action IDs	    */

#define D11_ACTION_MEASUREMENT_REQUEST 0
#define D11_ACTION_MEASUREMENT_REPORT  1
#define D11_ACTION_TPC_REQUEST	 2
#define D11_ACTION_TPC_REPORT	  3
#define D11_ACTION_CHANNEL_SW_ANNOUNCE 4

/* -- Measurement Request mode	  */

#define D11_MEAS_REQUEST_MODE_ENABLE   BIT(1)
#define D11_MEAS_REQUEST_MODE_REQUEST  BIT(2)
#define D11_MEAS_REQUEST_MODE_REPORT   BIT(3)

/* -- Measurement Report Mode bits      */

#define D11_MEAS_REPORT_MODE_LATE      BIT(0)
#define D11_MEAS_REPORT_MODE_INCAPABLE BIT(1)
#define D11_MEAS_REPORT_MODE_REFUSED   BIT(2)

/* -- Measurement Type values	   */

#define D11_MEAS_TYPE_BASIC 0
#define D11_MEAS_TYPE_CCA   1
#define D11_MEAS_TYPE_RPI   2

/* -- Basic report Map bits */

#define D11_MEAS_REPORT_BASIC_BSS_ACTIVITY  BIT(0)
#define D11_MEAS_REPORT_BASIC_OFDM_ACTIVITY BIT(1)
#define D11_MEAS_REPORT_BASIC_UNIDENTIFIED  BIT(2)
#define D11_MEAS_REPORT_BASIC_RADAR	 BIT(3)
#define D11_MEAS_REPORT_BASIC_UNMEASURED    BIT(4)

/* the following used in PS-Poll */
#define D11_AID_DURATION_VALUE	 0xC000
#define D11_AID_DURATION_VALUE_MASK    0x3FFF
/* The following used in CFP for all frames */
#define D11_CFP_DURATION_VALUE	 0x8000

/* Bit field for the 32 bits of IV */
#define D11_IV_EXTENDED_IV      0x20000000

#define D11_FRAG_MASK	   0xF
#define D11_SEQ_MASK	    0xFFF0
#define D11_FRAG(s)	     ((s) & D11_FRAG_MASK)
#define D11_SEQ(s)	      ((s) >> 4)
#define D11_SEQNUM_TO_SEQCTL(s) ((s) << 4)

/*  Capability bits. */
#define D11_CAPS_ESS		    0x0001
#define D11_CAPS_IBSS		   0x0002
#define D11_CAPS_CF_POLLABLE	    0x0004
#define D11_CAPS_CF_POLL_REQ	    0x0008
#define D11_CAPS_PRIVACY		0x0010
/*  The following fields were added in 802.11b */
#define D11_CAPS_SHORT_PREAMBLE	 0x0020
#define D11_CAPS_PBCC		   0x0040
#define D11_CAPS_CHANNEL_AGILE	  0x0080
/* 11h related bit */
#define D11_CAPS_SPECTRUM_MGMT	  0x0100
#define D11_CAPS_QOS		    0x0200
/*  The following fields were added in the security baseline. */
#define D11_CAPS_G_SHORT_SLOT	   0x0400

/* The following fields were added in 802.11e */
#define D11_CAPS_UAPSD		  0x0800
#define D11_CAPS_DELAYED_BLKACK	 0x4000
#define D11_CAPS_IMM_BLKACK	     0x8000

/*  Bit rates (in PLCP) */
#define D11_BASIC_RATE		  0x80

/* Authentication algorithms */
#define D11_ALGOR_OPEN_SYSTEM	   0x0
#define D11_ALGOR_SHARED_KEY	    0x1

/* ERP IE bits */
#define D11_ERP_NON_ERP_PRESENT	 0x01	/* 11b STA associated */
#define D11_ERP_USE_PROTECTION	  0x02	/* Send CTS-to-self */
#define D11_ERP_BARKER_PREAMBLE_MODE    0x04	/* other STA does not support Short Preamble */

/*  Element ids */
#define D11_SSID_ELT_ID		 0
#define D11_RATES_ELT_ID		1
#define D11_DS_ELT_ID		   3
#define D11_CF_ELT_ID		   4
#define D11_TIM_ELT_ID		  5
#define D11_IBSS_PARAM_ELT_ID	   6
#define D11_COUNTRY_ELT_ID	      7
#define D11_MCAST_CIPHER_ELT_ID	 8
#define D11_UNICAST_CIPHER_ELT_ID       9
#define D11_REQUEST_ELT_ID	      10
#define D11_REALM_NAME_ELT_ID	   11
#define D11_PRINCIPAL_NAME_ELT_ID       12
#define D11_CHALLENGE_ELT_ID	    16
#define D11_ERP_ELT_ID		  42
#define D11_QOS_CAPABILITY_ELT_ID       46
#define D11_RATES_EXTENDED_ELT_ID       50
#define D11_HT_OPERATION_ELT_ID         61
#define D11_MANAGEMENT_MIC_ELT_ID       76
#define D11_SECURITY_ELT_ID	     0xDD	/* obsolete */
#define D11_WIFI_ELT_ID		 0xDD	/* 0xDD=221 */
#define D11_RSN_ELT_ID		  0x30

#define D11_PWR_CONSTRAINT_ELT_ID       32
#define D11_PWR_CAPABILITY_ELT_ID       33
#define D11_TPC_REQUEST_ELT_ID	  34
#define D11_TPC_REPORT_ELT_ID	   35
#define D11_SUPPORTED_CHANNELS_ELT_ID   36
#define D11_CHANNEL_SWITCH_ELT_ID       37
#define D11_MEASUREMENT_REQ_ELT_ID      38
#define D11_MEASUREMENT_REP_ELT_ID      39
#define D11_QUIET_ELT_ID		40
/* OUI's */
#define WIFI_OUI			{0x00, 0x50, 0xF2}
#define WFA_OUI			 {0x50, 0x6F, 0x9A}
#define WIFI_WPA_OUI_TYPE	       0x01
#define WIFI_WME_OUI_TYPE	       0x02
#define WIFI_WPS_OUI_TYPE	       0x04
#define WIFI_P2P_IE_OUI_TYPE	    0x09

#define D11_RATES_MAX_SIZE_ID	   8

/*P2P Specific Constants*/
#define WIFI_P2P_WILDCARD_SSID	  "DIRECT-"

/*P2P Subelement Ids */
#define D11_P2P_STATUS_ELT_ID	    0x00
#define D11_P2P_REASON_ELT_ID	    0x01
#define D11_P2P_CAPABILITY_ELT_ID	0x02
#define D11_P2P_DEVICE_ELT_ID	    0x03
#define D11_P2P_NOA_ELT_ID	       0x0C
#define D11_P2P_DEVICE_INFO_ELT_ID       0x0D

/* TIM ELT ID Related definitions */
#define TIM_ELT_ID__LENGTH_OFFSET	  1
#define TIM_ELT_ID__DTIM_COUNT_OFFSET      2
#define TIM_ELT_ID__DTIM_PERIOD_OFFSET     3
#define TIM_ELT_ID__BITMAP_CONTROL_OFFSET  4
#define TIM_ELT_ID__PV_BITMAP__OFFSET      5
#define TIM_ELT_ID__MULTICAST_MASK	 1
#define TIM_ELT_ID__MULTICAST_CLEAR	0xFE
#define TIM_ELT_ID__NON_PV_OVERHEAD	3

/*  Authentication suite selectors. */
#define D11_ASE_OPEN		    0x00000000
#define D11_ASE_SHARED_KEY	      0x01000000
#define D11_ASE_UNSPECIFIED	     0x02000000
#define D11_ASE_KERBEROS		0x03000000

/* WMM Information Element */
#define D11_WMM_INFO_ELMT_SIZE	  9

#define D11_QUIET_ELT_LENGTH	     8
#define D11_CHANNEL_SWITCH_ELT_LENGTH    5

#define D11_CHANN_SWITCH_ELT_MODE  2
#define D11_CHANN_SWITCH_ELT_CHAN  3
#define D11_CHANN_SWITCH_ELT_COUNT 4

#define D11_QUIET_ELT_COUNT     2
#define D11_QUIET_ELT_PERIOD    3
#define D11_QUIET_ELT_DURATION  4
#define D11_QUIET_ELT_OFFSET    6

/*  Gets id from pointer to element */
#define D11_ELMT_ID(p)  (*(uint8*)(p))
/* Gets the length of an element excluding length and type fields. */
#define D11_ELMT_LEN(p) (*(((uint8*)(p))+1))
/*  Gets the total size of an element. */
#define D11_ELMT_SIZE(p)    (D11_ELMT_LEN(p)+2)
/*  Gets the start of the data part of an element. */
#define D11_ELMT_DATA(p)    ((uint8*)(p)+2)

/*  Gets the start of the data part of a Vendor specific element. */
#define D11_V_ELMT_DATA(p)  ((uint8*)(p)+6)


/* Gets the length field of a P2P sub-element*/
#if 1 /* Always in, fixed when USE_P2P_FIX_WSM302 */
#define D11_P2P_SUBELMT_LEN(p) ((((uint16)((uint8 *)(p))[2])<<8)+((uint8 *)(p))[1])
#else
#define D11_P2P_SUBELMT_LEN(p) (uint16)*((uint8 *)p+1)
#endif
/*  Gets the total size of a P2P Sub-element. */
#define D11_P2P_SUBELMT_SIZE(p)    (D11_P2P_SUBELMT_LEN(p)+3)


/*  Authentication frame size. */
#define D11_CHALLENGE_TEXT_SIZE	 128

#define BLK_ACK_BITMAP_SIZE_W	     4	/* 16 bit words = 64 bits */

typedef struct D11_HEADER_S {
	union ALL_HDRS {
		struct HDR_RAW {
			uint16 FrameCtl;
			uint16 DurationId;
			D0_ADDR A1;
			D0_ADDR A2;
			D0_ADDR A3;
			uint16 SeqCtl;
			union HDR_OPTIONAL {
				struct HDR_RAW_A4_QOS {
					D0_ADDR A4;	/*present */
					uint16 QosCtl;	/* optional */
				} DsDs;
				uint16 QosCtl;	/* optional */
			} opt;
		} Raw;
		struct HDR_IBSS {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR Sa;
			D0_ADDR BssId;
			uint16 SeqCtl;
			uint16 QosCtl;	/* optional */
		} DataIbss;
		struct HDR_FROM_DS {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR BssId;
			D0_ADDR Sa;
			uint16 SeqCtl;
			uint16 QosCtl;	/* optional */
		} DataFromDs;
		struct HDR_TO_DS {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR BssId;
			D0_ADDR Sa;
			D0_ADDR Da;
			uint16 SeqCtl;
			uint16 QosCtl;	/* optional */
		} DataToDs;
		struct HDR_DS_DS {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Ra;
			D0_ADDR Ta;
			D0_ADDR Da;
			uint16 SeqCtl;
			D0_ADDR Sa;
			uint16 QosCtl;	/* optional */
		} DataDsDs;
		struct HDR_PS_POLL {
			uint16 FrameCtl;
			uint16 Aid;
			D0_ADDR BssId;
			D0_ADDR Ta;
		} PsPoll;
		struct HDR_ACK {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Ra;
		} Ack;
		struct HDR_BLOCKACK_REQ {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Ra;
			D0_ADDR Ta;
			uint16 BarControl;
			uint16 SeqCtl;
		} BlockAckReq;
		struct HDR_BLOCKACK {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Ra;
			D0_ADDR Ta;
			uint16 BarControl;
			uint16 BaStartSeqCtrl;
			uint32 Bitmap[BLK_ACK_BITMAP_SIZE_W];
		} BlockAck;
		struct HDR_RTS {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Ra;
			D0_ADDR Ta;
		} Rts;
		struct HDR_CTS {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Ra;
		} Cts;
		struct HDR_CFEND {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Ra;
			D0_ADDR BssId;
		} CfEnd;
		struct HDR_MGMT {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR Sa;
			D0_ADDR BssId;
			uint16 SeqCtl;
		} Mgmt;
		struct HDR_ATIM {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR Sa;
			D0_ADDR BssId;
			uint16 SeqCtl;
		} Atim;
		PACKED struct HDR_BEACON {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR Sa;
			D0_ADDR BssId;
			uint16 SeqCtl;
			uint32 TimestampLo;
			uint32 TimestampHi;
			uint16 Interval;
			uint16 Capability;
			uint8 Elts[1];
		} Beacon;
		PACKED struct HDR_PROBE_REQ {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR Sa;
			D0_ADDR BssId;
			uint16 SeqCtl;
			uint8 Elts[1];
		} ProbeReq;
		PACKED struct HDR_PROBE_RSP {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR Sa;
			D0_ADDR BssId;
			uint16 SeqCtl;
			uint32 TimestampLo;
			uint32 TimestampHi;
			uint16 Interval;
			uint16 Capability;
			uint8 Elts[1];
		} ProbeRsp;
		struct HDR_AUTH {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR Sa;
			D0_ADDR BssId;
			uint16 SeqCtl;
			uint16 Algorithm;
			uint16 Seq;
			uint16 Status;
			uint8 ChallengeId;
			uint8 ChallengeLen;
			uint8 Challenge[D11_CHALLENGE_TEXT_SIZE];
#if ENABLE_11W			/*temporary until 11w is fixed*/
			uint8 Elts[1];
#endif
		} Auth;
		struct HDR_DEAUTH {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR Sa;
			D0_ADDR BssId;
			uint16 SeqCtl;
			uint16 Reason;
			uint8 Elts[1];
		} Deauth;
		PACKED struct HDR_ASSOC_REQ {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR Sa;
			D0_ADDR BssId;
			uint16 SeqCtl;
			uint16 Capability;
			uint16 ListenInterval;
			uint8 Elts[1];
		} AssocReq;
		PACKED struct HDR_ASSOC_RSP {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR Sa;
			D0_ADDR BssId;
			uint16 SeqCtl;
			uint16 Capability;
			uint16 Status;
			uint16 Aid;
			uint8 Elts[1];
		} AssocRsp;
		PACKED struct HDR_REASSOC_REQ {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR Sa;
			D0_ADDR BssId;
			uint16 SeqCtl;
			uint16 Capability;
			uint16 ListenInterval;
			D0_ADDR CurrentAp;
			uint8 Elts[1];
		} ReassocReq;
		PACKED struct HDR_REASSOC_RSP {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR Sa;
			D0_ADDR BssId;
			uint16 SeqCtl;
			uint16 Capability;
			uint16 Status;
			uint16 Aid;
			uint8 Elts[1];
		} ReassocRsp;
		PACKED struct HDR_DISASSOC {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR Da;
			D0_ADDR Sa;
			D0_ADDR BssId;
			uint16 SeqCtl;
			uint16 Reason;
			uint8 Elts[1];
		} Disassoc;
		PACKED struct HDR_ACTION_RAW {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR DA;
			D0_ADDR SA;
			D0_ADDR BSSID;
			uint16 SeqCtl;
			uint8 Category;
			uint8 Details[1];
		} ActionRaw;
		PACKED struct HDR_ADDBA_REQ {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR DA;
			D0_ADDR SA;
			D0_ADDR BSSID;
			uint16 SeqCtl;
			uint8 Category;
			uint8 Action;
			uint8 DialogToken;
			uint16 BAParamSet;	/*Note this is not word aligned as per the standard*/
			uint16 BATimeOut;
			uint16 BAStartSeqCtl;
			uint8 Elts[1];
		} HDR_ADDBA_REQ;
		PACKED struct HDR_ADDBA_RESP {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR DA;
			D0_ADDR SA;
			D0_ADDR BSSID;
			uint16 SeqCtl;
			uint8 Category;
			uint8 Action;
			uint8 DialogToken;
			uint16 StatusCode;	/*Note this is not word aligned as per the standard*/
			uint16 BAParamSet;
			uint16 BATimeOut;
			uint8 Elts[1];
		} HDR_ADDBA_RESP;
		struct HDR_DELBA_REQ {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR DA;
			D0_ADDR SA;
			D0_ADDR BSSID;
			uint16 SeqCtl;
			uint8 Category;
			uint8 Action;
			uint16 DELBAParamSet;
			uint16 ReasonCode;
			uint8 Elts[1];
		} HDR_DELBA_REQ;
		PACKED struct HDR_ADDBA_REQ_ALIGN {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR DA;
			D0_ADDR SA;
			D0_ADDR BSSID;
			uint16 SeqCtl;
			uint8 Category;
			uint8 Action;
			uint8 DialogToken;
			uint8 BaParamSetLsb;
			uint8 BaParamSetMsb;
			uint8 BaTimeoutLsb;
			uint8 BaTimeoutMsb;
			uint8 BaStartSeqCtlLsb;
			uint8 BaStartSeqCtlMsb;
			uint8 Elts[1];
		} HDR_ADDBA_REQ_ALIGN;
		PACKED struct HDR_ADDBA_RESP_ALIGN {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR DA;
			D0_ADDR SA;
			D0_ADDR BSSID;
			uint16 SeqCtl;
			uint8 Category;
			uint8 Action;
			uint8 DialogToken;
			uint8 StatusCodeLsb;
			uint8 StatusCodeMsb;
			uint8 BaParamSetLsb;
			uint8 BaParamSetMsb;
			uint8 BaTimeoutLsb;
			uint8 BaTimeoutMsb;
			uint8 Elts[1];
		} HDR_ADDBA_RESP_ALIGN;

		PACKED struct HDR_CTRL_WRAPPER {
			uint16 FrameCtl;
			uint16 Duration;
			D0_ADDR A1;
			uint16 CarriedFrmCntrl;
			uint32 HtCntrl;
			uint8 CarriedFrame[1];

		} HDR_CTRL_WRAPPER;
	} u;
} D11_HEADER;

#define D11_SIZE_3_HDR		      24
#define D11_SIZE_4_HDR		      30
#define D11_SIZE_QOS_FIELD		  2
#define D11_SIZE_3_QOS_HDR		  26
#define D11_SIZE_4_QOS_HDR		  32
#define D11_SIZE_BLKACK_HDR		 16
#define D11_SIZE_LARGEST_HDR		D11_SIZE_4_QOS_HDR
#define D11_SIZE_BAR_HDR		    16
#define D11_SIZE_BA_HDR		     16
#define D11_SIZE_BAR__BARCTRL	       2
#define D11_SIZE_BAR__BAINFO		2
#define D11_SIZE_BA__BARCTRL		2
#define D11_SIZE_BA__SSC		    2
#define D11_SIZE_BA__BASIC_BITMAP	   128
#define D11_SIZE_BA__COMPRESSED_BITMAP      8
#define D11_SIZE_BA__MULTITID_TIDINFO       2
#define D11_SIZE_BA__MULTITID_BITMAP	8

#define D11_SIZE_CRC			 4
#define D11_MAX_PAYLOAD_SIZE		2304
#define D11_MAX_IV_BYTES		    16

#define D11_STD_WEP_IV		       4
#define D11_STD_WEP_ICV		      4
#define D11_STD_EXT_IV		       4
#define D11_STD_WEP_OVERHEAD		(D11_STD_WEP_IV + D11_STD_WEP_ICV)

#define D11_STD_TKIP_IV		     8
#define D11_STD_TKIP_OVERHEAD	       (D11_STD_TKIP_IV + D11_STD_WEP_ICV)
#define D11_STD_MIC_LENGTH		  8

#define D11_STD_AES_CCMP_HDR_LEN	    8
#define D11_STD_AES_CCMP_MIC_LEN	    8
#define D11_STD_AES_OVERHEAD		(D11_STD_AES_CCMP_HDR_LEN + D11_STD_AES_CCMP_MIC_LEN)

#define D11_STD_WAPI_HDR_LEN		18
#define D11_STD_WAPI_MIC_LEN		16
#define D11_STD_WAPI_OVERHEAD	       (D11_STD_WAPI_HDR_LEN + D11_STD_WAPI_MIC_LEN)

#define D11_SIZE_ACK			10
#define D11_SIZE_RTS			16
#define D11_SIZE_CTS			10
#define D11_SIZE_CFEND		      16
#define D11_SIZE_BEACON_FIXED_EL	    (D11_SIZE_3_HDR+2+2+8)	/* timestamp, beacon interval, capability */
#define D11_SIZE_IE_TIM_MIN		 4
#define D11_SIZE_IE_CF		      6
#define D11_SIZE_IE_IBSS		    2
#define D11_SIZE_ATIM		       (D11_SIZE_3_HDR)
#define D11_SIZE_PS_POLL		    16
/* Auth size does not include the challenge text element */
#define D11_SIZE_AUTH		       (D11_SIZE_3_HDR+6)
/* Assoc size does not include the ssid and rates elements */
#define D11_SIZE_ASSOC_REQ		  (D11_SIZE_3_HDR+4)
#define D11_SIZE_ASSOC_RSP		  (D11_SIZE_3_HDR+6)
#define D11_SIZE_DEAUTH		     (D11_SIZE_3_HDR+2)
#define D11_SIZE_DISASSOC		   (D11_SIZE_3_HDR+2)
#define D11_SIZE_REASSOC_REQ		(D11_SIZE_3_HDR+10)
#define D11_SIZE_REASSOC_RSP		(D11_SIZE_3_HDR+6)
#define D11_SIZE_ADDBA_REQ		  (D11_SIZE_3_HDR+9)
#define D11_SIZE_ADDBA_RSP		  (D11_SIZE_3_HDR+9)
#define D11_SIZE_DELBA_REQ		  (D11_SIZE_3_HDR+6)

#define D11_SIZE_BAR	      (D11_SIZE_BAR_HDR + D11_SIZE_BAR__BARCTRL + D11_SIZE_BAR__BAINFO)

#define D11_SIZE_BA__BASIC	(D11_SIZE_BA_HDR + D11_SIZE_BAR__BARCTRL + D11_SIZE_BA__SSC + D11_SIZE_BA__BASIC_BITMAP)
#define D11_SIZE_BA__COMPRESSED   (D11_SIZE_BA_HDR + D11_SIZE_BAR__BARCTRL + D11_SIZE_BA__SSC + D11_SIZE_BA__COMPRESSED_BITMAP)
#define D11_SIZE_BA__HTI	  (D11_SIZE_BA_HDR + D11_SIZE_BAR__BARCTRL + D11_SIZE_BA__SSC + D11_SIZE_BA__COMPRESSED_BITMAP)
#define D11_SIZE_BA__MULTI_TID(x) (D11_SIZE_BA_HDR + D11_SIZE_BAR__BARCTRL +  ((x)*(D11_SIZE_BA__MULTITID_TIDINFO + D11_SIZE_BA__SSC + D11_SIZE_BA__MULTITID_BITMAP)))

#define D11_SIZE_BLKACK_REQ		 (D11_SIZE_BLKACK_HDR + 4)

#define D11_EXTRACT_SEQ(x)		  ((x) >> 4)
#define D11_EXTRACT_FRAG(x)		 ((x) & 0xF)

/**************************************************************************************/
/* Reason Codes*/
/* = == =========*/
/* Based on Standard Table 7.22 - Reason Codes*/
/**************************************************************************************/
#define D11_REASON_CODE__UNSPECIFIED	       1
#define D11_REASON_CODE__PREV_AUTH_INVALID	 2
#define D11_REASON_CODE__DAUTH_LEAVING_BSS	 3
#define D11_REASON_CODE__DISASSOC_INACTIVITY       4
#define D11_REASON_CODE__DISASSOC_TOO_MANY_STAS    5
#define D11_REASON_CODE__C2_FROM_NONAUTH	   6
#define D11_REASON_CODE__C3_FROM_NONASSOC	  7
#define D11_REASON_CODE__DISASSOC_LEAVING_BSS      8
#define D11_REASON_CODE__REQ_ASSOC_NOT_AUTH	9
#define D11_REASON_CODE__PWRCAP_UNACCEPTABLE      10	/* For 11h */
#define D11_REASON_CODE__SUP_CHAN_UNACCEPTABLE    11	/* For 11h */

/*TODO - other reason codes to be added here*/

#define D11_REASON_CODE__STA_LEAVING	      36
#define D11_REASON_CODE__END_MECHANISM	    37
#define D11_REASON_CODE__NO_MECHANISM_SETUP       38
#define D11_REASON_CODE__TIMEOUT		  39

/*TODO - other reason codes to be added here*/

/**************************************************************************************/

/**************************************************************************************/
/* Status Codes*/
/* = == =========*/
/* Based on Standard Table 7.23 - Status Codes*/
/**************************************************************************************/
#define D11_STATUS_CODE__SUCCESS		   0
#define D11_STATUS_CODE__UNSPECIFIED	       1
#define D11_STATUS_CODE__CANT_SUPPORT_ALL_CAPS    10
#define D11_STATUS_CODE__CANT_CONF_ASSOC	  11
#define D11_STATUS_CODE__ASSOC_DENIED	     12
#define D11_STATUS_CODE__UNSUPPORTED_ALGORITHM    13
#define D11_STATUS_CODE__OUT_OF_ORDER_AUTH	14
#define D11_STATUS_CODE__CHALLENGE_FAILURE	15
#define D11_STATUS_CODE__AUTH_TIMEOUT	     16
#define D11_STATUS_CODE__TOO_MANY_STATIONS	17
#define D11_STATUS_CODE__MISSING_BASIC_RATES      18
/* The following status codes were added in 802.11b */
#define D11_STATUS_CODE__NS_SHORT_PREAMBLE	19
#define D11_STATUS_CODE__NS_PBCC		  20
#define D11_STATUS_CODE__NS_CHANNEL_AGILE	 21
/* The following status codes were added in 802.11h */
#define D11_STATUS_CODE__NO_SPECTRUM_MGMT_CAP     22
#define D11_STATUS_CODE__PWRCAP_UNACCEPTABLE      23
#define D11_STATUS_CODE__SUP_CHAN_UNACCEPTABLE    24

/*TODO - other reason codes to be added here*/

/* The following status codes were added in 802.11e */
#define D11_STATUS_CODE__UNSPECIFIED_QOS_FAILURE  32
#define D11_STATUS_CODE__REQUEST_DECLINED	 37

/*TODO - other reason codes to be added here*/

/**************************************************************************************/

/* Size of CRC (in bytes) */
#define D11_CRC_SIZE			4

/*  Minimum and maximum size of beacon. */
#define D11_MIN_BEACON_SIZE		 49
#define D11_MAX_BEACON_SIZE		 347
/*  Minimum and maximum size of probe response. */
#define D11_MIN_PROBE_RESP_SIZE	     51
#define D11_MAX_PROBE_RESP_SIZE	     91

/* Information element */
typedef struct D11_INFO_ELEMENT_S D11_INFO_ELEMENT;
struct D11_INFO_ELEMENT_S {
	uint8 ElementId;
	uint8 Length;
	uint8 Data;
};

typedef struct D11_WMM_INFO_ELEMENT_S D11_WMM_INFO_ELEMENT;
struct D11_WMM_INFO_ELEMENT_S {
	uint8 ElementId;
	uint8 Length;
	uint8 Oui[3];
	uint8 OuiType;
	uint8 OuiSubType;
	uint8 Version;
	uint8 QosInfo;
};

typedef struct D11_MANAGEMENT_MIC_INFO_ELEMENT_S D11_MANAGEMENT_MIC_INFO_ELEMENT;
struct D11_MANAGEMENT_MIC_INFO_ELEMENT_S {
	uint8 ElementId;
	uint8 Length;
	uint16 KeyID;
	uint8 IPN[6];
	uint8 MIC[8];
};
#define D11_MMIE_KEYID_MASK 0x0FFF

#define D11_MAX_SSID_LENGTH		 32
#define D11_MAX_RATES		       16
#define D11_MIN_FRAG_THRESHOLD	      256
#define D11_MAX_FRAG_THRESHOLD	      2346
#define D11_DEF_FRAG_THRESHOLD	      2346

/* Calculates maximum number of fragments per MSDU/MMPDU */
#define D11_MAX_TX_FRAGS	((D11_MAX_PAYLOAD_SIZE+D11_MIN_FRAG_THRESHOLD-1)/D11_MIN_FRAG_THRESHOLD)

/* Sequence numebr increments from MSDU to MSDU */
#define D11_SEQ_NO_INC		      16

#define D11_RTS_THRESHOLD		   3000	/* was 2347*/
#define D11_MAX_RECEIVE_LIFETIME	    512
#define D11_MAX_TRANSMIT_MSDU_LIFETIME      512
#define D11_SHORT_RETRY_LIMIT	       7
#define D11_LONG_RETRY_LIMIT		4
#define D11_MAX_TX_LIFE_TIME		512
#define D11_EDCA_TABLE_LIFE_TIME	    500
#define D11_MAX_RX_LIFE_TIME		512
#define D11_MEDIUAM_OCCUP_LIM	       100
#define D11_MAX_LISTEN_INTERVAL	     31

/* EDCA Parameter Defintions*/
/* Access Catagory Priorities */
#define D11_NUM_ACIS			4	/* number of ACI based seq Queues*/

#define D11_NUM_ACS			 4
#define D11_PRIORITY_AC_BK		  0	/* Background*/
#define D11_PRIORITY_AC_BE		  1	/* Best Effort*/
#define D11_PRIORITY_AC_VI		  2	/* Video*/
#define D11_PRIORITY_AC_VO		  3	/* Voice*/

/* Access Catagory Indices*/
#define D11_ACI_AC_BE		       0	/* Best Effort*/
#define D11_ACI_AC_BK		       1	/* Background*/
#define D11_ACI_AC_VI		       2	/* Video*/
#define D11_ACI_AC_VO		       3	/* Voice*/

/* User Priorities */
#define D11_NO_UPS			  8

/* Traffic Identifiers */
#define D11_NO_TIDS			 16

#endif				/* _D11_DEFS_H */
#define D11_CATEGORY_11K_RADIO_MEASUREMENT  5
