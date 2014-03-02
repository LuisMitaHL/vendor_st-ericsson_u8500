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
 * \file umac_dev_conf.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: lmac_if.h
 * \brief
 * This header file defines the LMAC interface APIs
 * \ingroup Upper_MAC_Core
 * \date 22/04/08 17:23
 */

#ifndef LMAC_IF_H
#define LMAC_IF_H

/*****************************************************************************/
/***			       Include Files			           ***/
/*****************************************************************************/

#include "stddefs.h"
#include "d0_defs.h"
#include "d11_defs.h"

#include "umac_interface.h"

#define LMAC_IF_VER		0x0003


/*****************************************************************************/
/*				Structures				     */
/*****************************************************************************/

/******************************************************************************
 * 6.2.3 Reset Lower MAC					              *
 *****************************************************************************/
typedef struct LMAC_RESET_CONFIRMATION_S {
	uint16 MessageLength;
	uint16 MessageId;
	uint32 Result;
} LMAC_RESET_CONFIRMATION;

/******************************************************************************
 * 6.2.6 Initiate Scan						              *
 *****************************************************************************/
#define LMAC_MAX_SSID_LENGTH		32

/* 11 11b/g channels + 23 11a channels worst case (FCC) scenario */
#define LMAC_MAX_CHANNELS		34
/*todo reduce to 2 with WTT as per spec */
#define LMAC_MAX_SSIDS			2

typedef struct LMAC_CHANNELS_S {
	uint32 ChannelNum;
	uint32 MinChannelTime;
	uint32 MaxChannelTime;
	sint32 TxPowerLevel;
} LMAC_CHANNELS;

typedef struct LMAC_SSIDS_S {
	uint32 SSIDLength;
	uint8 SSID[LMAC_MAX_SSID_LENGTH];
} LMAC_SSIDS;

typedef struct SCAN_PARAMETERS_S {
	uint8 Band;
	uint8 ScanType;
	uint8 ScanFlags;
	uint8 MaxTransmitRate;
	uint32 AutoScanInterval;
	uint8 NumOfProbeRequests;
	uint8 NumOfChannels;
	uint8 NumOfSSIDs;
	uint8 ProbeDelay;
	LMAC_CHANNELS Channels[LMAC_MAX_CHANNELS];
	LMAC_SSIDS Ssids[LMAC_MAX_SSIDS];
} SCAN_PARAMETERS;

/******************************************************************************
 * 6.2.8 Join							              *
 *****************************************************************************/
typedef struct JOIN_PARAMETERS_S {
	uint8 Mode;
	uint8 Band;
	uint16 ChannelNumber;
	uint8 BSSID[6];
	uint16 AtimWindow;
	uint8 PreambleType;
	uint8 ProbeForJoin;
	uint8 DTIMPeriod;
	uint8 Flags;
	uint32 SSIDLength;
	uint8 SSID[32];
	uint32 BeaconInterval;
	uint32 BasicRateSet;
} JOIN_PARAMETERS;

typedef struct JOIN_CONFIRM_S {
	uint32 Status;
	sint32 MinPowerLevel;
	sint32 MaxPowerLevel;
} JOIN_CONFIRM;

/******************************************************************************
 * 6.2.9 Set BSS Parameters					              *
 *****************************************************************************/
typedef struct BSS_PARAMETERS_S {
	uint8 Falgs;
	uint8 BeaconLostCount;
	uint16 AID;
	uint32 OperationalRateSet;
} BSS_PARAMETERS;

/******************************************************************************
 * 6.2.11 Set Tx Queue Parameters				              *
 *****************************************************************************/
typedef struct QUEUE_PARAMS_S {
	uint8 QueueId;
	uint8 Reserved2;
	uint8 AckPolicy;
	uint8 Reserved1;
	uint32 MaxTransmitLifetime;
	uint16 AllowedMediumTime;
	uint16 Reserved3;
} QUEUE_PARAMS;

/******************************************************************************
 * 6.2.12 Set EDCA Parameters					              *
 *****************************************************************************/
typedef struct EDCA_PARAMS_S {
	uint16 CwMin[4];
	uint16 CwMax[4];
	uint8 Aifsn[4];
	uint16 TxOpLimit[4];
	uint32 MaxReceiveLifetime[4];
} EDCA_PARAMS;

/******************************************************************************
 * 6.2.13 Read MIB & 6.2.14 Write MIB				              *
 *****************************************************************************/
/* Beacon filter related */
#define IE_F_CHANGED			(1<<0)
#define IE_F_NO_LONGER_PRESENT		(1<<1)
#define IE_F_APPEARED			(1<<2)

typedef struct MIB_READ_DATA_S {
	uint32 Status;
	uint16 MibId;
	uint16 Length;
	uint8 MibData[4];
} MIB_READ_DATA;

typedef struct MIB_PARAMETERS_S {
	uint16 MibId;
	uint16 Length;
	uint8 MibData[4];
} MIB_PARAMETERS;

/******************************************************************************
 * 6.2.19 Set Template Frame					              *
 *****************************************************************************/
typedef struct TEMPLATE_FRAME_S {
	uint8 FrameType;	/*
				   0 - Beacon 1 - Probe Request 2 NULL data
				   3 - Probe Response 4 - QOS NULL Data
				*/
	uint8 Rate;		/*
				   Bit 7 -> 0 or 1 for Mixed/Greenfield
				   mode selection resp.
				*/
	uint16 FrameLength;
	uint8 *pFrame;
} TEMPLATE_FRAME;

/***********************************************************************
 * 6.2.20 Add Key Entry						*
 ***********************************************************************/
#define MAX_KEY_ENTRIES	 11

enum EHI_Encryption {
	eEncWEPDEFAULT = 0,
	eEncWEPPAIRWISE = 1,
	eEncTKIPGROUP = 2,
	eEncTKIPPAIRWISE = 3,
	eEncAESGROUP = 4,
	eEncAESPAIRWISE = 5,
	eEncWAPIGROUP = 6,
	eEncWAPIPAIRWISE = 7,
#if MGMT_FRAME_PROTECTION
	eEncIGTKGROUP = 8,
	eEncNONE = 9
#else
	eEncNONE = 8
#endif	/* MGMT_FRAME_PROTECTION */
};

typedef struct PRIVACY_KEY_DATA_S {
	uint8 KeyType;
	uint8 KeyEntryIndex;
	uint8 Reserved[2];
	uint8 Key[40];
} PRIVACY_KEY_DATA;

typedef union LMC_KEY_DATA_U {
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
		uint8 KeyId;
		uint8 Reserved;
		uint8 TkipKeyData[16];
		uint8 RxMicKey[8];
		uint8 TxMicKey[8];
	} TkipPairwiseKey;

	struct {
		uint8 TkipKeyData[16];
		uint8 RxMicKey[8];
		uint8 KeyId;
		uint8 Reserved[3];
		uint8 ReceiveSequenceCounter[8];
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
		uint8 ReceiveSequenceCounter[8];
	} AesGroupKey;

	struct {
		uint8 PeerAddress[6];
		uint8 Reserved[2];
	} GenericPairwiseKey;
} LMC_KEY_DATA;

/******************************************************************************
 * 6.2.xx Set Association Mode					              *
 *****************************************************************************/
#define SAM_F__PREAMBLE_ENABLE		(1<<0)
#define SAM_F__MIXED_GREEN_ENABLE	(1<<1)
#define SAM_F__BASICRATESET_ENABLE	(1<<2)
#define SAM_F__MPDUSTARTSPACING_ENABLE	(1<<3)

/******************************************************************************
 * 6.2.xx Switch Channel					              *
 *****************************************************************************/
typedef struct SWITCH_CHANNEL_S {
	uint8 ChannelMode;
	uint8 ChannelSwitchCount;
	uint16 NewChannelNumber;
} SWITCH_CHANNEL;

/******************************************************************************
 * 6.3.3 Scan Complete						              *
 *****************************************************************************/
typedef struct SCAN_COMPLETE_S {
	uint32 Status;
	uint8 PmMode;
	uint8 NumChannelsCompleted;
	uint16 Reserved;
} SCAN_COMPLETE;

/******************************************************************************
 * 6.3.4 Set PM Mode Complete					              *
 *****************************************************************************/
typedef struct SET_PM_MODE_COMPLETE_S {
	uint32 Result;
	uint8 PmMode;
	uint8 Reserved[3];
} SET_PM_MODE_COMPLETE;

/******************************************************************************
 * 6.3.x Event Indicate						              *
 *****************************************************************************/
#define LMAC_EVENT_IND_ERROR		0
#define LMAC_EVENT_IND_BSSLOST		1
#define LMAC_EVENT_IND_BSSREGAINED	2
#define LMAC_EVENT_IND_RADAR		3
#define LMAC_EVENT_IND_RCPI_RSSI	4
#define LMAC_EVENT_IND_BT_INACTIVE	5
#define LMAC_EVENT_IND_BT_ACTIVE	6

typedef struct EVENT_INDICATION_S {
	uint32 EventId;
	uint32 EventData;
} EVENT_INDICATION;

/******************************************************************************
 * 6.3.8 Block Ack Timeout					              *
 *****************************************************************************/
typedef struct BLOCK_ACK_TIMEOUT_S {
	uint32 Reserved1;	/* reserved for use by WSM adaptation layer */
	uint8 TID;
	uint8 Reserved2;
	uint8 TransmitAddress[6];
} BLOCK_ACK_TIMEOUT;

#endif	/* LMAC_IF_H */
