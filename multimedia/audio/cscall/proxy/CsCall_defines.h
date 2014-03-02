/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   CsCall_defines.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _CSCALL_DEFINES_H_
#define _CSCALL_DEFINES_H_

#define CSCALL_OUTPUT_PORT_INDEX 0
#define CSCALL_INPUT_PORT_INDEX  1
#define CSCALL_CONTROL_PORT_INDEX  2

#ifdef OMX_TRACE_UID
#undef OMX_TRACE_UID
#endif

#define OMX_TRACE_UID (1<<7)

#ifdef OMXCOMPONENT
#undef OMXCOMPONENT
#endif

#define OMXCOMPONENT "CSCALL_CPT"

#endif //_CSCALL_DEFINES_H_
