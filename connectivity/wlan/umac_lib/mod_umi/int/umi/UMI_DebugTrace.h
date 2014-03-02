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
 * \file UMI_DebugTrace.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_DebugTrace.h
 * \brief
 * Header for UMI Debug module.
 * \ingroup Upper_MAC_Interface
 * \date 23/01/09 10:46
 */

#ifndef _UMI_DEBUG_TRACE_H
#define _UMI_DEBUG_TRACE_H

#include "UMI_OsIf.h"

typedef enum UMI_MODULE_E {
	DBG_UMI_Api = 0,
	DBG_UMI_ProcUmacMsg,
	DBG_UMI_Queue,
	DBG_UMI_ScanList,
	DBG_UMI_Utility,
	DBG_UMI_UmacDil,
	DBG_UMAC,
	DBG_MAX
} UMI_MODULE;

/************************************************************************
 * DEBUGGING CONSTANTS
 ************************************************************************/

/************************************************************************
 *  DBG_NONE
 *  DBG_ALL
 *  DBG_ALWAYS
 *  DBG_DEFAULT
 *  DBG_TRACE
 *  DBG_VERBOSE
 ************************************************************************/

#define DBG_NONE	 0
#define DBG_ALL	  0xFFFFFFFF

#define DBG_ERROR	 (1<<0)
#define DBG_WARNING       (1<<1)
#define DBG_ALWAYS	(1<<2)
#define DBG_DEFAULT       (1<<3)
#define DBG_TRACE	 (1<<4)
#define DBG_VERBOSE       (1<<5)

/************************************************************************
 * UMI DEBUG HELPERS
 ************************************************************************/

#define UMI_ERROR(module,   _fmt) DBG_MODULE(DBG_##module,  DBG_ERROR,   _fmt)
#define UMI_WARNING(module, _fmt) DBG_MODULE(DBG_##module,  DBG_WARNING, _fmt)
#define UMI_ALWAYS(module,  _fmt) DBG_MODULE(DBG_##module,  DBG_ALWAYS,  _fmt)
#define UMI_DEFAULT(module, _fmt) DBG_MODULE(DBG_##module,  DBG_DEFAULT, _fmt)
#define UMI_TRACE(module,   _fmt) DBG_MODULE(DBG_##module,  DBG_TRACE,   _fmt)
#define UMI_VERBOSE(module, _fmt) DBG_MODULE(DBG_##module,  DBG_VERBOSE, _fmt)

/************************************************************************
 * UMI DEBUG PRINT
 ************************************************************************/
#ifdef UMI_ENABLE_DBG_LOG
#define DBG_MODULE(module, level, fmt) {		\
		if (UMI_DbgModule(module, level)) {	\
			UMI_DbgPrint fmt ;		\
		}					\
	}
#else	/*UMI_ENABLE_DBG_LOG */
#define DBG_MODULE(module, level, fmt)
#endif	/*UMI_ENABLE_DBG_LOG */

/******************************************************************************
 * NAME:	UMI_DebugInit
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Initializes the debug module. Debug prints of a particular module of UMAC
 * can be enabled through this function.
 * \returns none.
 *****************************************************************************/
void UMI_DebugInit(void);

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
uint8 UMI_DbgModule(uint32 module, uint32 level);
#endif	/*UMI_ENABLE_DBG_LOG */

#endif	/* _UMI_DEBUG_TRACE_H */
