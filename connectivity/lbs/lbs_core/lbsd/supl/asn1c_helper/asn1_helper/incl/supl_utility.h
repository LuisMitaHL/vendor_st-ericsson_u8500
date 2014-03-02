//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_utility.h
//
// 
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_utility.h
///
/// \file
/// \brief
///      ASN.1 helper routines implementation.
///
///      General Utilities specifically for using the SUPL encode/decode routines
///      in a way useful for the supl_subsystem.
///
//*************************************************************************
#ifndef INCLUSION_GUARD_SUPL_UTILITY_H
#define INCLUSION_GUARD_SUPL_UTILITY_H
/**************************************************************************
 *
 * $Copyright Ericsson AB 2009 $
 *
 **************************************************************************
 *
 *  Module : Positioning (SUPL)
 *
 *  Brief  : POSITIONING MANAGER SUPL Utility Functions
 *
 **************************************************************************
 *
 * $Workfile: SUPL_utility.h $
 *
 **************************************************************************
*/

/*
 **************************************************************************
 * Includes
 **************************************************************************
*/

//standard files
#include <string.h>
#include <stdlib.h>
#include <wchar.h>

#ifndef POS_SUPL_FTR
//basic system files
//#include "r_debug.h"
#endif /* #ifndef POS_SUPL_FTR */

//internal supl
//#include "SUPL_osa.h"

/*
 **************************************************************************
 * Defines and type declarations
 **************************************************************************
*/

//#ifdef POS_SUPL_FTR
#define SUPL_CRI_(...) CRI(__VA_ARGS__)
#define SUPL_ERR_(...) ERR(__VA_ARGS__)
#define SUPL_API_(...) API(__VA_ARGS__)
#define SUPL_INF_(...) INF(__VA_ARGS__)
#define SUPL_DBG_(...) DBG(__VA_ARGS__)
#define SUPL_TXT_(...) INF_BLANK(__VA_ARGS__)

#define SUPL_INT_HBYTE(i) (uint8_t)((i) >> 8)
#define SUPL_INT_LBYTE(i) (uint8_t)((i) & 0xff)

#define SUPL_BYTES_TO_UINT16(l, h) (uint16_t)((h << 8) + l)

#define SUPL_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define SUPL_MIN(x, y) (((x) < (y)) ? (x) : (y))

//#else


/** This type holds time zone information. The value of the local time zone is
 * in units of quarter-hours (i.e. 15 minutes per unit). Any integer from - 48
 * to + 56 should be allowed. This covers time zones from - 12 hours to + 14
 * hours.
 */
typedef int8_t SUPL_clock_time_zone_t;

typedef struct
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
}SUPL_date_t;

typedef struct
{
  uint8_t hours;
  uint8_t minutes;
  uint8_t seconds;
}SUPL_time_of_day_t;

typedef struct
{
  SUPL_date_t date;
  SUPL_time_of_day_t time_of_day;
}SUPL_date_and_time_t;

typedef uint32_t SUPL_universal_time_units_t;

/*
 **************************************************************************
 * Function prototypes
 **************************************************************************
*/

void SUPL_TimeZone_AddOrSubtract(
  const SUPL_clock_time_zone_t clock_time_zone,
  const boolean time_zone_add,
  SUPL_date_and_time_t * const utc_date_and_time );

SUPL_universal_time_units_t SUPL_DateAndTimeUTCToUniversalTimeUnits_Convert(
  const SUPL_date_and_time_t * const utc_date_and_time );

void SUPL_UniversalTimeUnitsToDateAndTimeUTC_Convert(
  const SUPL_universal_time_units_t utc_seconds,
  SUPL_date_and_time_t * const utc_date_and_time );

#endif /* INCLUSION_GUARD_SUPL_UTILITY_H */
