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
 * \file umac_dbg.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: umac_dbg.h
 * \brief
 * This file implements UMAC Debug functionalities.
 * \ingroup Upper_MAC_Core
 * \date 31/07/08 11:34
 */

#ifndef _UMAC_DBG_H
#define _UMAC_DBG_H

/******************************************************************************
				INCLUDE FILES
******************************************************************************/
#include "os_if.h"

/******************************************************************************
			      DEBUG MACROS
******************************************************************************/
#define DBG_ENABLE_UMC_LOG_EXT	  0

/******************************************************************************
			     TYPE DEFINITIONS
******************************************************************************/
#define UMAC_MEM_STAMP_INTERNAL_BUFFER(x, y)
#define UMAC_MEM_STAMP_PRINT(x)
#define UMAC_MEM_REMOVE_STAMP(x)


/******************************************************************************
			      MACRO DEFENITIONS
******************************************************************************/
#define WFM_UMAC_DBG_STORE_TX_DESC(x)
#define WFM_UMAC_DBG_ASSERT_IF_OUTSTANDING_TX_DESC(pUmacInstance)
#define WFM_UMAC_DBG_REMOVE_TX_DESC(x)
#define WFM_UMAC_DBG_PRINT_TX_DESC()

#define WFM_UMAC_DBG_STORE_RX_DESC(x)
#define WFM_UMAC_DBG_REMOVE_RX_DESC(x)

#define DBG_UMAC(x)    (x)

/* System debug masks - for UMAC */
#define DBG_WFM_TEMP			(14)
#define DBG_WFM_ENCRYPTION		DBG_UMAC(1<<15)
#define DBG_WFM_DEVLIST			DBG_UMAC(1<<16)
#define DBG_WFM_ALWAYS			DBG_UMAC(1<<17)
#define DBG_WFM_ERROR			DBG_UMAC(1<<18)
#define DBG_WFM_STATE_MC		DBG_UMAC(1<<19)
#define DBG_WFM_RX			DBG_UMAC(1<<20)
#define DBG_WFM_TX			DBG_UMAC(1<<21)
#define DBG_WFM_SCAN			DBG_UMAC(1<<22)
#define DBG_WFM_ASSOC			DBG_UMAC(1<<23)
#define DBG_WFM_IN_OUT_MSGS		DBG_UMAC(1<<24)
#define DBG_WFM_RATE_ADAPTATION		DBG_UMAC(1<<25)
#define DBG_WFM_OID			DBG_UMAC(1<<26)
#define DBG_WFM_HI			DBG_UMAC(1<<27)
#define DBG_WFM_UMAC			DBG_UMAC(1<<28)
#define DBG_WFM_FRAMES			DBG_UMAC(1<<29)
#define DBG_WFM_WATCH_DOG		DBG_UMAC(1<<30)
#define DBG_WFM_CURRENT_AK		DBG_UMAC(1<<DBG_WFM_TEMP)
#define DBG_WFM_CURRENT_GM		DBG_UMAC(1<<(DBG_WFM_TEMP-1))

#define SYS_DBG_MASK (DBG_WFM_ALWAYS | DBG_WFM_ERROR|DBG_WFM_WATCH_DOG)

#define LOG_BYTES(_m, _id, _p, _l, _dl)
#define LOG_MACADDR(_m, _id, _p)
#define LOG_STRING(_m, _id, _p, _l, _dl)

#endif	/* _UMAC_DBG_H */
