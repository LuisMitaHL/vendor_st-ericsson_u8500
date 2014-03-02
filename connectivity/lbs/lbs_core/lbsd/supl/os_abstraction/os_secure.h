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
// $Header: X:/MKS Projects/prototype/prototype/os_abstraction/rcs/os_secure.h 1.8 2009/01/13 14:09:40Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup os_abstraction
///
/// \file
/// \brief
///      Security helper routines declarations.
///
///      Access routines specifically for using the SUPL encode/decode routines
///      in a way useful for the supl implementation.
///
//*************************************************************************

#ifndef OS_SECURE_H
#define OS_SECURE_H

#include "gps_ptypes.h"

/// Length of VER in bytes.
#define GN_GPS_VER_LEN 8

BL GN_Secure_Calc_VER( unsigned char *p_VER, char *p_fqdn, unsigned char *p_suplinit, size_t pdu_length );

#endif // OS_SECURE_H
