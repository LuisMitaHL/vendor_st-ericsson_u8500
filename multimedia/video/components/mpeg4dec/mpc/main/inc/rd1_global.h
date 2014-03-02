/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _RD1_GLOBAL_H_
#define _RD1_GLOBAL_H_


/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include <t1xhv_types.h>
#ifndef __T1XHV_NMF_ENV
#include <host_interface_mpeg4dec.h>
#endif //#ifndef __T1XHV_NMF_ENV

#ifdef RD1_GLOBAL
#define EXTERN 
#else
#define EXTERN extern
#endif

#undef EXTERN 
#endif /* _RD1_GLOBAL_H_ */


