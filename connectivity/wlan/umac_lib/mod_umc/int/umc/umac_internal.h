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
 * \file umac_internal.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_internal.h
 * \brief
 * Internal Implementation functions for UMAC.
 * \ingroup Upper_MAC_Core
 * \date 05/11/08 08:41
 */

#ifndef _UMAC_INTERNAL_H
#define _UMAC_INTERNAL_H

/******************************************************************************
		      INCLUDE FILES
******************************************************************************/

#include "stddefs.h"
#include "umac_messages.h"
#include "umac_frames.h"
#include "umac_ll_if.h"
#include "umac_if.h"
#include "umac_dbg.h"

#define ALLOCATE_UMC_DEV_IF_TX_REQ(x)	\
	((uint8 *)x - sizeof(UMAC_DEVIF_TX_DATA))

#define ASSIGN_INTERNAL_DEV_IF_TX_REQ(x, y)							\
	{											\
		x->HeaderLength   = GET_TX_DESC_FRAME_LENGTH(y) - WFM_ETH_FRAME_HDR_SIZE ;	\
		x->pTxDescriptor  = y ;								\
		x->pDot11Frame    = (uint8 *)y + x->HeaderLength ;				\
		x->UlInfo	 = NULL  ;							\
	}

#define MOVE_TO_END_OF_DEV_IF_TX_REQ(x) ((uint8 *)x +		   \
					 sizeof(UMAC_DEVIF_TX_DATA))

#define MOVE_TO_START_OF_DEV_IF_TX_REQ(x)       \
	((uint8 *)x - sizeof(UMAC_DEVIF_TX_DATA))

typedef uint8 MAC_ADDR[6];

#define NUM_BLACK_LIST_ENTRIES			8

#define UMAC_BLKLST_STATUS_CLEARED		0
#define UMAC_BLKLST_STATUS_ADDED		1
#define UMAC_BLKLST_STATUS_ERROR		2
#define UMAC_BLKLST_LISTED			3
#define UMAC_BLKLST_NOT_LISTED			4

#define UMAC_DATABASE_FULL			1
#define SMGMT_REQ_TYPE_SCAN_REQ			0
#define SMGMT_REQ_TYPE_JOIN_REQ			1
#define SMGMT_REQ_TYPE_JOIN_SUCCESS		2
#define SMGMT_REQ_TYPE_ASSOC_SUCCESS		3
#define SMGMT_REQ_TYPE_DEL_ALL_KEYS		4
#define SMGMT_REQ_TYPE_DYNAMIC_UPDATIONS	5
#define SMGMT_REQ_TYPE_INIT			6

/*Management requests triggered by upper layers directly */
#define SMGMT_REQ_TYPE_HOST_UL_TRIGGERED	7
#define SMGMT_REQ_TYPE_SCAN_COMPLETE		8
#define SMGMT_REQ_TYPE_FT_AUTH_JOIN		9
#define SMGMT_REQ_TYPE_FT_AUTH_JOINBACK		10

#if DOT11K_SUPPORT
#define SMGMT_REQ_TYPE_MEASUREMENT_REQ		11
#endif	/* DOT11K_SUPPORT */

#if P2P_MINIAP_SUPPORT
#define SMGMT_REQ_TYPE_SEND_AUTH_RESP		12
#define SMGMT_REQ_TYPE_START_AP			13
#define SMGMT_REQ_TYPE_UNLINK_STA		14
#define SMGMT_REQ_TYPE_FIND_REQ			15
#define SMGMT_REQ_TYPE_GO_NEGOTIATION		16
#define SMGMT_REQ_TYPE_UPDATE_IE		17
#define SMGMT_REQ_TYPE_SEND_ASSOC_RESP		18
#define SMGMT_REQ_TYPE_SEND_DEASSOC		19
#define SMGMT_REQ_TYPE_RESET_SEQ_NUM_COUNTER	20
#endif	/* P2P_MINIAP_SUPPORT */

#define SMGMT_REQ_TYPE_PSMODE_UPDATE 21
#define SMGMT_REQ_TYPE_UPDATE_ERP_IE		22
/*This structure holds the Information regarding all the Tx's
  initiated by umac.It holds the buffer information, so that
  buffer can be freed up on completion of each Tx request.
*/
typedef struct UMAC_TX_PKT_INT_S {
	UMAC_TX_DESC *pTxDesc;
	uint32 PacketId;
	uint8 headerOffset;
	uint8 reserved[3];
	void  *pDriverInfo;
} UMAC_TX_PKT_INT;

typedef struct UMAC_PEER_DB_MINIAP_S {
	uint8 stamacaddr[6];
	uint8 blacklist_status;
	uint8 whitelist_status;
} UMAC_PEER_DB_MINIAP;

typedef struct UMAC_NEIGHBOR_BSS_DB_MINIAP_S {
	uint8 Bssid[6];
	uint8 Is11bOnly;
	uint8 IsNonErpStaPresent;
} UMAC_NEIGHBOR_BSS_DB_MINIAP;

typedef struct SMGMT_REQ_PARAMS_S {
	uint16 Type;
	uint8 ReqNum;
	uint8 HandleMgmtReq;
	uint32 status;
	UMAC_RUNTIME run_time;

	union {
		struct {
			uint8 phy_band;
			uint8 type;
			uint8 Reserved[2];
		} scan_mib_params;

		struct {
			uint8 phy_band;
			uint8 type;
			uint8 Reserved[2];
		} find_mib_params;

		struct {
			uint8 WriteBlkAckPolicy;
			uint8 reserved[3];
			uint32 UseNonProtection;
			uint32 SloteTime;
			HT_OPERATION_IE HtOperationIe;
			sint32 powerLevel;
			uint32 hostMsgTypeFilter;
		} join_mib_params;

		struct {
			uint32 RtsThreashold;

		} join_success_params;

		struct {
			uint8 WriteEdcaParam;
			uint8 WriteDefailtEdcaParam;
			uint8 WriteJoinModeBasicRateSet;
			uint8 IsQosLink;
			uint32 Result;
			uint8 QueueId;
			uint8 NumBeaconPeriods;
			uint8 Reserved[2];
			WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED EdcaParam;

		} assoc_success_params;

		struct {
			uint8 KeyEntryIndex;

		} deassoc_req_params;

		struct {
			uint8 MacAddress[6];
			uint8 reserved[2];

		} init_req_params;

		struct {
			uint16 Category;
			uint16 Activity;
			uint8 IsWepIdTowrite;
			uint8 WepDefaultKeyId;
			uint8 RemoveKeyIndex;
			uint8 Channel;
			uint8 NumBeaconPeriods;
			uint8 resv[3];
			uint8 *pPrivacyParams;
			uint8 *pEptaConfigInfo;
			uint8 *pConfigReq;
			uint8 *pConfigHiMsg;
			uint8 *pMsg;
			UMAC_SWITCH_CHANNEL_REQ SwitchChReq;
		} ul_triggered_params;

#if DOT11K_SUPPORT
		struct {
			uint8 measurementReqType;
			uint8 Reserved[3];
		} measurement_req;
#endif	/* DOT11K_SUPPORT */
#if P2P_MINIAP_SUPPORT
		struct {
			uint8 linkId;
			uint8 Reserved[3];
		} reset_seq_num_counter_params;
#endif	/* P2P_MINIAP_SUPPORT */

	} operation;

} SMGMT_REQ_PARAMS;

/*UMAC Internal Status Codes*/
#define UMAC_INT_STATUS_SUCCESS			0
#define UMAC_INT_STATUS_DUPLICATE_REQUEST	1
#define UMAC_INT_STATUS_BAD_PARAM		2
#define UMAC_INT_STATUS_VALUE_STORED		3

/******************************************************************************
 * NAME:	WFM_UMAC_Initialize_TxQueueParams_All
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets Tx Queue parameters and rates to a default value.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_Initialize_TxQueueParams_All(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_Initialize_TxQueueParams_Rates_All
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets rates params of all queues of Tx Queue parameters.
 * \param UmacHandle       - Handle to UMAC Instance.
 * \param SupportedRates   - Rates for data frames in wsm/wfm format.
 * \returns none.
 *****************************************************************************/
void UMAC_Initialize_TxQueueParams_Rates_All(UMAC_HANDLE UmacHandle,
					     uint32 SupportedRates);

/******************************************************************************
 * NAME:	WFM_UMAC_Initialize_Encryption
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets Encryption parameters to default (open/no encryption)
 * \param UmacHandle       - Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_Initialize_Encryption(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	WFM_UMAC_GetOid_EncryptionStatus
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function gets the present encryption status of the device.
 * \param UmacHandle       - Handle to UMAC Instance.
 * \param pEncryption_Status - Encryption Type in use
 *			     b0:1 => WEP is being used
 *			     b1:1 => TKIP (WPA) is being used
 *			     b2:1 => AES (WPA2) is being used
 *			     All b0/b1/b2 = 0 => No encryption is being used
 *			     b31:1 => Tx key available for that encryption mode
 * \returns uint32 WFM_STATUS_SUCCESS when successful.
 *****************************************************************************/
uint32 WFM_UMAC_GetOid_EncryptionStatus(UMAC_HANDLE UmacHandle,
					uint32 *pEncryption_Status);

/******************************************************************************
 * NAME:	WFM_UMAC_SetOid_AuthenticationMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets authentication mode to open/shared.
 * \param UmacHandle		  - Handle to UMAC Instance.
 * \param dot11AuthenticationMode - Authentication mode to set(Open/Shared)
 * \returns WFM_STATUS_SUCCESS    -  when successful
 *****************************************************************************/
uint32 WFM_UMAC_SetOid_AuthenticationMode(UMAC_HANDLE UmacHandle,
					  uint8 dot11AuthenticationMode);

/******************************************************************************
 * NAME:	WFM_UMAC_SetOid_EncryptionStatus
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets encryption type to be used by the device.
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param Encryption_Status - B0:1 => WEP can be used
 *			    B1:1 => TKIP (WPA) can be used
 *			    B2:1 => AES (WPA2) can be used
 *			    All 0 => No encryption can be used
 * \returns WFM_STATUS_SUCCESS  -  when successful
 *****************************************************************************/
uint32 WFM_UMAC_SetOid_EncryptionStatus(UMAC_HANDLE UmacHandle,
					uint32 Encryption_Status);

/******************************************************************************
 * NAME:	WFM_UMAC_SetOid_InfrastructureMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets infrastructure mode to IBSS/Infrastructure.
 * It also causes disassociation with the presently associated AP.
 * It also deletes all the Encryption Keys.
 * \param UmacHandle		  - Handle to UMAC Instance.
 * \param dot11InfrastructureMode - Infrastructure mode to
 *				    set (IBSS/Infrastructure)
 * \returns WFM_STATUS_SUCCESS    -  when successful
 *****************************************************************************/
uint32 WFM_UMAC_SetOid_InfrastructureMode(UMAC_HANDLE UmacHandle,
					  uint32 dot11InfrastructureMode);

/******************************************************************************
 * NAME:	WFM_UMAC_SetEncryptionType
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets encryption type to be used by the wfm.It uses
 * pUmacInstance->gVars.p.InfoHost.EncryptionStatusHost and
 * AuthenticationModeHost to set the
 * pUmacInstance->gVars.p.InfoHost.EncryptionType.
 * \param UmacHandle	         - Handle to UMAC Instance.
 * \param AuthenticationModeHost - Authentication mode sent by host
 * \returns WFM_STATUS_SUCCESS   -  when successful
 *****************************************************************************/
uint32 WFM_UMAC_SetEncryptionType(UMAC_HANDLE UmacHandle,
				  uint32 AuthenticationModeHost);

/******************************************************************************
 * NAME:	WFM_UMAC_UseGreenfieldMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets whether to use mixed mode or greenfield mode.
 * \param UmacHandle	      - Handle to UMAC Instance.
 * \param u8UseGreenfieldMode - TRUE -> use greenfield mode,
 *			        FALSE -> use mixed mode.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_UseGreenfieldMode(UMAC_HANDLE UmacHandle,
				  uint8 u8UseGreenfieldMode);

/******************************************************************************
 * NAME:	WFM_UMAC_UseErpProtection
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets whether to use CTS/RTS protection when sending OFDM
 * frames.
 * \param UmacHandle	      - Handle to UMAC Instance.
 * \param u8UseErpProtection  - TRUE -> use protection,
 *			        FALSE -> don't use protection.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_UseErpProtection(UMAC_HANDLE UmacHandle,
				 uint8 u8UseErpProtection);

/******************************************************************************
 * NAME:	WFM_UMAC_DeleteAllKeysOneByOne
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function deletes all keys. Encryption is disabled till a new key is set.
 * When a new key is added, encryption will be automatically enabled.This
 * function should be called repetedly until its returning zero.
 * \param UmacHandle	  - Handle to UMAC Instance.
 * \returns zero when all the keys are deleted, otherwise 1
 *****************************************************************************/
uint32 WFM_UMAC_DeleteAllKeysOneByOne(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_HandleRates
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function derives the different UMAC rate sets from the host connect
 * request.
 * \param UmacHandle     - Handle to UMAC Instance.
 * \param *pBssInfo      - Information about the AP to be connected with.
 * \returns zero when all the keys are deleted, otherwise 1
 *****************************************************************************/
void UMAC_HandleRates(UMAC_HANDLE UmacHandle, WFM_BSS_INFO *pBssInfo);

/******************************************************************************
 * NAME:	WFM_UMAC_SetOid_AddKey
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function add a key. Adding a key will not start encryption unless it
 * was enabled by the host using other OIDs.
 * \param UmacHandle	     - Handle to UMAC Instance.
 * \param *pDot11AddKey      - Pointer to WFM_OID_802_11_KEY.
 * \returns WFM_STATUS_SUCCESS  when successful
 *****************************************************************************/
uint32 WFM_UMAC_SetOid_AddKey(UMAC_HANDLE UmacHandle,
			      WFM_OID_802_11_KEY *pDot11AddKey);

/******************************************************************************
 * NAME:	WFM_UMAC_SetOid_DeleteKey
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function deletes a key. Note that deleting a key will not stop
 * encryption unless all keys were deleted. Encryption will be enabled again
 * when a key is added by the host.
 * \param UmacHandle       - Handle to UMAC Instance.
 * \param *pDot11RemoveKey - Pointer to WFM_OID_802_11_REMOVE_KEY.
 * \returns WFM_STATUS_SUCCESS  when successful
 *****************************************************************************/
uint32 WFM_UMAC_SetOid_DeleteKey(UMAC_HANDLE UmacHandle,
				 WFM_OID_802_11_REMOVE_KEY *pDot11RemoveKey);

/******************************************************************************
 * NAME:	UMAC_Initialize_Internal_TxDescList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the internal TxDescriptor list kept by umac.
 * \param UmacHandle - Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
void UMAC_Initialize_Internal_TxDescList(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_DeInit_Internal_TxDescList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function De-initializes the internal TxDescriptor list kept by umac.
 * \param UmacHandle - Handle to UMAC Instance.
 * \param linkId     - Link Id of the STA
 * \returns none.
 *****************************************************************************/
void UMAC_DeInit_Internal_TxDescList(UMAC_HANDLE UmacHandle, uint8 linkId);

/******************************************************************************
 * NAME:	UMAC_Store_Internal_TxDesc
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function stores an entry the internal TxDescriptor list kept by umac.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pTxDesc     -  TxDescpritor to be stored
 * \param umacTxDesc[] - Array of UMAC Tx Descriptors
 * \param numBuffs     - Number of buffers to be searched
 * \returns none.
 *****************************************************************************/
void UMAC_Store_Internal_TxDesc(UMAC_HANDLE UmacHandle,
				UMAC_TX_DESC *pTxDesc,
				UMAC_TX_PKT_INT umacTxDesc[],
				uint8 numBuffs,
				uint8 headerOffset,
				void *pDriverInfo);

/******************************************************************************
 * NAME:	UMAC_Release_Internal_TxDesc
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function releases an entry the internal TxDescriptor list kept by umac.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pTxDesc      -  TxDescpritor to be relased.
 * \param numBuffs      - Number of buffers to be searched
 * \returns UMAC_TX_DESC* pointer to the stored descriptor, NULL if no
 *                        descriptor found.
 *****************************************************************************/
UMAC_TX_DESC *UMAC_Release_Internal_TxDesc(UMAC_HANDLE UmacHandle,
					   UMAC_TX_DESC *pTxDesc,
					   UMAC_TX_PKT_INT umacTxDesc[],
					   uint8 numBuffs,
					   void **pDriverInfo);

/******************************************************************************
 * NAME:	UMAC_Get_Internal_TxDesc
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function Get an entry the internal TxDescriptor list kept by umac.
 * \param UmacHandle    - Handle to UMAC Instance.
 * \param *pTxDesc      - TxDescpritor to be relased.
 * \param umacTxDesc[]  - Array of UMAC Tx Descriptors
 * \param numBuffs      - Number of buffers to be searched
 * \returns UMAC_TX_DESC* pointer to the stored descriptor, NULL if no
 *                        descriptor found.
 *****************************************************************************/
UMAC_TX_DESC *UMAC_Get_Internal_TxDesc(UMAC_HANDLE UmacHandle,
					   UMAC_TX_DESC *pTxDesc,
					   UMAC_TX_PKT_INT umacTxDesc[],
					   uint8 numBuffs);

/******************************************************************************
 * NAME:	UMAC_Send_Beacons_ProbRsp_To_UpperLayer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends a Beacon or prob responce to the Upper Layer/Host
 * \param UmacHandle   - Handle to UMAC Instance.
 * \returns WFM_STATUS_SUCCESS when successful.
 *****************************************************************************/
uint32 UMAC_Send_Beacons_ProbRsp_To_UpperLayer(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_ProcessPendingTxRequests
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function returns Tx confirmation for the pending Tx requests buffered
 * in UMAC.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param linkId       - Link Id of the assoicated STA
 * \returns uint32 Corresponding UMAC status code
 *****************************************************************************/
uint32 UMAC_ProcessPendingTxRequests(UMAC_HANDLE UmacHandle, uint8 linkId);

/******************************************************************************
 * NAME:	UMAC_ProcessPendingPsBuffTxRequests
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function returns Tx confirmation for the pending PS Buffer Tx requests
 * buffered in UMAC.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param linkId       - Link Id of the assoicated STA
 * \param pTxDescriptor     - pointer to Tx Descriptor.
 * \returns uint32 Corresponding UMAC status code
 *****************************************************************************/
uint32 UMAC_ProcessPendingPsBuffTxRequests(UMAC_HANDLE UmacHandle,
					   uint8 linkId,
					   UMAC_TX_DESC *pTxDescriptor);

/******************************************************************************
 * NAME:	UMAC_ProcessSetMibRequests
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the direct setparameter request issued by upper
 * layers to WSM
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param *pMsg		-  Message to be processed
 * \returns One of the following values
 *	  UMAC_INT_STATUS_SUCCESS
 *	  UMAC_INT_STATUS_DUPLICATE_REQUEST
 *	  UMAC_INT_STATUS_BAD_PARAM
 *	  UMAC_INT_STATUS_VALUE_STORED
 *****************************************************************************/
uint32 UMAC_ProcessSetMibRequests(UMAC_HANDLE UmacHandle,
				  WFM_SET_PARAM_REQ *pMsg);

/******************************************************************************
 * NAME:	UMAC_ProcessGetMibCnf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the direct getparameter request issued by upper
 * layers to WSM
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param *pMsg		-  Message to be processed
 * \returns Corresponding UMAC status code
 *****************************************************************************/
uint32 UMAC_ProcessGetMibCnf(UMAC_HANDLE UmacHandle, WFM_GET_PARAM_CNF *pMsg);

/******************************************************************************
 * NAME:	UMAC_ProcessSetMibCnf
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function processes the direct setparameter request issued by upper
 * layers to WSM
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param *pMsg		-  Message to be processed
 * \returns Corresponding UMAC status code
 *****************************************************************************/
uint32 UMAC_ProcessSetMibCnf(UMAC_HANDLE UmacHandle, WFM_SET_PARAM_CNF *pMsg);

/******************************************************************************
 * NAME:	UMAC_ManageBlackListPeer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function manages the blacklist of peer address kept by UMAC.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pAddr       - MAC Address to be added/removed from black list.
 * \returns Corresponding UMAC black list status code
 *****************************************************************************/
uint32 UMAC_ManageBlackListPeer(UMAC_HANDLE UmacHandle, uint8 *pAddr);


/******************************************************************************
 * NAME:	UMAC_AddDeleteUserAllowedPeer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function regulates the peer access to MiniAP
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pAddr       - Mac Address + Action required.
 * \returns Corresponding status code
 *****************************************************************************/
uint32 UMAC_AddDeleteUserAllowedPeer(UMAC_HANDLE UmacHandle,  UMAC_OID_ALLOWED_DATABASE *macaddr_db);

/******************************************************************************
 * NAME:	UMAC_SearchAllowedPeer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function searches the new connection sta address in internal table
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pAddr       - Mac Address.
 * \returns corresponding action code
 *****************************************************************************/
WFM_MAC_ADDR_ACTION UMAC_SearchAllowedPeer(UMAC_HANDLE UmacHandle,  uint8 *pAddr);

/******************************************************************************
 * NAME:	UMAC_IsBlackListedPeer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function checks whether the given Address is a black listed one.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pAddr       - MAC Address to be to be verified.
 * \returns Corresponding UMAC black list status code
 *****************************************************************************/
uint32 UMAC_IsBlackListedPeer(UMAC_HANDLE UmacHandle, uint8 *pAddr);

/******************************************************************************
 * NAME:	UMAC_ClearBlackList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function clears the black list.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
void UMAC_ClearBlackList(UMAC_HANDLE UmacHandle);


/******************************************************************************
 * NAME:UMAC_ReleaseTxResources
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function releases all the resources UMAC has allocated for a Tx
 * operation.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pTxDesc     -  Pointer to TxDescriptor.
 * \param linkId       - Link Id of the STA
 * \returns none.
 *****************************************************************************/
void UMAC_ReleaseTxResources(UMAC_HANDLE UmacHandle,
			     UMAC_TX_DESC *pTxDesc,
			     uint8 linkId);

/******************************************************************************
 * NAME:	UMAC_ManageAsynMgmtOperations
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function manages the set of Management operations to be carried out in
 * a sequential fasion.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \returns Corresponding UMAC status code.
 *****************************************************************************/
uint32 UMAC_ManageAsynMgmtOperations(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	umac_interpret_wsm_status
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts WSM status codes to UMAC status codes.
 * \param wsm_status_code  - The WSM status code.
 * \returns uint32	   Corresponding wsm_status_code.
 *****************************************************************************/
uint32 umac_interpret_wsm_status(uint32 wsm_status_code);

/******************************************************************************
 * NAME:	umac_interpret_int_status
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts UMAC internal status codes to UMAC's exported
 * status codes,
 * \param int_status_code  - The internal status code.
 * \returns uint32	   Corresponding UMAC status code.
 *****************************************************************************/
uint32 umac_interpret_int_status(uint32 int_status_code);

/******************************************************************************
 * NAME:	umac_convert_to_internal_status
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function converts UMAC's exported status code UMAC internal
 * status code.
 * \param status_code  - The UMAC status code.
 * \returns uint32	 Corresponding UMAC Internal status code.
 *****************************************************************************/
uint32 umac_convert_to_internal_status(uint32 status_code);

/******************************************************************************
 * NAME:UMAC_IBSS_GetRandomBSSID
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function generates the random BSSID while creating the IBSS.
 * \param *addr - Address
 * \returns none.
 *****************************************************************************/
void UMAC_IBSS_GetRandomBSSID(uint8 *macAddr);

#if DOT11K_SUPPORT
/******************************************************************************
 * NAME:	UMAC_ProcessRRMCapabilities
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will update the RRMcapablities as per the capabilities set by
 * upper layer.
 * \param UmacHandle		- Handle to the UMAC Instance
 * \param BeaconRRMCapabilities - RRM Capabilities in Beacon
 * \returns none.
 *****************************************************************************/
void UMAC_ProcessRRMCapabilities(UMAC_HANDLE UmacHandle,
				 uint64 BeaconRRMCapabilities);

/******************************************************************************
 * NAME:	UMAC_ProcessRRMCapabilities
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will parse all the measurement request elements and stores them in
 *pUmacInstance
 * \param UmacHandle	 - Handle to the UMAC Instance
 * \param pFrame	 - Pointer to start of measurement request elements
 * \param reqElementsLen - Measurement request elements length
 * \returns none.
 *****************************************************************************/
void UMAC_ProcMeasurementRequestElements(UMAC_HANDLE UmacHandle,
					 uint8 *pFrame,
					 uint16 reqElementsLen);
/******************************************************************************
 * NAME:	UMAC_fnUpdateTotalReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will update the total req Index and check if regulatory class
 * is configured.
 * \param UmacHandle - Handle to the UMAC Instance
 * \param iIndex - Index of Request Element
 * \returns none.
 *****************************************************************************/
void UMAC_fnUpdateTotalReq(UMAC_HANDLE UmacHandle, uint8 iIndex);

#endif	/* DOT11K_SUPPORT */

#if P2P_MINIAP_SUPPORT
/******************************************************************************
 * NAME:	UMAC_Buffer_Frame
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function buffers the frame for particular LinkID.
 * \param UmacHandle - Handle to UMAC Instance.
 * \param linkId - linkID
 * \param pTxDesc - Pointer to pTxDesc
 * \param headerOffset - headerOffset
 * \returns TRUE if frame is buffered else FALSE.
 *****************************************************************************/
uint8 UMAC_Buffer_Frame(UMAC_HANDLE UmacHandle,
			uint8 linkId,
			UMAC_TX_DESC *pTxDesc,
			uint8 headerOffset,
			void *pDriverInfo);
/******************************************************************************
 * NAME:	UMAC_GetACFromPriority
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function gives the AC from the priority
 * \param priority - priority
 * \returns AC
 *****************************************************************************/
uint8 UMAC_GetACFromPriority(uint8 priority);

/******************************************************************************
 * NAME:	UMAC_CheckAllSTA_WMM_Capable
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function checks if all STA's are WMM capable
 * \param UmacHandle - Handle to UMAC Instance.
 * \returns TRUE if all STS'a are WMM capable else FALSE.
 *****************************************************************************/
uint8 UMAC_CheckAllSTA_WMM_Capable(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_Initialize_Internal_ApTxDescList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function initializes the internal TxDescriptor list kept by umac when
 * it is in AP mode.
 * \param UmacHandle - Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
void UMAC_Initialize_Internal_ApTxDescList(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_DeInit_Internal_ApTxDescList
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function De-initializes the internal TxDescriptor list kept by umac.
 * \param UmacHandle - Handle to UMAC Instance.
 * \param linkId     - Link Id of the STA
 * \returns none.
 *****************************************************************************/
void UMAC_DeInit_Internal_ApTxDescList(UMAC_HANDLE UmacHandle, uint8 linkId);

/******************************************************************************
 * NAME:UMAC_DB_Insert_STA
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function insert the station into the station database.
  * \param UmacHandle   - Handle to UMAC Instance.
 * \param *macAddr	- Address of the STA to be inserted
 * \returns uint8
 *****************************************************************************/
uint8 UMAC_DB_Insert_STA(UMAC_HANDLE UmacHandle, uint8 *macAddr);

/******************************************************************************
 * NAME:UMAC_DB_Search_STA
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function search the link ID for the provided STA MAC address.
 * \param UmacHandle	- Handle to UMAC instance.
 * \param *staAddr	- MAC Address of the STA
 * \returns Location index where the STA is inserted.
 *****************************************************************************/
uint8 UMAC_DB_Search_STA(UMAC_HANDLE UmacHandle, uint8 *staAddr);

/******************************************************************************
 * NAME:UMAC_CheckIfSTAPresent
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function checks if the STA is already present in the STA DB and retirn
 * its index
 * \param UmacHandle	- Handle to UMAC Instance.
 * \param *macAddr	- Address of the STA to be checked
 * \returns uint8
 *****************************************************************************/
uint8 UMAC_CheckIfSTAPresent(UMAC_HANDLE UmacHandle, uint8 *staAddr);

/******************************************************************************
 * NAME:UMAC_Configure_Join_Req
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function configures the Join request to be sent prior to GO negotiation
 * request.
 * \param  UmacHandle      - Handle to UMAC instance.
 * \param *pJoinParams     - Pointer to the Join Param structure
 * \returns none
 *****************************************************************************/
void UMAC_Configure_Join_Req(UMAC_HANDLE UmacHandle,
			     JOIN_PARAMETERS **pJoinParams);

/******************************************************************************
 * NAME:    UMAC_FreePendingMgmtRequests
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function free the memory allocated for pending mgmt frames when the STA
 * is being unlinked.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param linkId       - Link Id of the assoicated STA
 * \returns none
 *****************************************************************************/
void UMAC_FreePendingMgmtRequests(UMAC_HANDLE UmacHandle, uint8 linkId);

/******************************************************************************
 * NAME:UMAC_Any_Client_Dozing
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function checks whether any of the associated station is dozing.
 * \param UmacHandle - Handle to UMAC instance.
 * \returns TRUE if any STA is dozing, otherwise FALSE.
 *****************************************************************************/
uint8 UMAC_Any_Client_Dozing(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:UMAC_FlushSTA_Buffers
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function flushes the buffer of an associated STA who is going to be
 * disassociated.
 * \param UmacHandle - Handle to UMAC instance.
 * \param linkId     - Link Id of the associated STA.
 * \returns none
 *****************************************************************************/
void UMAC_FlushSTA_Buffers(UMAC_HANDLE UmacHandle, uint8 linkId);
#endif	/* P2P_MINIAP_SUPPORT */

/******************************************************************************
 * NAME:	UMAC_UpdateTxRatesBitmap
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function updates the Tx rates bitmap which is used in rate fallback
 * and rate recovery.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \returns none.
 *****************************************************************************/
void UMAC_UpdateTxRatesBitmap(UMAC_HANDLE UmacHandle, uint8 linkId);
/******************************************************************************
 * NAME:	UMAC_GetLinkId
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function returns the linkid corresponding to a given MAC address.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param RemoteMac    - MAC address of the remote STA/AP.
 * \returns uint8.
 *****************************************************************************/
uint8	UMAC_GetLinkId(UMAC_HANDLE UmacHandle, uint8 *RemoteMac);
/******************************************************************************
 * NAME:	UMAC_ProtectionRequired
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function determines if a given frame requires encryption.
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param linkId       - Link Id corresponding to the give STA/AP.
 * \param Type         - Type of the frame.
 * \param SubType      - SubType of the frame.
 * \returns uint8.
 *****************************************************************************/
uint8	UMAC_ProtectionRequired(UMAC_HANDLE UmacHandle, uint8 linkId, uint8 Type, uint8 SubType);

/******************************************************************************
 * NAME:	UMAC_FindIndexInNeighborBssDB
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function searches the new connection sta address in internal table
 * \param UmacHandle   - Handle to UMAC Instance.
 * \param *pAddr       - Mac Address.
 * \returns index of neighboring BSS if already in DB or of free space if not
 * \ already in DB
 *****************************************************************************/
uint32 UMAC_FindIndexInNeighborBssDB(UMAC_HANDLE UmacHandle,  uint8 *pAddr);
#endif	/*_UMAC_INTERNAL_H */
