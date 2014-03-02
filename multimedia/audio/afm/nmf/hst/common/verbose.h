/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   verbose.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef VERBOSE_H_
#define VERBOSE_H_

#ifndef NVERBOSE
#define VERBOSE(a) printf a
#else 
#define VERBOSE(a)
#endif

#endif // VERBOSE_H_
