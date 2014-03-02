/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
 * \addtogroup Upper_MAC_Interface
 * \brief
 *
 */
/**
 * \file UMI_Api.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_Api.h
 * \brief
 * This is the header file for UMI API module. This file implements the
 * interface as described in WLAN_Host_UMAC_API.doc
 * \ingroup Upper_MAC_Interface
 * \date 19/12/08 11:37
 */

#ifndef _UMI_API_H
#define _UMI_API_H

#include "UMI_OsIf.h"

#define UMI_TX_POWER_LEVEL_CONTROLED_BY_DEVICE  0x00

#define ETH_HEADER_SIZE		14
#define ETH_MAX_DATA_SIZE	1500
#define ETH_MAX_PACKET_SIZE	(ETH_HEADER_SIZE + ETH_MAX_DATA_SIZE)
#define ETH_MIN_PACKET_SIZE	60

#define SCAN_INFO_BEACON			1
#define SCAN_INFO_PROBE_RESP			0

#define LMAC_HI_MSG_SIZE			24
#define MAX_802_11_HDR_SIZE			30
#define SNAP_HDR_SIZE				8
#define ENCRYPTION_HDR_SIZE			18
#define UMAC_INTERNAL_HDR_SIZE			36
#define PADDING					2

/* If OS does not allow extra memory before 802.3 frame */
#define EXTRA_MEMORY_SIZE	   (UMAC_INTERNAL_HDR_SIZE + LMAC_HI_MSG_SIZE \
				     + MAX_802_11_HDR_SIZE + SNAP_HDR_SIZE \
				     + ENCRYPTION_HDR_SIZE + PADDING \
				     - ETH_HEADER_SIZE)

#define UMI_MAC_ADDRESS_SIZE			6

#define UMI_PMKID_BKID_SIZE			16
#define UMI_MAX_BSSID_INFO_ENTRIES		20

#define UMI_DOT11_COUNTRY_STRING_SIZE		3
#define UMI_MAX_SSIDS				2
#define UMI_MAX_SSID_SIZE			32
#define UMI_MAX_ETHERNET_FRAME_SIZE		1500

#define UMI_MAX_NUM_COUNTRIES			5
#define UMI_REG_CLASS_MAP_SIZE			40
#define UMI_NUM_CHANNEL_TRIPLET			5

/* Encryption Status */
#define UMI_ENC_STATUS_NO_ENCRYPTION		0
#define UMI_ENC_STATUS_WEP_SUPPORTED		(1<<0)
#define UMI_ENC_STATUS_TKIP_SUPPORTED		(1<<1)
#define UMI_ENC_STATUS_AES_SUPPORTED		(1<<2)
#define UMI_ENC_STATUS_MASK_ENC_TYPE		0x7	/*three bits */
#define UMI_ENC_STATUS_MASK_KEY_AVAIL		((uint32) (1<<31))

/* For Add Key */
#define UMI_ADDKEY_KEYSIZE_WEP_40			5
#define UMI_ADDKEY_KEYSIZE_WEP_104			13
#define UMI_ADDKEY_KEYSIZE_TKIP				32
#define UMI_ADDKEY_KEYSIZE_AES				16
#define UMI_ADDKEY_KEYIDX_TX_KEY			(uint32)(1<<31))
#define UMI_ADDKEY_KEYIDX_PAIRWISEKEY			(1<<30)
#define UMI_ADDKEY_KEYIDX_KEYRSC			(1<<29)
#define UMI_ADDKEY_KEYIDX_FIRST_TXKEY_THEN_RXKEY	(1<<28)
#define UMI_ADDKEY_KEYIDX_KEYID_MASK			0x000000FF

/* For PsMode */
#define UMI_PS_MODE_DISABLED			0
#define UMI_PS_MODE_ENABLED			1
#define WFM_PS_MODE_FLAG_FAST_PSM_ENABLE	0x80

/* Priority Mask */
#define UMI_PRIORITY_DEFAULT			0x00
#define UMI_PRIORITY_0				0x00
#define UMI_PRIORITY_1				0x01
#define UMI_PRIORITY_2				0x02
#define UMI_PRIORITY_3				0x03
#define UMI_PRIORITY_4				0x04
#define UMI_PRIORITY_5				0x05
#define UMI_PRIORITY_6				0x06
#define UMI_PRIORITY_7				0x07

#define UMI_MEM_BLK_DWORDS			(1024/4)

#define UMI_MAX_FILTER_ELEMENT			4
#define UMI_MAX_MAGIC_PATTERN_SIZE		32

#define UMI_NUM_ACS				4

#define UMI_MAX_GRP_ADDRTABLE_ENTRIES		8

#define UMI_MAC_ADDR_SIZE_S			6

#define UMI_OPR_PWR_MODE_DISABLE		0
#define UMI_OPR_PWR_MODE_ENABLE			2

typedef uint16 UMI_MAC_ADDRESS[UMI_MAC_ADDR_SIZE_S / 2];

typedef void *UL_HANDLE;	/* Standardized Name */

typedef void *LL_HANDLE;	/* Standardized Name */

typedef void *DRIVER_HANDLE;	/* Standardized Name */

typedef void *UMI_HANDLE;	/* Standardized Name */


/* Action Codes for MAC address Database List */
typedef enum MAC_ADDR_ACTION_E {
	UMI_BLACKLIST,
	UMI_WHITELIST,
	UMI_DELETE
} MAC_ADDR_ACTION;


typedef enum UMI_STATUS_CODE_E {
	UMI_STATUS_SUCCESS,
	UMI_STATUS_FAILURE,
	UMI_STATUS_PENDING,
	UMI_STATUS_BAD_PARAM,
	UMI_STATUS_OUT_OF_RESOURCES,
	UMI_STATUS_TX_LIFETIME_EXCEEDED,
	UMI_STATUS_RETRY_EXCEEDED,
	UMI_STATUS_LINK_LOST,
	UMI_STATUS_REQ_REJECTED,
	UMI_STATUS_AUTH_FAILED,
	UMI_STATUS_NO_MORE_DATA,
	UMI_STATUS_COUNTRY_INFO_MISMATCH,
	UMI_STATUS_COUNTRY_NOT_FOUND_IN_TABLE,
	UMI_STATUS_MISMATCH_ERROR,
	UMI_STATUS_CONF_ERROR,
	UMI_STATUS_UNKNOWN_OID_ERROR,
	UMI_STATUS_NOT_SUPPORTED,
	UMI_STATUS_UNSPECIFIED_ERROR,
	UMI_STATUS_MAX
} UMI_STATUS_CODE;

typedef enum UMI_EVENTS_E {
	UMI_EVT_IDLE,
	UMI_EVT_START_COMPLETED,
	UMI_EVT_STOP_COMPLETED,
	UMI_EVT_CONNECTING,
	UMI_EVT_CONNECTED,
	UMI_EVT_CONNECT_FAILED,
	UMI_EVT_DISCONNECTED,
	UMI_EVT_SCAN_COMPLETED,
	UMI_EVT_GENERAL_FAILURE,
	UMI_EVT_TX_RATE_CHANGED,
	UMI_EVT_RECONNECTED,
	UMI_EVT_MICFAILURE,
	UMI_EVT_RADAR,
	UMI_EVT_BSS_LOST,
	UMI_EVT_RCPI_RSSI,
	UMI_EVT_TX_FAILURE_THRESHOLD_EXCEEDED,
	UMI_EVT_BSS_REGAINED,
	UMI_EVT_FT_AUTH_RESPONSE,
	UMI_EVT_NEIGHBOR_REPORT,
	UMI_EVT_BSS_STARTED,
	UMI_EVT_DEV_DISCVRY_COMPLETE,
	UMI_EVT_GROUP_FORMATION_REQ,
	UMI_EVT_GROUP_FORMATION_SUCCEEDED,
	UMI_EVT_GROUP_FORMATION_FAILED,
	UMI_EVT_INVITATION_REQ,
	UMI_EVT_INVITATION_SUCCEEDED,
	UMI_EVT_INVITATION_FAILED,
	UMI_EVT_BT_ACTIVE,
	UMI_EVT_BT_INACTIVE,
	UMI_EVT_STOP_AP_COMPLETED,
	UMI_EVT_RX_MGMT_FRAME,
	UMI_EVT_RX_ACTION_FRAME,
	UMI_EVT_DEVICE_UNRESPONSIVE,
	UMI_EVT_MACADDR_ACTION,
	/* Add more events here */
	UMI_EVT_MAX
} UMI_EVENT;

typedef struct UMI_EVT_DATA_MIC_FAILURE_S {
	uint8 Rsc[6];
	uint8 IsPairwiseKey;
	uint8 KeyIndex;
	uint32 MicFailureCount;
	uint8 PeerAddress[6];
	uint16 Reserved;
} UMI_EVT_DATA_MIC_FAILURE;

typedef struct UMI_EVT_DATA_FT_AUTH_RESPONSE_S {
	uint8 Bssid[6];
	uint16 status;
	uint16 ies_len;
	uint8 flags;
	uint8 ies[1];
} UMI_EVT_DATA_FT_AUTH_RESPONSE;

typedef struct UMI_BSSID_OID_S {
	uint8 Bssid[6];		/*Bssid to connect */
	uint8 RoamingEnable;	/*If 1 then reassoc request is sent else assoc
				request is sent */
	uint8 Reserved;		/*Reserved */
} UMI_BSSID_OID;

typedef struct UMI_EVT_DATA_ACTION_FRAME_S {
	uint16 Channel;
	uint16 FrameLength;
	uint32 *pFrame;
} UMI_EVT_DATA_ACTION_FRAME;

typedef union UMI_EVENT_DATA_U {
	uint32 Rate;					/*UMI_EVT_TX_RATE_CHANGED */
	UMI_EVT_DATA_MIC_FAILURE MicFailureData;	/*UMI_EVT_MICFAILURE */
	UMI_EVT_DATA_FT_AUTH_RESPONSE FtAuthRespData;	/*UMI_EVT_FT_AUTH_RESPONSE*/
	UMI_EVT_DATA_ACTION_FRAME ActionFrame;		/*UMI_EVT_RX_ACTION_FRAME*/
} UMI_EVENT_DATA;

typedef enum UMI_DEVICE_OID_E {
	/* gets the MAC address set operation not supported */
	UMI_DEVICE_OID_802_11_STATION_ID = 0x0000,
	/* Gets the country string */
	UMI_DEVICE_OID_802_11_COUNTRY_STRING = 0x0001,
	/* BSS_LIST_SCAN - Scan Request, get operation is not supported */
	UMI_DEVICE_OID_802_11_BSSID_LIST_SCAN = 0x0002,
	/* BSS_CACHE_QUERY - Gets the current stored scan list,
	There is no set operation  corresponding to this */
	UMI_DEVICE_OID_802_11_BSSID_LIST = 0x0003,
	/* BSS - Set operation specifies the AP/IBSS to join/start
	Get operation returns the current BSS info. the device has joined */
	UMI_DEVICE_OID_802_11_BSSID = 0x0004,
	UMI_DEVICE_OID_802_11_ADD_KEY = 0x0005,
	UMI_DEVICE_OID_802_11_AUTHENTICATION_MODE = 0x0006,
	UMI_DEVICE_OID_802_11_ASSOCIATION_INFORMATION = 0x0007,
	UMI_DEVICE_OID_802_11_REMOVE_KEY = 0x0008,
	UMI_DEVICE_OID_802_11_DISASSOCIATE = 0x0009,
	UMI_DEVICE_OID_802_11_RSSI_RCPI = 0x000A,
	UMI_DEVICE_OID_802_11_RSSI_TRIGGER = 0x000B,
	UMI_DEVICE_OID_802_11_MEDIA_STREAM_MODE = 0x000C,
	UMI_DEVICE_OID_802_11_TX_ANTENNA_SELECTED = 0x000D,
	UMI_DEVICE_OID_802_11_RX_ANTENNA_SELECTED = 0x000E,
	UMI_DEVICE_OID_802_11_SUPPORTED_DATA_RATES = 0x000F,
	UMI_DEVICE_OID_802_11_TX_POWER_LEVEL = 0x0010,
	UMI_DEVICE_OID_802_11_NETWORK_TYPE_IN_USE = 0x0011,
	UMI_DEVICE_OID_802_11_INFRASTRUCTURE_MODE = 0x0012,
	UMI_DEVICE_OID_802_11_SSID = 0x0013,
	UMI_DEVICE_OID_802_11_ENCRYPTION_STATUS = 0x0014,

	UMI_DEVICE_OID_802_11_PRIVACY_FILTER = 0x0020,
	UMI_DEVICE_OID_802_11_STATISTICS = 0x0030,
	UMI_DEVICE_OID_802_11_POWER_MODE = 0x0031,
	UMI_DEVICE_OID_802_11_BLOCK_ACK_POLICY = 0x0032,
	UMI_DEVICE_OID_802_11_UPDATE_EPTA_CONFIG_DATA = 0x0033,
	UMI_DEVICE_OID_802_11_SET_AUTO_CALIBRATION_MODE = 0x0034,
	UMI_DEVICE_OID_802_11_CONFIGURE_IBSS = 0x0035,
	UMI_DEVICE_OID_802_11_SET_UAPSD = 0x0036,
	UMI_DEVICE_OID_802_11_MAX_RX_AMSDU_SIZE = 0x0037,
	UMI_DEVICE_OID_802_11_TXOP_LIMIT = 0x0038,
	UMI_DEVICE_OID_802_11_SET_LISTEN_INTERVAL = 0x0039,
	/*  0: Enable BG scan;  1: Disable BG scan */
	UMI_DEVICE_OID_802_11_DISABLE_BG_SCAN = 0x0040,
	UMI_DEVICE_OID_802_11_BLACK_LIST_ADDR = 0x0041,
	UMI_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE = 0x0042,
	UMI_DEVICE_OID_802_11_SET_HOST_MSG_TYPE_FILTER = 0x0043,
	UMI_DEVICE_OID_802_11_SET_RCPI_RSSI_THRESHOLD = 0x0044,
	UMI_DEVICE_OID_802_11_SET_PMKID_BKID = 0x0045,
	UMI_DEVICE_OID_802_11_SET_BEACON_LOST_COUNT = 0x0046,
	UMI_DEVICE_OID_802_11_TX_FAILURE_THRESHOLD_COUNT = 0x0048,
	UMI_DEVICE_OID_802_11_SET_ETHER_TYPE_FILTER = 0x0049,
	UMI_DEVICE_OID_802_11_SET_UDP_PORT_FILTER = 0x0050,
	UMI_DEVICE_OID_802_11_SET_MAGIC_FRAME_FILTER = 0x0051,
	UMI_DEVICE_OID_802_11_ENABLE_11D_FEATURE = 0x0052,
	UMI_DEVICE_OID_802_11_UPDATE_VENDOR_IE = 0x0053,
	UMI_DEVICE_OID_802_11_SET_ARP_IP_ADDR_FILTER = 0x0054,
	UMI_DEVICE_OID_802_11_MDIE = 0x0055,
	UMI_DEVICE_OID_802_11_FTIE = 0x0056,
	UMI_DEVICE_OID_802_11_FT_AUTHENTICATE = 0x0057,
	UMI_DEVICE_OID_802_11_DELETE_FT_AUTHENTICATION = 0x0058,
	UMI_DEVICE_OID_802_11_PTA_LISTEN_INTERVAL = 0x0059,
	UMI_DEVICE_OID_802_11_MEASUREMENT_CAPABILITY = 0x0060,
	UMI_DEVICE_OID_802_11_ENABLE_MEASUREMENTS = 0x0061,
	UMI_DEVICE_OID_802_11_NEIGHBOR_REPORT = 0x0062,
	UMI_DEVICE_OID_802_11_LCI_INFORMATION = 0x0063,
	UMI_DEVICE_OID_802_11_CONFIGURE_REGULATORY_DOMAIN = 0x0064,
	UMI_DEVICE_OID_802_11_PROTECTED_MGMT_POLICY = 0x0065,
	UMI_DEVICE_OID_802_11_SA_QUERY_INTERVAL = 0x0066,
	UMI_DEVICE_OID_802_11_MULTI_TX_CONFIRMATION = 0x0067,
	UMI_DEVICE_OID_802_11_USE_P2P = 0x0068,
	UMI_DEVICE_OID_802_11_DEVICE_DISCOVERY = 0x0069,
	UMI_DEVICE_OID_802_11_CONFIG_GROUP_FORMATION = 0x0070,
	UMI_DEVICE_OID_802_11_START_AP = 0x0071,
	UMI_DEVICE_OID_802_11_INVITATION = 0x0072,
	UMI_DEVICE_OID_802_11_CONFIGURE_WMM_PARAMS = 0x0073,
	UMI_DEVICE_OID_802_11_START_GROUP_FORMATION = 0x0074,
	UMI_DEVICE_OID_802_11_SOCIAL_CHANNELS = 0x0075,
	UMI_DEVICE_OID_802_11_GET_GROUP_TSC = 0x0076,
	UMI_DEVICE_OID_802_11_MULTICAST_ADDR_FILTER = 0x0077,
	UMI_DEVICE_OID_802_11_KEEP_ALIVE_PERIOD = 0x0078,
	UMI_DEVICE_OID_802_11_WEP_DEFAULT_KEY_ID = 0x0079,
	UMI_DEVICE_OID_802_11_GET_DEVICE_MAC_ADDRESS = 0x0080,
	UMI_DEVICE_OID_802_11_DISABLE_ACTIVE_JOIN = 0x0081,
	UMI_DEVICE_OID_802_11_INTRA_BSS_BRIDGING = 0x82,
	UMI_DEVICE_OID_802_11_STOP_AP             = 0x0083,
	UMI_DEVICE_OID_802_11_GET_LINK_SPEED	= 0x0084,
	UMI_DEVICE_OID_802_11_CHANGE_CHANNEL = 0x0085,
	UMI_DEVICE_OID_802_11_RESTORE_CHANNEL = 0x0086,
	UMI_DEVICE_OID_802_11_REGISTER_MGMT_FRAME_RX = 0x0087,
	UMI_DEVICE_OID_802_11_P2P_PS_MODE       = 0x0088,
	UMI_DEVICE_OID_802_11_SET_NUM_CLIENTS       = 0x0089,
	UMI_DEVICE_OID_802_11_GET_LOCAL_AMP_INFO = 0x0090,
	UMI_DEVICE_OID_802_11_GET_AMP_ASSOC_INFO = 0x0091,
	UMI_DEVICE_OID_802_11_ALLOW_ACCESS = 0x0092,
	UMI_DEVICE_OID_802_11_MAC_ADDR_UPDATE = 0x0093,
	UMI_DEVICE_OID_802_11_SET_HIDDEN_AP_MODE = 0x0094,
	UMI_DEVICE_OID_802_11_GET_TX_STATS = 0x0095
} UMI_DEVICE_OID;

typedef enum UMI_REGULATORY_CLASS_E {
	UMI_REG_CLASS_CEPT_1_3,
	UMI_REG_CLASS_FCC_1_4,
	UMI_REG_CLASS_MPHPT_1_6,
	UMI_REG_CLASS_MPHPT_7_11,
	UMI_REG_CLASS_MPHPT_12_15,
	UMI_REG_CLASS_MPHPT_16_20,
	UMI_REG_CLASS_MAX_VALUE
} UMI_REGULATORY_CLASS;

typedef enum UMI_INFRASTRUCTURE_MODE_E {
	UMI_802_IBSS,
	UMI_802_INFRASTRUCTURE,
	UMI_802_AUTO_UNKNOWN,
	UMI_802_INFRASTRUCTURE_MODE_MAX_VALUE
} UMI_INFRASTRUCTURE_MODE;

/* Authentication modes */
typedef enum UMI_AUTHENTICATION_MODE_E {
	UMI_AUTH_MODE_OPEN,
	UMI_AUTH_MODE_SHARED,
	UMI_AUTH_MODE_WPA,
	UMI_AUTH_MODE_WPA_PSK,
	UMI_AUTH_MODE_WPA_NONE,
	UMI_AUTH_MODE_WPA_2,
	UMI_AUTH_MODE_WPA_2_PSK,
	UMI_AUTH_MODE_WAPI,
	UMI_AUTH_MODE_WPA_2_FT,
	UMI_AUTH_MODE_WPA_2_FT_PSK,
	UMI_AUTH_MODE_WPA_2_SH256,
	UMI_AUTH_MODE_WPA_2_SH256_PSK
} UMI_AUTHENTICATION_MODE;

typedef struct UMI_DOT11_COUNTRY_STRING_S	/* 4 byte aligned */
{
	uint8 dot11CountryString[UMI_DOT11_COUNTRY_STRING_SIZE];
	uint8 Reserved;
} UMI_DOT11_COUNTRY_STRING;

typedef struct UMI_802_11_SSID_S	/* 4 byte aligned */
{
	uint32 ssidLength;
	uint8 ssid[UMI_MAX_SSID_SIZE];
} UMI_802_11_SSID;

typedef struct UMI_CHANNELS_S {
	uint32 ChannelNum;
	uint32 MinChannelTime;
	uint32 MaxChannelTime;
	sint32 TxPowerLevel;
} UMI_CHANNELS;

typedef struct UMI_BSS_LIST_SCAN_S	/* 4 byte aligned */
{
	uint32 flags;		/* Bit    0: auto band  Bit 1: 2.4G band
				   Bit    2: 4.9G band  Bit 3: 5G band
				   Bit    4 - 31 : Reserved
				   Value  0: stop scan
				 */
	uint8 NumOfProbeRequests;
	uint8 NumOfSSIDs;
	uint8 NumberOfChannels_2_4Ghz;
	uint8 NumberOfChannels_5Ghz;
	uint32 RssiThreshold;
	UMI_802_11_SSID Ssids[UMI_MAX_SSIDS]; /*2 supported */
	UMI_CHANNELS Channels[1]; /*This is allocated by the driver */
} UMI_BSS_LIST_SCAN;

typedef struct UMI_OID_802_11_ASSOCIATION_INFORMATION_S	/* 4 byte aligned */
{
	uint16 capabilitiesReq;
	uint16 listenInterval;
	uint8 currentApAddress[UMI_MAC_ADDRESS_SIZE];
	uint16 capabilitiesResp;
	uint16 statusCode; /* Status code return in assoc/reassoc response */
	uint16 associationId;
	uint16 variableIELenReq;
	uint16 variableIELenRsp;
} UMI_OID_802_11_ASSOCIATION_INFORMATION;

/***********************************************************************
 * Add-Key Request						     *
 ***********************************************************************/

typedef union UMI_PRIVACY_KEY_DATA_U {
	struct {
		uint8 peerAddress[6];
		uint8 reserved;
		uint8 keyLength;
		uint8 keyData[16];
	} WepPairwiseKey;

	struct {
		uint8 keyId;
		uint8 keyLength;
		uint8 reserved[2];
		uint8 keyData[16];
	} WepGroupKey;

	struct {
		uint8 peerAddress[6];
		uint8 reserved[2];
		uint8 tkipKeyData[16];
		uint8 rxMicKey[8];
		uint8 txMicKey[8];
	} TkipPairwiseKey;

	struct {
		uint8 tkipKeyData[16];
		uint8 rxMicKey[8];
		uint8 keyId;
		uint8 reserved[3];
		uint8 rxSequenceCounter[8];
	} TkipGroupKey;

	struct {
		uint8 peerAddress[6];
		uint8 reserved[2];
		uint8 aesKeyData[16];
	} AesPairwiseKey;

	struct {
		uint8 aesKeyData[16];
		uint8 keyId;
		uint8 reserved[3];
		uint8 rxSequenceCounter[8];
	} AesGroupKey;

	struct {
		uint8 peerAddress[6];
		uint8 keyId;
		uint8 reserved;
		uint8 wapiKeyData[16];
		uint8 micKeyData[16];
	} WapiPairwiseKey;

	struct {
		uint8 wapiKeyData[16];
		uint8 micKeyData[16];
		uint8 keyId;
		uint8 reserved[3];
	} WapiGroupKey;

	struct {
		uint8 IGTKKeyData[16];
		uint8 KeyId;
		uint8 Reserved[3];
		uint8 IPN[8];	/* actual IPN is only 6 bytes long */
	} IGTKGroupKey;

} UMI_PRIVACY_KEY_DATA;

typedef struct UMI_OID_802_11_KEY_S	/* 4 byte aligned */
{
	uint8 keyType;
	uint8 entryIndex;
	uint16 reserved;
	UMI_PRIVACY_KEY_DATA Key;

} UMI_OID_802_11_KEY;

typedef struct UMI_OID_802_11_REMOVE_KEY_S {
	uint8 EntryIndex;
	uint8 Reserved[3];
} UMI_OID_802_11_REMOVE_KEY;

typedef struct UMI_OID_802_11_STATISTICS_S	/* 4 byte aligned */
{
	uint32 countPlcpErrors;
	uint32 countFcsErrors;
	uint32 countTxPackets;
	uint32 countRxPackets;
	uint32 countRxPacketErrors;
	uint32 countRxDecryptionFailures;
	uint32 countRxMicFailures;
	uint32 countRxNoKeyFailures;
	uint32 countTxMulticastFrames;
	uint32 countTxFramesSuccess;
	uint32 countTxFrameFailures;
	uint32 countTxFramesRetried;
	uint32 countTxFramesMultiRetried;
	uint32 countRxFrameDuplicates;
	uint32 countRtsSuccess;
	uint32 countRtsFailures;
	uint32 countAckFailures;
	uint32 countRxMulticastFrames;
	uint32 countRxFramesSuccess;
	uint32 countRxCMACICVErrors;
	uint32 countRxCMACReplays;
	uint32 countRxMgmtCCMPReplays;
	uint32 countRxBIPMICErrors;
} UMI_OID_802_11_STATISTICS;

typedef struct UMI_OID_802_11_TX_STATS_S {
	uint32	TxCount;
	uint32	TxSuccessCount;
	uint32	TxFailureCount;
	uint32	TxRetriedCount;
	uint32	TxRetryExceedCount;
} UMI_OID_802_11_TX_STATS;

typedef struct UMI_POWER_MODE_S {
	uint8 PmMode;
	uint8 FastPsmIdlePeriod;
	uint8 ApPsmChangePeriod;
	uint8 reserved;
} UMI_POWER_MODE;
typedef PACKED struct UMI_BSS_CACHE_INFO_IND_S	/* 4 byte aligned */
{
	uint8 cacheLine;
	uint8 flags; /* b0 is set => beacon, clear => probe resp */
	uint8 bssId[UMI_MAC_ADDRESS_SIZE];
	sint8 rssi;
	uint8 rcpi;
	uint16 channelNumber;
	uint32 freqKhz;
/* From Beacon Body - 802.11 frame endianness (little endian) */
	uint64 timeStamp;
	uint16 beaconInterval;
	uint16 capability;
	uint16 ieLength; /* Note: Cpu endianness -(not from 802.11 frame) */
	uint8 ieElements[2]; /* IE elements may follow, if its larger than
				2 bytes */
} GCC_PACKED UMI_BSS_CACHE_INFO_IND;

typedef struct UMI_BEACON_CAPABILITIY_S {
	uint16 ess:1;		/* Bit 00 */
	uint16 ibss:1;		/* Bit 01 */
	uint16 cfPollable:1;	/* Bit 02 */
	uint16 cfPollRequest:1;	/* Bit 03 */
	uint16 privacy:1;	/* Bit 04 */
	uint16 shortPreamble:1;	/* Bit 05 */
	uint16 pbcc:1;		/* Bit 06 */
	uint16 channelAgility:1;	/* Bit 07 */
	uint16 spectrumMgmt:1;	/* Bit 08 */
	uint16 qos:1;		/* Bit 09 */
	uint16 shortSlotTime:1;	/* Bit 10 */
	uint16 apsd:1;		/* Bit 11 */
	uint16 reserved:1;	/* Bit 12 */
	uint16 dsssOFDM:1;	/* Bit 13 */
	uint16 delayedBlockAck:1;	/* Bit 14 */
	uint16 immdediateBlockAck:1;	/* Bit 15 */
} UMI_BEACON_CAPABILITY;

/* List of Beacon and Probe Response */
typedef PACKED struct UMI_BSS_CACHE_INFO_IND_LIST_S {
	uint32 numberOfItems;
	UMI_BSS_CACHE_INFO_IND bssCacheInfoInd[1];
} GCC_PACKED UMI_BSS_CACHE_INFO_IND_LIST;

typedef struct UMI_BLOCK_ACK_POLICY_S {
	/*Bits 7:0 Correspond to TIDs 7:0 respectively */
	uint8 blockAckTxTidPolicy;
	uint8 reserved1;
	/*Bits 7:0 Correspond to TIDs 7:0 respectively */
	uint8 blockAckRxTidPolicy;
	uint8 reserved2;
} UMI_BLOCK_ACK_POLICY;

typedef struct UMI_UPDATE_EPTA_CONFIG_DATA_S {
	uint32 enablePta;
	uint32 bluetoothQuality;
	uint32 PtaDebugCommand;
} UMI_UPDATE_EPTA_CONFIG_DATA;

typedef struct UMI_TX_DATA_S {
	uint8 *pExtraBuffer;
	uint8 *pEthHeader;
	uint32 ethPayloadLen;
	uint8 *pEthPayloadStart;
	void *pDriverInfo;
} UMI_TX_DATA;

typedef struct UMI_GET_TX_DATA_S {
	uint32 bufferLength;
	uint8 *pTxDesc;
	uint8 *pDot11Frame;
	void *pDriverInfo;
} UMI_GET_TX_DATA;

typedef struct UMI_GET_PARAM_STATUS_S {
	uint16 oid;		/* The OID number that associated with
				   the parameter. */
	UMI_STATUS_CODE status;	/* Status code for the get operation. */
	uint16 length;		/* Number of bytes in the data. */
	void *pValue;		/* The parameter data. */
} UMI_GET_PARAM_STATUS;

typedef struct UMI_HI_DPD_S	/* 4 byte aligned */
{
	uint16 length;		/* length in bytes of the DPD struct */
	uint16 version;		/* version of the DPD record */
	uint8 macAddress[6];	/* Mac addr of the device */
	uint16 flags;		/* record options */
	uint32 sddData[1];	/* 1st 4 bytes of the Static & Dynamic Data */
	/* rest of the Static & Dynamic Data */
} UMI_HI_DPD;

typedef struct UMI_CONFIG_REQ_S	/* 4 byte aligned */
{
	uint32 dot11MaxTransmitMsduLifeTime;
	uint32 dot11MaxReceiveLifeTime;
	uint32 dot11RtsThreshold;
	UMI_HI_DPD dpdData;
} UMI_CONFIG_REQ;

typedef struct UMI_CONFIG_CNF_S {
	uint32 result;
	uint8 dot11StationId[6];
	uint8 reserved[2];
} UMI_CONFIG_CNF;

typedef struct UMI_MEM_READ_REQ_S {
	uint32 address;		/* Address to read data   */
	uint16 length;		/* Length of data to read */
	uint16 flags;
} UMI_MEM_READ_REQ;

typedef struct UMI_MEM_READ_CNF_S {
	uint32 length;		/*Length of Data */
	uint32 result;		/* Status */
	uint32 data[UMI_MEM_BLK_DWORDS];
} UMI_MEM_READ_CNF;

typedef struct UMI_MEM_WRITE_REQ_S {
	uint32 address;
	uint16 length;		/* Length of data */
	uint16 flags;
	uint32 data[UMI_MEM_BLK_DWORDS];
} UMI_MEM_WRITE_REQ;

typedef struct UMI_TX_MGMT_FRAME_COMPLETED_S {
	uint32 Status;
	uint32 FrameLength;
	uint32 *pFrame;
} UMI_TX_MGMT_FRAME_COMPLETED;

typedef struct UMI_OID_802_11_CONFIGURE_IBSS_S	/* 4 bytes aligned */
{
	uint8 channelNum;
	uint8 enableWMM;
	uint8 enableWEP;
	uint8 networkTypeInUse;
	uint16 atimWinSize;
	uint16 beaconInterval;
} UMI_OID_802_11_CONFIGURE_IBSS;

typedef struct UMI_OID_802_11_SET_UAPSD_S	/* 4 bytes aligned */
{
	uint16 uapsdFlags;
	uint16 minAutoTriggerInterval;
	uint16 maxAutoTriggerInterval;
	uint16 autoTriggerStep;
} UMI_OID_802_11_SET_UAPSD;

typedef struct UMI_RCPI_RSSI_THRESHOLD_S {
	uint8 RssiRcpiMode;
	uint8 LowerThreshold;
	uint8 UpperThreshold;
	uint8 RollingAverageCount;
} UMI_RCPI_RSSI_THRESHOLD;

typedef struct UMI_TX_FAILURE_THRESHOLD_COUNT_S {
	uint32 TransmitFailureThresholdCount;
} UMI_TX_FAILURE_THRESHOLD_COUNT;

typedef struct UMI_BEACON_LOST_COUNT_S {
	uint8 BeaconLostCount;
	uint8 Reserved[3];
} UMI_BEACON_LOST_COUNT;

typedef struct UMI_BSSID_INFO_S {
	uint8 bssid[UMI_MAC_ADDRESS_SIZE];
	uint8 pmkidBkid[UMI_PMKID_BKID_SIZE];
	uint8 reserved[2];
} UMI_BSSID_INFO;
typedef struct UMI_BSSID_PMKID_BKID_S {
	uint32 bssidInfoCount;
	UMI_BSSID_INFO bssidInfo[UMI_MAX_BSSID_INFO_ENTRIES];
} UMI_BSSID_PMKID_BKID;

typedef struct UMI_ETHER_TYPE_FILTER_CONF_S {
	uint8 filterMode;
	uint8 reserved;
	uint16 etherType;
} UMI_ETHER_TYPE_FILTER_CONF;

typedef struct UMI_SET_ETHER_TYPE_FILTER_S {
	uint8 numOfFilter;
	uint8 reserved[3];
	UMI_ETHER_TYPE_FILTER_CONF etherTypeFilterConf[UMI_MAX_FILTER_ELEMENT];
} UMI_SET_ETHER_TYPE_FILTER;

typedef struct UMI_UDP_PORT_FILTER_CONF_S {
	uint8 filterMode;
	uint8 flag;
	uint16 udpPortNum;
} UMI_UDP_PORT_FILTER_CONF;

typedef struct UMI_SET_UDP_PORT_FILTER_S {
	uint8 numOfFilter;
	uint8 reserved[3];
	UMI_UDP_PORT_FILTER_CONF udpPortFilterConf[UMI_MAX_FILTER_ELEMENT];
} UMI_SET_UDP_PORT_FILTER;

typedef struct UMI_SET_MAGIC_FRAME_FILTER_S {
	uint8 filterMode;
	uint8 offset;
	uint8 magicPatternLength;
	uint8 reserved;
	uint8 magicPattern[UMI_MAX_MAGIC_PATTERN_SIZE];
} UMI_SET_MAGIC_FRAME_FILTER;

typedef struct UMI_TXOP_LIMIT_S {
	uint16 txop;
	uint16 reserved;
} UMI_TXOP_LIMIT;

typedef struct UMI_LISTEN_INTERVAL_S {
	uint16 dtimSkippingCount;
	uint16 reserved;
} UMI_LISTEN_INTERVAL;

typedef struct UMI_UPDATE_VENDOR_IE_S {
	uint32 probeReqIElength;
	uint8 *pProbeReqIE;
	uint32 probeRespIElength;
	uint8 *pProbeRespIE;
	uint32 beaconIElength;
	uint8 *pBeaconIE;
	uint32 assocReqIElength;
	uint8 *pAssocReqIE;
	uint32 assocRespIElength;
	uint8 *pAssocRespIE;
} UMI_UPDATE_VENDOR_IE;

typedef struct UMI_SET_ARP_IP_ADDR_FILTER_S {
	uint32 filterMode;
	uint8 ipV4Addr[4];
} UMI_SET_ARP_IP_ADDR_FILTER;

typedef struct UMI_MDIE_S {
	uint16 mdie_len;
	uint8 mdie[6];
} UMI_MDIE;

typedef struct UMI_FTIE_S {
	uint16 ftie_len;
	uint8 ftie[2];
} UMI_FTIE;

typedef struct UMI_FT_AUTHENTICATE_S {
	uint8 Bssid[6];
	uint8 flags;
	uint8 channel;
} UMI_FT_AUTHENTICATE;

typedef struct UMI_PTA_LISTEN_INTERVAL_S {
	uint8 PtaListenInterval;
	uint8 reserved[3];
} UMI_PTA_LISTEN_INTERVAL;

typedef struct UMI_NEIGHBOR_REPORT_S {
	uint8 category;
	uint8 action;
	uint8 dialogToken;
	uint8 ieElements[1];
} UMI_NEIGHBOR_REPORT;

typedef struct UMI_802_11_LCI_INFORMATION_S {
	uint8 elementID;
	uint8 latitiude[5];
	uint8 longitude[5];
	uint8 altitude[5];
	uint8 datum;
	uint8 Reserved[3];
} UMI_802_11_LCI_INFORMATION;

typedef struct UMI_CHANNEL_TRIPLET_S {
	uint16 firstChannel;
	uint16 noOfChannels;
	uint32 channelSpacing;
} UMI_CHANNEL_TRIPLET;

typedef struct UMI_REG_CLASS_MAP_S {
	uint32 txPowerLevel;
	uint16 channelOffset;
	uint8 RegClass;
	uint8 noOfTriplets;
	UMI_CHANNEL_TRIPLET channelTriplet[UMI_NUM_CHANNEL_TRIPLET];
} UMI_REG_CLASS_MAP;

typedef struct UMI_COUNTRY_REG_DETAILS_S {
	uint8 countryString[UMI_DOT11_COUNTRY_STRING_SIZE];
	uint8 noOfRegClass;
	UMI_REG_CLASS_MAP regClassMap[UMI_REG_CLASS_MAP_SIZE];
} UMI_COUNTRY_REG_DETAILS;

typedef struct UMI_REGULATORY_DOMAIN_TABLE_S {
	UMI_COUNTRY_REG_DETAILS countryRegDetails[1];
} UMI_REGULATORY_DOMAIN_TABLE;

typedef struct UMI_SA_QUERY_INTERVAL_S {
	uint32 saQueryRetryTimeout;
	uint32 saQueryMaxTimeout;
} UMI_SA_QUERY_INTERVAL;

typedef struct UMI_OID_802_11_BSS_CONFIGURATION_S {
	uint8 bssid[UMI_MAC_ADDRESS_SIZE];
	uint8 DTIMperiod;	/* DTIM period */
	uint8 probeDelay;
	uint16 ssidLength;
	uint8 ssid[32];
	uint8 preambleType;
	uint8 band;
	uint16 channelNum;
	uint16 beaconInterval;	/* beacon Interval */

	/* capabilities */
	uint16 capabilityInfo;	/* Capabilities Bitmask */
	uint16 supportedRates;	/* Supported rates */
	uint16 basicRates;	/* Basic rates */
	uint16 extSupportedRates;	/* Extended supported rates */

	uint32 CTWindow;	/* In units of TU's */

} UMI_OID_802_11_BSS_CONFIGURATION;

typedef struct UMI_OID_802_11_START_AP_S {
	uint8 mode;		/* mini AP, P2P GO */
	uint8 band;
	uint16 channelNum;
	uint32 CTWindow;	/* In units of TU's */
	uint32 beaconInterval;	/* beacon Interval */
	uint8 DTIMperiod;	/* DTIM period */
	uint8 preambleType;
	uint8 probeDelay;
	uint8 ssidLength;
	uint8 ssid[UMI_MAX_SSID_SIZE];
	uint32 basicRates;	/* Basic rates */

	uint8 reserved;		/* Reserved */
	uint8 networkType;	/* i.e. 11b, 11a etc */
	uint8 bssid[UMI_MAC_ADDRESS_SIZE];
} UMI_OID_802_11_START_AP;

typedef struct UMI_OID_P2P_GROUP_FORMATION_S {
	uint8 opChannel;
	uint8 intentValue;
	uint8 persistentGroup;
	uint8 configTimeGO;
	uint8 ConfigTimeClient;
	uint8 SSIDLength;
	uint16 reserved1;
	uint8 ssid[UMI_MAX_SSID_SIZE];
	uint32 CTWindow;
	uint32 beaconInterval;
	uint8 DTIMPeriod;
	uint8 preambleType;
	uint8 probeDelay;
	uint8 reserved2;
	uint32 basicRateSet;
} UMI_OID_P2P_GROUP_FORMATION;

typedef struct UMI_OID_802_11_P2P_PS_MODE_S {
	uint8 oppPsCTWindow;
	uint8 count;
	uint8 reserved;
	uint8  DtimCount;
	uint32 duration;
	uint32 interval;
	uint32 startTime;
} UMI_OID_802_11_P2P_PS_MODE;

typedef struct UMI_OID_P2P_START_GROUP_FORMATION_S {
	uint32 devNameLen;
	uint8 devName[UMI_MAX_SSID_SIZE];
} UMI_OID_P2P_START_GROUP_FORMATION;

typedef struct UMI_OID_P2P_SOCIAL_CHANNEL_S {
	uint8 numSocChannels;
	uint8 reserved[3];
	UMI_CHANNELS socChannels[1];
} UMI_OID_P2P_SOCIAL_CHANNEL;

typedef struct UMI_OID_P2P_DEVICE_DISCOVERY_S {
	uint16 listenChannel;
	uint8 p2pMinDiscoverableTime;
	uint8 p2pMaxDiscoverableTime;
	uint8 maxTransmitRate;
	uint8 probeDelay;
	uint16 findTimeout;
	UMI_BSS_LIST_SCAN bssListScan;
} UMI_OID_P2P_DEVICE_DISCOVERY;

typedef struct UMI_OID_P2P_INVITATION_S {
	uint8 role;
	uint8 flags;
	uint8 channel;
	uint8 configurationTime;
	uint8 targetStaAddress[UMI_MAC_ADDRESS_SIZE];
	uint8 p2pGroupBssid[UMI_MAC_ADDRESS_SIZE];
	uint8 p2pGroupIdAddress[UMI_MAC_ADDRESS_SIZE];
	uint8 ssidLength;
	uint8 p2pGroupIdSsid[UMI_MAX_SSID_SIZE];
} UMI_OID_P2P_INVITATION;

typedef struct WMM_AC_PARAM_S {
	uint8 aciAifsn;
	uint8 eCWminCWmax;
	uint16 txOpLimit;
} WMM_AC_PARAM;

typedef struct UMI_OID_802_11_CONFIG_WMM_PARAMS_S {
	uint8 qosInfo;
	uint8 reserved1;
	WMM_AC_PARAM ac_BestEffort;
	WMM_AC_PARAM ac_BackGround;
	WMM_AC_PARAM ac_Video;
	WMM_AC_PARAM ac_Voice;
} UMI_OID_802_11_CONFIG_WMM_PARAMS;

typedef struct UMI_OID_USE_P2P_S {
	uint8 useP2P;
	uint8 reserved[3];
} UMI_OID_USE_P2P;

typedef struct UMI_OID_ALLOWED_ACCESS_S {
	uint8 allow_access;
	uint8 reserved[3];
} UMI_OID_ALLOWED_ACCESS;

typedef struct UMI_OID_ALLOWED_DATABASE_S {
	uint8 mac_addr[UMI_MAC_ADDRESS_SIZE];
	uint16 reserved;
    MAC_ADDR_ACTION actionType;
} UMI_OID_ALLOWED_DATABASE;

typedef struct UMI_EVT_DATA_CONNECT_INFO_S {
	uint8 stationMacAddr[UMI_MAC_ADDRESS_SIZE];
	uint8 linkId;
	uint8 PWencStatus;
} UMI_EVT_DATA_CONNECT_INFO;

typedef struct UMI_EVT_DATA_DISCONNECT_INFO_S {
	uint8 linkId;
	uint8 reserved[3];
} UMI_EVT_DATA_DISCONNECT_INFO;

typedef struct UMI_EVT_P2P_DATA_GROUP_FORMATION_REQ_S {
	uint8 stationMacAddr[UMI_MAC_ADDRESS_SIZE];
	uint8 intentValue;
	uint8 persistentGroup;
} UMI_EVT_P2P_DATA_GROUP_FORMATION_REQ;

typedef struct UMI_EVT_P2P_DATA_GROUP_FORMATION_INFO_S {
	uint8 role;
	uint8 p2pGroupBssid[UMI_MAC_ADDRESS_SIZE];
	uint8 reserved;
} UMI_EVT_P2P_DATA_GROUP_FORMATION_INFO;

typedef struct UMI_EVT_P2P_DATA_INVITATION_REQ_S {
	uint8 configurationTime;
	uint8 p2pGroupBssid[UMI_MAC_ADDRESS_SIZE];
	uint8 p2pGroupIdAddress[UMI_MAC_ADDRESS_SIZE];
	uint8 ssidLength;
	uint8 p2pGroupIdSsid[UMI_MAX_SSID_SIZE];
} UMI_EVT_P2P_DATA_INVITATION_REQ;

typedef struct UMI_OID_802_11_GET_GROUP_TSC_S {
	uint32 bits_47_16;
	uint16 bits_15_00;
	uint16 reserved;
} UMI_OID_802_11_GET_GROUP_TSC;

typedef struct UMI_OID_802_11_MULTICAST_ADDR_FILTER_S {
	uint32 enable;
	uint32 numOfAddresses;
	UMI_MAC_ADDRESS AddressList[UMI_MAX_GRP_ADDRTABLE_ENTRIES];
} UMI_OID_802_11_MULTICAST_ADDR_FILTER;

typedef struct UMI_OID_802_11_KEEP_ALIVE_PERIOD_S {
	uint16 keepAlivePeriod;	/* in seconds */
	uint8 reserved[2];
} UMI_OID_802_11_KEEP_ALIVE_PERIOD;

typedef struct UMI_OID_802_11_WEP_DEFAULT_KEY_ID_S {
	uint8 wepDefaultKeyId;
	uint8 reserved[3];
} UMI_OID_802_11_WEP_DEFAULT_KEY_ID;

typedef struct UMI_OID_802_11_DEVICE_MAC_ADDRESS_S {
	uint8 deviceMacAddress[6];
	uint8 reserved[2];
} UMI_OID_802_11_DEVICE_MAC_ADDRESS;

typedef struct UMI_OID_802_11_TX_MGMT_FRAME_S {
	uint16 FrmLen;
	uint8  FrmBuf[2];
} UMI_OID_802_11_TX_MGMT_FRAME;
typedef struct UMI_OID_802_11_SET_NUM_CLIENTS_S {
	uint16 NumOfClients;
	uint16 Reserved;
} UMI_OID_802_11_SET_NUM_CLIENTS;

typedef struct UMI_OID_802_11_LOCAL_AMP_INFO_S {
	uint32 TotalBandwidth;
	uint32 MaxGuaranteedBandwidth;
	uint32 MinLatency;
	uint32 MaxPduSize;
	uint16 PalCapabilities;
	uint16 MaxAmpAssocLength;
} UMI_OID_802_11_LOCAL_AMP_INFO;

typedef struct UMI_CHANNEL_LIST_S {
	UMI_DOT11_COUNTRY_STRING CountryString;
	uint32 RegulatoryExtension;
	uint32 RegulatoryClass;
	uint32 CoverageClass;
	uint32 FirstChannel;
	uint32 NoOfChannel;
	sint32 MaxTxPower;
} UMI_CHANNEL_LIST;

typedef struct UMI_OID_802_11_AMP_ASSOC_INFO_S {
	uint8  MacAddress[UMI_MAC_ADDRESS_SIZE];
	uint16 Reserved;
	UMI_CHANNEL_LIST PreferredChannel;
	UMI_CHANNEL_LIST ConnectedChannel;
	uint32 PalCapabilities;
} UMI_OID_802_11_AMP_ASSOC_INFO;

typedef struct UMI_OID_802_11_SET_HIDDEN_AP_MODE_S {
	uint8 flag;
	uint8 Reserved[3];
} UMI_OID_802_11_SET_HIDDEN_AP_MODE;

/*****************************************************************************/
/********	  Callback Function Type Definition		      ********/
/*****************************************************************************/

typedef void (*IndicateEvent) (UL_HANDLE ulHandle,
			       UMI_STATUS_CODE statusCode,
			       UMI_EVENT umiEvent,
			       uint16 eventDataLength,
			       void *pEventData);

typedef void (*TxComplete) (UL_HANDLE ulHandle,
			    UMI_STATUS_CODE statusCode,
			    UMI_TX_DATA *pTxData);

typedef UMI_STATUS_CODE(*DataReceived) (UL_HANDLE ulHandle,
					UMI_STATUS_CODE status,
					uint16 length,
					void *pFrame,
					void *pDriverInfo,
					void *pFrameStart,
					uint32 flags);

typedef void (*ScanInfo) (UL_HANDLE ulHandle,
			  uint32 cacheInfoLen,
			  UMI_BSS_CACHE_INFO_IND *pCacheInfo);

typedef void (*GetParameterComplete) (UL_HANDLE ulHandle,
				      uint16 oid,
				      UMI_STATUS_CODE status,
				      uint16 length,
				      void *pValue);

typedef void (*SetParameterComplete) (UL_HANDLE ulHandle,
				      uint16 oid,
				      UMI_STATUS_CODE status);

typedef void (*ConfigReqComplete) (UL_HANDLE ulHandle,
				   UMI_CONFIG_CNF *pConfigCnf);

typedef void (*MemoryReadReqComplete) (UL_HANDLE ulHandle,
				       UMI_MEM_READ_CNF *pMemReadCnf);

typedef void (*MemoryWriteReqComplete) (UL_HANDLE ulHandle,
					UMI_STATUS_CODE status);

typedef void (*Schedule) (UL_HANDLE ulHandle);

typedef void (*TxMgmtFrmComplete) (UL_HANDLE ulHandle,
				   UMI_TX_MGMT_FRAME_COMPLETED *pTxMgmtData);

typedef uint8(*ScheduleTx) (LL_HANDLE lowerHandle);

typedef void (*RxComplete) (LL_HANDLE lowerHandle,
			    void *pFrame,
			    void *pDriverInfo);

typedef LL_HANDLE(*Create) (UMI_HANDLE umiHandle,
			    DRIVER_HANDLE driverHandle);

typedef UMI_STATUS_CODE(*Start) (LL_HANDLE lowerHandle,
				 uint32 fwLength,
				 void *pFirmwareImage);

typedef UMI_STATUS_CODE(*Stop) (LL_HANDLE lowerHandle);

typedef UMI_STATUS_CODE(*Destroy) (LL_HANDLE lowerHandle);

/*****************************************************************************/
/********	  External Data Structure			      ********/
/*****************************************************************************/

/* The following data structure defines the callback interface the upper and
   lower layer is supposed to register, inorder to be notified as and when
   the corresponding events occures.
*/
typedef struct UL_CB_S {
	IndicateEvent indicateEvent_Cb;
	TxComplete txComplete_Cb;
	DataReceived dataReceived_Cb;
	ScanInfo scanInfo_Cb;
	GetParameterComplete getParameterComplete_Cb;
	SetParameterComplete setParameterComplete_Cb;
	ConfigReqComplete configReqComplete_Cb;
	MemoryReadReqComplete memoryReadReqComplete_Cb;
	MemoryWriteReqComplete memoryWriteReqComplete_Cb;
	Schedule schedule_Cb;
	TxMgmtFrmComplete txMgmtFrmComplete_Cb;
} UL_CB;

typedef struct LL_CB_S {
	Create create_Cb;
	Start start_Cb;
	Destroy destroy_Cb;
	Stop stop_Cb;
	RxComplete rxComplete_Cb;
	ScheduleTx scheduleTx_Cb;
} LL_CB;

typedef struct UMI_CREATE_IN_S {
	uint32 apiVersion;
	uint32 flags;
	UL_HANDLE ulHandle;
	UL_CB ulCallbacks;
	LL_CB llCallbacks;
} UMI_CREATE_IN;

typedef struct UMI_CREATE_OUT_S {
	UMI_HANDLE umiHandle;	/* Handle for UMI  Module */
	LL_HANDLE llHandle;	/* Handle for Lower Layer Driver Instance */
	uint32 mtu;
} UMI_CREATE_OUT;

typedef struct UMI_START_T_S {
	uint8 *pFirmware;	/* The firmware image to be downloaded to the
				device */
	uint32 fmLength;	/* The number of bytes in the firmware image. */
	uint32 *pDPD;		/* Pointer to Device Production Data. */
	uint32 dpdLength;	/* The number of bytes in the DPD block. */
} UMI_START_T;

/*---------------------------------------------------------------------------*
 *		     Externally Visible Functions		             *
 *---------------------------------------------------------------------------*/

/******************************************************************************
 *			  HOST INTERFACE LAYER				     *
 *****************************************************************************/

/******************************************************************************
 * NAME:	UMI_Create
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This is the first API that needs to be called by the Host device.It creates
 * a UMI object and initializes the UMI module.
 * \param pIn     -   A structure that contains all the input parameters to
 *                    this function.
 * \param pOut    -   A structure that contains all the output parameters to
 *                    this function.
 * \returns UMI_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_Create(UMI_CREATE_IN *pIn, UMI_CREATE_OUT *pOut);

/******************************************************************************
 * NAME:	UMI_Start
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This needs to be called by the Host device.This function starts the UMI
 * module. The caller should wait for UL_CB::IndicateEvent() to return an event
 * that indicates the start operation is completed.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param pStart      -   Pointer to the start-request parameters.
 * \returns UMI_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_Start(UMI_HANDLE umiHandle, UMI_START_T *pStart);

/******************************************************************************
 * NAME:	UMI_Destroy
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function deletes a UMI object and free all resources owned by this
 * object. The caller should call UMI_Stop() and wait for UMI returning a
 * stop-event before removing the UMI object.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \returns UMI_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_Destroy(UMI_HANDLE umiHandle);

/******************************************************************************
 * NAME:	UMI_Stop
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function requests to stop the UMI module. The caller should wait for
 * UMI_CB::IndicateEvent() to return an event that indicates the stop operation
 * is completed.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \returns UMI_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_Stop(UMI_HANDLE umiHandle);

/******************************************************************************
 * NAME:UMI_Transmit
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function indicates to the UMI module the driver has 802.3 frames to be
 * sent to the device.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param priority    -   Priority of the frame.
 * \param flags       -   b0-b7:Link Id
			  b8-b30: Reserved
 *			  b31 : More data
 * \param pTxData     -   pointer to the structure which
 *			  include frame to transmit.
 * \returns UMI_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_Transmit(UMI_HANDLE umiHandle,
			     uint8 priority,
			     uint32 flags,
			     UMI_TX_DATA *pTxData);

/******************************************************************************
 * NAME:UMI_GetParameter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function request to get a parameter from the UMI module or the WLAN
 * device. The caller should wait for UL_CB::GetParameterComplete() to retrieve
 * the data.
 * \param umiHandle -   A UMI instance returned by UMI_Create().
 * \param oid	    -   The OID number that is associated with the parameter.
 * \param flag	    -   Flag field (b0-b7: Link Id)
 * \returns UMI_GET_PARAM_STATUS If the Status filled in UMI_GET_PARAM_STATUS
 *		       structure is set to UMI_STATUS_SUCCESS than caller
 *		       will get parameter data in pValue filed. If Status
 *		       filled is set to UMI_STATUS_PENDING than caller
 *		       should wait for UL_CB::GetParameterComplete() to
 *		       retrieve the data.
 *****************************************************************************/
UMI_GET_PARAM_STATUS *UMI_GetParameter(UMI_HANDLE umiHandle,
				       uint16 oid,
				       uint16 flag);

/******************************************************************************
 * NAME:UMI_SetParameter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function request to set a parameter to the UMI module or the WLAN
 * device. This API is used to issue WLAN commands to the device.
 * \param umiHandle -   A UMI instance returned by UMI_Create().
 * \param oid	    -   The OID number that is associated with the parameter.
 * \param length    -   Number of bytes in the data.
 * \param flag	    -   Flag field (b0-b7: Link Id).
 * \param pValue    -   The parameter data
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_SetParameter(UMI_HANDLE umiHandle,
				 uint16 oid,
				 uint16 length,
				 uint16 flag,
				 void *pValue);

/******************************************************************************
 * NAME:UMI_Worker
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function executes the UMI state machine in a exclusive context scheduled
 * by the driver software.The function UL_CB::Schedule() indicate driver that
 * UMAC need context to process imminent events. If driver is free, than it will
 * call this function.This function shall return as soon as it has processed all
 * imminent events. It is not a thread function.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_Worker(UMI_HANDLE umiHandle);

/******************************************************************************
 * NAME:	UMI_ConfigurationRequest
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will format and send a Configuration request down to the
 * Device. Before calling this function the UMI module has to be initialized
 * and started.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param length      -   Number of bytes in the data.
 * \param pValue      -   The parameter data.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_ConfigurationRequest(UMI_HANDLE umiHandle,
					 uint16 length,
					 UMI_CONFIG_REQ *pValue);

/******************************************************************************
 * NAME:	UMI_MemoryReadRequest
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will format and send a memory read request down to the Device.
 * \param umiHandle - A UMI instance returned by UMI_Create().
 * \param length - Number of bytes in the data.
 * \param pValue - The parameter data.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_MemoryReadRequest(UMI_HANDLE umiHandle,
				      uint16 length,
				      UMI_MEM_READ_REQ *pValue);

/******************************************************************************
 * NAME:	UMI_MemoryWriteRequest
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will format and send a memory write request down to the Device.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param length      -   Number of bytes in the data.
 * \param pValue      - The parameter data.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_MemoryWriteRequest(UMI_HANDLE umiHandle,
				       uint16 length,
				       UMI_MEM_WRITE_REQ *pValue);

/******************************************************************************
 * NAME:	UMI_RegisterEvents
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Through this API upper layer will register event of their interest.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param eventMask   -   Bit mask of registered events.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_RegisterEvents(UMI_HANDLE umiHandle, uint32 eventMask);

/******************************************************************************
 * NAME:	UMI_RxFrmComplete
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function is called when the receiving of a frame is complete by the
 * upper layer driver so that the memory associated with the frame can be freed.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param pFrame      -   Pointer to the received frame.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_RxFrmComplete(UMI_HANDLE umiHandle, void *pFrame);

/******************************************************************************
 * NAME:UMI_MgmtTxFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function indicates to the UMI module the driver has 802.11 Managment or
 * Action frame to be sent to the device.
 * \param umiHandle   -   A UMI instance returned by UMI_Create().
 * \param flags       -   b0-b7:Link Id
			  b8-b31: Reserved
 * \param pTxMgmtData -   pointer to the structure which
 *			  include frame to transmit.
 * \returns UMI_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_MgmtTxFrame(UMI_HANDLE umiHandle,
			     uint32 flags,
			     uint32 FrameLength,
			     uint8 *pTxMgmtData);

/******************************************************************************
 *			 DEVICE INTERFACE LAYER				      *
 *****************************************************************************/

/******************************************************************************
 * NAME:	UMI_GetTxFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Pulls the frame queued in the Device Interface Layer.
 * \param UmiHandle   -   A UMI instance returned by UMI_Create().
 * \param pTxData     -   A structure that contains all the input-output
 *                        parameters to this function.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_GetTxFrame(UMI_HANDLE umiHandle, UMI_GET_TX_DATA **pTxData);

/******************************************************************************
 * NAME:	UMI_ReceiveFrame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will receive frame buffer from lower layer driver.
 * \param umiHandle       -   A UMI instance returned by UMI_Create().
 * \param pFrameBuffer    -   The frame buffer form the Device.
 * \param pDriverInfo     -   Pointer to the Driver Info.
 * \returns UMI_STATUS_CODE A value of 0 indicates a success.
 *****************************************************************************/
UMI_STATUS_CODE UMI_ReceiveFrame(UMI_HANDLE umiHandle,
				 void *pFrameBuffer,
				 void *pDriverInfo);

#endif	/* _UMI_API_H */
