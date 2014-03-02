
//****************************************************************************
// GPS IP Centre, ST-Ericsson (UK) Ltd.
// Copyright (c) 2009 ST-Ericsson (UK) Ltd.
// 15-16 Cottesbrooke Park, Heartlands Business Park, Daventry, NN11 8YL, UK.
// All rights reserved.
//
// Filename  GN_AGPS_api.h
//
// $Header: S:/GN_49xxPE_MKS/49xxPE_API/rcs/GN_AGPS_api.h 1.9 2011/11/09 13:06:41Z geraintf Rel $
// $Locker: $
//****************************************************************************

#ifndef GN_AGPS_API_H
#define GN_AGPS_API_H

#ifdef __cplusplus
   extern "C" {
#endif

//****************************************************************************
// File level documentation
/// \file
/// \brief
///      GNSS PE Core Library - A-GPS external interface API header file.
//
/// \ingroup  group_GN_AGPS
//
//-----------------------------------------------------------------------------
/// \defgroup  GN_AGPS_api  GNNS PE Core Library - Internally Implemented A-GPS API
//
/// \brief
///     Assisted-GPS API definitions for functions implemented internally.
//
/// \details
///     GNSS PE Core Library API definitions for the A-GPS structures and
///     functions provided in the library and can be called by the Host
///     software (ie inward called) to exchange the fundamental A-GPS data
///     items.
///     <p> These GN_AGPS_??? API functions must not be called before #GN_GPS_Initialise().
///     <p> The input GN_AGPS_Set_??? API functions must only be called when
///     the GPS is awake,  ie not between a GN_GPS_Sleep() and a GN_GPS_WakeUp().
///     <p> The output GN_AGPS_Get_??? API functions may be called when the GPS
///     is asleep, but if done so will return stale data.
//
//*****************************************************************************

#include "gps_ptypes.h"

//*****************************************************************************
/// \addtogroup GN_AGPS_api
/// \{

//*****************************************************************************
/// \brief
///     GN A-GPS GAD (Geographical Area Description) Position & Velocity Data.
/// \details
///     GN A-GPS GAD (Geographical Area Description) position & velocity data
///     already scaled according to the GAD (Geographical Area Description)
///     format used in the SUPL, RRLP and RRC A-GPS protocols.
/// \note
///     See 3GPP TS 23.032 for a full description of the GAD fields.
typedef struct //GN_AGPS_GAD_Data
{
   U1 latitudeSign;           ///< Latitude sign [0=North, 1=South]
   U4 latitude;               ///< Latitude      [range 0..8388607 for 0..90 degrees]
   I4 longitude;              ///< Longitude     [range -8388608..8388607 for -180..+180 degrees]
   I1 altitudeDirection;      ///< Altitude Direction  [0=Height, 1=Depth, -1=Altitude not present]
   U2 altitude;               ///< Altitude            [range 0..32767 metres]
   U1 uncertaintySemiMajor;   ///< Horizontal Uncertainty Semi-Major Axis [range 0..127, 255=Unknown]
   U1 uncertaintySemiMinor;   ///< Horizontal Uncertainty Semi-Minor Axis [range 0..127, 255=Unknown]
   U1 orientationMajorAxis;   ///< Orientation of the Semi-Major Axis [range 0..89 x 2 degrees, 255=Unknown]
   U1 uncertaintyAltitude;    ///< Altitude Uncertainty  [range 0..127,  255=Unknown]
   U1 confidence;             ///< Position Confidence   [range 1..100%, 255=Unknown]

   I1 verdirect;              ///< Vertical direction [0=Upwards, 1=Downwards, -1=Not present]
   I2 bearing;                ///< Bearing            [range 0..359 degrees,   -1=Not present]
   I2 horspeed;               ///< Horizontal Speed   [range 0..32767 km/hr,   -1=Not present]
   I1 verspeed;               ///< Vertical Speed     [range 0..127 km/hr,     -1=Not present]
   U1 horuncertspeed;         ///< Horizontal Speed Uncertainty [range 0..254 km/hr, 255=Unknown]
   U1 veruncertspeed;         ///< Vertical Speed Uncertainty   [range 0..254 km/hr, 255=Unknown]

}  s_GN_AGPS_GAD_Data;        // GN A-GPS GAD (Geographical Area Description)


//*****************************************************************************
/// \brief
///     GN A-GPS Required "Quality of Position" criteria.
/// \details
///     GN A-GPS "Quality of Position" criteria required from a position fix.
///     <p> The Accuracy terms are scaled according to the GAD (Geographical Area
///     Description) format used in the SUPL, RRLP and RRC A-GPS protocols.
///     <p> The time-out by which a Position Fix is required has been converted
///     to a "Deadline" OS Time [milliseconds] which is compatible with that
///     returned by the host implemented API function GN_GPS_Get_OS_Time_ms().
///     In effect, the user is saying 'We have a maximum of x ms to get a fix. Give
///     me the fix when you have one which meets the required accuracy, but if
///     the time-out has been reached, give me the fix even if it might not meet
///     the required accuracy'. If there is no 'deadline' time then the
///     'Deadline_OS_Time_ms' parameter should be set to 0.
/// \note
///     See 3GPP TS 23.032 for a full description of the GAD fields.
/// \attention
///     The specified Deadline OS Time by which a Position Fix is required must
///     contain a sufficient margin so that the GN_AGPS Library does not have to
///     add any further margin.

typedef struct  // GN_AGPS_QoP
{
   U1 Horiz_Accuracy;         ///< Horizontal Accuracy Required [range 0..127, 255=Unknown]
                              ///  A value of 0 will result in the full time-out available
                              ///  being taken (provided that 'Deadline_OS_Time_ms' is not 0).
                              //   This is intended to be used only for test purposes.
   U1 Vert_Accuracy;          ///< Vertical   Accuracy Required [range 0..127, 255=Unknown]
   U4 Deadline_OS_Time_ms;    ///< "Position required by" deadline OS Time [ms].
                              ///  Set to 0 if there is no 'deadline' time

}  s_GN_AGPS_QoP;             // GN A-GPS Required "Quality of Position" criteria


//*****************************************************************************
/// \brief
///     GN A-GPS Satellite Measurement data Elements for one satellite.
/// \details
///     GN A-GPS Satellite Measurement data Elements for one satellite as included
///     in the GN A-GPS Satellite Measurement Report structure #s_GN_AGPS_Meas
///     used to provide the data output required for cellular A-GPS MS-Assisted
///     mode via the RRLP or RRC protocols.
typedef struct //GN_AGPS_Meas_El
{
   U1 SatID;                  ///< Satellite ID (PRN) Number  [range 1..32]
   U1 SNR;                    ///< Satellite Signal to Noise Ratio [range 0..63 dBHz]
   I2 Doppler;                ///< Satellite Doppler [range -32768..32767 x 0.2 Hz]
   U2 Whole_Chips;            ///< Satellite Code Phase Whole Chips [range 0..1022 chips]
   U2 Fract_Chips;            ///< Satellite Code Phase Fractional Chips [range 0..1023 x 2^-10 chips]
   U1 MPath_Ind;              ///< \brief  Pseudorange Multipath Error Indicator [range 0..3], representing
                              ///          { Not measured, Low (<5m), Medium (<43m), High (>43m) }
   U1 PR_RMS_Err;             ///< \brief  Pseudorange RMS Error [range 0..63] consisting of a 3 bit Mantissa 'x' & 3 bit Exponent 'y',
                              ///<         where RMS Error =  0.5 * (1 + x/8) * 2^y  metres.

} s_GN_AGPS_Meas_El;          // GN A-GPS Measurement Elements for one SV

/// \brief
///     GN A-GPS Satellite Measurement Report data.
/// \details
///     GN A-GPS Satellite Measurement Report data output required for cellular
///     A-GPS MS-Assisted mode via the RRLP or RRC protocols.
///     <p> A Measurement set "Quality" metric is provided in the range 0 (lowest)
///     to 255 (highest).  If the quality drops then the caller should consider
///     staying with the previous set obtained (eg because, the user might have
///     just walked into a building and the signal levels suddenly dropped).
///     <p>If the Measurement set Quality is reported as the maximum of 255, then
///     the caller should not wait for anything better.  If the Measurement set
///     Quality reduces the earlier measurement set should be used.
/// \attention
///     The Measurement GPS Time of Week is reported as accurately as has been
///     determined by the GPS, and may only be as accurate as the A-GPS Reference
///     Time provide (eg within +/- 2 seconds).  Therefore, it is assumed that the
///     Location Server's Positioning Engine is capable of resolving the correct
///     GPS Time of Week if required.
typedef struct  // GN_AGPS_Meas
{
   U4 Meas_GPS_TOW;           ///< Measurement GPS Time of Week [range 0..604799999 ms]
   U1 Meas_GPS_TOW_Unc;       ///< Measurement GPS Time of Week Uncertainty (K) [range 0..127], where Uncertainty =  0.0022*((1.18^K) - 1) [microseconds]
   I1 Delta_TOW;              ///< \brief  Difference in milliseconds [range 0..127] between Meas_GPS_TOW and the millisecond part of the SV time tsv_1
                              ///<         of the first entry in the Satellite Measurement Elements array.  See 3GPP TS 44.031 for a full description.
                              ///<         This field is set to -1 = "Unknown" if the correct millisecond difference is not known exactly.
   I4 EFSP_dT_us;             ///< Delta-Time [us] to the last received External Frame Sync Pulse, 0x7FFFFFFF = No pulse received.
   U1 Num_Meas;               ///< Number of measurement elements to follow [range 0..16].
   U1 Quality;                ///< A Quality metric [range 0 (lowest) .. 255 (highest)] for this message set.
   s_GN_AGPS_Meas_El Meas[16];///< Satellite Measurement Elements array.

} s_GN_AGPS_Meas;             // GN A-GPS Measurement report


//*****************************************************************************
/// \brief
///     GN A-GPS Assistance Data Requirements.
/// \details
///     GN A-GPS Assistance data requirements details which can used to
///     specify which A-GPS Assistance items should be requested via SUPL,
///     RRLP, or RRC protocols as appropriate.
/// \note
///     This API details which assistance data items should be "requested" in
///     order to achieve optimal A-GPS performance.
///     The structure elements #Approx_Time_Known, #Approx_Pos_Known, #Ion_Known,
///     #UTC_Known and #Num_Sat_Alm provide further details on what information
///     the GN GPS already has, but they may not be sufficiently accurate or up
///     to date to provide optimal A-GPS performance.  These may be used by the
///     integrator to prevent repeated assistance data requests if the server
///     has not provided everything that was requested in the recent past.
/// \attention
///     The terms #gpsWeek, #Toe, #Toe_Limit, #Num_Sat, #SatID and #IODE can only be
///     specified if the GPS is navigating and only an ephemeris top up is being
///     requested.  If this is not the case then they will all be zero.
typedef struct  // GN_AGPS_Assist_Req
{
   BL Ref_Time_Req;           ///< Is Reference Time assistance required ?
   BL Ref_Pos_Req;            ///< Is Reference Position assistance required ?
   BL Ion_Req;                ///< Is Ionospheric Model Parameter assistance required ?
   BL UTC_Req;                ///< Is UTC Model Parameter assistance required ?
   BL SV_Health_Req;          ///< Is Satellite Health Status assistance required ?
   BL Bad_SV_List_Req;        ///< Is Bad Satellite List assistance required ?
   BL Alm_Req;                ///< Is Satellite Almanac Data Assistance required ?
   BL Eph_Req;                ///< Is Satellite Ephemeris Data Assistance  required ?

   U2 gpsWeek;                ///< Ephemeris target week [range 0..1023 weeks]
   U1 Toe;                    ///< Ephemeris target Toe  [range 0..167 hours]
   U1 Toe_Limit;              ///< Age Limit on the Ephemeris Toe [range 0..10]
   U1 Num_Sat;                ///< Number of Satellites in use [range 0..32]
   U1 SatID[32];              ///< Satellite ID's in use [range 1..32]
   U1 IODE[32];               ///< Ephemeris IODE for the satellites in use [range 0.255]

   BL Approx_Time_Known;      ///< Is Approximate Time Known to better than +/- 3s ?
   BL Approx_Pos_Known;       ///< Is Approximate Position Known to better than +/- 30km ?
   BL Ion_Known;              ///< Is Ionosphere model data Known (but it may not be up to date) ?
   BL UTC_Known;              ///< Is UTC model data Known (but it may not be up to date) ?
   U1 Num_Sat_Alm;            ///< Number of Satellites with valid Almanacs (but it may not be up to date) ?
   U1 Num_Sat_Eph;            ///< Number of Satellites with valid Ephemeris (but it may not be up to date) ?

}  s_GN_AGPS_Assist_Req;      // GN A-GPS Assistance Data Requirements


//*****************************************************************************
/// \brief
///     GN A-GPS satellite Ephemeris subframe data
/// \details
///     The GPS satellite ephemeris and clock data is broadcast in subframes 1, 2 & 3
///     of the GPS Navigation Message.
///     Each subframe contains 10 x 30 bits words, and each word as 24 data bits
///     and 6 parity bits.
///     <p> The ephemeris data in this structure is in this format, but only the 20
///     subframe words that contain data of interest are included, and only for
///     the 24 data bits which are stored as 3 adjacent bytes.
/// \note
///     See the ICD-GPS-200 for a full description of the Navigation Message format.
/// \attention
///     If the Z-Count is not known then it should be set to zero.
typedef struct  // GN_AGPS_Eph
{
   U4 ZCount;                 ///< \brief  Reference Z-Count the Ephemeris Subframe was decoded at.
                              ///          (See \ref Note_ZCount "Note 1").

   U1 word[4+8+8][3];         ///< \brief  GPS Ephemeris & Clock Binary subframe words as follows:
                              ///           - 4 x 24 bit words, from Subframe 1 words 3 & 8-10.
                              ///           - 8 x 24 bit words, from Subframe 2 words 3-10.
                              ///           - 8 x 24 bit words, from Subframe 3 words 3-10.

}  s_GN_AGPS_Eph;             // GN A-GPS Satellite Ephemeris Subframe Data


//*****************************************************************************
/// \brief
///     GN A-GPS satellite Ephemeris data Elements.
/// \details
///     GN A-GPS satellite Ephemeris data Elements scaled as they are in the
///     broadcast GPS Navigation Message.
/// \attention
///     If the GPS #Week number is not known (eg when obtained from either an
///     RRLP or RRC Navigation Model message) then it should be set to zero.
/// \attention
///     If the full GPS #Week number is known, then the correct N x 1024
///     Week ambiguity should be included.
/// \attention
///     It is common that #toc = #toe. In some formats only one of these
///     terms is given, in which case it can be set equal to the other.
/// \note
///     See the ICD-GPS-200 for a full description of the Navigation Message format.
typedef struct  // GN_AGPS_Eph_El
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

}  s_GN_AGPS_Eph_El;          // GN A-GPS satellite Ephemeris data Elements.


//*****************************************************************************
/// \brief
///     GN A-GPS satellite Almanac subframe data.
/// \details
///     GN A-GPS satellite Almanac subframe data (see ICD-GPS-200).
///     <p> The GPS satellite almanac data is broadcast in either subframe 4 (pages 2-10)
///     or subframe 5 (pages 1-24) of the GPS Navigation Message.
///     Each subframe contains 10 x 30 bits words, and each word as 24 data bits
///     and 6 parity bits.
///     <p> The almanac data in this structure is in this format, but only the 8
///     subframe words (8-10) that contain data of interest are included, and only
///     for the 24 data bits which are stored as 3 adjacent bytes.
/// \note
///     See the ICD-GPS-200 for a full description of the Navigation Message format.
/// \attention
///     If the Z-Count is not known then it should be set to zero.
typedef struct  // GN_AGPS_Alm
{
   U4 ZCount;                 ///< Reference Z-Count the Almanac Subframe was decoded at.  (See \ref Note_ZCount "Note 1").

   U2 WeekNo;                 ///< Almanac Reference Week.  If known, including the GPS 1024 week rollovers.

   U1 word[8][3];             ///< GPS Almanac, 8 x 24 bit words, from subframe words 3-10.

}  s_GN_AGPS_Alm;             // GN A-GPS Satellite Almanac data


//*****************************************************************************
/// \brief
///     GN A-GPS satellite Almanac data Elements.
/// \details
///     GN A-GPS satellite Almanac data Elements scaled as they are in the
///     broadcast GPS Navigation Message.
/// \note
///     See the ICD-GPS-200 for a full description of the Navigation Message format.
typedef struct  // GN_AGPS_Alm_El
{
   U1 WNa;                    ///< Almanac Reference Week             :  8 bits  [range 0..255]
   U1 SatID;                  ///< Satellite ID                       :  6 bits  [range 1..32]
   U1 SVHealth;               ///< Satellite Health Bits              :  8 bits  [range 0..255]
   U1 toa;                    ///< Reference Time of Week             :  8 bits  [range 0..148 x 2^12 seconds]
   I2 af0;                    ///< SV Clock Bias                      : 11 bits  [range -1024..1023 x 2^-20 seconds]
   I2 af1;                    ///< SV Clock Drift                     : 11 bits  [range -1024..1023 x 2^-38 sec/sec]
   U2 e;                      ///< Eccentricity                       : 16 bits  [range 0..65536 x 2^-21]
   I2 delta_I;                ///< Delta_Inclination Angle            : 16 bits  [range -32768..32767 x 2^-19 semi-circles]
   I2 OmegaDot;               ///< Rate of Right Ascension            : 16 bits  [range -32768..32767 x 2^-38 semi-circles/sec]
   U4 APowerHalf;             ///< (Semi-Major Axis)^1/2              : 24 bits  [range 0..16777216 x 2^-11 meters^1/2]
   I4 Omega0;                 ///< Longitude of the Ascending Node    : 24 bits  [range -8388608..8388607 x 2^-23 semi-circles]
   I4 w;                      ///< Argument of Perigee                : 24 bits  [range -8388608..8388607 x 2^-23 semi-circles]
   I4 M0;                     ///< Mean Anomaly                       : 24 bits  [range -8388608..8388607 x 2^-23 semi-circles]

} s_GN_AGPS_Alm_El;           // GN A-GPS satellite Almanac data Elements.


//*****************************************************************************
/// \brief
///     GN A-GPS Klobuchar Ionospheric delay model parameters.
/// \note
///     See the ICD-GPS-200 for a full description of theses parameters and the
///     Klobuchar model used to compute the signal path delay through the ionosphere.
/// \attention
///     If the Z-Count is not known then it should be set to zero.
typedef struct  // GN_AGPS_Ion
{
   U4 ZCount;                 ///< Reference Z-Count the Almanac Subframe was decoded at.  (See \ref Note_ZCount "Note 1").

   I1 a0;                     ///< Klobuchar - alpha 0   : 8 bits  [range -128..127 x 2^-30      seconds]
   I1 a1;                     ///< Klobuchar - alpha 1   : 8 bits  [range -128..127 x 2^-27/PI   seconds/semi-circle]
   I1 a2;                     ///< Klobuchar - alpha 2   : 8 bits  [range -128..127 x 2^-24/PI^2 seconds/semi-circle^2]
   I1 a3;                     ///< Klobuchar - alpha 3   : 8 bits  [range -128..127 x 2^-24/PI^3 seconds/semi-circle^3]
   I1 b0;                     ///< Klobuchar - beta 0    : 8 bits  [range -128..127 x 2^11       seconds]
   I1 b1;                     ///< Klobuchar - beta 1    : 8 bits  [range -128..127 x 2^14/PI    seconds/semi-circle]
   I1 b2;                     ///< Klobuchar - beta 2    : 8 bits  [range -128..127 x 2^16/PI^2  seconds/semi-circle^2]
   I1 b3;                     ///< Klobuchar - beta 3    : 8 bits  [range -128..127 x 2^16/PI^3  seconds/semi-circle^3]

}  s_GN_AGPS_Ion;             // GN A-GPS Klobuchar Ionospheric model parameters


//*****************************************************************************
/// \brief
///     GN A-GPS UTC Correction model parameters.
/// \details
///     GN A-GPS UTC (Universal Time Coordinated) Correction model parameters.
/// \note
///     See the ICD-GPS-200 for a full description of theses parameters and the
///     algorithms used to compute the (GPS-UTC) time zone differences.
/// \attention
///     If the Z-Count is not known then it should be set to zero.
typedef struct  // GN_AGPS_UTC
{
   U4 ZCount;                 ///< Reference Z-Count the Almanac Subframe was decoded at.  (See \ref Note_ZCount "Note 1").

   I4 A1;                     ///< UTC model - parameter A1               [x 2^-50 seconds/second]
   I4 A0;                     ///< UTC model - parameter A0               [x 2^-30 seconds]
   U1 Tot;                    ///< UTC model - reference time of week     [x 2^12 seconds]
   U1 WNt;                    ///< UTC model - reference week number      [weeks]
   I1 dtLS;                   ///< UTC model - time difference due to leap seconds before event  [seconds]
   U1 WNLSF;                  ///< UTC model - week number when next leap second event occurs    [weeks]
   U1 DN;                     ///< UTC model - day of week when next leap second event occurs    [days]
   I1 dtLSF;                  ///< UTC model - time difference due to leap seconds after event   [seconds]

}  s_GN_AGPS_UTC;             // GN A-GPS UTC Correction data


//*****************************************************************************
/// \note
/// \anchor Note_ZCount
/// 1.  U4 ZCount:<p>
///     <p> ZCount is used in the #s_GN_AGPS_Eph::ZCount, #s_GN_AGPS_Alm::ZCount, #s_GN_AGPS_Ion::ZCount
///     and #s_GN_AGPS_UTC::ZCount structures and when available can be used by the GN GPS
///     Library to correctly time-out the corresponding input assistance data.
///     <p> The 19 least significant bits of the Z-Count can be decoded
///     from the HOW (Handover Word) which is Word 2 of every Subframe.
///     This gives the GPS Time of Week in units of 1.5 seconds.
///     <p> The full 29 bit Z-Count has the 10-bit GPS Week Number as the 10 most
///     significant bits.  (See GPS-ICD-200 for a full description).
///     <p> If the GPS Week Number portion is not known then GPS Time of Week
///     portion should still be input.
///     <p> If neither portion are known then ZCount should be input as zero.<p>


//*****************************************************************************
/// \brief
///     GN A-GPS Reference GPS Time.
/// \details
///     GN A-GPS Reference GPS Time corresponding to a given OS Time tick as
///     obtained via a call to #GN_GPS_Get_OS_Time_ms.
/// \attention
///     It is important that the Reference GPS Time RMS Accuracy is representative
///     of the actual errors in the Reference Time.
/// \attention
///     Setting the RMS Accuracy too small could result in satellites not being found.
/// \attention
///     Setting the RMS Accuracy too big could result in a longer Time-To-First-Fix.
typedef struct  // GN_AGPS_Ref_Time
{
   U4 OS_Time_ms;             ///< OS Time [milliseconds] when the Reference GPS Time was obtained.
   U4 TOW_ms;                 ///< Reference GPS Time of Week [milliseconds].
   U2 WeekNo;                 ///< Reference GPS Week Number.  If known, including the GPS 1024 week rollovers.
   U2 RMS_ms;                 ///< Reference GPS Time RMS Accuracy [milliseconds]

}  s_GN_AGPS_Ref_Time;        // GN A-GPS Reference GPS Time


//*****************************************************************************
/// \brief
///     GN A-GPS External Frame Sync Pulse Time corresponding to the last
///     synchronisation pulse input to the EXT_FRM_SYNC pin of the GN Baseband.
/// \details
///     This API allows a precise (typically sub-millisecond) time tag to be
///     input to the GPS that is synchronised with the GPS baseband hardware.
///     <p> This time-tag allows narrow satellite C/A Code search windows to be
///     computed, which can in turn result in improved sensitivity and reduced
///     Time To First Fix.
/// \attention
///     It is important that the External Frame Sync Pulse Time RMS Absolute and
///     Relative Accuracies are representative of the actual errors in the External
///     Frame Sync Pulse Time provided.
///     Setting the RMS Accuracy too small could result in satellites not being found.
///     Setting the RMS Accuracy too big could result in a longer Time-To-First-Fix.
/// \attention
///     If the Absolute Time is not known then the time provided in the TOW
///     and WeekNo fields may start at any value but then must increment in the
///     correct units thereafter and wrap correctly at 604800.0 seconds.
/// \attention
///     The 'MultiplePulses' flag is important, since it's used to decide if
///     it is necessary to try to make sure that the time data is associated with
///     the correct pulse.  If only a single pulse and message are provided, or if
///     the interval between each pulse is greater than a few seconds, then there
///     is very little danger of the wrong set of time data being associated with
///     the pulse.
typedef struct  // GN_AGPS_EFSP_Time
{
   R8 TOW;                    ///< External Frame Sync Pulse GPS Time of Week [0.0 to 6047999.999999999 seconds]
   U2 WeekNo;                 ///< External Frame Sync Pulse GPS Week Number.  If known, including the GPS 1024 week rollovers
   U4 Abs_RMS_us;             ///< External Frame Sync Pulse Absolute Time RMS Accuracy [microseconds], 0xFFFFFFFF = Absolute Time is not known
   U4 Rel_RMS_ns;             ///< External Frame Sync Pulse Relative Time RMS Accuracy [nanoseconds],  0xFFFFFFFF = Relative Time to a previous recent pulse should not be used
   U4 OS_Time_ms;             ///< OS Time [milliseconds] corresponding to the approximate time of the pulse
   BL MultiplePulses;         ///< Do we expect to see multiple pulses and messages?

}  s_GN_AGPS_EFSP_Time;       // GN A-GPS External Frame Sync Pulse Time


//*****************************************************************************
/// \brief
///     GN A-GPS Time Of Week Assistance data Elements for one GPS satellite.
/// \details
///     GN A-GPS Time Of Week Assistance data Elements for one GPS satellite
///     as included in the GN A-GPS Time Of Week Assistance data structure
///     #s_GN_AGPS_TOW_Assist used to provide the unknown data bits in the
///     GPS Satellite Navigation Message Subframe Words 1 & 2.
///     These data elements are consistent with those provided in the A-GPS
///     RRLP or RRC protocols.
typedef struct  // GN_AGPS_TOW_Assist_El
{
   U1 SatID;                  ///< Satellite ID (PRN) Number  [range 1..32]
   U1 TLM_Reserved;           ///< Satellite Subframe Word 1, Telemetry Message Reserved 2 bits  [range 0..3]
   U2 TLM_Word;               ///< Satellite Subframe Word 1, Telemetry Message Word 14 bits  [range 0..16383]
   U1 Anti_Spoof_flag;        ///< Satellite Subframe Word 2, Anti-Spoof flag  [range 0..1]
   U1 Alert_flag;             ///< Satellite Subframe Word 2, Alert flag  [range 0..1]

}  s_GN_AGPS_TOW_Assist_El;   // GN A-GPS Time of Week Assistance Elements

/// \brief
///     GN A-GPS Time Of Week Assistance data.
/// \details
///     GN A-GPS Time Of Week Assistance data which can be used by the GPS to
///     help identify the start of a Satellite Navigation Message Subframe and
///     hence accurately determine the local GPS Time of Week.
typedef struct  // GN_AGPS_TOW_Assist
{
   U4 TOW_ms;                 ///< Reference GPS Time of Week [milliseconds]
   U1 Num_TOWA;               ///< Number of TOW Assist Elements to follow  [range 1..24]
   s_GN_AGPS_TOW_Assist_El TOWA[24];///< Per Satellite TOW Assistance Elements

}  s_GN_AGPS_TOW_Assist;      // GN A-GPS Time of Week Assistance Data


//*****************************************************************************
/// \brief
///     GN A-GPS reference Clock Frequency calibration [ppb].
/// \details
///     GN A-GPS reference Clock (TCXO) Frequency calibration [ppb].
/// \attention
///     It is important that the Clock Frequency Calibration RMS Accuracy is
///     representative of the actual errors in the Clock Frequency Calibration.
/// \attention
///     Setting the RMS Accuracy too small could result in satellites not being found.
/// \attention
///     Setting the RMS Accuracy too big could result in a longer Time-To-First-Fix.
typedef struct  // GN_AGPS_ClkFreq
{
   U4 OS_Time_ms;             ///< OS Time [milliseconds] when the Clock Frequency Calibration was performed.
   R4 Calibration;            ///< Clock Frequency Calibration value [ppb]
   U4 RMS_ppb;                ///< Clock Frequency Calibration RMS Accuracy [ppb]

}  s_GN_AGPS_ClkFreq;         // GN A-GPS Clock Frequency Calibration data


//*****************************************************************************
/// \brief
///     GN A-GPS WGS84 geodetic Reference Position data.
/// \attention
///     It is important that the reference position component Accuracy is
///     representative of the actual errors in the reference Position.
/// \attention
///     Setting the Accuracy too small could result in satellites not being found.
/// \attention
///     Setting the RMS Accuracy too big could result in a longer Time-To-First-Fix.
typedef struct  // GN_AGPS_Ref_Pos
{
   R4 Latitude;               ///< WGS84 Geodetic Latitude  [degrees]
   R4 Longitude;              ///< WGS84 Geodetic Longitude [degrees]
   R4 RMS_SMaj;               ///< Horizontal Accuracy (1-sigma error ellipse Semi-Major axis) [metres]
   R4 RMS_SMin;               ///< Horizontal Accuracy (1-sigma error ellipse Semi-Minor axis) [metres]
   I2 RMS_SMajBrg;            ///< Horizontal error ellipse Semi-Major axis Bearing [deg]
   BL Height_OK;              ///< Is the Height component available & OK to use ?
   R4 Height;                 ///< WGS84 Geodetic Ellipsoidal Height [metres]
   R4 RMS_Height;             ///< Height RMS Accuracy [metres]

}  s_GN_AGPS_Ref_Pos;         // GN A-GPS WGS84 geodetic reference Position data.


//*****************************************************************************
/// \brief
///     GN A-GPS Satellite Acquisition Assistance data Elements for one satellite.
/// \details
///     GN A-GPS Satellite Acquisition Assistance data Elements for one satellite
///     as included in the GN A-GPS Satellite Acquisition Assistance data
///     structure #s_GN_AGPS_Acq_Ass used to provide the input data required for
///     cellular A-GPS MS-Assisted mode via the RRLP or RRC protocols.
/// \attention
///     If #Doppler_1 and #Doppler_Unc are unknown (ie they were not provided)
///     then they should both be set to zero.
///     If #Azimuth and #Elevation are unknown (ie they were not provided)
///     then they should both be set to zero.
typedef struct  // GN_AGPS_AA_El
{
   U1 SatID;                  ///< Satellite ID (PRN) Number  [range 1..32]
   I2 Doppler_0;              ///< Satellite Doppler 0th order term [range -2048..2047 x 2.5 Hz]
   U1 Doppler_1;              ///< Satellite Doppler 1st order term [range 0..63 x 1/42 Hz/s], offset by -42 to give values in the range -1.0 .. 0.5 Hz
   U1 Doppler_Unc;            ///< Satellite Doppler Uncertainty [range 0..4], representing +/- { <=200, <=100, <=50, <=25, <=12.5 } Hz
   U2 Code_Phase;             ///< Satellite Code Phase [range 0..1022 chips]
   U1 Int_Code_Phase;         ///< Satellite Integer Code Phase since the last GPS bit edge boundary [range 0..19 ms]
   U1 GPS_Bit_Num;            ///< Satellite GPS Bit Number (modulo 4) relative to #s_GN_AGPS_Acq_Ass::Ref_GPS_TOW
   U1 Code_Window;            ///< \brief  Satellite Code Phase Search Window [range 0..15], representing
                              ///          +/- { 512, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 196 } chips
   U1 Azimuth;                ///< Satellite Azimuth   [range 0..31 x 11.25 degrees]
   U1 Elevation;              ///< Satellite Elevation [range 0..7  x 11.25 degrees]

} s_GN_AGPS_AA_El;            // GN A-GPS Acquisition Assistance Elements for one SV

//*****************************************************************************
/// \brief
///     GN A-GPS Satellite Acquisition Assistance data.
/// \details
///     GN A-GPS Satellite Acquisition Assistance data used to provide the input
///     data required for cellular A-GPS MS-Assisted mode via the RRLP or RRC
///     protocols.
typedef struct  // GN_AGPS_Acq_Ass
{
   U4 Ref_GPS_TOW;            ///< Acquisition Assistance Reference GPS Time of Week [range 0..604799999 ms]
   U1 Num_AA;                 ///< Number of Acquisition Assistance elements to follow [range 0..16]
   s_GN_AGPS_AA_El AA[16];    ///< Satellite Acquisition Assistance Elements array

} s_GN_AGPS_Acq_Ass;          // GN A-GPS Acquisition Assistance data set



//*****************************************************************************
//
// GNSS PE Core Library  -  A-GPS External Interface API function prototypes.
//
// API Functions implemented by the GNSS Core PE Library and may be called by
// the Host platform software.
//
//*****************************************************************************

//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Get the latest GAD (Geographical Area
///     Description) format Position and Velocity data.
/// \details
///     GN A-GPS API Function to Get the latest GAD (Geographical Area
///     Description) format Position and Velocity data already pre-scaled
///     for use the SUPL, RRLP or RRC A-GPS protocols.
/// \note
///     See 3GPP TS 23.032 for a full description of the GAD fields.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether new GAD Position and Velocity data is available.
/// \retval #TRUE if new GAD Position and Velocity data based on a GPS fix is available.
/// \retval #FALSE if new GAD Position and Velocity data based on a GPS fix is not available.
BL GN_AGPS_Get_GAD_Data(
   U4 *Prev_OS_Time_ms,       ///< [in/out]  OS Time [milliseconds] for the previous data got, to avoid being given the same data again.  A NULL pointer input disables this feature.
   U4 *GAD_Ref_TOW,           ///< [out] GAD Data Reference GPS Time of Week [range 0..604799999 ms]
   I2 *GAD_Ref_TOW_Subms,     ///< [out] GAD Data Reference GPS Time of Week Sub-millisecond part [range 0..9999 x 0.0001 ms,  -1 = Unknown]
   I4 *EFSP_dT_us,            ///< [out] Delta-Time [us] to the last received External Frame Sync Pulse, 0x7FFFFFFF = No pulse received.
   s_GN_AGPS_GAD_Data *p_GAD  ///< [out] Pointer to the output GAD Data.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to "Qualify" the current Position fix against given
///     "Quality of Position" criteria.
/// \details
///     GN A-GPS API Function to determine if the current published position fix
///     can be treated as "Qualified" with respect to the given "Quality of
///     Position" criteria, or the given "Deadline" time has been reached and
///     a response is required now.
///     <p> The time-out by which a Position Fix is required has been converted
///     to a "Deadline" OS Time [milliseconds] which is compatible with that
///     returned by the host implemented API function GN_GPS_Get_OS_Time_ms().
///     In effect, the user is saying 'We have a maximum of x ms to get a fix. Give
///     me the fix when you have one which meets the required accuracy, but if
///     the time-out has been reached, give me the fix even if it might not meet
///     the required accuracy'. If there is no 'deadline' time then the
///     'Deadline_OS_Time_ms' parameter should be set to 0.
///     <p> If both the Horizontal and Vertical Accuracy QoP are "Unspecified"
///     then the library internal default values will be used.  However, if the
///     Horizontal Accuracy QoP is specified but the Vertical Accuracy QoP is
///     not, then only the Horizontal component will be qualified.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether the "Quality of Position" or "Deadline" criteria were met.
/// \retval #TRUE if the current position fix meets the "Quality of Position" or "Deadline" criteria.
/// \retval #FALSE if the current position fix does not meet the "Quality of Position" or "Deadline" criteria.
BL GN_AGPS_Qual_Pos(
   U4* p_Prev_OS_Time_ms,     ///< [in/out] Pointer to the OS Time  [milliseconds] for the previous data got,
                              ///           to avoid being given the same data again.
                              ///           A NULL pointer input disables this feature.
   s_GN_AGPS_QoP* p_GAD_QoP   ///< [in] Required "GAD Quality of Position" criteria.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Get Measurement Report data.
/// \details
///     GN A-GPS API Function to Get Measurement Report data for cellular A-GPS
///     MS-Assisted mode via the RRLP or RRC protocols.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether Measurement Report data is available.
/// \retval #TRUE if Measurement Report data is available.
/// \retval #FALSE if Measurement Report data is not available.
BL GN_AGPS_Get_Meas(
   U4 *Prev_OS_Time_ms,       ///< [in/out]  OS Time [milliseconds] for the previous data got, to avoid being given the same data again.  A NULL pointer input disables this feature.
   s_GN_AGPS_Meas* p_Meas     ///< [in] Pointer to where the GPS Library can get the Measurement data from.
 );


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Get the GPS Library's A-GPS Assistance data
//      requirements.
/// \details
///     GN A-GPS API Function to Get details of the GPS Library's A-GPS
///     Assistance data requirements.
///     <p> If a NULL pointer is provided as the input argument then this API
///     will return TRUE as soon as any assistance data requirement is found.
/// \attention
///     This A-GPS API function will continue to report that a particular A-GPS
///     Assistance data item is required until either it has been supplied via a
///     GN_AGPS_Set_??? API input, or it has been determined internally by the GPS.
/// \attention
///     The caller should take note of what data was requested and what data could
///     was not supplied by the A-GPS server, so as to prevent repeated requests
///     to the A-GPS server for a data item that will never come.
///     The GN AGPS Library does not know whether the caller is asking on behalf
///     of the same, or a different A-GPS server, as a previous caller.
///     Therefore, the GN AGPS Library takes no note of data previously requested
///     but not supplied, but just keeps asking for anything it would still like
///     to receive.
/// \attention
///     If any large data item is requested (eg Ephemerides, Almanacs) then a
///     top-up of the smaller data will also be requested (eg Time, Position,
///     Ionosphere, UTC, SV Health, etc) as they will only be a small part of the
///     overall data packet.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether new A-GPS Assistance Data is required.
/// \retval #TRUE if new A-GPS Assistance Data is required.
/// \retval #FALSE if new A-GPS Assistance Data is not required.
BL GN_AGPS_Get_Assist_Req(
   s_GN_AGPS_Assist_Req *p_AR ///< [out] A-GPS Assistance Requirements.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) Ephemeris Subframe data for the
///     specified GPS satellite.
/// \details
///     GN A-GPS API Function to Set (ie input) Ephemeris Subframe data for the
///     specified GPS satellite PRN.
/// \note
///     For cellular Control Plane implementations the GN_AGPS_Set_Eph_El()
///     API provides a better match to the RRLP and RRC message content.
/// \attention
///     This ephemeris data will only be used if a valid current broadcast
///     ephemeris has not recently been obtained from the satellite itself.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the ephemeris was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_Eph(
   U1 SV,                     ///< [in] Satellite identification (PRN) number [range 1..32].
   s_GN_AGPS_Eph* p_Eph       ///< [in] Pointer to where the GPS Library can get the Ephemeris data from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Get Ephemeris Subframe data for the specified
///     GPS satellite.
/// \details
///     GN A-GPS API Function to Get Ephemeris Subframe data for the specified
///     GPS satellite PRN.
/// \note
///     For cellular Control Plane implementations the GN_AGPS_Get_Eph_El()
///     API provides a better match to the RRLP and RRC message content.
/// \attention
///     The ephemeris data returned is what is currently being used internally
///     and could have recently been decoded from a live satellite signal.
///     This means that calling this API immediately after calling
///     #GN_AGPS_Set_Eph() may not return the ephemeris data just input.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether retrieving the ephemeris was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
BL GN_AGPS_Get_Eph(
   U1 SV,                     ///< [in] Satellite identification (PRN) number [range 1..32].
   s_GN_AGPS_Eph* p_Eph       ///< [out] Pointer to where the Host software can get the Ephemeris data from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) Ephemeris data elements for the
///     specified GPS satellite.
/// \details
///     GN A-GPS API Function to Set (ie input) Ephemeris data elements for the
///     specified GPS satellite PRN.
///     <p> The data fields are scaled as they appear in the broadcast GPS
///     Navigation Message subframe, and is also the same scaling as used in the
///     RRLP and RRC A-GPS protocols.
/// \attention
///     This ephemeris data will only be used if a valid current broadcast
///     ephemeris has not recently been obtained from the satellite itself.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the ephemeris was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_Eph_El(
   s_GN_AGPS_Eph_El* p_Eph_El ///< [in] Pointer to where the GPS Library can get the Ephemeris data from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Get Ephemeris data Elements for the specified
///     GPS satellite.
/// \details
///     GN A-GPS API Function to Get Ephemeris data Elements for the specified
///     GPS satellite PRN.
///     <p> The data fields are scaled as they appear in the broadcast GPS
///     Navigation Message subframe, and is also the same scaling as used in the
///     RRLP and RRC A-GPS protocols.
/// \attention
///     The ephemeris data returned is what is currently being used internally
///     and could have recently been decoded from a live satellite signal.
///     This means that calling this API immediately after calling
///     #GN_AGPS_Set_Eph_El() may not return the ephemeris data just input.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether retrieving the ephemeris was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
BL GN_AGPS_Get_Eph_El(
   U1 SV,                     ///< [in]  Satellite identification (PRN) number [range 1..32].
   s_GN_AGPS_Eph_El* p_Eph_El ///< [out] Pointer to where the Host software can get the Ephemeris data from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) Almanac Subframe data for the
///     specified GPS satellite.
/// \details
///     GN A-GPS API Function to Set (ie input) Almanac Subframe data for the
///     specified GPS satellite PRN.
/// \note
///     For cellular Control Plane implementations the GN_AGPS_Set_Alm_El()
///     API provides a better match to the RRLP and RRC message content.
/// \attention
///     This almanac data will only be used if a valid current broadcast
///     almanacs has not recently been obtained from the satellite itself.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the almanac was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_Alm(
   U1 SV,                     ///< [in] Satellite identification (PRN) number [range 1..32].
   s_GN_AGPS_Alm* p_Alm       ///< [in] Pointer to where the GPS Library can get the Almanac data from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Get Almanac Subframe data for the specified
///     GPS satellite.
/// \details
///     GN A-GPS API Function to Get Almanac Subframe data for the specified
///     GPS satellite PRN.
/// \note
///     For cellular Control Plane implementations the GN_AGPS_Get_Alm_El()
///     API provides a better match to the RRLP and RRC message content.
/// \attention
///     The almanac data returned is what is currently being used internally
///     and could have recently been decoded from a live satellite signal.
///     This means that calling this API immediately after calling
///     #GN_AGPS_Set_Alm() may not return the ephemeris data just input.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether retrieving the almanac was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
BL GN_AGPS_Get_Alm(
   U1 SV,                     ///< [in]  Satellite identification (PRN) number [range 1..32].
   s_GN_AGPS_Alm* p_Alm       ///< [out] Pointer to where the GPS Library is to put the Almanac data.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) Almanac data Elements for the
///     specified GPS satellite.
/// \details
///     GN A-GPS API Function to Set (ie input) Almanac data Elements for the
///     specified GPS satellite PRN.
///     <p> The data fields are scaled as they appear in the broadcast GPS
///     Navigation Message subframe, and is also the same scaling as used in the
///     RRLP and RRC A-GPS protocols.
/// \attention
///     This almanac data will only be used if a valid current broadcast
///     almanacs has not recently been obtained from the satellite itself.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the almanac was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_Alm_El(
   s_GN_AGPS_Alm_El* p_Alm_El ///< [in] Pointer to where the GPS Library can get the Almanac data from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Get Almanac data Elements for the specified
///     GPS satellite.
/// \details
///     GN A-GPS API Function to Get Almanac data Elements for the specified
///     GPS satellite PRN.
///     <p> The data fields are scaled as they appear in the broadcast GPS
///     NavigationMessage subframe, and is also the same scaling as used in the
///     RRLP and RRC A-GPS protocols.
/// \attention
///     The almanac data returned is what is currently being used internally
///     and could have recently been decoded from a live satellite signal.
///     This means that calling this API immediately after calling
///     #GN_AGPS_Set_Alm_El() may not return the ephemeris data just input.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether retrieving the almanac was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
BL GN_AGPS_Get_Alm_El(
   U1 SV,                     ///< [in]  Satellite identification (PRN) number [range 1..32].
   s_GN_AGPS_Alm_El* p_Alm_El ///< [out] Pointer to where the GPS Library is to put the Almanac data.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) the 4 Ephemeris Reserved Words
///     from Subframe 1, words 4,5,6 & 7 for the specified satellite.
/// \details
///     Eph_Res[0] = 23 bits from Subframe 1, Word 4, bits 0..22
///     <p> Eph_Res[1] = 24 bits from Subframe 1, Word 5, bits 0..23
///     <p> Eph_Res[2] = 24 bits from Subframe 1, Word 6, bits 0..23
///     <p> Eph_Res[3] = 16 bits from Subframe 1, Word 7, bits 8..23
/// \attention
///     These Ephemeris Reserved Words will only be used if a valid current
///     broadcast version have not recently been obtained from one of the
///     tracked satellites.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the Ephemeris Reserved Words was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_Eph_Res(
   U1 SV,                     ///< [in] Satellite identification (PRN) number [range 1..32].
   U4 Eph_Res[4]              ///< [in] Ephemeris Reserved Words array.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Get (ie output) the 4 Ephemeris Reserved Words
///     from Subframe 1, words 4,5,6 & 7 for the specified satellite.
/// \details
///     Eph_Res[0] = 23 bits from Subframe 1, Word 4, bits 0..22
///     <p> Eph_Res[1] = 24 bits from Subframe 1, Word 5, bits 0..23
///     <p> Eph_Res[2] = 24 bits from Subframe 1, Word 6, bits 0..23
///     <p> Eph_Res[3] = 16 bits from Subframe 1, Word 7, bits 8..23
/// \attention
///     The Ephemeris Reserved Words returned is what is currently being used
///     internally and could have recently been decoded from a live satellite signal.
///     This means that calling this API immediately after calling
///     #GN_AGPS_Set_Eph_Res() may not return the Ephemeris Reserved Words just input.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether retrieving the Ephemeris Reserved Words was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
BL GN_AGPS_Get_Eph_Res(
   U1 SV,                     ///< [in]  Satellite identification (PRN) number [range 1..32].
   U4 Eph_Res[4]              ///< [out] Ephemeris Reserved Words array.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) Klobuchar Ionospheric delay
///     model parameters.
/// \details
///     The data fields are scaled as they appear in the broadcast GPS Navigation
///     Message subframe, and is also the same scaling as used in the RRLP and
///     RRC A-GPS protocols.
/// \attention
///     These Ionospheric delay model parameters will only be used if a valid
///     current broadcast version have not recently been obtained from one of
///     the tracked satellites.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the Ionospheric delay model was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_Ion(
   s_GN_AGPS_Ion* p_Ion       ///< [in] Pointer to where the GPS Library can get the Ionosphere model parameters from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Get Klobuchar Ionospheric delay model parameters.
/// \details
///     The data fields are scaled as they appear in the broadcast GPS Navigation
///     Message subframe, and is also the same scaling as used in the RRLP and
///     RRC A-GPS protocols.
/// \attention
///     The Ionospheric delay mode parameters returned is what is actually being
///     used internally and could have recently been decoded from a live satellite
///     signal. This means that calling this API immediately after calling
///     #GN_AGPS_Set_Ion() may not return the Ionosphere parameters just input.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether retrieving the Ionospheric delay model was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
BL GN_AGPS_Get_Ion(
   s_GN_AGPS_Ion* p_Ion       ///< [in] Pointer to where the Host software can get the Ionosphere model parameters from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) UTC Correction model parameters.
/// \details
///     The data fields are scaled as they appear in the broadcast GPS Navigation
///     Message subframe, and is also the same scaling as used in the RRLP and
///     RRC A-GPS protocols.
/// \attention
///     These UTC Correction model parameters will only be used if a valid
///     current broadcast version have not recently been obtained from one of
///     the tracked satellites.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the UTC Correction model was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_UTC(
   s_GN_AGPS_UTC* p_UTC       ///< [in] Pointer to where the GPS Library can get the UTC Correction model parameters from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Get UTC Correction model parameters.
/// \details
///     The data fields are scaled as they appear in the broadcast GPS Navigation
///     Message subframe, and is also the same scaling as used in the RRLP and
///     RRC A-GPS protocols.
/// \attention
///     The UTC Correction model parameters returned are what is currently being
///     used internally and could have recently been decoded from a live satellite
///     signal.  This means that calling this API immediately after calling
///     #GN_AGPS_Set_UTC() may not return the UTC Correction model parameters just input.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether retrieving the UTC Correction model was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
BL GN_AGPS_Get_UTC(
   s_GN_AGPS_UTC* p_UTC       ///< [in] Pointer to where the Host software can get the UTC Correction model parameters from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) GPS Satellite Health data.
/// \details
///     GN A-GPS API Function to Set (ie input) GPS Satellite Health data as a
///     bit mask for the full 32 satellite constellation.
/// \attention
///     This input is treated like the other sources of satellite health data
///     in that it only takes effect until a newer source of satellite health
///     data is received, either off-air from the GPS satellites, or via an
///     A-GPS API input.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the GPS Satellite Health data was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_SV_Health(
   U4 SV_Health_Mask          ///< [in] Satellite Health bit Mask, where bits 0..31 are for SV ID's 1..32, and 1=Healthy, 0=Unhealthy.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Get GPS Satellite Health data.
/// \details
///     GN A-GPS API Function to Get GPS Satellite Health data as a bit mask
///     for the full 32 satellite constellation.
/// \attention
///     The GPS Satellite Health data returned are what is currently being used
///     internally and could have recently been decoded from a live satellite
///     signal.  This means that calling this API immediately after calling
///     #GN_AGPS_Set_SV_Health() may not return the GPS Satellite Health data
///     recently input.
/// \attention
///     This A-GPS API function is not re-entrant and must be called
///     in the same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether retrieving the GPS Satellite Health data was
///     successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
BL GN_AGPS_Get_SV_Health(
   U4 *SV_Health_Mask         ///< [in] Satellite Health bit Mask, where bits 0..31 are for SV ID's 1..32, and 1=Healthy, 0=Unhealthy.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) a list of "Bad" satellites.
/// \details
///     GN A-GPS API Function to Set (ie input) a list of "Bad" satellites
///     which should not be used as part of the GPS Navigation Solution.
/// \note
///     Each new Bad Satellite List provided via this API supercedes any
///     previous list provided.   Therefore, and empty list implicitly
///     sets all the satellites to be "Not known to be Bad".  As such, this
///     list cannot be used to enable the use of a satellite which is marked
///     as "Unhealthy" by the GPS constellation.
/// \note
///     In this list Satellite ID PRN 32 should be specified as 32, not 0.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the Bad Satellite List was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_Bad_SV_List(
   U1 Num_Bad_SV,             ///< [in] Number of "Bad" satellites in the list.
   U1 Bad_SV_List[]           ///< [in] List of "Bad" Satellites ID (PRN) Numbers [range 1..32].
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) the GPS Reference Time.
/// \details
///     GN A-GPS API Function to Set (ie input) the GPS Reference Time as the
///     GPS Week Number and Time of Week.
/// \attention
///     If possible the A-GPS Reference Time should be input before any of the
///     other A-GPS assistance data inputs as this ensures the other inputs
///     will be correctly time-tagged and, therefore, will time-out correctly.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the GPS Reference Time was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_Ref_Time(
   s_GN_AGPS_Ref_Time* p_RTime   ///< [in] Pointer to where the GPS Library can get the Reference GPS Time data from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) the precise GPS Time
///     corresponding to the last External Frame Sync Pulse input to the GN
///     Baseband EXT_FRAME_SYNC pin.
/// \details
///     GN A-GPS API Function to Set (ie input) the precise GPS Time
///     input to the GN corresponding to the leading edge of the last External Frame Sync Pulse
///     Baseband EXT_FRAME_SYNC pin.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the precise GPS Time was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_EFSP_Time(
   s_GN_AGPS_EFSP_Time* p_EFSPT  ///< [in] Pointer to where the GPS Library can get the External Frame Sync Pulse GPS Time from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) the GPS Time of Week Assistance
///     Data.
/// \details
///     GN A-GPS API Function to Set (ie input) the GPS Time of Week Assistance
///     Data which can be used by the GPS to help identify the start of a
///     Satellite Navigation Message Subframe and hence accurately determine
///     the local GPS Time of Week.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the GPS Time of Week Assistance Data
///     was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_TOW_Assist(
   s_GN_AGPS_TOW_Assist *p_TOWA  ///< [in] Pointer to where the GPS Library can get the GPS Time of Week Assistance data from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) GPS Reference Clock (TCXO)
///     Frequency Calibration data.
/// \details
///     GN A-GPS API Function to Set (ie input) GPS Reference Clock (TCXO)
///     Frequency Calibration offset [ppb] and it's associated uncertainty.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the GPS Reference Clock Calibration was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_ClkFreq(
   s_GN_AGPS_ClkFreq* p_ClkFreq  ///< [in] Pointer to where the GPS Library can get the Clock Frequency Calibration data from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) a WGS84 geodetic Reference Position.
/// \details
///     GN A-GPS API Function to Set (ie input) a WGS84 geodetic Reference Position
///     and it's associated uncertainty.
/// \note
///     For cellular Control Plane implementations the GN_AGPS_Set_GAD_Ref_Pos()
///     API provides a better match to the RRLP and RRC message content.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the WGS84 geodetic reference Position was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_Ref_Pos(
   s_GN_AGPS_Ref_Pos* p_RPos  ///< [in] Pointer to where the GPS Library can get the WGS84 Reference Position from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) a GAD format WGS84 Reference Position.
/// \details
///     GN A-GPS API Function to Set (ie input) a WGS84 Reference Position
///     with fields scaled according to the GAD (Geographical Area
///     Description) format used in the SUPL, RRLP and RRC A-GPS protocols.
///     <p>This functions does the GAD format translation and then calls
///     #GN_AGPS_Set_Ref_Pos().
/// \note
///     See 3GPP TS 23.032 for a full description of the GAD fields.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the GAD format Reference Position was accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_GAD_Ref_Pos(
   s_GN_AGPS_GAD_Data* p_RPos ///< [in] Pointer to where the GPS Library can get the GAD format Reference Position from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Set (ie input) Acquisition Assistance data.
/// \details
///     GN A-GPS API Function to Set (ie input) Acquisition Assistance data
///     for cellular A-GPS MS-Assisted mode via the RRLP or RRC protocols.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether setting the Acquisition Assistance data was
///     accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_AGPS_Set_Acq_Ass(
   s_GN_AGPS_Acq_Ass* p_AA    ///< [in] Pointer to where the GPS Library can get the Acquisition Assistance data from.
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to record the fact that a response has been
///     returned to the server.
/// \details
///     The only result of this function being called is that additional
///     information is recorded in the GPS debug logs. That is, the purpose of
///     this function is to help improve the diagnostics capability. It should
///     be called whenever a 2G "Measure Position Response" or 3G "Measurement
///     Report" is returned to the server. It should also be called whenever
///     a "Period Reporting" response to sent to the server.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     None.
void GN_AGPS_Mark_Response_Sent( void );


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Start the Frequency Aiding process.
/// \details
///     GN A-GPS API Function to Start the Frequency Aiding process where by a
///     known calibration frequency in the range 100 Hz to 10.0 MHz is input on
///     the EXT_REF_CLK pin over a period of at least 2-3 seconds, but preferably
///     for 4-8 seconds.
///     <p> Typically this calibration frequency is generated by the cellular modem
///     which is frequency locked to the network.
///     <p> There is a corresponding function to Stop the Frequency Aiding process,
///     #GN_AGPS_Stop_Freq_Aiding().
/// \note
///     The frequency calibration computed with this process is a single average
///     value measured over the aiding period, and degrades in accuracy with age.
///     <p> If the GPS Reference XO can change relatively quickly (eg due to
///     temperature changes) then it is beneficial to re-start the frequency
///     aiding process until a fix is obtained (eg every 4 seconds).
/// \attention
///     The calibration frequency must already be enabled on the EXT_REF_CLK pin
///     before this A-GPS API function is called.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     Flag to indicate whether the Frequency Aiding process was successfully
///     started.
/// \retval #TRUE if Frequency Aiding was started.
/// \retval #FALSE if Frequency Aiding was not started.
BL GN_AGPS_Start_Freq_Aiding(
   R8 Freq,                   ///< [in] Known frequency of the input pulses [MHz] [range 0.0001 .. 10.0 MHz]
   U4 RMS_ppb                 ///< [in] Uncertainty (RMS value) of the known calibration frequency [ppb].
);


//*****************************************************************************
/// \brief
///     GN A-GPS API Function to Stop the Frequency Aiding process.
/// \details
///     GN A-GPS API Function to Stop the Frequency Aiding process where by a
///     known calibration frequency in the range 100 Hz to 10.0 MHz is input on
///     the EXT_REF_CLK pin over a period of at least 2-3 seconds.
///     <p> Typically this calibration frequency is generated by the cellular modem
///     which is frequency locked to the network.
///     <p> There is a corresponding function to Start the Frequency Aiding process,
///     #GN_AGPS_Start_Freq_Aiding().
/// \note
///     The frequency calibration computed with this process is a single average
///     value measured over the aiding period, and degrades in accuracy with age.
///     <p> If the GPS Reference XO can change relatively quickly (eg due to
///     temperature changes) then it is beneficial to re-start the frequency
///     aiding process until a fix is obtained (eg every 4 seconds).
/// \attention
///     This A-GPS API function must be called while the input pulses are still enabled.
///     On return from this function, the host may then disable the pulses.
/// \attention
///     This A-GPS API function must not be called when the GPS is asleep, ie
///     in-between a #GN_GPS_Sleep() and #GN_GPS_WakeUp() API calls.
/// \attention
///     This A-GPS API function is not re-entrant and must be called in the
///     same task/thread context as function GN_GPS_Update().
/// \returns
///     None.
void GN_AGPS_Stop_Freq_Aiding( void );


//*****************************************************************************

/// \}

#ifdef __cplusplus
   }     // extern "C"
#endif

#endif   // GN_AGPS_API_H
