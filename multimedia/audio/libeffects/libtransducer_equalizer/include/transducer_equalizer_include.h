/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   transducer_equalizer_include.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
/*********************************************************************
 *
 *  transducer_equalizerinclude.h
 *
 *  This file regroups all the .h file needed for transducer_equalizer
 *
 *
 *  created : 09/10/2007
 *
 *********************************************************************/
#ifndef _transducer_equalizer_include_h_
#define _transducer_equalizer_include_h_

/*
 for bit_true (UNIX) compilation
*/
#include "btrue.h"
#include <stwdsp.h>

#ifdef _NMF_MPC_
#include "libeffects/libtransducer_equalizer.nmf"
#endif
#include "audiolibs/common/include/aep.h"
#include "transducer_equalizer.h"

#endif //_transducer_equalizer_include_h_

