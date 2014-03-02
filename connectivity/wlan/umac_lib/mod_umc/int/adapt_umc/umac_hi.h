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
 * \file umac_hi.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_hi.h
 * \brief
 * Implementation of WFM UMAC HI module. This file implements
*  the interface between HI module and UMAC module.
 * \ingroup Upper_MAC_Core
 * \date 13/02/08 14:05
 */

#ifndef _UMAC_HI_H
#define _UMAC_HI_H

/******************************************************************************
	      INCLUDE FILES
******************************************************************************/

#include "umac_messages.h"
#include "umac_if.h"

/***********************************************************************
			       Public Functions
***********************************************************************/

/* Function Prototype for WFM HI Message handlers */
typedef uint16(*WFM_UMAC_HI_MSG_HANDLER)
	(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE * hi_msg);

/******************************************************************************
 * NAME:	WFM_HI_Proc_MemoryReadReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the Memory Read Request.
 * \param UmacHandle - Handle to the UMAC Instance.
 * \param *hi_msg    - Pointer to Hi Message.
 * \returns uint16     Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_MemoryReadReq(UMAC_HANDLE UmacHandle,
				 WFM_HI_MESSAGE *hi_msg);

/******************************************************************************
 * NAME:	WFM_HI_Proc_MemoryWriteReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the Memory Write Request.
 * \param UmacHandle - Handle to the UMAC Instance.
 * \param *hi_msg    - Pointer to Hi Message.
 * \returns uint16     Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_MemoryWriteReq(UMAC_HANDLE UmacHandle,
				  WFM_HI_MESSAGE *hi_msg);

/******************************************************************************
 * NAME:	WFM_HI_Proc_ConfigReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the Configuration Request.
 * \param UmacHandle - Handle to the UMAC Instance.
 * \param *hi_msg    - Pointer to Hi Message.
 * \returns uint16     Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_ConfigReq(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *hi_msg);

/******************************************************************************
 * NAME:	WFM_HI_Proc_GenericReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the Generic Request.
 * \param UmacHandle  - Handle to the UMAC Instance.
 * \param  *hi_msg    - Pointer to Hi Message.
 * \returns uint16	Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_GenericReq(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *hi_msg);

/******************************************************************************
 * NAME:	WFM_HI_Proc_TransmitReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the transmit request.
 * \param UmacHandle - Handle to the UMAC Instance.
 * \param *pTxReq    - Tx Request from upper layer.
 * \returns uint16 Appropriate WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_TransmitReq(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *pTxReq);

/******************************************************************************
 * NAME:	WFM_HI_Proc_GetParamReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the Getparameter request.
 * \param UmacHandle   - Handle to the UMAC Instance.
 * \param *hi_msg      - Message from host
 * \returns uint16       Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_GetParamReq(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *hi_msg);

/******************************************************************************
 * NAME:	WFM_HI_Proc_SetParamReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the SetParameter request.
 * \param UmacHandle   - Handle to the UMAC Instance.
 * \param *hi_msg      - Message from host
 * \returns uint16       Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_SetParamReq(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *hi_msg);

/******************************************************************************
 * NAME:	WFM_HI_DefaultMsgHandler
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the default request.
 * \param UmacHandle   - Handle to the UMAC Instance.
 * \param *hi_msg      - Message from host
 * \returns uint16       Appropriate  WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_DefaultMsgHandler(UMAC_HANDLE UmacHandle, WFM_HI_MESSAGE *hi_msg);

/******************************************************************************
 * NAME:	WFM_HI_Proc_TransmitMgmtReq
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function handles the transmit Managment request.
 * \param UmacHandle  - Handle to the UMAC Instance.
 * \param *pMgmtTxReq - Tx Managment Request from upper layer.
 * \returns uint16 Appropriate WFM_STATUS_CODE.
 *****************************************************************************/
uint16 WFM_HI_Proc_TransmitMgmtReq(UMAC_HANDLE UmacHandle,
				   UMAC_MGMT_TRANSMIT_REQ *pMgmtTxReq);

#endif	/* _UMAC_HI_H */
