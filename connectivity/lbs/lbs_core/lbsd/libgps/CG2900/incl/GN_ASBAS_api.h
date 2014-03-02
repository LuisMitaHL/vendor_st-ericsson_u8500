
//****************************************************************************
// GPS IP Centre, ST-Ericsson (UK) Ltd.
// Copyright (c) 2011 ST-Ericsson (UK) Ltd.
// 15-16 Cottesbrooke Park, Heartlands Business Park, Daventry, NN11 8YL, UK.
// All rights reserved.
//
// Filename  GN_ASBAS_api.h
//
// $Header: S:/GN_49xxPE_MKS/49xxPE_API/rcs/GN_ASBAS_api.h 1.2 2011/08/15 11:51:17Z geraintf Rel $
// $Locker: $
//****************************************************************************

#ifndef GN_ASBAS_API_H
#define GN_ASBAS_API_H

#ifdef __cplusplus
   extern "C" {
#endif


//****************************************************************************
// File level documentation
/// \file
/// \brief
///      GNSS PE Core Library - A-SBAS external interface API header file.
//
/// \ingroup  group_GN_ASBAS
//
//-----------------------------------------------------------------------------
/// \defgroup  GN_ASBAS_api  GNSS PE Core Library - Internally Implemented A-SBAS API
//
/// \brief
///     Assisted-SBAS API definitions for functions implemented internally.
//
/// \details
///     GNSS PE Core Library API definitions for the Assisted-SBAS (Satellite Based
///     Augmentation System, eg WAAS, EGNOS, MSAS, GAGAN) structures and functions
///     provided in the library and can be called by the Host software (ie inward
///     called) to exchange the fundamental Assisted-SBAS data items.
///     <p> These GN_ASBAS_??? API functions must not be called before #GN_GPS_Initialise().
///     <p> The input GN_ASBAS_Set_??? API functions must only be called when
///     the GPS is awake,  ie not between a GN_GPS_Sleep() and a GN_GPS_WakeUp().
///     <p> The output GN_ASBAS_Get_??? API functions may be called when the GPS
///     is asleep, but if done so will return stale data.
//
//*****************************************************************************

#include "gps_ptypes.h"

//*****************************************************************************
/// \addtogroup GN_ASBAS_api
/// \{

//*****************************************************************************
/// \brief
///     GN A-SBAS satellite Ephemeris data Elements.
/// \details
///     GN A-SBAS satellite (eg WAAS, EGNOS, MSAS, GAGAN) Ephemeris data extracted
///     and presented as individual named elements, scaled as they are in the
///     broadcast SBAS GEO Navigation Message.
/// \note
///     See the RTCA DO-229D for a full description of the SBAS GEO Navigation
///     Message (type 9) in Section A.4.4.11 and Table A-18.
/// \note
///     Regarding the #URA User Range Accuracy value, see the note in Table 2.3
///     of RTCA DO-229D ("The conversion to sigma of the SBAS URA from message
///     type 9 is not standardized among service providers and should not be
///     used for that purpose."  Also see Note 2 in Section A.4.4.11 of
///     RTCA DO-229D ("A URA of 15 indicates the satellite ranging signal should
///     not be used.  Other URA values are not standardized amongst service
///     providers and should not be used.")
typedef struct  // GN_ASBAS_Eph_El
{
   U2 t0;               ///< Time of applicability of ephemeris [range 0..5399 x 16 s].
   U1 URA;              ///< User Range Accuracy  [either 0 or 15,  see URA note]
   I4 agf0;             ///< Satellite Clock Bias [range -2048..2047 x 2^-31 s]
   I4 agf1;             ///< Satellite Clock Drift [range -128..127 x 2^-40 s/s]
   I4 pos[3];           ///< Satellite X,Y,Z ECEF Position [X,Y range -536870912..536870911 x 0.08 m, Z range -16777216..16777215 x 0.4 m]
   I4 vel[3];           ///< Satellite X,Y,Z ECEF Velocity [X,Y range -65536..+65535 x 0.000625 m/s, Z range -131072..131071 x 0.004 m/s]
   I4 acc[3];           ///< Satellite X,Y,Z ECEF Acceleration [X,Y range -512..511 x 0.0000125 m/s^2, Z range -512..511 x 0.0000625 m/s^2]

} s_GN_ASBAS_Eph_El;             // GN A-SBAS Satellite Ephemeris data Elements


//*****************************************************************************
/// \brief
///     GN A-SBAS satellite Almanac data Elements.
/// \details
///     GN A-SBAS satellite (eg WAAS, EGNOS, MSAS, GAGAN) Almanac data extracted
///     and presented as individual named elements, scaled as they are in the
///     broadcast SBAS GEO Almanac Message.
/// \note
///     See the RTCA DO-229D for a full description of the SBAS GEO Almanac
///     Message (type 17)  Section A.4.4.12 and Table A-19.
typedef struct  // GN_ASBAS_Alm_El
{
   U2 t0;               ///< Time of applicability of Almanac [range 0..1349 x 64 s]
   U1 Health;           ///< Satellite Health and Status [range 0..255]
   I4 pos[3];           ///< Satellite X,Y,Z ECEF Position [XY range -16384..16383 x 2600 m, Z range -256..255 x 26000 m]
   I4 vel[3];           ///< Satellite X,Y,Z ECEF Velocity [XY range -4..3 x 10 m/s,  Y range -8..7 x 40.96 m/s]

} s_GN_ASBAS_Alm_El;             // GN A-SBAS Satellite Almanac data Elements



//*****************************************************************************
//
// GNSS PE Core Library  -  A-SBAS External Interface API function prototypes.
//
// API Functions implemented by the GNSS Core PE Library and may be called by
// the Host platform software.
//
//*****************************************************************************

//*****************************************************************************
/// \brief
///     GN A-SBAS API Function to Set (ie input) Ephemeris data elements for the
///     specified SBAS satellite.
/// \details
///     GN A-SBAS API Function to Set (ie input) Ephemeris data elements for the
///     specified SBAS (eg WAAS, EGNOS, MSAS, GAGAN) satellite.
/// \attention
///     This ephemeris data will only be used if a valid current SBAS broadcast
///     ephemeris has not recently been obtained from the satellite itself.
/// \attention
///     This A-SBAS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-SBAS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the ephemeris was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_ASBAS_Set_Eph_El(
   U1 SV,                           ///< [in] The SBAS SV identification number [range 120 .. 138]
   I1 IOD,                          ///< [in] Ephemeris Issue Of Data [range 0..255] or -1 if unknown
   s_GN_ASBAS_Eph_El* p_Eph_El      ///< [in] Pointer to the source Ephemeris data
);


//*****************************************************************************
/// \brief
///     GN A-SBAS API Function to Set (ie input) Almanac data elements for the
///     specified SBAS satellite.
/// \details
///     GN A-SBAS API Function to Set (ie input) Almanac data elements for the
///     specified SBAS (eg WAAS, EGNOS, MSAS, GAGAN) satellite.
/// \attention
///     This almanac data will only be used if a valid current SBAS broadcast
///     ephemeris has not recently been obtained from the satellite itself.
/// \attention
///     This A-SBAS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-SBAS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the ephemeris was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_ASBAS_Set_Alm_El(
   U1 SV,                           ///< [in] The SBAS SV identification number [range 120 .. 138]
   s_GN_ASBAS_Alm_El* p_Alm_El      ///< [in] Pointer to the source Almanac data
);


//*****************************************************************************

/// \}

#ifdef __cplusplus
   }     // extern "C"
#endif

#endif   // GN_ASBAS_API_H
