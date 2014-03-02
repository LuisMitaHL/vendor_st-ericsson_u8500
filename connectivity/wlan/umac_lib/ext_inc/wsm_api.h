/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
* \file
* \ingroup WSM EXPORTS
*
* \brief WSM Host Interface Message Defintions
*/

#ifndef WSM_API_H
#define WSM_API_H

/* The preceeding header files should define the basic types

	uint8,uint16,uint32,sint8,sint16,sint32

   The basic type defintion header file is stddefs.h on the device side,
   but could be named something else on the host side.
*/

/* The HI message format and basic HI messages are defined in hi_api.h, they are

    exception indication
    startup indication
    trace indication
    generic indication

    read-memory request/confirmation
    write-memory request/confirmation
    configuration request/confirmation
    generic request/confirmation
*/
#include "stddefs.h"
#include "hi_api.h"
/* Defintions of basic D0 and D11 types and constants used by modules of MAC firmware which do not include wsm_api.h */
#include "d11_basic_defs.h"

/* = == ================================================================= */
/* Constants							    */
/* = == ================================================================= */

/* The following are WSM-firmware specific messages
*/

/* This version is returned in HI_STARTUP_IND */
#define WSM_HI_API_VER		  (HI_API_VER|0x0012)	/* HI_API_VER takes b15-b8 */

/* Message ID defintions
 * HI_REQ_BASE	      0x0000
 * HI_CNF_BASE	      0x0400
 * HI_IND_BASE	      0x0800
 */

/*
  Bit usage of Message ID Field
  b0  - b5  -> Message Id
  b6  - b9  -> Link Id
  b10 - b11 -> Message Type  [REQ,CNF,IND]
  b12 - b15 -> Reserved for Host Interface
 */

#define WSM_LNK_ID_MASK		 (0x0F << 6)
#define WSM_MSG_ID_MASK		 (~WSM_LNK_ID_MASK)

/* The minimum application request ID is 0x0004 */
#define WSM_HI_TX_REQ_ID		0x0004
#define WSM_HI_TX_CNF_ID		(HI_CNF_BASE + WSM_HI_TX_REQ_ID)

#define WSM_HI_READ_MIB_REQ_ID	  0x0005
#define WSM_HI_READ_MIB_CNF_ID	  (HI_CNF_BASE + WSM_HI_READ_MIB_REQ_ID)

#define WSM_HI_WRITE_MIB_REQ_ID	 0x0006
#define WSM_HI_WRITE_MIB_CNF_ID	 (HI_CNF_BASE + WSM_HI_WRITE_MIB_REQ_ID)

#define WSM_HI_START_SCAN_REQ_ID	0x0007
#define WSM_HI_START_SCAN_CNF_ID	(HI_CNF_BASE + WSM_HI_START_SCAN_REQ_ID)

#define WSM_HI_STOP_SCAN_REQ_ID	 0x0008
#define WSM_HI_STOP_SCAN_CNF_ID	 (HI_CNF_BASE + WSM_HI_STOP_SCAN_REQ_ID)

#define WSM_HI_CONFIGURATION_REQ_ID     0x0009
#define WSM_HI_CONFIGURATION_CNF_ID     (HI_CNF_BASE + WSM_HI_CONFIGURATION_REQ_ID)

#define WSM_HI_RESET_REQ_ID	     0x000A
#define WSM_HI_RESET_CNF_ID	     (HI_CNF_BASE + WSM_HI_RESET_REQ_ID)

#define WSM_HI_JOIN_REQ_ID	      0x000B
#define WSM_HI_JOIN_CNF_ID	      (HI_CNF_BASE + WSM_HI_JOIN_REQ_ID)

#define WSM_HI_ADD_KEY_REQ_ID	   0x000C
#define WSM_HI_ADD_KEY_CNF_ID	   (HI_CNF_BASE + WSM_HI_ADD_KEY_REQ_ID)

#define WSM_HI_REMOVE_KEY_REQ_ID	0x000D
#define WSM_HI_REMOVE_KEY_CNF_ID	(HI_CNF_BASE + WSM_HI_REMOVE_KEY_REQ_ID)

#define WSM_HI_START_MEASUREMENT_REQ_ID 0x000E
#define WSM_HI_START_MEASUREMENT_CNF_ID (HI_CNF_BASE + WSM_HI_START_MEASUREMENT_REQ_ID)

#define WSM_HI_STOP_MEASUREMENT_REQ_ID  0x000F
#define WSM_HI_STOP_MEASUREMENT_CNF_ID  (HI_CNF_BASE + WSM_HI_STOP_MEASUREMENT_REQ_ID)

#define WSM_HI_SET_PM_MODE_REQ_ID       0x0010
#define WSM_HI_SET_PM_MODE_CNF_ID       (HI_CNF_BASE + WSM_HI_SET_PM_MODE_REQ_ID)

#define WSM_HI_SET_BSS_PARAMS_REQ_ID    0x0011
#define WSM_HI_SET_BSS_PARAMS_CNF_ID    (HI_CNF_BASE + WSM_HI_SET_BSS_PARAMS_REQ_ID)

#define WSM_HI_TX_QUEUE_PARAMS_REQ_ID   0x0012
#define WSM_HI_TX_QUEUE_PARAMS_CNF_ID   (HI_CNF_BASE + WSM_HI_TX_QUEUE_PARAMS_REQ_ID)

#define WSM_HI_EDCA_PARAMS_REQ_ID       0x0013
#define WSM_HI_EDCA_PARAMS_CNF_ID       (HI_CNF_BASE + WSM_HI_EDCA_PARAMS_REQ_ID)

#define WSM_HI_CONFIG_BLOCK_ACK_REQ_ID  0x0014
#define WSM_HI_CONFIG_BLOCK_ACK_CNF_ID  (HI_CNF_BASE + WSM_HI_CONFIG_BLOCK_ACK_REQ_ID)

#define WSM_HI_SET_SYSTEM_INFO_REQ_ID   0x0015
#define WSM_HI_SET_SYSTEM_INFO_CNF_ID   (HI_CNF_BASE + WSM_HI_SET_SYSTEM_INFO_REQ_ID)

#define WSM_HI_SWITCH_CHANNEL_REQ_ID    0x0016
#define WSM_HI_SWITCH_CHANNEL_CNF_ID    (HI_CNF_BASE + WSM_HI_SWITCH_CHANNEL_REQ_ID)

#define WSM_HI_START_REQ_ID	     0x0017
#define WSM_HI_START_CNF_ID	     (HI_CNF_BASE + WSM_HI_START_REQ_ID)

#define WSM_HI_BEACON_TRANSMIT_REQ_ID   0x0018
#define WSM_HI_BEACON_TRANSMIT_CNF_ID   (HI_CNF_BASE + WSM_HI_BEACON_TRANSMIT_REQ_ID)

#define WSM_HI_START_FIND_REQ_ID	0x0019
#define WSM_HI_START_FIND_CNF_ID	(HI_CNF_BASE + WSM_HI_START_FIND_REQ_ID)

#define WSM_HI_STOP_FIND_REQ_ID	 0x001A
#define WSM_HI_STOP_FIND_CNF_ID	 (HI_CNF_BASE + WSM_HI_STOP_FIND_REQ_ID)

#define WSM_HI_UPDATE_IE_REQ_ID	 0x001B
#define WSM_HI_UPDATE_IE_CNF_ID	 (HI_CNF_BASE + WSM_HI_UPDATE_IE_REQ_ID)

#define WSM_HI_MAP_LINK_REQ_ID	  0x001C
#define WSM_HI_MAP_LINK_CNF_ID	  (HI_CNF_BASE + WSM_HI_MAP_LINK_REQ_ID)

#define WSM_HI_PTA_STATISTICS_REQ_ID    0x001D
#define WSM_HI_PTA_STATISTICS_CNF_ID    (HI_CNF_BASE + WSM_HI_PTA_STATISTICS_REQ_ID)

#define WSM_HI_MULTI_TX_CNF_ID	  (HI_CNF_BASE + 0x001E)

/* The minimum application request ID is (HI_IND_BASE + 0x0004) */
#define WSM_HI_RX_IND_ID		    (HI_IND_BASE + 0x0004)
#define WSM_HI_EVENT_IND_ID		 (HI_IND_BASE + 0x0005)
#define WSM_HI_SCAN_CMPL_IND_ID	     (HI_IND_BASE + 0x0006)
#define WSM_HI_MEASURE_CMPL_IND_ID	  (HI_IND_BASE + 0x0007)
#define WSM_HI_BA_TIMEOUT_IND_ID	    (HI_IND_BASE + 0x0008)
#define WSM_HI_SET_PM_MODE_CMPL_IND_ID      (HI_IND_BASE + 0x0009)
#define WSM_HI_SWITCH_CHANNEL_IND_ID	(HI_IND_BASE + 0x000A)
#define WSM_HI_FIND_CMPL_IND_ID	     (HI_IND_BASE + 0x000B)
#define WSM_HI_SUSP_RESUME_TX_IND_ID	(HI_IND_BASE + 0x000C)
#define WSM_HI_MEAS_RX_BEACON_FRAME_IND_ID  (HI_IND_BASE + 0x000D)
#define WSM_HI_DEBUG_IND_ID  (HI_IND_BASE + 0x000E)

/* MIB IDs */
#define WSM_MIB_ID_DOT11_STATION_ID		 0x0000	/* 4.1  dot11StationId */
#define WSM_MIB_ID_DOT11_MAX_TRANSMIT_LIFTIME       0x0001	/* 4.2  dot11MaxtransmitMsduLifeTime */
#define WSM_MIB_ID_DOT11_MAX_RECEIVE_LIFETIME       0x0002	/* 4.3  dot11MaxReceiveLifeTime */
#define WSM_MIB_ID_DOT11_SLOT_TIME		  0x0003	/* 4.4  dot11SlotTime */
#define WSM_MIB_ID_DOT11_GROUP_ADDRESSES_TABLE      0x0004	/* 4.5  dot11GroupAddressesTable */
#define WSM_MIB_ID_DOT11_WEP_DEFAULT_KEY_ID	 0x0005	/* 4.6  dot11WepDefaultKeyId */
#define WSM_MIB_ID_DOT11_CURRENT_TX_POWER_LEVEL     0x0006	/* 4.7  dot11CurrentTxPowerLevel */
#define WSM_MIB_ID_DOT11_RTS_THRESHOLD	      0x0007	/* 4.8  dot11RTSThreshold */

#define WSM_MIB_ID_NON_ERP_PROTECTION	       0x1000	/* 4.9  NonErpProtection */
#define WSM_MIB_ID_ARP_IP_ADDRESSES_TABLE	   0x1001	/* 4.10 ArpIpAddressesTable */
#define WSM_MIB_ID_TEMPLATE_FRAME		   0x1002	/* 4.11 TemplateFrame */
#define WSM_MIB_ID_RX_FILTER			0x1003	/* 4.12 RxFilter */
#define WSM_MIB_ID_BEACON_FILTER_TABLE	      0x1004	/* 4.13 BeaconFilterTable */
#define WSM_MIB_ID_BEACON_FILTER_ENABLE	     0x1005	/* 4.14 BeaconFilterEnable */
#define WSM_MIB_ID_OPERATIONAL_POWER_MODE	   0x1006	/* 4.15 OperationalPowerMode */
#define WSM_MIB_ID_BEACON_WAKEUP_PERIOD	     0x1007	/* 4.16 BeaconWakeUpPeriod */
#define WSM_MIB_ID_RCPI_RSSI_THRESHOLD	      0x1009	/* 4.17 RcpiRssiThreshold */
#define WSM_MIB_ID_STATISTICS_TABLE		 0x100A	/* 4.18 StatisticsTable */
#define WSM_MIB_ID_IBSS_PS_CONFIG		   0x100B	/* 4.19 IbssPsConfig */
#define WSM_MIB_ID_COUNTERS_TABLE		   0x100C	/* 4.20 CountersTable */
#define WSM_MIB_ID_BLOCK_ACK_POLICY		 0x100E	/* 4.21 BlockAckPolicy */
#define WSM_MIB_ID_OVERRIDE_INTERNAL_TX_RATE	0x100F	/* 4.22 OverrideInternalTxRate */
#define WSM_MIB_ID_SET_ASSOCIATION_MODE	     0x1010	/* 4.23 SetAssociationMode */
#define WSM_MIB_ID_UPDATE_EPTA_CONFIG_DATA	  0x1011	/* 4.24 UpdateEptaConfigData */
#define WSM_MIB_ID_SELECT_CCA_METHOD		0x1012	/* 4.25 SelectCcaMethod */
#define WSM_MIB_ID_SET_UAPSD_INFORMATION	    0x1013	/* 4.26 SetUpasdInformation */
#define WSM_MIB_ID_SET_AUTO_CALIBRATION_MODE	0x1015	/* 4.27 SetAutoCalibrationMode  WBF00004073 */
#define WSM_MIB_ID_SET_TX_RATE_RETRY_POLICY	 0x1016	/* 4.28 SetTxRateRetryPolicy */
#define WSM_MIB_ID_SET_HOST_MSG_TYPE_FILTER	 0x1017	/* 4.29 SetHostMessageTypeFilter */

#define WSM_MIB_ID_P2P_FIND_INFO		    0x1018	/* 4.30 P2PFindInfo */
#define WSM_MIB_ID_P2P_PS_MODE_INFO		 0x1019	/* 4.31 P2PPsModeInfo */

#define WSM_MIB_ID_SET_ETHERTYPE_DATAFRAME_FILTER   0x101A	/* 4.32 SetEtherTypeDataFrameFilter */
#define WSM_MIB_ID_SET_UDPPORT_DATAFRAME_FILTER     0x101B	/* 4.33 SetUDPPortDataFrameFilter */
#define WSM_MIB_ID_SET_MAGIC_DATAFRAME_FILTER       0x101C	/* 4.34 SetMagicDataFrameFilter */

#define WSM_MIB_ID_P2P_DEVICE_INFO		  0x101D	/* 4.35 P2PDeviceInfo */

#define WSM_MIB_ID_SET_WCDMA_BAND		   0x101E	/* 4.36 SetWCDMABand */

#define WSM_MIB_ID_GRP_SEQ_COUNTER		  0x101F	/* 4.37 GroupTxSequenceCounter */

#define WSM_MIB_ID_PROTECTED_MGMT_POLICY	    0x1020  /* 4.38 Protected Management Frame policy */	/* 1021 4.39 SetHtProtection*/
#define WSM_MIB_ID_SET_HT_PROTECTION		0x1021	/* 4.39 SetHtProtection */

/* 1021 4.39 SetHtProtection*/

#define WSM_MIB_ID_GPIO_COMMAND		     0x1022	/* 4.40 GPIO Command */

#define WSM_MIB_ID_TSF_COUNTER		      0x1023	/* 4.41 TSF Counter Value */

#define WSM_MIB_ID_USE_MULTI_TX_CONF_MSG	    0x1024	/* 4.42 UseMultiTxConfMessage */

#define WSM_MIB_ID_BLOCK_ACK_INFO		   0x100D	/* Test Purposes Only */

#define WSM_MIB_ID_KEEP_ALIVE_PERIOD		0x1025	/* 4.43 Keep-alive period */

#define WSM_MIB_ID_DISABLE_BSSID_FILTER		0x1026	/* 4.44 Ignore bss_id filtering in firmware */
#define WSM_MIB_ID_QUIET_INTERVAL_MARGIN	0x1027	/* margin in us (before the quiet interval) */

#define WSM_MIB_ID_ARP_KEEP_ALIVE_PERIOD		0x1028	/* 4.46 Keep-alive period */
/* Desired scan mode (relevant for beacon measurements only)*/
#define WSM_SCAN_MODE_PASSIVE		   0	/*passive */
#define WSM_SCAN_MODE_ACTIVE		    1	/* active */
#define WSM_SCAN_MODE_PSSV_PILOT		2	/*passive pilot */
#define WSM_SCAN_MODE_STA_SELECTED	      3	/* STA selected */

/* Supported bands */
#define WSM_BAND24			      0
#define WSM_BAND5			       1

#define WSM_BANDS			       2

/* Types of Scans supported */
#define WSM_SCAN_TYPE_FG			0
#define WSM_SCAN_TYPE_BG			1
#define WSM_SCAN_TYPE_AUTO		      2

/* Scan Flags Definitions */
#define WSM_SCAN_F_FORCED_BACKGROUND	    (1 << 0)
#define WSM_SCAN_F_SPLIT_SCAN		   (1 << 1)
#define WSM_SCAN_F_PREAMBLE_TYPE		(1 << 2)	/* 0:Long Preamble 1:Short Preamble*/
#define WSM_SCAN_F_11N_MODE		     (1 << 3)	/* 0:Mixed Mode 1:Greenfield Mode*/

/* Operation mode of the station */
#define WSM_MODE_IBSS			   0
#define WSM_MODE_BSS			    1

/* Preamble Type */
#define WSM_PREAMBLE_LONG		       0
#define WSM_PREAMBLE_SHORT		      1	/* Long for 1Mbps */
#define WSM_PREAMBLE_SHORT_LONG12	       2	/* Long for 1Mbps & 2Mbps */

/* Power Management Mode */
#define WSM_PM_MODE_ACTIVE		      0
#define WSM_PM_MODE_PS			  1
#define WSM_PM_MODE_UNDETERMINED		2
#define WSM_PM_F_FAST_PSM_ENABLE			0x80

/* MIC key type */
#define WSM_MIC_KEY_PAIRWISE		    0
#define WSM_MIC_KEY_GROUP		       1

/* Encryption Key Type */
#define WSM_KEY_TYPE_WEP_DEFAULT		0
#define WSM_KEY_TYPE_WEP_PAIRWISE	       1
#define WSM_KEY_TYPE_TKIP_GROUP		 2
#define WSM_KEY_TYPE_TKIP_PAIRWISE	      3
#define WSM_KEY_TYPE_AES_GROUP		  4
#define WSM_KEY_TYPE_AES_PAIRWISE	       5
#define WSM_KEY_TYPE_WAPI_GROUP		 6
#define WSM_KEY_TYPE_WAPI_PAIRWISE	      7
#define WSM_KEY_TYPE_IGTK_GROUP		 8

/* Transmission Queue Types */
#define WSM_QUEUE_ID_BESTEFFORT		 0
#define WSM_QUEUE_ID_BACKGROUND		 1
#define WSM_QUEUE_ID_VIDEO		      2
#define WSM_QUEUE_ID_VOICE		      3

#define WSM_QUEUE_ID_2ND_CHAN_FLAG    0x80

/* Transmission HT Parameters */
#define WSM_TX_HT_FF_NON_HT		     0
#define WSM_TX_HT_FF_MIXED		      1
#define WSM_TX_HT_FF_GREENFIELD		 2
#define WSM_TX_HT_FF_MASK		       3
#define WSM_TX_HT_STBC_BIT		      7
#define WSM_TX_HT_STBC			  (1 <<  WSM_TX_HT_STBC_BIT)
#define WSM_TX_HT_AGGREGATION		   (1 << 16)
#define WSM_TX_HT_AGGR_FIRST		    (1 << 17)
#define WSM_TX_HT_AGGR_LAST		     (1 << 18)

/* Transmission Flags */
#define WSM_TX_F_FIRST_TX_EXPIRE		0
#define WSM_TX_F_TX_REQUEST_EXPIRE	      1
#define WSM_TX_F_TX_REQUEST_EXPIRE_MASK	 (1 << 0)
#define WSM_TX_F_PTA_PRIORITY_MASK	      (7 << 1)
#define WSM_TX_F_PTA_PRIORITY_OFFSET	    (1)

/* Transmission Confirmation Flags */
#define WSM_TXC_F_AGGREGATED		   (1 << 0)
#define WSM_TXC_F_AP_NORMAL		    (0 << 2)
#define WSM_TXC_F_AP_NOACK		     (1 << 2)
#define WSM_TXC_F_AP_NOEXPLICIT		(2 << 2)
#define WSM_TXC_F_AP_BLOCKACK		  (3 << 2)
#define WSM_TXC_F_AP_MASK		      (3 << 2)
#define WSM_TXC_F_TXOP_EXTENDED		(1 << 4)
#define WSM_TXC_F_MORE_TXC_FOR_MULTI	   (1 << 5)

/* ACK frame policy */
#define WSM_ACKPLCY_NORMAL		      0
#define WSM_ACKPLCY_TXNOACK		     1
#define WSM_ACKPLCY_BLCKACK		     2	/* Block ACK */

/* Privacy Key ID Values */
#define WSM_PRIVKEY_ID0			 0
#define WSM_PRIVKEY_ID1			 1
#define WSM_PRIVKEY_ID2			 2
#define WSM_PRIVKEY_ID3			 3

/* Frame Template Type */
#define WSM_TMPLT_PRBREQ			0	/* Probe request frame */
#define WSM_TMPLT_BCN			   1	/* Beacon frame */
#define WSM_TMPLT_NULL			  2	/* NULL data frame */
#define WSM_TMPLT_QOSNUL			3	/* QOS NULL data frame */
#define WSM_TMPLT_PSPOLL			4	/* PS-Poll frame */
#define WSM_TMPLT_PRBRES			5	/* Probe response frame */
#define WSM_TMPLT_ARP		     	6	/* ARP response frame */

/* 4.15 Operational Power Mode of the WLAN device */
#define WSM_OP_POWER_MODE_ACTIVE		0	/* active powered state */
#define WSM_OP_POWER_MODE_DOZE		  1	/* low power mode */
#define WSM_OP_POWER_MODE_QUIESCENT	     2	/* lowest power state */
#define WSM_OP_POWER_MODE_MASK		  0xF	/* Power mode mask */

/* Block Ack Agreement Modes */
#define WSM_BA_MODE_REMOVE		      0
#define WSM_BA_MODE_HTI			 1
#define WSM_BA_MODE_COMPRESSED		  2

/* Status of an Operation */
#define WSM_STATUS_SUCCESS		      0
#define WSM_STATUS_FAILURE		      1
#define WSM_INVALID_PARAMETER		   2
#define WSM_ACCESS_DENIED		       3
#define WSM_STATUS_DECRYPTFAILURE	       4
#define WSM_STATUS_MICFAILURE		   5
#define WSM_STATUS_RETRY_EXCEEDED	       6
#define WSM_STATUS_TX_LIFETIME_EXCEEDED	 7
#define WSM_STATUS_LINK_LOST		    8
#define WSM_STATUS_NO_KEY_FOUND		 9
#define WSM_STATUS_JAMMER_DETECTED	     10
#define WSM_REQUEUE			    11
#define WSM_STATUS_UNENCRYPTEDFAILURE	  12

/* Receive Indication (WSM_HI_RX_IND_S) Flags definitions */
#define WSM_RI_FLAGS_UNENCRYPTED		0
#define WSM_RI_FLAGS_WEP_ENCRYPTED	      1
#define WSM_RI_FLAGS_TKIP_ENCRYPTED	     2
#define WSM_RI_FLAGS_AES_ENCRYPTED	      3
#define WSM_RI_FLAGS_WAPI_ENCRYPTED	     4
#define WSM_RI_FLAGS_IGTK_ENCRYPTED	     5
#define WSM_RI_FLAGS_AGGR		       (1 << 3)
#define WSM_RI_FLAGS_AGGR_START		 (1 << 4)
#define WSM_RI_FLAGS_AGGR_END		   (1 << 5)
#define WSM_RI_FLAGS_DEFRAG		     (1 << 6)
#define WSM_RI_FLAGS_BEACON		     (1 << 7)
#define WSM_RI_FLAGS_STA_TIM_BIT_PRESENT	(1 << 8)
#define WSM_RI_FLAGS_MULTI_BIT_TIM	      (1 << 9)
#define WSM_RI_FLAGS_BCN_PR_SSID_MATCH	  (1 << 10)
#define WSM_RI_FLAGS_BSSID_MATCH		(1 << 11)
#define WSM_RI_FLAGS_MORE_PENDING	       (1 << 12)
#define WSM_RI_FLAGS_IN_MEASUREMENT	     (1 << 13)
#define WSM_RI_FLAGS_HT_FRAME		   (1 << 14)
#define WSM_RI_FLAGS_WITH_STBC		  (1 << 15)
#define WSM_RI_FLAGS_A1_STA_MATCH	       (1 << 16)
#define WSM_RI_FLAGS_A1_GROUP		   (1 << 17)
#define WSM_RI_FLAGS_A1_BROADCAST	       (1 << 18)
#define WSM_RI_FLAGS_ENC_GROUP_KEY	      (1 << 19)
#define WSM_RI_FLAGS_ENC_KEY_INDEX	      (1 << 20)	/* 4 - bits */
#define WSM_RI_FLAGS_BCN_PBRSP_TSF_PRESENT	  (1 << 24)

/* Types for an Event Indication */
#define WSM_EVENT_IND_ERROR		     0
#define WSM_EVENT_IND_BSSLOST		   1
#define WSM_EVENT_IND_BSSREGAINED	       2
#define WSM_EVENT_IND_RADAR		     3
#define WSM_EVENT_IND_RCPI_RSSI		 4
#define WSM_EVENT_IND_BT_INACTIVE	       5
#define WSM_EVENT_IND_BT_ACTIVE		 6
#define WSM_EVENT_IND_PS_MODE_ERROR         7

/* Used in PsModeError Event indication */
#define WSM_PS_ERROR_NO_ERROR	                    0
#define WSM_PS_ERROR_AP_NOT_RESP_TO_POLL	        1
#define WSM_PS_ERROR_AP_NOT_RESP_TO_UAPSD_TRIGGER   2
#define WSM_PS_ERROR_AP_SENT_UNICAST_IN_DOZE        3


/* Measurement Types */
#define WSM_MEASURE_TYPE_CHANNEL_LOAD	   0
#define WSM_MEASURE_TYPE_NOISE_HISTOGRAM	1
#define WSM_MEASURE_TYPE_BEACON		 2
#define WSM_MEASURE_TYPE_STAT_STATISTICS	3
#define WSM_MEASURE_TYPE_LINK_MEASUREMENT       4

#define WSM_MAX_NUM_SSIDS_IN_SCAN	       2

/* Start Modes */
#define WSM_START_MODE_MINIAP		   0
#define WSM_START_MODE_P2P_GO		   1
#define WSM_START_MODE_P2P_DEVICE	       2

/* Enable /Disable Beaconing*/
#define WSM_START_BEACONING		     1
#define WSM_STOP_BEACONING		      0

/*Update-IE Flags*/
#define WSM_UPDATE_IE_BCN		       0x0001
#define WSM_UPDATE_IE_PROB_RSP		  0x0002
#define WSM_UPDATE_IE_PROB_REQ		  0x0004

/*protected management frame policy flags */
#define WSM_PROT_MGMT_ENABLE		    (1 << 0)
#define WSM_PROT_MGMT_UNPROT_ALLOWED	    (1 << 1)
#define WSM_PROT_MGMT_NO_ENCR_FOR_AUTH_FRAMES (1 << 2)

/* Join Flags */
#define WSM_JOIN_F_UNSYNCHRONIZED_LINK	  0x01
#define WSM_JOIN_F_P2PGO_BSS		    0x02
#define WSM_JOIN_F_FORCE_JOIN		   0x04
#define WSM_JOIN_F_EPTA_PRIORITY_FOR_PROBE      0x08

/* Set BSS-Parameters Flags */
#define WSM_SET_BSS_F_BEACON_LOST_COUNT_ONLY    0x01

/* Types for an Debug indication Events (WSM_HI_DEBUG_IND_ID) */
#define WSM_DEBUG_IND_EPTA_RT_STATS     0
#define WSM_DEBUG_IND_EPTA_NRT_STATS    1
#define WSM_DEBUG_IND_EPTA_DBG_INFO     2
#define WSM_DEBUG_IND_PS_DBG_INFO       3
#define WSM_DEBUG_IND_PAS_DBG_INFO      4


/* = == ================================================================= */
/* Structures							   */
/* = == ================================================================= */

/* Template used in "Command Response Event" related messages */

typedef struct WSM_HI_GENERIC_CNF_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Status;
} WSM_HI_GENERIC_CNF;

/**********************************************************************
* 2.4.1 Startup Configuration Information				      *
***********************************************************************/
typedef struct WSM_HI_STARTUP_CONFIG_INFO_S {
	uint16	 NumLinksAp		;
	uint16	 NumLinksSTA	;
} WSM_HI_STARTUP_CONFIG_INFO;



/***********************************************************************
 * 3.1 Configuration Request					   *
 ***********************************************************************/
typedef struct WSM_HI_CONFIGURATION_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 dot11MaxTransmitMsduLifeTime;
	uint32 dot11MaxReceiveLifeTime;
	uint32 dot11RtsThreshold;
	HI_DPD_CONFIG DpdData;
} WSM_HI_CONFIGURATION_REQ;

/***********************************************************************
 * 3.2 Configuration Confirmation				      *
 ***********************************************************************/
typedef struct WSM_TX_POWER_RANGE_S {
	sint32 MinPowerLevel;
	sint32 MaxPowerLevel;
	uint32 Stepping;
} WSM_TX_POWER_RANGE;

typedef struct WSM_HI_CONFIGURATION_CNF_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Status;
	uint8 dot11StationId[6];
	uint8 dot11FrequencyBandsSupported;
	uint8 Reserved;
	uint32 SupportedRateMask;
	WSM_TX_POWER_RANGE TxPowerRange[2];
} WSM_HI_CONFIGURATION_CNF;

/***********************************************************************
 * 3.3 Reset Request						   *
 ***********************************************************************/
typedef struct WSM_HI_RESET_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Flags;
} WSM_HI_RESET_REQ;

/***********************************************************************
 * 3.4 Reset Confirmation					      *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_RESET_CNF;

/***********************************************************************
 * 3.9 Start-Scan Request					      *
 ***********************************************************************/
typedef struct WSM_SCAN_CHANNEL_DEF_S {
	uint16 ChannelNumber;
	uint16 Reserved;
	uint32 MinChannelTime;
	uint32 MaxChannelTime;
	sint32 TxPowerLevel;
} WSM_SCAN_CHANNEL_DEF;

typedef struct WSM_SSID_DEF_S {
	uint32 SSIDLength;
	uint8 SSID[D11_MAX_SSID_LEN];
} WSM_SSID_DEF;

typedef struct WSM_HI_START_SCAN_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 Band;
	uint8 ScanType;
	uint8 ScanFlags;
	uint8 MaxTransmitRate;
	uint32 AutoScanInterval;
	uint8 NumOfProbeRequests;
	uint8 NumOfChannels;
	uint8 NumOfSSIDs;
	uint8 ProbeDelay;

	/* followed by
	   WSM_SCAN_CHANNEL_DEF    Channels[NumOfChannels];
	   WSM_SSID_DEF	    SSIDs[NumOfSSIDs];
	 */
} WSM_HI_START_SCAN_REQ;

/***********************************************************************
 * 3.10 Start-Scan Confirmation					*
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_START_SCAN_CNF;

/***********************************************************************
 * 3.11 Stop-Scan request					      *
 ***********************************************************************/
typedef HI_MSG_HDR WSM_HI_STOP_SCAN_REQ;

/***********************************************************************
 * 3.12 Stop-Scan Confirmation					 *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_STOP_SCAN_CNF;

/***********************************************************************
 * 3.13 Scan Complete Indication				       *
 ***********************************************************************/
typedef struct WSM_HI_SCAN_CMPL_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Status;
	uint8 PmMode;
	uint8 NumChannelsCompleted;
	uint16 Reserved;
} WSM_HI_SCAN_CMPL_IND;

/***********************************************************************
 * 3.14 Transmit Request					       *
 ***********************************************************************/
#define WSM_TX_USE_DEFAULT_POWER_LEVEL 0x00008000

typedef struct WSM_HI_TX_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 PacketId;
	uint8 MaxTxRate;
	uint8 QueueId;		/* WSM_QUEUE_ID_XXXX */
	uint8 More;
	uint8 Flags;
	uint32 Reserved;
	uint32 ExpireTime;
	uint32 HtTxParameters;	/* to be defined, but 32-bit is enough */
	uint32 Frame;
} WSM_HI_TX_REQ;

/***********************************************************************
 * 3.15 Transmit Confirmation					  *
 ***********************************************************************/
typedef struct WSM_HI_TX_CNF_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 PacketId;
	uint32 Status;		/* WSM_STATUS_XXXXX */
	uint8 TxedRate;
	uint8 AckFailures;
	uint16 Flags;
	uint32 MediaDelay;
	uint32 TxQueueDelay;
} WSM_HI_TX_CNF;

/***********************************************************************
 * 3.16 Receive Indication					     *
 ***********************************************************************/
typedef struct WSM_HI_RX_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Status;
	uint16 ChannelNumber;	/* see channel definiton in d11_basic_defs.h */
	uint8 RxedRate;		/* temp */
	uint8 RcpiRssi;
	uint32 Flags;
	uint32 Frame;
} WSM_HI_RX_IND;

/***********************************************************************
 * 3.17 Event Indication					       *
 ***********************************************************************/
typedef struct WSM_HI_EVENT_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 EventId;		/* WSM_EVENT_XXXX */
	union {
		uint32 ErrorStatus;
		uint8 RcpiRssi;
	} uEventData;
} WSM_HI_EVENT_IND;

/***********************************************************************
 * 3.18 Start-Measurement Request				      *
 ***********************************************************************/
typedef struct WSM_MEAS_CHANNEL_LOAD_PARAMS_S {
	uint8	Reserved;
	uint8	ChannelLoadCCA;
	uint16	ChannelNum;
	uint16	RandomInterval;
	uint16	MeasurementDuration;
	uint64	MeasurementStartTime;
} WSM_MEAS_CHANNEL_LOAD_PARAMS;

typedef struct WSM_MEAS_NOISE_HISTOGRAM_PARAMS_S {
	uint8	Reserved;
	uint8	IpiRpi;
	uint16	ChannelNum;
	uint16	RandomInterval;
	uint16	MeasurementDuration;
	uint64	MeasurementStartTime;
} WSM_MEAS_NOISE_HISTOGRAM_PARAMS;

typedef struct WSM_MEAS_BEACON_PARAMS_S {
	/*uint8    RegulatoryClass; */
	/*uint8    MeasurementMode; */
	/*uint16   ChannelNum; */
	uint16 RandomInterval;
	/*uint16   MeasurementDuration; */
	/*uint8    Bssid[6]; */
	uint16 Reserved;
	/*SCAN_PARAMETERS ScanParameters; */
	uint8 Band;
	uint8 ScanType;
	uint8 ScanFlags;
	uint8 MaxTransmitRate;
	uint32 AutoScanInterval;
	uint8 NumOfProbeRequests;
	uint8 NumOfChannels;
	uint8 NumOfSSIDs;
	uint8 ProbeDelay;

	/* followed by
	   WSM_SCAN_CHANNEL_DEF    Channels[NumOfChannels];
	   WSM_SSID_DEF	    SSIDs[NumOfSSIDs];
	 */
} WSM_MEAS_BEACON_PARAMS;

typedef struct WSM_MEAS_STA_STATS_PARAMS_S {
	uint8 PeerMacAddress[6];
	uint16 RandomInterval;
	uint16 MeasurementDuration;
	uint8 GroupId;
	uint8 Reserved;
} WSM_MEAS_STA_STATS_PARAMS;

typedef struct WSM_MEAS_LINK_MEASUREMENT_PARAMS_S {
	uint8 Reserved[4];
} WSM_MEAS_LINK_MEASUREMENT_PARAMS;

typedef union WSM_MEAS_REQUEST_U {
	WSM_MEAS_CHANNEL_LOAD_PARAMS ChannelLoadParams;
	WSM_MEAS_NOISE_HISTOGRAM_PARAMS NoisHistogramParams;
	WSM_MEAS_BEACON_PARAMS BeaconParams;
	WSM_MEAS_STA_STATS_PARAMS StaStatsParams;
	WSM_MEAS_LINK_MEASUREMENT_PARAMS LinkMeasurementParams;
} WSM_MEAS_REQUEST;

typedef struct WSM_HI_START_MEASUREMENT_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	sint32 TxPowerLevel;
	uint8 DurationMandatory;
	uint8 MeasurementType;
	uint8 MeasurementRequestLength;
	uint8 Reserved[5];
	WSM_MEAS_REQUEST MeasurementRequest;
} WSM_HI_START_MEASUREMENT_REQ;

/***********************************************************************
 * 3.19 Start-Measurement Confirmation				 *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_START_MEASUREMENT_CNF;

/***********************************************************************
 * 3.20 Stop-Measurement Request				       *
 ***********************************************************************/
typedef HI_MSG_HDR WSM_HI_STOP_MEASUREMENT_REQ;

/***********************************************************************
 * 3.21 Stop-Measurement Confirmation				  *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_STOP_MEASUREMENT_CNF;

/***********************************************************************
 * 3.22 Measurement Complete Indication				*
 ***********************************************************************/
typedef struct WSM_MEAS_CHANNEL_LOAD_RESULTS_S {
	uint8 Reserved;
	uint8 ChannelLoadCCA;
	uint16 ChannelNum;
	uint64 ActualMeasurementStartTime;
	uint16 MeasurementDuration;
	uint8 CCAbusyFraction;
	uint8 ChannelLoad;
} WSM_MEAS_CHANNEL_LOAD_RESULTS;

typedef struct WSM_MEAS_NOISE_HISTOGRAM_RESULTS_S {
	uint16 Reserved;
	uint16 ChannelNum;
	uint64 ActualMeasurementStartTime;
	uint16 MeasurementDuration;
	uint8 AntennaID;
	uint8 IpiRpi;
	uint8 PI_0_Density;
	uint8 PI_1_Density;
	uint8 PI_2_Density;
	uint8 PI_3_Density;
	uint8 PI_4_Density;
	uint8 PI_5_Density;
	uint8 PI_6_Density;
	uint8 PI_7_Density;
	uint8 PI_8_Density;
	uint8 PI_9_Density;
	uint8 PI_10_Density;
	uint8 Reserved2;
} WSM_MEAS_NOISE_HISTOGRAM_RESULTS;

typedef struct WSM_MEAS_BEACON_RESULTS_S {
	uint16 MeasurementDuration;
	uint16 Reserved;
	uint64 StartTsf;
	uint64 Duration;
} WSM_MEAS_BEACON_RESULTS;

typedef struct WSM_HI_MEAS_RX_BEACON_FRAME_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 AntennaID;
	uint8 Reserved;
	uint16 NoiseLeveldBm;
	uint64 ParentTsf;
	WSM_HI_RX_IND WsmReceiveInd;
} WSM_HI_MEAS_RX_BEACON_FRAME_IND;

typedef struct WSM_MEAS_STA_STATS_RESULTS_S {
	uint16 MeasurementDuration;
	uint8 GroupId;
	uint8 StatisticsGroupDataLength;
	uint8 StatisticsGroupData[52];	/*variable length*/
} WSM_MEAS_STA_STATS_RESULTS;

typedef struct WSM_MEAS_LINK_MEASUREMENT_RESULTS_S {
	sint16 TransmitPower;
	uint8 RxAntennaID;
	uint8 TxAntennaID;
	sint32 NoiseLeveldBm;
	sint8 LatestRssi;
	uint8 Reserved1;
	uint8 Reserved2;
	uint8 Reserved3;
} WSM_MEAS_LINK_MEASUREMENT_RESULTS;

typedef union WSM_MEAS_REPORT_U {
	WSM_MEAS_CHANNEL_LOAD_RESULTS ChannelLoadResults;
	WSM_MEAS_NOISE_HISTOGRAM_RESULTS NoiseHistogramResults;
	WSM_MEAS_BEACON_RESULTS BeaconResults;
	WSM_MEAS_STA_STATS_RESULTS StaStatsResults;
	WSM_MEAS_LINK_MEASUREMENT_RESULTS LinkMeasurementResults;
} WSM_MEAS_REPORT;

typedef struct WSM_HI_MEASURE_CMPL_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 Dot11PowerMgmtMode;
	uint8 MeasurementType;
	uint16 MoreInd;		/* Set to 1 if more indications are to follow for this measurement, otherwise 0*/
	uint32 Status;
	uint8 MeasurementReportLength;
	uint8 Reserved2[3];
	WSM_MEAS_REPORT MeasurementReport;
} WSM_HI_MEASURE_CMPL_IND;

/***********************************************************************
 * 3.23 Join Request						   *
 ***********************************************************************/
typedef struct WSM_HI_JOIN_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 Mode;
	uint8 Band;
	uint16 ChannelNumber;
	uint8 BSSID[6];
	uint16 AtimWindow;
	uint8 PreambleType;
	uint8 ProbeForJoin;
	uint8 Reserved;
	uint8 Flags;
	uint32 SSIDLength;
	uint8 SSID[32];
	uint32 BeaconInterval;
	uint32 BasicRateSet;
} WSM_HI_JOIN_REQ;

/***********************************************************************
 * 3.24 Join Confirmation					      *
 ***********************************************************************/
typedef struct WSM_HI_JOIN_CNF_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Status;
	sint32 MinPowerLevel;
	sint32 MaxPowerLevel;
} WSM_HI_JOIN_CNF;

/***********************************************************************
 * 3.25 Set-PM-Mode Request					    *
 ***********************************************************************/
typedef struct WSM_HI_SET_PM_MODE_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 PmMode;		/* use flag WSM_PM_F_FAST_PSM_ENABLE to enable*/
	/* the fast power-saving mode*/
	uint8 FastPsmIdlePeriod;	/* in unit of 500us; 0 to use default; */
	uint8 ApPsmChangePeriod;	/* in unit of 500us; 0 to use default; */
	uint8 MinAutoPsPollPeriod;	/* in unit of 500us; 0 to disable auto-pspoll; */
} WSM_HI_SET_PM_MODE_REQ;

/***********************************************************************
 * 3.26 Set-PM-Mode Confirmation				       *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_SET_PM_MODE_CNF;

/***********************************************************************
 * 3.27 Set-PM-Mode Completion					 *
 ***********************************************************************/
typedef struct WSM_HI_SET_PM_MODE_CMPL_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Status;
	uint8 PmMode;
	uint8 Reserved[3];
} WSM_HI_SET_PM_MODE_CMPL_IND;

/***********************************************************************
 * 3.28 Set-BssParams Request					  *
 ***********************************************************************/
typedef struct WSM_HI_SET_BSS_PARAMS_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
#if (USE_SET_BSS_PARAMS_FLAGS)
	uint8 Flags;
#else
	uint8 Reserved;
#endif
	uint8 BeaconLostCount;
	uint16 AID;
	uint32 OperationalRateSet;
} WSM_HI_SET_BSS_PARAMS_REQ;

/***********************************************************************
 * 3.29 Set-BssParams Confirmation				     *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_SET_BSS_PARAMS_CNF;

/***********************************************************************
 * 3.30 Add-Key Request						*
 ***********************************************************************/

typedef union WSM_PRIVACY_KEY_DATA_U {
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

} WSM_PRIVACY_KEY_DATA;

typedef struct WSM_HI_ADD_KEY_REQ_S WSM_HI_ADD_KEY_REQ;
struct WSM_HI_ADD_KEY_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 Type;
	uint8 EntryIndex;
	uint16 Reserved;
	WSM_PRIVACY_KEY_DATA Key;
};

/***********************************************************************
 * 3.31 Add-Key Confirmation					   *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_ADD_KEY_CNF;

/***********************************************************************
 * 3.32 Remove-Key Request					     *
 ***********************************************************************/
typedef struct WSM_HI_REMOVE_KEY_REQ_S WSM_HI_REMOVE_KEY_REQ;
struct WSM_HI_REMOVE_KEY_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 EntryIndex;
	uint8 Reserved[3];
};

/***********************************************************************
 * 3.33 Remove-Key Confirmation					*
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_REMOVE_KEY_CNF;

/***********************************************************************
 * 3.34 Tx Queue Params Request					*
 ***********************************************************************/
typedef struct WSM_HI_TX_QUEUE_PARAMS_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 QueueId;
	uint8 Reserved2;
	uint8 AckPolicy;
	uint8 Reserved1;
	uint32 MaxTransmitLifetime;
	uint16 AllowedMediumTime;
	uint16 Reserved3;
} WSM_HI_TX_QUEUE_PARAMS_REQ;

/***********************************************************************
 * 3.35 Tx Queue Params Confirmation				   *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_TX_QUEUE_PARAMS_CNF;

/***********************************************************************
 * 3.36 EDCA Params Request					    *
 ***********************************************************************/
typedef struct WSM_HI_EDCA_PARAMS_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint16 CwMin[4];
	uint16 CwMax[4];
	uint8 AIFSN[4];
	uint16 TxOpLimit[4];
	uint32 MaxReceiveLifetime[4];
} WSM_HI_EDCA_PARAMS_REQ;

/***********************************************************************
 * 3.37 EDCA Params Confirmation				       *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_EDCA_PARAMS_CNF;

/***********************************************************************
 * 3.38 Set System Info Request					*
 ***********************************************************************/
typedef struct WSM_HI_SET_SYSTEM_INFO_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 HostSwVersion[32];
	uint8 HostSwWsmApiSup[4];	/* Hex ascii rep of WSM_HI_API_VER */
	uint8 HostSwDateStamp[8];	/* format yyyymmdd */
	uint8 HostSwTimeStamp[8];	/* format hhmmss */
	uint8 HostIfVersion[32];
	uint8 HostIfWsmApiSup[4];	/* Hex ascii rep of WSM_HI_API_VER */
	uint8 HostIfDateStamp[8];	/* format yyyymmdd */
	uint8 HostIfTimeStamp[8];	/* format hhmmss */
} WSM_HI_SET_SYSTEM_INFO_REQ;

/***********************************************************************
 * 3.39 Set System Info Confirmation				   *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_SET_SYSTEM_INFO_CNF;

/***********************************************************************
 * 3.40 Switch-Channel Request				    *
 ***********************************************************************/
#define WSM_HI_SC_CHANNELMODE_ALLOWTX   0
#define WSM_HI_SC_CHANNELMODE_STOPTX    1
#define WSM_HI_SC_CHANNELMODE_ENHANCED  2

#define WSM_HI_SC_F_DEVICE_SYNC_ON_START    0x01
#define WSM_HI_SC_F_DEVICE_SYNC_AT_END      0x02
#define WSM_HI_SC_F_FOREIGN_BSS_SYNC        0x04
#define WSM_HI_SC_F_PREAMBLE_TYPE_MASK      0x30
#define WSM_HI_SC_F_NO_START_IND            0x40
#define WSM_HI_SC_F_ABORT                   0x80

#define WSM_HI_SC_F_PREAMBLE_TYPE_BITSHIFT  4

#define WSM_HI_SC_STATE_COMPLETED           0
#define WSM_HI_SC_STATE_COMPLETED_EARLY     1
#define WSM_HI_SC_STATE_ABORTED             2
#define WSM_HI_SC_STATE_SWITCHING           3

typedef struct WSM_HI_SWITCH_CHANNEL_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
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
} WSM_HI_SWITCH_CHANNEL_REQ;

/***********************************************************************
 * 3.41 Switch-Channel Confirmation				    *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_SWITCH_CHANNEL_CNF;

/***********************************************************************
 * 3.42 Switch-Channel Complete Indication			     *
 ***********************************************************************/
typedef struct WSM_HI_SWITCH_CHANNEL_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 State;
} WSM_HI_SWITCH_CHANNEL_IND;

/***********************************************************************
 * 3.43 Start Request						  *
 ***********************************************************************/
typedef struct WSM_HI_START_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 Mode;
	uint8 Band;
	uint16 ChannelNumber;
	uint32 CTWindow;
	uint32 BeaconInterval;
	uint8 DTIMPeriod;
	uint8 PreambleType;
	uint8 ProbeDelay;
	uint8 SsidLength;
	uint8 Ssid[32];
	uint32 BasicRateSet;
} WSM_HI_START_REQ;

/***********************************************************************
 * 3.44 Start Confirmation					     *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_START_CNF;

/***********************************************************************
 * 3.45 Beacon-Transmit Request					*
 ***********************************************************************/
typedef struct WSM_HI_BEACON_TRANSMIT_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 EnableBeaconing;
	uint8 Reserved[3];
} WSM_HI_BEACON_TRANSMIT_REQ;

/***********************************************************************
 * 3.46 Beacon-Transmit Confirmation				   *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_BEACON_TRANSMIT_CNF;

/***********************************************************************
 * 3.47 Start-Find Request					     *
 ***********************************************************************/
typedef struct WSM_HI_START_FIND_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
} WSM_HI_START_FIND_REQ;

/***********************************************************************
 * 3.48 Start-Find Confirmation					*
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_START_FIND_CNF;

/***********************************************************************
 * 3.49 Stop-Find Request					      *
 ***********************************************************************/
typedef struct WSM_HI_STOP_FIND_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
} WSM_HI_STOP_FIND_REQ;

/***********************************************************************
 * 3.50 Stop-Find Confirmation					 *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_STOP_FIND_CNF;

/***********************************************************************
 * 3.51 Find-Complete Indication				       *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_FIND_CMPL_IND;

/***********************************************************************
 * 3.52 Suspend-Resume-Tx Indication				   *
 ***********************************************************************/
typedef struct WSM_HI_SUSPEND_RESUME_TX_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Flags;
} WSM_HI_SUSPEND_RESUME_TX_IND;

/***********************************************************************
 * 3.53 Update-IE Request					      *
 ***********************************************************************/
typedef struct WSM_HI_UPDATE_IE_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint16 Flags;
	uint16 NumIEs;
} WSM_HI_UPDATE_IE_REQ;

/***********************************************************************
 * 3.54 Update-IE Confirmation					 *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_UPDATE_IE_CNF;

/***********************************************************************
 * 3.55 Map-Link Request					       *
 ***********************************************************************/
typedef struct WSM_HI_MAP_LINK_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 MacAddr[6];
	uint8 Reserved[2];
} WSM_HI_MAP_LINK_REQ;

/***********************************************************************
 * 3.56 Map-Link Confirmation					  *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_MAP_LINK_CNF;

/***********************************************************************
 * 3.58 PTA Statistics Confirmation				    *
 ***********************************************************************/
typedef struct WSM_HI_PTA_STATISTICS_INFO_PER_LINK_S {
	uint32 BluetoothLinkId;
	uint32 NumberOfRequests;
	uint32 NumberRequestsGranted;
} WSM_HI_PTA_STATISTICS_INFO_PER_LINK;
typedef struct WSM_HI_PTA_STATISTICS_CNF_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Status;
	uint32 NumberOfBluetoothLinks;
	WSM_HI_PTA_STATISTICS_INFO_PER_LINK InfoPerLink[1];
} WSM_HI_PTA_STATISTICS_CNF;

/***********************************************************************
 * 3.59 Multi-Transmit Confirmation				    *
 ***********************************************************************/
typedef struct WSM_HI_MULTI_TRANSMIT_CNF_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 NumTxConfs;
	uint32 TxConfPayload;	/* Variable length */
} WSM_HI_MULTI_TRANSMIT_CNF;


/***********************************************************************
 * 3.61 Debug Indication					                           *
 ***********************************************************************/
#define WSM_HI_DEBUG_IND__EPTA_NRT_STATS__RESERVED 6
typedef struct WSM_HI_DEBUG_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 DbgId;	/* WSM_DEBUG_IND_XXXX */
	union WSM_HI_DEBUG_IND__DEBUG_DATA_U {
		struct WSM_HI_DEBUG_IND__EPTA_RT_STATS_S {
			uint32 MsgStartIdentifier;
			uint32 IsBtRt;
			uint32 Timestamp;
			uint32 LinkId;
			uint32 NumRequests;
			uint32 NumGrants;
			uint32 ServiceWindow;
			uint32 ServiceInterval;
			uint32 BtTrafficPriority;
			uint32 BtTrafficType;
			uint32 IsBtMaster;
		} EptaRtStats;
		struct WSM_HI_DEBUG_IND__EPTA_NRT_STATS_S {
			uint32 MsgStartIdentifier;
			uint32 IsBtRt;
			uint32 Timestamp;
			uint32 NumRequests;
			uint32 NumGrants;
			uint32 Reserved[WSM_HI_DEBUG_IND__EPTA_NRT_STATS__RESERVED];
		} EptaNrtStats;
		uint32 RawData[1];
	} uDbgData;
} WSM_HI_DEBUG_IND;


/***********************************************************************
 * Configure Block Ack Agreement Request			       *
 ***********************************************************************/
typedef struct WSM_HI_CONFIG_BLOCK_ACK_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 Mode;
	uint8 WinSize;
	uint8 TID;
	uint8 Reserved;
	uint8 TransmitAddress[6];
	uint16 SSN;
	uint16 Timeout;
	uint16 Reserved2;
} WSM_HI_CONFIG_BLOCK_ACK_REQ;

/***********************************************************************
 * Configure Block Ack Agreement Confirmation			  *
 ***********************************************************************/
typedef struct WSM_HI_CONFIG_BLOCK_ACK_CNF_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Status;
	uint32 FreeAgreements;
} WSM_HI_CONFIG_BLOCK_ACK_CNF;

/***********************************************************************
 * Block Ack Timeout Indication					*
 ***********************************************************************/
typedef struct WSM_HI_BA_TIMEOUT_IND_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint8 TID;
	uint8 Reserved;
	uint8 TransmitAddress[6];
} WSM_HI_BA_TIMEOUT_IND;
/***********************************************************************
 * WSM MIB DEFINITIONS						 *
 ***********************************************************************/

/*
 * 4.5 dot11GroupAddressesTable
 */
#define WSM_MAX_GRP_ADDRTABLE_ENTRIES       8

typedef struct WSM_MIB_GRP_ADDR_TABLE_S {
	uint32 Enable;
	uint32 NumOfAddresses;
	D0_ADDRESS AddressList[WSM_MAX_GRP_ADDRTABLE_ENTRIES];
} WSM_MIB_GRP_ADDR_TABLE;

/*
 * 4.6 dot11WepdefaultKeyId
 */

typedef struct WSM_MIB_WEP_DEFAULT_KEY_ID_S {
	uint8 WepDefaultKeyId;
	uint8 Reserved[3];
} WSM_MIB_WEP_DEFAULT_KEY_ID;

/*
 * 4.10 ArpIpAddressesTable
 */
typedef struct WSM_MIB_ARP_IP_ADDR_TABLE_S {
	uint32 Enable;
	uint8 Ipv4Address[4];
} WSM_MIB_ARP_IP_ADDR_TABLE;

/*
 * 4.11 TemplateFrame
 */
#define WSM_MAX_TEMPLATE_FRAME_SIZE     1024

typedef struct WSM_MIB_TEMPLATE_FRAME_S {
	uint8 FrameType;
	uint8 Rate;
	uint16 FrameLength;
	uint32 Frame[WSM_MAX_TEMPLATE_FRAME_SIZE / 4];
} WSM_MIB_TEMPLATE_FRAME;

/*
 * 4.13 BeaconFilterTable
 */
#define BEACON_IE_F_CHANGED	     (1 << 0)
#define BEACON_IE_F_NO_LONGER_PRESENT   (1 << 1)
#define BEACON_IE_F_APPEARED	    (1 << 2)

#define BEACON_IE_MATCH_COUNT_SHIFT     (4)

typedef struct WSM_IE_TABLE_S {
	uint8 IeId;
	uint8 ActionFlags;	/* Bits 7:4 = Match Data byte count */
	uint8 Oui[3];		/* Only applicable to IE 221	*/
	uint8 MatchData[3];	/*  "       "       "  "  "	 */
} WSM_IE_TABLE;

typedef struct WSM_MIB_BCN_FILTER_TABLE_S {
	uint32 NumOfInfoElmts;
	WSM_IE_TABLE IeTable[4];
} WSM_MIB_BCN_FILTER_TABLE;

/*
 *  4.14 BeaconFilterEnable
 */
#define WSM_BEACON_FILTERING_DISABLE  0
#define WSM_BEACON_FILTERING_ENABLE   1

typedef struct WSM_MIB_BCN_FILTER_ENABLE_S {
	uint32 Enable;
	uint32 BcnCount;
} WSM_MIB_BCN_FILTER_ENABLE;

/*
 * 4.16 BeaconWakeUpPeriod
 */
typedef struct WSM_MIB_BEACON_WAKEUP_PERIOD_S {
	uint8 NumBeaconPeriods;
	uint8 Reserved;
	uint16 ListenInterval;
} WSM_MIB_BEACON_WAKEUP_PERIOD;

/*
 * 4.17 RcpiRssiThreshold
 */
typedef struct WSM_MIB_RCPI_RSSI_THRESHOLD_S {
	uint8 RssiRcpiMode;
	uint8 LowerThreshold;
	uint8 UpperThreshold;
	uint8 RollingAverageCount;
} WSM_MIB_RCPI_RSSI_THRESHOLD;

/*
 * 4.18 StatisticsTable
 */
typedef struct WSM_MIB_STATS_TABLE_S {
	uint16 LatestSnr;
	uint8 LatestRcpi;
	sint8 LatestRssi;
} WSM_MIB_STATS_TABLE;

/*
 * 4.20 CountersTable
 */
typedef struct WSM_MIB_COUNT_TABLE_S {
	uint32 countPlcpErrors;
	uint32 countFcsErrors;
	uint32 countTxPackets;
	uint32 countRxPackets;
	uint32 countRxPacketErrors;
	uint32 countRxDecryptionFailures;
	uint32 countRxMicFailures;
	uint32 countRxNoKeyFailures;
	/* WBF00004752 - Start */
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
	/* WBF00004752 - End */
	uint32 countRxCMACICVErrors;
	uint32 countRxCMACReplays;
	uint32 countRxMgmtCCMPReplays;
	uint32 countRxBIPMICErrors;
} WSM_MIB_COUNT_TABLE;

/*
 * 4.21 BlockAckPolicy
 */
typedef struct WSM_MIB_BLOCK_ACK_POLICY_S {
	uint8 BlockAckTxTidPolicy;
	uint8 Reserved1;
	uint8 BlockAckRxTidPolicy;
	uint8 Reserved2;
} WSM_MIB_BLOCK_ACK_POLICY;

/*
 * 4.22 OverrideManagementRate
*/
typedef struct WSM_MIB_OVERRIDE_INT_RATE_S {
	uint8 InternalTxRate;
	uint8 NonErpInternalTxRate;
	uint8 Reserved[2];
} WSM_MIB_OVERRIDE_INT_RATE;

/*
 * 4.23 SetAssociationMode
*/
#define WSM_SAM_F_PREAMBLE_ENABLE	 (1 << 0)
#define WSM_SAM_F_MIXED_GREEN_ENABLE      (1 << 1)
#define WSM_SAM_F_BASICRATESET_ENABLE     (1 << 2)
/* WBF00003660 */
#define WSM_SAM_F_MPDUSTARTSPACING_ENABLE (1 << 3)
#define WSM_SAM_MIXED_MODE		 0
#define WSM_SAM_GREENFIELD_MODE	    1

typedef struct WSM_MIB_SET_ASSOCIATION_MODE_S {
	uint8 Flags;
	uint8 PreambleType;
	uint8 MixedOrGreenfieldType;
	uint8 MpduStartSpacing;	/* WBF00003660 */
	uint32 BasicRateSet;
} WSM_MIB_SET_ASSOCIATION_MODE;

/*
 * 4.24 UpdateEptaConfigData (updated for WBF00003406)
*/
typedef struct WSM_MIB_UPDATE_EPTA_CONFIG_DATA_S {
	uint32 EnablePta;
	uint32 BluetoothQuality;
	uint32 SubscribeBtEvent;
	uint32 PtaDebugCommand;
	uint32 Reserved;
} WSM_MIB_UPDATE_EPTA_CONFIG_DATA;

/*
 * 4.26 SetUapsdInformation
*/

#define WSM_SUI_F_TRIGGER_ACS_MASK	(0x000F)
#define WSM_SUI_F_DELIVERY_ACS_MASK       (0x0F00)
#define WSM_SUI_DELIVERY_ACS_SHIFT	8
#define WSM_SUI_F_PSEUDO_UAPSD_ENABLE     (0x0010)
#define WSM_SUI_F_DONT_APPEND_PSPOLL      (0x0020)

typedef struct WSM_MIB_SET_UAPSD_INFORMATION_S {
	uint16 UapsdFlags;
	uint16 MinAutoTriggerInterval;
	uint16 MaxAutoTriggerInterval;
	uint16 AutoTriggerStep;
} WSM_MIB_SET_UAPSD_INFORMATION;

/*
 * 4.28 SetTxRateRetryPolicy
*/
typedef struct WSM_MIB_TX_RATE_RETRY_POLICY_S {
	uint8 PolicyIndex;
	uint8 ShortRetryCount;
	uint8 LongRetryCount;
	uint8 PolicyFlags;
	uint8 RateRecoveryCount;
	uint8 Reserved[3];
	uint32 RateCountIndices_07_00;
	uint32 RateCountIndices_15_08;
	uint32 RateCountIndices_23_16;
} WSM_MIB_TX_RATE_RETRY_POLICY;

typedef struct WSM_MIB_SET_TX_RATE_RETRY_POLICY_S {
	uint8 NumTxRatePolicies;
	uint8 Reserved[3];
	WSM_MIB_TX_RATE_RETRY_POLICY TxRateRetryPolicy;
} WSM_MIB_SET_TX_RATE_RETRY_POLICY;

/*
 * 4.29 SetHostMessageTypeFilter
*/
typedef struct WSM_MIB_SET_HOST_MSG_TYPE_FILTER_S {
	uint16 FrameCtrlFilterMask;
	uint16 FrameCtrlFilterValue;
} WSM_MIB_SET_HOST_MSG_TYPE_FILTER;

/*
 * 4.30 P2PFindInfo
*/
typedef struct WSM_MIB_P2P_FIND_INFO_S {
	uint8 Band;
	uint8 Reserved;
	uint16 ListenChannel;
	uint8 P2pMinDiscoverableTime;
	uint8 P2pMaxDiscoverableTime;
	uint8 MaxTransmitRate;
	uint8 ProbeDelay;
	uint16 FindTimeOut;
	uint8 NumProbs;
	uint8 NumChannels;
	/* followed by

	   CHANNEL SocialChannels[NumOfChannels];
	 */
} WSM_MIB_P2P_FIND_INFO;

/*
 * 4.31 P2PPsModeInfo
*/
typedef struct WSM_MIB_P2P_PS_MODE_INFO_S {
	uint8 OppPsCTWindow;
	uint8 Count;
	uint8 Reserved;
	uint8  DtimCount;
	uint32 Duration;
	uint32 Interval;
	uint32 StartTime;
} WSM_MIB_P2P_PS_MODE_INFO;

/* Data Frame Filter Support*/
#define WSM_MIB_DATA_FRAME_FILTERMODE_DISABLED      0
#define WSM_MIB_DATA_FRAME_FILTERMODE_FILTEROUT     1
#define WSM_MIB_DATA_FRAME_FILTERMODE_FILTERIN      2
#define WSM_MAX_FILTER_ELEMENTS		     4

/*
 * 4.32 SetEtherTypeDataFrameFilter WO
*/

typedef struct WSM_MIB_ETHERTYPE_DATA_FRAME_FILTER_S {
	uint8 FilterMode;
	uint8 Reserved;
	uint16 EtherType;
} WSM_MIB_ETHERTYPE_DATA_FRAME_FILTER;

typedef struct WSM_MIB_ETHERTYPE_DATA_FRAME_FILTER_SET_S {
	uint8 NrFilters;
	uint8 Reserved[3];
	WSM_MIB_ETHERTYPE_DATA_FRAME_FILTER EtherTypeFilter[WSM_MAX_FILTER_ELEMENTS];
} WSM_MIB_ETHERTYPE_DATA_FRAME_FILTER_SET;

/*
 * 4.33 SetUDPPortDataFrameFilter - WO
*/

typedef struct WSM_MIB_UDPPORTS_DATA_FRAME_FILTER_S {
	uint8 FilterMode;
	uint8 IsSrcPort;
	uint16 UDPPort;
} WSM_MIB_UDPPORTS_DATA_FRAME_FILTER;

typedef struct WSM_MIB_UDPPORTS_DATA_FRAME_FILTER_SET_S {
	uint8 NrFilters;
	uint8 Reserved[3];
	WSM_MIB_UDPPORTS_DATA_FRAME_FILTER UDPPortsFilter[WSM_MAX_FILTER_ELEMENTS];
} WSM_MIB_UDPPORTS_DATA_FRAME_FILTER_SET;

/*
 * 4.34 SetMagicDataFrameFilter - WO
*/

#define WSM_MAX_MAGIC_PATTERN_LENGTH		32
typedef struct WSM_MIB_MAGIC_DATA_FRAME_FILTER_S {
	uint8 FilterMode;
	uint8 Offset;		/* offset from the end of 802.11 header*/
	uint8 MagicPatternLength;
	uint8 Reserved;
	uint8 MagicPattern[WSM_MAX_MAGIC_PATTERN_LENGTH];
} WSM_MIB_MAGIC_DATA_FRAME_FILTER;

/*
 * 4.35 P2PDeviceInfo
*/

typedef struct P2P_DEVICE_TYPE_S {
	uint16 CategoryId;
	uint8 Oui1_2[2];
	uint8 Oui3_4[2];
	uint16 SubCategoryId;
} P2P_DEVICE_TYPE;

typedef struct WSM_MIB_P2P_DEVICE_INFO_S {
	P2P_DEVICE_TYPE PrimaryDevice;
	uint8 Reserved1[3];
	uint8 DevNameSize;
	uint8 LocalDevName[D11_MAX_SSID_LEN];
	uint8 Reserved2[3];
	uint8 NumSecDevSupported;
	P2P_DEVICE_TYPE SecondaryDevices[1];
	/*Further Secondary Device Information should follow */
} WSM_MIB_P2P_DEVICE_INFO;

/*
 * 4.36 SetWCDMABand - WO
*/
typedef struct WSM_MIB_WCDMA_BAND_S {
	uint8 WCDMA_Band;
	uint8 Reserved[3];
} WSM_MIB_WCDMA_BAND;

/*
 * 4.37 GroupTxSequenceCounter - RO
*/
typedef struct WSM_MIB_GROUP_SEQ_COUNTER_S {
	uint32 Bits_47_16;
	uint16 Bits_15_00;
	uint16 Reserved;
} WSM_MIB_GROUP_SEQ_COUNTER;

#define WSM_GPIO_COMMAND_SETUP    0
#define WSM_GPIO_COMMAND_READ     1
#define WSM_GPIO_COMMAND_WRITE    2
#define WSM_GPIO_COMMAND_RESET    3
#define WSM_GPIO_ALL_PINS	 0xFF

/*
 * 4.39 SetHtProtection - WO
*/
#define WSM_DUAL_CTS_PROT_ENB		(1 << 0)
#define WSM_NON_GREENFIELD_STA_PRESENT	(1 << 1)
#define WSM_HT_PROT_MODE__NO_PROT	(0 << 2)
#define WSM_HT_PROT_MODE__NON_MEMBER	(1 << 2)
#define WSM_HT_PROT_MODE__20_MHZ	(2 << 2)
#define WSM_HT_PROT_MODE__NON_HT_MIXED	(3 << 2)
#define WSM_LSIG_TXOP_PROT_FULL		(1 << 4)
#define WSM_LARGE_L_LENGTH_PROT		(1 << 5)

/*
 * 4.40 GPIO Command - R/W
*/
typedef struct WSM_MIB_GPIO_COMMAND_S {
	uint8 GPIOCommand;
	uint8 Pin;
	uint16 Config;
} WSM_MIB_GPIO_COMMAND;

/*
 * 4.41 TSFCounter -  RO
*/
typedef struct WSM_MIB_TSF_COUNTER_S {
	uint32 TSFCounterlo;
	uint32 TSFCounterhi;
} WSM_MIB_TSF_COUNTER;

/*
 * Block Ack Table - For Test Purposes Only
 */
typedef struct WSM_MIB_BLOCK_ACK_INFO_S {
	uint8 BufferSize;
	uint8 MaxNumAgreements;
	uint8 Reserved[2];
} WSM_MIB_BLOCK_ACK_INFO;

/*
 * 4.43 Keep-alive period -  WO
*/
typedef struct WSM_MIB_KEEP_ALIVE_PERIOD_S {
	uint16 KeepAlivePeriod;	/* in seconds */
	uint8 Reserved[2];
} WSM_MIB_KEEP_ALIVE_PERIOD;


/*
 * 4.43 Keep-alive period -  WO
*/
typedef struct WSM_MIB_DISABLE_BSSID_FILTER_S {
	uint8 Filter	;
	uint8 Reserved[3];
} WSM_MIB_DISABLE_BSSID_FILTER;

/*
* 4.46 ARP Keep-alive period -   MIB Id 0x1028  W
*/
typedef struct WSM_MIB_ARP_KEEP_ALIVE_PERIOD_S
{
    uint16  ArpKeepAlivePeriod; /* in seconds */
    uint8   EncrType; /* (ex. WSM_KEY_TYPE_WEP_DEFAULT) */
    uint8   Reserved;
    uint8   SenderIpv4Address[4]; // in uint32 big endian format
    uint8   TargetIpv4Address[4];
} WSM_MIB_ARP_KEEP_ALIVE_PERIOD;


typedef union WSM_MIB_DATA_U {
	D0_ADDRESS dot11StationId;	/* 4.1  dot11StationId */
	uint32 dot11MaxTransmitMsduLifeTime;	/* 4.2  dot11MaxtransmitMsduLifeTime */
	uint32 dot11MaxReceiveLifeTime;	/* 4.3  dot11MaxReceiveLifeTime */
	uint32 dot11SlotTime;	/* 4.4  dot11SlotTime */
	WSM_MIB_GRP_ADDR_TABLE dot11GroupAddressesTable;	/* 4.5  dot11GroupAddressesTable */
	WSM_MIB_WEP_DEFAULT_KEY_ID dot11WepdefaultKeyId;	/* 4.6  dot11WepDefaultKeyId */
	sint32 dot11CurrentTxPowerLevel;	/* 4.7  dot11CurrentTxPowerLevel */
	uint32 dot11RtsThreshold;	/* 4.8  dot11RTSThreshold */
	uint32 useCtsToSelf;	/* 4.9  NonErpProtection */
	WSM_MIB_ARP_IP_ADDR_TABLE ArpIpAddressesTable;	/* 4.10 ArpIpAddressesTable */
	WSM_MIB_TEMPLATE_FRAME TemplateFrame;	/* 4.11 TemplateFrame */
	uint32 RxFilter;	/* 4.12 RxFilter */
	WSM_MIB_BCN_FILTER_TABLE BeaconFilterTable;	/* 4.13 BeaconFilterTable */
	WSM_MIB_BCN_FILTER_ENABLE BeaconFilterEnable;	/* 4.14 BeaconFilterEnable */
	uint32 OperationalPowerMode;	/* 4.15 OperationalPowerMode */
	WSM_MIB_BEACON_WAKEUP_PERIOD BeaconWakeUpPeriod;	/* 4.16 BeaconWakeUpPeriod */
	WSM_MIB_RCPI_RSSI_THRESHOLD RcpiRssiThreshold;	/* 4.17 RcpiRssiThreshold */
	WSM_MIB_STATS_TABLE StatisticsTable;	/* 4.18 StatisticsTable */
	uint32 IbssPsConfig;	/* 4.19 IbssPsConfig */
	WSM_MIB_COUNT_TABLE CountTable;	/* 4.20 CountersTable */
	WSM_MIB_BLOCK_ACK_POLICY BlockAckPolicy;	/* 4.21 BlockAckPolicy */
	WSM_MIB_OVERRIDE_INT_RATE MibOverrideInternalTxRate;	/* 4.22 OverrideInternalTxRate */
	WSM_MIB_SET_ASSOCIATION_MODE SetAssociationMode;	/* 4.23 SetAssociationMode */
	WSM_MIB_UPDATE_EPTA_CONFIG_DATA UpdateEptaConfigData;	/* 4.24 UpdateEptaConfigData */
	uint32 SelectCcaMethod;	/* 4.25 SelectCcaMethod */
	WSM_MIB_SET_UAPSD_INFORMATION SetUapsdInformation;	/* 4.26 SetUapsdInformation */
	uint32 SetAutoCalibrationMode;	/* 4.27 SetAutoCalibrationMode - WBF00004073 */
	WSM_MIB_SET_TX_RATE_RETRY_POLICY SetTxRateRetryPolicy;	/* 4.28 SetTxRateRetryPolicy */
	WSM_MIB_SET_HOST_MSG_TYPE_FILTER SetHostMessagetypeFilter;	/* 4.29 SetHostMessageTypeFilter */
	WSM_MIB_P2P_FIND_INFO P2PFindInfo;	/* 4.30 P2PFindInfo */
	WSM_MIB_P2P_PS_MODE_INFO P2PPsModeInfo;	/* 4.31 P2PPsModeInfo */
	WSM_MIB_ETHERTYPE_DATA_FRAME_FILTER_SET SetEtherTypeDataFrameFilter;	/* 4.32 SetEtherTypeDataFrameFilter */
	WSM_MIB_UDPPORTS_DATA_FRAME_FILTER_SET SetUdpPortsDataFrameFilter;	/* 4.33 SetUDPPortDataFrameFilter */
	WSM_MIB_MAGIC_DATA_FRAME_FILTER SetMagicDataFrameFilter;	/* 4.34 SetMagicDataFrameFilter */
	uint32 ProtectedMgmtFramesPolicy;	/* 4.38 Protected Management */
	WSM_MIB_P2P_DEVICE_INFO P2PDeviceInfo;	/* 4.35 P2P Device Information */
	WSM_MIB_WCDMA_BAND WCDMA_Band;	/* 4.36 SetWCDMABand */
	WSM_MIB_GROUP_SEQ_COUNTER GroupSeqCounter;	/* 4.37 Group Tx Sequence Counter */
	uint32 SetHtProtection;	/* 4.39 SetHtProtection */
	WSM_MIB_GPIO_COMMAND GPIOCommand;	/* 4.40 GPIO Command */
	WSM_MIB_TSF_COUNTER TSFCounter;	/* 4.41 TSF Counter */
	uint32 UseMultiTxConfMsg;	/* 4.42 UseMultiTxConfMessage */
	WSM_MIB_BLOCK_ACK_INFO MibBlockAckInfo;	/* Test Purposes Only */
	WSM_MIB_KEEP_ALIVE_PERIOD KeepAlivePeriod;	/* 4.43 Keep-alive */
	WSM_MIB_DISABLE_BSSID_FILTER	bssid_filter	; /* 4.44 bssid filter */
	WSM_MIB_ARP_KEEP_ALIVE_PERIOD ArpKeepAlivePeriod;	/* 4.46 ARP Keep-alive */
} WSM_MIB_DATA;

/***********************************************************************
 * 3.5 Read-MIB Request						*
 ***********************************************************************/
typedef struct WSM_HI_READ_MIB_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint16 MibId;
	uint16 Reserved;
} WSM_HI_READ_MIB_REQ;

/***********************************************************************
 * 3.6 Read-MIB Confirmation					   *
 ***********************************************************************/
typedef struct WSM_HI_READ_MIB_CNF_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint32 Status;
	uint16 MibId;
	uint16 Length;
	WSM_MIB_DATA MibData;
} WSM_HI_READ_MIB_CNF;

/***********************************************************************
 * 3.7 Write-MIB Request					       *
 ***********************************************************************/
typedef struct WSM_HI_WRITE_MIB_REQ_S {
	uint16 MsgLen;
	uint16 MsgId;
	uint16 MibId;
	uint16 Length;
	WSM_MIB_DATA MibData;
} WSM_HI_WRITE_MIB_REQ;

/***********************************************************************
 * 3.8 Write-MIB Confirmation					  *
 ***********************************************************************/
typedef WSM_HI_GENERIC_CNF WSM_HI_WRITE_MIB_CNF;

#endif				/* WSM_API_H */
