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
 * - <b>FILE</b>	: umac_dev_conf.h
 * \brief
 * This header file defines the capabilities of UMAC, and the
 * default values with which UMAC is operating
 * \ingroup Upper_MAC_Core
 * \date 22/04/08 17:23
 */

#ifndef _UMAC_DEV_CONF_H
#define _UMAC_DEV_CONF_H

#include "umac_interface.h"
#include "umac_dbg.h"

/******************************************************************************
			      UMAC FEATURES
******************************************************************************/
/* To be removed later, once the TPC is implemented in the firmware */
#define USE_DEFAULT_POWER			0

/******************************************************************************
			      UMAC CONSTANTS
******************************************************************************/

/*Time to listen on each channel*/
#define WFM_UMAC_ACTIVE_SCAN_MAX_CHANNEL_TIME		40

#define WFM_UMAC_ACTIVE_SCAN_MIN_CHANNEL_TIME		10

#define WFM_UMAC_PASSIVE_SCAN_MAX_CHANNEL_TIME		110

#define WFM_UMAC_PASSIVE_SCAN_MIN_CHANNEL_TIME		50

#define WFM_UMAC_DEFAULT_POWER_LEVEL			128

#define WFM_UMAC_MAX_DEVICE_POWER			300

#define WFM_UMAC_MIN_DEVICE_POWER			0

#define WFM_UMAC_POWER_LEVEL_FACTOR			10

#define WFM_UMAC_PROB_FOR_JOIN				0

/*
  Timeout for the management frames, if not getting a response before this,
  UMAC will go back to UMAC_INIT state.Using 1 Seconds now
*/
/* 1  sec */
#define WFM_UMAC_MGMT_FRAME_TIMEOUT		 TIME_SEC_TO_TGT_UTS(2)

/*
  The elapsed time in TUs, after the initial transmission of an MSDU, after
  which further attempts to transmit the MSDU shall be terminated. Overrides
  the global dot11MaxTransmitMsduLifeTime setting [optional]
*/
#define WFM_UMAC_TX_EXPIRE_TIME			200

/*
  Number of milliseconds to be waited before throwing away a defrgmented frame
*/

#define WFM_MAX_RX_LIFE_TIME			200

#define WFM_DEFAULT_BECON_LOST_COUNT		50

#define UMAC_SET_BSS_F_BEACON_LOST_COUNT_ONLY	0x01
#define UMAC_SET_BSS_F_AID			0x00
/*
  Size of UMAC event queue
*/
#define WFM_UMAC_MAX_SIZE_EVT_QUEUE		4

/* for 6.5Mbps, bit 14 is set */
#define UMAC_HT_LOWEST_RATE_BIT_POS		14

/* for 65Mbps, bit 22 is set */
#define UMAC_HT_HIGHEST_RATE_BIT_POS		22

#define UMAC_NUM_INTERNAL_TX_BUFFERS		128
/*
  Following macro should always have the value of the LMAC
  Macro MAX_KEY_ENTRIES
*/
#define UMAC_MAX_KEY_ENTRIES			9

/* [0,5];  0: For AP; 1-8 Clients */
#define MAX_NUM_CLIENTS_SUPPORTED		9

#define UMAC_WAI_AUTH_ETHER_TYPE		0x88B4

/* Following macros for Transmit request FALGS */
#define UMAC_TX_REQ_FLAG_FRAME_CTRL_OFFSET_ENABLED	0x80

#define UMAC_RATE_1		0x00000001
#define UMAC_RATE_2		0x00000002
#define UMAC_RATE_5_5		0x00000004
#define UMAC_RATE_11		0x00000008
#define UMAC_RATE_22		0x00000010
#define UMAC_RATE_33		0x00000020
#define UMAC_RATE_6		0x00000040
#define UMAC_RATE_9		0x00000080
#define UMAC_RATE_12		0x00000100
#define UMAC_RATE_18		0x00000200
#define UMAC_RATE_24		0x00000400
#define UMAC_RATE_36		0x00000800
#define UMAC_RATE_48		0x00001000
#define UMAC_RATE_54		0x00002000
/*
   Note: if value of UMAC_RATE_6_5 etc change, then WFM_SUP_MCS_RX_MCS_BMAP_0
   need to be changed as well
*/
#define UMAC_RATE_6_5		0x00004000
#define UMAC_RATE_13		0x00008000
#define UMAC_RATE_19_5		0x00010000
#define UMAC_RATE_26		0x00020000
#define UMAC_RATE_39		0x00040000
#define UMAC_RATE_52		0x00080000
#define UMAC_RATE_58_5		0x00100000
#define UMAC_RATE_65		0x00200000

#define UMAC_RATE_11B		0x0000000F
#define RATE_INDEX_B_1M		0
#define RATE_INDEX_B_2M		1
#define RATE_INDEX_B_5_5M	2
#define RATE_INDEX_B_11M	3
#define RATE_INDEX_PBCC_22M	4
#define RATE_INDEX_PBCC_33M	5
#define RATE_INDEX_A_6M		6
#define RATE_INDEX_A_9M		7
#define RATE_INDEX_A_12M	8
#define RATE_INDEX_A_18M	9
#define RATE_INDEX_A_24M	10
#define RATE_INDEX_A_36M	11
#define RATE_INDEX_A_48M	12
#define RATE_INDEX_A_54M	13
#define RATE_INDEX_N_6_5M	14
#define RATE_INDEX_N_13M	15
#define RATE_INDEX_N_19_5M	16
#define RATE_INDEX_N_26M	17
#define RATE_INDEX_N_39M	18
#define RATE_INDEX_N_52M	19
#define RATE_INDEX_N_58_5M	20
#define RATE_INDEX_N_65M	21

/*All the possible rates supported by the chip */
#define UMAC_SUPPORTED_RATES		\
	(UMAC_RATE_1			\
	| UMAC_RATE_2			\
	| UMAC_RATE_5_5			\
	| UMAC_RATE_11			\
	| UMAC_RATE_6			\
	| UMAC_RATE_9			\
	| UMAC_RATE_12			\
	| UMAC_RATE_18			\
	| UMAC_RATE_24			\
	| UMAC_RATE_36			\
	| UMAC_RATE_48			\
	| UMAC_RATE_54			\
	| UMAC_RATE_6_5			\
	| UMAC_RATE_13			\
	| UMAC_RATE_19_5		\
	| UMAC_RATE_26			\
	| UMAC_RATE_39			\
	| UMAC_RATE_52			\
	| UMAC_RATE_58_5		\
	| UMAC_RATE_65)

#define UMAC_PHY_BAND_2_4G_SUPPORTED_RATES    UMAC_SUPPORTED_RATES

#define UMAC_PHY_BAND_5G_SUPPORTED_RATES				\
	(								\
	UMAC_SUPPORTED_RATES						\
	&								\
	(~(UMAC_RATE_1 | UMAC_RATE_2 | UMAC_RATE_5_5 | UMAC_RATE_11))	\
	)

/* Rates that we want to support in fw */
/*
   Change here if want to change default value of supported rate . Or, change
   UmacInstance.OurAllSupportedRates
*/
#define UMAC_SUPPORTED_RATES_OURS_DEFAULT  (			\
	(UMAC_RATE_1						\
	  | UMAC_RATE_2						\
	  | UMAC_RATE_5_5					\
	  | UMAC_RATE_11					\
	  | UMAC_RATE_6						\
	  | UMAC_RATE_9						\
	  | UMAC_RATE_12					\
	  | UMAC_RATE_18					\
	  | UMAC_RATE_24					\
	  | UMAC_RATE_36					\
	  | UMAC_RATE_48					\
	  | UMAC_RATE_54					\
	  | UMAC_RATE_6_5					\
	  | UMAC_RATE_13					\
	  | UMAC_RATE_19_5					\
	  | UMAC_RATE_26					\
	  | UMAC_RATE_39					\
	  | UMAC_RATE_52					\
	  | UMAC_RATE_58_5					\
	  | UMAC_RATE_65					\
	    )							\
	&							\
	UMAC_SUPPORTED_RATES)	/*
				   UMAC_SUPPORTED_RATES is max what our device
				   can support
				*/

/* Basic Rates that we want to support in fw */
/*
   Change here if want to change default value of basic supported rate.
   Or, change UmacInstance.OurAllBasicSupportedRates
*/
#define UMAC_BASIC_SUPPORTED_RATES_OURS_DEFAULT  (		\
	(UMAC_RATE_1						\
	  | UMAC_RATE_2						\
	  | UMAC_RATE_5_5					\
	  | UMAC_RATE_11					\
	  | UMAC_RATE_6						\
	  | UMAC_RATE_9						\
	  | UMAC_RATE_12					\
	  | UMAC_RATE_18					\
	  | UMAC_RATE_24					\
	  | UMAC_RATE_36					\
	  | UMAC_RATE_48					\
	  | UMAC_RATE_54					\
	  | UMAC_RATE_6_5					\
	  | UMAC_RATE_13					\
	  | UMAC_RATE_19_5					\
	  | UMAC_RATE_26					\
	  | UMAC_RATE_39					\
	  | UMAC_RATE_52					\
	  | UMAC_RATE_58_5					\
	  | UMAC_RATE_65					\
	    )							\
	&							\
	UMAC_SUPPORTED_RATES_OURS_DEFAULT)	/*
						   it cannot be a something
						   which is not a supported
						   rate - eg, 9mbps is removed
						   from supp rate for checking
						*/

/*Rates Supported in Different Modulation Schemes */

#define WFM_ERP_DSSS_CCK_RATES_SUPPORTED  (			\
	(UMAC_RATE_1						\
	  | UMAC_RATE_2						\
	  | UMAC_RATE_5_5					\
	  | UMAC_RATE_11					\
	    ) & UMAC_SUPPORTED_RATES)	/*
					   cannot be more than what our device
					   supports, hence the masking
					*/

#define WFM_DSSS_CCK_RATES_EPTA  (				\
	(UMAC_RATE_1						\
	  | UMAC_RATE_2						\
	  | UMAC_RATE_5_5					\
	    ) & UMAC_SUPPORTED_RATES)	/*
					   cannot be more than what our device
					   supports, hence the masking
					*/

#define WFM_ERP_OFDM_RATES_SUPPORTED      (			\
	(UMAC_RATE_6						\
	  | UMAC_RATE_9						\
	  | UMAC_RATE_12					\
	  | UMAC_RATE_18					\
	  | UMAC_RATE_24					\
	  | UMAC_RATE_36					\
	  | UMAC_RATE_48					\
	  | UMAC_RATE_54					\
	    ) & UMAC_SUPPORTED_RATES)	/*
					   cannot be more than what our device
					   supports, hence the masking
					*/

#define WFM_ERP_OFDM_RATES_MANDATORY      (			\
	(UMAC_RATE_6						\
	  | UMAC_RATE_12					\
	  | UMAC_RATE_24					\
	    ) & UMAC_SUPPORTED_RATES )	/*
					   cannot be more than what our device
					   supports, hence the masking
					*/

#define WFM_HT_OFDM_RATES_SUPPORTED       (			\
	(UMAC_RATE_6_5						\
	  | UMAC_RATE_13					\
	  | UMAC_RATE_19_5					\
	  | UMAC_RATE_26					\
	  | UMAC_RATE_39					\
	  | UMAC_RATE_52					\
	  | UMAC_RATE_58_5					\
	  | UMAC_RATE_65					\
	    ) & UMAC_SUPPORTED_RATES)	/*
					   cannot be more than what our device
					   supports, hence the masking
					*/

#if P2P_MINIAP_SUPPORT
#define UMAC_NUM_AP_INTERNAL_TX_BUFFERS		32
/******************************************************************************
	    STATUS CODES
******************************************************************************/
#define WFM_UMAC_SC_SUCCESSFUL				0
#define WFM_UMAC_SC_UNSPECIFIED				1
#define WFM_UMAC_SC_UNSUPPORTED_CAPABILITIES		10
#define WFM_UMAC_SC_NO_PREV_ASSOCIATION			11
#define WFM_UMAC_SC_OUTSIDE_SCOPE			12
#define WFM_UMAC_SC_UNSUPPORTED_AUTH_ALG		13
#define WFM_UMAC_SC_OUT_OF_SEQUENCE			14
#define WFM_UMAC_SC_CHALLENGE_FAILURE			15
#define WFM_UMAC_SC_TIMEOUT				16
#define WFM_UMAC_SC_NO_ADDITIONAL_ASSOCIATION		17
#define WFM_UMAC_SC_UNSUPPORTED_BASIC_RATES		18
#define WFM_UMAC_SC_UNSUPPORTED_SHORT_PREAMBLE		19
#define WFM_UMAC_SC_SPECTRUM_MANAGEMENT_REQD		22
#define WFM_UMAC_SC_POWER_CAPABILITY_UNACCEPTABLE	23
#define WFM_UMAC_SC_UNSUPPORTED_CHANNELS		24
#define WFM_UMAC_SC_UNSUPPORTED_SHORT_SLOT		25
#define WFM_UMAC_SC_INVALID_INFORMATION_ELEMENT		40

#endif	/* P2P_MINIAP_SUPPORT */

/******************************************************************************
	    CAPABILITY INFORMATION FIELDS
******************************************************************************/

#define WFM_UMAC_CAPABILITY_ESS			0x0001
#define WFM_UMAC_CAPABILITY_IBSS		0x0002
#define WFM_UMAC_CAPABILITY_CF_POLLABLE		0x0004
#define WFM_UMAC_CAPABILITY_CF_POLL_REQ		0x0008
#define WFM_UMAC_CAPABILITY_PRIVACY		0x0010
#define WFM_UMAC_CAPABILITY_SHORT_PREAMBLE	0x0020
#define WFM_UMAC_CAPABILITY_PBCC		0x0040
#define WFM_UMAC_CAPABILITY_CHANNEL_AGILITY	0x0080
#define WFM_UMAC_CAPABILITY_SPECTRUM_MGMT	0x0100
#define WFM_UMAC_CAPABILITY_QOS			0x0200
#define WFM_UMAC_CAPABILITY_SHORT_SLOT_TIME	0x0400
#define WFM_UMAC_CAPABILITY_APSD		0x0800
#define WFM_UMAC_CAPABILITY_RADIO_MEASUREMENT	0x1000
#define WFM_UMAC_CAPABILITY_DSSS_OFDM		0x2000
#define WFM_UMAC_CAPABILITY_DELAYED_BLK_ACK	0x4000
#define WFM_UMAC_CAPABILITY_IMMEDIATE_BLK_ACK	0x8000

/* The capabilities of WBF WFM device*/
#define WFM_UMAC_SUPPORTED_CAPABILITIES		(WFM_UMAC_CAPABILITY_ESS \
						 | WFM_UMAC_CAPABILITY_SHORT_PREAMBLE \
						 | WFM_UMAC_CAPABILITY_SPECTRUM_MGMT \
						 | WFM_UMAC_CAPABILITY_QOS \
						 | WFM_UMAC_CAPABILITY_SHORT_SLOT_TIME \
						 | WFM_UMAC_CAPABILITY_PRIVACY \
						 | WFM_UMAC_CAPABILITY_IMMEDIATE_BLK_ACK)

#if DOT11K_SUPPORT
#define UMAC_RRMCAPABILITY_LINK_MEASUREMENT		0x00000001
#define UMAC_RRMCAPABILITY_NEIGHBOR_REPORT		0x00000002
#define UMAC_RRMCAPABILITY_REPEATED_MEASUREMENT		0x00000008
#define UMAC_RRMCAPABILITY_BEACON_PASSIVE		0x00000010
#define UMAC_RRMCAPABILITY_BEACON_ACTIVE		0x00000020
#define UMAC_RRMCAPABILITY_BEACON_TABLE			0x00000040
#define UMAC_RRMCAPABILITY_BEACON_MEASUREMENT_REPORTING	0x00000080
#define UMAC_RRMCAPABILITY_FRAME_MEASUREMENT		0x00000100
#define UMAC_RRMCAPABILITY_CHANNEL_LOAD			0x00000200
#define UMAC_RRMCAPABILITY_NOISE_HISTOGRAM		0x00000400
#define UMAC_RRMCAPABILITY_STATISTICS			0x00000800
#define UMAC_RRMCAPABILITY_LCI_MEASUREMENT		0x00001000
#define UMAC_RRMCAPABILITY_LCI_AZIMUTH			0x00002000
#define UMAC_RRMCAPABILITY_TRANSMIT_STREAM		0x00004000
#define UMAC_RRMCAPABILITY_MEASUREMENT_PILOT		0x01000000

/*Measurement Request Elements Type */
/*Spectrum Measurement request Types*/
#define UMAC_MEASUREMENT_REQ_TYPE_BASIC			0
#define UMAC_MEASUREMENT_REQ_TYPE_CCA			1
#define UMAC_MEASUREMENT_REQ_TYPE_RPI_HISTOGRAM		2
/*Radio measurement Request Types*/
#define UMAC_MEASUREMENT_REQ_TYPE_CHANNEL_LOAD		3
#define UMAC_MEASUREMENT_REQ_TYPE_NOISE_HISTOGRAM	4
#define UMAC_MEASUREMENT_REQ_TYPE_BEACON		5
#define UMAC_MEASUREMENT_REQ_TYPE_FRAME			6
#define UMAC_MEASUREMENT_REQ_TYPE_STA_STATISTICS	7
#define UMAC_MEASUREMENT_REQ_TYPE_LCI			8
#define UMAC_MEASUREMENT_REQ_TYPE_TRANSMIT_STREAM	9
/*This is not in standard.It is mentioned here just for UMAC*/
#define UMAC_MEASUREMENT_REQ_TYPE_LINK_MEAS		10
#define UMAC_MEASUREMENT_REQ_TYPE_MEASUREMENT_PAUSE	255

/*Measurement Mode for Beacon*/
#define UMAC_MEAS_MODE_BEACON_TABLE			2
/*Spectrum Measurement Report Types*/
#define UMAC_MEASUREMENT_REPORT_TYPE_BASIC		0
#define UMAC_MEASUREMENT_REPORT_TYPE_CCA		1
#define UMAC_MEASUREMENT_REPORT_TYPE_RPI_HISTOGRAM	2
/*Radio measurement Reprort Types*/
#define UMAC_MEASUREMENT_REPORT_TYPE_CHANNEL_LOAD	3
#define UMAC_MEASUREMENT_REPORT_TYPE_NOISE_HISTOGRAM	4
#define UMAC_MEASUREMENT_REPORT_TYPE_BEACON		5
#define UMAC_MEASUREMENT_REPORT_TYPE_FRAME		6
#define UMAC_MEASUREMENT_REPORT_TYPE_STA_STATISTICS	7
#define UMAC_MEASUREMENT_REPORT_TYPE_LCI		8
#define UMAC_MEASUREMENT_REPORT_TYPE_TRANSMIT_STREAM	9

/*Default length for measurement request elements */
#define UMAC_DEFAULT_CHANNEL_LOAD_LEN			6
#define UMAC_DEFAULT_NOISE_HIST_LEN			6
#define UMAC_DEFAULT_BEACON_REQ_LEN			13
#define UMAC_DEFAULT_FRAME_REQ_LEN			13
#define UMAC_DEFAULT_STATISTICS_LEN			11
#define UMAC_DEFAULT_LCI_LEN				4
#define UMAC_DEFAULT_TRANSMIT_STREAM_LEN		12
#define UMAC_DEFAULT_MEAS_PAUSE_LEN			2

#define UMAC_SUPPORTED_DEFAULT_CAPABILITES		(UMAC_RRMCAPABILITY_REPEATED_MEASUREMENT \
							    | UMAC_RRMCAPABILITY_MEASUREMENT_PILOT)

#define UMAC_SUPPORTED_BEACON_CAPABILITIES		(UMAC_RRMCAPABILITY_BEACON_PASSIVE \
							    | UMAC_RRMCAPABILITY_BEACON_ACTIVE \
							    | UMAC_RRMCAPABILITY_BEACON_TABLE)

#define UMAC_SUPPORTED_LCI_CAPABILITIES			(UMAC_RRMCAPABILITY_LCI_MEASUREMENT \
							    | UMAC_RRMCAPABILITY_LCI_AZIMUTH)

#define UMAC_ENABLE_MASK				(1<<1)

#endif	/* DOT11K_SUPPORT */

/* HT - Capabilities*/
#define WFM_HT_CAP_LDPC_CODING_CAPABILITY		(1 << 0)
/* Support both 20 and 40 Mhz channels */
#define WFM_HT_CAP_SUPPORTED_CHANNEL_WIDTH_SET		(1 << 1)
/* 2 bits */
#define WFM_HT_CAP_SM_POWER_SAVE_STATIC			(0 << 2)
/* 2 bits */
#define WFM_HT_CAP_SM_POWER_SAVE_DYNAMIC		(1 << 2)
/* 2 bits */
#define WFM_HT_CAP_SM_POWER_SAVE_RESERVED		(2 << 2)
/* 2 bits */
#define WFM_HT_CAP_SM_POWER_SAVE_DISABLED		(3 << 2)
#define WFM_HT_CAP_GREEN_FIELD				(1 << 4)
#define WFM_HT_CAP_SHORT_GI_20_MHZ			(1 << 5)
#define WFM_HT_CAP_SHORT_GI_40_MHZ			(1 << 6)
#define WFM_HT_CAP_TX_STBC				(1 << 7)
/* 2 bits */
#define WFM_HT_CAP_RX_STBC				(1 << 8)
/* 2 bits */
#define WFM_HT_CAP_RX_STBC_ONE_SPATIAL			(1 << 8)
/* 2 bits */
#define WFM_HT_CAP_RX_STBC_ONE_AND_TWO_SPATIAL		(2 << 8)
/* 2 bits */
#define WFM_HT_CAP_RX_STBC_ONE_TWO_THREE_SPATIAL	(3 << 8)
#define WFM_HT_CAP_DELAYED_BLK_ACK			(1 << 10)
#define WFM_HT_CAP_MAX_A_MSDU_LENGTH_3849		(0 << 11)
#define WFM_HT_CAP_MAX_A_MSDU_LENGTH_7935		(1 << 11)
#define WFM_HT_CAP_DSSS_CCK_MODE_IN_40_MHZ		(1 << 12)
#define WFM_HT_CAP_PSMP_SUPPORT				(1 << 13)
#define WFM_HT_CAP_FOURTY_MH_INTOLERANT			(1 << 14)
#define WFM_HT_CAP_L_SIG_TXOP_PROTECTION_SUPPORT	(1 << 15)

/* TBD: Check again when 40Mhz is supported */
#define WFM_UMAC_HT_CAP_OURS			\
	(WFM_HT_CAP_SM_POWER_SAVE_DISABLED	\
	| WFM_HT_CAP_GREEN_FIELD		\
	| WFM_HT_CAP_RX_STBC			\
	| WFM_HT_CAP_DELAYED_BLK_ACK		\
	| WFM_HT_CAP_MAX_A_MSDU_LENGTH_3849	\
	)

/*HT- A-MPDU Parameters*/
#define WFM_AMPDU_PARAM_MAX_LEN_EXP_8191			(0 << 0)
#define WFM_AMPDU_PARAM_MAX_LEN_EXP_16383			(1 << 0)
#define WFM_AMPDU_PARAM_MAX_LEN_EXP_32767			(2 << 0)
#define WFM_AMPDU_PARAM_MAX_LEN_EXP_65535			(3 << 0)

#define WFM_AMPDU_PARAM_MIN_MPDU_START_SPACING_NO_RESTRICTION	(0 << 2)
#define WFM_AMPDU_PARAM_MIN_MPDU_START_SPACING_1_4_US		(1 << 2)
#define WFM_AMPDU_PARAM_MIN_MPDU_START_SPACING_1_2_US		(2 << 2)
#define WFM_AMPDU_PARAM_MIN_MPDU_START_SPACING_1_US		(3 << 2)
#define WFM_AMPDU_PARAM_MIN_MPDU_START_SPACING_2_US		(4 << 2)
#define WFM_AMPDU_PARAM_MIN_MPDU_START_SPACING_4_US		(5 << 2)
#define WFM_AMPDU_PARAM_MIN_MPDU_START_SPACING_8_US		(6 << 2)
#define WFM_AMPDU_PARAM_MIN_MPDU_START_SPACING_16_US		(7 << 2)

#define WFM_AMPDU_PARAM_MAX_LEN_EXP		WFM_AMPDU_PARAM_MAX_LEN_EXP_8191
#define WFM_AMPDU_PARAM_MIN_MPDU_START_SPACING	WFM_AMPDU_PARAM_MIN_MPDU_START_SPACING_NO_RESTRICTION
#define WFM_AMPDU_PARAM_OURS			(WFM_AMPDU_PARAM_MAX_LEN_EXP \
						  | WFM_AMPDU_PARAM_MIN_MPDU_START_SPACING)

/*HT- ASEL (Antenna Selection) Capability */
#define WFM_ASEL_ASEL_CAP					(1 << 0)
#define WFM_ASEL_EXPLICIT_CSI_FEEDBACK_BASED_TX_ASEL		(1 << 1)
#define WFM_ASEL_ANTENNA_INDICES_FEEDBACK_BASED_TX_ASEL		(1 << 2)
#define WFM_ASEL_EXPLICIT_CSI_FEEDBACK				(1 << 3)
#define WFM_ASEL_ANTENNA_INDICES_FEEDBACK_CAP			(1 << 4)
#define WFM_ASEL_RX_ASEL_CAP					(1 << 5)
#define WFM_ASEL_TX_SOUNDING_PPDU_CAP				(1 << 6)
#define WFM_ASEL_RESERVED					(1 << 7)

/* TBD :  what do we support in antenna selection ? */
#define WFM_ASEL_OURS						(0)

/* HT - Our Supported MCS Set */
#define WFM_SUP_MCS_RX_MCS_BMAP_0		((WFM_HT_OFDM_RATES_SUPPORTED>>UMAC_HT_LOWEST_RATE_BIT_POS) & 0xFF)	/* support mcs rates 0 to 7 (bits 0-7 are set). Means HT rates 6.5 to 65 mbps are supported */
#define WFM_SUP_MCS_RX_MCS_BMAP_1		0	/* not supported */
#define WFM_SUP_MCS_RX_MCS_BMAP_2		0	/* not supported */
#define WFM_SUP_MCS_RX_MCS_BMAP_3		0	/* not supported */
#define WFM_SUP_MCS_RX_MCS_BMAP_4		0	/* not supported */
#define WFM_SUP_MCS_RX_MCS_BMAP_5		0	/* not supported */
#define WFM_SUP_MCS_RX_MCS_BMAP_6		0	/* not supported */
#define WFM_SUP_MCS_RX_MCS_BMAP_7		0	/* not supported */
#define WFM_SUP_MCS_RX_MCS_BMAP_8		0	/* not supported */
#define WFM_SUP_MCS_RX_MCS_BMAP_9		0	/* bits 72-79 */
#define WFM_SUP_MCS_RX_HIGHEST_SUP_DATA_RATE	65	/* b80-b89 */
#define WFM_SUP_MCS_TX_MCS_SET_DEFINED		1	/* b96 */
#define WFM_SUP_MCS_TXRX_MCS_SET_NOT_EQUAL	0	/* b97 */
#define WFM_SUP_MCS_TX_MAX_NUM_SPATIAL_STRAMS_SUP	0	/* b98-b99 */
#define WFM_SUP_MCS_TX_UNEQUAL_MODULATION_SUP		0	/* b100 */

/*HT- Transmit Beam Forming Capability */
#define WFM_TX_BEAM_FORM_CAP_RX_IMPLICIT_TX		(1 << 0)
#define WFM_TX_BEAM_FORM_CAP_RX_STAGGERED_SOUNDING	(1 << 1)
#define WFM_TX_BEAM_FORM_CAP_TX_STAGGERED_SOUNDING	(1 << 2)
#define WFM_TX_BEAM_FORM_CAP_RX_NDP			(1 << 3)
#define WFM_TX_BEAM_FORM_CAP_TX_NDP			(1 << 4)
#define WFM_TX_BEAM_FORM_CAP_IMPLICIT_TX		(1 << 5)

#define WFM_TX_BEAM_FORM_CAP_CALIBRATION_RESPOND		(1 << 6)
#define WFM_TX_BEAM_FORM_CAP_CALIBRATION_RESPOND_INITIATE	(3 << 6)

#define WFM_TX_BEAM_FORM_CAP_EXPLICIT_CSI_TX			(1 << 8)
#define WFM_TX_BEAM_FORM_CAP_EXPLICIT_NON_CMPRSD_STEERING	(1 << 9)
#define WFM_TX_BEAM_FORM_CAP_EXPLICIT_CMPRSD_STEERING		(1 << 10)

#define WFM_TX_BEAM_FORM_CAP_EXPLICIT_TX_CSI_FB_DELAYED		(1 << 11)
#define WFM_TX_BEAM_FORM_CAP_EXPLICIT_TX_CSI_FB_IMM		(2 << 11)
#define WFM_TX_BEAM_FORM_CAP_EXPLICIT_TX_CSI_FB_DELAYED_IMM	(3 << 11)

#define WFM_TX_BEAM_FORM_CAP_EXPLICIT_NON_CMPRSD_FB_DELAYED	(1 << 13)
#define WFM_TX_BEAM_FORM_CAP_EXPLICIT_NON_CMPRSD_FB_IMM		(2 << 13)
#define WFM_TX_BEAM_FORM_CAP_EXPLICIT_NON_CMPRSD_FB_DELAYED_IMM	(3 << 13)

#define WFM_TX_BEAM_FORM_CAP_EXPLICIT_CMPRSD_FB_DELAYED		(1 << 15)
#define WFM_TX_BEAM_FORM_CAP_EXPLICIT_CMPRSD_FB_IMM		(2 << 15)
#define WFM_TX_BEAM_FORM_CAP_EXPLICIT_CMPRSD_FB_DELAYED_IMM	(3 << 15)

#define WFM_TX_BEAM_FORM_CAP_MINIMAL_GROUPING_1_2		(1 << 17)
#define WFM_TX_BEAM_FORM_CAP_MINIMAL_GROUPING_1_4		(2 << 17)
#define WFM_TX_BEAM_FORM_CAP_MINIMAL_GROUPING_1_2_4		(3 << 17)

#define WFM_TX_BEAM_FORM_CAP_CSI_NUM_ANT_SUP_1			(0 << 19)
#define WFM_TX_BEAM_FORM_CAP_CSI_NUM_ANT_SUP_2			(1 << 19)
#define WFM_TX_BEAM_FORM_CAP_CSI_NUM_ANT_SUP_3			(2 << 19)
#define WFM_TX_BEAM_FORM_CAP_CSI_NUM_ANT_SUP_4			(3 << 19)

#define WFM_TX_BEAM_FORM_CAP_NON_CMPRSD_NUM_ANT_SUP_1		(0 << 21)
#define WFM_TX_BEAM_FORM_CAP_NON_CMPRSD_NUM_ANT_SUP_2		(1 << 21)
#define WFM_TX_BEAM_FORM_CAP_NON_CMPRSD_NUM_ANT_SUP_3		(2 << 21)
#define WFM_TX_BEAM_FORM_CAP_NON_CMPRSD_NUM_ANT_SUP_4		(3 << 21)

#define WFM_TX_BEAM_FORM_CAP_CMPRSD_STEERING_NUM_ANT_SUP_1	(0 << 23)
#define WFM_TX_BEAM_FORM_CAP_CMPRSD_STEERING_NUM_ANT_SUP_2	(1 << 23)
#define WFM_TX_BEAM_FORM_CAP_CMPRSD_STEERING_NUM_ANT_SUP_3	(2 << 23)
#define WFM_TX_BEAM_FORM_CAP_CMPRSD_STEERING_NUM_ANT_SUP_4	(3 << 23)

#define WFM_TX_BEAM_FORM_CAP_CSI_MAX_NUM_ROWS_1			(1 << 25)
#define WFM_TX_BEAM_FORM_CAP_CSI_MAX_NUM_ROWS_2			(1 << 25)
#define WFM_TX_BEAM_FORM_CAP_CSI_MAX_NUM_ROWS_3			(2 << 25)
#define WFM_TX_BEAM_FORM_CAP_CSI_MAX_NUM_ROWS_4			(3 << 25)

#define WFM_TX_BEAM_FORM_CAP_CH_EST_SPACE_TIME_1		(0 << 27)
#define WFM_TX_BEAM_FORM_CAP_CH_EST_SPACE_TIME_2		(1 << 27)
#define WFM_TX_BEAM_FORM_CAP_CH_EST_SPACE_TIME_3		(2 << 27)
#define WFM_TX_BEAM_FORM_CAP_CH_EST_SPACE_TIME_4		(3 << 27)

#define WFM_TX_BEAM_FORM_CAP_RESERVED_29			(1 << 29)
#define WFM_TX_BEAM_FORM_CAP_RESERVED_30			(1 << 30)
#define WFM_TX_BEAM_FORM_CAP_RESERVED_31			(1 << 31)

/* TBD : what do we support in antenna selection ? */
#define WFM_TX_BEAM_FORM_CAP_OURS				(0)

/*HT- Extended Capabilities */
/* bits 3-7, 12-15 are reserved */
#define WFM_HT_EXTEND_CAP_PCO_SUPPORT				(1 << 0)
#define WFM_HT_EXTEND_CAP_PCO_TRANS_TIME_NO_TRANSITION		(0 << 1)
#define WFM_HT_EXTEND_CAP_PCO_TRANS_TIME_400_US			(1 << 1)
#define WFM_HT_EXTEND_CAP_PCO_TRANS_TIME_1_5_MS			(2 << 1)
#define WFM_HT_EXTEND_CAP_PCO_TRANS_TIME_5_MS			(3 << 1)
#define WFM_HT_EXTEND_CAP_MCS_FEEDBACK_NO_FEEDBACK		(0 << 8)
#define WFM_HT_EXTEND_CAP_MCS_FEEDBACK_RESERVED			(1 << 8)
#define WFM_HT_EXTEND_CAP_MCS_FEEDBACK_UNSOLICITED_FEEDBACK	(2 << 8)
#define WFM_HT_EXTEND_CAP_MCS_FEEDBACK_BOTH_DELAYED_IMM_UNSOLICITED	(3 << 8)
#define WFM_HT_EXTEND_CAP_HTC_SUPPORT				(1 << 10)
#define WFM_HT_EXTEND_CAP_RD_RESPONDER				(1 << 11)
#define WFM_HT_EXTEND_CAP_NOT_SUPPORTED				(0)

#define WFM_HT_EXTEND_CAP_OURS		(WFM_HT_EXTEND_CAP_MCS_FEEDBACK_BOTH_DELAYED_IMM_UNSOLICITED \
					  | WFM_HT_EXTEND_CAP_HTC_SUPPORT)

/*Channels supported on different Regulatory classes*/

#define WFM_2_4GHZ_CHANNELS_SUPPORTED	{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 }

#define WFM_5GHZ_20MHZ_CEPT_CHANNELS_SUPPORTED				\
	{									\
		36,   40,   44,   48,   52,   56,   60,   64,   100,  104,  108,  112,  116,  120,  124,  128,	\
		132,  136,  140							\
	}

#define WFM_5GHZ_20MHZ_FCC_CHANNELS_SUPPORTED				\
	{									\
		36,   40,   44,   48,   52,   56,   60,   64,   149,    153,    157,    161,   100,  104,  108, \
		112,  116,  120,  124,  128,  132,  136,  140			\
	}

#define WFM_5GHZ_20MHZ_CHANNELS_SUPPORTED	{ 8, 12,  16,  34, 38, 42, 46 }

#define WFM_5GHZ_10MHZ_CHANNELS_SUPPORTED	{ 7, 8,  9,  11 }

#define WFM_4_9GHZ_20MHZ_CHANNELS_SUPPORTED	{ 184, 188,  192,  196 }

#define WFM_4_9GHZ_10MHZ_CHANNELS_SUPPORTED	{ 183, 184,  185,  187, 188, 189 }

#define WFM_5GHZ_4_9GHZ_ALL_CHANNELS_SUPPORTED				\
	{									\
		36,   40,   44,   48,   52,   56,   60,   64,   149,    153,    157,    161,   100,  104,  108, \
		112,  116,  120,  124,  128,  132,  136,  140,  8,      12,     16,     34,    38,   42,   46, \
		184,  188,  192,  196					\
	}

#define WFM_BEACON_LISTEN_INTERVAL				1
#define WFM_DEFAULT_BEACON_INTERVAL				100

#define WFM_NUM_PROBE_REQ_FOR_ACTIVE_SCAN			4

#define WFM_DEFAULT_RTS_THRESHOLD				3000

#define WFM_DEFAULT_SHORT_RETRY_COUNT				7

/* time between two probe req in active scan */
#define WFM_DEFAULT_TIME_DIFF_BET_PROBE_REQ_IN_SCAN	TIME_USEC_TO_TGT_UTS(100)

/*max size of pending join message is 1024 bytes */
#define WFM_MAX_SZ_JOIN_MSG				1024

/* Size of WFM_BSS_LIST_SCAN + max size of SSID(32) - 2 + 2 byte padding */
#define WFM_MAX_INTERNAL_SCAN_MSG_SIZE			44

#define WFM_ASSOC_IE_MAX_SIZE				512
#define WFM_SLOT_TIME_SHORT_SLOT			9
#define WFM_SLOT_TIME_LONG_SLOT				20
#define WFM_SLOT_TIME_11J_10MHZ_MODE			13

/* Device Power level is multiple of 0.1 dbm */
#define WFM_POWER_LEVEL_UNIT				10
/*Received txop is multiple of 32Us */
#define TXOP_UNIT					32

/*
  If dot11CurrentTxPowerLevel mib is set then use followning power level value
  in all transmit request
*/
#define WFM_TX_POWER_LEVEL				0x00008000

#define WFM_MANG_POLICY_INDEX				0
#define WFM_DATA_POLICY_INDEX				1

/*WPA-TKIP and WPA2-AES Cipher oui*/
#define WFM_WPA_TKIP_CIPHER_OUI				{0x00, 0x50, 0xF2, 0x02}
#define WFM_WPA_AES_CIPHER_OUI				{0x00, 0x50, 0xF2, 0x04}
#define WFM_WPA2_TKIP_CIPHER_OUI			{0x00, 0x0F, 0xAC, 0x02}
#define WFM_WPA2_AES_CIPHER_OUI				{0x00, 0x0F, 0xAC, 0x04}
#define CIPHER_OUI_SIZE					4
#define WFM_WPA_8021X_AKM_OUI				{0x00, 0x50, 0xF2, 0x01}
#define WFM_WPA_PSK_AKM_OUI				{0x00, 0x50, 0xF2, 0x02}
#define WFM_WPA2_8021X_AKM_OUI				{0x00, 0x0F, 0xAC, 0x01}
#define WFM_WPA2_PSK_AKM_OUI				{0x00, 0x0F, 0xAC, 0x02}
#if FT_SUPPORT
#define WFM_WPA2_FT_AKM_OUI				{0x00, 0x0F, 0xAC, 0x03}
#define WFM_WPA2_FT_PSK_AKM_OUI				{0x00, 0x0F, 0xAC, 0x04}
#endif	/* FT_SUPPORT */
#if MGMT_FRAME_PROTECTION
#define WFM_WPA2_BIP_CIPHER_OUI				{0x00, 0x0F, 0xAC, 0x06}
#define WFM_WPA2_8021X_SH256_AKM_OUI			{0x00, 0x0F, 0xAC, 0x05}
#define WFM_WPA2_PSK_SH256_AKM_OUI			{0x00, 0x0F, 0xAC, 0x06}
#endif	/* MGMT_FRAME_PROTECTION */
#define AKM_OUI_SIZE					4

/*protected management frame policy flags */
#define UMAC_MFP_DISABLE				0
#define UMAC_MFP_REQUIRED				1
#define UMAC_MFP_CAPABLE				2
#define UMAC_MFP_CAPABLE_REQUIRED			3

#define UMAC_PROT_MGMT_ENABLE				(1<<0)
#define UMAC_PROT_MGMT_UNPROT_ALLOWED			(1<<1)

#endif	/* _UMAC_DEV_CONF_H */
