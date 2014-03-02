
//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename os_secure.h
//
// $Header: X:/MKS Projects/prototype/prototype/os_abstraction/rcs/os_assert.h 1.2 2009/01/13 14:09:39Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup os_abstraction
///
/// \file
/// \brief
///      OS abstraction for debug "asserts".
///
///      Macro to define the OS abstarction to a debug "assert".
///
//*************************************************************************

#ifndef OS_ASSERT_H
#define OS_ASSERT_H

///  Asserts are only enabled in PC based WIN32 & _DEBUG builds.

#if defined(WIN32) && defined(_DEBUG)
   #include <assert.h>
   #define OS_ASSERT(x)  assert((x))
#else
   #define OS_ASSERT(x)
#endif

#endif // OS_ASSERT_H
