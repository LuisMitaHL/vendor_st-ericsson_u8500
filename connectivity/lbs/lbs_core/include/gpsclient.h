/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
*
* gpsclient.h
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/
/*!
 * \file     gpsClient.h
 */

/*! @defgroup   gpsClient   gpsClient Interface
 *     @brief      Defines the interfaces between GPS client and Location provider
 */


/*! @addtogroup gpsClient
 *     @details
 *      Defines the interfaces between GPS client and Location provider .
 *
 *
 */
    /*@{*/
        /*!@defgroup gpsClient_APIs        gpsClient_APIs*/
        /*!@defgroup gpsClient_InternalDef  gpsClient_InternalDef*/
    /*@}*/


#ifndef _GPS_CLIENT_H
#define _GPS_CLIENT_H

/*! \addtogroup gpsClient_InternalDef */
/*@{*/


/**
* \enum  e_gpsClient_Status
* List of possible GPS states
*/
typedef enum
{
    GPSCLIENT_NO_ERROR,                 /**< @brief no error specified */
    GPSCLIENT_INVALID_PARAMETER,     /**< @brief invalid parameter supplied */
    GPSCLIENT_GPS_NOT_STARTED,         /**< @brief Status GPS failed to start */
    GPSCLIENT_SERVER_BUSY,             /**< @brief Server busy */
    GPSCLIENT_ERROR                     /**< @brief Other Errors */
}e_gpsClient_Status;


/**
* \enum  e_gpsClient_GpsStatus
* List of GPS ON OFF States
*/
typedef enum
{
  GPSCLIENT_GPS_ENGINE_ON,
  GPSCLIENT_GPS_ENGINE_OFF,
  GPSCLIENT_GPS_NO_ERROR,
  GPSCLIENT_GPS_ERROR,
  GPSCLIENT_LAST = 0xFFFFFFFF
} e_gpsClient_GpsStatus;


/**
* \enum  e_gpsClient_EncodingType
* List of Encoding types used for client name / requestor ID /  Code word for Notification data.
*/
typedef enum
{
  GPSCLIENT_NONE = 0,                /**< @brief no encoding type specified */
  GPSCLIENT_UCS2 = 0,                /**< @brief ucs2 format */
  GPSCLIENT_GSM_DEFAULT,             /**< @brief gsm-default refers to the 7-bit default alphabet and the SMS packing specified in [3GPP 23.038]. */
  GPSCLIENT_UTF8,                    /**< @brief utf8 format */
  GPSCLIENT_DCS_8_BIT_DATA,          /**< @brief raw 8-bit data. can be ASCII - 8bit */
  GPSCLIENT_UCS2_COMPRESSED,         /**< @brief Compressed ucs2 format */
  GPSCLIENT_DCS_8_BIT_TEXT,          /**< @brief GSM 7-bit alphabet padded with MSB 0 to make them 8-bits each. */
  GPSCLIENT_MAX_ENCODINGTYPE = 0x7F
} e_gpsClient_EncodingType;


/**
* \enum  e_gpsClient_NotificationType
* List of possible Notification types for GPS.
*/
typedef enum
{
  GPSCLIENT_NO_NOTIFICATION_NO_VERIFICATION = 0,        /**< @brief User is neither notified nor asked for verification. */
  GPSCLIENT_NOTIFICATION_ONLY,                          /**< @brief User is notified but not asked for verification. */
  GPSCLIENT_NOTIFICATION_AND_VERIFICATION_ALLOWED_NA,   /**< @brief User is notified and asked for verification. Request will be allowed if user does not answer. */
  GPSCLIENT_NOTIFICATION_AND_VERIFICATION_DENIED_NA,    /**< @brief User is notified and asked for verification. Request will be denied if user does not answer. */
  GPSCLIENT_PRIVACY_OVERRIDE,                           /**< @brief Is used for preventing notification and verification without leaving any traces.  */
  GPSCLIENT_NOTIFICATION_CANCEL                         /**< @brief Previous notification with same handle has expired */
} e_gpsClient_NotificationType;


/**
* \enum  e_gpsClient_LcsServiceType
* List of service used in notification
*/
typedef enum{
  GPSCLIENT_EMERGENCY_SERVICES = 0,                   /**< @brief Emergency_services.*/
  GPSCLIENT_EMERGENCY_ALERT_SERVICES,                 /**< @brief Emergency_alert_services.*/
  GPSCLIENT_PERSON_TRACKING,                          /**< @brief Person_tracking.*/
  GPSCLIENT_FLEET_MANAGEMENT,                         /**< @brief Fleet_management.*/
  GPSCLIENT_ASSET_MANAGEMENT,                         /**< @brief Asset_management.*/
  GPSCLIENT_TRAFFIC_CONGESTION_REPORTING,             /**< @brief Traffic_congestion_reporting.*/
  GPSCLIENT_ROAD_SIDE_ASSISTANCE,                     /**< @brief Road_side_assistance.*/
  GPSCLIENT_ROUTING_TO_NEAREST_COMMERCIAL_ENTERPRISE, /**< @brief Routing_to_nearest_commercial_enterprise .*/
  GPSCLIENT_NAVIGATION,                               /**< @brief navigation.*/
  GPSCLIENT_CITY_SIGHTSEEING,                         /**< @brief Agps city sightseeing .*/
  GPSCLIENT_LOCALIZED_ADVERTISING,                    /**< @brief Localized_advertising.*/
  GPSCLIENT_MOBILE_YELLOW_PAGES,                      /**< @brief Mobile_yellow_pages.*/
  GPSCLIENT_MAX_LCSSERVICETYPE = 0x7FFFFFFF           /**< @brief Max_lcsservicetype.*/
}e_gpsClient_LcsServiceType;



#ifndef DEBUG_LOG_PRINT
#ifndef ARM_LINUX_AGPS_FTR
#define DEBUG_LOG_PRINT( X )                ALOGD X
#else
#define DEBUG_LOG_PRINT( X )                printf X
#endif


#ifdef DEBUG_LOG_LEV2
#define DEBUG_LOG_PRINT_LEV2( X )   DEBUG_LOG_PRINT( X )
#else  /* DEBUG_LOG_LEV2 */
#define DEBUG_LOG_PRINT_LEV2( X )
#endif /* DEBUG_LOG_LEV2 */

#endif


/**
* \enum  e_gpsClient_NmeaMask
* List of avaliable NMEA masks
*/
typedef enum
{
    GPSCLIENT_GPGLL_ENABLE  = (1<<0),            /**< @brief Mask for NMEA $GPGLL (Geographic Position - Latitude, Longitude) */
    GPSCLIENT_GPGGA_ENABLE  = (1<<1),            /**< @brief Mask for NMEA $GPGGA (GPS Fix Data) */
    GPSCLIENT_GPGSA_ENABLE  = (1<<2),            /**< @brief Mask for NMEA $GPGSA (GNSS DOPS and Active Satellites) */
    GPSCLIENT_GPGST_ENABLE  = (1<<3),            /**< @brief Mask for NMEA $GPGST (GNSS Pseudorange Error Statistics) */
    GPSCLIENT_GPGSV_ENABLE  = (1<<4),            /**< @brief Mask for NMEA $GPGSV (GNSS Satellites in View) */
    GPSCLIENT_GPRMC_ENABLE  = (1<<5),            /**< @brief Mask for NMEA $GPRMC (Recommended Minimum GNSS Sentence) */
    GPSCLIENT_GPVTG_ENABLE  = (1<<6),            /**< @brief Mask for NMEA $GPVTG (Course Over Ground and Ground Speed) */
    GPSCLIENT_GPZCD_ENABLE  = (1<<7),            /**< @brief Mask for NMEA $GPZCD (output rate - OS Timestamp for the Fix [seconds]) */
    GPSCLIENT_GPZDA_ENABLE  = (1<<8),            /**< @brief Mask for NMEA $GPZDA (Time & Date) */
    GPSCLIENT_PGNVD_ENABLE  = (1<<9)             /**< @brief Mask for NMEA $PGNVD ( proprietary Diagnostics data ) */
}e_gpsClient_NmeaMask;

/**
* \enum  e_gpsClient_OutputType
* List of avaliable Output types for GPS.
*/
typedef enum
{
    GPSCLIENT_NMEA_TYPE    = 0,                   /**< @brief Navigation is of NMEA type */
    GPSCLIENT_CSTRUCT_TYPE = 1                    /**< @brief Navigation is of CSTRUCT type */
}e_gpsClient_OutputType;


/**
* \enum  e_gpsClient_DeleteAssistData
* List of avaliable NMEA masks
*/
typedef enum
{
   GPSCLIENT_ASSIST_DELETE_EPHEMERIS     =   0x0001,    /**< @brief   Mask to Delete Ephemeris Data */
   GPSCLIENT_ASSIST_DELETE_ALMANAC       =   0x0002,    /**< @brief   Mask to Delete Almanac Data */
   GPSCLIENT_ASSIST_DELETE_POSITION      =   0x0004,    /**< @brief   Mask to Delete Reference Position Data */
   GPSCLIENT_ASSIST_DELETE_TIME          =   0x0008,    /**< @brief   Mask to Reference Time Data */
   GPSCLIENT_ASSIST_DELETE_IONO          =   0x0010,    /**< @brief   Mask to Iono Model Data */
   GPSCLIENT_ASSIST_DELETE_UTC           =   0x0020,    /**< @brief   Mask to Delete UTC Model Data */
   GPSCLIENT_ASSIST_DELETE_HEALTH        =   0x0040,    /**< @brief   Mask to Delete Health Data */
   GPSCLIENT_ASSIST_DELETE_SVDIR         =   0x0080,    /**< @brief   Mask to Delete SV Direction Data */
   GPSCLIENT_ASSIST_DELETE_SVSTEER       =   0x0100,    /**< @brief   Mask to Delete SV Steering Data */
   GPSCLIENT_ASSIST_DELETE_SADATA        =   0x0200,    /**< @brief   Mask to Delete SA Data */
   GPSCLIENT_ASSIST_DELETE_RTI           =   0x0400,    /**< @brief   Mask to Delete Real Time Integrity Data */
   GPSCLIENT_ASSIST_DELETE_SEED_EPHEMERIS=   0x0800,    /**< @brief   Mask to Delete Seed Data*/
   GPSCLIENT_ASSIST_DELETE_LOCID         =   0x4000,    /**< @brief   Mask to Delete Location ID Data */
   GPSCLIENT_ASSIST_DELETE_CELLDB_INFO   =   0x8000,    /**< @brief   Mask to Delete Cell Data */
   GPSCLIENT_ASSIST_DELETE_ALL           =   0xFFFF     /**< @brief   Mask to Delete All assistance Data */
} e_gpsClient_DeleteAssistData;


/**
* \enum  e_gpsClient_UserResponseType
* Possible user responses for Notification messages.
*/
typedef enum
{
    GPSCLIENT_NI_RESPONSE_ACCEPT         = 1,    /**< @brief NI Response:Accepted by User */
    GPSCLIENT_NI_RESPONSE_DENY           = 2,    /**< @brief NI Response:Deny from User */
    GPSCLIENT_NI_RESPONSE_NORESP         = 3     /**< @brief NI Response:No Response from User */
} e_gpsClient_UserResponseType;


/**
* \enum  e_gpsClient_AgpsStatData
* List of agps connection status.
*/
typedef enum
{
    GPSCLIENT_REQUEST_AGPS_DATA_CONN = 1, /**< @brief GPS requests data connection for AGPS.  */
    GPSCLIENT_RELEASE_AGPS_DATA_CONN = 2, /**< @brief GPS releases the AGPS data connection.*/
    GPSCLIENT_AGPS_DATA_CONNECTED    = 3, /**< @brief AGPS data connection initiated. */
    GPSCLIENT_AGPS_DATA_CONN_DONE    = 4, /**< @brief AGPS data connection completed. */
    GPSCLIENT_AGPS_DATA_CONN_FAILED  = 5  /**< @brief AGPS data connection failed. */
}e_gpsClient_AgpsStatData;



/**
* \enum  e_gpsClient_NetworkInfoType
* List of possible type of network connection.
*/
typedef enum
{
    GPSCLIENT_NETOWRK_INFO_TYPE_CELLULAR = 1, /**< @brief Request infrormation is of type mobile cellular netowrk.  */
    GPSCLIENT_NETOWRK_INFO_TYPE_WLAN     = 2, /**< @brief Request infrormation is of type WLAN */
}e_gpsClient_NetworkInfoType;



/**
* \enum  e_gpsClient_AreaEventType
* List of Area events to be considered for geo fencing.
*/
typedef enum
{
    GPSCLIENT_ENTERING_AREA    = 0,   /**< Considering event occurence only in case user is entering defined area */
    GPSCLIENT_INSIDE_AREA      = 1,   /**< Considering event occurence only in case user is inside defined area */
    GPSCLIENT_OUTSIDE_AREA     = 2,   /**< Considering event occurence only in case user is outside defined area */
    GPSCLIENT_LEAVING_AREA     = 3    /**< Considering event occurence only in case user is leaving defined area */
} e_gpsClient_AreaEventType;


/**
* \enum  e_gpsClient_ShapeType
* List of shapes considered for geo fencing.
*/
typedef enum
{
    GPSCLIENT_CIRCULAR_AREA      = 0,  /**< Describes the Target Area type as Circular */
    GPSCLIENT_ELLIPTICAL_AREA    = 1,  /**< Describes the Target Area type as Elliptical */
    GPSCLIENT_POLYGON_AREA       = 2   /**< Describes the Target Area type as Polygon */
} e_gpsClient_ShapeType;


/**
* \enum  e_gpsClient_ThirdPartyId
* List of possible identifiers for a third party entity with which position information can be exchanged.
*/
typedef enum
{
    GPSCLIENT_THIRDPARTYID_NOTHING              = 0,            /**< Third Party Information is not present */
    GPSCLIENT_THIRDPARTYID_LOGICALNAME          = 1,            /**< Describes the third party through its logical name */
    GPSCLIENT_THIRDPARTYID_MSISDN               = 2,            /**< Describes the third party through its Mobile Station International Subscriber Directory Number (MSISDN) */
    GPSCLIENT_THIRDPARTYID_EMAILADDR            = 3,            /**< Describes the third party through its Email address */
    GPSCLIENT_THIRDPARTYID_SIP_URI              = 4,            /**< Describes the third party through its Session Initiation Protocol (SIP) URI */
    GPSCLIENT_THIRDPARTYID_IMS_PUBLIC_IDENTITY  = 5,            /**< Describes the third party through its IP Multimedia Subsystem (IMS) public ID */
    GPSCLIENT_THIRDPARTYID_MIN                  = 6,            /**< Describes the third party through its Mobile Identification Number (MIN) */
    GPSCLIENT_THIRDPARTYID_MDN                  = 7,            /**< Describes the third party through its Mobile Directory Number (MDN)*/
    GPSCLIENT_THIRDPARTYID_URI                  = 8,            /**< Describes the third party through its Uniform Resource Identifier (URI) */
    GPSCLIENT_THIRDPARTYID_MAX_IND              = 0X7FFFFFFF
}e_gpsClient_ThirdPartyId;


/**
*  @brief Number of Polygon points. @see t_gpsClient_GeographicTargetArea
*/
#define GPSCLIENT_MAX_POLYGON_POINTS           15

/**
*  @brief Number of Satellite. @see t_gpsClient_NavData
*/
#define GPSCLIENT_NUM_SV  32

/**
*  @brief Maximum Size (in bytes) of Requester Identity
*/
#define GPSCLIENT_MAX_LENGTH_REQUESTER_ID      75

/**
*  @brief Maximum Size (in bytes) of Client Name
*/
#define GPSCLIENT_MAX_LENGTH_CLIENT_NAME       67

/**
*  @brief Maximum Size (in bytes) of Code Word
*/
#define GPSCLIENT_MAX_LENGTH_CODE_WORD         75

/**
*  @brief Maximum Size (in bytes) of Phone Number
*/
#define GPSCLIENT_MAX_LENGTH_PHONE_NUMBER      65

/**
*  @brief Maximum Size (in bytes) of Client External ID
*/
#define GPSCLIENT_MAX_CLIENT_EXT_ID_LENGTH     72

/**
*  @brief Maximum Size (in bytes) of MLC Number
*/
#define GPSCLIENT_MAX_MLC_NUMBER_LEN           64

/**
*  @brief Maximum Size (in bytes) of Application Provider
*/
#define GPSCLIENT_MAX_APP_PROVIDER_LEN         65

/**
*  @brief Maximum Size (in bytes) of Application Name
*/
#define GPSCLIENT_MAX_APP_NAME_LEN             65

/**
*  @brief Maximum Size (in bytes) of Application Versioning
*/
#define GPSCLIENT_MAX_APP_VERSION_LEN          65


/**
*  @brief Maximum Size (in bytes) of Application Versioning
*/
#define GPSCLIENT_MAX_THIRD_PARTY_NAME_LEN          40


/**
*  @brief Maximum Size (in bytes) of Production test Frequency string
*/
#define GPSCLIENT_MAX_TREQ_FREQ_LEN          15    /** Input frequency max string size */
#define GPSCLIENT_MAX_SA_TEST_PGA_VALUE_LEN  20    /** Max length of parameters in Set PGA/AGV value test for Spectrum Analyzer*/

/**
*  @struct t_gpsClient_NavData
*  @brief Structure contains the navigation data.
*  @detail This include Satellite Status, Speed of Device and
*          Geographical Position represented in Latitude, Longituide and Altitude  at a particular instance which
*          is indicated by the time of navigation data.
*/
typedef struct /** t_gpsClient_NavData */
{

  uint32_t v_LocalTTag;                        /**< @brief Local baseband millisecond time-tag [msec] */
  uint32_t v_OSTimeMs;                         /**< @brief Local Operating System Time [msec] */
  uint16_t v_Year;                             /**< @brief UTC Year A.D. [eg 2007]  */
  uint16_t v_Month;                            /**< @brief UTC Month into the year [range 1..12] */
  uint16_t v_Day;                              /**< @brief UTC Days into the month [range 1..31] */
  uint16_t v_Hours;                            /**< @brief UTC Hours into the day [range 0..23] */
  uint16_t v_Minutes;                          /**< @brief UTC Minutes into the hour [range 0..59] */
  uint16_t v_Seconds;                          /**< @brief UTC Seconds into the hour [range 0..59] */
  uint16_t v_Milliseconds;                     /**< @brief UTC Milliseconds into the second [range 0..999] */
  int16_t v_GpsWeekNo;                        /**< @brief GPS Week Number */
  double v_GpsTOW;                        /**< @brief Corrected GPS Time of Week [seconds] */
  double v_UtcCorrection;                 /**< @brief Current (GPS-UTC) time zone difference [seconds] */
  double v_X;                             /**< @brief WGS84 ECEF X Cartesian coordinate [m]. */
  double v_Y;                             /**< @brief WGS84 ECEF Y Cartesian coordinate [m]. */
  double v_Z;                             /**< @brief WGS84 ECEF Z Cartesian coordinate [m]. */
  double v_Latitude;                      /**< @brief WGS84 Latitude  [degrees, positive North] */
  double v_Longitude;                     /**< @brief WGS84 Longitude [degrees, positive East] */
  float v_AltitudeEll;                    /**< @brief Altitude above the WGS84 Ellipsoid [m] */
  float v_AltitudeMSL;                    /**< @brief Altitude above Mean Sea Level [m] */
  float v_SpeedOverGround;                /**< @brief 2-dimensional Speed Over Ground [m/s] */
  float v_CourseOverGround;               /**< @brief 2-dimensional Course Over Ground [degrees] */
  float v_VerticalVelocity;               /**< @brief Vertical velocity [m/s, positive Up] */
  float v_NAccEst;                        /**< @brief Northing RMS 1-sigma (67%) Accuracy estimate [m] */
  float v_EAccEst;                        /**< @brief Easting  RMS 1-sigma (67%) Accuracy estimate [m] */
  float v_VAccEst;                        /**< @brief Vertical RMS 1-sigma (67%) Accuracy estimate [m] */
  float v_HAccMaj;                        /**< @brief Horizontal RMS 1-sigma (67%) Error ellipse semi-major axis [m] */
  float v_HAccMin;                        /**< @brief Horizontal RMS 1-sigma (67%) Error ellipse semi-minor axis [m] */
  float v_HAccMajBrg;                     /**< @brief Bearing of the Horizontal Error ellipse semi-major axis [degrees] */
  float v_HVelAccEst;                     /**< @brief Horizontal Velocity RMS 1-sigma (67%) Accuracy estimate [m/s] */
  float v_VVelAccEst;                     /**< @brief Vertical Velocity RMS 1-sigma (67%) Accuracy estimate [m/s] */
  float v_COG_AccEst;                     /**< Course Over Ground 1-sigma (i.e. rms error) Accuracy estimate [degrees]*/
  float v_PRResRMS;                       /**< @brief Standard deviation of the Pseudorange a posteriori residuals [m] */
  float v_HDOP;                           /**< @brief HDOP (Horizontal Dilution of Precision) */
  float v_VDOP;                           /**< @brief VDOP (Vertical Dilution of Precision) */
  float v_PDOP;                           /**< @brief PDOP (3-D Position Dilution of Precision) */
  uint8_t v_FixType;                           /**< @brief Position Fix Type */
  uint8_t v_Valid2DFix;                        /**< @brief Is the published 2D position fix "Valid" relative to the required Horizontal Accuracy Masks ? */
  uint8_t v_Valid3DFix;                        /**< @brief Is the published 3D position fix "Valid" relative to both the required Horizontal and Vertical Accuracy Masks ? */
  uint8_t v_FixMode;                           /**< @brief Solution Fixing Mode (1 = Forced 2-D at Mean-Sea-Level, 2 = 3-D with automatic fall back to 2-D mode) */
  uint8_t v_SatsInView;                        /**< @brief Satellites in View count */
  uint8_t v_SatsUsed;                          /**< @brief Satellites in Used for Navigation count */
  uint16_t v_Filler1;                          /**< @brief Dummy Bytes to align with 4 bytes*/
  uint8_t  v_SatsInViewConstell[GPSCLIENT_NUM_SV]; /**< Satellites in View Constellation identifier [ ] */
  uint8_t v_SatsInViewSVId[GPSCLIENT_NUM_SV];    /**< @brief Satellites in View SV id number [PRN] */
  uint8_t v_SatsInViewSNR[GPSCLIENT_NUM_SV];     /**< @brief Satellites in View Signal To Noise Ratio [dBHz] */
  uint8_t v_SatsInViewJNR[GPSCLIENT_NUM_SV];     /**< @brief Satellites in View Jammer To Noise Ratio [dBHz] */
  uint16_t v_SatsInViewAzim[GPSCLIENT_NUM_SV];    /**< @brief Satellites in View Azimuth [degrees] */
  int8_t v_SatsInViewElev[GPSCLIENT_NUM_SV];    /**< @brief Satellites in View Elevation [degrees].  If = -99 then Azimuth & Elevation angles are currently unknown */
  uint8_t v_SatsInViewUsed[GPSCLIENT_NUM_SV];    /**< @brief Satellites in View Used for Navigation ? */
  uint8_t v_Filler2[4];                       /**< @brief Dummy Bytes to align with 4 bytes*/
} t_gpsClient_NavData;

/**
*  @struct t_gpsClient_NmeaData
*  @brief structure to represent data packet
*/
typedef struct {
    int       length;      /**< @brief Length of NMEA Sentences */
    void *    pData;       /**< @brief Pointer of Data containing NMEA Sentences. Note data must be copied and Pointer must not be freed after use.*/
}t_gpsClient_NmeaData;



/**
*  @struct t_gpsClient_ThirdPartyInfo
*  @brief structure to represent third party entity with which position information can be exchanged.
*/
typedef struct
{
    uint32_t    v_ThirdPartyId;    /**<@Third Party ID */
    int8_t     a_ThirdPartyName[ GPSCLIENT_MAX_THIRD_PARTY_NAME_LEN ];/**< @Third Party Name to which the location transfer has to be made */
}t_gpsClient_ThirdPartyInfo;


/**
*  @struct t_gpsClient_ApplicationIDInfo
*  @brief structure to represent application information which is requesting positioning information.
*/
typedef struct
{
    uint8_t    v_ApplicationIDInfoPresent;                            /**<@ Application Info Present or not */
    int8_t     v_ApplicationProvider[GPSCLIENT_MAX_APP_PROVIDER_LEN]; /**<@Application Provider String*/
    int8_t     v_ApplicationName[GPSCLIENT_MAX_APP_NAME_LEN];         /**<@Application Name String*/
    int8_t     v_ApplicationVersion[GPSCLIENT_MAX_APP_VERSION_LEN];   /**<@Application Name String*/

}t_gpsClient_ApplicationIDInfo;


/**
* \struct t_gpsClient_ThirdClientInfo
* \brief Structure with the Third party information
*/
typedef struct
{
    uint8_t     v_ClientIdTonNpi;                                       /**< @brief Client ID Type of Number and number Plan identification. Refer 3GPP TS 24.008, section 10.5.4.7. */
    uint8_t     v_ClientIdConfig;                                       /**< @brief Indicates if v_TonNpi Client Id has been populated.  */
    uint8_t     v_MlcNumTonNpi;                                         /**< @brief MLC Number Type of Number and number Plan identification. Refer 3GPP TS 24.008, section 10.5.4.7. */
    uint8_t     v_MlcNumConfig;
    uint8_t     v_ClientExtIdLen;                                       /**< @brief Requestor Id string length */
    uint8_t     v_MlcNumberLen;
    uint8_t     v_Filler[2];                                            /**< @brief Client Name string length *//**< Indicates if v_TonNpi MLC Number has been populated.    */
    int8_t     a_ClientExtId[ GPSCLIENT_MAX_CLIENT_EXT_ID_LENGTH ];    /**< @brief a_Client External Id string */
    int8_t     a_MlcNumber[ GPSCLIENT_MAX_MLC_NUMBER_LEN ];            /**< @brief MLC Number string */
    t_gpsClient_ThirdPartyInfo v_ThirdPartyInfo;                        /**< @Third Party ID Information */
} t_gpsClient_ThirdClientInfo;


typedef struct
{
    int16_t  v_WLAN_AP_TransmitPower;                /*!<AP Transmit power in dBm. Optional parameter. INTEGER (-127...128). Invalid Value 0X7FFF*/
    int16_t  v_WLAN_AP_AntennaGain;                  /*!<AP antenna gain in dBi,Optional parameter.  INTEGER (-127...128). Invalid Value 0X7FFF*/
    int16_t  v_WLAN_AP_SNR;                          /*!<AP Signal to Noise received at the SET in dB,Optional parameter.  INTEGER (-127...128). Invalid Value 0X7FFF*/
    uint8_t  v_WLAN_AP_DeviceType;                   /*!<ENUM, 802.11a device or 802.11b device or 802.11g device or Unknown - Optional parameter- valid values(0,1,2).Invalid value 5*/
    int16_t  v_WLAN_AP_SignalStrength;               /*!<AP signal strength received at the SET in dBm. INTEGER (-127...128). Invalid Value 0X7FFF- Optional parameter.*/
    uint16_t v_WLAN_AP_Channel;                      /*!<AP channel/frequency of Tx/Rx - Optional parameter- valid range (0...256).Invalid Value0XFFFFFFFF*/
    uint32_t v_WLAN_AP_RTDValue;                     /*!<Measured Round Trip Delay value between the SET and AP-Optional parameter- Valid range (0...1677216).*/
    uint8_t  v_WLAN_AP_RTDUnits;                     /*!<Units for Round Trip Delay value and Round Trip Delay accuracy -Optional parameter-Valid range (0...4).*/
    uint16_t v_WLAN_AP_Accuracy;                     /*!<Round Trip Delay standard deviation in relative units. INTEGER (0...255)-Optional parameter.Invalid Value0XFFFFFFFF*/
    int16_t  v_WLAN_AP_SETTransmitPower;             /*!<SET Transmit power in dBm-Optional parameter.INTEGER (-127...128).*/
    int16_t  v_WLAN_AP_SETAntennaGain;               /*!<SET antenna gain in dBi-Optional parameter-.INTEGER (-127...128).Invalid Value 0X7FFF*/
    int16_t  v_WLAN_AP_SETSNR;                       /*!<SET Signal to Noise received at the AP in dB-Optional parameter.INTEGER (-127...128)Invalid Value 0X7FFF. */
    int16_t  v_WLAN_AP_SETSignalStrength;            /*!<SET signal strength received at the AP in dBm-Optional parameter.INTEGER (-127...128)Invalid Value 0X7FFF.*/

} t_gpsClient_WlanOptinalInfo;

/**
* \struct t_gpsClient_WlanInfo
* \brief Structure with the Wlan Information
*/
typedef struct
{
    uint16_t v_WLAN_AP_MS_Addr;                      /*!<Media Access Control address of WLAN access point (most significant 16 bits)*/
    uint32_t v_WLAN_AP_LS_Addr;                      /*!<Media Access Control address of WLAN access point (least significant 32 bits)*/
    t_gpsClient_WlanOptinalInfo *v_WLAN_OptionalInfo; /*!<Wlan Optional Information*/

} t_gpsClient_WlanInfo;



/**
* \struct t_gpsClient_Qop
* \brief Structure with the Quality of Position exposed
*/
typedef struct
{

  uint32_t     vp_Timeout;                 /**< @brief Response Time */
  uint16_t     vp_HorizAccuracy;        /**< @brief Horizontal Accuracy */
  uint16_t     vp_VertAccuracy;         /**< @brief Vertical Accuracy */
  uint32_t     vp_AgeLimit;                /**< @brief Age Limit */

} t_gpsClient_Qop;


/**
*  @struct t_gpsClient_NotifyData
*  @brief structure to represent agps notification
*/
typedef struct {
    uint32_t  v_Handle;                           /**< @brief  Handle of session */
    uint8_t   v_Type;                             /**< @brief  Type of Notification \ref e_gpsClient_NotificationType
      \verbatim
            0 - User is neither notified nor asked for verification.
            1 - User is notified but not asked for verification.
            2 - User is notified and asked for verification. Request will be allowed if user does not answer.
            3 - User is notified and asked for verification. Request will be denied if user does not answer.
            4 - Is used for preventing notification and verification without leaving any traces
            5 - Previous notification with same handle has expired
            6 and above : Reserved
      \endverbatim
       */
    uint8_t   v_LcsServiceTypeId;                 /**< @brief  LCS Servce Type Id \ref e_gpsClient_LcsServiceType
      \verbatim
            0 - Emergency_services.
            1 - Emergency_alert_services.
            2 - Person_tracking.
            3 - Fleet_management.
            4 - Asset_management.
            5 - Traffic_congestion_reporting.
            6 - Road_side_assistance.
            7 - Routing_to_nearest_commercial_enterprise .
            8 - navigation.
            9 - Agps city sightseeing .
            10 - Localized_advertising.
            11 - Mobile_yellow_pages.
            12 - Max_lcsservicetype.
            13 and above : Reserved
      \endverbatim
       */

    uint8_t   v_RequestorIdEncodingType;          /**< @brief Requestor Id Encoding Type  \ref e_gpsClient_EncodingType
     \verbatim
        0 - no encoding type specified
        0 - ucs2 format
        1 - gsm-default refers to the 7-bit default alphabet and the SMS packing specified in [3GPP 23.038].
        2 - utf8 format
        3 - raw 8-bit data. can be ASCII - 8bit
        4 - Compressed ucs2 format
        5 - GSM 7-bit alphabet padded with MSB 0 to make them 8-bits each.
        6 and above : Reserved
      \endverbatim
    */
    int8_t   a_RequestorId[ GPSCLIENT_MAX_LENGTH_REQUESTER_ID ];  /**< @brief Requestor Id. Must be a Null Terminated String */
    uint8_t   v_ClientNameEncodingType;                            /**< @brief Client Name Ecoding Type \ref e_gpsClient_EncodingType
         \verbatim
        0 - no encoding type specified
        0 - ucs2 format
        1 - gsm-default refers to the 7-bit default alphabet and the SMS packing specified in [3GPP 23.038].
        2 - utf8 format
        3 - raw 8-bit data. can be ASCII - 8bit
        4 - Compressed ucs2 format
        5 - GSM 7-bit alphabet padded with MSB 0 to make them 8-bits each.
        6 and above : Reserved
      \endverbatim
      */
    int8_t   a_ClientName[ GPSCLIENT_MAX_LENGTH_CLIENT_NAME ];    /**< @brief Client Name. Must be a Null Terminated String */
    uint8_t   v_CodeWordEncodingType;                              /**< @brief Code word Encoding Type  \ref e_gpsClient_EncodingType
         \verbatim
        0 - no encoding type specified
        0 - ucs2 format
        1 - gsm-default refers to the 7-bit default alphabet and the SMS packing specified in [3GPP 23.038].
        2 - utf8 format
        3 - raw 8-bit data. can be ASCII - 8bit
        4 - Compressed ucs2 format
        5 - GSM 7-bit alphabet padded with MSB 0 to make them 8-bits each.
        6 and above : Reserved
      \endverbatim
      */
    int8_t   a_CodeWord[ GPSCLIENT_MAX_LENGTH_CODE_WORD ];        /**< @brief Code word. Must be a Null Terminated String */
    int8_t   a_PhoneNumber[ GPSCLIENT_MAX_LENGTH_PHONE_NUMBER ];  /**< @brief Phone Number. Must be a Null Terminated String */
    uint32_t  v_TonNpi;                                            /**< @brief Type of Number and number Plan identification. Refer 3GPP TS 24.008, section 10.5.4.7
         \verbatim
            Type of Number (Bit 6,5,4)
                X000xxxx - Unknown
                X001xxxx - International
                X010xxxx - National
                X011xxxx - Network_specific
                X100xxxx - Subscriber
                X101xxxx - Alphanumeric
                X110xxxx - Abbreviated

            Number Plan Identification (Bit 3,2,1,0)
                xxxx0000 - Unknown
                xxxx0001 - Isdn_telephony
                xxxx0011 - Data
                xxxx0100 - Telex
                xxxx0110 - Land_mobile
                xxxx1000 - National
                xxxx1001 - Private
        Note: Bit 7 is spare and is ignored.
      \endverbatim
      */
    uint32_t  v_TonNpiConfig;                                      /**< @brief Indicates if v_TonNpi has been populated.
     \verbatim
            0 - Type of Number and number Plan identification Not Provided.
            1 - Type of Number and number Plan identification Provided.
            2 and above : Reserved
      \endverbatim
       */

}t_gpsClient_NotifyData;


typedef struct
{
    e_gpsClient_ShapeType   v_ShapeType;                                      /**< Describes the Shape Type  */
    uint8_t                 v_NumberOfPoints;                                 /**< Number of points in polygon. [range 3..15]. Value applicable only if Shape Type is polygon  */
    uint8_t                 v_EllipSemiMinorAngle;                            /**< Angle between the semi-major axis and North, increasing in a clockwise direction in degrees [range 0..179]*/
                                                                              /**< Angle 0 represents ellipse with semi-major axis pointing North/South*/
                                                                              /**< while angle of 90 represents an ellipse with semi-major axis pointing East/West.*/
                                                                              /**< Value applicable only if Shape Type is ellipse. */
    uint8_t                 v_LatSign[GPSCLIENT_MAX_POLYGON_POINTS];          /**< Array of Latitude Sign. [0=North, 1=South]*/
                                                                              /**< In case of Shape Type is circle/ellipse, latitude sign is a single element that of center.*/
                                                                              /**<In case of Shape Type is polygon, array contains a sequnce of latitude signs of all points */
    uint32_t                v_Latitude[GPSCLIENT_MAX_POLYGON_POINTS];         /**< Array of Latitudes. [range 0..8388607 for 0..90 degrees]*/
                                                                              /**<In case of Shape Type is circle/ellipse, latitudes is a single element that of center.*/
                                                                              /*<In case of Shape Type is polygon, array contains a sequnce of latitudes of all points */
    int32_t                 v_Longitude[GPSCLIENT_MAX_POLYGON_POINTS];        /**< Array of Longitudes. [range -8388608..8388607 for -180..+180 degrees]*/
                                                                              /**<In case of Shape Type is circle/ellipse, longitudes is a single element that of center.*/
                                                                              /**<In case of Shape Type is polygon, array contains a sequnce of longitudes of all points */
    uint32_t                v_CircularRadius;                                 /**< Radius of the circle in meters. [range 1..1000000m].*/
                                                                              /**<Value valid only if Shape Type is circle*/
    uint32_t                v_CircularRadiusMin;                              /**< Hysteresis Minimum Radius of the circle in meters [range 0..1000000m] */
                                                                              /**<Value valid only if Shape Type is circle. Considered not set if 0*/
    uint32_t                v_CircularRadiusMax;                              /**< Hysteresis Maximum Radius of the circle in meters [range 0..1500000m] */
                                                                              /**<Value valid only if Shape Type is circle. Considered not set if 0*/
    uint32_t                v_EllipSemiMajor;                                 /**< Semi Major of Ellipse in meters. [range 1..1000000m].*/
                                                                              /**<Value valid only if Shape Type is ellipse */
    uint32_t                v_EllipSemiMajorMin;                              /**< Hysteresis Minimum Semi Major of Ellipse in meters [range 0..1000000m] */
                                                                              /**<Value valid only if Shape Type is ellipse. Considered not set if 0*/
    uint32_t                v_EllipSemiMajorMax;                              /**< Hysteresis Maximum Semi Major of Ellipse in meters [range 0..1500000m] */
                                                                              /**<Value valid only if Shape Type is ellipse. Considered not set if 0*/
    uint32_t                v_EllipSemiMinor;                                 /**< Semi Minor of Ellipse in meters. [range 1..1000000m].*/
                                                                              /**<Value valid only if Shape Type is ellipse.*/
    uint32_t                v_EllipSemiMinorMin;                              /**< Hysteresis Minimum Semi Minor of Ellipse in meters [range 0..1000000m] */
                                                                              /**<Value valid only if Shape Type is ellipse. Considered not set if 0*/
    uint32_t                v_EllipSemiMinorMax;                              /**< Hysteresis Maximum Semi Minor of Ellipse in meters [range 0..1500000m] */
                                                                              /**<Value valid only if Shape Type is ellipse. Considered not set if 0*/
    uint32_t                v_PolygonHysteresis;                              /**< Hysteresis of polygon in meters. [range 0..100000m]. */
                                                                              /**<Value valid only if Shape Type is polygon. Considered not set if 0*/
}t_gpsClient_GeographicTargetArea;


typedef struct
{
    uint32_t     v_Timeout;         /**< Response Time in milli seconds [range 1..3600000ms]. 3600000 ms = 1hour.*/
                                    /**< Value applicable only if v_NumberOfFixes = 1*/
    uint32_t     v_AgeLimit;        /**< Age Limit in milliseconds. Too old data can't be delivered. 0 value sends only a new position fix. */
    uint32_t     v_FixInterval;     /** <Fix Interval in milli seconds [range 100..604800000ms]. 604800000 = 1 week in milli seconds. Value applicable only if v_NumberOfFixes > 1*/
    uint32_t     v_StartTime;       /**< Start Time in seconds [range 1 to 2678400s]. If value set to 0, reporting started immediately*/
                                    /**< Value applicable only if v_NumberOfFixes > 1*/
    uint32_t     v_StopTime;        /**< Stop Time in seconds [range 1 to 11318399s]. If set to 0, value considered not set.*/
                                    /**< Value applicable only if v_NumberOfFixes > 1. */
                                    /**< Stop Time > Start Time. Start Time - Stop Time <= 8639999s (100 days) */
                                    /**< If stop time > (number of fixes * fix interval) + start time, then stop time is ignored */
    uint16_t     v_NumberOfFixes;   /**< Number of Fixes to be reported. [range 1..65535 ]  */
    uint16_t     v_HorizAccuracy;   /**< Horizontal Accuracy [range 1.. 65535 m]. If value set to 0 default of 100m is considered*/
    uint16_t     v_VertAccuracy;    /**< Vertical Accuracy [range 1.. 65535 m]. If value set to 0 default of 150m is considered*/
    uint16_t     v_Padding;         /*!< Padding to align on 32 */
} t_gpsClient_PositioningParams;


#ifdef AGPS_ANDR_GINGER_FTR
typedef struct
{
    uint16_t type;    /**<Cell type 0-GSM, 1-WCDMA, 2-TDSCDMA. Invalid Value -1*/
    uint16_t mcc;     /**<PLMN-MCC value Mobile Country Code [range 0..999]. Invalid Value -1.*/
    uint16_t mnc;     /**<PLMN-MNC value Mobile Network Code [range 0..999]. Invalid Value -1.*/
    uint16_t lac;     /**<LAC value Location Area Code.*/
    uint32_t cid;     /**<GSM Cell number identifier of the serving cell [range 0..268435455].Invalid Value -1.*/
} t_gpsClient_RefLocationCellID;


typedef struct
{
    uint8_t mac[6];   /**<PLMM-Medium Access Control value.*/
} t_gpsClient_RefLocationMac;


typedef struct
{
    uint16_t type;                                 /**<Reference location type 1-GSM, 2-UMTS, 3-MAC.Invalid value -1.*/
    union
    {
        t_gpsClient_RefLocationCellID   cellID;    /**<Reference location in terms of Cell ID.*/
        t_gpsClient_RefLocationMac      mac;       /**<Reference location in terms of MAC ID.*/
    } u;
} t_gpsClient_RefLocation;

#define GPSCLIENT_REF_LOCATION_TYPE_GSM_CELLID    1
#define GPSCLIENT_REF_LOCATION_TYPE_UMTS_CELLID   2
#define GPSCLIENT_REG_LOCATION_TYPE_MAC           3
#endif

/*@} group gpsClient_InternalDef */

/*! \addtogroup gpsClient_APIs */
/*@{*/

/**
*  @brief Callback with location information in C Structures. This shall be sent in response to the location request gpsClient_PeriodicFix & gpsClient_SingleShotFix.
*   @see t_gpsClient_NmeaData
*   @see gpsClient_PeriodicFix
*   @see gpsClient_SingleShotFix
*/
typedef void (* gpsClient_navigation_callback)(t_gpsClient_NavData *NavData);

/**
*   @brief Callback used to provide location information in NMEA sentences. This shall be sent in response to the location request gpsClient_PeriodicFix & gpsClient_SingleShotFix.
*   @see t_gpsClient_NmeaData
*   @see gpsClient_PeriodicFix
*   @see gpsClient_SingleShotFix
*/
typedef void (* gpsClient_nmea_callback)( t_gpsClient_NmeaData *NmeaData );


/**
*   @brief Callback used to provide GPS START STOP NOTIFICATION in case of NI.
*/

typedef void (* gpsClient_ni_callback)( uint8_t ni_notification );


/**
*  @brief Callback used for Notifying user of the incoming location request from network.
*         Note : Some of the incoming location request may not be notified as per the settings from network. Such location request will be done
*                without user's knowlede.
*         User may accept/reject/ignore the request. User response can be sent using gpsclient_NotificationResponse
*  @see gpsclient_NotificationResponse
*  @see t_gpsClient_NotifyData
*/
typedef void (*gpsClient_ni_notify_callback)(t_gpsClient_NotifyData *notification);


/**
*  @brief Callback used to notify the application about the agps connection status. @see e_gpsClient_AgpsStatData
*/
typedef void (*gpsClient_agpsstat_callback)(e_gpsClient_AgpsStatData *agpsStatData);

#ifdef AGPS_ANDR_GINGER_FTR
/**
*  @brief Callback used to request the ril interface for mobile information
*/
typedef void (*gpsClient_ril_callback)( e_gpsClient_NetworkInfoType infoType );

/**
*  @brief Callback used to request the ril interface for SetID information
*/
typedef void (*gpsClient_ril_setid_callback)(void);
#endif  /*AGPS_ANDR_GINGER_FTR*/

/**
*  @brief GPS callback structure related to GPS / AGPS procedures.
*/
typedef struct {
        gpsClient_navigation_callback nav_cb;    /**< @brief Callback for Navigation Data. This must be marked as NULL if reponse is not expected.*/
        gpsClient_nmea_callback       nmea_cb;   /**< @brief Callback for Nmea callback. This must be marked as NULL if reponse is not expected. */
        gpsClient_ni_callback         ni_cb;     /**< @brief Callback for NI callback*/
} t_gpsClient_Callbacks;

/**
*  @brief GPS callback structure related to Notifcation Procedures. User may accept/reject/ignore the request. User response can be sent using gpsclient_NotificationResponse
*  @see gpsclient_NotificationResponse
*/
typedef struct {
        gpsClient_ni_notify_callback notify_cb;  /**< @brief Callback for Notification Data. This must be marked as NULL if reponse is not expected.*/
} t_gpsClient_NiCallbacks;


/**
*  @brief GPS callback structure related to Assisted GPS procedures.
*/
typedef struct {
        gpsClient_agpsstat_callback agpsstat_cb; /**< @brief Callback for AGPS Status. This must be marked as NULL if reponse is not expected.*/
} t_gpsClient_agpsCallbacks;


#ifdef AGPS_ANDR_GINGER_FTR
/**
*  @brief GPS callback structure related to ril related procedures.
*/
typedef struct {
        gpsClient_ril_callback ril_cb;             /**< @brief Callback for AGPS Status. This must be marked as NULL if reponse is not expected.*/
        gpsClient_ril_setid_callback ril_setid_cb; /**< @brief Callback for AGPS Status. This must be marked as NULL if reponse is not expected.*/
} t_gpsClient_RilCallbacks;
#endif /*AGPS_ANDR_GINGER_FTR*/

/*****************************************************************************
* GPS Client Prototypes
*****************************************************************************/

/**
  *  @brief  This function is for initializing the gps client. Thread is created in application process.
  *  @param callbacks          Structure which has pointer to various call back functions
  *  @return                   returns 0 on success
  */
int gpsClient_Init( t_gpsClient_Callbacks *callbacks );


/**
  *  @brief  This function is used to disable the gps client.
  *  @return                   returns 0 on success
  */
int gpsClient_Disable();


/**
  *  @brief  This function is used to start the gps service for a client. This must be called before registering for Single shot or periodic Fix.
  *  @see gpsClient_PeriodicFix, @see gpsClient_SingleShotFix
  *  @return                   returns 0 on success
  */
int gpsClient_ServiceStart(void);

/**
  *  @brief  This function is used to start the gps service for a client. This must be called before registering for Single shot or periodic Fix.
  *  @see gpsClient_PeriodicFix, @see gpsClient_SingleShotFix
  *  @return                   returns 0 on success
  */
int gpsClient_ServiceStart2(t_gpsClient_ApplicationIDInfo *p_Appld_Info);




/**
  *  @brief  Request a periodic positioning fix using GPS. Note. This function must be called after gpsClient_ServiceStart
  *  @param outputType        Type of output expected by client. Should be either NMEA or C Structure refer e_gpsClient_OutputType
      \verbatim
            0 - Nmea
            1 - C Struct
            2 and above:  Reserved.
      \endverbatim
  *  @param nmeaMask          NMEA sentence types required Refer e_gpsClient_NmeaMask
  *  @param fixRate              The rate in which periodic updates to be sent (in milliseconds)
  *  @return                   returns 0 on success
  *  @see e_gpsClient_OutputType
  */
int gpsClient_PeriodicFix(unsigned int outputType, unsigned int nmeaMask, unsigned int fixRate);


/**
  *  @brief  Request a single shot fix using GPS. This function must be called after gpsClient_ServiceStart
  *  @param outputType       Type of output expected by client. Should be either NMEA or C Structure refer e_gpsClient_OutputType
      \verbatim
            0 - Nmea
            1 - C Struct
            2 and above:  Reserved.
      \endverbatim
  *  @param nmeaMask         NMEA sentence types required Refer e_gpsClient_NmeaMask
  *  @param timeOut             Time in which the callback is expected to be called (in milliseconds)
  *  @param horizAccuracy    Horizontal accuracy required in  meters
  *  @param vertAccuracy     Vertical accuracy required in  meters
  *  @param ageLimit         Specifies how old Gps data can be acceptable for the application (in milliseconds).
  *  @return                  returns 0 on success
  *  @see e_gpsClient_OutputType
  */
int gpsClient_SingleShotFix( unsigned int outputType, unsigned int nmeaMask, \
                      unsigned int timeOut, unsigned int horizAccuracy, unsigned int vertAccuracy, unsigned int ageLimit );

/**
  *  @brief  Request a single shot fix using GPS.
  *  @param outputType       Type of output expected by client. Should be either NMEA or C Structure refer e_gpsClient_OutputType
      \verbatim
            0 - Nmea
            1 - C Struct
            2 and above:  Reserved.
      \endverbatim
  *  @param nmeaMask                 NMEA sentence types required Refer e_gpsClient_NmeaMask
  *  @param positioning params     Positioning params required for getting position fix
  *  @param targetArea                Geographical target area  maybe a circle, ellipse and polygon and the boundary defining parameters  for the selected shape
  *  @param speedThreshold         Speed Threshold limit
  *  @param areaEventType          Geographic Area Event type
  *  @return                                 returns 0 on success
  */

int gpsClient_FenceGeographicalArea( uint8_t outputType, uint16_t nmeaMask, t_gpsClient_PositioningParams positioningParams,
                                     const t_gpsClient_GeographicTargetArea* targetArea, uint16_t speedThreshold, e_gpsClient_AreaEventType areaEventType
                                   );


/**
  *  gpsClient_LocationForward() function  << Starts location forward request >>
  *  @brief  This function sends a request to forward location request.
  *
  *  A location is sent to a third party as part of a 3GPP MOLR - TRansfer to third party.

  *  @param qop       Quality of Position
  *  @param clientInfo         Third party client information
  *  @param serviceType       Service Type of Location Request
  *  @return                  returns 0 on success
  */
int gpsClient_LocationForward(  const t_gpsClient_Qop *qop, const t_gpsClient_ThirdClientInfo *clientInfo, e_gpsClient_LcsServiceType serviceType);
/**
  *  gpsClient_LocationRetrieval() function  << Starts Retrieval of location of the third party >>
  *  @brief  This function sends a request to retrieve location of the third party.
  *
  *  A location is sent to a third party as part of a 3GPP MOLR - TRansfer to third party.
  *  @param qop Qop - Quality of position expected from the third party client. Optional, set to NULL if not present.
  *  @param clientInfoClient Info - Client information to identify the third party for which location information is of interest. Mandatory
  *  @return                  returns 0 on success
  */
int gpsClient_LocationRetrieval(const t_gpsClient_Qop *qop, const t_gpsClient_ThirdClientInfo *clientInfo );


/**
  *     @brief  This function sets the user configurations which can be used to enable/disable features and update modes of GPS operations
  *  @param configMaskValid       This field shall be set for all elements whose information is to be updated in v_ConfigMask
      \verbatim
            Bit 0 - MS Assisted
            Bit 1 - MS Based
            Bit 2 - Autonomous
            Bit 3 - Enhanced Cell Id
            Bit 4 - SUPL Support
            Bit 5 - Control Plane
            Bit 6 - User Plane Preferred over Control Plane
            Bit 7 - Mobile Originated Location Supported
            Bit 8 - Mobile Termintaed Location Supported
            Bit 9 - SUPL Multiple Sessionn Supported
            Bit 10 - Network Based Positioning Supported
            Bit 11 - NO TLS Supported
            Bit 12 - SUPL V2 Supported
            Bit 13 - Frequency Aiding
            Bit 14 - Fine Time Aid
            Bit 15 - Semi/Self Auto Mode
            Bit 16 and above - Reserved
      \endverbatim

  *  @param configMask            The element bit in this field shall be set if the corresponding bit is set in configMaskValid
  *                               1- Field is enabled, 0 - Field is disabled.
  *                               The bit field is exactly same as in configMaskValid
  *  @param config2Valid         This field shall based on which all optional element are updated.
      \verbatim
            Bit 0 - SUPL Server Configuration
            Bit 1 - TTFF Sensitivity
            Bit 2 - Power Preferences
            Bit 3 - Sensitivity Mode
            Bit 4 - Cold Start
            Bit 5 - SUPL Bearer Config
            Bits 6-15:  Reserved for future use.
      \endverbatim

  *  @param sensMod
      \verbatim
            0- High Sensitivity
            1- Normal Sensitivity (DEFAULT)
            2- Dynamic Sensitivity
            3 and above:  Reserved.
      \endverbatim

  *  @param sensTTFF
      \verbatim
            0- Balanced Sensitivity & TTFF (DEFAULT)
            1- faster cold start TTFF, but reduced cold start sensitivity
            2- Increased sensitivity cold starts, but at potentially a slower cold start TTFF
            3 and above:  Reserved.
      \endverbatim

  *  @param powerPref
      \verbatim
            0- Default (DEFAULT)
            1- Higher Performance
            2- Low Power Usage
            3 and above:  Reserved.
      \endverbatim

  *  @param coldStart
      \verbatim
            0- Quick Start
            1- Cold Start
            2- Cold Start Always
            3- Warm Start Always
            4- Hot Start Always
            5 and above : Reserved
      \endverbatim

  *  @return                  returns 0 on success
  */
int gpsClient_Set_Configuration(int configMaskValid, int configMask, int config2Valid, unsigned char sensMod, unsigned char sensTTFF, \
            unsigned char powerPref, unsigned char coldStart);

/**
  *     @brief  This function sets the platform configurations which can be used to enable/disable features and update modes of GPS operations
  *  @param Config1MaskValid        This field shall be set for all elements whose information is to be updated in Config1Mask
  *    \verbatim
            Bit 0 - Pure MSA CP
            Bit 1 - Pure MSA SUPL
            Bit 2 - External LNA
      \endverbatim
  *  @param Config1Mask             The element bit in this field shall be set if the corresponding bit is set in Config1MaskValid
  *                                         1- Field is enabled, 0 - Field is disabled.
  *                                         The bit field is exactly same as in Config1MaskValid
  *  @param Config2Mask             This field shall be set for all elements whose information is to be updated
        \verbatim
            Bit 0 - Mask for Mandatory MSA Assistance Data
            Bit 1 - Mask for Mandatory MSB Assistance Data
            Bit 2 - Mask for GPS Power saving Interval
      \endverbatim

  *  @param MsaMandatoryMask            This field defines the Mandatory Assistance Data Mask for MSA.This field will
                                                  be set only if Mandatory MSA Assistance Data bit is set in Config2Mask.

  *  @param MsbMandatoryMask            This field defines the Mandatory Assistance Data Mask for MSB.This field will
                                                  be set only if Mandatory MSB Assistance Data bit is set in Config2Mask.

  *  @param SleepTimerDuration          This field shall set the Sleep Timer duration of GPS. This field will
                                                          be set only if GPS Power saving Int. bit is set in Config2Mask.

  *  @param ComaTimerDuration           This field shall set the Coma Timer duration of GPS. This field will
                                                   be set only if GPS Power saving Int. bit is set in Config2Mask.

  *  @param DeepComaTimerDuration       This field shall set the DeepComa Timer duration of GPS. This field will
                                                     be set only if GPS Power saving Int. bit is set in Config2Mask.

  *  @return                  returns 0 on success
  */


int gpsClient_Set_PlatformConfiguration(int Config1MaskValid, int Config1Mask, int Config2Mask, int MsaMandatoryMask, int MsbMandatoryMask, \
            uint32_t SleepTimerDuration,uint32_t ComaTimerDuration,uint32_t DeepComaTimerDuration);


/**
  *  @brief  This function is used to delete tha corresponding aiding data.
  *  @param aidingDataType   Specifies which aiding data to be deleted. Mask is defined in e_gpsClient_DeleteAssistData
      \verbatim
            Bit 0 - Ephemeris
            Bit 1 - Almanac
            Bit 2 - Reference Position
            Bit 3 - Reference Time
            Bit 4 - Iono Model
            Bit 5 - Utc Model
            Bit 6 - Health
            Bit 7 - SV Direction
            Bit 8 - SV Steering
            Bit 9 - SA Data
            Bit 10 - Real Time Integrity
            Bit 11 - Cell Database
            Bits All: All data.
      \endverbatim
  *  @return                  returns 0 on success
  *  @see e_gpsClient_DeleteAssistData
  */
int gpsClient_DeleteAidingData(uint16_t aidingDataType );


/**
  *  @brief  This function is used to set the corresponding position mode.
  *  @param gpsPositionMode  Specifies the mode of positioning for example autonomous or MS based or MS assisted.
      \verbatim
            0 - Standalone GPS
            1 - MS Based Mode
            2 - MS Assisted Mode
            3 and above: Reserved.
      \endverbatim
  *  @return                  returns 0 on success
  */
int gpsClient_SetPositionMode( uint32_t gpsPositionMode );


/**
  *  @brief  This function stops GPS Client and delete the thread.
  *  @return                 returns 0 on success
  */
int gpsClient_Stop();


#ifdef AGPS_ANDR_CUPCAKE_FTR


/**
  *  @brief  Used to set slp addres
  *  @param ipAddress         address contains the SLP ip address to connect.
  *  @param port            port contains the port number of SLP .
  *  @return                 returns 0 on success
  */
int gpsClient_SlpAddress( unsigned int ipAddress, int port );
#else
/**
  *  @brief  Used to set slp addres
  *  @param hostname         address contains the SLP hostname to connect.
  *  @param port            port contains the port number of SLP .
  *  @return                 returns 0 on success
  */
int gpsClient_SlpAddress( char *hostname , int port );

/**
  *  @brief  This function is used to open a data connection with the network
  *  @param apn  Contains the apn name.
  *  @return                 returns 0 on success
  */
int gpsclient_DataConnOpen( char *apn );


/**
  *  @brief  Used to close a data connection with the network
  *  @return                 returns 0 on success
  */
int gpsclient_DataConnClosed();


/**
  *  @brief  Used to indicate the failure in the data connection
  *  @return                 returns 0 on success
  */
int gpsclient_DataConnFailed();
int gpsclient_DataConnCloseFailed();


/**
  *  @brief  Initializes the agps part for assisted operation and register agps callbacks used to send the data to application
  *  @param agpsCallbacks  callbacks used to send the data to application
  *  @return                         returns 0 on success
  */
int gpsClient_AgpsInit( t_gpsClient_agpsCallbacks *agpsCallbacks );

#endif /* #ifdef AGPS_ANDR_CUPCAKE_FTR */

/**
  *  @brief  Initialize gps client for receiving notification for network initiated location request
  *         Note : Some of the incoming location request may not be notified as per the settings from network. Such location request will be done
  *                without user's knowlede.
  *         User may accept/reject/ignore the request. User response can be sent using gpsclient_NotificationResponse
  *  @param callbacks  Callback used for Notifying user of the incoming location request from network
  *  @return                         returns 0 on success  and returns 4 on failure
  */
int gpsclient_NotificationInit( t_gpsClient_NiCallbacks *callbacks );


/**
  *  @brief  Sends the response to the notification.
  *  @param handle        handle used to indicate the Request for which notification had been sent
  *  @param response    user response for the notification
      \verbatim
            0 - NI accepted by User
            1 - NI Deny from User
            2 - NI No Response from User
            3 and above: Reserved.
      \endverbatim
  *  @return             returns 0 on success and returns 4 on no response
  *  @see gpsClient_ni_notify_callback
  */
int gpsclient_NotificationResponse( int handle , e_gpsClient_UserResponseType response );

/**
  *  @brief  Sends supl init pdu to lbsd.
  *  @param p_Push        supl init pdu
  *  @param v_PushLen     length of supl init pdu
  *  @param p_Hash        SHA1 hash of supl init pdu
  *  @param v_HashLen     length of SHA1 hash of supl init pdu
      \verbatim
            p_Hash can be null indicating no hash available.
            In such a case, v_HashLen should be zero
      \endverbatim
  *  @return             void
  */
void gpsclient_SendSuplPush( char *p_Push , int v_PushLen , char *p_Hash , int v_HashLen );
#ifdef AGPS_ANDR_GINGER_FTR
/**
  *  @brief  Initializes gps client for ril interface callback.
  *  @param callbacks    Pointer to callback, which requests for reference location cell id.
  *  @return             void
  */
void gpsclient_AgpsRilInit( t_gpsClient_RilCallbacks *callbacks );


/**
  *  @brief  Provides cell information and IMSI value.
  *  @param agps_reflocation    Reference location either by CellID or by MAC.
  *  @param sz_struct           Size of t_gpsClient_RefLocation
  *  @param imsi                IMSI value.
  *  @return                    void
  */
void gpsclient_SetMobileInfo( const t_gpsClient_RefLocation *agps_reflocation, size_t sz_struct, const char *imsi );
#endif
/*@} group gpsClient_APIs */


/**
* \enum e_gpsClient_TREQMask Production test request Mask
* Production test request paramater.
* sync with e_cgps_TREQMask
*/
typedef enum
{
    GPSCLIENT_TREQ_SNR = 1,      /**< @brief Mask for TEST Request Input signal SNR */
    GPSCLIENT_TREQ_ME,           /**< @brief Mask for TEST Request ME oscillator offset test */
    GPSCLIENT_TREQ_RTC,          /**< @brief Mask for TEST Request RTC oscillator test */
    GPSCLIENT_TREQ_INPIN,        /**< @brief Mask for TEST Request Input pin test*/
    GPSCLIENT_TREQ_OUTPIN,       /**< @brief Mask for TEST Request Output pin test*/
    GPSCLIENT_TREQ_ROMCHKSUM,    /**< @brief Mask for TEST Request ROM checksum test*/
    GPSCLIENT_TREQ_PATCHCHKSUM,  /**< @brief Mask for TEST Request Patch checksum test*/
    GPSCLIENT_TREQ_BIST,         /**< @brief Mask for TEST Request BIST test*/
    GPSCLIENT_TREQ_SPECANALYZ    /**< @brief Mask for TEST Request Spectrum Analyzer test*/
}e_gpsClient_TREQMask;

typedef enum
{
    GPSCLIENT_GPS_WIDEBAND_SCAN = 1,      /**< @brief TREQ 9 1 test for GPS 4 MHz Wideband Scan */
    GPSCLIENT_GPS_NARROWBAND_SCAN,        /**< @brief TREQ 9 2 test for GPS 30 KHz Narrowband Scan */
    GPSCLIENT_GLONASS_WIDEBAND_SCAN,      /**< @brief TREQ 9 3 test for GLONASS 9.5 MHz Wideband Scan */
    GPSCLIENT_GLONASS_NARROWBAND_SCAN,    /**< @brief TREQ 9 4 test for GLONASS 30 KHz Narrowband Scan */
    GPSCLIENT_READ_PGA_AGC_VALUE,         /**< @brief TREQ 9 5 test for reading RF parameters-PGA(Programmable gain amplifier)/AGC(Automatic Gain Control) values */
    GPSCLIENT_SET_PGA_AGC_VALUE,          /**< @brief TREQ 9 6 test for setting PGA/AGC values */
    GPSCLIENT_RESET_RF                    /**< @brief TREQ 9 7 test for Resetting RF */
}e_gpsClient_SpectrumTestType;


/**
* \struct  t_gpsClient_TestRequest
* Production test request paramater.
* \struct type sync with t_agps_TestRequest and  t_CGPS_TestRequest
*/
typedef struct
{
    uint16_t     v_TestReqMask;                                       /** Test request mask */
    char         v_InputSigFreq[GPSCLIENT_MAX_TREQ_FREQ_LEN];         /** Input signal frequency */
    char         v_RefClockFreq[GPSCLIENT_MAX_TREQ_FREQ_LEN];         /** Reference clock frequency */
    char         v_PgaValues[GPSCLIENT_MAX_SA_TEST_PGA_VALUE_LEN];    /** RF parameter values(PGA/AGC) to be set in Spectrum analyzer test*/
    uint16_t     v_InPinValue;                                        /**< @brief  Input pin mask and Value */
    uint16_t     v_InPinMask;                                         /**< @brief  Input pin mask*/
    uint16_t     v_OutPinValue;                                       /**< @brief  Output pin mask and Value */
    uint16_t     v_OutPinMask;                                        /**< @brief  Output pin mask*/
    uint8_t      v_SpectrumTestType;                                  /** Spectrum analyzer test type */
    uint8_t      v_FileLogResult;                                     /** File logging of results */
}t_gpsClient_TestRequest;

/**
  *  @brief  ProductionTestStartRequest function neeed to paramater for the test
  *  @param t_gpsClient_TestRequest  required for the Test
  *  @return                         returns 0 on success
  */
int gpsClient_ProductionTestStartReq(t_gpsClient_TestRequest prodStartTREQ);


/**
* \struct  t_gpsClient_TestStop
* Production test stop request paramater.
* \struct type sync with t_CGPS_StopReq and  t_agps_StopRequest
*/
typedef struct
{
    uint16_t     v_TestMask;     /** Test request mask */
    uint8_t        v_State;             /** Test Result State */
}t_gpsClient_TestStop;


/**
  *  @brief  gpsClient_ProductionTestStopReq function neeed to paramater for the test
  *  @param t_gpsClient_TREQTestStop  required to test result
  *  @return                         returns 0 on success
  */
int gpsClient_ProductionTestStopReq(t_gpsClient_TestStop testStopReq);


int gpsClient_Set_LoggingConfiguration(
            int ModuleType, int LogType,int LogOpt, int LogFileSize);


int gpsClient_Mobile_Info( int mcc , int mnc , int cellId , int v_Lac , int celltype , const char* imsi , char* msisdn );


#endif /* #ifndef _GPS_CLIENT_H */
