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
 * \file umac_messages.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_messages.h
 * \brief
 * Header for WFM Message module (communication with target).
 * This file defines the data structures as specified by the
 * document WBF_WLAN_HI_Messages.doc rev [1.2]
 * \ingroup Upper_MAC_Core
 * \date 18/12/07 14:35
 */

#ifndef _UMAC_MESSAGES_H
#define _UMAC_MESSAGES_H

#include "stddefs.h"
#include "umac_interface.h"

/*  Bit 0: auto band
    Bit 1: 2.4G band
    Bit 2: 4.9G band
    Bit 3: 5G band
    Bit 31: no active scan
    Value 0: stop scan
*/
#define WFM_BSS_LIST_SCAN_STOP	       (0x0)
#define WFM_BSS_LIST_SCAN_AUTO_BAND    (1<<0)
#define WFM_BSS_LIST_SCAN_2_4G_BAND    (1<<1)
#define WFM_BSS_LIST_SCAN_4_9G_BAND    (1<<2)
#define WFM_BSS_LIST_SCAN_5G_BAND      (1<<3)

#define WFM_PRE_AUTH_MASK	       (1<<1)

#define WFM_BSS_LIST_SCAN_BAND_MASK	     \
	(WFM_BSS_LIST_SCAN_AUTO_BAND	       \
	 | WFM_BSS_LIST_SCAN_2_4G_BAND	     \
	 | WFM_BSS_LIST_SCAN_4_9G_BAND	     \
	 | WFM_BSS_LIST_SCAN_5G_BAND)
#define WFM_BSS_LIST_NO_ACTIVE_SCAN    (1<<31)

#define WFM_HI_MESSAGE_HDR_SIZE	(sizeof(WFM_HI_MESSAGE)	 \
					 - sizeof(WFM_HI_PAYLOAD)       \
	)

#define WFM_SET_PARAM_REQ_HDR_SIZE     (sizeof(WFM_SET_PARAM_REQ)     \
					 - sizeof(WFM_WLAN_PARAMETER_INFO) \
	)

#define WFM_GET_PARAM_REQ_HDR_SIZE     (sizeof(WFM_GET_PARAM_REQ))

#define WFM_GET_PARAM_CNF_HDR_SIZE     (sizeof(WFM_GET_PARAM_CNF)      \
					 - sizeof(WFM_WLAN_PARAMETER_INFO) \
	)

#define WFM_TX_REQ_HDR_SIZE	    (sizeof(WFM_TRANSMIT_REQ)       \
					 - sizeof(WFM_802_3_FRAME)      \
	)

#define WFM_RECEIVE_IND_HDR_SIZE       (sizeof(WFM_RECEIVE_IND)	\
					 - sizeof(WFM_802_3_FRAME)      \
	)

#define WFM_ETH_FRAME_HDR_SIZE	 (14)

/* for WFM_BSS_INFO.Flags */
#define WFM_BSS_INFO_FLAGS_11N_CAPABLE	  (1<<0)
#define WFM_BSS_INFO_FLAGS_SUPP_11N_RATES (1<<1)
#define WFM_BSS_INFO_FLAGS_BSS_WMM_CAP	  (1<<2)
#define WFM_BSS_INFO_FLAGS_NON_ERP_AP	  (1<<3)
#define WFM_BSS_INFO_FLAGS_PROT_ENABLED	  (1<<4)
#define WFM_BSS_INFO_FLAGS_SUPP_SHORT_PREAMBLE  (1<<5)
#define WFM_BSS_INFO_FLAGS_SUPP_WPA	        (1<<6)
#define WFM_BSS_INFO_FLAGS_SUPP_WPA2	        (1<<7)
#if WAPI_SUPPORT
#define WFM_BSS_INFO_FLAGS_SUPP_WAPI	        (1<<8)
#endif	/* WAPI_SUPPORT */

#if FT_SUPPORT
#define WFM_BSS_INFO_FLAGS_SUPP_FT	        (1<<9)
#endif	/* FT_SUPPORT */

#if DOT11K_SUPPORT
#define WFM_BSS_INFO_FLAGS_RRM_MEASUREMENT_CAPABLE (1<<10)
#endif	/* DOT11K_SUPPORT */

/* for WFM_BSS_CACHE_INFO_IND */
#define WFM_BSS_CACHE_INFO_FLAGS_BEACON	        (1<<0)
#define WFM_BSS_CACHE_INFO_FLAGS_PROBE_RESP     (0<<0)

/* for WFM_BSS_INFO.SupportedRates */
/* to make the structure 4 byte aligned/packed */
#define WFM_BSS_INFO_SUPPORTED_RATE_SIZE 2

#define WFM_MEM_BLK_DWORDS		      (1024/4)

#define WFM_MAX_FILTER_ELEMENT		      4
#define WFM_MAX_MAGIC_PATTERN_SIZE	      32

#if P2P_MINIAP_SUPPORT
#define UMAC_MAX_SOCIAL_CHANNELS	      23
#endif	/* P2P_MINIAP_SUPPORT */

#define UMAC_MAX_GRP_ADDRTABLE_ENTRIES	  8

#define UMAC_MAC_ADDR_SIZE_S		  6

typedef uint16 UMAC_MAC_ADDRESS[UMAC_MAC_ADDR_SIZE_S / 2];

/* Authentication modes */
typedef enum WFM_AUTHENTICATION_MODE_E {
	WFM_AUTH_MODE_OPEN,
	WFM_AUTH_MODE_SHARED,
	WFM_AUTH_MODE_WPA,
	WFM_AUTH_MODE_WPA_PSK,
	WFM_AUTH_MODE_WPA_NONE,
	WFM_AUTH_MODE_WPA_2,
	WFM_AUTH_MODE_WPA_2_PSK,
	WFM_AUTH_MODE_WAPI,
#if FT_SUPPORT
	WFM_AUTH_MODE_WPA_2_FT,
	WFM_AUTH_MODE_WPA_2_FT_PSK,
#endif	/* FT_SUPPORT */
#if MGMT_FRAME_PROTECTION
	WFM_AUTH_MODE_WPA_2_SH256,
	WFM_AUTH_MODE_WPA_2_SH256_PSK
#endif	/* MGMT_FRAME_PROTECTION */
} WFM_AUTHENTICATION_MODE;

typedef enum WFM_NETWORK_TYPE_E {
	WFM_802_11FH,
	WFM_802_11DS,
	WFM_802_11OFDM5,
	WFM_802_11OFDM24,
	WFM_802_11AUTOMODE = -1
} WFM_NETWORK_TYPE;

typedef enum WFM_MEDIA_STREAM_MODE_E {
	WFM_802_MediaStreamOff,
	WFM_802_MediaStreamOn
} WFM_MEDIA_STREAM_MODE;

typedef enum WFM_MAC_ADDR_ACTION_E {
	WFM_BLACKLIST,
	WFM_WHITELIST,
	WFM_DELETE,
	WFM_NOTAVAILABLE
} WFM_MAC_ADDR_ACTION;

/******************************************************************************
    DATA STRUCTURE DEFENITIONS CORRESPONDING TO WFM DEVICE OBJECTS
******************************************************************************/

#pragma pack(push, 4)

/*
  Fields in the following structure is re-ordered to handle byte padding.
  So Host should copy each field explicitly to prevent having meaningless
  values in these fields.
*/

typedef PACKED union WFM_HT_SUPPORTED_MCS_SET_PACKED_U {
	PACKED struct {
		uint32 RxMCSBitmap0 : 8;
		uint32 RxMCSBitmap1 : 8;
		uint32 RxMCSBitmap2 : 8;
		uint32 RxMCSBitmap3 : 8;
		uint32 RxMCSBitmap4 : 8;
		uint32 RxMCSBitmap5 : 8;
		uint32 RxMCSBitmap6 : 8;
		uint32 RxMCSBitmap7 : 8;
		uint32 RxMCSBitmap8 : 8;
		uint32 RxMCSBitmap9 : 5;
		uint32 Reserved1 : 3;
		uint32 RxHighestSupportedDataRate : 10;
		uint32 Reserved2 : 6;
		uint32 TxMCSSetDefined : 1;
		uint32 TxRxMCSSetNotEqual : 1;
		uint32 TxMaxNumSpatialStreamsSup : 2;
		uint32 TxUnequalModulationSup : 1;
		uint32 Reserved3 : 27;
	} GCC_PACKED s;
	uint8 All[16];
} GCC_PACKED WFM_HT_SUPPORTED_MCS_SET_PACKED;

/* 4 byte aligned. Note: the order of elements is not same as 802.11 frame */
typedef struct WFM_HT_CAPABILITIES_S {
	uint16 CapabilitiesInfo;
	uint8 AMPDU_Params;
	uint8 ASEL_Capabilities;
	WFM_HT_SUPPORTED_MCS_SET_PACKED SupportedMCSSet;
	uint32 TxBeamFormingCapabilities;
	uint16 ExtendedCapabilities;
	uint16 Padding;
} WFM_HT_CAPABILITIES;

/*
  Fields in the following structure is re-ordered to handle byte padding.
  So Host should copy each field explicitly to prevent having meaningless
  values in these fields.
*/

typedef PACKED union WFM_HT_OPERATION_IE_PARAM1_PACKED_U {
	PACKED struct {
		uint8 SecondaryChannelOffset:2;		/* b0,b1 */
		uint8 STAChannelWidth:1;		/* b2 */
		uint8 RIFSMode:1;			/* b3 */
		uint8 SPSMPSupport:1;			/* b4 */
		uint8 ServiceIntervalGranularity:3;	/* b5-b7 */
	} GCC_PACKED s;
	uint8 All;
} GCC_PACKED WFM_HT_OPERATION_IE_PARAM1_PACKED;

typedef PACKED union WFM_HT_OPERATION_IE_PARAM2_PACKED_U {
	PACKED struct {
		uint16 HTProtection:2;			/* b0, b1 */
		uint16 NonGreenfieldHTSTAsPresent:1;	/* b2 */
		uint16 Reserved1:1;			/* b3 */
		uint16 OBSSNonHTSTAsPresent:1;		/* b4 */
		uint16 Reserved2:11;			/* b5-b15 */
	} GCC_PACKED s;
	uint16 All;
} GCC_PACKED WFM_HT_OPERATION_IE_PARAM2_PACKED;

typedef PACKED union WFM_HT_OPERATION_IE_PARAM3_PACKED_U {
	PACKED struct {
		uint16 Reserved1:6;			/* b0-b5 */
		uint16 DualBeacon:1;			/* b6 */
		uint16 DualCTSProtection:1;		/* b7 */
		uint16 STBCBeacon:1;			/* b8 */
		uint16 LSIGTXOPProtectionFullSupport:1;	/* b9 */
		uint16 PCOActive:1;			/* b10 */
		uint16 PCOPhase:1;			/* b11 */
		uint16 Reserved2:4;			/* b12-b15 */
	} GCC_PACKED s;
	uint16 All;
} GCC_PACKED WFM_HT_OPERATION_IE_PARAM3_PACKED;

typedef struct WFM_HT_OPERATION_IE_S	/* 4 byte aligned */
{
	uint8 PrimaryChannel;				/* 1 byte */
	WFM_HT_OPERATION_IE_PARAM1_PACKED HTInfoParam1;	/* 1 byte */
	WFM_HT_OPERATION_IE_PARAM2_PACKED HTInfoParam2;	/* 2 bytes */
	WFM_HT_OPERATION_IE_PARAM3_PACKED HTInfoParam3;	/* 2 bytes */
	uint16 Padding;					/* 2 bytes */
	WFM_HT_SUPPORTED_MCS_SET_PACKED BasicMCSSet;	/* 16 bytes */
} WFM_HT_OPERATION_IE;

typedef PACKED struct WFM_WMM_AC_PARAM_PACKED_S {
	uint8 AciAifn;		/* b0-b3  - AIFSN
				   b4     - ACM
				   b5-b6  - ACI
				   b7     - Reserved
				 */
	uint8 EcWminEcWmax;	/*
				   b0-b3  - ECWmin
				   b4-b7  - ECWmax
				 */
	uint16 TxopLimit;
} GCC_PACKED WFM_WMM_AC_PARAM_PACKED;

typedef struct WFM_WMM_PARAM_ELEMENT_S	/* 4 byte aligned (and 1 byte packed) */
{
	uint8 OUI[3];
	uint8 OUIType;
	uint8 OUISubType;
	uint8 Version;
	uint8 QoSInfo;
	uint8 Reserved;
	WFM_WMM_AC_PARAM_PACKED Ac_BestEffort;
	WFM_WMM_AC_PARAM_PACKED Ac_BackGround;
	WFM_WMM_AC_PARAM_PACKED Ac_Video;
	WFM_WMM_AC_PARAM_PACKED Ac_Voice;
} GCC_PACKED WFM_WMM_PARAM_ELEMENT;

typedef struct WFM_WPA_PARAM_ELEMENT_S	/* 4 byte aligned */
{
	uint8 ParamInfo[254];	/* make it first element so that its
				   4 byte aligned */
	uint8 Length;
	uint8 Reserved_Padding;
} WFM_WPA_PARAM_ELEMENT;

#if P2P_MINIAP_SUPPORT
typedef struct UMAC_NOA_ATTRIBUTE_S {
	uint8 reserved;
	uint8 index;
	uint8 oppsCtWindow;
	uint8 count;
	uint32 duration;
	uint32 interval;
	uint32 startTime;

} UMAC_NOA_ATTRIBUTE;
#endif

typedef struct WFM_WPA2_PARAM_ELEMENT_S	/* 4 byte aligned */
{
	uint8 ParamInfo[254];	/* make it first element so that its
				   4 byte aligned */
	uint8 Length;
	uint8 Reserved_Padding;
} WFM_WPA2_PARAM_ELEMENT;

#if FT_SUPPORT
typedef struct WFM_MOBILITY_DOMAIN_ELEMENT_S {
	uint8 mdid[2];
	uint8 ft_cap;
} WFM_MOBILITY_DOMAIN_ELEMENT;
#endif	/* FT_SUPPORT */

#if WAPI_SUPPORT
typedef struct WFM_WAPI_PARAM_ELEMENT_S	/* 4 byte aligned */
{
	uint8 ParamInfo[254];	/* make it first element so that its
				   4 byte aligned */
	uint8 Length;
	uint8 Reserved_Padding;
} WFM_WAPI_PARAM_ELEMENT;
#endif	/* WAPI_SUPPORT */

typedef struct WFM_BSS_INFO_S	/* 4 byte aligned */
{
	uint16 Capabilities;	/*Device Capabilities */
	uint8 DTIMPeriod;	/*DTIM Period from TIM information element */
	uint8 RoamingEnabled;
	uint8 MacAddress[WFM_MAC_ADDRESS_SIZE];	/* The BSSID that information
						   to be retrived. */
	uint16 Flags;		/* Bit 0: 11n capable
				   Bit 1: The WBF WLAN device can support the 11n
				   rates which the BSS mandates.
				   Bit 2: BSS is WMM capable.
				   Bit 3: Set if Non-ERP AP
				   Bit 4: Set if protection is enabled
				   Bit 5: Set if short Preamble supported.
				   Bit 6: TKIP Encryption Supported.
				   Bit 7: AES  Encryption Supported.
				   Bit 8: WAPI Encryption Supported.
				   Bit 9: Set if FT/802.11r supported.
				   Bit 10-12: Encryption status
				 */
#if FT_SUPPORT
	uint8 FtAuthFlags;	/* Bit 0: over the DS allowed
				   Bit 1: over the air allowed
				   Bit 2: May Leave current BSS
				 */
	uint8 Reserved[2];
#else	/* FT_SUPPORT */
	uint8 Reserved[3];
#endif	/* FT_SUPPORT */
	uint8 RCPI;		/* Range [0-220] as defined in IEEE 802.11K */
	uint32 SsidLength;
	uint8 Ssid[WFM_MAX_SSID_SIZE];
	uint32 Privacy;
	sint32 Rssi;
	uint32 BeaconPeriod;
	uint32 ATIMWindow;
	uint32 DSConfig;
	uint32 InfrastructureMode;
	uint32 Country;
	uint32 ChannelNumber;
	uint64 TSF;
	uint64 RRMMeasurementCapabilities;
	WFM_HT_CAPABILITIES HtCapabilities;
	WFM_HT_OPERATION_IE HtOperationIe;
	WFM_WMM_PARAM_ELEMENT WmmParamElement;
	WFM_WPA_PARAM_ELEMENT WpaParamElement;
	WFM_WPA2_PARAM_ELEMENT Wpa2ParamElement;
#if WAPI_SUPPORT
	WFM_WAPI_PARAM_ELEMENT WapiParamElement;
#endif	/* WAPI_SUPPORT */
#if FT_SUPPORT
	WFM_MOBILITY_DOMAIN_ELEMENT MobilityDomainElement;
#endif	/* FT_SUPPORT */
#if P2P_MINIAP_SUPPORT
	UMAC_NOA_ATTRIBUTE noaAttrib;
#endif
	uint16 NumRates;
	/* Supported rates may follow, if its more than 2 bytes */
	uint8 SupportedRates[WFM_BSS_INFO_SUPPORTED_RATE_SIZE / sizeof(uint8)];
} WFM_BSS_INFO;

typedef struct WFM_OID_802_11_ASSOCIATION_INFORMATION_S	/* 4 byte aligned */
{
	uint16 Capabilities_Req;
	uint16 Listen_Interval;
	uint8 Current_AP_Address[WFM_MAC_ADDRESS_SIZE];
	uint16 Capabilities_Resp;
	uint16 Status_Code; /* Status code return in assoc/reassoc response */
	uint16 Association_Id;
	uint16 VariableIELenReq;
	uint16 VariableIELenRsp;
} WFM_OID_802_11_ASSOCIATION_INFORMATION;

typedef struct WFM_OID_802_11_STATISTICS_S /* 4 byte aligned */
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
} WFM_OID_802_11_STATISTICS;

typedef struct WFM_ETHER_TYPE_FILTER_CONF_S {
	uint8 filterMode;
	uint8 reserved;
	uint16 etherType;
} WFM_ETHER_TYPE_FILTER_CONF;

typedef struct WFM_SET_ETHER_TYPE_FILTER_S {
	uint8 numOfFilter;
	uint8 reserved[3];
	WFM_ETHER_TYPE_FILTER_CONF etherTypeFilterConf[WFM_MAX_FILTER_ELEMENT];
} WFM_SET_ETHER_TYPE_FILTER;

typedef struct WFM_UDP_PORT_FILTER_CONF_S {
	uint8 filterMode;
	uint8 flag;
	uint16 udpPortNum;
} WFM_UDP_PORT_FILTER_CONF;

typedef struct WFM_SET_UDP_PORT_FILTER_S {
	uint8 numOfFilter;
	uint8 reserved[3];
	WFM_UDP_PORT_FILTER_CONF udpPortFilterConf[WFM_MAX_FILTER_ELEMENT];
} WFM_SET_UDP_PORT_FILTER;

typedef struct WFM_SET_MAGIC_FRAME_FILTER_S {
	uint8 filterMode;
	uint8 offset;
	uint8 magicPatternLength;
	uint8 reserved;
	uint8 magicPattern[WFM_MAX_MAGIC_PATTERN_SIZE];
} WFM_SET_MAGIC_FRAME_FILTER;

typedef struct UMAC_TXOP_LIMIT_S {
	uint16 txop;
	uint16 reserved;
} UMAC_TXOP_LIMIT;

typedef struct UMAC_LISTEN_INTERVAL_S {
	uint16 dtimSkippingCount;
	uint16 reserved;
} UMAC_LISTEN_INTERVAL;

typedef struct UMAC_UPDATE_VENDOR_IE_S {
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
} UMAC_UPDATE_VENDOR_IE;

typedef struct UMAC_SET_ARP_IP_ADDR_FILTER_S {
	uint32 filterMode;
	uint8 ipV4Addr[4];
} UMAC_SET_ARP_IP_ADDR_FILTER;

typedef struct UMAC_SET_ARP_KEEP_ALIVE_PERIOD_S {
	uint16  ArpKeepAlivePeriod; /* in seconds */
	uint8   EncrType; /* (ex. WSM_KEY_TYPE_WEP_DEFAULT) */
	uint8   Reserved;
	uint8   SenderIpv4Address[4]; /* in uint32 big endian format */
	uint8   TargetIpv4Address[4];
} UMAC_SET_ARP_KEEP_ALIVE_PERIOD;

#if FT_SUPPORT
typedef struct UMAC_MDIE_S {
	uint16 mdie_len;
	uint8 mdie[6];
} UMAC_MDIE;
typedef struct UMAC_FTIE_S {
	uint16 ftie_len;
	uint8 ftie[258];
} UMAC_FTIE;
typedef struct UMAC_FT_AUTHENTICATE_S {
	uint8 Bssid[6];
	uint8 Flags;
	uint8 Channel;
} UMAC_FT_AUTHENTICATE;
#endif	/* FT_SUPPORT */

typedef struct UMAC_PTA_LISTEN_INTERVAL_S {
	uint8 PtaListenInterval;
	uint8 reserved[3];
} UMAC_PTA_LISTEN_INTERVAL;

#if MGMT_FRAME_PROTECTION
typedef struct UMAC_SA_QUERY_INTERVAL_S {
	uint32 saQueryRetryTimeout;
	uint32 saQueryMaxTimeout;
} UMAC_SA_QUERY_INTERVAL;
#endif	/* MGMT_FRAME_PROTECTION */

#if P2P_MINIAP_SUPPORT
typedef struct UMAC_P2P_GROUP_CONFIGURATION_S {
	uint8 operatingMode;
	uint8 configTimeGO;
	uint8 configTimeClient;
	uint8 deviceAddr[WFM_MAC_ADDRESS_SIZE];
	uint8 groupCapability;
	uint16 peerListenChnl;
	uint8 regClass;
	uint8 opChannel;
} UMAC_P2P_GROUP_CONFIGURATION;

typedef struct UMAC_OID_802_11_BSS_CONFIGURATION_S {
	uint8 mode;		/* 0: mini AP; 1: P2P GO */
	uint8 band;
	uint16 channelNum;
	uint32 CTWindow;	/* In units of TU's */
	uint32 beaconInterval;	/* beacon Interval */
	uint8 DTIMperiod;	/* DTIM period */
	uint8 preambleType;
	uint8 probeDelay;
	uint8 ssidLength;
	uint8 ssid[32];
	uint32 basicRates;	/* Basic rates */
	uint8 bssid[WFM_MAC_ADDRESS_SIZE];
	uint8 networkType;	/* i.e. 11b, 11a etc */
	uint16 capabilityInfo;	/* Capabilities Bitmask */
	uint8 numOfChannels;
	uint8 channels[14];
	uint8 groupCapability;
	uint8 deviceCapability;
	uint8 reserved[3];
	uint8 regClass;
	uint8 numSTAsConnected;
	uint8 vdrSpecificCapInfo; /* Bit2: WMM, Bit6: TKIP, Bit7: AES;
				     Rest reserved (BSS Info Flags reused) */
} UMAC_OID_802_11_BSS_CONFIGURATION;

typedef struct UMAC_OID_802_11_START_AP_S {
	uint8 mode;		/* mini AP, P2P GO */
	uint8 band;
	uint16 channelNum;
	uint32 CTWindow;	/* In units of TU's */
	uint32 beaconInterval;	/* beacon Interval */
	uint8 DTIMperiod;	/* DTIM period */
	uint8 preambleType;
	uint8 probeDelay;
	uint8 ssidLength;
	uint8 ssid[32];
	uint32 basicRates;	/* Basic rates */
	uint8 reserved;		/* reserved */
	uint8 networkType;	/* i.e. 11b, 11a etc */
	uint8 bssid[WFM_MAC_ADDRESS_SIZE];
} UMAC_OID_802_11_START_AP;

typedef struct UMAC_OID_P2P_GROUP_FORMATION_S {
	uint8 opChannel;
	uint8 intentValue;
	uint8 persistentGroup;
	uint8 configTimeGO;
	uint8 ConfigTimeClient;
	uint8 SSIDLength;
	uint16 reserved1;
	uint8 ssid[WFM_MAX_SSID_SIZE];
	uint32 CTWindow;
	uint32 beaconInterval;
	uint8 DTIMPeriod;
	uint8 preambleType;
	uint8 probeDelay;
	uint8 reserved2;
	uint32 basicRateSet;
} UMAC_OID_P2P_GROUP_FORMATION;

typedef struct UMAC_OID_P2P_START_GROUP_FORMATION_S {
	uint32 devNameLen;
	uint8 devName[WFM_MAX_SSID_SIZE];
} UMAC_OID_P2P_START_GROUP_FORMATION;

typedef struct UMAC_OID_P2P_SOCIAL_CHANNEL_S {
	uint8 numSocChannels;
	uint8 reserved[3];
	WFM_CHANNELS socChannels[UMAC_MAX_SOCIAL_CHANNELS];
} UMAC_OID_P2P_SOCIAL_CHANNEL;

typedef struct UMAC_OID_P2P_DEVICE_DISCOVERY_S {
	uint16 listenChannel;
	uint8 p2pMinDiscoverableTime;
	uint8 p2pMaxDiscoverableTime;
	uint8 maxTransmitRate;
	uint8 probeDelay;
	uint16 findTimeout;
	WFM_BSS_LIST_SCAN bssListScan;
} UMAC_OID_P2P_DEVICE_DISCOVERY;

typedef struct UMAC_OID_P2P_INVITATION_S {
	uint8 role;
	uint8 flags;
	uint8 channel;
	uint8 GOConfigTime;
	uint8 clientConfigTime;
	uint8 targetStaAddress[WFM_MAC_ADDRESS_SIZE];
	uint8 p2pGroupBssid[WFM_MAC_ADDRESS_SIZE];
	uint8 p2pGroupIdAddress[WFM_MAC_ADDRESS_SIZE];
	uint8 ssidLength;
	uint8 p2pGroupIdSsid[WFM_MAX_SSID_SIZE];
} UMAC_OID_P2P_INVITATION;

typedef struct UMAC_OID_802_11_CONFIG_WMM_PARAMS_S {
	uint16 cwMin[WFM_NUM_ACS];
	uint16 cwMax[WFM_NUM_ACS];
	uint8 aifsn[WFM_NUM_ACS];
	uint16 txOpLimit[WFM_NUM_ACS];
	uint32 maxReceiveLifetime[WFM_NUM_ACS];
} UMAC_OID_802_11_CONFIG_WMM_PARAMS;

typedef struct UMAC_OID_USE_P2P_S {
	uint8 useP2P;
	uint8 reserved[3];
} UMAC_OID_USE_P2P;

typedef struct UMAC_OID_ALLOWED_ACCESS_S {
	uint8 allow_access;
	uint8 reserved[3];
} UMAC_OID_ALLOWED_ACCESS;

typedef struct UMAC_OID_ALLOWED_DATABASE_S {
	uint8 mac_addr[WFM_MAC_ADDRESS_SIZE];
	uint16 reserved;
	WFM_MAC_ADDR_ACTION actionType;
} UMAC_OID_ALLOWED_DATABASE;

typedef struct UMAC_EVT_DATA_CONNECT_INFO_S {
	uint8 stationMacAddr[WFM_MAC_ADDRESS_SIZE];
	uint8 linkId;
	uint8 PWencStatus;
} UMAC_EVT_DATA_CONNECT_INFO;

typedef struct UMAC_EVT_DATA_DISCONNECT_INFO_S {
	uint8 linkId;
	uint8 reserved[3];
} UMAC_EVT_DATA_DISCONNECT_INFO;

typedef struct UMAC_EVT_P2P_DATA_GROUP_FORMATION_REQ_S {
	uint8 stationMacAddr[WFM_MAC_ADDRESS_SIZE];
	uint8 intentValue;
	uint8 persistentGroup;
} UMAC_EVT_P2P_DATA_GROUP_FORMATION_REQ;

typedef struct UMAC_EVT_P2P_DATA_GROUP_FORMATION_INFO_S {
	uint8 role;
	uint8 p2pGroupBssid[WFM_MAC_ADDRESS_SIZE];
	uint8 reserved;
} UMAC_EVT_P2P_DATA_GROUP_FORMATION_INFO;

typedef struct UMAC_EVT_P2P_DATA_INVITATION_REQ_S {
	uint8 configurationTime;
	uint8 p2pGroupBssid[WFM_MAC_ADDRESS_SIZE];
	uint8 p2pGroupIdAddress[WFM_MAC_ADDRESS_SIZE];
	uint8 ssidLength;
	uint8 p2pGroupIdSsid[WFM_MAX_SSID_SIZE];
} UMAC_EVT_P2P_DATA_INVITATION_REQ;

typedef struct UMAC_OID_802_11_GET_GROUP_TSC_S {
	uint32 bits_47_16;
	uint16 bits_15_00;
	uint16 reserved;
} UMAC_OID_802_11_GET_GROUP_TSC;

#endif	/* P2P_MINIAP_SUPPORT */

typedef struct UMAC_MULTICAST_ADDR_FILTER_S {
	uint32 enable;
	uint32 numOfAddresses;
	UMAC_MAC_ADDRESS AddressList[UMAC_MAX_GRP_ADDRTABLE_ENTRIES];
} UMAC_MULTICAST_ADDR_FILTER;

typedef struct UMAC_KEEP_ALIVE_PERIOD_S {
	uint16 keepAlivePeriod;	/* in seconds */
	uint8 reserved[2];
} UMAC_KEEP_ALIVE_PERIOD;

typedef struct UMAC_WEP_DEFAULT_KEY_ID_S {
	uint8 wepDefaultKeyId;
	uint8 reserved[3];
} UMAC_WEP_DEFAULT_KEY_ID;

typedef struct UMAC_DEVICE_MAC_ADDRESS_S {
	uint8 deviceMacAddress[6];
	uint8 reserved[2];
} UMAC_DEVICE_MAC_ADDRESS;
#if P2P_MINIAP_SUPPORT
typedef struct UMAC_CHANGE_CHANNEL_REQ_S {
	uint8 Channel;
	uint8 Band;
	uint16 DurationInKus;
} UMAC_CHANGE_CHANNEL_REQ;

#define UMAC_RESTORE_CHANNEL_NONE	0
#define UMAC_RESTORE_CHANNEL_STA        1
#define UMAC_RESTORE_CHANNEL_AP         2
#define UMAC_RESTORE_CHANNEL_P2P_CL	3
#define UMAC_RESTORE_CHANNEL_P2P_GO	4

typedef struct UMAC_RESTORE_CHANNEL_REQ_S {
	uint8 RestorationMode;
	uint8 Reserved[3];
} UMAC_RESTORE_CHANNEL_REQ;
#endif /* P2P_MINIAP_SUPPORT */

#if TX_PER_SUPPORT
typedef struct UMAC_TX_STATS_S {
	uint32	TxCount;
	uint32	TxSuccessCount;
	uint32	TxFailureCount;
	uint32	TxRetriedCount;
	uint32	TxRetryExceedCount;
} UMAC_TX_STATS;
#endif

typedef union WFM_WLAN_PARAMETER_INFO_U	/* 4 byte aligned */
{
	uint8 dot11StationId[WFM_MAC_ADDRESS_SIZE];
	uint8 CacheLine;	/* WFM_DEVICE_OID_802_11_BSSID_LIST */
	uint8 dot11AuthenticationMode;	/*  Value 1 : Open system
					   Value 2 : Shared Key
					 */
	WFM_DOT11_COUNTRY_STRING dot11CountryString;
	WFM_BSS_LIST_SCAN bssListScan;
	WFM_BSSID_OID BssidOid;

	uint8 Streaming_Mode;	/* 1: Media Stream On
				   2: Media Stream Off
				 */
	uint8 Enable11dFeature;
	uint8 Padding[2];

	sint32 TxPowerLevel;

	/* SET_OID: WFM_DEVICE_OID_802_11_BSSID  */
	WFM_BSS_INFO BssInfo;
	/* GET_OID: WFM_DEVICE_OID_802_11_BSSID  */
	uint8 RemoteMacAddress[WFM_MAC_ADDRESS_SIZE];
	uint8 Padding2[2];
	WFM_OID_802_11_KEY dot11AddKey;

	WFM_OID_802_11_ASSOCIATION_INFORMATION dot11AssocInfo;
	WFM_OID_802_11_REMOVE_KEY dot11RemoveKey;
	WFM_DOT11_COUNTRY_STRING CountryStringInfo;
#if P2P_MINIAP_SUPPORT
	/*WFM_OID_802_11_BSS_CONFIGURATION */
	UMAC_OID_802_11_START_AP dot11StartAp;
	UMAC_OID_P2P_GROUP_FORMATION p2pGrpFormtn;
	UMAC_OID_P2P_START_GROUP_FORMATION p2pStartGrpFormatn;
	UMAC_OID_P2P_DEVICE_DISCOVERY p2pDevDiscvry;
	UMAC_OID_P2P_INVITATION p2pInvtn;
	WFM_WMM_PARAM_ELEMENT dot11ConfigWMMparams;
	UMAC_OID_USE_P2P useP2P;
	UMAC_OID_802_11_GET_GROUP_TSC dot11GroupTSC;
	UMAC_OID_P2P_SOCIAL_CHANNEL socialChannels;
	UMAC_OID_ALLOWED_ACCESS	usrAllowAccess;
	UMAC_OID_ALLOWED_DATABASE databasereq;
#endif	/* P2P_MINIAP_SUPPORT */

	/* WFM_DEVICE_OID_802_11_RSSI_RCPI    */
	sint32 RSSI_Value;
	uint32 RCPI_Value;
	/* WFM_DEVICE_OID_802_11_RSSI_TRIGGER */
	uint32 RSSI_Trigger_Value;	/* RSSI in dBm. */
	/* WFM_DEVICE_OID_802_11_MEDIA_STREAM_MODE */

	/* WFM_DEVICE_OID_802_11_TX_ANTENNA_SELECTED */
	uint32 Tx_Antenna_number;
	/* WFM_DEVICE_OID_802_11_RX_ANTENNA_SELECTED */
	uint32 Rx_Antenna_number;
	/* WFM_DEVICE_OID_802_11_SUPPORTED_DATA_RATES */
	uint64 dot11SupportedDataRates;
	sint32 dot11TxPowerLevel;	/* TX_POWER_LEVEL */

	uint32 dot11NetworkTypeInUse;
	uint32 dot11InfraStructureMode;
	WFM_802_11_SSID ssid;
	/* WFM_DEVICE_OID_802_11_ENCRYPTION_STATUS */
	uint32 Encryption_Status;
	/* WFM_DEVICE_OID_802_11_PRIVACY_FILTER */
	uint32 Privacy_Filter;
	/*
	   1: Open Mode
	   2: Filtering Mode
	 */
	WFM_OID_802_11_STATISTICS dot11Statistics;
	/* WFM_DEVICE_OID_802_11_POWER_MODE */
	UMAC_POWER_MODE psMode;
	/* WFM_DEVICE_OID_802_11_OPERATIONAL_POWER_MODE */
	uint32 OprPwrMode;
	/* WFM_DEVICE_OID_802_11_BLOCK_ACK_POLICY */
	WFM_802_11_BLCK_ACK_POLICY block_ack_policy;
	WFM_BSS_CACHE_INFO_IND bss_cache_info_ind;
	/* UMI_DEVICE_OID_802_11_UPDATE_EPTA_CONFIG_DATA */
	WFM_UPDATE_EPTA_CONFIG_DATA update_EPTA_config_data;
	/* UMI_DEVICE_OID_802_11_CONFIGURE_IBSS */
	WFM_OID_802_11_CONFIGURE_IBSS configureIBSS;
	/* WFM_DEVICE_OID_802_11_SET_UAPSD */
	WFM_OID_802_11_SET_UAPSD setUAPSD;
	/*UMI_DEVICE_OID_802_11_SET_AUTO_CALIBRATION_MODE */
	uint32 setAutoCalibrationMode;
	/* UMI_DEVICE_OID_802_11_DISABLE_BG_SCAN */
	uint32 disableBGScan;
	/* WFM_DEVICE_OID_802_11_SET_HOST_MSG_TYPE_FILTER */
	uint32 hostMsgTypeFilter;
	uint8 BlackListAddr[WFM_MAC_ADDRESS_SIZE];
	uint8 reserved[2];
	WFM_RCPI_RSSI_THRESHOLD RcpiRssiThreshold;
	WFM_BEACON_LOST_COUNT BeaconLostCount;
	WFM_TX_FAILURE_THRESHOLD_COUNT TxFailureCount;
	WFM_BSSID_PMKID_BKID PmkidBkidInfo;
#if DOT11K_SUPPORT
	UMAC_802_11_LCI_INFORMATION lciInfo;
	WFM_802_11_SSID NeighborReportSsid;
	uint16 measurementCapability;
	uint16 enableMeasurements;
#endif	/* DOT11K_SUPPORT */
	UMAC_REGULATORY_DOMAIN_TABLE regDomainTable;
	uint32 maxRxAmsduSize;
	WFM_SET_ETHER_TYPE_FILTER setEtherTypeFilter;
	WFM_SET_UDP_PORT_FILTER setUdpPortFilter;
	WFM_SET_MAGIC_FRAME_FILTER setMagicFrameFilter;
	/* WFM_DEVICE_OID_802_11_TXOP_LIMIT */
	UMAC_TXOP_LIMIT txopLimit;
	/* WFM_DEVICE_OID_802_11_SET_LISTEN_INTERVAL */
	UMAC_LISTEN_INTERVAL listenInterval;
	/* WFM_DEVICE_OID_802_11_UPDATE_VENDOR_IE */
	UMAC_UPDATE_VENDOR_IE vendorIe;
	/* WFM_DEVICE_OID_802_11_SET_ARP_IP_ADDR_FILTER */
	UMAC_SET_ARP_IP_ADDR_FILTER ArpFilter;
#if FT_SUPPORT
	UMAC_MDIE mdie;
	UMAC_FTIE ftie;
	UMAC_FT_AUTHENTICATE ftauth;
#endif	/* FT_SUPPORT */
	UMAC_PTA_LISTEN_INTERVAL PtaListenInterval;
#if MGMT_FRAME_PROTECTION
	/* WFM_DEVICE_OID_802_11_PROTECTED_MGMT_POLICY */
	uint32 procMgmtFrmPolicy;
	/* WFM_DEVICE_OID_802_11_SA_QUERY_INTERVAL */
	UMAC_SA_QUERY_INTERVAL saQueryInterval;
#endif	/* MGMT_FRAME_PROTECTION */
	uint32 useMultiTxConfMsg;
	/* UMAC_DEVICE_OID_802_11_MULTICAST_ADDR_FILTER */
	UMAC_MULTICAST_ADDR_FILTER multicastAddrFilter;
	/* UMAC_DEVICE_OID_802_11_KEEP_ALIVE_PERIOD */
	UMAC_KEEP_ALIVE_PERIOD keepAlivePeriod;
	/* UMAC_DEVICE_OID_802_11_WEP_DEFAULT_KEY_ID */
	UMAC_WEP_DEFAULT_KEY_ID wepDefaultKeyId;
	/* UMAC_DEVICE_OID_802_11_GET_DEVICE_MAC_ADDRESS */
	UMAC_DEVICE_MAC_ADDRESS deviceMacAddress;
	uint32 DisableActiveJoin;
#if P2P_MINIAP_SUPPORT
	uint32 IntraBssBridging;
	UMAC_CHANGE_CHANNEL_REQ ChangeChannel;
	UMAC_P2P_PS_MODE p2pPSMode;
#endif
	uint32 CurrentRateIndex;
	UMAC_NUM_CLIENTS NumOfClients;
	UMAC_LOCAL_AMP_INFO LocalAmpInfo;
	UMAC_AMP_ASSOC_INFO AmpAssocInfo;
	UMAC_SET_HIDDEN_AP_MODE HiddenAPMode;
#if TX_PER_SUPPORT
	UMAC_TX_STATS	TxStats;
#endif
	UMAC_SET_ARP_KEEP_ALIVE_PERIOD ArpKeepAlivePeriod;
} WFM_WLAN_PARAMETER_INFO;

typedef struct WFM_TRANSMIT_REQ_S	/* 4 byte aligned */
{
	uint32 Reference;
	uint32 Priority;
	uint32 Flags;		/* b0 : 1 ->means more frames available with
				   upper layer
				   0 ->no packets waiting to be
				   serviced
				   b1-b31 : Reserved
				 */
	WFM_802_3_FRAME EthernetFrame;
} WFM_TRANSMIT_REQ;

typedef struct WFM_TRANSMIT_CNF_S	/* 4 byte aligned */
{
	uint32 Reference;
	uint32 Status;
} WFM_TRANSMIT_CNF;

typedef struct WFM_RECEIVE_IND_S	/* 4 byte aligned */
{
	uint16 Priority;
	uint16 Flags;		/*
				   b0 set means there is a two byte padding
				   after this field.
				   b0 not set means there is no padded bytes.
				 */
	WFM_802_3_FRAME EthernetFrame;
} WFM_RECEIVE_IND;

typedef struct WFM_EVENT_IND_S {
	uint16 EventId;		/* WFM_WLAN_DEV_EVT */
	uint16 EventDataLength;
	uint8 EventData[4];	/* Event Data May follow */
} WFM_EVENT_IND;

typedef struct WFM_GET_PARAM_REQ_S	/* 4 byte aligned */
{
	uint32 Reference;
	uint16 ParameterId;	/* WFM_DEVICE_OID */
	uint16 ElementIndex;
} WFM_GET_PARAM_REQ;

typedef struct WFM_GET_PARAM_CNF_S	/* 4 byte aligned */
{
	uint32 Reference;
	uint32 Status;
	uint16 ParameterId;	/* WFM_DEVICE_OID */
	uint16 ParameterLength;
	WFM_WLAN_PARAMETER_INFO ParameterData;
} WFM_GET_PARAM_CNF;

typedef struct WFM_SET_PARAM_REQ_S	/* 4 byte aligned */
{
	uint32 Reference;
	uint16 ParameterId;	/* WFM_DEVICE_OID */
	uint16 ParameterLength;
	WFM_WLAN_PARAMETER_INFO ParameterData;
} WFM_SET_PARAM_REQ;

typedef struct WFM_SET_PARAM_CNF_S	/* 4 byte aligned */
{
	uint32 Reference;
	uint16 ParameterId;	/* WFM_DEVICE_OID */
	uint16 Status;
} WFM_SET_PARAM_CNF;

/******************************************************************************
			GENERIC HI MESSAGES
******************************************************************************/
typedef struct WFM_HI_DPD_S	/* 4 byte aligned */
{
	uint16 Length;		/* length in bytes of the DPD struct */
	uint16 Version;		/* version of the DPD record */
	uint8 MacAddress[6];	/* Mac addr of the device */
	uint16 Flags;		/* record options */
	uint32 PhyData[1];	/* 1st 4 bytes of the Phy data, TBD */
	/* rest of the Phy data */
} WFM_HI_DPD;

typedef struct WFM_CONFIG_REQ_S	/* 4 byte aligned */
{
	uint32 dot11MaxTransmitMsduLifeTime;
	uint32 dot11MaxReceiveLifeTime;
	uint32 dot11RtsThreshold;
	WFM_HI_DPD DpdData;
} WFM_CONFIG_REQ;

typedef struct WFM_CONFIG_CNF_S	/* 4 byte aligned */
{
	uint32 Result;
	uint8 dot11StationId[6];
	uint8 reserved[2];
} WFM_CONFIG_CNF;

typedef struct WFM_MEM_READ_REQ_S {
	uint32 Address;
	uint16 Length;
	uint16 Flags;
} WFM_MEM_READ_REQ;

typedef struct WFM_MEM_READ_CNF_S {
	uint32 Length;
	uint32 Result;
	uint32 Data[WFM_MEM_BLK_DWORDS];
} WFM_MEM_READ_CNF;

typedef struct WFM_MEM_WRITE_REQ_S {
	uint32 Address;
	uint16 Length;
	uint16 Flags;
	uint32 Data[WFM_MEM_BLK_DWORDS];
} WFM_MEM_WRITE_REQ;

typedef struct WFM_MEM_WRITE_CNF_S	/* 4 byte aligned */
{
	uint32 Result;

} WFM_MEM_WRITE_CNF;

/******************************************************************************
			END GENERIC HI MESSAGES
******************************************************************************/

typedef union WFM_HI_PAYLOAD_U	/* 4 byte aligned */
{
	WFM_TRANSMIT_REQ wfm_transmit_req;
	WFM_TRANSMIT_CNF wfm_transmit_cnf;
	WFM_RECEIVE_IND wfm_receive_ind;
	WFM_EVENT_IND wfm_event_ind;
	WFM_GET_PARAM_REQ wfm_get_param_req;
	WFM_GET_PARAM_CNF wfm_get_param_cnf;
	WFM_SET_PARAM_REQ wfm_set_param_req;
	WFM_SET_PARAM_CNF wfm_set_param_cnf;
	WFM_BSS_CACHE_INFO_IND wfm_bss_cache_info_ind;
	WFM_CONFIG_REQ wfm_config_req;
	WFM_CONFIG_CNF wfm_config_cnf;
	WFM_MEM_READ_REQ wfm_mem_read_req;
	WFM_MEM_READ_CNF wfm_mem_read_cnf;
	WFM_MEM_WRITE_REQ wfm_mem_write_req;
	WFM_MEM_WRITE_CNF wfm_mem_write_cnf;
} WFM_HI_PAYLOAD;

typedef enum WFM_HI_MESSAGE_ID_E {
	WFM_MEMORY_READ_REQ_ID,		/* 0x0000 */
	WFM_MEMORY_WRITE_REQ_ID,	/* 0x0001 */
	WFM_CONFIG_REQ_ID,		/* 0x0002 */
	WFM_GENERIC_REQ_ID,		/* 0x0003 */
	WFM_TRANSMIT_REQ_ID,		/* 0x0004 */
	WFM_GET_PARAM_REQ_ID,		/* 0x0005 */
	WFM_SET_PARAM_REQ_ID,		/* 0x0006 */

	/* Generic Confirmations */
	WFM_CNF_BASE = 0x0400,
	WFM_MEMORY_READ_CNF_ID = WFM_CNF_BASE,
	WFM_MEMORY_WRITE_CNF_ID,	/* 0x0401 */
	WFM_CONFIG_CNF_ID,		/* 0x0402 */
	WFM_GENERIC_CNF_ID,		/* 0x0403 */
	WFM_TRANSMIT_CNF_ID,		/* 0x0404 */
	WFM_GET_PARAM_CNF_ID,		/* 0x0405 */
	WFM_SET_PARAM_CNF_ID,		/* 0x0406 */

	/* Indications starts from 0x0800 */
	WFM_IND_BASE = 0x0800,
	WFM_EXCEPTION_IND_ID = WFM_IND_BASE,
	WFM_STARTUP_IND_ID,		/* 0x0801 */
	WFM_TRACE_IND_ID,		/* 0x0802 */
	WFM_GENERIC_IND_ID,		/* 0x0803 */
	WFM_RECEIVE_IND_ID,		/* 0x0804 */
	WFM_EVENT_IND_ID,		/* 0x0805 */
	WFM_BSS_CACHE_INFO_IND_ID,	/* 0x0806 */
	WFM_MAX_MESSAGE_ID_ID = WFM_BSS_CACHE_INFO_IND_ID + 1
} WFM_HI_MESSAGE_ID;

typedef struct WFM_HI_MESSAGE_S	/* 4 byte aligned */
{
	uint16 MessageLength;
	uint16 MessageId;
	WFM_HI_PAYLOAD PayLoad;
} WFM_HI_MESSAGE;

#pragma pack(pop)	/* release pragma */
#endif	/* _UMAC_MESSAGES_H */
