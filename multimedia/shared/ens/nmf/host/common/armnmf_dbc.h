/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   armnmf_dbc.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef ARMNMF_DBC_H_
#define ARMNMF_DBC_H_

#include "ENS_Redefine_Class.h"

#ifdef NDBC

#define ARMNMF_DBC_ASSERT(a)           
#define ARMNMF_DBC_PRECONDITION(a)     
#define ARMNMF_DBC_POSTCONDITION(a)    
#define ARMNMF_DBC_INVARIANT(a)  
#define ARMNMF_DBC_ASSERT_MSG(a, msg)      

#else
#include "inc/type.h"

#define ARMNMF_DBC_ASSERT_MSG(a, msg) { if (!(a)) { NMF_PANIC("%s - %s:%d\n", msg, __FILE__, __LINE__) ; }}

#define ARMNMF_DBC_ASSERT(a)           ARMNMF_DBC_ASSERT_MSG(a, "ASSERTION FAILED")
#define ARMNMF_DBC_PRECONDITION(a)     ARMNMF_DBC_ASSERT_MSG(a, "PRECONDITION FAILED")
#define ARMNMF_DBC_POSTCONDITION(a)    ARMNMF_DBC_ASSERT_MSG(a, "POSTCONDITION FAILED")
#define ARMNMF_DBC_INVARIANT(a)        ARMNMF_DBC_ASSERT_MSG(a, "INVARIANT FAILED")

#endif
#endif
