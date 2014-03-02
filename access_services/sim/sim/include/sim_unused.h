/*                               -*- Mode: C -*- 
 * Copyright (C) ST-Ericsson SA 2010
 * $Id$
 * 
 * File name       : sim_unused.h
 * Description     : Definition of the UNUSED macro
 * 
 * Author          : Mats Bergstrom <mats.m.bergstrom@stericsson.com>
 * 
 */

#ifndef __sim_unused_h__
#define __sim_unused_h__ (1)

#ifdef UNUSED 
// EMPTY
#elif defined(__GNUC__) 
# define UNUSED(x) UNUSED_ ## x __attribute__((unused)) 
#elif defined(S_SPLINT_S) 
# define UNUSED(x) /*@unused@*/ x 
#else 
# define UNUSED(x) x 
#endif 

#endif
