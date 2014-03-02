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
 * \file umac_interface.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_interface.h
 * \brief
 * Header for WFM Message module. This file defines the data
 * structures which are common between WFM API interface
 * to Miniport Driver (wfm_api.h) and WFM HI Interface to
 * target (umac_messages.h).
 * \ingroup Upper_MAC_Core
 * \date 03/10/08 18:20
 */

#ifndef _UMAC_INTERFACE_H
#define _UMAC_INTERFACE_H
#include "stddefs.h"

#ifndef DOT11K_SUPPORT
#define DOT11K_SUPPORT			1
#endif

#ifndef WAPI_SUPPORT
#define WAPI_SUPPORT			1
#endif

#ifndef MGMT_FRAME_PROTECTION
#define MGMT_FRAME_PROTECTION		1
#endif

#ifndef FT_SUPPORT
#define FT_SUPPORT			1
#endif

#ifndef P2P_MINIAP_SUPPORT
#define P2P_MINIAP_SUPPORT		1
#endif

#ifndef SEND_PROBE
#define SEND_PROBE			1
#endif

#ifndef TX_PER_SUPPORT
#define TX_PER_SUPPORT			1
#endif

#define WFM_MAX_BSSID_INFO_ENTRIES		20
#define WFM_MAC_ADDRESS_SIZE			6
#define WFM_IP_ADDRESS_SIZE			4
#define WFM_PMKID_BKID_SIZE			16
#define WFM_DOT11_COUNTRY_STRING_SIZE		3
#define WFM_MAX_SSID_SIZE			32
#define WFM_MAX_ETHERNET_FRAME_SIZE		1500

#define UMAC_MAX_NUM_COUNTRIES			1
#define UMAC_REG_CLASS_MAP_SIZE			40
#define UMAC_NUM_CHANNEL_TRIPLET		5

#define WFM_MAX_SSIDS				2
#define WFM_MAX_SUPPORTED_CHANNELS		46

#if DOT11K_SUPPORT
#define UMAC_RMM_CAPABILITIES_LEN		5

#define UMAC_MAX_MEASUREMENT_REQ_ELEMENTS_SIZE	7
#endif	/* DOT11K_SUPPORT */

/* Encryption Status */
#define WFM_ENC_STATUS_NO_ENCRYPTION		0
#define WFM_ENC_STATUS_WEP_SUPPORTED		(1<<0)
#define WFM_ENC_STATUS_TKIP_SUPPORTED		(1<<1)
#define WFM_ENC_STATUS_AES_SUPPORTED		(1<<2)

#define WFM_ENC_STATUS_WAPI_SUPPORTED		(1<<3)

#define WFM_ENC_STATUS_MASK_ENC_TYPE		0x7	/*three bits */
#define WFM_ENC_STATUS_MASK_KEY_AVAIL		((uint32) (1<<31))

#define WFM_PRE_AUTH_MASK			(1<<1)

#define UMAC_ENC_NO_ENCRYPTION			0
#define UMAC_ENC_WEP_SUPPORTED			1
#define UMAC_ENC_TKIP_SUPPORTED			3
#define UMAC_ENC_AES_SUPPORTED			7

/* Encryption Key Type */
#define WFM_KEY_TYPE_WEP_DEFAULT		0
#define WFM_KEY_TYPE_WEP_PAIRWISE		1
#define WFM_KEY_TYPE_TKIP_GROUP			2
#define WFM_KEY_TYPE_TKIP_PAIRWISE		3
#define WFM_KEY_TYPE_AES_GROUP			4
#define WFM_KEY_TYPE_AES_PAIRWISE		5

#if P2P_MINIAP_SUPPORT
#define CIPHER_TYPE_WEP40			1
#define CIPHER_TYPE_TKIP			2
#define CIPHER_TYPE_AES				3
#define CIPHER_TYPE_WEP104			5

#endif	/* P2P_MINIAP_SUPPORT */

/* For PsMode */
#define WFM_PS_MODE_DISABLED			0
#define WFM_PS_MODE_ENABLED			1
#define WFM_PM_MODE_STATE_UNDETERMINED		2
#define WFM_PS_MODE_FLAG_FAST_PSM_ENABLE	0x80

#define DEFAULT_DTIM_PERIOD			5
#define WFM_DTIM_PERIOD_FLAG_UNKNOWN		0x80
#define DTIM_PERIOD_1				1

/* For Frame Brusting */
#define FRAME_BRUSTING_DISABLED			0
#define FRAME_BRUSTING_ENABLED			1

/* For OprPwrMode */
#define WFM_OPR_PWR_MODE_ACTIVE			0
#define WFM_OPR_PWR_MODE_DOZE			1
#define WFM_OPR_PWR_MODE_QUIESCENT		2

/* For Scan request */
#define WFM_BSS_LIST_SCAN__DEFAULT_SSID_LEN	2

/* For IBSS configuration */
#define NETWORK_TYPE_11B			0
#define NETWORK_TYPE_11A			1
#define NETWORK_TYPE_11BG			2
#define NETWORK_TYPE_11BGN			3
#define NETWORK_TYPE_11AN			4


#define WSM_QOS_INFO_STA_UAPSD_AC_BK		0x01
#define WSM_QOS_INFO_STA_UAPSD_AC_BE		0x02
#define WSM_QOS_INFO_STA_UAPSD_AC_VI		0x04
#define WSM_QOS_INFO_STA_UAPSD_AC_VO		0x08

#if P2P_MINIAP_SUPPORT

#define WFM_NUM_ACS				4
#define UMAC_P2P_MODE_CLIENT			0
#define UMAC_P2P_MODE_GO			1

/* Device Capability Bitmap */
#define UMAC_DC_SERVICE_DISCOVERY		(1<<0)
#define UMAC_DC_CLIENT_DISCVRBLTY		(1<<1)
#define UMAC_DC_CONCURRENT_OPERATION		(1<<2)
#define UMAC_DC_INFRA_MANAGED			(1<<3)
#define UMAC_DC_DEVICE_LIMIT			(1<<4)
#define UMAC_DC_INVTN_PROCEDURE			(1<<5)

/* Group Capability Bitmap */
#define UMAC_GC_GO				(1<<0)
#define UMAC_GC_PERSISTENT_GROUP		(1<<1)
#define UMAC_GC_GROUP_LIMIT			(1<<2)
#define UMAC_GC_INTRA_BSS_DIST			(1<<3)
#define UMAC_GC_CROSS_CONNECTION		(1<<4)
#define UMAC_GC_PERSIST_RECONNECT		(1<<5)
#define UMAC_GC_GROUP_FORMATION			(1<<6)

#endif	/* P2P_MINIAP_SUPPORT */

#define UMAC_DEFAULT_KEEP_ALIVE_PERIOD		150

/* For AMP */
#define UMAC_DEFAULT_COVERAGE_CLASS 0

typedef enum WFM_DEVICE_OID_E {
	/* gets the MAC address set operation not supported */
	WFM_DEVICE_OID_802_11_STATION_ID = 0x0000,
	/* Gets the country string */
	WFM_DEVICE_OID_802_11_COUNTRY_STRING = 0x0001,
	/* BSS_LIST_SCAN - Scan Request, get operation is not supported */
	WFM_DEVICE_OID_802_11_BSSID_LIST_SCAN = 0x0002,
	/*
	  BSS_CACHE_QUERY - Gets the current stored scan list, There is no set
	  operation  corresponding to this
	*/
	WFM_DEVICE_OID_802_11_BSSID_LIST = 0x0003,
	/*
	  BSS - Set operation specifies the AP/IBSS to join/start Get
	  operation returns the current BSS info. the device has joined
	*/
	WFM_DEVICE_OID_802_11_BSSID = 0x0004,
	/* Windows defined OID's */
	WFM_DEVICE_OID_802_11_ADD_KEY = 0x0005,
	WFM_DEVICE_OID_802_11_AUTHENTICATION_MODE = 0x0006,
	WFM_DEVICE_OID_802_11_ASSOCIATION_INFORMATION = 0x0007,
	WFM_DEVICE_OID_802_11_REMOVE_KEY = 0x0008,
	WFM_DEVICE_OID_802_11_DISASSOCIATE = 0x0009,
	WFM_DEVICE_OID_802_11_RSSI_RCPI = 0x000A,
	WFM_DEVICE_OID_802_11_RSSI_TRIGGER = 0x000B,
	WFM_DEVICE_OID_802_11_MEDIA_STREAM_MODE = 0x000C,
	WFM_DEVICE_OID_802_11_TX_ANTENNA_SELECTED = 0x000D,
	WFM_DEVICE_OID_802_11_RX_ANTENNA_SELECTED = 0x000E,
	WFM_DEVICE_OID_802_11_SUPPORTED_DATA_RATES = 0x000F,
	WFM_DEVICE_OID_802_11_TX_POWER_LEVEL = 0x0010,
	WFM_DEVICE_OID_802_11_NETWORK_TYPE_IN_USE = 0x0011,
	WFM_DEVICE_OID_802_11_INFRASTRUCTURE_MODE = 0x0012,
	WFM_DEVICE_OID_802_11_SSID = 0x0013,
	WFM_DEVICE_OID_802_11_ENCRYPTION_STATUS = 0x0014,

	WFM_DEVICE_OID_802_11_PRIVACY_FILTER = 0x0020,
	WFM_DEVICE_OID_802_11_STATISTICS = 0x0030,
	WFM_DEVICE_OID_802_11_POWER_MODE = 0x0031,
	WFM_DEVICE_OID_802_11_BLOCK_ACK_POLICY = 0x0032,
	WFM_DEVICE_OID_802_11_UPDATE_EPTA_CONFIG_DATA = 0x0033,
	WFM_DEVICE_OID_802_11_SET_AUTO_CALIBRATION_MODE = 0x0034,
	WFM_DEVICE_OID_802_11_CONFIGURE_IBSS = 0x0035,
	WFM_DEVICE_OID_802_11_SET_UAPSD = 0x0036,
	WFM_DEVICE_OID_802_11_MAX_RX_AMSDU_SIZE = 0x0037,
	WFM_DEVICE_OID_802_11_TXOP_LIMIT = 0x0038,
	WFM_DEVICE_OID_802_11_SET_LISTEN_INTERVAL = 0x0039,
	WFM_DEVICE_OID_802_11_DISABLE_BG_SCAN = 0x0040,
	WFM_DEVICE_OID_802_11_BLACK_LIST_ADDR = 0x0041,
	WFM_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE = 0x0042,
	WFM_DEVICE_OID_802_11_SET_HOST_MSG_TYPE_FILTER = 0x0043,
	WFM_DEVICE_OID_802_11_SET_RCPI_RSSI_THRESHOLD = 0x0044,
	WFM_DEVICE_OID_802_11_SET_PMKID_BKID = 0x0045,
	WFM_DEVICE_OID_802_11_SET_BEACON_LOST_COUNT = 0x0046,
	WFM_DEVICE_OID_802_11_TX_FAILURE_THRESHOLD_COUNT = 0x0048,
	WFM_DEVICE_OID_802_11_SET_ETHER_TYPE_FILTER = 0x0049,
	WFM_DEVICE_OID_802_11_SET_UDP_PORT_FILTER = 0x0050,
	WFM_DEVICE_OID_802_11_SET_MAGIC_FRAME_FILTER = 0x0051,
	WFM_DEVICE_OID_802_11_ENABLE_11D_FEATURE = 0x0052,
	WFM_DEVICE_OID_802_11_UPDATE_VENDOR_IE = 0x0053,
	WFM_DEVICE_OID_802_11_SET_ARP_IP_ADDR_FILTER = 0x0054,
#if FT_SUPPORT
	WFM_DEVICE_OID_802_11_MDIE = 0x0055,
	WFM_DEVICE_OID_802_11_FTIE = 0x0056,
	WFM_DEVICE_OID_802_11_FT_AUTHENTICATE = 0x0057,
	WFM_DEVICE_OID_802_11_DELETE_FT_AUTHENTICATION = 0x0058,
#endif	/* FT_SUPPORT */
	WFM_DEVICE_OID_802_11_PTA_LISTEN_INTERVAL = 0x0059,
#if DOT11K_SUPPORT
	WFM_DEVICE_OID_802_11_MEASUREMENT_CAPABILITY = 0x0060,
	WFM_DEVICE_OID_802_11_ENABLE_MEASUREMENTS = 0x0061,
	WFM_DEVICE_OID_802_11_NEIGHBOR_REPORT = 0x0062,
	WFM_DEVICE_OID_802_11_LCI_INFORMATION = 0x0063,
#endif	/* DOT11K_SUPPORT */

	WFM_DEVICE_OID_802_11_CONFIGURE_REGULATORY_DOMAIN = 0x0064,

#if MGMT_FRAME_PROTECTION
	WFM_DEVICE_OID_802_11_PROTECTED_MGMT_POLICY = 0x0065,
	WFM_DEVICE_OID_802_11_SA_QUERY_INTERVAL = 0x0066,
#endif	/* MGMT_FRAME_PROTECTION */
	WFM_DEVICE_OID_802_11_MULTI_TX_CONFIRMATION = 0x0067,
#if P2P_MINIAP_SUPPORT
	UMAC_DEVICE_OID_802_11_USE_P2P = 0x0068,
	UMAC_DEVICE_OID_802_11_DEVICE_DISCOVERY = 0x0069,
	UMAC_DEVICE_OID_802_11_GROUP_FORMATION = 0x0070,
	UMAC_DEVICE_OID_802_11_START_AP = 0x0071,
	UMAC_DEVICE_OID_802_11_INVITATION = 0x0072,
	UMAC_DEVICE_OID_802_11_CONFIGURE_WMM_PARAMS = 0x0073,
	UMAC_DEVICE_OID_802_11_START_GROUP_FORMATION = 0x0074,
	UMAC_DEVICE_OID_802_11_SOCIAL_CHANNELS = 0x0075,
	UMAC_DEVICE_OID_802_11_GET_GROUP_TSC = 0x0076,
#endif	/* P2P_MINIAP_SUPPORT */
	UMAC_DEVICE_OID_802_11_MULTICAST_ADDR_FILTER = 0x0077,
	UMAC_DEVICE_OID_802_11_KEEP_ALIVE_PERIOD = 0x0078,
	UMAC_DEVICE_OID_802_11_WEP_DEFAULT_KEY_ID = 0x0079,
	UMAC_DEVICE_OID_802_11_GET_DEVICE_MAC_ADDRESS = 0x0080,
	UMAC_DEVICE_OID_802_11_DISABLE_ACTIVE_JOIN = 0x0081,
	UMAC_DEVICE_OID_802_11_INTRA_BSS_BRIDGING = 0x82,
	UMAC_DEVICE_OID_802_11_STOP_AP             = 0x0083,
	UMAC_DEVICE_OID_802_11_GET_LINK_SPEED = 0x0084,
	UMAC_DEVICE_OID_802_11_CHANGE_CHANNEL = 0x0085,
	UMAC_DEVICE_OID_802_11_RESTORE_CHANNEL = 0x0086,
	UMAC_DEVICE_OID_802_11_REGISTER_MGMT_FRAME_RX = 0x0087
#if P2P_MINIAP_SUPPORT
	,
	UMAC_DEVICE_OID_802_11_P2P_PS_MODE         = 0x0088,
	UMAC_DEVICE_OID_802_11_SET_NUM_CLIENTS	   = 0x0089
#endif
	,
	UMAC_DEVICE_OID_802_11_GET_LOCAL_AMP_INFO = 0x0090,
	UMAC_DEVICE_OID_802_11_GET_AMP_ASSOC_INFO = 0x0091,
	UMAC_DEVICE_OID_802_11_ALLOW_ACCESS = 0x0092,
	UMAC_DEVICE_OID_802_11_MAC_ADDR_UPDATE = 0x0093,
#if TX_PER_SUPPORT
	UMAC_DEVICE_OID_802_11_GET_TX_STATS = 0x0095,
#endif
	UMAC_DEVICE_OID_802_11_SET_ARP_KEEP_ALIVE_PERIOD = 0x0096
} WFM_DEVICE_OID;

typedef enum WFM_EVENTS_E {
	WFM_EVT_IDLE,
	WFM_EVT_START_COMPLETED,
	WFM_EVT_STOP_COMPLETED,
	WFM_EVT_CONNECTING,
	WFM_EVT_CONNECTED,
	WFM_EVT_CONNECT_FAILED,
	WFM_EVT_DISCONNECTED,
	WFM_EVT_SCAN_COMPLETED,
	WFM_EVT_GENERAL_FAILURE,
	WFM_EVT_TX_RATE_CHANGED,
	WFM_EVT_RECONNECTED,
	WFM_EVT_MICFAILURE,
	WFM_EVT_RADAR,
	WFM_EVT_BSS_LOST,
	WFM_EVT_RCPI_RSSI,
	WFM_EVT_TX_FAILURE_THRESHOLD_EXCEEDED,
	WFM_EVT_BSS_REGAINED,
	WFM_EVT_FT_AUTH_RESPONSE,
	WFM_EVT_NEIGHBOR_REPORT,
#if P2P_MINIAP_SUPPORT
	WFM_EVT_BSS_STARTED,
	WFM_EVT_DEV_DISCVRY_COMPLETED,
	WFM_EVT_GROUP_FORMATION_REQ,
	WFM_EVT_GROUP_FORMATION_SUCCEEDED,
	WFM_EVT_GROUP_FORMATION_FAILED,
	WFM_EVT_INVITATION_REQ,
	WFM_EVT_INVITATION_SUCCEEDED,
	WFM_EVT_INVITATION_FAILED,
#endif	/* P2P_MINIAP_SUPPORT */
	WFM_EVT_BT_ACTIVE,
	WFM_EVT_BT_INACTIVE,
	WFM_EVT_STOP_AP_COMPLETED,
#if P2P_MINIAP_SUPPORT
	WFM_EVT_RX_MGMT_FRAME,
	WFM_EVT_RX_ACTION_FRAME,
#endif	/* P2P_MINIAP_SUPPORT */
	WFM_EVT_DEVICE_UNRESPONSIVE,
	WFM_EVT_MACADDR_ACTION,
	WFM_EVT_CC_STATE_SWITCHING,
	WFM_EVT_CC_STATE_COMPLETED,
	WFM_EVT_CC_STATE_ABORTED,
	/* Add more events here */
	WFM_EVT_MAX
} WFM_EVENT;

typedef struct EVT_DATA_MIC_FAILURE_S {
	uint8 Rsc[6];
	uint8 IsPairwiseKey;
	uint8 KeyIndex;
	uint32 MicFailureCount;
	uint8 PeerAddress[6];
	uint16 Reserved;
} EVT_DATA_MIC_FAILURE;

#if FT_SUPPORT
typedef struct EVT_DATA_FT_AUTH_RESPONSE_S {
	uint8 Bssid[6];
	uint16 Status;
	uint16 ies_len;
	uint8 flags;
	uint8 ies[541];
} EVT_DATA_FT_AUTH_RESPONSE;
#endif	/* FT_SUPPORT */

typedef struct EVT_DATA_ACTION_FRAME_S {
	uint16 Channel;
	uint16 FrameLength;
	uint32 *pFrame;
} EVT_DATA_ACTION_FRAME;

typedef union WFM_EVENT_DATA_U {

	uint32 Rate; /* WFM_EVT_TX_RATE_CHANGED */
	EVT_DATA_MIC_FAILURE MicFailureData; /* WFM_EVT_MICFAILURE */
#if FT_SUPPORT
	EVT_DATA_FT_AUTH_RESPONSE FtAuthRespData; /* WFM_EVT_FT_AUTH_RESPONSE */
#endif	/* FT_SUPPORT */
#if P2P_MINIAP_SUPPORT
	EVT_DATA_ACTION_FRAME ActionFrame; /* WFM_EVT_RX_ACTION_FRAME */
#endif	/* P2P_MINIAP_SUPPORT */
	/*EVT_DATA_RX_MGMT_FRAME	  RxMgmtFrame;*/
} WFM_EVENT_DATA;

typedef enum WFM_STATUS_CODE_E {
	WFM_STATUS_SUCCESS,
	WFM_STATUS_FAILURE,
	WFM_STATUS_PENDING,
	WFM_STATUS_BAD_PARAM,
	WFM_STATUS_OUT_OF_RESOURCES,
	WFM_STATUS_TX_LIFETIME_EXCEEDED,
	WFM_STATUS_RETRY_EXCEEDED,
	WFM_STATUS_LINK_LOST,
	WFM_STATUS_REQ_REJECTED,	/* The request has been rejected by the device, as the device
					   can not process this request in the current state.
					 */
	WFM_STATUS_AUTH_FAILED,
	WFM_STATUS_NO_MORE_DATA,	/* Indicate No More Data Is Available */
	WFM_STATUS_COUNTRY_INFO_MISMATCH,
	WFM_STATUS_COUNTRY_NOT_FOUND_IN_TABLE,
	WFM_STATUS_MAX
} WFM_STATUS_CODE;

typedef enum WFM_REGULATORY_CLASS_E {
	WFM_REG_CLASS_CEPT_1_3,
	WFM_REG_CLASS_FCC_1_4,
	WFM_REG_CLASS_MPHPT_1_6,
	WFM_REG_CLASS_MPHPT_7_11,
	WFM_REG_CLASS_MPHPT_12_15,
	WFM_REG_CLASS_MPHPT_16_20,
	WFM_REG_CLASS_MAX_VALUE
} WFM_REGULATORY_CLASS;

typedef enum WFM_INFRASTRUCTURE_MODE_E {
	WFM_802_IBSS,
	WFM_802_INFRASTRUCTURE,
	WFM_802_AUTO_UNKNOWN,
	WFM_802_INFRASTRUCTURE_MODE_MAX_VALUE
} INFRASTRUCTURE_MODE;

#if P2P_MINIAP_SUPPORT
typedef enum P2P_STATUS_CODE_E {
	P2P_STATUS_SUCCESS,
	P2P_STATUS_PASSED_TO_UL,
	P2P_STATUS_FAILED_INCOMPAT_PARAMS,
	P2P_STATUS_FAILED_LIMIT_REACHED,
	P2P_STATUS_FAILED_INVALID_PARAMS,
	P2P_STATUS_FAILED_UNABLE,
	P2P_STATUS_FAILED_DISTRUPT_BEHAVR,
	P2P_STATUS_FAILED_INFO_UNAVLBLE,
	P2P_STATUS_FAILED_NO_COMM_CHNLS,
	P2P_STATUS_FAILED_UNKNWN_GROUP,
	P2P_STATUS_FAILED_INTENT_15,
	P2P_STATUS_FAILED_INCOMPAT_PROVSNG,
	P2P_STATUS_MAX
} P2P_STATUS_CODE;
#endif				/*P2P_MINIAP_SUPPORT */

#pragma pack(push, 4)

typedef struct WFM_DOT11_COUNTRY_STRING_S	/* 4 byte aligned */
{
	uint8 dot11CountryString[WFM_DOT11_COUNTRY_STRING_SIZE];
	uint8 reserved;
} WFM_DOT11_COUNTRY_STRING;

typedef struct WFM_802_11_SSID_S	/* 4 byte aligned */
{
	uint32 SsidLength;
	uint8 Ssid[WFM_MAX_SSID_SIZE];
} WFM_802_11_SSID;

typedef struct WFM_CHANNELS_S {
	uint32 ChannelNum;
	uint32 MinChannelTime;
	uint32 MaxChannelTime;
	sint32 TxPowerLevel;
} WFM_CHANNELS;

typedef struct WFM_BSS_LIST_SCAN_S	/* 4 byte aligned */
{
	uint32 Flags;		/* Bit    0: auto band  Bit 1: 2.4G band
				   Bit    2: 4.9G band  Bit 3: 5G band
				   Bit    4 - 7 : Regulatory Class
				   Bit    10 - 12: Encryption status
				   Bit    31: no active scan
				   Value  0: stop scan
				 */
	uint8 NumOfProbeRequests;
	uint8 NumOfSSIDs;
	uint8 NumberOfChannels_2_4Ghz;
	uint8 NumberOfChannels_5Ghz;
	uint32 RssiThreshold;
	WFM_802_11_SSID Ssids[WFM_MAX_SSIDS];	/*2 supported */
	WFM_CHANNELS Channels[WFM_MAX_SUPPORTED_CHANNELS];
} WFM_BSS_LIST_SCAN;

typedef struct WFM_BSSID_OID_S {
	uint8 Bssid[6];		/*Bssid to connect */
	uint8 RoamingEnable;	/*If 1 then reassoc request is sent else assoc request is sent */
	uint8 Reserved;		/*Reserved */
} WFM_BSSID_OID;

typedef PACKED struct WFM_BSS_CACHE_INFO_IND_S	/* 4 byte aligned */
{
	uint8 CacheLine;
	uint8 Flags;		/* b0 is set => beacon, clear => probe resp */
	uint8 BssId[WFM_MAC_ADDRESS_SIZE];
	sint8 RSSI;
	uint8 RCPI;
	uint16 ChannelNumber;
	uint32 freqKhz;
/* From Beacon Body - 802.11 frame endianness (little endian) */
	uint64 TimeStamp;
	uint16 BeaconInterval;
	uint16 Capability;
	uint16 IELength;	/* Note: Cpu endianness -(not from 802.11 frame) */
	uint8 IEElements[2];	/* IE elements may follow, if its larger than 2 bytes */
} GCC_PACKED WFM_BSS_CACHE_INFO_IND;

typedef struct WFM_802_3_FRAME_S	/* 4 byte aligned */
{
	uint8 DestinationAddress[WFM_MAC_ADDRESS_SIZE];
	uint8 SourceAddress[WFM_MAC_ADDRESS_SIZE];
	uint16 EthernetType;
	uint8 Data[WFM_MAX_ETHERNET_FRAME_SIZE];
	uint16 Padding;
} WFM_802_3_FRAME;		/* TBD : How to allign this? total size right now is 1514, it needs to be 1516
				   to be alligned for 32 bit boundary
				 */

typedef struct WFM_802_11_BLCK_ACK_POLICY_S {
	uint8 BlockAckTxTidPolicy;
	uint8 Reserved1;
	uint8 BlockAckRxTidPolicy;
	uint8 Reserved2;
} WFM_802_11_BLCK_ACK_POLICY;

typedef union WFM_OID_802_11_KEY_INFO_U {
	struct {
		uint8 PeerAddress[6];
		uint8 Reserved;
		uint8 KeyLength;
		uint8 KeyData[16];
	} WepPairwiseKey;

	struct {
		uint8 KeyId;
		uint8 KeyLength;
		uint8 Reserved[2];
		uint8 KeyData[16];
	} WepGroupKey;

	struct {
		uint8 PeerAddress[6];
		uint8 Reserved[2];
		uint8 TkipKeyData[16];
		uint8 RxMicKey[8];
		uint8 TxMicKey[8];
	} TkipPairwiseKey;

	struct {
		uint8 TkipKeyData[16];
		uint8 RxMicKey[8];
		uint8 KeyId;
		uint8 Reserved[3];
		uint8 RxSequenceCounter[8];
	} TkipGroupKey;

	struct {
		uint8 PeerAddress[6];
		uint8 Reserved[2];
		uint8 AesKeyData[16];
	} AesPairwiseKey;

	struct {
		uint8 AesKeyData[16];
		uint8 KeyId;
		uint8 Reserved[3];
		uint8 RxSequenceCounter[8];
	} AesGroupKey;

	struct {
		uint8 PeerAddress[6];
		uint8 KeyId;
		uint8 Reserved;
		uint8 WapiKeyData[16];
		uint8 MicKeyData[16];
	} WapiPairwiseKey;

	struct {
		uint8 WapiKeyData[16];
		uint8 MicKeyData[16];
		uint8 KeyId;
		uint8 Reserved[3];
	} WapiGroupKey;

	struct {
		uint8 IGTKKeyData[16];
		uint8 KeyId;
		uint8 Reserved[3];
		uint8 IPN[8];	/* actual IPN is only 6 bytes long */
	} IGTKGroupKey;
} WFM_OID_802_11_KEY_INFO;

typedef struct WFM_OID_802_11_KEY_S {
	uint8 Type;
	uint8 EntryIndex;
	uint16 Reserved;
	WFM_OID_802_11_KEY_INFO Key;
} WFM_OID_802_11_KEY;

typedef struct WFM_OID_802_11_REMOVE_KEY_S {
	uint8 EntryIndex;
	uint8 Reserved[3];
} WFM_OID_802_11_REMOVE_KEY;

typedef struct WFM_UPDATE_EPTA_CONFIG_DATA_S {
	uint32 enablePta;
	uint32 bluetoothQuality;
	uint32 subscribeBtEvent;
	uint32 PtaDebugCommand;
	uint32 reserved;
} WFM_UPDATE_EPTA_CONFIG_DATA;

typedef struct WFM_OID_802_11_CONFIGURE_IBSS_S {
	uint8 channelNum;
	uint8 enableWMM;
	uint8 enableWEP;
	uint8 networkTypeInUse;
	uint16 atimWinSize;
	uint16 beaconInterval;
} WFM_OID_802_11_CONFIGURE_IBSS;

typedef struct WFM_OID_802_11_SET_UAPSD_S {
	uint16 uapsdFlags;
	uint16 minAutoTriggerInterval;
	uint16 maxAutoTriggerInterval;
	uint16 autoTriggerStep;
} WFM_OID_802_11_SET_UAPSD;

typedef struct WFM_RCPI_RSSI_THRESHOLD_S {
	uint8 RssiRcpiMode;
	uint8 LowerThreshold;
	uint8 UpperThreshold;
	uint8 RollingAverageCount;
} WFM_RCPI_RSSI_THRESHOLD;

typedef struct WFM_TX_FAILURE_THRESHOLD_COUNT_S {
	uint32 TransmitFailureThresholdCount;
} WFM_TX_FAILURE_THRESHOLD_COUNT;

typedef struct WFM_BEACON_LOST_COUNT_S {
	uint8 BeaconLostCount;
	uint8 Reserved[3];
} WFM_BEACON_LOST_COUNT;

typedef struct WFM_BSSID_INFO_S {
	uint8 bssid[WFM_MAC_ADDRESS_SIZE];
	uint8 pmkidBkid[WFM_PMKID_BKID_SIZE];
	uint8 reserved[2];
} WFM_BSSID_INFO;

typedef struct WFM_BSSID_PMKID_BKID_S {
	uint32 bssidInfoCount;
	WFM_BSSID_INFO bssidInfo[WFM_MAX_BSSID_INFO_ENTRIES];
} WFM_BSSID_PMKID_BKID;

typedef struct UMAC_POWER_MODE_S {
	uint8 PmMode;
	uint8 FastPsmIdlePeriod;
	uint8 ApPsmChangePeriod;
	uint8 reserved;
} UMAC_POWER_MODE;

#if FT_SUPPORT
typedef struct WFM_MDIE_S {
	uint8 mdie_len;
	uint8 mdie[5];
} WFM_MDIE;

typedef struct WFM_FTIE_S {
	uint16 ftie_len;
	uint8 ftie[258];
} WFM_FTIE;

typedef struct WFM_FT_AUTHENTICATE_S {
	uint8 Bssid[6];
	uint8 flags;
	uint8 channel;
} WFM_FT_AUTHENTICATE;

#endif	/* FT_SUPPORT */
#if DOT11K_SUPPORT
/*typedef struct WFM_802_11_NEIGHBOR_REPORT_S
{
    uint32 ssidLength;
    uint8 ssid[WFM_MAX_SSID_SIZE];
}WFM_802_11_NEIGHBOR_REPORT;*/

/*typedef struct UMAC_802_11_LCI_INFORMATION_S
{
    uint32 latitiudeFraction;
    uint32 longitudeFraction;
    uint32 altitudeInteger;
    uint16 longitudeInteger;
    uint8 latitudeResolution;
    uint8 longitudeResolution;
    uint8 altitudeResolution;
    uint8 altitudeFraction;
    uint8 altitudeType;
    uint8 reserved;
}UMAC_802_11_LCI_INFORMATION;*/

typedef struct UMAC_802_11_LCI_INFORMATION_S {
	uint8 elementID;
	uint8 latitiude[5];
	uint8 longitude[5];
	uint8 altitude[5];
	uint8 datum;
	uint8 Reserved[3];
} UMAC_802_11_LCI_INFORMATION;
#endif

typedef struct UMAC_CHANNEL_TRIPLET_S {
	uint16 firstChannel;
	uint16 noOfChannels;
	uint32 channelSpacing;
} UMAC_CHANNEL_TRIPLET;

typedef struct UMAC_REG_CLASS_MAP_S {
	uint32 txPowerLevel;
	uint16 channelOffset;
	uint8 RegClass;
	uint8 noOfTriplets;
	UMAC_CHANNEL_TRIPLET channelTriplet[UMAC_NUM_CHANNEL_TRIPLET];
	/*uint8 ChannelList[20]; */
	/*Later we can add channels also in this */
} UMAC_REG_CLASS_MAP;

typedef struct UMAC_COUNTRY_REG_DETAILS_S {
	uint8 countryString[WFM_DOT11_COUNTRY_STRING_SIZE];
	uint8 noOfRegClass;
	UMAC_REG_CLASS_MAP regClassMap[UMAC_REG_CLASS_MAP_SIZE];
} UMAC_COUNTRY_REG_DETAILS;

typedef struct UMAC_REGULATORY_DOMAIN_TABLE_S {
	UMAC_COUNTRY_REG_DETAILS countryRegDetails;
} UMAC_REGULATORY_DOMAIN_TABLE;

#if P2P_MINIAP_SUPPORT
typedef struct UMAC_P2P_PS_MODE_S {
	uint8 oppPsCTWindow;
	uint8 count;
	uint8 reserved;
	uint8  DtimCount;
	uint32 duration;
	uint32 interval;
	uint32 startTime;
} UMAC_P2P_PS_MODE;

typedef struct UMAC_NUM_CLIENTS_S {
	uint16 NumOfClients;
	uint16 Reserved;
} UMAC_NUM_CLIENTS;

#endif

typedef struct UMAC_LOCAL_AMP_INFO_S {
	uint32 TotalBandwidth;
	uint32 MaxGuaranteedBandwidth;
	uint32 MinLatency;
	uint32 MaxPduSize;
	uint16 PalCapabilities;
	uint16 MaxAmpAssocLength;
} UMAC_LOCAL_AMP_INFO;

typedef struct UMAC_CHANNEL_LIST_S {
	WFM_DOT11_COUNTRY_STRING CountryString;
	uint32 RegulatoryExtension;
	uint32 RegulatoryClass;
	uint32 CoverageClass;
	uint32 FirstChannel;
	uint32 NoOfChannel;
	sint32 MaxTxPower;
} UMAC_CHANNEL_LIST;

typedef struct UMAC_AMP_ASSOC_INFO_S {
	uint8  MacAddress[WFM_MAC_ADDRESS_SIZE];
	uint16 Reserved;
	UMAC_CHANNEL_LIST PreferredChannel;
	UMAC_CHANNEL_LIST ConnectedChannel;
	uint32 PalCapabilities;
} UMAC_AMP_ASSOC_INFO;

typedef struct UMAC_SET_HIDDEN_AP_MODE_S {
	uint8 flag;
	uint8 Reserved[3];
} UMAC_SET_HIDDEN_AP_MODE;
#pragma pack(pop)		/* release pragma */
#endif	/* _UMAC_INTERFACE_H */
