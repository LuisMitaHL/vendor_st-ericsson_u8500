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
 * \file mod_conf.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: mod_conf.h
 * \brief
 * LMAC/UMAC module configuration file.
 * \ingroup Upper_MAC_Core
 * \date 15/07/08 7:29
 */

#ifndef MODULE_CONFIG_H
#define MODULE_CONFIG_H

/*
  Additional buffer of 16 bytes to be added for inserting WFM UMAC header
  Another 16 bytes for Storing UMAC_TX_DATA structure.
*/
#define SM_WFM_HEADER_SIZE	  (16+16)
#define ENABLE_UMAC_DEBUG	   1

#endif	/* MODULE_CONFIG_H */
