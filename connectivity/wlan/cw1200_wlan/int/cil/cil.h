/* =============================================================================
 *
 *Linux Driver for CW1200 series
 *
 *
 *Copyright (c) ST-Ericsson SA, 2010
 *
 *This program is free software; you can redistribute it and/or modify it
 *under the terms of the GNU General Public License version 2 as published
 *by the Free Software Foundation.
 *
 * ========================================================================== */
/**
 *addtogroup Linux Driver CIL Layer
 *brief
 *
 */
/**
 *file cil.h
 *- <b>PROJECT</b>			 : CW1200_LINUX_DRIVER
 *- <b>FILE</b>				: cil.h
 *brief
 *This module interfaces with the Linux Kernel CFG80211 layer.
 *ingroup CIL
 *date 25/02/2010
 */


/*****************************************************************************
*								INCLUDE FILES
******************************************************************************/
#ifndef __CW1200_HEADER__
#define __CW1200_HEADER__
#include "cw1200_common.h"

#define IP_ALEN 4  /*length of IP Address*/
#define WAIT_TIME   5000   /*In millisecs */
#define 	MAX_PROBE 15
#define IE_LEN_MAX 200
#define RX_BLOCK_ACK_TIDS  0xff
#define TX_BLOCK_ACK_TIDS  0xff  /*0-3 dont work in CUT1.1. Will be fixed in CUT2.0 */

#define RSSI_RCPI_MODE     0x03 /* Currently using only RSSI for Lower&Upper Thresh*/
#define RSSI_DISABLED      0x00 /* disable RSSI flag*/
#define DEFAULT_BEACON_INT	100

#define WPS_IE_VENDOR_TYPE 0x0050f204
#define WMM_IE_VENDOR_TYPE 0x0050f202
#define P2P_IE_VENDOR_TYPE 0x506f9a09

#define CONNECT_TIMEOUT 5*HZ

#define COOKIE_REM_ON_CHAN 	1234567890
#define COOKIE_MGMT_TX 		987654321

#define GET_BE32(a) ((((u32) (a)[0]) << 24) | (((u32) (a)[1]) << 16) | \
			 (((u32) (a)[2]) << 8) | ((u32) (a)[3]))

#define GET_BE16(a) ((u16) (((a)[0] << 8) | (a)[1]))

#define PUT_LE32(a, val)					\
	do {							\
		(a)[3] = (u8) ((((u32) (val)) >> 24) & 0xff);	\
		(a)[2] = (u8) ((((u32) (val)) >> 16) & 0xff);	\
		(a)[1] = (u8) ((((u32) (val)) >> 8) & 0xff);	\
		(a)[0] = (u8) (((u32) (val)) & 0xff);		\
		(a) += 4;						\
	} while (0)

#define PUT_LE16(a, val)			\
	do {					\
		(a)[1] = (u8) (((u16) (val)) >> 8);	\
		(a)[0] = (u8) (((u16) (val)) & 0xff);	\
		(a) += 2;				\
	} while (0)

#define PUT_U8(a, val)			\
	do {				\
		(*a) = (val) & 0xff;	\
		a++;			\
	} while (0)


#define CQM_BEACON_LOSS		20
#define CQM_LINK_LOSS		80
#define P2P_OPS_ENABLE		0x80
#define CIL_P2P_OPS		1
#define CIL_P2P_NOA		2
/* Generate Frame Control */
#define IEEE80211_FC(type, stype) cpu_to_le16((type << 2) | (stype << 4))
#define NOA_ATTR_LEN			15
#define NOA_DESC_LEN			13

#define RSSI_THRESHOLD			0

struct wmm_ac_parameter {
	u8 aci_aifsn; /* AIFSN, ACM, ACI */
	u8 cw; /* ECWmin, ECWmax (CW = 2^ECW - 1) */
	uint16_t txop_limit;
}__attribute__ ((packed));

struct wmm_parameter_element {
	/* Element ID: 221 (0xdd); Length: 24 */
	/* required fields for WMM version 1 */
	u8 oui[3]; /* 00:50:f2 */
	u8 oui_type; /* 2 */
	u8 oui_subtype; /* 1 */
	u8 version; /* 1 for WMM version 1.0 */
	u8 qos_info; /* AP/STA specif QoS info */
	u8 reserved; /* 0 */
	struct wmm_ac_parameter ac[4]; /* AC_BE, AC_BK, AC_VI, AC_VO */
} __attribute__ ((packed));

/*****************************************************************************
*						CIL Global Function prototypes
******************************************************************************/
CW1200_STATUS_E CIL_Init(struct CW1200_priv *priv);
CW1200_STATUS_E CIL_Shutdown(struct CW1200_priv *priv);
CW1200_STATUS_E cw1200_stop_scan(struct CW1200_priv *priv);

void UMI_CB_Indicate_Event(UL_HANDLE UpperHandle,
				UMI_STATUS_CODE Status,
				UMI_EVENT Event,
				uint16_t EventDataLength,
				void *EventData);

void UMI_CB_GetParameterComplete(UL_HANDLE UpperHandle,
				uint16_t Oid,
				UMI_STATUS_CODE Status,
				uint16_t Length,
				void *pValue);

void UMI_CB_SetParameterComplete(UL_HANDLE UpperHandle,
				uint16_t Oid,
				UMI_STATUS_CODE  Status);

void *CIL_Get(struct CW1200_priv *priv,
				enum UMI_DEVICE_OID_E oid_to_get,
				int32_t length);

CW1200_STATUS_E CIL_Set(struct CW1200_priv *priv,
				enum UMI_DEVICE_OID_E oid_to_set,
				void *oid_data,
				int32_t length);

int UMI_mc_list(struct CW1200_priv *priv);

void UMI_CB_ScanInfo(UL_HANDLE UpperHandle,
				uint32  cacheInfoLen,
				UMI_BSS_CACHE_INFO_IND *pCacheInfo);

int CIL_get_sta_linkid(struct CW1200_priv *priv,const uint8_t * mac_addr);

void UMI_CB_TxMgmtFrmComplete(UL_HANDLE ulHandle,
			UMI_TX_MGMT_FRAME_COMPLETED *pTxMgmtData);

typedef enum {
	UMI_OPEN_SYSTEM = 0,
	UMI_SHARED_KEY,
	UMI_WPA_TKIP,
	UMI_WPA_PSK,
	UMI_WPA,
	UMI_WPA2_AES,
	UMI_WPA2_PSK,
	UMI_WAPI_1,
} CW1200_AUTH_MODE_E;

typedef enum {
	WEP_GROUP = 0,
	WEP_PAIR,
	TKIP_GROUP,
	TKIP_PAIR,
	AES_GROUP,
	AES_PAIR,
	WAPI_GROUP,
	WAPI_PAIR,
} CW1200_KEY_TYPE_E;

typedef enum {
	CW1200_DISCONNECTED = 0,
	CW1200_CONNECTING,
	CW1200_CONNECTED,
} CW1200_CONNECTION_STATUS_E;

struct cfg_priv {
	struct CW1200_priv *driver_priv;
};

/********OPERATIONAL POWER MODE  Macros******/
#define WLAN_ACTIVE_MODE	0
#define WLAN_DOZE_MODE		1
#define WLAN_QUIESCENT_MODE 	2
/*************/
/********WPA Macros******/
#undef WLAN_AKM_SUITE_8021X
#undef WLAN_AKM_SUITE_PSK
#define WLAN_AKM_SUITE_8021X          0x000FAC01
#define WLAN_AKM_SUITE_PSK            0x000FAC02
#define WPA_AKM_SUITE_802_1X          0x0050F201
#define WPA_AKM_SUITE_PSK             0x0050F202

#define WPA_1 1
#define WPA_2 2
/*************/
#endif
