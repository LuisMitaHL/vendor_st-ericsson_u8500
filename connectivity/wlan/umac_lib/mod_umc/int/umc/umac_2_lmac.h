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
 * \file umac_2_lmac.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_2_lmac.h
 * \brief
 * This file acts as an interface between LMAC and UMAC where ever its required.
 * \ingroup Upper_MAC_Core
 * \date 22/02/08 14:52
 */

#ifndef _UMAC_2_LMAC_H
#define _UMAC_2_LMAC_H

/******************************************************************************
			      INCLUDE FILES
******************************************************************************/
#include "stddefs.h"
#include "umac_frames.h"
#include "umac_messages.h"

/******************************************************************************
		      EDCA PARAMETER VALUES
******************************************************************************/

/******************************************
	   CWMIN & CWMAX
*******************************************/
/*
  The values for aCWmin and aCWMax given below are based on
  IEEE 802.11-2007  spec.

  Ref : Table 18-5 for DSSS phy and Table 17-15  for OFDM Phy
*/

/*Default values for aCWMin for OFDM Phy*/
#define WFM_ACWMIN_OFDM_20MHZ		15
#define WFM_ACWMIN_OFDM_10MHZ		15
#define WFM_ACWMIN_OFDM_5MHZ		15

#define WFM_ACWMAX_OFDM_20MHZ		1023
#define WFM_ACWMAX_OFDM_10MHZ		1023
#define WFM_ACWMAX_OFDM_5MHZ		1023

/*Default values for aCWMin for DSSS Phy*/

#define WFM_ACWMIN_DSSS			31
#define WFM_ACWMAX_DSSS			1023

/******************************************
		AIFSN
*******************************************/
/*
  Please refer Table 7-37 of  IEEE 802.11-2007  spec. for the following
  values.
*/
#define WFM_DEFAULT_AIFSN	2
#define WFM_AIFSN_BK		7
#define WFM_AIFSN_BE		3
#define WFM_AIFSN_VI		2
#define WFM_AIFSN_VO		2

/******************************************
	     TXOP_LIMIT
*******************************************/
/*
  1 micro second = 1/1000,000 of a second.
  1 milli second = 1/1000 of a second

  The values for Txop limit given in table 7-37 are in milli seconds
  we have to convert these values in to micro seconds.

  1 milli second = 1000 micro seconds

  6.016 Millisecond   = 6016 Microseconds
  3.264 Millisecond   = 3264 Microseconds
  3.008 Millisecond   = 3008 Microseconds
  1.504 Millisecond   = 1504 Microseconds

*/

#define  WFM_TXOPLIMIT_DSSS_PHY_AC_BK		0
#define  WFM_TXOPLIMIT_DSSS_PHY_AC_BE		0
#define  WFM_TXOPLIMIT_DSSS_PHY_AC_VI		6016
#define  WFM_TXOPLIMIT_DSSS_PHY_AC_VO		3264

#define  WFM_TXOPLIMIT_OFDM_PHY_AC_BK		0
#define  WFM_TXOPLIMIT_OFDM_PHY_AC_BE		0
#define  WFM_TXOPLIMIT_OFDM_PHY_AC_VI		3008
#define  WFM_TXOPLIMIT_OFDM_PHY_AC_VO		1504

/******************************************************************************
		      END -- EDCA PARAMETER VALUES
******************************************************************************/

/******************************************************************************
 * NAME:	wfm_umac_set_edca_parameters
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function extracts information from EDCA PARAMETER structure and sets
 * the EDCA Parameters to LMAC.
 * \param UmacHandle   - Handle to the UMAC Instance.
 * \param *pEdcaParams - Pointer to WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED
 *                       structure.
 * \returns status WFM_STATUS_SUCCESS if successful.
 *****************************************************************************/
uint32 wfm_umac_set_edca_parameters(UMAC_HANDLE UmacHandle,
				    WFM_INFO_ELEMENT_EDCA_PARAMETER_PACKED *pEdcaParams);

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
 * \param Band		  - Band for this association
 * \returns status WFM_STATUS_SUCCESS if successful.
 *****************************************************************************/
uint32 wfm_umac_set_default_edca_parameters(UMAC_HANDLE UmacHandle,
					    uint16 ChannelNumber,
					    uint32 OpRateSet,
					    uint16 Band);

/******************************************************************************
 * NAME:	wfm_umac_set_default_ac_parameters
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function derives default Access Catagory parameters to be used for a
 * non WMM case, based on band information.
 * \param UmacHandle      - Handle to the UMAC Instance.
 * \param OpRateSet       - Operational rate set for this association.
 * \param Band		  - Band for this association
 * \returns status WFM_STATUS_SUCCESS if successful.
 *****************************************************************************/
uint32 wfm_umac_set_default_ac_parameters(UMAC_HANDLE UmacHandle,
					  uint32 OpRateSet,
					  uint16 Band);

#endif	/* _UMAC_2_LMAC_H */
