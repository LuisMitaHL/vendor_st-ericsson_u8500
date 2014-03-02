/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    ste3a_debug.h
 * \brief   STE 3A library debug interface
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef STE3A_DEBUG_H_
#define STE3A_DEBUG_H_

#include "ste3a_types.h"
#include "ste3a_aec_debug.h"
#include "ste3a_afc_debug.h"
#include "ste3a_art_debug.h"
#include "ste3a_awb_debug.h"


/***** library instantiation for playback ************************************
*
* instantiates an instance of the STE 3A library for playing back a debug dump
* and returns an opaque handle that will identify the new instance when
* calling other functions.
*
* inputs:
*
* cbs           collection of pointers to callback functions: they cannot be
*               NULL
* dump          pointer to dumped data
*
* outputs:
*
* next          next pointer to dumped data
*
* returns:      opaque pointer to the new instance
*
*****************************************************************************/
ste3a_handle ste3a_openback(			const	ste3a_callbacks					& cbs,
										const	void *							  dump,
												void *							& next);



/***** library dumps player ***************************************************
*
* plays back library dumps
*
* inputs:
*
* handle        opaque pointer to the instance
* dump          pointer to dumped data
*
* outputs:
*
* next          next pointer to dumped data
* match         flag indicating that computed result matched dumped result
*
* returns:		flag indicating that data was correctly interpreted
*
*****************************************************************************/
ste3a_bool ste3a_playback(				const	ste3a_handle					  handle,
										const	void *							  dump,
												void *							& next,
												ste3a_bool						& match);



/***** AEC intermediate values inspection ************************************
*
* returns a copy of the active intermediate values for the AEC algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
*
* outputs:
*
* intermediates current set of intermediates values
*
*****************************************************************************/
void ste3a_aec_get_intermediates(		const	ste3a_handle					  handle,
												ste3a_aec_intermediates			& intermediates);



/***** AFC intermediate values inspection ************************************
*
* returns a copy of the active intermediate values for the AFC algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
*
* outputs:
*
* intermediates current set of intermediates values
*
*****************************************************************************/
void ste3a_afc_get_intermediates(		const	ste3a_handle					  handle,
												ste3a_afc_intermediates			& intermediates);



/***** AWB intermediate values inspection ************************************
*
* returns a copy of the active intermediate values for the AWB algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
*
* outputs:
*
* intermediates current set of intermediates values
*
*****************************************************************************/
void ste3a_awb_get_intermediates(		const	ste3a_handle					  handle,
												ste3a_awb_intermediates			& intermediates);



/***** ART intermediate values inspection ************************************
*
* returns a copy of the active intermediate values for the ART algorithm
*
* inputs:
*
* handle        opaque pointer to the instance
*
* outputs:
*
* intermediates current set of intermediates values
*
*****************************************************************************/
void ste3a_art_get_intermediates(		const	ste3a_handle					  handle,
												ste3a_art_intermediates			& tuning);


#endif /* STE3A_DEBUG_H_ */
