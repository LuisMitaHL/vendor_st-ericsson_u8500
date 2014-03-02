/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   mixer_include.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _mixer_include_h_
#define _mixer_include_h_

/*
 for bit_true (UNIX) compilation
*/
#include <stwdsp.h>

/*
  define the limits of the types depending the mode (16 bits or 24 bits)
*/
#include <limits.h>

#include "mixer.h"

/*
 the #define
*/
#include "mixer_define.h"

/*
  the structures
 */
#include "mixer_structure.h"

/*
  the functions
 */
#include "mixer_util.h"
#endif /* _mixer_include_h_ */
