/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   volctrl_include.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _volctrl_include_h_
#define _volctrl_include_h_


#include "audiolibs_common.h"
/*
 for bit_true (not MMDSP) compilation
*/
#include <stwdsp.h>

/*
  define the limits of the types depending the mode (16 bits or 24 bits)
*/
#include <limits.h>

#include "volctrl.h"

/*
  for dB <-> linear conversion 
 */
#ifdef MMDSP
#include "db_conv.h"
#endif
/*
 the #define
*/
#include "volctrl_define.h"

#include "volctrl_util.h"
#endif //_volctrl_include_h_
