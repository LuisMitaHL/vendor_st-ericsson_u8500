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
 * \file section_wfm.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: section_wfm.h
 * \brief
 * Define sections in a C file for UMAC sub-system.
 * \ingroup Upper_MAC_Core
 * \date 22/10/08 10:16
 */

/* Mark cached code and data sections */
#ifdef USE_SECTIONS

#if __arm
#pragma arm section code = "WFM", rwdata = "WFM_RW", rodata = "WFM_CONST", zidata = "WFM_ZI"
#endif				/*__arm */

#endif				/*USE_SECTIONS */
