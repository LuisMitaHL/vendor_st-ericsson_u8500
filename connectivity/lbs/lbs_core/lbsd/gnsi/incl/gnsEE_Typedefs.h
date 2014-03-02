/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *
 *****************************************************************************/

#ifndef GNS_EE_TYPEDEFS_H
#define GNS_EE_TYPEDEFS_H


/*! \addtogroup Location_gnsEE_Constants */
/*@{*/

#define K_gnsEE_MAX_CONSTANT_NUM_GPS_PRNS 32
#define K_gnsEE_MAX_CONSTANT_NUM_GLONASS_PRNS 24


/**
* \enum e_gnsEE_ConstelType
* \brief enum to represent Satellite constellation Type
*/

typedef enum
{
    K_GNSEE_GPS_CONSTEL = 0,          /**< GPS constellation.*/
    K_GNSEE_GLO_CONSTEL               /**< GLONASS constellation. */
} e_gnsEE_ConstelType;


/**
* \enum e_gnsEE_LocationType
* \brief enum to represent Reference location Type
*/

typedef enum /** e_gnsEE_LocationType */
{
    GNSEE_LBS_LOC_UNSET = 0,       /**< Mask for Reference Positioning Capabilities */
    GNSEE_LBS_LOC_ECEF,       /**< Mask for Server Based Extended Ephemeris Capabilities */
    GNSEE_LBS_LOC_LLA,           /**< Mask for Self Asssited Extended Ephemeris Capabilities */
    GNSEE_LBS_LOC_BOTH           /**< Mask for Broad Cast Ephemeris Cpabilities */

} e_gnsEE_LocationType;

/*@}*/



/*! \addtogroup Location_gnsEE_Structures */
/*@{*/


/**
* \struct t_agps_GPSExtendedEphData
* \brief GPS Extended Ephemeris data from Client
*/
typedef struct
{
    uint8_t         v_Prn;        /*!< Ephemeris PRN or SV. Range 1-32.*/
    uint8_t         v_CAOrPOnL2;    /*!< Only 2 least sig bits used. Not in RXN std ephemeris struct. */
    uint8_t         v_Ura;        /*!< User Range Accuracy index.  See IS-GPS-200 Rev D for index values.*/
    uint8_t         v_Health;     /*!< Corresponds to the SV health value. 6 bits as described within IS-GPS-200 Rev D.*/
    uint8_t         v_L2PData;      /*!< When 1 indicates that the NAV data stream was commanded OFF on the P-code of the L2 channel. Descrete 1/0. */
    uint8_t         v_Ure;        /*!< User Range Error. Indicates EE accuracy. Units: meters.*/
    uint8_t         v_AODO;           /*!< Age Of Data Offset.*/
    int8_t          v_TGD;          /*!< Time Group Delay. Scale: 2^-31. Units: seconds.*/
    int8_t          v_Af2;        /*!< Clock Drift Rate Correction Coefficient. Scale: 2^-55. Units: sec/sec^2. */
    int8_t          v_EphemFit;  /*!< Fit interval relating to the fit interval flag. Typically 4 hrs. */
    uint16_t        v_GpsWeek;     /*!< Extended week number (i.e. > 1024, e.g.1486). */
    uint16_t        v_IoDc;            /*!< Issue Of Data (Clock). */
    uint16_t        v_Toc;         /*!< Time Of Clock or time of week. Scale: 2^4. Units: seconds. */
    uint16_t        v_Toe;         /*!< Time Of Ephemeris. Scale: 2^4. Units: seconds. */
    uint16_t        v_Iode;         /*!< Issue Of Data (Ephemeris). */
    int16_t         v_Af1;        /*!< Clock Drift Correction Coefficient. Scale: 2^-43. Units: sec/sec. */
    int16_t         v_Crs;        /*!< Amplitude of the Sine Harmonic Correction Term to the Orbit Radius. Scale: 2^-5. Units: meters. */
    int16_t         v_DeltaN;    /*!< Mean Motion Difference from Computed Value. Scale: 2^-43. Units: semi-circles/sec. */
    int16_t         v_Cuc;        /*!< Amplitude of the Cos Harmonic Correction Term to the Arguement of Latitude. Scale: 2^-29. Units: radians. */
    int16_t         v_Cus;        /*!< Amplitude of the Sine Harmonic Correction Term to the Argument of Latitude. Scale: 2^-29. Units: radians. */
    int16_t         v_Cic;        /*!< Amplitude of the Cos Harmonic Correction Term to the Angle of Inclination. Scale:    2^-29. Units: radians. */
    int16_t         v_Cis;        /*!< Amplitude of the Sine Harmonic Correction Term to the Angle of Inclination. Scale: 2^-29. Units: radians. */
    int16_t         v_Crc;        /*!< Amplitude of the Cos Harmonic Correction Term to the Orbit Radius. Scale: 2^-5. Units: meters. */
    int16_t         v_IDot;        /*!< Rate of Inclination Angle. Scale: 2^-43. Units: semi-circles/sec. */
    uint32_t        v_E;          /*!< Eccentricity. Scale: 2^-33. Units: N/A - Dimensionless. */
    uint32_t        v_SqrtA;     /*!< Square Root of the Semi-Major Axis. Scale: 2^-19. Units: square root of meters. */
    int32_t         v_Af0;        /*!< Clock Bias Correction Coefficient. Scale: 2^-31. Units: sec. */
    int32_t         v_M0;         /*!< Mean Anomaly at Reference Time. Scale: 2^-31/PI. Units: semi-circles. */
    int32_t         v_Omega0;     /*!< Longitude of Ascending Node of Orbit Plane and Weekly Epoch. Scale: 2^-31/PI. Units: semi-circles. */
    int32_t         v_I0;         /*!< Inclination Angle at Reference Time. Scale: 2^-31/PI. Units: semi-circles. */
    int32_t         v_W;            /*!< Argument of Perigee. Scale: 2^-31/PI. Units: semi-circles. */
    int32_t         v_OmegaDot;    /*!< Rate of Right Ascension. Scale: 2^-43. Units: semi-circles/sec. */

}s_gnsEE_GPSExtendedEphData;


/**
* \struct t_agps_GlonassExtendedEphData
* \brief Glonass Extended Ephemeris data from Client
*/
typedef struct
{
    uint8_t     v_Slot;         /*!< \brief Ephemeris Id for SV. Range 1-24.*/
    uint8_t     v_FT;           /*!< \brief User Range Accuracy index.  P32 ICD Glonass for value of Ft.*/
    uint8_t     v_M;            /*!< \brief Glonass vehicle type. M=1 means type M*/
    uint8_t     v_Bn;           /*!< \brief Bn SV health see p30 ICD Glonass. */
    uint8_t     v_UtcOffset;   /*!< \brief Current GPS-UTC leap seconds [sec]; 0 if unknown. */
    int8_t      v_FreqChannel;  /*!< \brief Freq slot: -7 to +13 incl. */
    int16_t     v_Gamma;        /*!< \brief SV clock frequency error ratio scale factor 2^-40 [seconds / second] */
    int16_t     v_Lsx;          /*!< \brief x luni solar accel scale factor 2^-30 Km/s^2 PZ90 datum. */
    int16_t     v_Lsy;          /*!< \brief y luni solar accel scale factor 2^-30 Km/s^2 PZ90 datum. */
    int16_t     v_Lsz;          /*!< \brief z luni solar accel scale factor 2^-30 Km/s^2 PZ90 datum. */
    uint32_t    v_GloSec;       /*!< \brief gloSec=[(N4-1)*1461 + (NT-1)]*86400 + tb*900, [sec] ie sec since Jan 1st 1996 <b>see caution note in struct details description</b> */
    int32_t     v_TauN;         /*!< \brief SV clock bias scale factor 2^-30 [seconds]. */
    int32_t     v_X;            /*!< \brief x position at toe scale factor 2^-11 Km PZ90 datum. */
    int32_t     v_Y;            /*!< \brief y position at toe scale factor 2^-11 Km PZ90 datum. */
    int32_t     v_Z;            /*!< \brief z position at toe scale factor 2^-11 Km PZ90 datum. */
    int32_t     v_Vx;           /*!< \brief x velocity at toe scale factor 2^-20 Km/s PZ90 datum. */
    int32_t     v_Vy;           /*!< \brief y velocity at toe scale factor 2^-20 Km/s PZ90 datum. */
    int32_t     v_Vz;           /*!< \brief z velocity at toe scale factor 2^-20 Km/s PZ90 datum. */

}s_gnsEE_GlonassExtendedEphData;


/**
 * \struct s_gnsEE_NavDataList
 * \brief
 * Structure to store navigation data elements. Based on 3GPP TS 44.031.
 * Note that the Satellite ID value can be derived from the RXN_FullEph_t
 * prn value (Sat ID with range 0 - 31 = RXN_FullEph_t.prn - 1).
 */
typedef struct 
{
    uint8_t                             v_NumEntriesGps;                                   /*!< Number of entries in the list. Range 1 - 32. */
    uint8_t                             v_NumEntriesGlonass;                               /*!< Number of entries in the list. Range 1 - 24. */  
    s_gnsEE_GPSExtendedEphData          a_GpsList[K_gnsEE_MAX_CONSTANT_NUM_GPS_PRNS];                    /*!< RXN_FullEph_t elements. */
    s_gnsEE_GlonassExtendedEphData      a_GloList[K_gnsEE_MAX_CONSTANT_NUM_GLONASS_PRNS];        /*!< RXN_FullEph_GLO_t elements. */
} s_gnsEE_NavDataList;



/**
*  @struct t_gnsEE_NavDataBCE
*  @brief Structure contains the broadcast Ephemeris data to be fed to MSL.
*  @detail This include broadcast ephemeris for GPS & Glonass Satellite Status
*             along with type to identify the constellation.
*/
typedef struct
{
    e_gnsEE_ConstelType              v_ConsteType;
    s_gnsEE_GPSExtendedEphData       v_GpsBCE;                /*!< RXN_FullEph_t elements. */
    s_gnsEE_GlonassExtendedEphData   v_GloBCE;                /*!< RXN_FullEph_GLO_t elements. */
} t_gnsEE_NavDataBCE;



/**
* \struct t_gnsEE_ExtendedEphInd
* \brief structure to represent Extended Ephemeris request
*/
typedef struct {
    uint32_t                      v_PrnBitMask;
    e_gnsEE_ConstelType           v_ConstelType;
    uint32_t                      v_EERefGPSTime;
}t_gnsEE_ExtendedEphInd;




/**
* \struct t_gnsEE_RefPosData
* \brief structure to represent Reference Position Data
*/

typedef struct /** t_agpsEEClient_RefPosData */
{
    e_gnsEE_LocationType    v_Type;            /*!< Type. LocationType.*/
    float                   v_Latitude;          /*!< Value in degrees. */
    float                   v_Longitude;          /*!< Value in degrees. */
    float                   v_Altitude;          /*!< Value in meters. */
    int32_t                 v_ECEF_X;        /*!< Value in meters. */
    int32_t                 v_ECEF_Y;        /*!< Value in meters. */
    int32_t                 v_ECEF_Z;        /*!< Value in meters. */
    uint16_t                v_UncertSemiMajor; /*!< Uncertainty along the major axis in centimeters. */
    uint16_t                v_UncertSemiMinor; /*!< Uncertainty along the minor axis in centimeters. */
    uint16_t                v_OrientMajorAxis; /*!< Angle between major and north in degrees Range 0 - 180 (OPTIONAL). 0 - circle.*/
    uint8_t                 v_Confidence;      /*!< General indication of confidence. Range 0 - 100 (OPTIONAL). */

} t_gnsEE_RefPosData;

/**
* \struct t_gnsEE_RefPosition
* \brief structure to represent Reference Position Data member
*/

typedef struct /** t_gnsEE_RefPosition */
{
    t_gnsEE_RefPosData             v_RefPosData;

} t_gnsEE_RefPosition;



/*@}*/



#endif /* GNS_EE_TYPEDEFS_H*/

