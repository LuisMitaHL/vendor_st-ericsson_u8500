
//****************************************************************************
// GPS IP Centre, ST-Ericsson (UK) Ltd.
// Copyright (c) 2011 ST-Ericsson (UK) Ltd.
// 15-16 Cottesbrooke Park, Heartlands Business Park, Daventry, NN11 8YL, UK.
// All rights reserved.
//
// Filename  GN_AGNSS_api.h
//
// $Header: S:/GN_49xxPE_MKS/49xxPE_API/rcs/GN_AGNSS_api.h 1.4 2012/02/29 17:24:25Z geraintf Rel $
// $Locker: $
//****************************************************************************

#ifndef GN_AGNSS_API_H
#define GN_AGNSS_API_H

#ifdef __cplusplus
   extern "C" {
#endif

//****************************************************************************
// File level documentation
/// \file
/// \brief
///      GNSS PE Core Library - A-GNSS external interface API header file.
//
/// \ingroup  group_GN_AGNSS
//
//-----------------------------------------------------------------------------
/// \defgroup  GN_AGNSS_api  GNSS PE Core Library - Internally Implemented A-GNSS API
//
/// \brief
///     Assisted-GNSS API definitions for functions implemented internally.
//
/// \details
///     GNSS PE Core Library API definitions for the Assisted-GNSS structures and
///     functions provided in the library and can be called by the Host
///     software (ie inward called) to exchange the fundamental Assisted-GNSS data
///     items.
///     <p> These GN_AGNSS_??? Assisted-GNSS API's are not supported in CG2900.
///     <p> These GN_AGNSS_??? API functions must not be called before #GN_GPS_Initialise().
///     <p> The input GN_AGNSS_Set_??? API functions must only be called when
///     the GPS is awake,  ie not between a GN_GPS_Sleep() and a GN_GPS_WakeUp().
///     <p> The output GN_AGNSS_Get_??? API functions may be called when the GPS
///     is asleep, but if done so will return stale data.
//
//*****************************************************************************

#include "gps_ptypes.h"

//*****************************************************************************
/// \addtogroup GN_AGNSS_api
/// \{

//*****************************************************************************
/// \brief
///     GN A-GNSS Positioning Method (satellite constellation id).
/// \details
///     GN A-GNSS Positioning Method definition of a set IDs for the various
///     GNSS satellite constellations.
/// \note
///     A-GNSS API's are not supported in CG2900.
/// \note
///     Nomenclature was taken from 3GPP TS44.031 V9.1.0.
typedef enum GN_AGNSS_PM
{
   e_GN_AGNSS_PM_None      = 0,           ///< Positioning Method = None
   e_GN_AGNSS_PM_GPS       = 0x1 << 0,    ///< Positioning Method = GPS
   e_GN_AGNSS_PM_Galileo   = 0x1 << 1,    ///< Positioning Method = Galileo
   e_GN_AGNSS_PM_SBAS      = 0x1 << 2,    ///< Positioning Method = SBAS
   e_GN_AGNSS_PM_ModGPS    = 0x1 << 3,    ///< Positioning Method = Modernised GPS
   e_GN_AGNSS_PM_QZSS      = 0x1 << 4,    ///< Positioning Method = QZSS
   e_GN_AGNSS_PM_Glonass   = 0x1 << 5     ///< Positioning Method = Glonass

}  e_GN_AGNSS_PM;


//*****************************************************************************
/// \brief
///     GN A-GNSS satellite Ephemeris Keplerian data Elements.
/// \details
///     GN A-GNSS satellite Ephemeris Keplerian data Elements.
///     <p> The data fields are scaled as they appear in the broadcast GPS
///     Navigation Message subframe, and is also the same scaling as used in the
///     RRLP and RRC A-GLONASS protocols.
/// \note
///     A-GNSS API's are not supported in CG2900.
/// \note
///     See the GPS ICD-GPS-200 for a full description of the GPS Navigation
///     Message format.
/// \attention
///     If the GPS #Week number is not known (eg when obtained from either an
///     RRLP or RRC Navigation Model message) then it should be set to zero.
/// \attention
///     If the full GPS #Week number is known, then the correct N x 1024
///     Week ambiguity should be included.
/// \attention
///     It is common that #toc = #toe. In some formats only one of these
///     terms is given, in which case it can be set equal to the other.
typedef struct //GN_AGNSS_Eph_El
{
   U1 SatID;                  ///< Satellite ID (PRN)                       :  6 bits [range 1..32 GPS, 1..24 Glonass, 120..138 SBAS, 193..197 QZSS]
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

}  s_GN_AGNSS_Eph_El;         // GN A-GNSS satellite Ephemeris data Elements.



//*****************************************************************************
//
// GNSS PE Core Library  -  A-GNSS External Interface API function prototypes.
//
// API Functions implemented by the GNSS Core PE Library and may be called by
// the Host platform software.
//
//*****************************************************************************

//*****************************************************************************
/// \brief
///     GN A-GNSS API Function to Set (ie input) Ephemeris data elements for the
///     specified GNSS satellite.
/// \details
///     GN A-GNSS API Function to Set (ie input) Ephemeris data elements for the
///     specified GNSS satellite.
///     <p> The data fields are scaled as they appear in the broadcast GPS
///     Navigation Message subframe, and is also the same scaling as used in the
///     RRLP and RRC A-GLONASS protocols.
/// \note
///     A-GNSS API's are not supported in CG2900.
/// \attention
///     This ephemeris data will only be used if a valid current broadcast
///     ephemeris has not recently been obtained from the satellite itself.
/// \attention
///     This A-GNSS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GNSS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the ephemeris was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGNSS_Set_Eph_El(
   e_GN_AGNSS_PM       sv_Type,     ///< [in] SV Type (constellation ID)
   U1                  sv_Id,       ///< [in] SV Id within constellation (PRN, Slot number) [range 1..32 GPS, 1..24 Glonass, 120..138 SBAS, 193..197 QZSS]
   s_GN_AGNSS_Eph_El  *p_Eph_El     ///< [in] Pointer to the source Ephemeris data
);


//*****************************************************************************

/// \}

#ifdef __cplusplus
   }     // extern "C"
#endif

#endif   // GN_AGNSS_API_H
