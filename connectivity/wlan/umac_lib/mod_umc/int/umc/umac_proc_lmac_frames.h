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
 * \file umac_proc_lmac_frames.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_proc_lmac_frames.h
 * \brief
 * This module processes frames from lmac.
 * \ingroup Upper_MAC_Core
 * \date 05/00/08 17:19
 */

#ifndef _UMAC_PROC_LMAC_FRAMES_H
#define _UMAC_PROC_LMAC_FRAMES_H

/******************************************************************************
	      INCLUDE FILES
******************************************************************************/
#include "umac_if.h"
#include "umac_ll_if.h"

/*
  Handler type for frames from LMAC.There will be a handler for Data, Mgmt
  and Control frames
*/
typedef void (*UMAC_FRAME_HANDLER)
	(UMAC_HANDLE UmacHandle, uint8 SubType, UMAC_RX_DESC *pRxDescriptor);

#endif	/* _UMAC_PROC_LMAC_FRAMES_H */
