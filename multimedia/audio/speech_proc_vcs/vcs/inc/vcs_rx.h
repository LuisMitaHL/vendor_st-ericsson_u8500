#ifndef _VCS_RX_H_
#define _VCS_RX_H_
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_rx.h
 * \brief    Header file for creating voice shell downlink (RX)
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs.h"

#ifdef __cplusplus
extern "C"
{
#endif

  /**
	 Creates and instantiate a voice shell for RX (downlink
	 processing)
	 @param vcs_calloc Memory allocation method
	 @param vcs_free Memory free method
	 @return returns a context for rx processing or 0 on failure
  */
  vcs_ctx_t* vcs_rx_create(vcs_log_t *log);

  
#ifdef __cplusplus
}
#endif

#endif // _VCS_RX_H_

