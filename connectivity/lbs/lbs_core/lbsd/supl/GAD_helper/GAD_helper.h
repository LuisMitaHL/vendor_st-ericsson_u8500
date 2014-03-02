//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename GAD_helper.h
//
// $Header: X:/MKS Projects/prototype/prototype/GAD_helper/rcs/GAD_helper.h 1.12 2008/10/22 15:11:37Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_helper
///
/// \file
/// \brief
///     GAD helper routines declarations.
///
/// Specification:
/// 3GPP TS 23.032 V5.0.0 (2003-03)
/// 3rd Generation Partnership Project;
/// Technical Specification Group Core Network;
/// Universal Geographical Area Description (GAD)
/// (Release 5)
/// Access routines specifically for converting to and from GAD format.
//*************************************************************************

#ifndef GAD_HELPER_H
#define GAD_HELPER_H

#include <stdio.h>

#include "gps_ptypes.h"
#include "GN_GPS_api.h"
#include "GN_AGPS_api.h"

/// Container for GAD Message type.
typedef struct GAD_Message {
   U1 buflen;        ///< This is implicit and the maximum length of the message is 14
   U1 message[14];   ///< Max length is length of type:
                     ///   Ellipsoid point with altitude and uncertainty Ellipsoid
} s_GAD_Message;

/// GAD Shape type identifier.
/// \note
/// The switch statement central to GAD_Shape_Type_From_Message relies on the enum values
/// in #e_GAD_Shape_Type matching the values in the GAD document.
typedef enum {
    GN_GAD_Ellipsoid_Point                                          = 0x0, //0000
    GN_GAD_Ellipsoid_point_with_uncertainty_Circle                  = 0x1, //0001
    GN_GAD_Ellipsoid_point_with_uncertainty_Ellipse                 = 0x3, //0011
    GN_GAD_Polygon                                                  = 0x5, //0101
    GN_GAD_Ellipsoid_point_with_altitude                            = 0x8, //1000
    GN_GAD_Ellipsoid_point_with_altitude_and_uncertainty_Ellipsoid  = 0x9, //1001
    GN_GAD_Ellipsoid_Arc                                            = 0xa, //1010
    GN_GAD_Unknown
} e_GAD_Shape_Type;

BL GN_AGPS_Pos_From_GAD_Message( s_GAD_Message *p_GAD_Message, s_GN_AGPS_Ref_Pos *p_GN_AGPS_Ref_Pos );
BL GN_AGPS_GAD_Data_From_GAD_Message( s_GAD_Message *p_GAD_Message, s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data );
BL GN_GAD_Message_From_AGPS_Pos( s_GN_AGPS_Ref_Pos *p_GN_AGPS_Ref_Pos, s_GAD_Message *p_GAD_Message );
BL GN_GAD_Message_From_AGPS_GAD_Data( s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data, s_GAD_Message *p_GAD_Message );
BL GN_AGPS_GAD_Data_From_Nav_Data( s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data, s_GN_GPS_Nav_Data *p_GN_GPS_Nav_Data );

#endif // GAD_HELPER_H
