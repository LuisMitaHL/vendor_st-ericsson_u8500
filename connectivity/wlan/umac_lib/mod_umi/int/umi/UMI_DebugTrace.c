/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
 * \addtogroup Upper_MAC_Interface
 * \brief
 *
 */
/**
 * \file UMI_DebugTrace.c
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_DebugTrace.c
 * \brief
 * Implementation of WUM Debug module. This file implements the debug interface.
 * \ingroup Upper_MAC_Interface
 * \date 23/01/09 10:06
 */

/*****************************************************************************/
/********		       Include Files			      ********/
/*****************************************************************************/
#include "UMI_DebugTrace.h"

/*****************************************************************************/
/********		  Internally Visible Static Data	      ********/
/*****************************************************************************/
uint32 aDebugMsg[DBG_MAX];	/* Debug Settings */

/*****************************************************************************/
/********		   Internally Visible Functions		      ********/
/*****************************************************************************/

/******************************************************************************
 * NAME:	UMI_DebugInit
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Initializes the debug module. Debug prints of a particular module of UMAC
 * can be enabled through this function.
 * \returns none.
 *****************************************************************************/
void UMI_DebugInit()
{
	uint32 i = 0;

	for (i = 0; i < DBG_MAX; i++)
		aDebugMsg[i] = DBG_ALWAYS;
	/* aDebugMsg[DBG_UMI_Api] = aDebugMsg[DBG_UMI_Api] | DBG_ALWAYS; */
}

#ifdef UMI_ENABLE_DBG_LOG
/******************************************************************************
 * NAME:	UMI_DbgModule
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Verify settings of the debug level.
 * \param module - module name.
 * \param level  - Debug level.
 * \returns Status - 1 if TRUE, 0 if FLASE.
 *****************************************************************************/
uint8 UMI_DbgModule(uint32 module, uint32 level)
{
	uint8 status = FALSE;

	if ((level & DBG_ALL) <= (aDebugMsg[module] & DBG_ALL))
		status = TRUE;
	return status;
}
#endif	/*UMI_ENABLE_DBG_LOG */
