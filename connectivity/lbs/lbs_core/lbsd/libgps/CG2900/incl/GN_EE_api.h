
//****************************************************************************
// GPS IP Centre, ST-Ericsson (UK) Ltd.
// Copyright (C) 2007-2009 ST-Ericsson (UK) Ltd.
// 15-16 Cottesbrooke Park, Heartlands Business Park, Daventry, NN11 8YL, UK.
// All rights reserved.
//
// Filename  GN_EE_api.h
//
// $Header: S:/GN_49xxPE_MKS/49xxPE_API/rcs/GN_EE_api.h 1.4 2011/08/15 11:51:17Z geraintf Rel $
// $Locker: $
//****************************************************************************

#ifndef GN_EE_API_H
#define GN_EE_API_H

#ifdef __cplusplus
   extern "C" {
#endif

//****************************************************************************
// File level documentation
/// \file
/// \brief
///      GNSS PE Core Library - Extended Ephemeris external interface API header file.
//
/// \ingroup  group_GN_EE
//
//-----------------------------------------------------------------------------
/// \defgroup  GN_EE_api  GNSS PE Core Library - Externally Implemented Extended Ephemeris API.
//
/// \brief
///     Extended Ephemeris API definitions for functions implemented externally.
//
/// \details
///     GNSS PE Core Library API definitions for the Extended Ephemeris structures
///     and functions called by the library and implemented externally by the Host
///     software (ie outward called) to exchange the fundamental Extended Ephemeris
///     data items.
///     <p>If Extended Ephemeris functionality is not required, then these
///     GN_EE_???() API's can be simply replaced with the stubs provided in
///     source file GN_EE_api_stubs.c
//
//*****************************************************************************

#include "gps_ptypes.h"

//*****************************************************************************
/// \addtogroup GN_EE_api
/// \{

//*****************************************************************************
/// \brief
///     GN Extended Ephemeris GPS satellite Ephemeris Elements.
/// \details
///     GN Extended Ephemeris GPS satellite Ephemeris Elements scaled as they
///     are in the broadcast GPS Navigation Message.
/// \note
///     See the ICD-GPS-200 for a full description of the Navigation Message format.
/// \attention
///     If the full GPS #Week number is known, then the correct N x 1024
///     Week ambiguity should be included.
/// \attention
///     It is common that #toc = #toe. In some formats only one of these
///     terms is given, in which case it can be set equal to the other.
typedef struct  // GN_EE_GPS_Eph_El
{
   U1 SatID;                  ///< Satellite ID (PRN)                       :  6 bits [range 1..32]
   U1 CodeOnL2;               ///< C/A or P on L2                           :  2 bits [range 0..3]
   U1 URA;                    ///< User Range Accuracy Index                :  4 bits [range 0..15]
   U1 SVHealth;               ///< Satellite Health Bits                    :  6 bits [range 0..63]
   U1 FitIntFlag;             ///< Fit Interval Flag                        :  1 bit  [range 0..1, 0=4hrs, 1=6hrs]
   U1 AODA;                   ///< Age Of Data Offset                       :  5 bits [range 0..31 x 900 sec]
   I1 L2Pflag;                ///< L2 P Data Flag                           :  1 bit  [range 0..1]
   I1 TGD;                    ///< Total Group Delay                        :  8 bits [range -128..127 x 2^-31 sec]
   I1 af2;                    ///< SV Clock Drift Rate                      :  8 bits [range -128..127 x 2^-55 sec/sec2]
   U2 Week;                   ///< GPS Reference Week Number                : 10 bits [range 0..1023 weeks]
   U2 toc;                    ///< Clock Reference Time of Week             : 16 bits [range 0..37800 x 2^4 sec]
   U2 toe;                    ///< Ephemeris Reference Time of Week         : 16 bits [range 0..37800 x 2^4 sec]
   U2 IODC;                   ///< Issue Of Data Clock                      : 10 bits [range 0..1023]
   I2 af1;                    ///< SV Clock Drift                           : 16 bits [range -32768..32767 x 2^-43 sec/sec]
   I2 dn;                     ///< Delta n                                  : 16 bits [range -32768..32767 x 2^-43 semi-circles/sec]
   I2 IDot;                   ///< Rate of Inclination Angle                : 14 bits [range -8192..8191 x 2^-43 semi-circles/sec]
   I2 Crs;                    ///< Coefficient-Radius-sine                  : 16 bits [range -32768..32767 x 2^-5 meters]
   I2 Crc;                    ///< Coefficient-Radius-cosine                : 16 bits [range -32768..32767 x 2^-5 meters]
   I2 Cus;                    ///< Coefficient-Argument_of_Latitude-sine    : 16 bits [range -32768..32767 x 2^-29 radians]
   I2 Cuc;                    ///< Coefficient-Argument_of_Latitude-cosine  : 16 bits [range -32768..32767 x 2^-29 radians]
   I2 Cis;                    ///< Coefficient-Inclination-sine             : 16 bits [range -32768..32767 x 2^-29 radians]
   I2 Cic;                    ///< Coefficient-Inclination-cosine           : 16 bits [range -32768..32767 x 2^-29 radians]
   I4 af0;                    ///< SV Clock Bias                            : 22 bits [range -2097152..2097151 x 2^-31 sec]
   I4 M0;                     ///< Mean Anomaly                             : 32 bits [range -2147483648..2147483647 x 2^-31 semi-circles]
   U4 e;                      ///< Eccentricity                             : 32 bits [range 0..4294967296 x 2^-33]
   U4 APowerHalf;             ///< (Semi-Major Axis)^1/2                    : 32 bits [range 0..4294967296 x 2^-19 metres^1/2]
   I4 Omega0;                 ///< Longitude of the Ascending Node          : 32 bits [range -2147483648..2147483647 x 2^-31 semi-circles]
   I4 i0;                     ///< Inclination angle                        : 32 bits [range -2147483648..2147483647 x 2^-31 semi-circles]
   I4 w;                      ///< Argument of Perigee                      : 32 bits [range -2147483648..2147483647 x 2^-31 meters]
   I4 OmegaDot;               ///< Rate of Right Ascension                  : 24 bits [range -8388608..8388607 x 2^-43 semi-circles/sec]

}  s_GN_EE_GPS_Eph_El;        // Extended Ephemeris - GPS Ephemeris Elements


//*****************************************************************************
/// \brief
///     GN Extended Ephemeris Glonass satellite Ephemeris data Elements.
/// \details
///     GN Extended Ephemeris Glonass satellite Ephemeris Elements scaled as they
///     are in the broadcast Glonass Navigation Message.
/// \note
///     A-GLONASS API's are not supported in CG2900.
/// \note
///     See the Glonass ICD v5.1 for a full description.
typedef struct  // GN_EE_GLON_Eph_El
{
   // Terms transmitted in strings 1-4, naming from Glonass ICD v5.1.
   I1 gloN;             ///< Glonass satellite Slot Number [range 1..24]
   U1 gloTb;            ///< Serial number of time interval within current day (in UTC_SU + 3hrs) [range 1..96 x 15min]
   U2 gloNT;            ///< Current Glonass date [range 1..1461 days within 4 year period].  Set to 0 if unknown
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

}  s_GN_EE_GLON_Eph_El;       // Extended Ephemeris - Glonass Ephemeris Elements

//*****************************************************************************



//*****************************************************************************
/// \brief
///      GN EE API Function that performs tall the GN Extended Ephemeris Software
//       Initialisation activities at the GNSS sub-system Initialisation.
/// \details
///      GN EE API Function that performs tall the GN Extended Ephemeris Software
///      Initialisation activities at the GNSS sub-system Initialisation.
///      This API function is called at end of GN_GPS_Initialise().
/// \returns
///      None.
void GN_EE_Initialise( void );


//*****************************************************************************
/// \brief
///     GN EE Function that requests a new GPS Extended Ephemeris Element record
///     input for the specified GPS satellite PRN number.
/// \details
///     GN EE Function that requests a new GPS Extended Ephemeris Element record
///     input for the specified GPS satellite PRN number.
///     <p> The EE data fields are scaled as they appear in the broadcast
///     GPS Navigation Message subframe.
///     <p>This function is called from GN_GPS_Update(), but only if neither a
///     current valid Broadcast or A-GPS ephemeris is available internally.
/// \attention
///     This extended ephemeris data will stop being used if a valid broadcast
///     ephemeris or A-GPS ephemeris becomes available.
/// \returns
///     Flag to indicate whether the requested extended ephemeris is provided.
/// \retval #TRUE if extended ephemeris is provided.
/// \retval #FALSE if extended ephemeris is not available.
BL GN_EE_Get_GPS_Eph_El(
   U1 SV,                           ///< [in] GPS Satellite identification (PRN) number [range 1..32].
   U2 WeekNo,                       ///< [in] Current full GPS Week No (including 1024 roll overs) [range 0..4095 weeks].
   U4 TOW_s,                        ///< [in] Current GPS Time of Week [range 0..604799 s].
   s_GN_EE_GPS_Eph_El *p_Eph_El     ///< [in] Pointer to destination GPS Extended Ephemeris data Elements
);


//*****************************************************************************
/// \brief
///     GN EE Function that provides a new GPS Broadcast or A-GPS Assistance
///     Ephemeris to the Extended Ephemeris provider.
/// \details
///     GN EE Function that provides a new GPS Broadcast or A-GPS Assistance
///     Ephemeris to the Extended Ephemeris provider so that it can be used to
///     aid the generation of future extended ephemeris data.
///     <p> The Ephemeris data elements fields are scaled as they appear in the
///     broadcast GPS Navigation Message subframe.
///     <p>This function is called from GN_GPS_Update() when a new GPS Broadcast
///     or A-GPS Assistance Ephemeris becomes available internally.
/// \returns
///     Flag to indicate whether setting the ephemeris was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_EE_Set_GPS_Eph_El(
   s_GN_EE_GPS_Eph_El *p_Eph_El     ///< [in] Pointer to source GPS Extended Ephemeris data Elements
);


//*****************************************************************************
///     GN EE Function that requests a new Glonass Extended Ephemeris Element
///     record input for the specified Glonass satellite slot number.
/// \details
///     GN EE Function that requests a new Glonass Extended Ephemeris Element
///     record input for the specified Glonass satellite slot number.
///     <p> The EE data fields are scaled as they appear in the broadcast
///     Glonass Navigation Message subframe.
///     <p>This function is called from GN_GPS_Update(), but only if neither a
///     current valid Broadcast or A-GLONASS ephemeris is available internally.
/// \attention
///     This extended ephemeris data will stop being used if a valid broadcast
///     ephemeris or A-GLONASS ephemeris becomes available.
/// \returns
///     Flag to indicate whether the requested extended ephemeris is provided.
/// \retval #TRUE if extended ephemeris is provided.
/// \retval #FALSE if extended ephemeris is not available.
BL GN_EE_Get_GLON_Eph_El(
   U1 SV,                           ///< [in] Glonass Satellite Slot number [range 1..24].
   U2 NT,                           ///< [in] Current Glonass date [range 1..1461 days within 4 year period].
   U4 TOD_s,                        ///< [in] Current Glonass Time of Day (UTC SU) [range 0..86399 s].
   s_GN_EE_GLON_Eph_El *p_Eph_El    ///< [in] Pointer to destination Glonass Extended Ephemeris data Elements
);


//*****************************************************************************
/// \brief
///     GN EE Function that provides a new Glonass Broadcast or A-GLONASS
///     Assistance Ephemeris to the Extended Ephemeris provider.
/// \details
///     GN EE Function that provides a new Glonass Broadcast or A-GLONASS
///     Assistance Ephemeris to the Extended Ephemeris provider so that it can
///     be used to aid the generation of future extended ephemeris data.
///     <p> The Ephemeris data elements fields are scaled as they appear in the
///     broadcast Glonass Navigation Message subframe.
///     <p>This function is called from GN_GPS_Update() when a new Glonass
///     Broadcast or A-Glonass Assistance Ephemeris becomes available internally.
/// \returns
///     Flag to indicate whether setting the ephemeris was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_EE_Set_GLON_Eph_El(
   s_GN_EE_GLON_Eph_El *p_Eph_El    ///< [in] Pointer to source GLONASS Extended Ephemeris data Elements
 );


//*****************************************************************************
/// \brief
///      GN EE Function that performs all the GN Extended Ephemeris Software
///      Shutdown activities at the GNSS sub-system Shutdown.
/// \details
///      GN EE Function that performs all the GN Extended Ephemeris Software
///      Shutdown activities at the GPS sub-system Shutdown.
///      This API function is called near the end of GN_GPS_Shutdown().
/// \returns
///      None.
void GN_EE_Shutdown( void );


//*****************************************************************************

/// \}

#ifdef __cplusplus
   }     // extern "C"
#endif

#endif   // GN_EE_API_H
