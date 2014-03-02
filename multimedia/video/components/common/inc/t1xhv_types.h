/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*******************************************************************************
 * -- coming from :
 * -- Luca Celetto
 * -- Advanced System Technologies lab.
 * -- from ANSI C Architectural Model release 1.2 (june 2001)
 * -- HW/SW partioning modified for HAMAC project
 * -- code optimized for MMDSP
 *------------------------------------------------------------------------
 * global types for mmtc
 *------------------------------------------------------------------------
 * Created by Philippe Rochette on Thu Sep 20 13:48:44 2001
 * $Log$
 * (2/12/2003) Revision /main/dev_scheduler_upgrade_1/1 by rochettp :
 *   removed TRUE/FALSE 
 * 
 * (10/10/2002) Revision /main/dev_rochettp/2 by rochettp :
 *   Updated structure and types
 * 
 ******************************************************************************/

#ifndef _MMTC_TYPES_H_
#define _MMTC_TYPES_H_

#ifdef __T1XHV_NMF_ENV
/* In case NMF is used, all those definition are implicitely done.  */
# include <inc/type.h>

typedef  unsigned long       t_address;

#else

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/
/* These type definitions are compiler and platform dependent */
/* (from AST) */
typedef  unsigned char       t_uint8; /* 8-bit */
typedef    signed char       t_sint8;

typedef  unsigned short      t_uint16; /* 16-bit container on 32-bits architectures */
typedef    signed short      t_sint16;

typedef  unsigned long       t_uint32; /* 32-bit container on 32-bits architectures */
typedef    signed long       t_sint32;

typedef  unsigned long       t_address;

#endif /* __T1XHV_NMF_ENV */

#endif /* _MMTC_TYPES_H_ */


