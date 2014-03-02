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
 * \file sysconfig.h
 * - <b>PROJECT</b>	: WLAN HOST UMAC
 * - <b>FILE</b>	: sysconfig.h
 * \brief
 * MAC Project configuration.
 * \ingroup Upper_MAC_Core
 * \date 10/10/06 7:29
 */

#ifndef SYSCONFIG_H
#define SYSCONFIG_H

#include "mod_conf.h"


/******************************************************************************
* General Section
******************************************************************************/

#define CONVERT_BYTE_SIZE_TO_WORD_SIZE(x)   (((x)+3)/4)

#define SYS_HOST_TX_LIMIT	  128

/*
   Maximum number of Transmit buffers (requests) supported
   by the Host interface
*/
#define CFG_HI_NUM_REQS	     SYS_HOST_TX_LIMIT	/* must be power of 2 */

#endif	/* SYSCONFIG_H */
