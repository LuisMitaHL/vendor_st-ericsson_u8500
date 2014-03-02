
//****************************************************************************
// GPS IP Centre, ST-Ericsson (UK) Ltd.
// Copyright (c) 2010-2012 ST-Ericsson (UK) Ltd.
// 15-16 Cottesbrooke Park, Heartlands Business Park, Daventry, NN11 8YL, UK.
// All rights reserved.
//
// Filename  GN_AGLON_api.h
//
// $Header: S:/GN_49xxPE_MKS/49xxPE_API/rcs/GN_AGLON_api.h 1.6 2012/01/12 16:51:10Z geraintf Rel $
// $Locker: $
//****************************************************************************

#ifndef GN_AGLON_API_H
#define GN_AGLON_API_H

#ifdef __cplusplus
   extern "C" {
#endif

//****************************************************************************
// File level documentation
/// \file
/// \brief
///      GNSS PE Core Library - A-GLONASS external interface API header file.
//
/// \ingroup  group_GN_AGLON
//
//-----------------------------------------------------------------------------
/// \defgroup  GN_AGLON_api  GNSS PE Core Library - Internally Implemented A-GLONASS API
//
/// \brief
///     Assisted-GLONASS API definitions for functions implemented internally.
//
/// \details
///     GNSS PE Core Library API definitions for the Assisted-GLONASS structures and
///     functions provided in the library and can be called by the Host
///     software (ie inward called) to exchange the fundamental Assisted-GLONASS data
///     items.
///     <p> These GN_AGLON_??? Assisted-GLONASS API's are not supported in CG2900.
///     <p> These GN_AGLON_??? API functions must not be called before #GN_GPS_Initialise().
///     <p> The input GN_AGLON_Set_??? API functions must only be called when
///     the GPS is awake,  ie not between a GN_GPS_Sleep() and a GN_GPS_WakeUp().
///     <p> The output GN_AGLON_Get_??? API functions may be called when the GPS
///     is asleep, but if done so will return stale data.
//
//*****************************************************************************

#include "gps_ptypes.h"

//*****************************************************************************
/// \addtogroup GN_AGLON_api
/// \{

//*****************************************************************************
/// \brief
///     GN A-GLONASS satellite Raw Ephemeris data.
/// \details
///     GN A-GLONASS satellite Ephemeris data in compressed binary form, as it
///     is transmitted by the satellites.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \note
///     See the Glonass ICD v5.1 for a full description.
/// \attention
///     If the #UTCTod is not known then it should be set to zero.
typedef struct  // GN_AGLON_Eph
{
   U4 UTCTod;           ///< UTC Moscow Time of Day [range 0..86399 s]
   I1 FrqCh;            ///< Glonass satellite Frequency Channel Id [range -7..+13, 127=Unknown].
   U4 word[12];         ///< Glonass Ephemeris & Clock Binary words

}  s_GN_AGLON_Eph;               // GN A-GLONASS Satellite Ephemeris Data


//*****************************************************************************
/// \brief
///     GN A-GLONASS satellite Raw Almanac data.
/// \details
///     GN A-GLONASS satellite Almanac data in compressed binary form, as it
///     is transmitted by the satellites.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \note
///     See the Glonass ICD v5.1 for a full description.
/// \attention
///     If the #UTCTod is not known then it should be set to zero.
typedef struct    // GN_AGLON_Alm
{
   U4 UTCTod;           ///< UTC Moscow Time of Day [range 0..86399 s]
   U2 NA;               ///< Almanac Reference Day Number [range 1..1461 days within 4 year period]
   U4 word[6];          ///< Glonass Almanac words

}  s_GN_AGLON_Alm;               // GN A-GLONASS Satellite Almanac data


//*****************************************************************************
/// \brief
///     GN A-GLONASS satellite Ephemeris data Elements.
/// \details
///     GN A-GLONASS satellite Ephemeris data extracted and presented as individual
///     named elements, scaled as they are in the broadcast Glonass Navigation Message.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \note
///     See the Glonass ICD v5.1 for a full description.
/// \attention
///     If the Current Glonass date #gloNT is not known (eg when obtained from either an
///     RRLP or RRC Navigation Model message) then it should be set to zero.
typedef struct  // GN_AGLON_Eph_El
{
   // Terms transmitted in strings 1-4, naming from Glonass ICD v5.1.
   I1 gloN;             ///< Glonass satellite Slot Number [range 1..24]
   I1 gloFrqCh;         ///< Glonass satellite Frequency Channel Id [range -7..+13, 127=Unknown].
   U1 gloTb;            ///< Serial number of time interval within current day (in UTC_SU + 3hrs) [range 1..96 x 15min]
   U2 gloNT;            ///< Glonass date [range 1..1461 days within 4 year period,  0=Unknown]].
   U1 gloLn;            ///< Healthy Flag for transmitting satellite [range 0..1, where 0=Healthy; 1=Unhealthy]
   U1 gloFT;            ///< Encoded User Range Accuracy (URA) [range 0..15,  representing {1, 2, 2.5, 4, 5, 7, 10, 12, 14, 16, 32, 64, 128, 256, 512 metres, Not used}]

   // Positioning terms, naming from NavModel-GLONASS-ECEF  3GPP TS 36.355 v9.0.0 (pp56)
   U1 gloEn;            ///< Age of data [range 0..31 days]
   I1 gloP1;            ///< Encoded time interval between adjacent Tb values [range 0..3, representing {0, 30, 45, 60 minutes}]
   BL gloP2;            ///< Oddness value of the value of Tb (for intervals of 30 or 60min) [range 0..1]
   I1 gloM;             ///< Glonass satellite type [range 0..3, representing {Glonass, Glonass-M, Reserved, Reserved }]

   I4 gloX;             ///< PZ90.02 ECEF X co-ordinate of satellite position at time Tb [range -67108864..67108863 x 2^-11 km]
   I4 gloY;             ///< PZ90.02 ECEF Y co-ordinate of satellite position at time Tb [range -67108864..67108863 x 2^-11 km]
   I4 gloZ;             ///< PZ90.02 ECEF Z co-ordinate of satellite position at time Tb [range -67108864..67108863 x 2^-11 km]

   I4 gloXdot;          ///< PZ90.02 ECEF X co-ordinate of satellite velocity at time Tb [range -8388608..8388607 x 2^-20 km/s]
   I4 gloYdot;          ///< PZ90.02 ECEF Y co-ordinate of satellite velocity at time Tb [range -8388608..8388607 x 2^-20 km/s]
   I4 gloZdot;          ///< PZ90.02 ECEF Z co-ordinate of satellite velocity at time Tb [range -8388608..8388607 x 2^-20 km/s]

   I1 gloXdotdot;       ///< PZ90.02 ECEF X co-ordinate of satellite acceleration at time Tb [range -16..15 x 2^-30 km/s^2]
   I1 gloYdotdot;       ///< PZ90.02 ECEF Y co-ordinate of satellite acceleration at time Tb [range -16..15 x 2^-30 km/s^2]
   I1 gloZdotdot;       ///< PZ90.02 ECEF Z co-ordinate of satellite acceleration at time Tb [range -16..15 x 2^-30 km/s^2]

   // Timing terms, naming from GLONASS-ClockModel 3GPP TS 36.355 v9.0.0 (pp51).
   I1 gloDeltaTau;      ///< Time difference between transmission in G1 and G2 [range -16..15 x 2^-30 s]
   I4 gloTau;           ///< Satellite clock offset [range -2097152..2097151 x 2^-30 s]
   I2 gloGamma;         ///< Relative frequency offset from nominal value [range -1024..1023 x 2^-40 s/s]

}  s_GN_AGLON_Eph_El;            // GN A-GLONASS satellite Ephemeris data Elements.


//*****************************************************************************
/// \brief
///     GN A-GLONASS satellite Almanac data Elements.
/// \details
///     GN A-GLONASS satellite Almanac data extracted and presented as individual
///     named elements, scaled as they are in the broadcast Glonass Navigation Message.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \note
///     See the Glonass ICD v5.1 for a full description.
typedef struct  // GN_AGLON_Alm_El
{
   // Positioning terms
   // Naming from NavModel-GLONASS-ECEF  3GPP TS 36.355 v9.0.0 (pp66)
   // Description from Glonass ICD v5.1 (pp36)
   U2 gloAlmNA;         ///< Calendar day number within four year period [range 1..1461 days] (Note this is taken from string 5)
   U1 gloAlmnA;         ///< Glonass satellite Slot Number [range 1..24]
   U1 gloAlmHA;         ///< Carrier frequency number of navigation RF signal [range 0..31]
   I4 gloAlmLambdaA;    ///< Longitude of the first ascending node in the day (Lambda nA) [range -1048576..1048575 x 2^-20 semi-circles]
   U4 gloAlmtlambdaA;   ///< Time of Lambda nA [range 0..2097151 x 2^-5 s]
   I4 gloAlmDeltaIa;    ///< Correction to the mean inclination angle of 63 degrees, at t Lambda nA [range -131072..131071 x 2^-20 semi-circles]
   I4 gloAlmDeltaTA;    ///< Correction to mean Draconian period of 43200s at t Lambda nA [range -2097152..2097151 x 2^-9 s]
   I1 gloAlmDeltaTdotA; ///< Rate of change of Draconian period [range -64..63 x 2^-14 s/orbit period]
   U2 gloAlmEpsilonA;   ///< Eccentricity at t Lambda nA [range 0..32767 x 2^-20]
   I2 gloAlmOmegaA;     ///< Argument of perigee at t Lambda nA [range -32768..32767 x 2^-15 semi-circles]
   I2 gloAlmTauA;       ///< Coarse value of Satellite time correction at t Lambda nA [range -512..511 x 2^-18 s]

   // Other terms
   // Naming from NavModel-GLONASS-ECEF  3GPP TS 36.355 v9.0.0 (pp66)
   // Description from Glonass ICD v5.1 (pp36)
   U1 gloAlmCA;         ///< Generalised satellite 'Health Flag' at almanac upload time [range 0..1, 0=Unhealthy, 1=Healthy]
   U1 gloAlmMA;         ///< Glonass satellite type [range 0..3, representing {Glonass, Glonass-M, Reserved, Reserved }]

}  s_GN_AGLON_Alm_El;          // GN A-GLONASS satellite Almanac data Elements.



//*****************************************************************************
//
// GNSS PE Core Library  -  A-GLONASS External Interface API function prototypes.
//
// API Functions implemented by the GNSS Core PE Library and may be called by
// the Host platform software.
//
//*****************************************************************************

//*****************************************************************************
/// \brief
///     GN A-GLONASS API Function to Set (ie input) Ephemeris string data for the
///     specified Glonass satellite slot number.
/// \details
///     GN A-GLONASS API Function to Set (ie input) Ephemeris string data for the
///     specified Glonass satellite slot number.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \attention
///     This ephemeris data will only be used if a valid current Glonass broadcast
///     ephemeris has not recently been obtained from the satellite itself.
/// \attention
///     This A-GLONASS API function must not be called when the Receiver is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GLONASS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the ephemeris was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGLON_Set_Eph(
   U1 SV,                           ///< [in] Glonass satellite slot number [range 1..24]
   s_GN_AGLON_Eph *p_Eph            ///< [in] Pointer to the source Ephemeris data
);


//*****************************************************************************
/// \brief
///     GN A-GLONASS API Function to Get Ephemeris string data for the specified
///     Glonass satellite slot number.
/// \details
///     GN A-GLONASS API Function to Get Ephemeris string data for the specified
///     Glonass satellite slot number.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \attention
///     The ephemeris data returned is what is currently being used internally
///     and could have recently been decoded from a live satellite signal.
///     This means that calling this API immediately after calling
///     #GN_AGLON_Set_Eph() may not return the ephemeris data just input.
/// \attention
///     This A-GLONASS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether retrieving the ephemeris was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
BL GN_AGLON_Get_Eph(
   U1 SV,                           ///< [in] Glonass satellite slot number [range 1..24]
   s_GN_AGLON_Eph *p_Eph            ///< [in] Pointer to the destination Ephemeris data
);


//*****************************************************************************
/// \brief
///     GN A-GLONASS API Function to Set (ie input) Almanac string data for the
///     specified Glonass satellite slot number.
/// \details
///     GN A-GLONASS API Function to Set (ie input) Almanac string data for the
///     specified Glonass satellite slot number.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \attention
///     This almanac data will only be used if a valid current Glonass broadcast
///     almanac has not recently been obtained from the satellite itself.
/// \attention
///     This A-GLONASS API function must not be called when the Receiver is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GLONASS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the ephemeris was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGLON_Set_Alm(
   U1 SV,                           ///< [in] Glonass satellite slot number [range 1..24]
   s_GN_AGLON_Alm *p_Alm            ///< [in] Pointer to the source Almanac data
);


//*****************************************************************************
/// \brief
///     GN A-GLONASS API Function to Get Almanac string data for the specified
///     Glonass satellite slot number.
/// \details
///     GN A-GLONASS API Function to Get Almanac string data for the specified
///     Glonass satellite slot number.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \attention
///     The almanac data returned is what is currently being used internally
///     and could have recently been decoded from a live satellite signal.
///     This means that calling this API immediately after calling
///     #GN_AGLON_Set_Alm() may not return the almanac data just input.
/// \attention
///     This A-GLONASS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether retrieving the ephemeris was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
BL GN_AGLON_Get_Alm(
   U1 SV,                           ///< [in] Glonass satellite slot number [range 1..24]
   s_GN_AGLON_Alm *p_Alm            ///< [in] Pointer to the destination Almanac data
);


//*****************************************************************************
/// \brief
///     GN A-GLONASS API Function to Set (ie input) Ephemeris data elements for the
///     specified Glonass satellite slot number.
/// \details
///     GN A-GLONASS API Function to Set (ie input) Ephemeris data elements for the
///     specified Glonass satellite slot number.
///     <p> The data fields are scaled as they appear in the broadcast Glonass
///     Navigation Message subframe, and is also the same scaling as used in the
///     RRLP and RRC A-GLONASS protocols.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \attention
///     This ephemeris data will only be used if a valid current Glonass broadcast
///     ephemeris has not recently been obtained from the satellite itself.
/// \attention
///     This A-GLONASS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GLONASS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the ephemeris was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGLON_Set_Eph_El(
   s_GN_AGLON_Eph_El* p_Eph_El      ///< [in] Pointer to the source Ephemeris data
);


//*****************************************************************************
/// \brief
///     GN A-GLONASS API Function to Get Ephemeris data Elements for the specified
///     Glonass satellite slot number.
/// \details
///     GN A-GLONASS API Function to Get Ephemeris data Elements for the specified
///     Glonass satellite slot number.
///     <p> The data fields are scaled as they appear in the broadcast Glonass
///     Navigation Message subframe, and is also the same scaling as used in the
///     RRLP and RRC A-GLONASS protocols.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \attention
///     The ephemeris data returned is what is currently being used internally
///     and could have recently been decoded from a live satellite signal.
///     This means that calling this API immediately after calling
///     #GN_AGLON_Set_Eph_El() may not return the ephemeris data just input.
/// \attention
///     This A-GLONASS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether retrieving the ephemeris was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
BL GN_AGLON_Get_Eph_El(
   U1 SV,                           ///< [in] Glonass satellite slot number [range 1..24]
   s_GN_AGLON_Eph_El* p_Eph_El      ///< [in] Pointer to the destination Ephemeris data
);


//*****************************************************************************
/// \brief
///     GN A-GLONASS API Function to Set (ie input) Almanac data elements for the
///     specified Glonass satellite slot number.
/// \details
///     GN A-GLONASS API Function to Set (ie input) Almanac data elements for the
///     specified Glonass satellite slot number.
///     <p> The data fields are scaled as they appear in the broadcast Glonass
///     Navigation Message subframe, and is also the same scaling as used in the
///     RRLP and RRC A-GLONASS protocols.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \attention
///     This almanac data will only be used if a valid current Glonass broadcast
///     almanac has not recently been obtained from the satellite itself.
/// \attention
///     This A-GLONASS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GLONASS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the ephemeris was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGLON_Set_Alm_El(
   s_GN_AGLON_Alm_El* p_Alm_El      ///< [in] Pointer to the source Almanac data
);


//*****************************************************************************
/// \brief
///     GN A-GLONASS API Function to Get Almanac data Elements for the specified
///     Glonass satellite slot number.
/// \details
///     GN A-GLONASS API Function to Get Almanac data Elements for the specified
///     Glonass satellite slot number.
///     <p> The data fields are scaled as they appear in the broadcast Glonass
///     Navigation Message subframe, and is also the same scaling as used in the
///     RRLP and RRC A-GLONASS protocols.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \attention
///     The almanac data returned is what is currently being used internally
///     and could have recently been decoded from a live satellite signal.
///     This means that calling this API immediately after calling
///     #GN_AGLON_Set_Eph_El() may not return the ephemeris data just input.
/// \attention
///     This A-GLONASS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether retrieving the ephemeris was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
BL GN_AGLON_Get_Alm_El(
   U1 SV,                           ///< [in] Glonass satellite slot number [range 1..24]
   s_GN_AGLON_Alm_El* p_Alm_El      ///< [in] Pointer to the destination Almanac data
);


//*****************************************************************************
/// \brief
///     GN A-GLONASS API Function to Set the Glonass channel number for the
///     specified satellite slot number.
/// \details
///     GN A-GLONASS API Function to Set the Glonass channel number the
///     specified satellite slot number is currently transmitting on.
///     <p> GLONASS is a Frequency Division Multiplex Access system where each
///     satellite in view is transmitting in a different frequency channel.
///     Two antipodal satellites (ie slot numbers) use the same frequency
///     channel, but only one can be seen at any point in time.   As such,
///     it is important to know which satellite slot number is currently visible
///     in each frequency channel so that the correct Almanac or Ephemeris
///     assistance can be associated with it.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \attention
///     This A-GLONASS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether retrieving the ephemeris was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
BL GN_AGLON_Set_Chan_Num(
   U1 SV,                           ///< [in] Glonass satellite slot number [range 1..24]
   I1 gloChanNum                    ///< [in] Glonass channel number [range -7..+13]
);


//*****************************************************************************

/// \}

#ifdef __cplusplus
   }     // extern "C"
#endif

#endif   // GN_AGLON_API_H
