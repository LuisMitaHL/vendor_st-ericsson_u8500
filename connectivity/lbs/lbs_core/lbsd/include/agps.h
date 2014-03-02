/*
* Copyright (C) ST-Ericsson 2009
*
* agps.h
* Date: 17-07-2009
* Author: Anil Kumar
* Email : anil.nambiar@stericsson.com
*/


#ifndef AGPS_H
#define AGPS_H


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>


#define  MAX_GPS_CLIENT                 11
#define  MAX_AGPS_CLIENT                2
#define  BUFFER_SIZE                    1024
#define  INADDR_SERVER_HOST             0x7f000001  //inet_addr("127.0.0.1") localhost
/* +ER:270717 */
#define  PORT                           54532      /* A random port selected which is unused according to IANA*/
/* -ER:270717 */


#define  K_AGPS_NUM_SV                  32
#define  MAX_LENGTH_REQUESTER_ID        75
#define  MAX_LENGTH_CLIENT_NAME         67
#define  MAX_LENGTH_CODE_WORD           75
#define  MAX_LENGTH_PHONE_NUMBER        65
#define  MAX_LENGTH_SLP_ADDRESS         259

#define AGPS_MOBILE_INFO_IMSI_LENGTH    16
#define AGPS_MOBILE_INFO_MSISDN_LENGTH  42

#define MAX_SECONDS_TO_POLL             0xFFFF;

#define MAX_POLYGON_POINTS              15

#define MAX_CLIENT_EXT_ID_LENGTH        72

#define MAX_MLC_NUMBER_LEN              64

#define MAX_APP_PROVIDER_LEN         65

#define MAX_APP_NAME_LEN             65

#define MAX_APP_VERSION_LEN          65

#define MAX_THIRDPARTY_NAME_LEN      40

#define MAX_CONSTANT_NUM_GPS_PRNS      32

#define MAX_CONSTANT_NUM_GLONASS_PRNS  24
/**
* \enum  e_agps_NavDataType
* List of constants used to indicate the type of navigation data.
*/
typedef enum
{
  K_AGPS_NMEA = 0,     /**< This returned value means that navigation data are NMEA type  */
  K_AGPS_C_STRUCT,      /**< This returned value means that navigation data are t_agps_CStructNavData 'C' Struct type */
  /* + LMSqc26087 */
  K_AGPS_NMEA_AND_C_STRUCT
  /* - LMSqc26087 */
} e_agps_NavDataType;


/**
* \enum  e_agps_FixType
* List of constants used to indicate the notification mode.
*/
typedef enum
{
  K_AGPS_SINGLE_SHOT = 0,   /**< This returned value means that notification mode is single shot  */
  K_AGPS_PERIODIC           /**< This returned value means that notification is periodic */
} e_agps_FixType;

/**
* \enum  e_NmeaMask
* this enum is used when calling \ref MC_START_PERIODIC_FIX_REQUEST
*/
typedef enum
{
   K_GPGLL_ENABLE = (1<<0),   /**< Mask for NMEA $GPGLL (Geographic Position - Latitude, Longitude) */
   K_GPGGA_ENABLE = (1<<1),   /**< Mask for NMEA $GPGGA (GPS Fix Data) */
   K_GPGSA_ENABLE = (1<<2),   /**< Mask for NMEA $GPGSA (GNSS DOPS and Active Satellites) */
   K_GPGST_ENABLE = (1<<3),   /**< Mask for NMEA $GPGST (GNSS Pseudorange Error Statistics) */
   K_GPGSV_ENABLE = (1<<4),   /**< Mask for NMEA $GPGSV (GNSS Satellites in View) */
   K_GPRMC_ENABLE = (1<<5),   /**< Mask for NMEA $GPRMC (Recommended Minimum GNSS Sentence) */
   K_GPVTG_ENABLE = (1<<6),   /**< Mask for NMEA $GPVTG (Course Over Ground and Ground Speed) */
   K_GPZCD_ENABLE = (1<<7),   /**< Mask for NMEA $GPZCD (output rate - OS Timestamp for the Fix [seconds]) */
   K_GPZDA_ENABLE = (1<<8),   /**< Mask for NMEA $GPZDA (Time & Date) */
   K_PGNVD_ENABLE = (1<<9)    /**< Mask for NMEA $PGNVD ( proprietary Diagnostics data ) */
} e_agps_NmeaMask;

/**
* \enum  e_agps_NVStoreFields
* this enum is used when calling \ref MC_START_PERIODIC_FIX_REQUEST
*/

typedef enum
{
   K_AGPS_FIELD_ALMANAC         = (1<< 0),
   K_AGPS_FIELD_EPHEMERIS       = (1<< 1),
   K_AGPS_FIELD_POSITION        = (1<< 2),
   K_AGPS_FIELD_TIME            = (1<< 3),
   K_AGPS_FIELD_UTC             = (1<< 4),
   K_AGPS_FIELD_IONO_MODEL      = (1<< 5),
   K_AGPS_FIELD_SV_HEALTH       = (1<< 6),
   K_AGPS_FIELD_TCXO_CALIB      = (1<< 7),
   K_AGPS_FIELD_COLD_START      = (1<<16),
   K_AGPS_FIELD_WARM_START      = (1<<17),
   K_AGPS_FIELD_FACTORY_START   = (1<<18),
   K_AGPS_FIELD_HOT_START       = (1<<19),
   K_AGPS_FIELD_SEED_DATA       = (1<<20),
   K_AGPS_FIELD_LOCID           = (1<<21)
} e_agps_NVStoreFields;


/**
* \enum e_agps_ShapeType
* Constant containing area definition.
*/
typedef enum
{
    K_AGPS_CIRCULAR_AREA,    /**< Describes the Target Area type as Circular */
    K_AGPS_ELLIPTICAL_AREA,  /**< Describes the Target Area type as Elliptical */
    K_AGPS_POLYGON_AREA,     /**< Describes the Target Area type as Polygon */
    K_AGPS_MAX_SHAPE_TYPE = 0x7FFFFFFF
}e_agps_ShapeType;

/**
* \enum  e_agps_AreaEventType
* List of constants used to define the events that requires reporting.
*/
typedef enum
{
    K_AGPS_ENTERING_AREA,   /**< Considering event occurence only in case user is entering defined area */
    K_AGPS_INSIDE_AREA,     /**< Considering event occurence only in case user is inside defined area */
    K_AGPS_OUTSIDE_AREA,    /**< Considering event occurence only in case user is outside defined area */
    K_AGPS_LEAVING_AREA,    /**< Considering event occurence only in case user is leaving defined area */
    K_AGPS_AREA_EVENT_TYPE = 0x7FFFFFFF
} e_agps_AreaEventType;


/**
* \enum t_agps_CommandType
* \brief enum to indicate command type
*/
typedef enum {
    SOCKET_AGPS_STATUS , /* Server < -> Client */ /* This carries error info if any based on what the message contains */
    SOCKET_AGPS_SERVICE_START, /* Client -> Server */
    SOCKET_AGPS_APPLICATIONID_START,/* Client -> Server */
    SOCKET_AGPS_SERVICE_STOP, /* Client -> Server */
    SOCKET_AGPS_SWITCH_OFF_GPS, /* Client -> Server */
    SOCKET_AGPS_PERIODIC_FIX, /* Client -> Server */
    SOCKET_AGPS_SINGLE_SHOT_FIX, /* Client -> Server */
    SOCKET_AGPS_NAVDATA,  /* Server -> Client */
    SOCKET_AGPS_DELETE_AIDING_DATA, /* Client -> Server */
    /* + LMSqc26087 */
    SOCKET_AGPS_NMEA_DATA, /* Server -> Client */
    /* - LMSqc26087 */
    SOCKET_AGPS_FENCE_GEOGRAPHICAL_AREA, /* Client -> Server */
    SOCKET_AGPS_LOCATION_FORWARD, /* Client -> Server */
    SOCKET_AGPS_LOCATION_RETRIEVAL,/* Client -> Server */
    SOCKET_AGPS_NOTIFICATION, /* Client -> Server */
    SOCKET_AGPS_NOTIFICATION_USER_RESPONSE, /* Server -> Client */
    SOCKET_AGPS_SET_USER_CONFIG, /* Client -> Server */
    SOCKET_AGPS_USER_CONFIG_REQ, /* Client -> Server  */
    SOCKET_AGPS_USER_CONFIG_RSP, /* Server -> Client */
    SOCKET_AGPS_START_BEARER_REQ, /* Server -> Client */
    SOCKET_AGPS_START_BEARER_CNF, /* Client -> Server */
    SOCKET_AGPS_START_BEARER_ERR, /* Client -> Server */
    SOCKET_AGPS_CLOSE_BEARER_REQ, /* Server -> Client */
    SOCKET_AGPS_CLOSE_BEARER_IND, /* Client -> Server */
    SOCKET_AGPS_CLOSE_BEARER_CNF, /* Client -> Server */
    SOCKET_AGPS_CLOSE_BEARER_ERR, /* Client -> Server */
    SOCKET_AGPS_SUPL_TCPIP_CONNECT_REQ, /* Server -> Client */ /** \ref t_agps_TcpIpConnectReq */
    SOCKET_AGPS_SUPL_TCPIP_CONNECT_CNF, /* Client -> Server */ /** \ref t_agps_TcpIpConnectCnf */
    SOCKET_AGPS_SUPL_TCPIP_CONNECT_ERR, /* Client -> Server */ /** \ref t_agps_TcpIpConnectErr */
    SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_REQ, /* Server -> Client */ /** \ref t_agps_TcpIpDisconnectReq */
    SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_IND,/* Client -> Server */ /** \ref t_agps_TcpIpDisconnectInd */
    SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_ERR,/* Client -> Server */ /** \ref t_agps_TcpIpDisconnectErr */
    SOCKET_AGPS_SUPL_TCPIP_DISCONNECT_CNF,/* Client -> Server */ /** \ref t_agps_TcpIpDisconnectCnf */
    SOCKET_AGPS_SUPL_MOBILE_INFO_IND,/* Client -> Server */ /** \ref t_agps_MobileInfoInd */
    SOCKET_AGPS_WIRELESS_NETWORK_INFO_IND,/* Client -> Server */ /** \ref t_agps_WLANInfoInd */
    SOCKET_AGPS_SUPL_SMS_PUSH_IND, /* Client -> Server *//** \ref t_agps_SmsPushInd */
    SOCKET_AGPS_SUPL_SEND_DATA_REQ,/* Server -> Client */ /** \ref t_agps_SuplData */
    SOCKET_AGPS_SUPL_SEND_DATA_CNF,/* Client -> Server */ /** \ref t_agps_SuplSendDataCnf */
    SOCKET_AGPS_SUPL_RECEIVE_DATA_IND,/* Client -> Server */ /** \ref t_agps_SuplData */
    SOCKET_AGPS_SUPL_RECEIVE_DATA_CNF, /* Server -> Client *//** \ref t_agps_SuplReceiveDataCnf */
    /*+Anil 19-03-2011*/
    SOCKET_AGPS_SBEE_INITIALIZE,       /* Client -> Server */
    SOCKET_AGPS_SBEE_SET_DEVICE_CHARGING_STATUS, /* Client -> Server */
    SOCKET_AGPS_SBEE_SET_DEVICE_BEARER_STATUS, /* Client -> Server */
    SOCKET_AGPS_SBEE_SET_NETWORK_STATUS, /* Client -> Server */

    SOCKET_AGPS_SBEE_SET_SERVER_CONFIGURATION, /* Client -> Server */
    SOCKET_AGPS_SBEE_SET_USER_CONFIGURATION,  /* Client -> Server */

    SOCKET_AGPS_SBEE_START_EXTENDED_EPHEMERIS,  /* Client -> Server */
    SOCKET_AGPS_SBEE_ABORT_EXTENDED_EPHEMERIS, /* Client -> Server */

    SOCKET_AGPS_SBEE_SET_DATA_CONNECTION_STATUS, /* Client -> Server */
    SOCKET_AGPS_SBEE_SET_USER_RESPONSE, /* Client -> Server */

    SOCKET_AGPS_SBEE_RECV_DATA,  /* Client -> Server */
    SOCKET_AGPS_SBEE_NOTIFY_CONNECTION, /* Server -> Client */
    SOCKET_AGPS_SBEE_REQUEST_DATA_CONNECTION, /* Server -> Client */
    SOCKET_AGPS_SBEE_REQUEST_DATA_DISCONNECT, /* Server -> Client */

    SOCKET_AGPS_SBEE_DATA, /* Server -> Client */
    /*-Anil 19-03-2011*/
/* +LMSqc19820 -Anil */
    SOCKET_AGPS_GET_VERSION, /* Client -> Server */
    SOCKET_AGPS_VERSION, /* Server < -> Client */
/* -LMSqc19820 -Anil */
    SOCKET_AGPS_REGISTER_CLIENT, /* Client -> Server */
    SOCKET_AGPS_MOBILE_INFO_REQ,/* Server -> Client */
    SOCKET_AGPS_WIRELESS_NETWORK_INFO_REQ,/* Server -> Client */
/* + PROD_TEST */
    SOCKET_AGPS_PRODUCTION_TEST_START,
    SOCKET_AGPS_PRODUCTION_TEST_DATA,
/* - PROD_TEST */
    SOCKET_AGPS_SET_LOGGING_CONFIG, /* Client -> Server */
    SOCKET_AGPS_END_COMMAND_TYPE,
    SOCKET_AGPS_SET_PLATFORM_CONFIG, /* Client -> Server */
    SOCKET_AGPS_DEBUG_LOG_START,
    SOCKET_AGPS_DEBUG_LOG_RESP, /* Server -> Client */
    SOCKET_AGPS_NI_NOTIFICATION, /* Client -> Server */
    SOCKET_AGPS_PRODUCTION_TEST_START_REQ,    /* Client -> Server  67 */
    SOCKET_AGPS_PRODUCTION_TEST_STOP_IND,    /* Server -> Client 68 */
    SOCKET_AGPS_REGISTER_EE_CLIENT_CAPABILITIES, /* Client -> Server */
    SOCKET_AGPS_EE_GET_EPHEMERIS_IND ,  /* Server -> Client */
    SOCKET_AGPS_EE_GET_EPHEMERIS_RSP, /* Client -> Server */
    SOCKET_AGPS_EE_GET_REFLOCATION_IND , /* Server -> Client */
    SOCKET_AGPS_EE_GET_REFLOCATION_RSP, /* Client -> Server */
    SOCKET_AGPS_EE_WRITE_BROADCASTEPHEMERIS_REQ, /* Client -> Server */
    SOCKET_AGPS_EE_WRITE_BROADCASTEPHEMERIS_CNF, /* Server -> Client */
    SOCKET_AGPS_EE_MSL_SET_GPSTIME_IND, /* Server -> Client: */          //Used to set GPS time to EE module. This is expected to be sent before Assistance Data is requested.
    SOCKET_AGPS_EE_MSL_SET_GPSTIME_REQ, /* Client -> Server:  */ /*TODO: Doubt: discuss with saswatha*/
    SOCKET_AGPS_EE_MSL_SET_GPSTIME_CNF, /* Server -> Client */
    SOCKET_AGPS_SET_REFERENCE_TIME_IND, /* Client -> Server */
    SOCKET_AGPS_GET_REFERENCE_TIME_REQ, /* Server -> Client: */
    SOCKET_AGPS_DELETE_SEED_REQ, /* Server -> Client: */
    SOCKET_AGPS_EE_SET_BCEDATA_IND, /* Server -> Client: */

}t_agps_CommandType;


typedef BOOL t_agps_SocketStatus;

/**
* \enum t_agps_SocketStatus
* \brief enum to indicate status message from socket
*/
typedef enum {
    SOCKET_AGPS_NO_ERROR,
    SOCKET_AGPS_NO_FREE_HANDLE,
    SOCKET_AGPS_SERVICE_NOT_STARTED,
    SOCKET_AGPS_CALLBACK_ALREADY_REGISTERED,
    SOCKET_AGPS_NO_STATUS_AVAILABLE,
    SOCKET_AGPS_INVALID_PARAMETER
} e_agps_Status;


/**
* \enum  e_agps_LcsServiceType
* this enum is used in t_cgps_ClientInfo when calling \ref MC_AGPS_REGISTER_LOCATION_FORWARD
*/
typedef enum{
    K_AGPS_EMERGENCY_SERVICES = 0,       /**< Type of service : EMERGENCY_SERVICES.*/
    K_AGPS_EMERGENCY_ALERT_SERVICES,     /**< Type of service : EMERGENCY_ALERT_SERVICES.*/
    K_AGPS_PERSON_TRACKING,              /**< Type of service : PERSON_TRACKING.*/
    K_AGPS_FLEET_MANAGEMENT,             /**< Type of service : FLEET_MANAGEMENT.*/
    K_AGPS_ASSET_MANAGEMENT,             /**< Type of service : ASSET_MANAGEMENT.*/
    K_AGPS_TRAFFIC_CONGESTION_REPORTING, /**< Type of service : TRAFFIC_CONGESTION_REPORTING.*/
    K_AGPS_ROAD_SIDE_ASSISTANCE,         /**< Type of service : ROAD_SIDE_ASSISTANCE.*/
    K_AGPS_ROUTING_TO_NEAREST_COMMERCIAL_ENTERPRISE, /**< Type of service : ROUTING_TO_NEAREST_COMMERCIAL_ENTERPRISE .*/
    K_AGPS_NAVIGATION,                  /**< Type of service : NAVIGATION.*/
    K_AGPS_CITY_SIGHTSEEING,             /**<K_AGPS_CITY_SIGHTSEEING .*/
    K_AGPS_LOCALIZED_ADVERTISING,        /**< Type of service : LOCALIZED_ADVERTISING.*/
    K_AGPS_MOBILE_YELLOW_PAGES,         /**< Type of service : MOBILE_YELLOW_PAGES.*/
    K_AGPS_MAX_LCSSERVICETYPE = 0x7FFFFFFF /**< Type of service : MAX_LCSSERVICETYPE.*/
}e_agps_LcsServiceType;

/**
* \enum  t_agps_third_party_id
* this enum is used in t_cgps_ClientInfo when calling \ref MC_AGPS_REGISTER_LOCATION_FORWARD
*/
typedef enum
{
    K_AGPS_ThirdPartyID_NOTHING=0,           /**< No Valid Third Party ID Present*/
    K_AGPS_ThirdPartyID_logicalName,         /**<Third Party ID Logical Name*/
    K_AGPS_ThirdPartyID_msisdn,              /**<Third Party ID MSISDN*/
    K_AGPS_ThirdPartyID_emailaddr,           /**< Third Party ID Email Address*/
    K_AGPS_ThirdPartyID_sip_uri,             /**<  Third Party ID SIP URI*/
    K_AGPS_ThirdPartyID_ims_public_identity, /**< Third Party ID IMS Public Identity*/
    K_AGPS_ThirdPartyID_min,                 /**< Third Party ID MIN*/
    K_AGPS_ThirdPartyID_mdn,                 /**< Third Party ID MDN*/
    K_AGPS_ThirdPartyID_uri,                 /**< Third Party ID URI*/
    K_AGPS_ThirdPartyID_MAX_IND = 0x7FFFFFFF /**<MAX Third Party ID Value*/
}t_agps_third_party_id;


/**
* \enum e_agps_DataTypes
* \brief enum to represent data type used in agps
*/
typedef enum
{
    AGPS_U32_TYPE,
    AGPS_U16_TYPE,
    AGPS_U8_TYPE,
    AGPS_U8_ARRAY_TYPE,
} e_agps_DataTypes;

/**
* \enum t_agps_thirdparty_info
* \brief enum to represent the third party Info details
*/
typedef struct
{
    uint32_t      v_ThirdPartyId;                            /**<Third Party ID*/
    uint8_t       a_ThirdPartyName[MAX_THIRDPARTY_NAME_LEN]; /**<Third Party Name*/
}t_agps_thirdparty_info;


/**
* \struct t_agps_Packet
* \brief structure to represent data packet
*/
typedef struct {
    int       length;
    void *    pData;
}t_agps_Packet;


/* +LMSqc19820 -Anil */
/**
* \struct t_agps_Version
* \brief structure to represent Server version
*/
typedef struct {
    uint8_t data[64];
}t_agps_Version;
/* +LMSqc19820 -Anil */


/**
* \struct t_agps_PeriodicFixCommand
* \brief structure to represent periodic fix command
*/
typedef struct {
    uint8_t         outputType;
    e_agps_NmeaMask nmeaMask;
    uint32_t        fixRate;
}t_agps_PeriodicFixCommand;


/**
* \struct t_agps_SingleShotFixCommand
* \brief structure to represent singleshot fix command
*/
typedef struct {
    uint8_t         outputType;
    e_agps_NmeaMask nmeaMask;
    uint32_t        timeout;
    uint16_t        horizAccuracy;
    uint16_t        vertAccuracy;
    uint32_t        ageLimit;
}t_agps_SingleShotFixCommand;



typedef struct
{
    uint8_t    v_ClientIdTonNpi;          /**< Client ID Type of Number and number Plan identification. Refer 3GPP TS 24.008, section 10.5.4.7. */
    uint8_t    v_ClientIdConfig;          /**<Indicates if v_TonNpi Client Id has been populated.  */
    uint8_t    v_MlcNumTonNpi;            /**< MLC Number Type of Number and number Plan identification. Refer 3GPP TS 24.008, section 10.5.4.7. */
    uint8_t    v_MlcNumConfig;            /**< Indicates if v_TonNpi MLC Number has been populated.    */
    uint8_t    v_ClientExtIdLen;          /**< Requestor Id string length */
    uint8_t    v_MlcNumberLen;            /**< Client Name string length */
    uint8_t    v_Filler[2];               /*Padding Bytes*/
    int8_t     a_ClientExtId[ MAX_CLIENT_EXT_ID_LENGTH ];      /**< Requestor Id string */
    int8_t     a_MlcNumber  [ MAX_MLC_NUMBER_LEN ];            /**< Client Name string */
    t_agps_thirdparty_info    ThirdPartyInfo;
} t_agps_ClientInfo;


/**
* \struct t_agps_SingleShotFixCommand
* \brief structure to represent singleshot fix command
*/
typedef struct
{
  uint32_t     vp_Timeout;              /**< Response Time */
  uint16_t     vp_HorizAccuracy;        /**< Horizontal Accuracy */
  uint16_t     vp_VertAccuracy;         /**< Vertical Accuracy */
  uint32_t     vp_AgeLimit;             /**< Age Limit */
} t_agps_QoP;



/**
* \struct t_agps_SingleShotFixCommand
* \brief structure to represent singleshot fix command
*/
typedef struct {
    t_agps_QoP qop;
    t_agps_ClientInfo clientInfo;
    e_agps_LcsServiceType serviceType;
}t_agps_LocationForwardCommand;


/**
* \struct t_agps_PositioningParamss
* \brief Structure with the Quality of Position exposed
*/
typedef struct
{
    uint32_t     v_Timeout;         /**< Response Time in milli seconds [range 1..3600000ms]. 3600000 ms = 1hour.
                                         Value applicable only if v_NumberOfFixes = 1*/
    uint32_t     v_AgeLimit;        /**< Age Limit in milliseconds. Too old data can't be delivered. 0 value sends only a new position fix. */
    uint32_t     v_FixInterval;     /** <Fix Interval in seconds [range 100..604800000ms]. 604800000 = 1 week in milli seconds. Value applicable only if v_NumberOfFixes > 1*/
    uint32_t     v_StartTime;       /**< Start Time in seconds [range 1 to 2678400s]. If value set to 0, reporting started immediately
                                         Value applicable only if v_NumberOfFixes > 1*/
    uint32_t     v_StopTime;        /**< Stop Time in seconds [range 1 to 11318399s]. If set to 0, value considered not set.
                                         Value applicable only if v_NumberOfFixes > 1.
                                         Stop Time > Start Time. Start Time - Stop Time <= 8639999s (100 days)
                                         If stop time > (number of fixes * fix interval) + start time, then stop time is ignored */
    uint16_t     v_NumberOfFixes;   /**< Number of Fixes to be reported. [range 1..65535 ]  */
    uint16_t     v_HorizAccuracy;   /**< Horizontal Accuracy [range 1.. 65535 m]. If value set to 0 default of 100m is considered*/
    uint16_t     v_VertAccuracy;    /**< Vertical Accuracy [range 1.. 65535 m]. If value set to 0 default of 150m is considered*/
    uint16_t     v_Padding;         /*!< Padding to align on 32 */
}t_agps_PositioningParams;


/**
* \struct t_agps_GeographicTargetArea
* \brief Internal context dedicated to SUPL
*/
typedef struct
{
    e_agps_ShapeType  v_ShapeType;                /**< Describes the Shape Type  */
    uint8_t           v_NumberOfPoints;           /**< Number of points in polygon. [range 3..15]. Value applicable only if Shape Type is polygon  */
    uint8_t           v_EllipSemiMinorAngle;      /**< Angle between the semi-major axis and North, increasing in a clockwise direction in degrees [range 0..179]
                                                       Angle 0 represents ellipse with semi-major axis pointing North/South
                                                       while angle of 90 represents an ellipse with semi-major axis pointing East/West.
                                                       Value applicable only if Shape Type is ellipse. */
    uint8_t           v_Filler[2];                /*Padding Bytes*/
    uint8_t           v_LatSign[MAX_POLYGON_POINTS];                  /**< Array of Latitude Sign. [0=North, 1=South]
                                                       In case of Shape Type is circle/ellipse, latitude sign is a single element that of center.
                                                       In case of Shape Type is polygon, array contains a sequnce of latitude signs of all points */
    uint32_t          v_Latitude[MAX_POLYGON_POINTS];                 /**< Array of Latitudes. [range 0..8388607 for 0..90 degrees]
                                                       In case of Shape Type is circle/ellipse, latitudes is a single element that of center.
                                                       In case of Shape Type is polygon, array contains a sequnce of latitudes of all points */
    int32_t           v_Longitude[MAX_POLYGON_POINTS];                /**< Array of Longitudes. [range -8388608..8388607 for -180..+180 degrees]
                                                       In case of Shape Type is circle/ellipse, longitudes is a single element that of center.
                                                       In case of Shape Type is polygon, array contains a sequnce of longitudes of all points */
    uint32_t          v_CircularRadius;           /**< Radius of the circle in meters. [range 1..1000000m].
                                                       Value valid only if Shape Type is circle*/
    uint32_t          v_CircularRadiusMin;        /**< Hysteresis Minimum Radius of the circle in meters [range 0..1000000m]
                                                       Value valid only if Shape Type is circle. Considered not set if 0*/
    uint32_t          v_CircularRadiusMax;        /**< Hysteresis Maximum Radius of the circle in meters [range 0..1500000m]
                                                       Value valid only if Shape Type is circle. Considered not set if 0*/
    uint32_t          v_EllipSemiMajor;           /**< Semi Major of Ellipse in meters. [range 1..1000000m].
                                                       Value valid only if Shape Type is ellipse */
    uint32_t          v_EllipSemiMajorMin;        /**< Hysteresis Minimum Semi Major of Ellipse in meters [range 0..1000000m]
                                                       Value valid only if Shape Type is ellipse. Considered not set if 0*/
    uint32_t          v_EllipSemiMajorMax;        /**< Hysteresis Maximum Semi Major of Ellipse in meters [range 0..1500000m]
                                                       Value valid only if Shape Type is ellipse. Considered not set if 0*/
    uint32_t          v_EllipSemiMinor;           /**< Semi Minor of Ellipse in meters. [range 1..1000000m].
                                                       Value valid only if Shape Type is ellipse.*/
    uint32_t          v_EllipSemiMinorMin;        /**< Hysteresis Minimum Semi Minor of Ellipse in meters [range 0..1000000m]
                                                       Value valid only if Shape Type is ellipse. Considered not set if 0*/
    uint32_t          v_EllipSemiMinorMax;        /**< Hysteresis Maximum Semi Minor of Ellipse in meters [range 0..1500000m]
                                                       Value valid only if Shape Type is ellipse. Considered not set if 0*/
    uint32_t          v_PolygonHysteresis;        /**< Hysteresis of polygon in meters. [range 0..100000m].
                                                       Value valid only if Shape Type is polygon. Considered not set if 0*/
}t_agps_GeographicTargetArea;




/**
* \struct t_agps_FenceGeographicalAreaCommand
* \brief structure to represent singleshot fix command
*/
typedef struct {
    uint8_t                       v_OutputType;        /**< Indicates the type of data returned to the callback (NMEA or 'C' struct) */
    uint16_t                      v_NmeaMask;          /**< Indicates the Nmea mask */
    t_agps_PositioningParams      v_PositioningParams; /**< Indicates the Positioning params required for getting position fix */
    t_agps_GeographicTargetArea   v_TargetArea;        /**< Indicates the target area type (circle, ellipse and polygon )*/
    uint16_t                      v_SpeedThreshold;    /**< Indicates the Speed Threshold limit */
    e_agps_AreaEventType          v_AreaEventType;     /**< Indicates the Area Event type(entering,leaving,inside or outside) */
}t_agps_FenceGeographicalAreaCommand;

/**
* \struct t_agps_ApplicationIDInfo
* \brief structure to represent Application Id Info
*/
typedef struct
{
   uint8_t   v_ApplicationIDInfoPresent;                   /**< Application Id Information Present or Not */
   int8_t    v_ApplicationProvider[MAX_APP_PROVIDER_LEN];  /**< Application Provider string  */
   int8_t    v_ApplicationName[MAX_APP_NAME_LEN];          /**< Application Name string  */
   int8_t    v_ApplicationVersion[MAX_APP_VERSION_LEN];    /**< Application Version string  */

}t_agps_ApplicationIDInfo;

/**
* \struct t_agps_LocationRetrieveCommand
* \brief structure to represent Third Party Location Retrieval
*/
typedef struct {
    t_agps_QoP qop;
    t_agps_ClientInfo clientInfo;
    t_agps_ApplicationIDInfo appld_Info;
}t_agps_LocationRetrieveCommand;

/**
* \enum  SlpAddressType
* List of constants used to indicate the type of slp address
*/
typedef enum
{
    K_AGPS_SLP_ADDRESS_DEFAULT,
    K_AGPS_SLP_ADDRESS_FQDN,
    K_AGPS_SLP_ADDRESS_IPV4,
    K_AGPS_SLP_ADDRESS_IPV6
}e_agps_SlpAddressType;


/**
* \enum  e_agps_AndroidPositionMode
* List of constants used to indicate the position mode - used by android
*/
typedef enum
{
    K_AGPS_CONFIG_MS_POS_TECH_MSASSISTED    = (1<<0),   /**< Mask for POS TECHNOLOGY-MS ASSISTED */
    K_AGPS_CONFIG_MS_POS_TECH_MSBASED       = (1<<1),   /**< Mask for POS TECHNOLOGY-MS BASED */
    K_AGPS_CONFIG_MS_POS_TECH_AUTONOMOUS    = (1<<2),   /**< Mask for POS TECHNOLOGY-AUTONOMOUS */
    K_AGPS_CONFIG_MS_POS_TECH_ECID          = (1<<3),   /**< Mask for POS TECHNOLOGY-Enhanced Cell Id */
    K_AGPS_CONFIG_TRANSPORT_UPLANE          = (1<<4),   /**< Mask for User Plane Support */
    K_AGPS_CONFIG_TRANSPORT_CPLANE          = (1<<5),   /**< Mask for Control Plane Support */
    K_AGPS_CONFIG_TRANSPORT_PREF_UPLANE     = (1<<6),   /**< Mask for Preference to User Plane over Control Plane */
    K_AGPS_CONFIG_SESSION_MO_ALLOWED        = (1<<7),   /**< Mask for Mobile Originated Location Session */
    K_AGPS_CONFIG_SESSION_MT_ALLOWED        = (1<<8),   /**< Mask for Mobile Terminated Location Session */
    K_AGPS_CONFIG_SUPL_MULTI_SESSION        = (1<<9),   /**< Mask for SUPL multi session */
}e_agps_AndroidPositionMode;

/**
* \enum  e_cgps_Config2BitMask
* this enum is used in t_cgps_Config when calling \ref MC_CGPS_UPDATE_USER_CONFIGURATION
*/
typedef enum
{

   K_AGPS_CONFIG2_SUPL_SERVER_CONFIG      = (1<<0),   /**< Mask for SUPL Server configuration */
   K_AGPS_CONFIG2_TTFF_SENSITIVITY        = (1<<1),   /**< Mask for TTFF sensitivity */
   K_AGPS_CONFIG2_POWER_PREFERENCE        = (1<<2),   /**< Mask for Power Preference */
   K_AGPS_CONFIG2_SENSITIVITY_MODE        = (1<<3),   /**< Mask for Sensitivity Mode */
   K_AGPS_CONFIG2_COLD_START_MODE         = (1<<4),   /**< Mask for Cold Start Mode */
/* ++ LMSqb89864 - BNS 27 / 04 / 2009*/
   K_AGPS_CONFIG2_SUPL_BEARER_CONFIG      = (1<<5),   /**< Mask for SUPL bearer configuration */
/* -- LMSqb89864 - BNS 27 / 04 / 2009*/
} e_agps_Config2BitMask;


typedef struct {
    uint16_t portNumber;
    e_agps_SlpAddressType type;
    int8_t address[ MAX_LENGTH_SLP_ADDRESS ];
}t_agps_SlpAddress;


/**
* \struct t_AgpsConfiguration
* \brief structure to represent gps configuration
*/
typedef struct {
    uint32_t configMaskValid;
    uint32_t configMask;
    uint8_t  config2Valid;
    uint8_t  sensMod;
    uint8_t  sensTTFF;
    uint8_t  powerPref;
    uint8_t  coldStart;
    t_agps_SlpAddress slpAddress;
}t_agps_Configuration;


typedef struct{
    int8_t v_Version[50];        /**<PlatformConfiguration file version*/
    uint16_t v_Config1MaskValid;    /** This field shall be set for all elements whose information is to be updated in v_Config1Mask */
    uint16_t v_Config1Mask;       /** The element bit in this field shall be set if the corresponding bit is set in v_Config1MaskValid*/
    uint16_t v_Config2Mask;    /**<This field shall based on which all optional element are updated*/
    uint16_t v_MsaMandatoryMask;    /**<This field defines the Mandatory Assistance Data Mask for MSA.This field will
                                                        be set only if Mandatory MSA Assistance Data bit is set in v_Config2Mask.*/
    uint16_t v_MsbMandatoryMask;    /**<This field defines the Mandatory Assistance Data Mask for MSB.This field will
                                                        be set only if Mandatory MSB Assistance Data bit is set in v_Config2Mask.*/
    uint32_t v_SleepTimerDuration;      /**<This field shall set the Sleep Timer duration of GPS. This field will
                                                            be set only if GPS Power saving Int. bit is set in v_Config2Mask.*/
    uint32_t v_ComaTimerDuration;       /**<This field shall set the Coma Timer duration of GPS. This field will
                                                        be set only if GPS Power saving Int. bit is set in v_Config2Mask.*/
    uint32_t v_DeepComaDuration;        /**<This field shall set the DeepComa Timer duration of GPS. This field will
                                                            be set only if GPS Power saving Int. bit is set in v_Config2Mask.*/
}t_agps_PlatformConfiguration;


/**
* \enum e_lbscfg_LogModuleType
* \brief Num to provide module log
*/

typedef enum
{
   K_LBS_MODULE_FSM,      /**< FSM Logging Configuration*/
   K_LBS_MODULE_CGPS,     /**< CGPS Logging Configuration*/
   K_LBS_MODULE_HALGPS,   /**< HALGPS Logging Configuration*/
   K_LBS_MODULE_CMCC,     /**< CMCC Logging Configuration*/
   K_LBS_MODULE_LSIM,     /**< LSIM Logging Configuration*/
   K_LBS_MODULE_SBEE,     /**< SBEE Logging Configuration*/
   K_LBS_MODULE_SUPL,     /**< SUPL Logging Configuration*/
   K_LBS_MODULE_CHIP_LOG1,/**< CHIP LOG 1 Logging Configuration*/
   K_LBS_MODULE_CHIP_LOG2,/**< CHIP LOG 2 Logging Configuration*/
   K_LBS_MODULE_CHIP_LOG3,/**< CHIP LOG 3 Logging Configuration*/
   K_LBS_MODULE_CHIP_LOG4,/**< CHIP LOG 4 Logging Configuration*/

   K_LBS_MODULE_MAX   /**< This should be last in the enum*/
} e_agps_LogModuleType;


/**
* \struct t_AgpsLoggingConfiguration
* \brief structure to represent gps configuration
*/
typedef struct {
    e_agps_LogModuleType v_Moduletype;
    uint8_t              v_LogType;        /**< Message Type - Application,File,None \ref e_lbsconfig_LogType*/
    uint16_t             v_LogFileSize;    /**< Maximum File size*/
    uint8_t              v_LogOpt;         /**< Source Type -E-Error,W-Warnings,I-Information,D-Debug,F-Function,T-Timestamp
                                                                       \ref e_lbsconfig_LogSeverity*/
}t_agps_LoggingConfiguration;


/**
* \struct t_agps_Command
* \brief structure to represent a command
*/
typedef struct {
    t_agps_CommandType type;
    t_agps_Packet *data;
}t_agps_Command;


/**
* \enum  t_agps_FdType
* List of constants used to indicate the type of file descriptor
*/
typedef enum {
    FD_TYPE_NORMAL,
    FD_TYPE_NOTIFICATION
}t_agps_FdType;



/**
* \struct t_agps_CallbackMap
* \brief structure to map file descriptor and callbacks
*/
typedef struct {
    int fd;
    uint32_t handle;
    int status;
    t_agps_FdType type;
}t_agps_CallbackMap;



/**
* \struct t_agps_CStructNavData
* \brief Structure with the navigation data
*/
typedef struct /** t_agps_CStructNavData */
{
  uint32_t v_LocalTTag;                      /**< Local baseband millisecond time-tag [msec] */
  uint32_t v_OSTimeMs;                       /**< Local Operating System Time [msec] */
  uint16_t v_Year;                           /**< UTC Year A.D. [eg 2007]*/
  uint16_t v_Month;                          /**< UTC Month into the year [range 1..12] */
  uint16_t v_Day;                            /**< UTC Days into the month [range 1..31] */
  uint16_t v_Hours;                          /**< UTC Hours into the day [range 0..23] */
  uint16_t v_Minutes;                        /**< UTC Minutes into the hour [range 0..59] */
  uint16_t v_Seconds;                        /**< UTC Seconds into the hour [range 0..59] */
  uint16_t v_Milliseconds;                   /**< UTC Milliseconds into the second [range 0..999] */
  int16_t  v_GpsWeekNo;                      /**< GPS Week Number */
  double   v_GpsTOW;                         /**< Corrected GPS Time of Week [seconds] */
  double   v_UtcCorrection;                  /**< Current (GPS-UTC) time zone difference [seconds] */
  double   v_X;                              /**< WGS84 ECEF X Cartesian coordinate [m]. */
  double   v_Y;                              /**< WGS84 ECEF Y Cartesian coordinate [m]. */
  double   v_Z;                              /**< WGS84 ECEF Z Cartesian coordinate [m]. */
  double   v_Latitude;                       /**< WGS84 Latitude  [degrees, positive North] */
  double   v_Longitude;                      /**< WGS84 Longitude [degrees, positive East] */
  float    v_AltitudeEll;                    /**< Altitude above the WGS84 Ellipsoid [m] */
  float    v_AltitudeMSL;                    /**< Altitude above Mean Sea Level [m] */
  float    v_SpeedOverGround;                /**< 2-dimensional Speed Over Ground [m/s] */
  float    v_CourseOverGround;               /**< 2-dimensional Course Over Ground [degrees] */
  float    v_VerticalVelocity;               /**< Vertical velocity [m/s, positive Up] */
  float    v_NAccEst;                        /**< Northing RMS 1-sigma (67%) Accuracy estimate [m] */
  float    v_EAccEst;                        /**< Easting  RMS 1-sigma (67%) Accuracy estimate [m] */
  float    v_VAccEst;                        /**< Vertical RMS 1-sigma (67%) Accuracy estimate [m] */
  float    v_HAccMaj;                        /**< Horizontal RMS 1-sigma (67%) Error ellipse semi-major axis [m] */
  float    v_HAccMin;                        /**< Horizontal RMS 1-sigma (67%) Error ellipse semi-minor axis [m] */
  float    v_HAccMajBrg;                     /**< Bearing of the Horizontal Error ellipse semi-major axis [degrees] */
  float    v_HVelAccEst;                     /**< Horizontal Velocity RMS 1-sigma (67%) Accuracy estimate [m/s] */
  float    v_VVelAccEst;                     /**< Vertical Velocity RMS 1-sigma (67%) Accuracy estimate [m/s] */
  float    v_COG_AccEst;                     /**< Course Over Ground 1-sigma (i.e. rms error) Accuracy estimate [degrees]*/
  float    v_PRResRMS;                       /**< Standard deviation of the Pseudorange a posteriori residuals [m] */
  float    v_HDOP;                           /**< HDOP (Horizontal Dilution of Precision) */
  float    v_VDOP;                           /**< VDOP (Vertical Dilution of Precision) */
  float    v_PDOP;                           /**< PDOP (3-D Position Dilution of Precision) */
  uint8_t  v_FixType;                        /**< Position Fix Type */
  uint8_t  v_Valid2DFix;                     /**< Is the published 2D position fix "Valid" relative to the required Horizontal Accuracy Masks ? */
  uint8_t  v_Valid3DFix;                     /**< Is the published 3D position fix "Valid" relative to both the required Horizontal and Vertical Accuracy Masks ? */
  uint8_t  v_FixMode;                        /**< Solution Fixing Mode (1 = Forced 2-D at Mean-Sea-Level, 2 = 3-D with automatic fall back to 2-D mode) */
  uint8_t  v_SatsInView;                     /**< Satellites in View count */
  uint8_t  v_SatsUsed;                       /**< Satellites in Used for Navigation count */
  uint16_t v_Filler1;                        /**< Dummy Bytes to align with 4 bytes*/
  uint8_t  v_SatsInViewConstell[K_AGPS_NUM_SV]; /**< Satellites in View Constellation identifier [ ] */
  uint8_t  v_SatsInViewSVId[K_AGPS_NUM_SV];  /**< Satellites in View SV id number [PRN] */
  uint8_t  v_SatsInViewSNR[K_AGPS_NUM_SV];   /**< Satellites in View Signal To Noise Ratio [dBHz] */
  uint8_t  v_SatsInViewJNR[K_AGPS_NUM_SV];
  uint16_t v_SatsInViewAzim[K_AGPS_NUM_SV];  /**< Satellites in View Azimuth [degrees] */
  int8_t   v_SatsInViewElev[K_AGPS_NUM_SV];  /**< Satellites in View Elevation [degrees].  If = -99 then Azimuth & Elevation angles are currently unknown */
  uint8_t  v_SatsInViewUsed[K_AGPS_NUM_SV];  /**< Satellites in View Used for Navigation ? */
  uint8_t  v_Filler2[4];                     /**< Dummy Bytes to align with 4 bytes*/
} t_agps_CStructNavData;


/**
* \struct t_agps_NavData
* \brief Structure which the navigation data passed through \ref t_agps_NavigationCallback
*/
typedef struct
{
  void*               p_NavData;    /**< Pointer to navigation in format t_agps_CStructNavData or directly NMEA data  specified by v_Type */
  uint16_t                 v_Length;     /**< Length of navigation data, sizeof(t_agps_CStructNavData) if p_NavData is a t_agps_CStructNavData or ascii buffer length if p_NavData is a t_agps_NmeaNavData */
  e_agps_NavDataType  v_Type;       /**< Type of Navigation data (NMEA or s_CGPS_NavData 'C' Struct) */
  uint8_t                  v_Padding;
} t_agps_NavData;

/**
* \struct t_agps_U8Array
* \brief structure to represent array of uint8_t type data with the length field which can store the length of the data
*/
typedef struct
{
    uint8_t         *p_Pointer;
    uint16_t        v_Length;
} t_agps_U8Array;

typedef enum
{
  K_AGPS_NONE = 0,                /**<  no encoding type specified */
  K_AGPS_UCS2 = 0,                /**<  ucs2 format */
  K_AGPS_GSM_DEFAULT,             /**<  gsm-default refers to the 7-bit default alphabet and the SMS packing specified in [3GPP 23.038]. */
  K_AGPS_UTF8,                    /**<  utf8 format */
  K_AGPS_DCS_8_BIT_DATA,        /**<  raw 8-bit data. can be ASCII - 8bit */
  K_AGPS_UCS2_COMPRESSED,                  /**<  Compressed ucs2 format */
  K_AGPS_DCS_8_BIT_TEXT,         /**<  GSM 7-bit alphabet padded with MSB 0 to make them 8-bits each. */
  K_AGPS_MAX_ENCODINGTYPE = 0x7F
} e_agps_EncodingType;

typedef enum
{
  K_AGPS_NO_NOTIFICATION_NO_VERIFICATION = 0,        /**< User is neither notified nor asked for verification. */
  K_AGPS_NOTIFICATION_ONLY,                          /**< User is notified but not asked for verification. */
  K_AGPS_NOTIFICATION_AND_VERIFICATION_ALLOWED_NA,   /**< User is notified and asked for verification. Request will be allowed if user does not answer. */
  K_AGPS_NOTIFICATION_AND_VERIFICATION_DENIED_NA,    /**< User is notified and asked for verification. Request will be denied if user does not answer. */
  K_AGPS_PRIVACY_OVERRIDE,                           /**< Is used for preventing notification and verification without leaving any traces.  */
  K_AGPS_NOTIFICATION_CANCEL                 /**< Previous notification with same handle has expired */
} e_agps_NotificationType;

typedef enum
{
  K_AGPS_USER_REJECT = 0,   /**< This returned value means that user has rejected the position request */
  K_AGPS_USER_ACCEPT       /**< This returned value means that user has accepted the position request */
} e_agps_UserAnswer;


typedef enum
{
    K_AGPS_GPS_CONSTEL = 0,        /**< NAVSTAR constellation.*/
    K_AGPS_GLO_CONSTEL               /**< GLONASS constellation. */
} e_agps_ConstelType;



/**
* \struct t_agps_NotificationInd
* \brief structure to represent agps notification
*/
typedef struct {
    uint32_t  v_Handle;
    uint8_t   v_Type;
    uint8_t   v_LcsServiceTypeId;
    uint8_t   v_RequestorIdEncodingType;
    int8_t   a_RequestorId[ MAX_LENGTH_REQUESTER_ID ];
    uint8_t   v_ClientNameEncodingType;
    int8_t   a_ClientName[ MAX_LENGTH_CLIENT_NAME ];
    uint8_t   v_CodeWordEncodingType;
    int8_t   a_CodeWord[ MAX_LENGTH_CODE_WORD ];
    int8_t   a_PhoneNumber[ MAX_LENGTH_PHONE_NUMBER ];
    uint32_t  v_TonNpi;
    uint32_t  v_TonNpiConfig;
}t_agps_NotificationInd;


/**
* \struct t_agps_ExtendedEph
* \brief structure to represent Extended Ephemeris request
*/
typedef struct {
    uint32_t                     v_PrnBitMask;
    e_agps_ConstelType           v_ConstelType;
    uint32_t                     v_EERefGPSTime;
}t_agps_ExtendedEphInd;


/**
* \struct t_agps_NotificationRsp
* \brief structure to represent agps notification response
*/
typedef struct {
    uint32_t  v_Handle;
    uint8_t   v_Answer;
} t_agps_NotificationRsp;

/**
* \struct t_agps_MobileInfoInd
* \brief structure to represent mobile information
*/
typedef struct
{
    uint16_t v_MCC;                      /*!< PLMN-MCC value Mobile Country Code.  INTEGER (0..999). Invalid Value -1*/
    uint16_t v_MNC;                      /*!< PLMN-MNC value Mobile Network Code.  INTEGER (0..999). Invalid Value -1*/
    uint32_t v_CellIdentity;             /*!< GSM Cell number identifier of the serving cell. INTEGER (0..268435455).Invalid Value -1*/
/* ++ LMSqc36741 */
    uint16_t v_Lac;                      /*!< LAC GSM Specific information */
/* -- LMSqc36741 */
    uint8_t  v_CellType;                 /*!< Cell type , 0-GSM , 1-WCDMA , 2-TDSCDMA . Invalid Value -1 */
    char a_IMSI[ AGPS_MOBILE_INFO_IMSI_LENGTH ]; /*!< IMSI . If not valid set to zero */
    char a_MSISDN[ AGPS_MOBILE_INFO_MSISDN_LENGTH ]; /*!< MSISDN . If not valid set to zero */
} t_agps_MobileInfoInd;
/**
* \enum  WlanDeviceType
* List of constants used to indicate the type WLAN devices like 802.11a, 802.11b, 802.11g
*/
typedef enum
{
    K_AGPS_WLAN_802_11A_DEVICE   = 0,     /**< Type of Device : 802.11A device.*/
    K_AGPS_WLAN_802_11B_DEVICE   = 1,     /**< Type of Device : 802.11Bdevice.*/
    K_AGPS_WLAN_802_11G_DEVICE   = 2,     /**< Type of Device : 802.11G device.*/
    K_AGPS_WLAN_UNKOWN_DEVICE    = 0xFF   /**< Type of Device : Unknown Device*/
}e_agps_WlanDeviceType;

/**
* \enum  WlanRtdUnits
* List of constants used to indicate the units of Round Trip Delay vaue and Round trip Delay Accuracy
*/
typedef enum
{
    K_AGPS_WLAN_RTDUNIT_MICROSECONDS           = 0,           /**< Unit of RTD delay in Micro seconds.*/
    K_AGPS_WLAN_RTDUNIT_100NANOSECONDS         = 1,           /**< Unit of RTD delay in 100 Nano seconds*/
    K_AGPS_WLAN_RTDUNIT_10NANOSECONDS          = 2,           /**< Unit of RTD delay in 10 Nano seconds*/
    K_AGPS_WLAN_RTDUNIT_NANOSECONDS            = 3,            /**< Unit of RTD delay in Nano seconds*/
    K_AGPS_WLAN_RTDUNIT_ONE_TENTH_NANOSECONDS   =4,            /**< Unit of RTD delay in Nano seconds*/
    K_AGPS_WLAN_RTDUNIT_INVALID_VALUE          = 0xFFFFFFFF   /**< Unit of RTD delay Invalid Value*/
}WlanRtdUnits;

/**
* \struct t_agps_WLANInfoInd
* \brief structure to represent mobile information
*/
typedef struct
{
    uint16_t v_WLAN_AP_MS_Addr;                      /*!<Media Access Control address of WLAN access point (most significant 16 bits)*/
    uint32_t v_WLAN_AP_LS_Addr;                      /*!<Media Access Control address of WLAN access point (least significant 32 bits)*/
    int16_t  v_WLAN_AP_TransmitPower;                /*!<AP Transmit power in dBm. Optional parameter. INTEGER (-127...128). Invalid Value 0X7FFF*/
    int16_t  v_WLAN_AP_AntennaGain;                  /*!<AP antenna gain in dBi,Optional parameter.  INTEGER (-127...128). Invalid Value 0X7FFF*/
    int16_t  v_WLAN_AP_SNR;                          /*!<AP Signal to Noise received at the SET in dB,Optional parameter.  INTEGER (-127...128). Invalid Value 0X7FFF*/
    uint8_t  v_WLAN_AP_DeviceType;                   /*!<ENUM, 802.11a device or 802.11b device or 802.11g device or Unknown*/
    int16_t  v_WLAN_AP_SignalStrength;               /*!<AP signal strength received at the SET in dBm. INTEGER (-127...128).*/
    uint16_t v_WLAN_AP_Channel;                      /*!<AP channel/frequency of Tx/Rx.*/
    uint32_t v_WLAN_AP_RTDValue;                     /*!<Measured Round Trip Delay value between the SET and AP.*/
    uint8_t  v_WLAN_AP_RTDUnits;                     /*!<Units for Round Trip Delay value and Round Trip Delay accuracy*/
    uint16_t v_WLAN_AP_Accuracy;                     /*!<Round Trip Delay standard deviation in relative units. INTEGER (0...255).*/
    int16_t  v_WLAN_AP_SETTransmitPower;             /*!<SET Transmit power in dBm.*/
    int16_t  v_WLAN_AP_SETAntennaGain;               /*!<SET antenna gain in dBi*/
    int16_t  v_WLAN_AP_SETSNR;                       /*!<SET Signal to Noise received at the AP in dB. */
    int16_t  v_WLAN_AP_SETSignalStrength;            /*!<SET signal strength received at the AP in dBm.*/
} t_agps_WLANInfoInd;

/**
* \struct t_agps_SmsPushInd;]
* \brief structure to represent mobile information
*/
typedef struct
{
    t_agps_U8Array v_SmsPushInd;
    t_agps_U8Array v_Hash;
}t_agps_SmsPushInd;

/**
* \struct t_agps_TcpIpConnectReq;
* \brief Structure defining tcp ip connect req
*/
typedef struct
{
    t_agps_SlpAddress slpAddress;
    uint32_t v_ConnectionHandle;
    uint8_t  v_IsTls;
} t_agps_TcpIpConnectReq;


typedef enum
{
    K_AGPS_ERROR_TYPE_UNKNOWN,        /**< Used when the error type is unknown. */
    K_AGPS_ERROR_DNS_RESOLVE_FAILED,  /**< error not able to solve the resolution name */
    K_AGPS_ERROR_CONNECT_FAILED,          /**< error while connecting the TCP socket  */
    K_AGPS_ERROR_TLS_HANDSHAKE_FAILED
}e_agps_ErrorType;





/**
* \struct t_agps_TcpIpConnectErr;
* \brief structure defining a response for a tcp ip connect req
*/
typedef struct
{
    uint32_t v_ConnectionHandle;
    e_agps_ErrorType v_ErrorType;
} t_agps_TcpIpConnectErr;

/**
* \struct t_agps_TcpIpConnectCnf;
* \brief structure defining a response for a tcp ip connect req
*/
typedef struct
{
    uint32_t v_ConnectionHandle;
} t_agps_TcpIpConnectCnf;


/**
* \struct t_agps_TcpIpDisconnectReq;
* \brief structure defining a tcp ip disconnect req
*/
typedef struct
{
    uint32_t v_ConnectionHandle;
} t_agps_TcpIpDisconnectReq;

/**
* \struct t_agps_TcpIpDisconnectCnf;
* \brief structure defining a tcp ip disconnect response
*/
typedef struct
{
    uint32_t v_ConnectionHandle;
} t_agps_TcpIpDisconnectCnf;

/**
* \struct t_agps_TcpIpDisconnectErr;
* \brief structure defining a tcp ip disconnect response
*/
typedef struct
{
    uint32_t v_ConnectionHandle;
} t_agps_TcpIpDisconnectErr;



/**
* \struct t_agps_TcpIpDisconnectInd;
* \brief structure defining a tcp ip disconnect response
*/
typedef struct
{
    uint32_t v_ConnectionHandle;
} t_agps_TcpIpDisconnectInd;

/**
* \struct t_agps_SuplSendDataCnf;
* \brief structure defining a send data cnf
*/
typedef struct
{
    uint32_t v_ConnectionHandle;
} t_agps_SuplSendDataCnf;

/**
* \struct t_agps_SuplReceiveDataCnf;
* \brief structure defining a tcp ip disconnect response
*/
typedef struct
{
    uint32_t v_ConnectionHandle;
} t_agps_SuplReceiveDataCnf;

/**
* \struct t_agps_SuplData
* \brief structure to represent mobile information
*/
typedef struct
{
    uint32_t        v_ConnectionHandle;
    t_agps_U8Array  v_Data;
} t_agps_SuplData;

/* + PROD_TEST */
typedef struct
{
   uint16_t v_AU;       /**< A,B,C,D,E,F upper limit */
   uint16_t v_SU;       /**< S upper limit */
   uint16_t v_SL;       /**< S lower limit */
   uint16_t v_E3U;      /**< E3 upper limit */
   uint16_t v_E8L1;     /**< E8 lower limit 1 */
   uint16_t v_E8U1;     /**< E8 upper limit 1 */
   uint16_t v_E8L2;     /**< E8 lower limit 2 */
   uint16_t v_E8U2;     /**< E8 upper limit 2 */
   uint16_t v_EAL;      /**< EA lower limit */
   uint16_t v_E9L;      /**< E9 lower limit */
   uint8_t  v_TCXO;     /**< TCXO frequence MHz (16 or 26) */
}t_agps_CnoCriteria;


/**
* \struct t_agps_GPSExtendedEphData
* \brief GPS Extended Ephemeris data from Client
*/
typedef struct
{
    uint8_t     v_Prn;        /*!< Ephemeris PRN or SV. Range 1-32.*/
    uint8_t     v_CAOrPOnL2;    /*!< Only 2 least sig bits used. Not in RXN std ephemeris struct. */
    uint8_t     v_Ura;        /*!< User Range Accuracy index.  See IS-GPS-200 Rev D for index values.*/
    uint8_t     v_Health;     /*!< Corresponds to the SV health value. 6 bits as described within IS-GPS-200 Rev D.*/
    uint8_t     v_L2PData;      /*!< When 1 indicates that the NAV data stream was commanded OFF on the P-code of the L2 channel. Descrete 1/0. */
    uint8_t     v_Ure;        /*!< User Range Error. Indicates EE accuracy. Units: meters.*/
    uint8_t     v_AODO;           /*!< Age Of Data Offset.*/
    int8_t      v_TGD;          /*!< Time Group Delay. Scale: 2^-31. Units: seconds.*/
    int8_t      v_Af2;        /*!< Clock Drift Rate Correction Coefficient. Scale: 2^-55. Units: sec/sec^2. */
    int8_t      v_EphemFit;  /*!< Fit interval relating to the fit interval flag. Typically 4 hrs. */
    uint16_t    v_GpsWeek;     /*!< Extended week number (i.e. > 1024, e.g.1486). */
    uint16_t    v_IoDc;            /*!< Issue Of Data (Clock). */
    uint16_t    v_Toc;         /*!< Time Of Clock or time of week. Scale: 2^4. Units: seconds. */
    uint16_t    v_Toe;         /*!< Time Of Ephemeris. Scale: 2^4. Units: seconds. */
    uint16_t    v_Iode;         /*!< Issue Of Data (Ephemeris). */
    int16_t     v_Af1;        /*!< Clock Drift Correction Coefficient. Scale: 2^-43. Units: sec/sec. */
    int16_t     v_Crs;        /*!< Amplitude of the Sine Harmonic Correction Term to the Orbit Radius. Scale: 2^-5. Units: meters. */
    int16_t     v_DeltaN;    /*!< Mean Motion Difference from Computed Value. Scale: 2^-43. Units: semi-circles/sec. */
    int16_t     v_Cuc;        /*!< Amplitude of the Cos Harmonic Correction Term to the Arguement of Latitude. Scale: 2^-29. Units: radians. */
    int16_t     v_Cus;        /*!< Amplitude of the Sine Harmonic Correction Term to the Argument of Latitude. Scale: 2^-29. Units: radians. */
    int16_t     v_Cic;        /*!< Amplitude of the Cos Harmonic Correction Term to the Angle of Inclination. Scale:    2^-29. Units: radians. */
    int16_t     v_Cis;        /*!< Amplitude of the Sine Harmonic Correction Term to the Angle of Inclination. Scale: 2^-29. Units: radians. */
    int16_t     v_Crc;        /*!< Amplitude of the Cos Harmonic Correction Term to the Orbit Radius. Scale: 2^-5. Units: meters. */
    int16_t     v_IDot;        /*!< Rate of Inclination Angle. Scale: 2^-43. Units: semi-circles/sec. */
    uint32_t    v_E;          /*!< Eccentricity. Scale: 2^-33. Units: N/A - Dimensionless. */
    uint32_t    v_SqrtA;     /*!< Square Root of the Semi-Major Axis. Scale: 2^-19. Units: square root of meters. */
    int32_t     v_Af0;        /*!< Clock Bias Correction Coefficient. Scale: 2^-31. Units: sec. */
    int32_t     v_M0;         /*!< Mean Anomaly at Reference Time. Scale: 2^-31/PI. Units: semi-circles. */
    int32_t     v_Omega0;     /*!< Longitude of Ascending Node of Orbit Plane and Weekly Epoch. Scale: 2^-31/PI. Units: semi-circles. */
    int32_t     v_I0;         /*!< Inclination Angle at Reference Time. Scale: 2^-31/PI. Units: semi-circles. */
    int32_t     v_W;            /*!< Argument of Perigee. Scale: 2^-31/PI. Units: semi-circles. */
    int32_t     v_OmegaDot;    /*!< Rate of Right Ascension. Scale: 2^-43. Units: semi-circles/sec. */

}t_agps_GPSExtendedEphData;


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

}t_agps_GlonassExtendedEphData;


/**
 * \brief
 * Structure to store navigation data elements. Based on 3GPP TS 44.031.
 * Note that the Satellite ID value can be derived from the RXN_FullEph_t
 * prn value (Sat ID with range 0 - 31 = RXN_FullEph_t.prn - 1).
 */
typedef struct
{
    uint8_t                         v_NumEntriesGps;                                   /*!< Number of entries in the list. Range 1 - 32. */
    uint8_t                         v_NumEntriesGlonass;                               /*!< Number of entries in the list. Range 1 - 24. */
    t_agps_GPSExtendedEphData       a_GpsList[MAX_CONSTANT_NUM_GPS_PRNS];              /*!< RXN_FullEph_t elements. */
    t_agps_GlonassExtendedEphData   a_GloList[MAX_CONSTANT_NUM_GLONASS_PRNS];          /*!< RXN_FullEph_GLO_t elements. */
} t_agps_NavDataList;


/**
*  @struct t_agps_NavDataBCE
*  @brief Structure contains the broadcast Ephemeris data to be fed to MSL.
*  @detail This include broadcast ephemeris for GPS & Glonass Satellite Status
*             along with type to identify the constellation.
*/

typedef struct
{
    e_agps_ConstelType              v_ConsteType;
    t_agps_GPSExtendedEphData       v_GpsBCE;                /*!< RXN_FullEph_t elements. */
    t_agps_GlonassExtendedEphData   v_GloBCE;                /*!< RXN_FullEph_GLO_t elements. */
} t_agps_NavDataBCE;

typedef enum /** e_agps_LocationType */
{
   AGPS_LBS_LOC_UNSET = 0,       /**< Mask for Reference Positioning Capabilities */
   AGPS_LBS_LOC_ECEF,       /**< Mask for Server Based Extended Ephemeris Capabilities */
   AGPS_LBS_LOC_LLA,           /**< Mask for Self Asssited Extended Ephemeris Capabilities */
   AGPS_LBS_LOC_BOTH           /**< Mask for Broad Cast Ephemeris Cpabilities */

} e_agps_LocationType;


typedef struct /** t_agpsEEClient_RefPosData */
{
    e_agps_LocationType     v_Type;            /*!< Type. LocationType.*/
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

} t_agps_RefPosData;

typedef struct /** t_agps_RefPosition */
{
    t_agps_RefPosData             v_RefPosData;

} t_agps_RefPosition;


/*XYBRID Integration :194997*/


typedef struct
{
    uint8_t    v_CheckLimit;
    uint16_t   v_CNoValue;
} t_agps_ProductionTestData;
/* - PROD_TEST */

/*IMPORTANT NOTE:
These macros are defined as part of interface document for MSL in EEclient.h.
The order or values of these macros MUST NOT be changed.
K_AGPS_REGISTER_REFPOS_CLIENT                                  = (1<< 7),
K_AGPS_REGISTER_EXTENDED_EPHEMERIS_CLIENT           = (1<< 8),
K_AGPS_REGISTER_SAEE_CLIENT                                     = (1<< 9),
K_AGPS_REGISTER_BCE_CLIENT                                       = (1<< 10)

*/

typedef enum
{
    K_AGPS_REGISTER_FIX_CLIENT                          = (1<< 0),
    K_AGPS_REGISTER_NOTIFICATION_CLIENT                 = (1<< 1),
    K_AGPS_REGISTER_BEARER_CLIENT                       = (1<< 2),
    K_AGPS_REGISTER_DATA_CONN_CLIENT                    = (1<< 3),
    K_AGPS_REGISTER_MOBILE_INFO_CLIENT                  = (1<< 4),
    K_AGPS_REGISTER_SBEE_CLIENT                         = (1<< 5),
    K_AGPS_REGISTER_NI_CLIENT                           = (1<< 6),
    K_AGPS_REGISTER_REFPOS_CLIENT                       = (1<< 7),
    K_AGPS_REGISTER_EXTENDED_EPHEMERIS_CLIENT           = (1<< 8),
    K_AGPS_REGISTER_SAEE_CLIENT                         = (1<< 9),
    K_AGPS_REGISTER_BCE_CLIENT                          = (1<< 10),
    K_AGPS_REGISTER_WIRELESS_NETWORK_CLIENT             = (1<< 11),
} e_agps_RegisterClientMask;


typedef enum
{
  K_AGPS_GPS_ENGINE_ON,
  K_AGPS_GPS_ENGINE_OFF,
  K_AGPS_GPS_NO_ERROR,
  K_AGPS_GPS_ERROR,
  K_AGPS_LAST = 0xFFFFFFFF
} e_agps_GpsStatus;

typedef uint32_t  t_agps_RegisterClientMask;


#define MAX_FREQ_LEN 15
#define MAX_PGA_VALUE_LEN 20

/**
* \struct  t_agps_TestRequest for the Production test start request
* \struct type sync with t_gpsClient_TestRequest t_CGPS_TestRequest
*/
typedef struct
{
    uint16_t v_TestReqMask;                     /**< @brief   Test request mask */
    char     v_InputSigFreq[MAX_FREQ_LEN];      /**< @brief   Input signal frequency */
    char     v_RefClockFreq[MAX_FREQ_LEN];      /**< @brief   Reference clock frequency */
    char     v_PgaValues[MAX_PGA_VALUE_LEN];    /**< @brief   PGA/AGC values to be set in Spectrum analyzer test*/
    uint16_t v_InPinValue;                      /**< @brief   Input pin mask Value */
    uint16_t v_InPinMask;                       /**< @brief   Input pin mask*/
    uint16_t v_OutPinValue;                     /**< @brief   Output pin mask and Value */
    uint16_t v_OutPinMask;                      /**< @brief   Output pin mask*/
    uint8_t  v_SpectrumTestType;                /**< @brief   Spectrum analyser test Command type */
    uint8_t  v_FileLogResult;                   /**< @brief   File logging of results */
}t_agps_TestRequest;

/**
* \struct  t_agps_TREQStopRequest for the Production test stop request
* \struct type sync with t_CGPS_StopReq and t_gpsClient_TestStop
*/
typedef struct
{
    uint16_t    v_TestReqMask;    /**< @brief  Test Request Mask */
    uint8_t        v_State;       /**< @brief  Test Result state    */
} t_agps_StopRequest;


typedef struct /** t_agps_RefTimeData */
{
    uint32_t TOWmSec;    /*!< Time-Of-Week in mSec. */
    uint32_t TOWnSec;    /*!< Time-Of-Week in nSec. Set to 0, if nSec accuracy not supported.*/
    uint32_t TAccmSec;   /*!< Time-Accuracy in mSec. Set to 0, if accuracy cannot be specified.*/
    uint32_t TAccnSec;   /*!< Time-Accuracy in nSec. Set to 0, if nSec accuracy cannot be specified.*/
    uint16_t weekNum;    /*!< Week number in extended format (i.e. > 1024).*/
} t_agps_RefTimeData;



#endif /* AGPS_H */


