/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
*
* EEClient.h
* Date: 22-12-2011
* Author: Saswata Roy
* Email : saswata.roy@stericsson.com
*/
/*!
 * \file     EEClient.h
 */

/*! @defgroup   EEClient   EEClient Interface
 *     @brief      Defines the interfaces between MSL Adaptation Layer Client and Location provider
 */


/*! @addtogroup EEClient
 *     @details
 *      Defines the interfaces between MSL Adaptation Layer Client and Location provider.
 *
 *
 */
/*@{*/
    /*!@defgroup EEClient_APIs        EEClient_APIs*/
    /*!@defgroup EEClient_InternalDef  EEClient_InternalDef*/
/*@}*/


#ifndef _EE_CLIENT_H
#define _EE_CLIENT_H

/*! \addtogroup EEClient_InternalDef */
/*@{*/


#ifndef DEBUG_LOG_PRINT

#define DEBUG_LOG_PRINT( X )                ALOGD X

#ifdef DEBUG_LOG_LEV2
#define DEBUG_LOG_PRINT_LEV2( X )   DEBUG_LOG_PRINT( X )
#else  /* DEBUG_LOG_LEV2 */
#define DEBUG_LOG_PRINT_LEV2( X )
#endif /* DEBUG_LOG_LEV2 */

#endif


/**
* \enum e_EEClient_ConstelType
* \brief enum to indicate the constellation from which to draw or
*  generate assistance
*/
typedef enum {
    EEClient_GPS_CONSTEL = 0,/*!< \brief NAVSTAR constellation.*/
    EEClient_GLO_CONSTEL      /*!< \brief GLONASS constellation.*/
}e_EEClient_ConstelType;


/**
* \enum e_EEClient_Status
* \brief enum to indicate the status of the confirmation for request
*/
typedef enum {
    EEClient_SUCESS,        /*!< \brief SUCCESS on Confirmation.*/
    EEClient_FAILURE        /*!< \brief FAILURE on Confirmation.*/
}e_EEClient_Status;


/**
*  \enum e_EEClient_SetCapabilitiesInd
*  \brief Structure contains the Capabilities EEClient is registered for.
*
*/
typedef enum /** e_EEClient_SetCapabilitiesInd */
{
    EEClient_LBS_REFPOS  = (1<< 7),   /**< Mask for Reference Positioning Capabilities */
    EEClient_LBS_SBEE    = (1<< 8),   /**< Mask for Server Based Extended Ephemeris Capabilities */
    EEClient_LBS_SAEE    = (1<< 9),   /**< Mask for Self Asssited Extended Ephemeris Capabilities */
    EEClient_LBS_BCE     = (1<< 10)    /**< Mask for Broad Cast Ephemeris Cpabilities */

} e_EEClient_SetCapabilitiesInd;

/**
*  \enum e_EEClient_LocationType
*  \brief Structure contains the Location Type .
*
*/
typedef enum /** e_EEClient_LocationType */
{
    EEClient_LBS_LOC_UNSET = 0,       /**< Mask for Reference Positioning Capabilities */
    EEClient_LBS_LOC_ECEF,       /**< Mask for Server Based Extended Ephemeris Capabilities */
    EEClient_LBS_LOC_LLA,           /**< Mask for Self Asssited Extended Ephemeris Capabilities */
    EEClient_LBS_LOC_BOTH           /**< Mask for Broad Cast Ephemeris Cpabilities */

} e_EEClient_LocationType;



/**
*  @brief  Defines the maximum number of GPS PRNs.@see t_EEClient_NavData
*/
#define EE_CONSTANT_NUM_GPS_PRNS  32


/**
*  @brief  Defines the maximum number of GLONASS PRNs.@see t_EEClient_NavData
*/
#define EE_CONSTANT_NUM_GLONASS_PRNS    24

/**
*  @brief  Defines the constant version string length. This strings defines the Lbs Software version
*           and the chip version.
*/
#define EECLIENT_CONSTANT_VERSION_STRING_LENGTH     255


/**
*  @struct t_EEClient_FullEph_Glonass
*  @brief Structure contains the Glonass specific Ephemeris data list.
*  @detail This include the additional Navigational data specific to Glonass.
*/
typedef struct /** t_EEClient_FullEph_Glonass */
{
    uint8_t slot;         /*!< \brief Ephemeris Id for SV. Range 1-24.*/
    uint8_t FT;           /*!< \brief User Range Accuracy index.  P32 ICD Glonass for value of Ft.*/
    uint8_t M;            /*!< \brief Glonass vehicle type. M=1 means type M*/
    uint8_t Bn;           /*!< \brief Bn SV health see p30 ICD Glonass. */
    uint8_t utc_offset;   /*!< \brief Current GPS-UTC leap seconds [sec]; 0 if unknown. */
    int8_t  freqChannel;  /*!< \brief Freq slot: -7 to +13 incl. */
    int16_t gamma;        /*!< \brief SV clock frequency error ratio scale factor 2^-40 [seconds / second] */
    int16_t lsx;          /*!< \brief x luni solar accel scale factor 2^-30 Km/s^2 PZ90 datum. */
    int16_t lsy;          /*!< \brief y luni solar accel scale factor 2^-30 Km/s^2 PZ90 datum. */
    int16_t lsz;          /*!< \brief z luni solar accel scale factor 2^-30 Km/s^2 PZ90 datum. */
    uint32_t gloSec;       /*!< \brief gloSec=[(N4-1)*1461 + (NT-1)]*86400 + tb*900, [sec] ie sec since Jan 1st 1996 <b>see caution note in struct details description</b> */
    int32_t tauN;         /*!< \brief SV clock bias scale factor 2^-30 [seconds]. */
    int32_t x;            /*!< \brief x position at toe scale factor 2^-11 Km PZ90 datum. */
    int32_t y;            /*!< \brief y position at toe scale factor 2^-11 Km PZ90 datum. */
    int32_t z;            /*!< \brief z position at toe scale factor 2^-11 Km PZ90 datum. */
    int32_t vx;           /*!< \brief x velocity at toe scale factor 2^-20 Km/s PZ90 datum. */
    int32_t vy;           /*!< \brief y velocity at toe scale factor 2^-20 Km/s PZ90 datum. */
    int32_t vz;           /*!< \brief z velocity at toe scale factor 2^-20 Km/s PZ90 datum. */
} t_EEClient_FullEph_Glonass;


/**
*  @struct t_EEClient_FullEph
*  @brief Structure contains the Full Ephemeris data list.
*  @detail This include the complete Navigational data.
*/
typedef struct /** t_EEClient_FullEph */
{
    uint8_t     prn;        /*!< Ephemeris PRN or SV. Range 1-32.*/
    uint8_t     CAOrPOnL2;  /*!< Only 2 least sig bits used. Not in RXN std ephemeris struct. */
    uint8_t     ura;        /*!< User Range Accuracy index.  See IS-GPS-200 Rev D for index values.*/
    uint8_t     health;     /*!< Corresponds to the SV health value. 6 bits as described within IS-GPS-200 Rev D.*/
    uint8_t     L2PData;      /*!< When 1 indicates that the NAV data stream was commanded OFF on the P-code of the L2 channel. Descrete 1/0. */
    uint8_t     ure;        /*!< User Range Error. Indicates EE accuracy. Units: meters.*/
    uint8_t     AODO;          /*!< Age Of Data Offset.*/
    int8_t     TGD;          /*!< Time Group Delay. Scale: 2^-31. Units: seconds.*/
    int8_t     af2;        /*!< Clock Drift Rate Correction Coefficient. Scale: 2^-55. Units: sec/sec^2. */
    int8_t     ephem_fit;  /*!< Fit interval relating to the fit interval flag. Typically 4 hrs. */
    uint16_t     gps_week;   /*!< Extended week number (i.e. > 1024, e.g.1486). */
    uint16_t     iodc;          /*!< Issue Of Data (Clock). */
    uint16_t     toc;        /*!< Time Of Clock or time of week. Scale: 2^4. Units: seconds. */
    uint16_t     toe;        /*!< Time Of Ephemeris. Scale: 2^4. Units: seconds. */
    uint16_t     iode;       /*!< Issue Of Data (Ephemeris). */
    int16_t     af1;        /*!< Clock Drift Correction Coefficient. Scale: 2^-43. Units: sec/sec. */
    int16_t     crs;        /*!< Amplitude of the Sine Harmonic Correction Term to the Orbit Radius. Scale: 2^-5. Units: meters. */
    int16_t     delta_n;    /*!< Mean Motion Difference from Computed Value. Scale: 2^-43. Units: semi-circles/sec. */
    int16_t     cuc;        /*!< Amplitude of the Cos Harmonic Correction Term to the Arguement of Latitude. Scale: 2^-29. Units: radians. */
    int16_t     cus;        /*!< Amplitude of the Sine Harmonic Correction Term to the Argument of Latitude. Scale: 2^-29. Units: radians. */
    int16_t     cic;        /*!< Amplitude of the Cos Harmonic Correction Term to the Angle of Inclination. Scale:  2^-29. Units: radians. */
    int16_t     cis;        /*!< Amplitude of the Sine Harmonic Correction Term to the Angle of Inclination. Scale: 2^-29. Units: radians. */
    int16_t     crc;        /*!< Amplitude of the Cos Harmonic Correction Term to the Orbit Radius. Scale: 2^-5. Units: meters. */
    int16_t     i_dot;      /*!< Rate of Inclination Angle. Scale: 2^-43. Units: semi-circles/sec. */
    uint32_t     e;          /*!< Eccentricity. Scale: 2^-33. Units: N/A - Dimensionless. */
    uint32_t     sqrt_a;     /*!< Square Root of the Semi-Major Axis. Scale: 2^-19. Units: square root of meters. */
    int32_t     af0;        /*!< Clock Bias Correction Coefficient. Scale: 2^-31. Units: sec. */
    int32_t     m0;         /*!< Mean Anomaly at Reference Time. Scale: 2^-31/PI. Units: semi-circles. */
    int32_t     omega0;     /*!< Longitude of Ascending Node of Orbit Plane and Weekly Epoch. Scale: 2^-31/PI. Units: semi-circles. */
    int32_t     i0;         /*!< Inclination Angle at Reference Time. Scale: 2^-31/PI. Units: semi-circles. */
    int32_t     w;          /*!< Argument of Perigee. Scale: 2^-31/PI. Units: semi-circles. */
    int32_t     omega_dot;  /*!< Rate of Right Ascension. Scale: 2^-43. Units: semi-circles/sec. */
  
} t_EEClient_FullEph;


/**
*  @struct t_EEClient_NavData
*  @brief Structure contains the navigation data.
*  @detail This include Satellite Status, Speed of Device and
*          Geographical Position represented in Latitude, Longituide and Altitude  at a particular instance which
*          is indicated by the time of navigation data.
*/
typedef struct /** t_EEClient_NavData */
{
    /*Plz Do not chnage the order of the Structure Members*/
    uint8_t                         v_numEntriesGPS;                                   /*!< Number of entries in the list. Range 1 - 32. */
    uint8_t                         v_numEntriesGlonass;                               /*!< Number of entries in the list. Range 1 - 24. */
    t_EEClient_FullEph             v_FullEphGPS[EE_CONSTANT_NUM_GPS_PRNS];            /*!< RXN_FullEph_t elements. */
    t_EEClient_FullEph_Glonass    v_FullEphGlonass[EE_CONSTANT_NUM_GLONASS_PRNS];     /*!< RXN_FullEph_GLO_t elements. */
} t_EEClient_NavData;


/**
*  @struct t_EEClient_NavDataBCE
*  @brief Structure contains the broadcast Ephemeris data to be fed to MSL.
*  @detail This include broadcast ephemeris for GPS & Glonass Satellite Status
*             along with type to identify the constellation.
*/
typedef struct
{
    e_EEClient_ConstelType          v_ConsteType;
    t_EEClient_FullEph              v_GpsBCE;                /*!< RXN_FullEph_t elements. */
    t_EEClient_FullEph_Glonass      v_GloBCE;                /*!< RXN_FullEph_GLO_t elements. */
} t_EEClient_NavDataBCE;


/**
*  @struct t_EEClient_RefTimeData
*  @brief Structure contains the Reference Time data.
*  @detail This include TOW, Time Accuracy and week number.
*
*/
typedef struct /** t_EEClient_RefTimeData */
{
    uint32_t TOWmSec;    /*!< Time-Of-Week in mSec. */
    uint32_t TOWnSec;    /*!< Time-Of-Week in nSec. Set to 0, if nSec accuracy not supported.*/
    uint32_t TAccmSec;   /*!< Time-Accuracy in mSec. Set to 0, if accuracy cannot be specified.*/
    uint32_t TAccnSec;   /*!< Time-Accuracy in nSec. Set to 0, if nSec accuracy cannot be specified.*/
    uint16_t weekNum;    /*!< Week number in extended format (i.e. > 1024).*/
} t_EEClient_RefTimeData;



/**
*  @struct t_EEClient_RefPosData
*  @brief Structure contains the Reference Position data.
*  @detail This include Reference position with associated uncertainty & Confidence
*
*/
typedef struct /** t_EEClient_RefPosData */
{
    e_EEClient_LocationType    type;            /*!< Type. LocationType.*/
    float            Latitude;          /*!< Value in degrees. */
    float            Longitude;          /*!< Value in degrees. */
    float            Altitude;          /*!< Value in meters. */
    int32_t        ECEF_X;        /*!< Value in meters. */
    int32_t        ECEF_Y;        /*!< Value in meters. */
    int32_t        ECEF_Z;        /*!< Value in meters. */
    uint16_t        uncertSemiMajor; /*!< Uncertainty along the major axis in centimeters. */
    uint16_t        uncertSemiMinor; /*!< Uncertainty along the minor axis in centimeters. */
    uint16_t        OrientMajorAxis; /*!< Angle between major and north in degrees Range 0 - 180 (OPTIONAL). 0 - circle.*/
    uint8_t        confidence;      /*!< General indication of confidence. Range 0 - 100 (OPTIONAL). */

} t_EEClient_RefPosData;



/**
*  @struct t_EEClient_RefLocationData
*  @brief Structure contains wifi, cell type.
*  @detail This includes the wifi, cell type.Note: This need to be defined.
*
*/
typedef struct /** t_EEClient_RefLocationData */
{
    uint32_t                        TBD;

} t_EEClient_RefLocationData;


/**
*  @struct t_EEClient_EphemerisData
*  @brief Structure contains Ephemeris Request data.
*  @detail This includes the prnBitMask and the Constellation type
*
*/
typedef struct /** t_EEClient_EphemerisData */
{
    uint32_t                     v_PrnBitMask;
    e_EEClient_ConstelType       v_ConstelType;
    uint32_t                     v_EERefGPSTime;

} t_EEClient_EphemerisData;


/**
*  @struct t_EEClient_RefPosition
*  @brief Structure contains Reference Position Request Confirmation data.
*  @detail This includes the Reference Position Data
*
*/
typedef struct /** t_EEClient_RefPosition */
{
    t_EEClient_RefPosData             v_RefPosData;

} t_EEClient_RefPosition;


/**
*  @struct t_EEClient_Ephemeris
*  @brief Structure contains Ephemeris data Request Confirmation data.
*  @detail This includes the Ephemeris Data
*
*/
typedef struct /** t_EEClient_Ephemeris */
{
    t_EEClient_NavData             v_NavData;

} t_EEClient_Ephemeris;



/**
*  @struct t_EEClient_GpsTime
*  @brief Structure contains Reference Time data Indication.
*  @detail This includes the Reference Time data
*
*/
typedef struct /** t_EEClient_GpsTime */
{

    t_EEClient_RefTimeData             v_RefTimeData;

} t_EEClient_GpsTime;



/**
*  @struct t_EEClient_ChipVer
*  @brief Structure contains the GPS Firmware and Hardware version.
*  @detail This includes the version information.
*
*/
typedef struct /** t_EEClient_ChipVer */
{
    uint8_t             v_Version[EECLIENT_CONSTANT_VERSION_STRING_LENGTH];

} t_EEClient_ChipVer;



/*@} group EEClient_InternalDef */

/*! \addtogroup EEClient_APIs */
/*@{*/

/**
*  @brief Callback with Get Ephemeris Indication. This shall be sent as an Indication for Extended Navigational Data from the MSL.
*/
typedef void (* EEClient_getEphemeris_callback)(t_EEClient_EphemerisData *GetEphemeris);


/**
*  @brief Callback with Get Reference Location Indication. This shall be sent as an Indication for Reference Location Data from the MSL.
*/
typedef void (* EEClient_getRefLocation_callback)(t_EEClient_RefLocationData *GetRefLocationData);


/**
*  @brief Callback with Get Broadcast Ephemeris. This shall be sent as a confirmation for Broadcast Ephemeris Request from the MSL.
*/
typedef void (* EEClient_getBroadcastEphemeris_callback)(e_EEClient_Status Status);


/**
*  @brief Callback with Set Broadcast Ephemeris Indication. This shall be sent as a Indication for Broadcast Ephemeris from the MSL.
*/
typedef void (* EEClient_setBroadcastEphemeris_callback)(t_EEClient_NavDataBCE *BCEData);


/**
*  @brief Callback with Get GPS Time. This shall be sent as a confirmation for GPS Time request from the MSL .
*/
typedef void (* EEClient_getGpsTime_callback)(e_EEClient_Status Status);

/**
*  @brief Callback with Set GPS Time. This shall be sent as a Indication for GPS Time Request from the MSL .
*/
typedef void (* EEClient_setGpsTime_callback)(t_EEClient_GpsTime *SetGpsTime);


/**
*  @brief Callback with GPS Firmware and Hardware version. This shall be sent as a confirmation to a request for the GPS Firmware and Hardware version .
*/
typedef void (* EEClient_setChipVer_callback)(t_EEClient_ChipVer *SetChipVer);

/**
*  @brief Callback with Get Reference Location Indication. This shall be sent as an Indication for Reference Location Data from the MSL.
*/
typedef void (* EEClient_getRefTime_callback)();

typedef void (* EEClient_deleteSeed_callback)();


/**
*  @brief GPS callback structure related to Extended GPS / Extended AGPS procedures for MSL.
*/
typedef struct
{
    EEClient_getEphemeris_callback              getephemerisdata_cb;            /**< @brief Callback for Assistance data. This must be marked as NULL if reponse is not expected.*/
    EEClient_getRefLocation_callback            getrefLocationdata_cb;            /**< @brief Callback for Assistance data. This must be marked as NULL if reponse is not expected.*/
    EEClient_getBroadcastEphemeris_callback     getBroadcastephemerisdata_cb;   /**< @brief Callback for BCE data callback. This must be marked as NULL if reponse is not expected. */
    EEClient_setBroadcastEphemeris_callback     setBroadcastephemerisdata_cb;   /**< @brief Callback for BCE data callback. This must be marked as NULL if reponse is not expected. */
    EEClient_getGpsTime_callback                getgpstime_cb;                    /**< @brief Callback for GPS Time callback. This must be marked as NULL if reponse is not expected. */
    EEClient_setGpsTime_callback                setgpstime_cb;                    /**< @brief Callback for GPS Time callback. This must be marked as NULL if reponse is not expected. */
    EEClient_setChipVer_callback                setChipVer_cb;                    /**< @brief Callback for GPS Firmware and Hardware version. This must be marked as NULL if reponse is not expected. */
    EEClient_getRefTime_callback                getReferenceTime_cb;
    EEClient_deleteSeed_callback                deleteSeed_cb;
    
} t_EEClient_Callbacks;



/*****************************************************************************
* EE Client Prototypes
*****************************************************************************/

/**
  *  @brief  This function is for initializing the lbs EE client. Thread is created in application process.
  *  @param callbacks          Structure which has pointer to various call back functions
  *  @return                   returns 0 on success
  */
int EEClient_Init( t_EEClient_Callbacks *callbacks );


/**
  *  @brief  This function is used to Inject Ephemeris data. This is triggered on request for Ephemeris data Indication.
  *  @see EEClient_getEphemeris_callback for confirming the request
  *  @return                   returns 0 on success
  */
int EEClient_SetEphemerisData(t_EEClient_NavData *ephemerisData);


/**
  *  @brief  This function is used to Inject Position data. This is triggered on request for Position data Indication.
  *  @see EEClient_getRefLocation_callback for confirming the request
  *  @return                   returns 0 on success
  */
int EEClient_SetRefPositionData(t_EEClient_RefPosition *refPositionData);


/**
  *  @brief  This function is used to Inject Time data. This is triggered on request for Position data Indication.
  *  @see EEClient_getRefLocation_callback for confirming the request
  *  @return                   returns 0 on success
  */
int EEClient_SetRefTimeData(t_EEClient_RefTimeData *refTimeData);




/**
  *  @brief  This function is used to request for Broadcast Ephemeris from the Chip.
  *  @see EEClient_getBroadcastEphemeris_callback for confirming the request
  *  @return                   returns 0 on success
  */
int EEClient_GetBroadcastEphemeris(e_EEClient_ConstelType constelType);


/**
  *  @brief  This function is used to request the GPS Time from the chip.
  *  @see EEClient_getGpsTime_callback for confirming the request
  *  @return                   returns 0 on success
  */
int EEClient_GetGpsTime();


/**
  *  @brief  This function is used to request the Chip Firmware and Hardware version
  *  @see EEClient_setChipVer_callback for confirming the request
  *  @return                   returns 0 on success
  */
int EEClient_GetChipVer();


/**
  *  @brief  This function is used to indicate the Capablities EEClient is registered for.
  *  @return                   returns 0 on success
  */
int EEClient_SetCapabilities(e_EEClient_SetCapabilitiesInd setCapabilities);


/*@} group EEClient_APIs */



#endif /* #ifndef _EE_CLIENT_H */
