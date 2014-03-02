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
 * \file umac_2_lmac.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: umac_2_lmac.c
 * \brief
 * This file acts as an interface between LMAC and UMAC where ever its required.
 * \ingroup Upper_MAC_Core
 * \date 04/07/08 10:16
 */

/******************************************************************************
	      INCLUDE FILES
******************************************************************************/
#include "os_if.h"
#include "section_wfm.h"

#include "umac_dev_conf.h"
#include "umac_sm.h"
#include "umac_2_lmac.h"
#include "umac_ll_if.h"
#include "umac_data_handler.h"
#include "umac_dbg.h"

#include "umac_ll_if.h"
#include "umac_if.h"

#define WFM_CALCULATE_CWMIN(ac) ((1 << (pEdcaParams->ac.EcWminEcWmax & 0x0f)) - 1)

#define WFM_CALCULATE_CWMAX(ac) ((1 << ((pEdcaParams->ac.EcWminEcWmax & 0xf0) >> 4)) - 1)

/******************************************************************************
 * NAME:	wfm_umac_set_edca_parameters
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function extracts information from
 * WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED and sets  the EDCA Parameters to LMAC.
 * \param UmacHandle   - Handle to the UMAC Instance.
 * \param *pEdcaParams - Pointer to WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED
 *			 structure.
 * \returns status WFM_STATUS_SUCCESS if successful.
 *****************************************************************************/
uint32 wfm_umac_set_edca_parameters(UMAC_HANDLE UmacHandle,
				    WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED *pEdcaParams)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	EDCA_PARAMS EdcaParams = { {0}
	};
	EDCA_PARAMS *pEdcaParamReq = &EdcaParams;
	uint32 status = WFM_STATUS_SUCCESS;

	OS_ASSERT(pUmacInstance);

	pEdcaParamReq->CwMin[WFM_AC_BK] = WFM_CALCULATE_CWMIN(Ac_BackGround);
	pEdcaParamReq->CwMax[WFM_AC_BK] = WFM_CALCULATE_CWMAX(Ac_BackGround);
	pEdcaParamReq->Aifsn[WFM_AC_BK] = (pEdcaParams->Ac_BackGround.AciAifn & 0x0f);
	pEdcaParamReq->TxOpLimit[WFM_AC_BK] = pEdcaParams->Ac_BackGround.TxopLimit * TXOP_UNIT;

	pEdcaParamReq->CwMin[WFM_AC_BE] = WFM_CALCULATE_CWMIN(Ac_BestEffort);
	pEdcaParamReq->CwMax[WFM_AC_BE] = WFM_CALCULATE_CWMAX(Ac_BestEffort);
	pEdcaParamReq->Aifsn[WFM_AC_BE] = (pEdcaParams->Ac_BestEffort.AciAifn & 0x0f);
	pEdcaParamReq->TxOpLimit[WFM_AC_BE] = pEdcaParams->Ac_BestEffort.TxopLimit * TXOP_UNIT;

	pEdcaParamReq->CwMin[WFM_AC_VI] = WFM_CALCULATE_CWMIN(Ac_Video);
	pEdcaParamReq->CwMax[WFM_AC_VI] = WFM_CALCULATE_CWMAX(Ac_Video);
	pEdcaParamReq->Aifsn[WFM_AC_VI] = (pEdcaParams->Ac_Video.AciAifn & 0x0f);
	pEdcaParamReq->TxOpLimit[WFM_AC_VI] = pEdcaParams->Ac_Video.TxopLimit * TXOP_UNIT;

	pEdcaParamReq->CwMin[WFM_AC_VO] = WFM_CALCULATE_CWMIN(Ac_Voice);
	pEdcaParamReq->CwMax[WFM_AC_VO] = WFM_CALCULATE_CWMAX(Ac_Voice);
	pEdcaParamReq->Aifsn[WFM_AC_VO] = (pEdcaParams->Ac_Voice.AciAifn & 0x0f);
	pEdcaParamReq->TxOpLimit[WFM_AC_VO] = pEdcaParams->Ac_Voice.TxopLimit * TXOP_UNIT;

	/*
	   b0 - b3 of QoSInfo field is EDCA Parameter set update count. Not
	   setting it to zero initially as the value AP using at this moment
	   can be non-zero
	 */
	pUmacInstance->EdcaParamSetUpdateCount = pEdcaParams->QoSInfo & 0x0F;

	/*Setting the value EdcaParams.MaxReceiveLifetime */

	pEdcaParamReq->MaxReceiveLifetime[WFM_AC_BK] = WFM_MAX_RX_LIFE_TIME;
	pEdcaParamReq->MaxReceiveLifetime[WFM_AC_BE] = WFM_MAX_RX_LIFE_TIME;
	pEdcaParamReq->MaxReceiveLifetime[WFM_AC_VI] = WFM_MAX_RX_LIFE_TIME;
	pEdcaParamReq->MaxReceiveLifetime[WFM_AC_VO] = WFM_MAX_RX_LIFE_TIME;

	UMAC_LL_REQ_TX_EDCA_PARAMS(UmacHandle, pEdcaParamReq);

	return status;
} /*end wfm_umac_set_edca_parameters() */

/******************************************************************************
 * NAME:	wfm_umac_set_default_edca_parameters
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function derives default EDCA parameters to be used for a non QOS/Non
 * WMM association, based on band information.
 * \param UmacHandle      - Handle to the UMAC Instance.
 * \param ChannelNumber   - Channel Number on which STA is associated with AP.
 * \param OpRateSet       - Operational rate set for this association.
 * \param Band	    - Band for this association
 * \returns status WFM_STATUS_SUCCESS if successful.
 *****************************************************************************/
uint32 wfm_umac_set_default_edca_parameters(UMAC_HANDLE UmacHandle,
					    uint16 ChannelNumber,
					    uint32 OpRateSet,
					    uint16 Band)
{
	uint16 aCWMin = 0;
	uint16 aCWMax = 0;
	EDCA_PARAMS EdcaParams = { {0} };
	EDCA_PARAMS *pEdcaParamReq = &EdcaParams;
	uint32 status = WFM_STATUS_SUCCESS;

	if (OpRateSet & WFM_ERP_OFDM_RATES_SUPPORTED) {
		/*Currently we support only 20MHz Channels */
		aCWMin = WFM_ACWMIN_OFDM_20MHZ;
		aCWMax = WFM_ACWMAX_OFDM_20MHZ;

		pEdcaParamReq->TxOpLimit[WFM_AC_BK] = WFM_TXOPLIMIT_OFDM_PHY_AC_BK;
		pEdcaParamReq->TxOpLimit[WFM_AC_BE] = WFM_TXOPLIMIT_OFDM_PHY_AC_BE;
		pEdcaParamReq->TxOpLimit[WFM_AC_VI] = WFM_TXOPLIMIT_OFDM_PHY_AC_VI;
		pEdcaParamReq->TxOpLimit[WFM_AC_VO] = WFM_TXOPLIMIT_OFDM_PHY_AC_VO;

	} else if (OpRateSet & WFM_ERP_DSSS_CCK_RATES_SUPPORTED) {
		/*Its DSSS Phy Mode */
		aCWMin = WFM_ACWMIN_DSSS;
		aCWMax = WFM_ACWMAX_DSSS;

		pEdcaParamReq->TxOpLimit[WFM_AC_BK] = WFM_TXOPLIMIT_DSSS_PHY_AC_BK;
		pEdcaParamReq->TxOpLimit[WFM_AC_BE] = WFM_TXOPLIMIT_DSSS_PHY_AC_BE;
		pEdcaParamReq->TxOpLimit[WFM_AC_VI] = WFM_TXOPLIMIT_DSSS_PHY_AC_VI;
		pEdcaParamReq->TxOpLimit[WFM_AC_VO] = WFM_TXOPLIMIT_DSSS_PHY_AC_VO;

	} /*if( OpRateSet & WFM_ERP_DSSS_CCK_RATES_SUPPORTED ) */

	pEdcaParamReq->CwMin[WFM_AC_BK] = aCWMin;
	pEdcaParamReq->CwMin[WFM_AC_BE] = aCWMin;
	pEdcaParamReq->CwMin[WFM_AC_VI] = (((aCWMin + 1) / 2) - 1);
	pEdcaParamReq->CwMin[WFM_AC_VO] = (((aCWMin + 1) / 4) - 1);

	pEdcaParamReq->CwMax[WFM_AC_BK] = aCWMax;
	pEdcaParamReq->CwMax[WFM_AC_BE] = aCWMax;
	/*
	   As per Spec, the following two values to be derived from aCWMin
	 */
	pEdcaParamReq->CwMax[WFM_AC_VI] = aCWMin;
	pEdcaParamReq->CwMax[WFM_AC_VO] = (((aCWMin + 1) / 2) - 1);

	pEdcaParamReq->Aifsn[WFM_AC_BK] = WFM_AIFSN_BK;
	pEdcaParamReq->Aifsn[WFM_AC_BE] = WFM_AIFSN_BE;
	pEdcaParamReq->Aifsn[WFM_AC_VI] = WFM_AIFSN_VI;
	pEdcaParamReq->Aifsn[WFM_AC_VO] = WFM_AIFSN_VO;

	/*Setting the value EdcaParams.MaxReceiveLifetime */

	pEdcaParamReq->MaxReceiveLifetime[WFM_AC_BK] = WFM_MAX_RX_LIFE_TIME;
	pEdcaParamReq->MaxReceiveLifetime[WFM_AC_BE] = WFM_MAX_RX_LIFE_TIME;
	pEdcaParamReq->MaxReceiveLifetime[WFM_AC_VI] = WFM_MAX_RX_LIFE_TIME;
	pEdcaParamReq->MaxReceiveLifetime[WFM_AC_VO] = WFM_MAX_RX_LIFE_TIME;

	UMAC_LL_REQ_TX_EDCA_PARAMS(UmacHandle, pEdcaParamReq);

	return status;

} /*end wfm_umac_set_default_edca_parameters() */

/******************************************************************************
 * NAME:	wfm_umac_set_default_ac_parameters
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function derives default Access Catagory parameters to be used for a non
 * WMM case, based on band information.
 * \param UmacHandle      - Handle to the UMAC Instance.
 * \param OpRateSet       - Operational rate set for this association.
 * \param Band		  - Band for this association
 * \returns status WFM_STATUS_SUCCESS if successful.
 *****************************************************************************/
uint32 wfm_umac_set_default_ac_parameters(UMAC_HANDLE UmacHandle,
					  uint32 OpRateSet,
					  uint16 Band)
{
	WFM_UMAC_INSTANCE *pUmacInstance = (WFM_UMAC_INSTANCE *) UmacHandle;
	uint16 aCWMin = 0;
	uint16 aCWMax = 0;
	EDCA_PARAMS EdcaParams = { {0} };
	EDCA_PARAMS *pEdcaParamReq = &EdcaParams;
	uint32 status = WFM_STATUS_SUCCESS;

	if (OpRateSet & WFM_ERP_OFDM_RATES_SUPPORTED) {
		/*Currently we support only 20MHz Channels */
		aCWMin = WFM_ACWMIN_OFDM_20MHZ;
		aCWMax = WFM_ACWMAX_OFDM_20MHZ;

		pEdcaParamReq->TxOpLimit[WFM_AC_BK] = pUmacInstance->gVars.p.txopLimit;
		pEdcaParamReq->TxOpLimit[WFM_AC_BE] = pUmacInstance->gVars.p.txopLimit;
		pEdcaParamReq->TxOpLimit[WFM_AC_VI] = pUmacInstance->gVars.p.txopLimit;
		pEdcaParamReq->TxOpLimit[WFM_AC_VO] = pUmacInstance->gVars.p.txopLimit;
	} else if (OpRateSet & WFM_ERP_DSSS_CCK_RATES_SUPPORTED) {
		/*Its DSSS Phy Mode */
		aCWMin = WFM_ACWMIN_DSSS;
		aCWMax = WFM_ACWMAX_DSSS;

		pEdcaParamReq->TxOpLimit[WFM_AC_BK] = pUmacInstance->gVars.p.txopLimit;
		pEdcaParamReq->TxOpLimit[WFM_AC_BE] = pUmacInstance->gVars.p.txopLimit;
		pEdcaParamReq->TxOpLimit[WFM_AC_VI] = pUmacInstance->gVars.p.txopLimit;
		pEdcaParamReq->TxOpLimit[WFM_AC_VO] = pUmacInstance->gVars.p.txopLimit;
	} /*if( OpRateSet & WFM_ERP_DSSS_CCK_RATES_SUPPORTED ) */

	pEdcaParamReq->CwMin[WFM_AC_BK] = aCWMin;
	pEdcaParamReq->CwMin[WFM_AC_BE] = aCWMin;
	pEdcaParamReq->CwMin[WFM_AC_VI] = (((aCWMin + 1) / 2) - 1);
	pEdcaParamReq->CwMin[WFM_AC_VO] = (((aCWMin + 1) / 4) - 1);

	pEdcaParamReq->CwMax[WFM_AC_BK] = aCWMax;
	pEdcaParamReq->CwMax[WFM_AC_BE] = aCWMax;
	/*
	   As per Spec, the following two values to be derived from aCWMin
	 */
	pEdcaParamReq->CwMax[WFM_AC_VI] = aCWMin;
	pEdcaParamReq->CwMax[WFM_AC_VO] = (((aCWMin + 1) / 2) - 1);

	pEdcaParamReq->Aifsn[WFM_AC_BK] = WFM_DEFAULT_AIFSN;
	pEdcaParamReq->Aifsn[WFM_AC_BE] = WFM_DEFAULT_AIFSN;
	pEdcaParamReq->Aifsn[WFM_AC_VI] = WFM_DEFAULT_AIFSN;
	pEdcaParamReq->Aifsn[WFM_AC_VO] = WFM_DEFAULT_AIFSN;

	/*Setting the value EdcaParams.MaxReceiveLifetime */

	pEdcaParamReq->MaxReceiveLifetime[WFM_AC_BK] = WFM_MAX_RX_LIFE_TIME;
	pEdcaParamReq->MaxReceiveLifetime[WFM_AC_BE] = WFM_MAX_RX_LIFE_TIME;
	pEdcaParamReq->MaxReceiveLifetime[WFM_AC_VI] = WFM_MAX_RX_LIFE_TIME;
	pEdcaParamReq->MaxReceiveLifetime[WFM_AC_VO] = WFM_MAX_RX_LIFE_TIME;

	UMAC_LL_REQ_TX_EDCA_PARAMS(UmacHandle, pEdcaParamReq);

	return status;

} /*end wfm_umac_set_default_ac_parameters() */
