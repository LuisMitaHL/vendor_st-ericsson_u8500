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
 * \file umac_sm.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_sm.h
 * \brief
 * Implementation of WFM UMAC module. This file implements the UMAC State
 * Machine.
 * \ingroup Upper_MAC_Core
 * \date 14/02/08 14:57
 */

#ifndef _UMAC_SM_H
#define _UMAC_SM_H

/******************************************************************************
			    INCLUDE FILES
******************************************************************************/
#include "stddefs.h"
#include "wsm_api.h"
#include "umac_dev_conf.h"
#include "umac_dbg.h"
#include "umac_utils.h"
#include "umac_messages.h"
#include "umac_if.h"
#include "umac_globals.h"
#include "wsm_api.h"

/* UMAC FSM Handler type  */
typedef uint32(*UMAC_FSM_HANDLER) (UMAC_HANDLE UmacHandle, void *pMsg);

/******************************************************************************
			UMAC_INSTANCE.Flags
******************************************************************************/

/*
   The capabilities advertised by the AP in beacon/ prob response
   These are the values given by the HOST to the device with CONNECT request
*/
#define WFM_UMAC_INSTANCE_FLAGS_11N_SUPPORTED_BY_AP		      0x00000001
#define WFM_UMAC_INSTANCE_FLAGS_WMM_SUPPORTED_BY_AP		      0x00000002
#define WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_CTSRTS_PROTECTION	      0x00000004
#define WFM_UMAC_INSTANCE_FLAGS_WPA_IE_SUPPORTED_BY_AP		      0x00000008
#define WFM_UMAC_INSTANCE_FLAGS_RSN_IE_SUPPORTED_BY_AP		      0x00000010
#define WFM_UMAC_INSTANCE_FLAGS_HT_GREENFIELD_MODE_SUPPORTED_BY_AP    0x00000020
#define WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_BARKER_SHORT_PREAMBLE    0x00000040
#define WFM_UMAC_INSTANCE_FLAGS_ERP_IE_USING_SHORT_SLOT		      0x00000080
#define WFM_UMAC_INSTANCE_FLAGS_HT_DISABLE_GREENFIELD_MODE	      0x00000100
#define WFM_UMAC_INSTANCE_FLAGS_HT_TX_STBC_SUPPORTED_BY_AP	      0x00000200
#if WAPI_SUPPORT
#define WFM_UMAC_INSTANCE_FLAGS_WAPI_IE_SUPPORTED_BY_AP		      0x00000400
#endif	/* WAPI_SUPPORT */
#if FT_SUPPORT
#define WFM_UMAC_INSTANCE_FLAGS_FT_SUPPORTED_BY_AP		      0x00000800
#endif	/* FT_SUPPORT */
#define WFM_UMAC_INSTANCE_FLAGS_RRM_MEASUREMENT_SUPPORTED_BY_AP	      0x00000400

/*
  The Most significant byte is reserved to store status after Association
*/
#define WFM_UMAC_INSTANCE_FLAGS_QOS_ASSOCIATION			      0x40000000
#define WFM_UMAC_INSTANCE_FLAGS_WMM_ASSOCIATION			      0x80000000

#define UMAC_QUEUE_ID_2ND_CHAN_FLAG				      0x80

#define UMAC_PACKET_ID_MASK					      0x00000400
#if P2P_MINIAP_SUPPORT
#define UMAC_FWDED_FRAME					      0x10000000
#define UMAC_PACKETID_FLAG_EXT_MGMT				      0x20000000
#define UMAC_PACKETID_PS_BUFFRED_FRAME				      0x40000000
#endif /* P2P_MINIAP_SUPPORT */

#define WFM_UMAC_INSTANCE_FLAGS_BCN_PBRSP_TSF_PRESENT		(1<<24)
#define WFM_UMAC_INSTANCE_TSF_TIMESTAMP_SIZE			8
/******************************************************************************
		     END   UMAC_INSTANCE.Flags
******************************************************************************/

/* Encryption Types */
#define WFM_UMAC_NO_ENC_SUPPORTED   0
#define WFM_UMAC_WEP_ENC_SUPPORTED  1
#define WFM_UMAC_TKIP_ENC_SUPPORTED 2
#define WFM_UMAC_AES_ENC_SUPPORTED  4
#if WAPI_SUPPORT
#define WFM_UMAC_WAPI_ENC_SUPPORTED 8
#endif	/* WAPI_SUPPORT */

/* Rates / Rate Adaptation */
#define WFM_UMAC_NUM_TX_RATES					22
#define WFM_UMAC_RATE_ADAP_THRESHOLD_BAD_HISTORY		(-5)
#define WFM_UMAC_RATE_ADAP_NUM_SUCCESS_FOR_TX_RATE_CHANGE	10
#define WFM_UMAC_RATE_ADAP_NUM_FAILURE_FOR_TX_RATE_CHANGE	(-10)
#define WFM_UMAC_RATE_ADAP_CLEAR_BAD_HISTORY_TIMEOUT	  TIME_MIN_TO_TGT_UTS(15)	/*timeout=15 min */
#define WFM_UMAC_RATE_ADAP_HISTORY_MAX_VALUE		  ((-WFM_UMAC_RATE_ADAP_THRESHOLD_BAD_HISTORY) * 3)
#define WFM_UMAC_RATE_ADAP_HISTORY_MIN_VALUE		  (WFM_UMAC_RATE_ADAP_THRESHOLD_BAD_HISTORY * 3)

#define WFM_UMAC_RATE_FALL_DOWN_AVERAGE		50
#define WFM_UMAC_RATE_MOVE_UP_AVERAGE		89
#define WFM_UMAC_MOVING_AVERAGE			101

/* Timeout for checking for active connection */
/* timeout=5 min */
#define WFM_UMAC_CHECK_FOR_CONNECTION_TIMEOUT		TIME_MIN_TO_TGT_UTS(5)

/* Timeout for checking if unjoin can be called */
/* timeout=1 sec */
#define WFM_UMAC_UNJOIN_TIMEOUT				TIME_MSEC_TO_TGT_UTS(50)

#define WFM_UMAC_CANCEL_REMAIN_ON_CH_TIMEOUT		TIME_MSEC_TO_TGT_UTS(10)

/* Timeout for checking if force unjoin can be called */
/* timeout=5 sec */
#define WFM_UMAC_FORCE_UNJOIN_TIMEOUT			TIME_SEC_TO_TGT_UTS(5)

#define WFM_UMAC_CHECK_FOR_NULL_CNF_TIMEOUT		TIME_SEC_TO_TGT_UTS(1)

#if SEND_PROBE
#define WFM_UMAC_CHECK_FOR_PRBOE_RESP_TIMEOUT		TIME_SEC_TO_TGT_UTS(1)
#endif

#define WFM_UMAC_NUM_TX_QUEUES				4

#define WFM_UMAC_TX_SUCCESS_CREDIT		256
#define WFM_UMAC_RATE_EVALUATION_WINDOW		50
#define WFM_UMAC_AGGRESSIVE_RATE_WINDOW		10
#define WFM_UMAC_EPTA_UNPRFERRED_RATE_SET	0x7

#if P2P_MINIAP_SUPPORT
/*
  Timeout for checking association request after successful authentication when
  we are acting as an AP or GO
*/
/* timeout=1 sec */
#define UMAC_CLIENT_CONNECTION_TIMEOUT		TIME_SEC_TO_TGT_UTS(1)
/* Inactivity Timeout */
/* timeout=3 minutes */
#define UMAC_CLIENT_INACTIVITY_TIMEOUT		TIME_SEC_TO_TGT_UTS(180)
/* timeout=600 msec */
#define UMAC_AP_PS_BUFFER_CLEANUP_TIMER		TIME_MSEC_TO_TGT_UTS(600)

/* Total number of buffers available per STA */
#define AP_NUM_BUFFERS				32

/* Total number of buffers available per STA */
#define NUM_REQUEUE_BUFFERS			8

/* [0,1]; 0: Legacy; 1: Delivery Enabled */
#define AP_BUFF_TYPES				2
#define AP_BUFF_TYPES_LEGACY			0
#define AP_BUFF_TYPES_DLVRY_ENABLED		1

/* We just need to store the start pointer of the packet */
#define MAX_BUFFER_SIZE				4
#define UMAC_TX_CNF_REQUEUE			0x0002
#define WFM_FRAME_REQUEUE			0x0B

#define OPERATING_MODE_STA			0	/* Default Mode */
#define OPERATING_MODE_P2P_DEVICE		1
#define OPERATING_MODE_P2P_CLIENT		2
#define OPERATING_MODE_AP			3
#define OPERATING_MODE_GO			4
#define OPERATING_MODE_PERSISTENT_GO		5

#define STA_PS_STATE_DOZE			0
#define STA_PS_STATE_AWAKE			1

#define MAX_WHITELIST_ENTRIES	16
#define MAX_BLACKLIST_ENTRIES	16
#define MAX_DB_ENTRIES		(MAX_WHITELIST_ENTRIES + MAX_BLACKLIST_ENTRIES)
/* Client Station states */
typedef enum WFM_STA_STATES_E {
	STA_STATE_NONE = 0,
	STA_STATE_AUTHENTICATING,
	STA_STATE_AUTHENTICATED,
	STA_STATE_ASSOCIATING,
	STA_STATE_ASSOCIATED,
	STA_STATE_DEAUTHENTICATING,
	/* Insert new state before this point */
	STA_STATE_MAX
} WFM_STA_STATES;
#else
#define MAX_NUM_CLIENTS_SUPPORTED		1
#endif	/* P2P_MINIAP_SUPPORT */

#define DEFAULT_LINK_ID				0

/* UMAC States */
typedef enum WFM_UMAC_STATES_E {
	UMAC_UNINITIALIZED = 0,	/* 0 */
	UMAC_INITIALIZED,	/* 1 */
#if P2P_MINIAP_SUPPORT
	UMAC_BSS_STARTING,	/* 2 */
	UMAC_BSS_STARTED,	/* 3 */
	UMAC_GROUP_FORMING,	/* 4 */
	UMAC_GROUP_FORMED,	/* 5 */
	UMAC_JOINING,		/* 6 */
	UMAC_JOINED,		/* 7 */
	UMAC_UNJOINING,		/* 8 */
	UMAC_AUTHENTICATING,	/* 9 */
	UMAC_AUTHENTICATED,	/* 10 */
	UMAC_DE_AUTHENTICATING,	/* 11 */
	UMAC_ASSOCIATING,	/* 12 */
	UMAC_ASSOCIATED,	/* 13 */
	UMAC_DISASSOCIATING,	/* 14 */
	UMAC_REASSOCIATING,	/* 15 */
	UMAC_PROCESSING,	/* 16 */
#else	/* P2P_MINIAP_SUPPORT */
	UMAC_JOINING,		/* 2 */
	UMAC_JOINED,		/* 3 */
	UMAC_UNJOINING,		/* 4 */
	UMAC_AUTHENTICATING,	/* 5 */
	UMAC_AUTHENTICATED,	/* 6 */
	UMAC_DE_AUTHENTICATING,	/* 7 */
	UMAC_ASSOCIATING,	/* 8 */
	UMAC_ASSOCIATED,	/* 9 */
	UMAC_DISASSOCIATING,	/* 10 */
	UMAC_REASSOCIATING,	/* 11 */
	UMAC_PROCESSING,	/* 12 */
#endif	/* P2P_MINIAP_SUPPORT */
	/* Insert new state before this point */
	UMAC_MAX_STATE
} WFM_UMAC_STATES;

/*UMAC Sub-States for SCAN*/
typedef enum WFM_UMAC_SCAN_STATES_E {
	/* System is ready to accept a scan request from host */
	UMAC_READY_TO_SCAN,
	/* System is currently servicing a scan request */
	UMAC_SCANNING
} WFM_UMAC_SCAN_STATES;

#if P2P_MINIAP_SUPPORT
/*UMAC Sub-States for FIND*/
typedef enum UMAC_FIND_STATES_E {
	/*System is ready to accept a find request from host */
	UMAC_READY_TO_FIND,
	/*System is currently servicing a find request */
	UMAC_FINDING
} UMAC_FIND_STATES;
#endif	/* P2P_MINIAP_SUPPORT */

/*UMAC Events*/
typedef enum WFM_UMAC_EVENTS_E {
	UMAC_INIT = 0,			/*  0 */
	UMAC_DEINIT,			/*  1 */
	UMAC_START_SCAN,		/*  2 */
	UMAC_STOP_SCAN,			/*  3 */
	UMAC_PROC_SCAN_RSP,		/*  4 */
	UMAC_START_JOIN,		/*  5 */
	UMAC_JOIN_SUCCESS,		/*  6 */
	UMAC_JOIN_FAILED,		/*  7 */
	UMAC_START_AUTH,		/*  8 */
	UMAC_AUTH_SUCCESS,		/*  9 */
	UMAC_AUTH_FAILED,		/* 10 */
	UMAC_DE_AUTHE,			/* 11 */
	UMAC_DE_AUTH_SUCCESS,		/* 12 */
	UMAC_START_ASSOC,		/* 13 */
	UMAC_ASSOC_SUCCESS,		/* 14 */
	UMAC_ASSOC_FAILED,		/* 15 */
	UMAC_DATA_TX,			/* 16 */
	UMAC_DATA_RX,			/* 17 */
	UMAC_IN_BEACON,			/* 18 */
	UMAC_DE_ASSOC,			/* 19 */
	UMAC_DE_ASSOC_SUCCESS,		/* 20 */
	UMAC_DE_ASSOC_REQ,		/* 21 */
	UMAC_DE_AUTH_REQ,		/* 22 */
	UMAC_UNJOIN,			/* 23 */
	UMAC_UNJOIN_SUCCESS,		/* 24 */
	UMAC_SCAN_COMPLETED,		/* 25 */
	UMAC_TX_COMPLETED,		/* 26 */
	UMAC_START_REASSOC,		/* 27 */
	UMAC_REASSOC_SUCCESS,		/* 28 */
	UMAC_REASSOC_FAILED,		/* 29 */
	UMAC_EXT_REQ_TO_WSM,		/* 30 */
	UMAC_START_FT_AUTH,		/* 31 */
	UMAC_FT_AUTH_COMPLT,		/* 32 */
	UMAC_RADIO_MEASUREMENT,		/* 33 */
	UMAC_MEASUREMENT_REPORT,	/* 34 */
	UMAC_LINK_MEASUREMENT,		/* 35 */
	UMAC_SEND_MEASUREMENT_REQ,	/* 36 */
	UMAC_SEND_NEIGHBOR_REPORT_REQ,	/* 37 */
	UMAC_SEND_LINK_MSRMT_REQ,	/* 38 */
	UMAC_SEND_LINK_MSRMT_REPORT,	/* 39 */
	UMAC_SEND_NEIGHBOR_REP_RESP,	/* 40 */
	UMAC_START_SAQUERY,		/* 41 */
	UMAC_SAQUERY_SUCCESS,		/* 42 */
	UMAC_SAQUERY_REQ,		/* 43 */
#if P2P_MINIAP_SUPPORT
	UMAC_START_DEVICE_DISCOVERY,	/* 44 */
	UMAC_START_FIND,		/* 45 */
	UMAC_STOP_FIND,			/* 46 */
	UMAC_FIND_COMPLETED,		/* 47 */
	UMAC_START_GO_NEGO,		/* 48 */
	UMAC_START_GO_NEGO_CNF,		/* 49 */
	UMAC_START_GO_NEGO_RESP,	/* 50 */
	UMAC_GO_NEGO_REQ,		/* 51 */
	UMAC_GO_NEGO_RESP,		/* 52 */
	UMAC_GO_NEGO_SUCCESS,		/* 53 */
	UMAC_GO_NEGO_FAILED,		/* 54 */
	UMAC_START_INVTN,		/* 55 */
	UMAC_INVTN_REQ,			/* 56 */
	UMAC_INVTN_RESP,		/* 57 */
	UMAC_INVTN_SUCCESS,		/* 58 */
	UMAC_INVTN_FAILED,		/* 59 */
	UMAC_AUTH_RESP_SUCCESS,		/* 60 */
	UMAC_AUTH_RESP_FAILED,		/* 61 */
	UMAC_ASSOC_REQ,			/* 62 */
	UMAC_START_AP,			/* 63 */
	UMAC_START_AP_SUCCESS,		/* 64 */
	UMAC_START_AP_FAILED,		/* 65 */
	UMAC_ASYNC_MGMT,		/* 66 */
	UMAC_DEAUTH_STA,		/* 67 */
	UMAC_UNLINK_STA,		/* 68 */
	UMAC_FWD_FRM,			/* 69 */
	UMAC_FWD_FRM_TX_CMPL,		/* 70 */
	UMAC_PS_DLVR_LEGACY_DATA,	/* 71 */
	UMAC_PS_DLVR_WMM_DATA,		/* 72 */
	UMAC_SEND_NULL_DATA,		/* 73 */
	UMAC_UPDATE_TIM_IE,		/* 74 */
	/*Used when a non-associated STA sends data frame to mini AP */
	UMAC_DISCONNECT_STA,		/* 75 */
	UMAC_STOP_AP,                   /* 76 */
	UMAC_TX_MGMT_FRAME,		/* 77 */
	UMAC_RESET_SEQ_NUM_COUNTER,	/* 78 */
	UMAC_DLVR_REQUEUE_DATA,		/* 79 */
#if SEND_PROBE
	UMAC_SEND_PROBE_REQ,		/* 80 */
#endif
	UMAC_UPDATE_ERP_IE,		/* 81 */
	UMAC_MAX_EVT			/* 82 */
#else	/* P2P_MINIAP_SUPPORT */
	UMAC_MAX_EVT			/* 31 */
	    /* Insert new events before this point */
#endif	/* P2P_MINIAP_SUPPORT */
} WFM_UMAC_EVENTS;

typedef struct WFM_QUEUE_PARAMS_S {
	uint8 Reserved[3];
	uint8 AckPolicy;
	uint32 MaxTransmitLifetime;
	uint32 SupportedRateSet;
} WFM_QUEUE_PARAMS;

typedef struct WFM_BSS_LIST_SCAN_MORE_MEM_S {
	WFM_BSS_LIST_SCAN ScanReq;
	/* Space extension for memory for ssid in scan request */
} WFM_BSS_LIST_SCAN_MORE_MEM;

#if DOT11K_SUPPORT
typedef struct DOT11K_MEAS_PARAMS_S {
	uint64 RRMCapability;
	uint8 dialogToken;
	uint8 totalArrayIndex;
	uint16 MeasurementReqElementsLen;
	uint16 noOfRep;
	uint16 totalRequests;	/*Includes no of repetitions also */
	uint8 CurrentArrayIndex;
	uint8 reqUnderProcess;
	uint8 RadioMeasReqUnderProcess;
	uint8 bssidIndex;
	uint32 bssidCount;
	REPORT_OPTIONS CurrentReportOp;
	MEASUREMENT_REQUEST_ELEMENT_IE
	    measurementReqElementsIE[UMAC_MAX_MEASUREMENT_REQ_ELEMENTS_SIZE];
	WSM_HI_MEASURE_CMPL_IND measurementComplInd;
	LINK_MEASUREMENT_REQUEST LinkMeasuremetReq;
	uint8 Addr3[6];
	uint16 MeasPauseTime;
	uint8 MeasPauseAvl;
	uint8 MeasPauseIndex;
	uint8 MeasPauseReq;
	/*transmit Stream */
	uint8 transmitStreamUnderProcess;
	TANSMIT_STREAM_RESULTS txStreamResults;
	uint16 channelOffset;
	uint8 linkMeasReqRCPI;
	uint8 reserved;
	uint32 txPowerLevel;
} DOT11K_MEAS_PARAMS;
#endif	/* DOT11K_SUPPORT */

#if MGMT_FRAME_PROTECTION
typedef struct UMAC_PROTECT_MGMT_FRAME_S {
	uint32 CheckSaQueryInterval;
	uint16 tansactionIdentifer;
	uint8 protectRobustMgmtFrm;
	uint8 isValidSaQueryResp;
} UMAC_PROTECT_MGMT_FRAME;
#endif	/* MGMT_FRAME_PROTECTION */

#if P2P_MINIAP_SUPPORT
typedef struct UMAC_CONNECTION_HANDLE_S {
	UMAC_HANDLE UmacHandle;
	uint8 linkId;
	uint8 reserved[3];
} UMAC_CONNECTION_HANDLE;

typedef struct UMAC_FRAME_BUFFER_S {
	uint8 *buffPtr;
	uint32 timeStamp;
} UMAC_FRAME_BUFFER;
#if P2P_MINIAP_SUPPORT
/* Channel change state */
#define UMAC_CHANNEL_STATE_NONE		0
#define UMAC_CHANNEL_STATE_CHANGING	1
#define UMAC_CHANNEL_STATE_CHANGED	2
#define UMAC_CHANNEL_STATE_RESTORING	3

#define UMAC_SC_CHANNELMODE_ENHANCED	2
#define UMAC_SC_RESV_TX_SLOT		2

#define UMAC_SC_F_DEVICE_SYNC_ON_START	0x01
#define UMAC_SC_F_DEVICE_SYNC_AT_END	0x02
#define UMAC_SC_F_PREAMBLE_TYPE		0x04
#define UMAC_SC_F_FOREIGN_BSS_SYNC	0x10
#define UMAC_SC_F_HOST_TX		0x20
#define UMAC_SC_F_INT_TX_FOLLOWED	0x40
#define UMAC_SC_F_ABORT			0x80

typedef struct UMAC_CHANNEL_CHANGE_DESC_S {
	uint8 ChannelState;
	uint8 ChangedChannel;
	uint8 PreviosChannel;
	uint8 PreviousState;
} UMAC_CHANNEL_CHANGE_DESC;
#endif /* P2P_MINIAP_SUPPORT */


/* Contains the WSC IEs needed in P2P frames */
typedef struct WSC_IE_S {
	uint8 version;
	uint8 numSecDevType;
	uint8 secDevType[UMAC_WSC_SEC_DEV_TYP_IE_LENGTH];
	uint8 primDevType[UMAC_WSC_PRIM_DEV_TYP_IE_LENGTH];
	uint8 configMethod[UMAC_WSC_CONFIG_METHOD_IE_LENGTH];
	uint16 devNameLength;
	uint8 devName[UMAC_WSC_DEV_NAME_IE_LENGTH];
	uint8 devPasswd[UMAC_WSC_DEV_PASSWD_IE_LENGTH];
} WSC_IE;

typedef struct UMAC_STA_DB_S {
	uint8 macAddr[WFM_MAC_ADDRESS_SIZE];
	uint8 state;
	uint8 isWMMEnabled;
	uint8 currentRateIndex;
	uint8 PsState;
	uint8 linkId;
	/* Device Capablity Bitmap */
	uint8 p2PDevCap;
	uint8 Inactive;
	/* b6-b5: Max SP length; b3:b0 UAPSD flags(BE, BK, VI, VO) */
	uint8 qosInfo;
	uint8 pendingFrames;
	uint8 sendDisconnEvent;
	uint8 numPendingFrames[AP_BUFF_TYPES];
	uint16 capabilities;
	uint16 listenInterval;
	uint16 AssocRespStatusCode;
	uint32 get[AP_BUFF_TYPES];
	uint32 put[AP_BUFF_TYPES];
	uint32 allSupportedRates; /*All supported rates by STA in WSM format*/
	/* Statistics */
	uint32 numTxedPackets;
	uint32 numRxedPackets;
	UMAC_FRAME_BUFFER buffer[AP_BUFF_TYPES][AP_NUM_BUFFERS];
	HI_MSG_HDR hiMsgHdr;
	UMAC_CONNECTION_HANDLE UmacConHandle;
	uint8 currentAPAddress[WFM_MAC_ADDRESS_SIZE];
	uint16 capabilityResp;
	uint16 AssocReqIELen;
	uint16 AssocRespIELen;
	uint8 AssocReqIE[WFM_ASSOC_IE_MAX_SIZE];
	uint8 AssocRspIE[WFM_ASSOC_IE_MAX_SIZE];
	uint32 ResetReqSent;
	uint8 Is11bUser;
	uint8 IsLongPreamble;
	uint8 reserved[2];
} UMAC_STA_DB;
#endif	/* P2P_MINIAP_SUPPORT */

typedef struct UMAC_BUFF_REQUEUE_FRMS_S {
	uint32 get;
	uint32 put;
	uint8 numPendingFrames;
	uint8 reserved[3];
	UMAC_FRAME_BUFFER frmBuffPtr[NUM_REQUEUE_BUFFERS];
} UMAC_BUFF_REQUEUE_FRMS;

/* Data Structure for keeping UMAC State Information */
typedef struct WFM_UMAC_INSTANCE_S {
	/*Globals */
	UMAC_GLOBALS gVars;
	UMAC_UPDATE_VENDOR_IE VendorIe;
	uint16 UmacCurrentState;
	uint16 UmacPreviousState;
	uint16 UmacScanState;	/*Scan state */
	uint8 UmacTxblkackpolicy;
	uint8 UmacRxblkackpolicy;
	uint8 MacAddress[6];
	/* Association Information */
	/* Stored as 802.11 frame endianness (little endian) */
	uint16 CapabilitiesReq;
	/* Remove this and use bssInfo (BSSID)*/
	/* this is the BSSID of the AP with which we are assocated */
	uint8 RemoteMacAdd[6];
	uint8 CurrentAPAdd[6];
	uint8 PMKIDSent;
	uint8 updateTimIeInProgress;
	/* 802.11 frame endianness (little endian) */
	uint16 AssociationId;
	uint32 SsidLength;
	uint8 Ssid[32];
	sint32 MinPowerLevel;
	sint32 MaxPowerLevel;
	uint32 setAutoCalibrationMode;
	/* Rates */
	uint32 OurAllSupportedRates_Abgn;
	uint32 OurAllBasicSupportedRates_Abgn;
	uint32 AllSupportedRates_WithAP_Abgn;
	uint32 AllBasicSupportedRates_WithAP_Abgn;
	uint32 SupportedRates_WithAP_Abg;
	uint32 BasicSupportedRates_WithAP_Abg;
	/*Bitmask for operational rates for current mode (a or b or g or n) */
	uint32 CurrentModeOpRates;
	/*Current Rate Index */
	uint32 CurrentRateIndex;
	uint32 ErpDssCckRates;
	uint32 ErpOfdmRates;
	uint32 HtOfdmRates;
	/*Used in rate fallback and rate recovery */
	uint32 TxRatesBitmap[MAX_NUM_CLIENTS_SUPPORTED];
	uint32 TxRetryRateBitMap[MAX_NUM_CLIENTS_SUPPORTED];
	/* WSM HtTxParameters - use greenfield mode etc */
	uint32 WfmHtTxParameters;
	uint32 dot11InfraStructureMode;
	/* 802.11 frame endianness (little endian) */
	uint16 AssocStatusCode;
	/* 802.11 frame endianness (little endian) */
	uint16 AssocListenInterval;
	/* 802.11 frame endianness (little endian) */
	uint16 CapabilitiesRsp;
	/* DTIM period from TIM information element */
	uint8 DTIMPeriod;
	uint8 sendDisassoc;
	uint32 Flags;		/* See WFM_UMAC_INSTANCE_FLAGS_11N_SUPPORTED etc
				   B0  - 11N supported
				   B1  - Remote Device supports WMM
				   B2  - Protection Enabled
				   B3  - WPA IE's present in the Beacons
					 which needs to be included in
					 Association Request.
				   B4  - RSN IE is present
				   B5  - Greenfield mode supported by AP
				   B6  - Device is using Barker short preamble
					 presently
				   B7  - Device is using Barker short slot
					 presently
				   B8  - Device is using Barker greenfield mode
					 for tx presently

				   B30 - Set : QoS Association
					 Not Set : Legacy Association

				   B31 - Set : WMM Association
					 Not set : Legacy Association
				 */

	/* Association Req/Resp IE */
	/*
	   These buffers are for storing the variable length Information
	   Elements which are part of Association Request and Association
	   Response. Host will need this ASSOCIATION_INFORMATION at some point.
	 */
	uint16 AssocReqIELen;
	uint16 AssocRespIELen;
	uint8 AssocReqIE[WFM_ASSOC_IE_MAX_SIZE];
	uint8 AssocRspIE[WFM_ASSOC_IE_MAX_SIZE];
	uint16 NumTxBufPending[MAX_NUM_CLIENTS_SUPPORTED];
	uint16 NumRxBufPending[MAX_NUM_CLIENTS_SUPPORTED];
	uint8 EdcaParamSetUpdateCount;
	uint8 Band;
	uint16 Channelnumber;
	uint8 enableWMM;
	uint8 enableWEP;
	uint16 atimWinSize;
	uint16 beaconInterval;
	uint8 networkTypeInUse;
	uint8 isCoalescing;
	uint32 writeMibPS;
	sint32 PowerLevel;
	/* Rate Adaptation */
	/* History of rates at which tx is successful : Rate Adaptation */
	sint32 RateAdapHistoryTxRate[WFM_UMAC_NUM_TX_RATES];
	/*Tx Success Failure count */
	sint16 RateAdapTxSuccessFailCount;
	/* To check whether sufficient data has been collected */
	uint16 statisticCount[MAX_NUM_CLIENTS_SUPPORTED];
	uint8 mod;
	uint8 isNullDataFrame;
	uint8 statusNullDataFrameCnf;
#if SEND_PROBE
	uint8 isProbeReqFrame;
	uint8 isProbeRespRcvd;
	uint8 reserved2[2];
#endif
	uint8 updateERPIeInProgress;
	/* Statistic average on which rate selection decision is taken */
	uint16 statisticAvg[MAX_NUM_CLIENTS_SUPPORTED];
	/* Rx RSSI */
	sint8 RxRSSI;
	uint8 JoinPending;
	/* Check for connection being active */
	uint32 CheckForConnectionDataRxCtr;
	uint32 CheckForConnectionDataTxCtr;
	uint32 MgMtTxCnfmCounter;
	uint32 MicFailureCount;
	/* Mib Buffer */
	uint32
	    MibBuf[SIZE_PADDED_TO_MAKE_4_BYTES_ALIGNED
		   (sizeof(WSM_MIB_TEMPLATE_FRAME)) / 4];

	/*
	  Result from WSM is copied here as the memory will be lost when
	  LMACtoUMAC_ScanComplete returns
	*/
	SCAN_COMPLETE ScanCompleteResult;
	/*
	  Result from WSM is copied here as the memory will be lost when
	  LMACtoUMAC_JoinComplete returns
	*/
	JOIN_CONFIRM JoinConfirmResult;

	WFM_QUEUE_PARAMS TxQueueParam[WFM_UMAC_NUM_TX_QUEUES];

	WFM_BSS_LIST_SCAN_MORE_MEM ScanReqFromHost;
	uint8 StartScanInNextBand;

	/*
	   1 if this is an internal scan initiated by UMAC , 0 otherwise
	   if this variable is 1, InternalScanMsg should have some values
	*/
	uint8 IsInternalScan;
	uint8 IsInternalScanSuccess;
	/*IsBgScan-> 1 Background scan -> 0  ForeGround Scan */
	uint8 IsBgScan;
	uint8 isArpFilterEnabled;
	uint32
	    InternalScanMsg[CONVERT_BYTE_SIZE_TO_WORD_SIZE
			    (WFM_MAX_INTERNAL_SCAN_MSG_SIZE)];
	/*
	  This buffer can be removed and insted JoinMsg can be used insted
	  of this
	*/
	uint32 bssInfo[CONVERT_BYTE_SIZE_TO_WORD_SIZE(1024)];
	WFM_OID_802_11_STATISTICS statisticsInfo;
	uint32 RecvData[1032 / 4];
	void *pDriverInfo;
	sint32 TxPowerLevel;

#if P2P_MINIAP_SUPPORT
	UMAC_OID_802_11_BSS_CONFIGURATION dot11BssConf;
	UMAC_OID_P2P_GROUP_FORMATION p2pGrpFormtn;
	UMAC_OID_P2P_DEVICE_DISCOVERY p2pDevDiscvry;
	UMAC_OID_P2P_INVITATION p2pInvtn;
	UMAC_OID_802_11_CONFIG_WMM_PARAMS dot11ConfigWMMparams;
	UMAC_OID_802_11_GET_GROUP_TSC groupTSC;
	UMAC_P2P_GROUP_CONFIGURATION p2pGroupConfig;
	UMAC_OID_P2P_START_GROUP_FORMATION p2pStartGrpFormatn;
	UMAC_OID_P2P_SOCIAL_CHANNEL socialChannels;
	/* Operating mode of UMAC. Either STA or AP */
	uint8 operatingMode;
	uint8 dialogToken;
	UMAC_STA_DB sta[MAX_NUM_CLIENTS_SUPPORTED];
	uint8 bitmapCtrl;
	uint8 virtualBitmap;
	uint8 findCompleteStatus;
	uint8 UpdateNewTIMInfo;
	uint8 deviceDiscovery;
	WSC_IE wscIE;
	UMAC_TIM_IE TIMparams;
	UMAC_CHANNEL_CHANGE_DESC ChannelChange;
	uint32 PendingSusResTxInd;
	UMAC_SUSPEND_RESUME_TX_IND SuspendResumeTxInd;
	uint16 PsBuffDataCount;
	uint16 PsNullFrameStatus;
	UMAC_PEER_DB_MINIAP peerAllowDB[32];
	uint8 miniap_whitelistcount;
	uint8 miniap_blacklistcount;
	uint32 autoChanSel;
#endif	/* P2P_MINIAP_SUPPORT */
	uint8 Reserved4[3];
	uint8 linkId;


	uint32 txFailureRecvd;
	uint32 RxRcpi;
	uint8 RoamingEnabled;
	uint8 BssLostEventRcvd;
	uint8 BssRegainedEventRcvd;
	uint8 RssiRcpiThresholdEventRcvd;
	uint8 TxFailureEventRcvd;
	uint8 CheckGFProblem;
	uint8 RxPacketCnt;
	WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED EdcaParam;
	uint8 QosPrio2Queue[8];
	uint8 PrioDowngradeMap[8];
	uint8 AC2DilQ[NUM_PRIO_QUEUE];
	uint8 WriteEdcaParam;
	uint8 IsFirmwareBssParamSet;
	uint16 dtimSkippingCount;
	uint8 WriteJoinModeBasicRateSet;
	uint8 Reserved5[2];
	uint8 UnjoinTimerCount;

#if MGMT_FRAME_PROTECTION
	uint32 hostMfpPolicy;
	uint32 bssInfoMfpPolicy;
	UMAC_PROTECT_MGMT_FRAME protectMgmtFrame;
	UMAC_SA_QUERY_INTERVAL saQueryInterval;
#endif	/* MGMT_FRAME_PROTECTION */

#if DOT11K_SUPPORT
	DOT11K_MEAS_PARAMS dot11k_Meas_Params;
#endif	/* DOT11K_SUPPORT */

#if FT_SUPPORT
	uint8 FtFlags;		/* Bit 0(0x01): over the DS auth ongoing
				   Bit 1(0x02): over the air auth ongoing
				   Bit 2(0x04): leaving current BSS for auth
				   Bit 3(0x08): FT reassoc ongoing
				   Bit 4(0x10): Joining new BSS while doing FT
				   auth over air.
				   Bit 7(0x80): Initial MD association Done.
				 */
	uint8 Reserved3[3];
	WFM_BSS_INFO FtTargetBss;
	EVT_DATA_FT_AUTH_RESPONSE FtResp;
#endif	/* FT_SUPPORT */

	uint8 PtaInterval;
	uint8 UmacFindState;
	uint8 MgtTxRateIndex;
	uint8 ErpInfo;
	uint32 nullFrmPktId;
	uint32 eventIndicationData;
	uint32 extendedUnJoinTimer;
	uint32 ResetReaminch;
	uint32 p2pPsSet;
	uint8  bss_count[14];
#if TX_PER_SUPPORT
	UMAC_TX_STATS	TxStats;
#endif
	uint8 IsPmModeQueued;
	UMAC_BUFF_REQUEUE_FRMS BuffRequeueFrms;
	uint32 SwitchChannelReq;
	uint8 ErpInformation;
	uint8 ErpInfoPrevious;
	uint8 ErpOLBC; /* Overlapping Legacy BSS Condition */
	uint8 ErpUseCtsToSelf;
	uint32 ErpIeToBeUpdated;
	UMAC_NEIGHBOR_BSS_DB_MINIAP neighborBssDB[MAXIMUM_NUMBER_OF_NEIGHBORING_BSS];
} WFM_UMAC_INSTANCE;

#if FT_SUPPORT
#define     UMAC_FT_FLAG_AUTH_OVER_DS_BIT       (1<<0)
#define     UMAC_FT_FLAG_AUTH_OVER_AIR_BIT      (1<<1)
#define     UMAC_FT_FLAG_AUTH_LEAVE_BSS_BIT     (1<<2)
#define     UMAC_FT_FLAG_AUTH_JOIN_BSS_BIT      (1<<3)
#define     UMAC_FT_FLAG_AUTH_JOINBACK_BSS_BIT  (1<<4)
#define     UMAC_FT_FLAG_REASSOCIATING_BIT      (1<<5)
#define     UMAC_FT_FLAG_INIT_MD_ASSOC_DONE     (1<<7)
#endif	/* FT_SUPPORT */

#define    UMAC_SET_DUAL_CTS_PROTECTION			(1<<0)
#define    UMAC_SET_NON_GREEN_FIELD_CAPABLE_STA		(1<<1)
#define    UMAC_SET_LSIG_TXOP_PROTECTION		(1<<4)
#define    UMAC_SET_L_LENGTH_PROTECTION			(1<<5)
#define    UMAC_NO_PROT_MODE				0x00000000
#define    UMAC_NON_MEMBER_PROT_MODE			0x00000004
#define    UMAC_20_MHZ_PROT_MODE			0x00000008
#define    UMAC_NON_HT_MIXED_PROT_MODE			0x0000000C

#define UMAC_SET_STATE_PROCESSING(x)					\
	do {								\
		((WFM_UMAC_INSTANCE *)x)->UmacPreviousState  = ((WFM_UMAC_INSTANCE *)x)->UmacCurrentState ; \
		((WFM_UMAC_INSTANCE *)x)->UmacCurrentState   = UMAC_PROCESSING ; \
	} while (0)


#define UMAC_RESTORE_PREVIOUS_STATE(x)					\
	do {								\
		if (((WFM_UMAC_INSTANCE *)x)->UmacCurrentState == UMAC_PROCESSING) {   \
			((WFM_UMAC_INSTANCE *)x)->UmacCurrentState  = ((WFM_UMAC_INSTANCE *)x)->UmacPreviousState; \
	}								\
	UMAC_MOVE_PENDING_MSGS_TO_ACTIVE_Q(x);				\
	} while (0)

#define UMAC_SET_STATE(x, state)					\
	do { \
		((WFM_UMAC_INSTANCE *)x)->UmacPreviousState  = ((WFM_UMAC_INSTANCE *)x)->UmacCurrentState  ; \
		((WFM_UMAC_INSTANCE *)x)->UmacCurrentState  = state ;	\
	} while (0)

#if P2P_MINIAP_SUPPORT
#define UMAC_INSERT_ATTRIB(x, ATTRIB, length)				\
	do { \
		*x++ = ATTRIB;						\
		*x++ = length & 0x00FF;					\
		*x++ = (length & 0xFF00)>>8 ;				\
	} while (0)

/* WSC IEs are in Big Endian format */
#define UMAC_INSERT_WSC_IE(x, IE, length)	\
	do {					\
		*x++ = 0;			\
		*x++ = (IE  & 0xFF00) >> 8;	\
		*x++ = IE  & 0x00FF;		\
		*x++ = (length & 0xFF00) >> 8;	\
		*x++ = length & 0x00FF;		\
	} while (0)

#define UMAC_STA_UPDATE_STATE(x, linkId, STATE)				\
	((WFM_UMAC_INSTANCE *)x)->sta[linkId].state  = STATE;

#define UMAC_CHANGE_STA_PS_STATE(x, linkId, STATE)			\
((WFM_UMAC_INSTANCE *)x)->sta[linkId].PsState  = STATE;

/* Gets link Id based on the MAC address */
#define UMAC_GET_LINK_ID(x, txMacAddr, linkId)				\
	{									\
		uint8   i;							\
		WFM_UMAC_INSTANCE  *pUmacInstance  = (WFM_UMAC_INSTANCE *)x ;	\
		linkId = 0;							\
		for (i = 1; i < pUmacInstance->gVars.p.maxNumClientSupport; i++) {		\
			if (OS_MemoryEqual((void *)&pUmacInstance->sta[i].macAddr[0], WFM_MAC_ADDRESS_SIZE,  \
			(void *)&txMacAddr[0], WFM_MAC_ADDRESS_SIZE)) {		\
				linkId = i;					\
				break;						\
			}							\
		}								\
	}									\

#define UMAC_STORE_DEV_IF_TX_DESC(x, linkId, devIfTxReq)		\
do {									\
	uint8  i      = 0 ;						\
	uint8  flag   = 1 ;						\
	WFM_UMAC_INSTANCE  *pUmacInstance  = (WFM_UMAC_INSTANCE *)x ;   \
	for (i = 0; i < AP_PS_BUFFER_SIZE; i++) {			\
		if (pUmacInstance->sta[linkId].devIfTxData[i] == NULL) {       \
			pUmacInstance->sta[linkId].devIfTxData[i] = devIfTxReq; \
			pUmacInstance->sta[linkId].numPendingFrames++  ;	\
			flag  = 0 ;					\
			break ;						\
		}							\
	}								\
	if (flag) {							\
		LOG_EVENT(DBG_WFM_ERROR,				\
				"ERROR: UMAC_STORE_DEV_IF_TX_DESC(): Not Enough Memory to store devIfTxReq\n"); \
		OS_ASSERT(0)  ;						\
	}								\
} while (0)	/* end UMAC_STORE_DEV_IF_TX_DESC() */
#endif	/* P2P_MINIAP_SUPPORT */

/*
  Packet Id Generator, macro is used to speed up execution
*/

#define UMAC_GET_PACKET_ID(x, y)						\
	{									\
		uint32  Get;							\
		OS_LOCK(((WFM_UMAC_INSTANCE *)x)->gVars.p.PacketIdPoolLock) ;	\
		Get = ((WFM_UMAC_INSTANCE *)x)->gVars.PktIdGet ;		\
		if ((((WFM_UMAC_INSTANCE *)x)->gVars.PktIdPut - Get) > 0) {	\
			y = ((WFM_UMAC_INSTANCE *)x)->gVars.PacketIdPool[Get & (CFG_HI_NUM_REQS - 1)]; \
			Get++ ;							\
			((WFM_UMAC_INSTANCE *)x)->gVars.PktIdGet = Get ;	\
		}								\
		else {								\
		    y = CFG_HI_NUM_REQS;					\
		}								\
		OS_UNLOCK(((WFM_UMAC_INSTANCE *)x)->gVars.p.PacketIdPoolLock) ; \
	}


#if P2P_MINIAP_SUPPORT
/* Retrives link id from the HI msg ID*/
#define UMAC_RETRIEVE_LINK_ID(x, y)					\
{									\
	y = (uint8)((x & UMAC_HI_MSG_LINK_ID)>>6);			\
}
#define UMAC_PS_STORE_HDR(x, linkId, pDevIfTxReq)					     \
{									\
	uint32 put;							\
	OS_LOCK(((WFM_UMAC_INSTANCE *)x)->gVars.p.ApPsTxBuffPoolLock[linkId]) ;\
	put = ((WFM_UMAC_INSTANCE *)x)->sta[linkId].put ;		\
	OS_ASSERT((put - ((WFM_UMAC_INSTANCE *)x)->sta[linkId].get) <=	\
		  AP_NUM_BUFFERS);					\
	OS_MemoryCopy(((WFM_UMAC_INSTANCE *)x)->sta[linkId].buffer[put],     \
		       pDevIfTxReq,					\
		       sizeof(UMAC_DEVIF_TX_DATA) + sizeof(UMAC_TX_DESC)); \
	put++ ;								\
	((WFM_UMAC_INSTANCE *)x)->gVars.ApPktIdPut  = put ;		\
	OS_UNLOCK(pUmacInstance->gVars.p.ApPsTxBuffPoolLock[linkId]) ;\
}

#define UMAC_GET_PACKET_ID_AP(x, y)						\
	{									\
		uint32  Get, Put;						\
		OS_LOCK(((WFM_UMAC_INSTANCE *)x)->gVars.p.ApPacketIdPoolLock) ; \
		Get = ((WFM_UMAC_INSTANCE *)x)->gVars.ApPktIdGet ;		\
		Put = ((WFM_UMAC_INSTANCE *)x)->gVars.ApPktIdPut;		\
		if ((Put - Get) > 0) {						\
			y = (((WFM_UMAC_INSTANCE *)x)->gVars.ApPacketIdPool[Get & (UMAC_NUM_AP_INTERNAL_TX_BUFFERS - 1)]) | UMAC_FWDED_FRAME; \
			Get++ ;							\
			((WFM_UMAC_INSTANCE *)x)->gVars.ApPktIdGet = Get ;	\
		}								\
		else {								\
		    y = UMAC_NUM_AP_INTERNAL_TX_BUFFERS  ;			\
		}								\
		OS_UNLOCK(((WFM_UMAC_INSTANCE *)x)->gVars.p.ApPacketIdPoolLock) ; \
	}

#define UMAC_RELEASE_PACKET_ID_AP(x, y)						\
	{									\
		uint32 Put, Get;							\
		OS_LOCK(((WFM_UMAC_INSTANCE *)x)->gVars.p.ApPacketIdPoolLock) ; \
		Put = ((WFM_UMAC_INSTANCE *)x)->gVars.ApPktIdPut ;		\
		Get = ((WFM_UMAC_INSTANCE *)x)->gVars.ApPktIdGet ;		\
		OS_ASSERT((Put - Get) <= \
			  UMAC_NUM_AP_INTERNAL_TX_BUFFERS);			\
		((WFM_UMAC_INSTANCE *)x)->gVars.ApPacketIdPool[Put & (UMAC_NUM_AP_INTERNAL_TX_BUFFERS - 1)] \
		    = y & (UMAC_NUM_AP_INTERNAL_TX_BUFFERS - 1);						\
		Put++ ;								\
		((WFM_UMAC_INSTANCE *)x)->gVars.ApPktIdPut  = Put;		\
		OS_UNLOCK(((WFM_UMAC_INSTANCE *)x)->gVars.p.ApPacketIdPoolLock) ; \
	}
#endif	/* P2P_MINIAP_SUPPORT */

#define UMAC_RELEASE_PACKET_ID(x, y)					\
	{									\
		uint32 Put;							\
		OS_LOCK(((WFM_UMAC_INSTANCE *)x)->gVars.p.PacketIdPoolLock) ;	\
		Put = ((WFM_UMAC_INSTANCE *)x)->gVars.PktIdPut ;		\
		OS_ASSERT((Put - ((WFM_UMAC_INSTANCE *)x)->gVars.PktIdGet) <=    \
			  CFG_HI_NUM_REQS);				    \
		((WFM_UMAC_INSTANCE *)x)->gVars.PacketIdPool[Put & (CFG_HI_NUM_REQS - 1)] \
		= y & (CFG_HI_NUM_REQS - 1)  ;						      \
		Put++ ;							 \
		((WFM_UMAC_INSTANCE *)x)->gVars.PktIdPut  = Put ;	       \
		OS_UNLOCK(((WFM_UMAC_INSTANCE *)x)->gVars.p.PacketIdPoolLock) ; \
	}

void WFM_UMAC_Init(void);

#define WFM_UMAC_EXEC_STATE_MACHINE(UmacHandle, WfmEvent, pMsg)	  \
	UMAC_StateEventTable[((WFM_UMAC_INSTANCE *)UmacHandle)->UmacCurrentState][WfmEvent](UmacHandle, pMsg)

uint32 UMAC_ProcPostAssocIE(UMAC_HANDLE UmacHandle, void *pMsg);

#endif /*_UMAC_SM_H */
