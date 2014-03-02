//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename GAD_Conversions.h
//
// $Header: X:/MKS Projects/prototype/prototype/GAD_helper/rcs/GAD_Conversions.h 1.5 2008/12/16 18:03:57Z geraintf Rel $
// $Locker: $
//*************************************************************************
//*************************************************************************
///
/// \ingroup rrlp_helper
///
/// \file
/// \brief
///     GAD Conversion Routines.
///
/// Specification:
/// 3GPP TS 23.032 V5.0.0 (2003-03)
/// 3rd Generation Partnership Project;
/// Technical Specification Group Core Network;
/// Universal Geographical Area Description (GAD)
/// (Release 5)
/// Access MACROS specifically for converting to and from GAD format.
//*************************************************************************

#ifndef GAD_CONVERSION_H
#define GAD_CONVERSION_H

#include <math.h>

// GAD macro definitions.
#define GAD_SIGMA_ONE_CONFIDENCE       67          ///< Confidence level for a sigma 1 deviation

#define GAD_LATITUDE_SCALING_FACTOR    (1<<23)     ///< Scaling factor for GAD Latitude - pow(2,23).
#define GAD_LATITUDE_DEGREES_IN_RANGE  (90.0)      ///< Range of degrees in GAD Latitude.
#define GAD_LATITUDE_SIGN_MASK         0x800000    ///< Position of SIGN bit in GAD Latitude.
#define GAD_LATITUDE_RANGE_MASK        0x7fffff    ///< Position of value in GAD Latitude.

#define GAD_LONGITUDE_SCALING_FACTOR   (1<<24)     ///< Scaling factor for GAD Longitude - pow(2,24)
#define GAD_LONGITUDE_DEGREES_IN_RANGE (360.0)     ///< Range of degrees in GAD Longitude.

#define GAD_ALTITUDE_DIRECTION_MASK    0x8000      ///< Position of Direction bit in GAD Altitude.
#define GAD_ALTITUDE_RANGE_MASK        0x7fff      ///< Position of value in GAD Altitude.

#define GAD_UNCERTAINTY_C              10.0        ///< C value for use in the binomial convertion from Uncertainty to K.
#define GAD_UNCERTAINTY_x               0.1        ///< x value for use in the binomial convertion from Uncertainty to K.
#define GAD_UNCERTAINTY_MASK           0x7f        ///< Mask constraining the range of K for Uncertainty.

#define GAD_UNCERTAINTY_ALTITUDE_C     45.0        ///< C value for use in the binomial convertion from Uncertainty Altitude to K.
#define GAD_UNCERTAINTY_ALTITUDE_x      0.025      ///< x value for use in the binomial convertion from Uncertainty Altitude to K.
#define GAD_UNCERTAINTY_ALTITUDE_MASK  0x7f        ///< Mask constraining the range of K for Uncertainty Altitude.

#define GAD_DEGREES_INCREMENT                2     ///< Granularity for conversion from degrees to GAD format.
#define GAD_ORIENTATION_ANGLE_INCREMENT_MAX  179   ///< Scale of GAD format increment for range checking.
#define GAD_ORIENTATION_ANGLE_DEGREES_MAX    360   ///< Number of degrees for range checking.

/// Macro for determining Uncertainty or Uncertainty Altitude from GAD K.
#define GAD_BINOMIAL_UNCERTAINTY_FROM_K(C, x, k) ((C) * (pow((1 + (x)), (k)) - 1))
/// Macro for determining GAD K from Uncertainty or Uncertainty Altitude.
#define GAD_BINOMIAL_K_FROM_UNCERTAINTY(Uncertainty, C, x) (log(((Uncertainty) / (C)) + 1) / log(1 + (x)))

#endif //GAD_CONVERSION_H
