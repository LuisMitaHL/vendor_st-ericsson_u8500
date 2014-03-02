/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   wrapping_macros.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef  _WRAPPING_MACROS_H_
#define  _WRAPPING_MACROS_H_
#include "cm/inc/cm_macros.h"
#include "ENS_Nmf.h"
#include "audiocodec.h"

using namespace ENS;

//! used to add printf in code to help developpers to debug
//! in final product it must not be defined.
//! verbosity depends of macro values (1 or 2)
//#define DEBUG 2

#ifdef DEBUG
    //! Verbose version. returns from caller if an OMX error has occured
    #define RETURN_IF_OMX_ERROR(expr) {                                           \
        OMX_ERRORTYPE error = expr;                                             \
        if (error != OMX_ErrorNone){                                            \
            NMF_LOG("An OMX error (0x%x) occured while trying to issue command : >" #expr "< in file %s line %d\n",error,__FILE__,__LINE__);\
            return error;                                                       \
        }                                                                       \
    }
    
    //! Verbose version. Always returns from caller
    #define RETURN_OMX_ERROR(expr) {                                           \
        OMX_ERRORTYPE error = expr;                                             \
        if (error != OMX_ErrorNone){                                            \
            NMF_LOG("An OMX error (0x%x) has been returned in file %s line %d\n",error,__FILE__,__LINE__);\
        }                                                                       \
        return error;                                                       \
    }
#else
    //! Product version. returns from caller if an OMX error has occured
    #define RETURN_IF_OMX_ERROR(expr) {  \
        OMX_ERRORTYPE error = expr;   \
        if (error != OMX_ErrorNone){  \
            return error;             \
        }                             \
    }
    //! Product version. Always returns from caller
    #define RETURN_OMX_ERROR(expr) {                                           \
       OMX_ERRORTYPE error = expr;                                             \
       return error;                                                       \
    }
#endif

#if defined(DEBUG) && (DEBUG > 1)
    //! Verbose version. Display message
  #define DEBUG_LOG(a, args...) NMF_LOG("%s:%d: " a ,__FILE__,__LINE__, args)
#else
    //! Product version. Filter message
    #define DEBUG_LOG(...)
#endif

//! mono channel index.
const unsigned int MONO_CHANNEL_INDEX    = 0;
//! Nmf fifo interface default size
const unsigned int FIFO_DEPTH_ONE        = 2;
//! BT Nmf component port index
//! \note BT is the only client of MSP0
const unsigned int BT_NMF_PORT_IDX       = 0;

//! Open Max master port index
const unsigned int OMX_MASTER_PORT_IDX   = 0;
//! Open Max master port index
//!\note that only sinks can have a reference port
const unsigned int OMX_FEEDBACK_PORT_IDX    = 1;
//!  Open Max port buffer size min for Ab8500 
const unsigned int OMX_PORT_BUFFER_SIZE_MIN_AB8500 = 1152;


//! indicates that a sink have a reference port
const OMX_BOOL HAVE_FEEDBACK_PORT        = OMX_TRUE;
//! indicates that a sink do not have a reference port
const OMX_BOOL DO_NOT_HAVE_FEEDBACK_PORT = OMX_FALSE;

//! indicates that a sink support volume config
const OMX_BOOL SUPPORT_VOLUME_CONFIG        =  OMX_TRUE;
//! indicates that a sink do not support volume config
const OMX_BOOL DO_NOT_SUPPORT_VOLUME_CONFIG =  OMX_FALSE;

#endif   // _WRAPPING_MACROS_H_
