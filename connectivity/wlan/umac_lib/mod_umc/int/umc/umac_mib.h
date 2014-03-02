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
 * \file umac_mib.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_mib.h
 * \brief
 * R/W Mib
 * \ingroup Upper_MAC_Core
 * \date 27-Aug-2008
 */

#ifndef _UMAC_MIB_H_
#define _UMAC_MIB_H_

/******************************************************************************
	       INCLUDE FILES
******************************************************************************/
#include "stddefs.h"
#include "umac_if.h"

typedef struct WFM_READ_MIB_PARAMETERS_S {
	uint16 MibId;
	uint16 Reserved;
} WFM_READ_MIB_PARAMETERS;

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_TemplateFrame_ProbeReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes probe request template frame to lmac.
 * \param UmacHandle -   Handle to the UMAC Instance.
 * \param PhyBand    -   The phyBand to be used.
 * \param Type       -   0 : SCAN and 1 : FIND
 * \returns WFM_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_TemplateFrame_ProbeReq(UMAC_HANDLE UmacHandle,
						uint8 PhyBand,
						uint8 Type);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_RtsThreshold
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes Rts Threshold to lmac.
 * \param UmacHandle	    -   Handle to the UMAC Instance.
 * \param u32RtsThreshold   -   Value of Rts Threshold to use.
 * \returns WFM_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_RtsThreshold(UMAC_HANDLE UmacHandle,
				      uint32 u32RtsThreshold);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_NonErpProtection
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function enables/disables non-erp protection in lmac
 * \param UmacHandle		    -   Handle to the UMAC Instance.
 * \param u32UseNonErpProtection    -   TRUE (1) (use cts/rts),
 *				        FALSE (0) (dont use cts/rts).
 * \returns WFM_STATUS_CODE     A value of 0 indicates a success.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_NonErpProtection(UMAC_HANDLE UmacHandle,
					  uint32 u32UseNonErpProtection);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_SetHTProtection
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets the SetHtProtection MIB in lmac
 * \param UmacHandle	     - Handle to the UMAC Instance.
 * \param u32setHTProtection - contains the parameters related to HT Protection
 * \returns WFM_STATUS_CODE    A value of 0 indicates a success.
 ****************************************************************************/
uint32 WFM_UMAC_WriteMib_SetHTProtection(UMAC_HANDLE UmacHandle,
					 uint32 u32setHTProtection);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_BlockAckPolicy
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets block ack policy for tx/rx directions in lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param u8BATxTidPolicy	-   Value of 0x00-0xff.
 * \param u8BARxTidPolicy	-   Value of 0x00-0xff.
 * \returns WFM_STATUS_CODE         A value of 0 indicates a success.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_BlockAckPolicy(UMAC_HANDLE UmacHandle,
					uint8 u8BATxTidPolicy,
					uint8 u8BARxTidPolicy);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_BeaconFilterTable
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes to beacon filter table.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param SetBcnFilterTable	-   Set or reset beacon filter table, 1 or 0.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_BeaconFilterTable(UMAC_HANDLE UmacHandle,
					   uint8 SetBcnFilterTable);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_BeaconFilterEnable
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function enables/disables beacon filtering.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param u32BeaconFilterEnable	-   Beacon filter enable.
 * \param u32BeaconCount	-   Nth beacon sent to host when
 *                                  u32BeaconFilterEnable is 0
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_BeaconFilterEnable(UMAC_HANDLE UmacHandle,
					    uint32 u32BeaconFilterEnable,
					    uint32 u32BeaconCount);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_RemoveKey
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function deletes a key from WSM.
 * \param UmacHandle			-   Handle to the UMAC Instance.
 * \param u8KeyEntryIndex		-   Key number to be deleted.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_RemoveKey(UMAC_HANDLE UmacHandle,
				   uint8 u8KeyEntryIndex);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_SlotTime
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs Slot Time in Lmac.
 * \param UmacHandle	   -   Handle to the UMAC Instance.
 * \param u32SlotTime	   -   Slot Time value in microseconds.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_SlotTime(UMAC_HANDLE UmacHandle, uint32 u32SlotTime);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UpdateJoinedMode_All
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes Mib for Joining.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param Flags			-   Flags.
 * \param PreambleType		-   Preamble Type.
 * \param MixedOrGreenfieldType	-   MixedOrGreenfieldType.
 * \param MpduStartSpacing      -   MpduStartSpacing.
 * \param BasicRateSet		-   BasicRateSet.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UpdateJoinedMode_All(UMAC_HANDLE UmacHandle,
					      uint8 Flags,
					      uint8 PreambleType,
					      uint8 MixedOrGreenfieldType,
					      uint8 MpduStartSpacing,
					      uint32 BasicRateSet);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UpdateJoinedMode_PreambleType
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets preamble type after joining.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param u8PreambleType	-   Preamble Type to be used.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UpdateJoinedMode_PreambleType(UMAC_HANDLE UmacHandle,
						       uint8 u8PreambleType);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UpdateJoinedMode_MixedOrGreenfieldMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets whether to use mixed mode or greenfield mode
 * for auto responses/retried frames after join.
 * \param UmacHandle		    -   Handle to the UMAC Instance.
 * \param u8MixedOrGreenfieldMode   -   Use mixed mode or greenfield mode
 *				        (WSM_UJM_MIXED_MODE => Mixed,
 *				        WSM_UJM_GREENFIELD_MODE => greenfield).
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UpdateJoinedMode_MixedOrGreenfieldMode(UMAC_HANDLE UmacHandle,
								uint8 u8MixedOrGreenfieldMode);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UpdateJoinedMode_MpduStartSpacing
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function updates MPDU spacing after join.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param u8MpduStartSpacing	-   MPDU spacing to be used.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UpdateJoinedMode_MpduStartSpacing(UMAC_HANDLE UmacHandle,
							   uint8 u8MpduStartSpacing);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UpdateJoinedMode_BasicRateSet
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function changes basic rate set to be used after joining.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param u8BasicRateSet	-   Use the specified basic rate set.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UpdateJoinedMode_BasicRateSet(UMAC_HANDLE UmacHandle,
						       uint32 u8BasicRateSet);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_WepDefaultKeyId
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs default KeyId for WEP into WSM.
 * \param UmacHandle	    -   Handle to the UMAC Instance.
 * \param pDefKeyId	    -	Default KeyId to be set.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_WepDefaultKeyId(UMAC_HANDLE UmacHandle,
					 uint8 *pDefKeyId);

/******************************************************************************
 * NAME:	WFM_UMAC_ReadMib_MacAddress
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function enables/disables non-erp protection in lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param *pu8MacAddress	-   MAC Address to be returned from LMAC.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_ReadMib_MacAddress(UMAC_HANDLE UmacHandle,
				   uint8 *pu8MacAddress);

/******************************************************************************
 * NAME:	UMAC_Read_TSF_Timer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function reads the TSF timer
 * \param UmacHandle	    -   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 UMAC_Read_TSF_Timer(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	WFM_UMAC_SendReqToWsm_TxQueueParams
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets Tx Queue parameters into wsm for a queue id.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param QueueId	-   Queue Id to be programmed.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_SendReqToWsm_TxQueueParams(UMAC_HANDLE UmacHandle,
					   uint8 QueueId);

/******************************************************************************
 * NAME:	WFM_UMAC_SendReqToWsm_TxQueueParams_All
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets all Tx Queue parameters into wsm.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_SendReqToWsm_TxQueueParams_All(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	WFM_UMAC_SendReqToWsm_SetBssParams
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets Bss Parameters.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param Falgs		-   Flags to indicate which mode firmware has to use.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_SendReqToWsm_SetBssParams(UMAC_HANDLE UmacHandle, uint8 Flags);

/******************************************************************************
 * NAME:	WFM_UMAC_SendReqToWsm_SetPsMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets PS mode in WSM.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param u8PsMode	-   Ps Mode (WFM_PS_MODE_DISABLED/WFM_PS_MODE_ENABLED).
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_SendReqToWsm_SetPsMode(UMAC_HANDLE UmacHandle,
				       UMAC_POWER_MODE *pPsMode);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_TemplateFrame_NullData
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes null data template frame to lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param IsQoS		-   1 -> QoS NULL Frame , 0 -> Non-QoS NULL Frame.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_TemplateFrame_NullData(UMAC_HANDLE UmacHandle,
						uint8 IsQoS);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_TemplateFrame_PsPoll
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes PsPoll template frame to lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_TemplateFrame_PsPoll(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_EPTAConfigData
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function updates EPTA config data to LMAC.
 * \param UmacHandle	- Handle to the UMAC Instance.
 * \param *pconfigData	- Pointer to the EPTA config data.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_EPTAConfigData(UMAC_HANDLE UmacHandle,
					WFM_UPDATE_EPTA_CONFIG_DATA *pconfigData);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_SetAutoCalibrationMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sets auto calibration mode to lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param autoCalMode	-   Value of Auto calibration to use.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_SetAutoCalibrationMode(UMAC_HANDLE UmacHandle,
						uint32 autoCalMode);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_PowerLevel
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs Power Level in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param powerLevel	-   Power Level value in 0.1 dBm.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_PowerLevel(UMAC_HANDLE UmacHandle, sint32 powerLevel);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_TxRetryRatePolicy
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs rate retry policy in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param policyIndex	-   Index of the policy.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_TxRetryRatePolicy(UMAC_HANDLE UmacHandle,
					   uint8 policyIndex);

/******************************************************************************
 * NAME:	WFM_UMAC_UpdateRatePolicy
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function updates WSM_MIB_TX_RATE_RETRY_POLICY structure.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param *pTxRateRetryPolicy	-   Pointer to the Rate retry policy.
 * \returns none.
 *****************************************************************************/
void WFM_UMAC_UpdateRatePolicy(UMAC_HANDLE UmacHandle,
			       WSM_MIB_TX_RATE_RETRY_POLICY *pTxRateRetryPolicy,
			       uint8 linkId);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_BeaconWakeUpPeriod
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function set DTIM perod and listen interval.
 * \param UmacHandle	   - Handle to the UMAC Instance.
 * \param NumBeaconPeriods - DTIM period recived from TIM information element.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_BeaconWakeUpPeriod(UMAC_HANDLE UmacHandle,
					    uint8 NumBeaconPeriods);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes Beacon/Probe response template frame to lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param FrameType		-   Beacon or probe response
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_TemplateFrame_BeaconOrProbeResp(UMAC_HANDLE UmacHandle,
							 uint8 FrameType);

/******************************************************************************
 * NAME:	WFM_UMAC_put_IE
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function inserts information element at the given position.
 * \param *P	   -    Position Pointer.
 * \param eid	   -	Element Id to be inserted.
 * \param size	   -	Size of the information element.
 * \param *d	   -	Information to be copied.
 * \returns Updated position pointer.
 *****************************************************************************/
uint8 *WFM_UMAC_put_IE(uint8 *p, uint8 eid, int size, uint8 * d);

/******************************************************************************
 * NAME:	WFM_UMAC_ReadMib_CounterTable
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function send request to LMAC, to get statistics counter table .
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_ReadMib_CounterTable(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_OperationalPwrMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs operational power mode in Lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \param OprPerMode		-   Operational power mode.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_OperationalPwrMode(UMAC_HANDLE UmacHandle,
					    uint32 OprPerMode);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_SetUAPSDInformation
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the UAPSD information in Lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_SetUAPSDInformation(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_SetRcpiRssiThreshold
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the RcpiRssiThreshold information in Lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_SetRcpiRssiThreshold(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_EtherTypeDataFrameFilter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the Ether Type Data Frame Filter in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pFilterMsg	-   Filter Message.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_EtherTypeDataFrameFilter(UMAC_HANDLE UmacHandle,
						  uint8 *pFilterMsg);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UdpPortDataFrameFilter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the UDP Port Data Frame Filter in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pFilterMsg	-   Filter Message.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UdpPortDataFrameFilter(UMAC_HANDLE UmacHandle,
						uint8 *pFilterMsg);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_MagicDataFrameFilter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the MAgic Data Frame Filter in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pFilterMsg	-   Filter Message.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_MagicDataFrameFilter(UMAC_HANDLE UmacHandle,
					      uint8 *pFilterMsg);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_ArpIpAddrFilter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the Arp IP Address Filter in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pFilterMsg	-   Filter Message.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_ArpIpAddrFilter(UMAC_HANDLE UmacHandle,
					 uint8 *pFilterMsg);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_ArpKeepAlivePeriod
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the Arp Keep Alive Period in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pFilterMsg	-   Filter Message.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_ArpKeepAlivePeriod(UMAC_HANDLE UmacHandle,
					 uint8 *pFilterMsg);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_OverrideInternalTxRate
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the MIB OverrideInternalTxRate in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param TxRate	-   TxRate for internal frames.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_OverrideInternalTxRate(UMAC_HANDLE UmacHandle);

#if MGMT_FRAME_PROTECTION
/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_ProtectedMgmtPolicy
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the Protected MGMT Policy in Lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_ProtectedMgmtPolicy(UMAC_HANDLE UmacHandle);
#endif	/* MGMT_FRAME_PROTECTION */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_UseMultiTxCnfMsg
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function enable/disable Multiple Tx Confirmation Message.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_UseMultiTxCnfMsg(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_MulticastAddrFilter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs the Multicast Address Filter in Lmac.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pFilterMsg	-   Filter Message.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_MulticastAddrFilter(UMAC_HANDLE UmacHandle,
					     uint8 *pFilterMsg);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_KeepAlivePeriod
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function cnfigure Keep alive period in WSM firmware.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_KeepAlivePeriod(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	WFM_UMAC_SendReqToWsm_SwitchChReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function send request for switch channel, as AP has moved to other
 * channel.
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param ChSwitchMode	-   Channel Switch Mode.
 * \param NewChNum	-   New Channel Number.
 * \param ChSwitchCount	-   Channel Switch Count.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_SendReqToWsm_SwitchChReq(UMAC_HANDLE UmacHandle,
					 uint8	*pSwitchChReq);

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_HostMsgTypeFilter
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function programs Host message type filter in Lmac.
 * \param UmacHandle		-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_HostMsgTypeFilter(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	WFM_UMAC_SendReqToWsm_JoinReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function send request to WSM for Joining a BSS
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \param pBssInfo	-   Pointer to the BSS to join.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_SendReqToWsm_JoinReq(UMAC_HANDLE UmacHandle,
				     WFM_BSS_INFO *pBssInfo);

#if DOT11K_SUPPORT
/******************************************************************************
 * NAME:	UMAC_SendReqToWsm_ChannelLoadReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends channel load request to WSM
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 UMAC_SendReqToWsm_ChannelLoadReq(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_SendReqToWsm_NoiseHistogramReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends Noise histogram request to WSM
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 UMAC_SendReqToWsm_NoiseHistogramReq(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_SendReqToWsm_BeaconReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends beacon request to WSM
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 UMAC_SendReqToWsm_BeaconReq(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_SendReqToWsm_StatisticsReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends statistics request to WSM
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 UMAC_SendReqToWsm_StatisticsReq(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:	UMAC_SendReqToWsm_LinkMeasReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends link measurement request to WSM
 * \param UmacHandle	-   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 UMAC_SendReqToWsm_LinkMeasReq(UMAC_HANDLE UmacHandle);

#endif	/* DOT11K_SUPPORT */

#if P2P_MINIAP_SUPPORT
/******************************************************************************
 * NAME:WFM_UMAC_MAP_LINK_REQUEST
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function sends the STA accept request to LMAC
 * \param UmacHandle    -   Handle to the UMAC Instance.
 * \param pMacAddress   -   Pointer to the MAC address of the station
 * \returns uint32.
 *****************************************************************************/
uint32 WFM_UMAC_MAP_LINK_REQUEST(UMAC_HANDLE UmacHandle, uint8 *pMacAddress);

/******************************************************************************
 * NAME:WFM_UMAC_WriteMib_FindInfo
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function enables/disables beacon filtering.
 * \param UmacHandle	    -   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_FindInfo(UMAC_HANDLE UmacHandle);

/******************************************************************************
 * NAME:WFM_UMAC_Get_Group_TSC
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function gets the Group TSC value from the LMAC for multicast/broadcast
 * frames.
 * \param UmacHandle    -   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_Get_Group_TSC(UMAC_HANDLE UmacHandle);
uint32 WFM_UMAC_WriteMib_DisableBSSIDFiltering(UMAC_HANDLE UmacHandle,
					    uint8 DisableBSSIDFilter);

/******************************************************************************
 * NAME:WFM_UMAC_WriteMib_P2P_PSMode
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes the MIB for P2P PS mode to firmware.
 * \param UmacHandle    -   Handle to the UMAC Instance.
 * \returns WFM_STATUS_CODE.
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_P2P_PSMode(UMAC_HANDLE UmacHandle);
#endif	/* P2P_MINIAP_SUPPORT */

/******************************************************************************
 * NAME:	WFM_UMAC_WriteMib_TemplateFrame_ArpReply
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function writes ARP Reply Template Frame.
 * \param UmacHandle	-   Handle to the UMAC Instance.
  * \returns WFM_STATUS_CODE
 *****************************************************************************/
uint32 WFM_UMAC_WriteMib_TemplateFrame_ArpReply(UMAC_HANDLE UmacHandle);

uint32 WFM_UMAC_SendReqToWsm_JoinReqAll(UMAC_HANDLE UmacHandle,
				     uint8 Mode,
				     uint8 Band,
				     uint16 Channel,
				     uint8 Bssid[],
				     uint16 AtimWindow,
				     uint8 PreambleType,
				     uint8 ProbeForJoin,
				     uint8 Flags,
				     uint32 SSIDLen,
				     uint8  SSID[],
				     uint32 BeaconInterval,
				     uint32 BasicRateSet
				     );
uint32 WFM_UMAC_SendReqToWsm_StartReq(UMAC_HANDLE UmacHandle,
				      uint8 Mode,
				      uint8 Band,
				      uint16 Channel,
				      uint32 CTWindow,
				      uint32 BeaconInterval,
				      uint8 DTIM,
				      uint8 PreambleType,
				      uint8 ProbeDelay,
				      uint8 SSIDLength,
				      uint8 SSID[],
				      uint32 BasicRate);

uint32 UMAC_SendReqToWsm_Reset(UMAC_HANDLE UmacHandle,
			       uint32 Flags,
			       uint8 linkId);
uint32 UMAC_ScanReqACS(UMAC_HANDLE UmacHandle, void *pMsg);
#define UMAC_MIB_ALLOCATE_BUFFER(UmacHandle)	\
	(&((WFM_UMAC_INSTANCE *)UmacHandle)->MibBuf[0])

/*#define UMAC_MIB_ALLOCATE_BUFFER(UmacHandle)	    \
    (&((WFM_UMAC_INSTANCE *)UmacHandle)->MibBuf[0]) */

#endif				/*_UMAC_MIB_H_ */
