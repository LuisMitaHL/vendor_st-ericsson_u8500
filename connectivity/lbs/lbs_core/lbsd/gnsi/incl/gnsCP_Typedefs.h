/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/
#ifndef GNS_CP_TYPEDEFS_H
#define GNS_CP_TYPEDEFS_H

/*! \addtogroup Location_gnsCP_Constants */
/*@{*/


/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsCP_MsgType
 *      List of constant used to define type of message
 */
typedef enum
{
    E_gnsCP_AID_REQ,            /*!< Assistance Requested over CP. Data defined by \ref s_gnsCP_GpsAidRequest */
    E_gnsCP_MEAS_RESULT_IND,    /*!< Measurements available for CP. Data defined by \ref s_gnsCP_GpsMeasResult */
    E_gnsCP_POSN_RESULT_IND,    /*!< Position available for CP. Data defined by \ref s_gnsCP_GpsPosnResult */
    E_gnsCP_ABORT_CNF,            /*!< Abort from Network handled. Data defined by \ref s_gnsCP_GpsAbortCnf */
    E_gnsCP_NOTIFY_RSP,         /*!< Notifycation Response. Data defined by \ref s_gnsCP_GpsNotifyRsp */
    E_gnsCP_MOLR_START_IND,       /*!< Start the MOLR procedure. Data defined by \ref s_gnsCP_GpsMolrStartInd */
    E_gnsCP_MOLR_STOP_IND,     /*!< Abort the ongoing MOLR request. Data defined by \ref s_gnsCP_GpsMolrStopInd */
    E_gnsCP_POSN_CAPABILITY_IND     /*!< Send Capability to Modem. Data defined by \ref s_gnsCP_GpsPosCapabilityInd */
} e_gnsCP_MsgType;

/*!
 *  \enum   e_gnsAS_AssistSrc
 *      List of constant used to define type of message
 */
typedef enum
{
    E_gns_ASSIST_SRC_CP,            /*!< Assistance over CP */
    E_gns_ASSIST_SRC_SUPL,          /*!< Assistance over SUPL  */
    E_gns_ASSIST_SRC_WIFI,          /*!< Assistance over WIFI  */
    E_gns_ASSIST_SRC_APPLI,         /*!< Assistance over APPLI  */
    E_gns_ASSIST_SRC_UNKNOWN,       /*!< Assistance over UKNOWN  */
} e_gnsAS_AssistSrc;


/*-------------------------------------------------------*/
/*!
 *  \enum   e_gnsCP_LcsServiceType
 *  \brief  Control Plane A-GPS LCS service  Type
 */
typedef enum
{
      E_gnsCP_EMERGENCY_SERVICES = 0,          /**< Type of service : EMERGENCY_SERVICES.*/
      E_gnsCP_EMERGENCY_ALERT_SERVICES,       /**< Type of service : EMERGENCY_ALERT_SERVICES.*/
      E_gnsCP_PERSON_TRACKING,                      /**< Type of service : PERSON_TRACKING.*/
      E_gnsCP_FLEET_MANAGEMENT,                      /**< Type of service : FLEET_MANAGEMENT.*/
      E_gnsCP_ASSET_MANAGEMENT,                     /**< Type of service : ASSET_MANAGEMENT.*/
      E_gnsCP_TRAFFIC_CONGESTION_REPORTING, /**< Type of service : TRAFFIC_CONGESTION_REPORTING.*/
      E_gnsCP_ROAD_SIDE_ASSISTANCE,               /**< Type of service : ROAD_SIDE_ASSISTANCE.*/
      E_gnsCP_ROUTING_TO_NEAREST_COMMERCIAL_ENTERPRISE, /**< Type of service : ROUTING_TO_NEAREST_COMMERCIAL_ENTERPRISE .*/
      E_gnsCP_NAVIGATION,                               /**< Type of service : NAVIGATION.*/
      E_gnsCP_CITY_SIGHTSEEING,                       /**<K_AGPS_CITY_SIGHTSEEING .*/
      E_gnsCP_LOCALIZED_ADVERTISING,            /**< Type of service : LOCALIZED_ADVERTISING.*/
      E_gnsCP_MOBILE_YELLOW_PAGES,                /**< Type of service : MOBILE_YELLOW_PAGES.*/
      E_gnsCP_MAX_LCSSERVICETYPE = 0x7FFFFFFF /**< Type of service : MAX_LCSSERVICETYPE.*/
} e_gnsCP_LcsServiceType;



/*-------------------------------------------------------*/
/*!
 *  \enum   e_gnsCP_Positioning_Method_Type
 *  \brief  Control Plane A-GPS Positioning Method Type (MS-Based or MS-Assisted).
 */
typedef enum{
    E_gnsCP_MS_ASSISTED    = 0, /*!< Mobile Assisted Mode of Operation where network provides assistance to the mobile.
                                        The Mobile then provides raw GPS measurement back to the Network which performs
                                        the positioning calculation, and if the mobile requested it, provides the
                                        answer back to the Mobile in MO-LR. */
    E_gnsCP_MS_BASED       = 1  /*!< Mobile Based Mode of Operation where network provides
                                        assistance to the mobile. Mobile does the GPS measurement and provides
                                        the Position to the user. */
} e_gnsCP_Positioning_Method_Type;


/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsCP_PosReportType
 *  \brief  Control Plane A-GPS Positioning Report Type.
 */
typedef enum
{
    E_gnsCP_NO_REPORT       = 0,    /*!< No Positioning Reports are expected from Mobile. */
    E_gnsCP_SINGLE_SHOT     = 1,    /*!< Mobile is expected to report a single position report. */
    E_gnsCP_PERIODIC        = 2,    /*!< Mobile is expected to report periodically. */
    E_gnsCP_EVENT_7A        = 3,    /*!< Mobile is expected to provide measurement only when certain threshold is reached.
                                                                    <br> Refer to 3GPP 25.331 for detail on the procedures. */
    E_gnsCP_EVENT_7B        = 4,    /*!< Mobile is expected to provide measurement only when certain threshold is reached.
                                                                    <br> Refer to 3GPP 25.331 for detail on the procedures.*/
    E_gnsCP_EVENT_7C        = 5     /*!< Mobile is expected to provide measurement only when certain threshold is reached.
                                                                    <br> Refer to 3GPP 25.331 for detail on the procedures. */
} e_gnsCP_PosReportType;

/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsCP_PosInstructType
 *  \brief  Control Plane A-GPS Positioning PosInstruct Type.
 */
typedef enum
{
    E_gnsCP_POS_INSTRUCT_HORIZONTAL_ACCURACY  =  0x01,    /*!< Bit 0: Horizontal Accuracy is provided. */
    E_gnsCP_POS_INSTRUCT_VERTICAL_ACCURACY    =  0x02,    /*!< Bit 1: Vertical Accuracy is provided. */
    E_gnsCP_POS_INSTRUCT_TIMEOUT              =  0x04,    /*!< Bit 2: Response Time is provided. */
    E_gnsCP_POS_INSTRUCT_PERIODIC_INTERVAL    =  0x08,    /*!< Bit 3: Periodic Interval is provided. */
    E_gnsCP_POS_INSTRUCT_EVENT_THRESHOLD      =  0x10,    /*!< Bit 4: Event threshold is provided, shall be valid only for Event 7A, 7B or 7C. */
    E_gnsCP_POS_INSTRUCT_AMOUNT_OF_REPORT     =  0x20,    /*!< Bit 5: Number of Reports expected for a session is provided. */
    E_gnsCP_POS_INSTRUCT_MULTIPLE_SET         =  0x40     /*!< Bit 6: Multiple Set Information is provided. */    
} e_gnsCP_PosInstructType;

/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsCP_LocNotificationType
 *  \brief  Control Plane A-GPS Positioning LocNotification Type.
 */
typedef enum
{
    E_gnsCP_LOCATION_CURRENT                 =  00,    /*!< Bit 0: CURRENT */
    E_gnsCP_LOCATION_CURRENT_OR_LAST_KNOWN   =  01,    /*!< Bit 1: CURRENT_OR_LAST_KNOWN */
    E_gnsCP_LOCATION_INITIAL                 =  02,    /*!< Bit 2: INITIAL    */
    E_gnsCP_LOCATION_ACTIVATE_DEFERRED_LOC   =  10,    /*!< Bit 3: ACTIVATE_DEFERRED_LOC  */
    E_gnsCP_LOCATION_CANCEL_DEFERRED_LOC     =  11    /*!< Bit 4: CANCEL_DEFERRED_LOC     */
} e_gnsCP_LocNotificationType;


/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsCP_NavModelSatStatus
 *  \brief  Control Plane A-GPS Assistance Navigation Model Satellite Status.  Used in #s_gnsCP_NavModelElm::v_SatStatus.
 */
typedef enum{
    E_gnsCP_NEW_SV_NEW_NAV_MODEL            = 0,    /*!< New Satellite - New Model. */
    E_gnsCP_EXISTING_SV_EXISTING_NAV_MODEL  = 1,    /*!< Existing Satellite - Existing Model. */
    E_gnsCP_EXISTING_SV_NEW_NAV_MODEL       = 2     /*!< Existing Satellite - New Model. */
} e_gnsCP_NavModelSatStatus;


/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsCP_Status
 *  \brief  Control Plane A-GPS Position or Measurement report Status.  Used in #s_gnsCP_GpsMeasurements::v_Status.
 */
typedef enum{
    E_gnsCP_STATUS_OK       = 0,        /*!< Valid Position or Measurement report. */
    E_gnsCP_STATUS_ERR      = 1         /*!< Invalid Position or Measurement report. */
} e_gnsCP_Status ;


/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsCP_Shape
 *  \brief  Control Plane A-GPS GAD (Geographical Area Description) format Position Shape Type.  Used in  #s_gnsCP_GpsPosition::v_ShapeType.
 */
typedef enum{
    E_gnsCP_SHAPE_E_PT                  =  0,   /*!< Shape Ellipsoid Point. */
    E_gnsCP_SHAPE_E_PT_UNC_CIRCLE       =  1,   /*!< Shape Ellipsoid Point with Uncertainty Circle. */
    E_gnsCP_SHAPE_E_PT_UNC_ELLIP        =  3,   /*!< Shape Ellipsoid Point with Uncertainty Eclipse. */
    E_gnsCP_SHAPE_E_PT_ALT              =  8,   /*!< Shape Ellipsoid Point with Altitude. */
    E_gnsCP_SHAPE_E_PT_ALT_UNC_ELLIP    =  9,   /*!< Shape Ellipsoid Point with Altitude and Uncertainty Eclipse. */
    E_gnsCP_SHAPE_E_ARC                 = 10    /*!< Shape Ellipsoid Arc. */
} e_gnsCP_Shape;

/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsCP_FixType
 *  \brief  Control Plane A-GPS Position Fix Type (2D or 3D).  Used in  #s_gnsCP_GpsPosition::v_FixType
 */
typedef enum{
    E_gnsCP_FIX_TYPE_2D     = 0,        /*!< 2-D Fix Type. */
    E_gnsCP_FIX_TYPE_3D     = 1         /*!< 3-D Fix Type. */
} e_gnsCP_FixType;

/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsCP_LatitudeSign
 *  \brief  Control Plane A-GPS Position, sign of the Latitude (North or South).   Used in  s_gnsCP_GpsPosition::v_SignOfLat.
 */
typedef enum{
    E_gnsCP_LAT_SIGN_NORTH  = 0,        /*!< Latitude sign is North. */
    E_gnsCP_LAT_SIGN_SOUTH  = 1         /*!< Latitude sign is South. */
} e_gnsCP_LatitudeSign;

/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsCP_AltitudeDirection
 *  \brief  Control Plane A-GPS Position, direction of the Altitude (Height or Depth).   Used in  #s_gnsCP_GpsPosition::v_DirectOfAlt.
 */
typedef enum{
    E_gnsCP_ALT_DIRECT_HEIGHT   = 0,    /*!< Altitude above the Ellipsoid surface. */
    E_gnsCP_ALT_DIRECT_DEPTH    = 1,    /*!< Altitude below the Ellipsoid surface. */
    E_gnsCP_ALT_DIRECT_UNKNOWN          /*!< Direction of Altitude unknown. */
} e_gnsCP_AltitudeDirection;

/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsCP_FinishStatus
 *  \brief  Return status from CP session completion.
 */
typedef enum {
    E_gnsCP_FINISH_SUCCESS,                             /*!< Session completed successfully. */
    E_gnsCP_FINISH_FAILURE_NOT_ENOUGH_SATELLITES,       /*!< GPS was unable to resolve it's position. */
    E_gnsCP_FINISH_FAILURE_METHOD_NOT_SUPPORTED,        /*!< Requested position method is not supported. */
    E_gnsCP_FINISH_FAILURE_NOT_PROCESSED,               /*!< Unable to process request because of insufficient resources. */
    E_gnsCP_FINISH_FAILURE_UNDEFINED                    /*!< Failure not listed above. */
} e_gnsCP_FinishStatus;

/*!
 *  \enum   e_gnsCP_UserResponse
 *  \brief  User reponse for the Location Notification.
 */

typedef enum{
    E_gnsCP_USER_REJECT,
    E_gnsCP_USER_ACCEPT,
    E_gnsCP_USER_NO_RSP
} e_gnsCP_UserResponse;
/*!
 *  \enum   e_gnsCP_VerificationType
 *  \brief  Type of Notifiction Request.
 */

typedef enum{
    E_gnsCP_TYPE_NOTIFY_ONLY,
    E_gnsCP_TYPE_NOTIFY_VERIFY
} e_gnsCP_VerificationType;


/*!
 *  \enum   e_gnsCP_NotificationType
 *  \brief  Verification allowed or denied if no answer.
 */

typedef enum{
    E_gnsCP_VERIFY_ALLOWED_NA = 1,
    E_gnsCP_VERIFY_DENIED_NA
} e_gnsCP_NotificationType;



    /*!
 *  \enum   e_gnsCP_MolrErrorReason
 *  \brief  Reason for terminating MOLR session.
 */

typedef enum{
    E_gnsCP_MOLR_ERR_NONE = 0,                     /*!< No Error, Normal tearing down the session. */
    E_gnsCP_MOLR_ERR                                /*!< Error, Session is terminated abnormally.  */
} e_gnsCP_MolrErrorReason;


/*!
 *  \enum   e_gnsCP_MolrType
 *  \brief  Type of MOLR session.
 */

typedef enum{
    E_gnsCP_MOLR_TYPE_MOAR = 0,                       /*!< Mobile Originated Location Request for Assistance Data Request*/
    E_gnsCP_MOLR_TYPE_MOLR,                           /*!< Mobile Originated Location Request for Basic Self Location.  */
    E_gnsCP_MOLR_TYPE_MOLF                            /*!< Mobile Originated Location Request for forwarding to 3rd party.  */
} e_gnsCP_MolrType;
/*! \brief Molr option types. */
/*Type of MOLR Options*/

typedef enum{
    E_gnsCP_MOLR_OPT_ASSISTREQ   = 0x01,
    E_gnsCP_MOLR_OPT_QUALITYREQ  =0x02,
    E_gnsCP_MOLR_OPT_SERVICETYPE =0x04 ,
    E_gnsCP_MOLR_OPT_THIRDCLIENT =0x08
} e_gnsCP_MolrOptionType;



/*! \anchor Bitmask_for_t_gnsCP_AidRequest
 *  \name   Bitmask_for_t_gnsCP_AidRequest
 *  \brief  Used in #s_gnsCP_GpsAidRequest::v_AidMask */
/* Bit Masks for aid_mask field in T_GPS_AID_REQUEST */
/*@{*/

#define K_gnsCP_AID_REQ_ALMANAC        0x0001  /*!< Bit 0: Almanac assistance data requested. */
#define K_gnsCP_AID_REQ_UTC_MODEL      0x0002  /*!< Bit 1: UTC Model assistance data requested. */
#define K_gnsCP_AID_REQ_IONO_MODEL     0x0004  /*!< Bit 2: Ionospheric model assistance data requested.  */
#define K_gnsCP_AID_REQ_NAV_MODEL      0x0008  /*!< Bit 3: Navigation Model assistance data requested. */
#define K_gnsCP_AID_REQ_DGPS_CORR      0x0010  /*!< Bit 4: DGPS Corrections assistance data requested. */
#define K_gnsCP_AID_REQ_REF_LOC        0x0020  /*!< Bit 5: Reference Location assistance data requested. */
#define K_gnsCP_AID_REQ_REF_TIME       0x0040  /*!< Bit 6: Reference Time assistance data requested. */
#define K_gnsCP_AID_REQ_ACQ_ASSIST     0x0080  /*!< Bit 7: Acquisition Assistance assistance data request  */
#define K_gnsCP_AID_REQ_RTI            0x0100  /*!< Bit 8: Real-Time Integrity assistance data requested.  */

/*@}*/

#define K_gnsCP_MAX_SAT_NR          16      /*!< Maximum number of Satellites in the GPS Assistance data arrays. */

#define K_gnsCP_SV_HEALTH_LENGTH    46      /*!< Size of the Satellite Global Health assistance data array.
                                                 This corresponds to 364 bits. */



#define K_gnsCP_MAX_PHONE_NUM_LENGTH    42
#define K_gnsCP_MAX_CLIENT_NAME_LENGTH   50

#define K_gnsCP_MAX_REQUESTOR_ID_LENGTH   65
#define K_gnsCP_MAX_CODE_WORD_LENGTH      65

#define K_gnsCP_DCS_7_BIT_DATA_CODED   0
                    /* the text string is transmitted in GSM_APPI
                    (8 bits characters, NULL-terminated)
                    over the interface, but was received coded in
                    GSM_REC septets, or is to be sent coded
                    in GSM_REC septets */

#define K_gnsCP_DCS_8_BIT_DATA         1
                    /* the payload of the message is in DATA_APPI
                    format. The first two bytes indicate the
                    number of following data bytes. Those data
                    bytes have been received or will be stored as
                    they are, with no modification. */

#define K_gnsCP_DCS_UCS2               2
                    /* the string is transmitted in UCS2_APPI.
                    The first byte indicates the UCS2 type
                    (0x80, 0x81 or 0x82), the second byte
                    indicates the number of UCS2 characters
                    in the string, and the remaining bytes
                    contain the header (for 0x81 and 0x82)
                    and the UCS2 characters */

#define K_gnsCP_UCS2_COMPRESSED        3
                    /* the string is transmitted in compressed UCS2
                    (for future use, not managed) */

#define K_gnsCP_DCS_8_BIT_TEXT         4
                    /* the text string is transmitted in GSM_APPI
                    (8 bits characters, NULL-terminated) but was
                    stored in GSM_REC format or is to be stored
                    in GSM_REC format (not packed : one byte per
                    character) */

#define K_gnsCP_DCS_GSM_DEFAULT     5

/*@}group Location_gnsCP_Constants*/


/*! \addtogroup Location_gnsCP_Structures */
/*@{*/

/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_GpsAidRequest
 *  \brief  Structure to indicate the required AGPS assistance data.
 *  \note   Macros used: <br>\ref Bitmask_for_t_gnsCP_AidRequest.
 *                       <br>\ref K_gnsCP_MAX_SAT_NR.
 */

typedef struct {
    uint16_t v_AidMask;          /*!< Bitmask for requested aiding data \ref Bitmask_for_t_gnsCP_AidRequest.
    \verbatim
        Bit 0:      Almanac assistance data requested.
        Bit 1:      UTC Model assistance data requested.
        Bit 2:      Ionospheric model assistance data requested.
        Bit 3:      Navigation Model assistance data requested.
        Bit 4:      DGPS Corrections assistance data requested.
        Bit 5:      Reference Location assistance data requested.
        Bit 6:      Reference Time assistance data requested.
        Bit 7:      Acquisition Assistance assistance data requested.
        Bit 8:      Real-Time Integrity assistance data requested.
        Bits 9-15:  Reserved.
    \endverbatim */
    uint16_t v_GpsWeek;          /*!< Ephemeris target GPS Week Number.  INTEGER (0..1022).
                                    <br>Valid if Bit 3 of the v_AidMask is set.
                                    <br>Value as per the RRC and RRLP specifications. */
    uint8_t  v_GpsToe;           /*!< Target GPS Time of Ephemeris (hours.  INTEGER (0..167).
                                    <br>Valid if Bit 3 of the v_AidMask is set.
                                    <br>Value as per the RRC and RRLP specifications. */
    uint8_t  v_TTOELimit;        /*!< Age Limit on the Ephemeris Toe (hours).  INTEGER (0..10).
                                    <br>Valid if Bit 3 of the v_AidMask is set.
                                    <br>OPTIONAL (255 = 0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8).
                                    <br>Value as per the RRC and RRLP specifications. */
    uint8_t  v_NrOfSats;         /*!< Number of Satellite Information records in #a_SatID and #a_Iode.
                                     INTEGER (0..K_gnsCP_MAX_SAT_NR).*/
    uint8_t  a_SatID[ K_gnsCP_MAX_SAT_NR ]; /*!< Satellite Id currently in use.  INTEGER (0..61).
                                                <br>Values as per the RRC and RRLP specifications. */
    uint8_t  a_Iode[ K_gnsCP_MAX_SAT_NR ];  /*!< Issue of Data Ephemeris for satellite ID specified
                                                in the corresponding index of a_SatID.  INTEGER (0..255).
                                                <br>Values as per the RRC and RRLP specifications. */
} s_gnsCP_GpsAidRequest;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_PosInstruct
 *  \brief  Expected Quality of Position, measurement response time.
 *  \note   Macros used: \ref Bitmask_for_s_gnsCP_PosInstruct.
 */
typedef struct {
    e_gnsCP_PosInstructType     v_OptField; /*!< Specifies which optional elements have been set.*/

    uint32_t    v_Timeout;                  /*!< Time-out (milliseconds) within which the report must be provided to network.
                                                <br>INTEGER(1..7200000).
                                                <br>Field required for:
                                                    - #E_gnsCP_SINGLE_SHOT
                                                    . */
    uint8_t      v_HorAccuracy;              /*!< Horizontal Accuracy is optional in MS-Assisted Mode of Operation.
                                                <br>INTEGER (0..127).
                                                <br>Value as per the GAD format used in the RRC and RRLP specifications.
                                                <br>OPTIONAL (0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8 ).
                                                <br>Field optional for:
                                                    - #E_gnsCP_SINGLE_SHOT
                                                    - #E_gnsCP_PERIODIC
                                                    - #E_gnsCP_EVENT_7A
                                                    - #E_gnsCP_EVENT_7B
                                                    - #E_gnsCP_EVENT_7C
                                                    . */
    uint8_t      v_VertAccuracy;             /*!< Vertical Accuracy is optional in MS-Assisted Mode of Operation.
                                                <br>INTEGER (0..127).
                                                <br>Value as per the GAD format used in the RRC and RRLP specifications.
                                                <br>OPTIONAL (0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8 ).
                                                <br>Field optional for:
                                                    - #E_gnsCP_SINGLE_SHOT
                                                    - #E_gnsCP_PERIODIC
                                                    - #E_gnsCP_EVENT_7A
                                                    - #E_gnsCP_EVENT_7B
                                                    - #E_gnsCP_EVENT_7C
                                                    . */
    uint16_t      v_Amount;                   /*!< Number of Reports expected for a session.
                                                <br> RRC values:
                                                    - Periodic: INTEGER(1, 2, 4, 8, 16, 32, 64, 0=Infinity).
                                                    - 7a:       INTEGER(1, 2, 4, 8, 16, 32, 64, 0=Infinity).
                                                    - 7b:       INTEGER(1, 2, 4, 8, 16, 32, 64, 0=Infinity).
                                                    - 7c:       INTEGER(1, 2, 4, 8, 16, 32, 64, 0=Infinity).
                                                    - Value set to infinity if modem is handling the reporting amount.
                                                    . */
    uint32_t     v_PeriodicInterval;         /*!< Periodic Interval (milliseconds).
                                                <br>RRC Values if v_PosReportType is:
                                                    - #E_gnsCP_PERIODIC : INTEGER(250, 500, 1000, 2000, 3000, 4000, 6000, 8000, 12000, 16000, 20000, 24000, 28000, 32000, 64000)
                                                    - #E_gnsCP_EVENT_7A : INTEGER(5000,15000,60000,300000,900000,1800000,3600000,7200000).
                                                    - #E_gnsCP_EVENT_7B : INTEGER(5000,15000,60000,300000,900000,1800000,3600000,7200000).
                                                    - #E_gnsCP_EVENT_7A : INTEGER(5000,15000,60000,300000,900000,1800000,3600000,7200000).
                                                \note This interval will be converted to the nearest multiple of GPS sub-system supported periodic rate. */
    uint32_t     v_EventThreshold;           /*!< RRC Event Threshold if the requested Reporting Event is 7A, 7B or 7C.
                                                <br>Value as per the RRC specifications.
                                                    - 7a:Threshold Position Change: INTEGER(10,20,30,40,50,100,200,300,500,1000,2000,5000,10000,20000,50000,100000) metres.
                                                    - 7b:Threshold SFN-SFN change:  Not supported.
                                                    - 7c:Threshold SFN-GPS TOW:     Not supported.
                                                    .
                                                <br>Field required for:
                                                    - #E_gnsCP_EVENT_7A
                                                    . */
    e_gnsCP_PosReportType           v_PosReportType;    /*!< Specifies the type of position reporting required by the network.
                                                             For Event 7A and event 7B, the modem has to handle the event occurrence
                                                             by taking into consideration the SFN measurements.
                                                             For Event 7B and 7C, GNS will send measurements at regular interval
                                                             specified by v_PeriodicInterval.
                                                             */
    e_gnsCP_Positioning_Method_Type v_PosMethod;        /*!< a-GPS MS-Based or MS-Assisted mode. */

    uint8_t     v_AssistanceReqGrant;    /*!< If information not available as in case of RRLP, would be set to Allowed. Allowed =0, Not Allowed = 1 */

} s_gnsCP_PosInstruct;

/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_TowAssist
 *  \brief  A-GPS Control Plane GPS Time of Week Assistance data.  Consists of TLM message, Anti-spoof flag, Alert flag, and 2 reserved bits in TLM Word.
 *  \brief  This enables GPS time recovery and possibly extended GPS correlation intervals.
 */
typedef struct {
    uint8_t  v_Satid;        /*!< Satellite Id.      INTEGER (0.63).
                                0-31 indicating GPS satellites 1-32. 32-63 indicating augmentation systems. */
    uint8_t  v_AntiSpoof;    /*!< Anti Spoof Flag.   INTEGER (0..1).
                                Value as per the RRC and RRLP specifications. */
    uint8_t  v_Alert;        /*!< Alert Flag.        INTEGER (0..1).
                                Value as per the RRC and RRLP specifications. */
    uint8_t  v_TlmRes;       /*!< TLM Reserved Bits. INTEGER (0..3).
                                Value as per the RRC and RRLP specifications. */
    uint16_t v_TlmWord;      /*!< TLM World.         INTEGER (0..16383).
                                Value as per the RRC and RRLP specifications. */
} s_gnsCP_TowAssist;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_NavModelElm
 *  \brief  A-GPS Control Plane Navigation Model (Ephemeris) Elements Assistance for a single satellite.
 */
typedef struct {
    uint8_t  v_SatId;            /*!< Satellite Id.  INTEGER (0..63).
                                    <br>0-31 indicating GPS satellites 1-32, 32-63 indicating augmentation systems.
                                    <br>Value as per the RRC and RRLP specifications. */
    e_gnsCP_NavModelSatStatus v_SatStatus;       /*!< Satellite Status. */
    uint8_t  v_EphemCodeOnL2;    /*!< CA-Code or P-Code on L2.  INTEGER(0..3).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint8_t  v_EphemURA;         /*!< URA (User Range Accuracy) Index.  INTEGER (0..15).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint8_t  v_EphemSvHealth;    /*!< Satellite Health.   INTEGER (0..63).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint8_t  v_Epheml2pFlag;     /*!< L2 P-Code flag.  INTEGER (0..1).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint16_t v_EphemIodc;        /*!< IODC (Issue of Data Clock).  INTEGER  (0..1023).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.
                                    <br>This is the same as IODE (Issue of Data Ephemeris). */
    uint16_t v_EphemToc;         /*!< Reference Time of Clock.  INTEGER (0..37799).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.
                                    <br>This must be the same as v_EphemToe.*/
    uint32_t v_Reserved1;        /*!< Sub-Frame 1, 1st reserved word.  INTEGER (0..8388607).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint32_t v_Reserved2;        /*!< Sub-Frame 1, 2nd reserved word.  INTEGER (0..16777215).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint32_t v_Reserved3;        /*!< Sub-Frame 1, 3rd reserved word.  INTEGER (0..16777215).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint32_t v_Reserved4;        /*!< Sub-Frame 1, 4th reserved word.  INTEGER (0..65535).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int8_t  v_EphemTgd;         /*!< Total Group Delay.  INTEGER (-128..127).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int8_t  v_EphemAf2;         /*!< Satellite Clock Drift Rate.  INTEGER (-128..127).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int16_t v_EphemAf1;         /*!< Satellite Clock Drift.  INTEGER (-32768..32767).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int32_t v_EphemAf0;         /*!< Satellite Clock Bias.  INTEGER (-2097152..2097151).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int16_t v_EphemCrs;         /*!< Amplitude of the Sine Harmonic Correction Term to the Orbit Radius.  INTEGER (-32768..32767).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int16_t v_EphemDeltaN;      /*!< Mean Motion Difference From Computed Value.  INTEGER (-32768..32767).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int32_t v_EphemMo;          /*!< Mean Anomaly at Reference Time.  INTEGER (-2147483648..2147483647).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int16_t v_EphemCuc;         /*!< Amplitude of the Cosine Harmonic Correction Term To The Argument Of Latitude.  INTEGER (-32768..32767).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int16_t v_EphemCus;         /*!< Amplitude of the Sine Harmonic Correction Term To The Argument Of Latitude.  INTEGER (-32768..32767).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint32_t v_Epheme;           /*!< Eccentricity.  INTEGER (0..4294967295).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint32_t v_EphemAPowerhalf;  /*!< Square root of the Semi-Major Axis.  INTEGER (0..4294967295).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint16_t v_EphemToe;         /*!< Reference Time of Ephemeris.  INTEGER (0..37799).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.
                                    <br>This must be the same as v_EphemToc. */
    uint8_t  v_EphemFitFlag;     /*!< Ephemeris Fit Interval.  INTEGER (0..1).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint8_t  v_EphemAODO;        /*!< Age Of Data Offset.  INTEGER (0..31).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int16_t v_EphemCic;         /*!< Amplitude of the Cosine Harmonic Correction Term To The Angle Of Inclination. INTEGER (-32768..32767).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int16_t v_EphemCis;         /*!< Amplitude of the Sine Harmonic Correction Term To The Angle Of Inclination.  INTEGER (-32768..32767).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.*/
    int32_t v_EphemOmegaA0;     /*!< Longitude of Ascending Node of Orbit Plane at Weekly Epoch.  INTEGER (-2147483648..2147483647).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int32_t v_EphemIo;          /*!< Inclination Angle at Reference Time.  INTEGER (-2147483648..2147483647).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int16_t v_EphemIdot;        /*!< Rate of Inclination Angle.  INTEGER (-8192..8191).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int16_t v_EphemCrc;         /*!< Amplitude of the Cosine Harmonic Correction Term to the Orbit Radius.  INTEGER (-32768..32767).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int32_t v_EphemW;           /*!< Argument of Perigee.  INTEGER (-2147483648..2147483647).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int32_t v_EphemOmegaAdot;   /*!< Longitude of Ascending Node of Orbit Plane at Weekly Epoch.  INTEGER (-8388608..8388607).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
} s_gnsCP_NavModelElm;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_AcquisElement
 *  \brief  A-GPS Control Plane Acquisition Assistance Element for a single satellite.
 */
typedef struct {
    uint8_t  v_SatId;            /*!< Satellite Id.  INTEGER (0..63).
                                    <br>0-31 indicating GPS satellites 1-32, 32-63 indicating augmentation systems.
                                    <br>Value as per the RRC and RRLP specifications. */
    uint8_t  v_Doppler1;         /*!< Satellite Doppler 1st order term (1/42 Hz/s).  INTEGER (0..63).   Represents -0.978 .. 0.500 Hz.
                                    <br>OPTIONAL (0 = Not Present/Unknown ).
                                    <br>Value as per the RRLP specifications.
                                    <br>Note that the value received from RRC is in the range -42..21 and needs to be offset by +42. */
    int16_t v_Doppler0;         /*!< Satellite Doppler 0th order term (2.5 Hz). INTEGER (-2048..2047).  Represents -5120.0 .. 5117.5 Hz.
                                    <br>Value as per the RRC and RRLP specifications. */
    uint8_t  v_DopplerUncert;    /*!< Satellite Doppler Uncertainty.  INTEGER (0..4).  Represents +/- { <=200, <=100, <=50, <=25, <=12.5 } Hz.
                                   OPTIONAL (0 = Not Present/Unknown ).
                                    <br>Value as per the RRLP specifications. */
    uint8_t  v_IntCodePhase;     /*!< Satellite Integer Code Phase since the last GPS bit edge boundary (milliseconds).  INTEGER (0..19).
                                    <br>Value as per the RRC and RRLP specifications. */
    uint16_t v_CodePhase;        /*!< Satellite Code Phase (CA-Chips). INTEGER (0..1022).
                                    <br>Value as per the RRC and RRLP specifications. */
    uint8_t  v_GpsBitNumber;     /*!< Satellite GPS Bit Number (modulo 4) relative to #s_gnsCP_AcquisAssist::v_GpsTow.  INTEGER (0..3).
                                    <br>Value as per the RRC and RRLP specifications. */
    uint8_t  v_SearchWindow;     /*!< Satellite Code Phase Search Window.  Integer (0..15).  Represents
                                    <br>+/- { 512, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 196 } C/A-Chips.
                                    <br>Value as per the RRC and RRLP specifications. */
    uint8_t  v_Azimuth;          /*!< Satellite Azimuth (11.25 degrees).  INTEGER (0..31).  Maps to 0 .. 348.75 degrees.
                                   OPTIONAL (255 = 0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8).
                                    <br>Value as per the RRC and RRLP specifications. */
    uint8_t  v_Elevation;        /*!< Satellite Elevation (11.25 degrees).  INTEGER (0..7).  Maps to 0 .. 78.75 degrees.
                                   OPTIONAL (255 = 0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8).
                                    <br>Value as per the RRC and RRLP specifications. */
} s_gnsCP_AcquisElement;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_AcquisAssist
 *  \brief  A-GPS Control Plane Acquisition Assistance Data.
 *  \note   Macros used: \ref K_gnsCP_MAX_SAT_NR.
 */
typedef struct {
    uint32_t v_GpsTow;           /*!< GPS Time of Week (milliseconds).  INTEGER (0..604799999).
                                    <br>Value as per the RRC specification.
                                    <br>Note that the value received from RRLP needs to be multiplied by 80ms to convert to this range.
                                    <br>OPTIONAL (0xFFFFFFFF = Not Present/Unknown #K_gnsUNKNOWN_U32). */
    uint32_t v_GpsTowUnc;        /*!< Uncertainty in the provided GPS Time of Week (microseconds).  
                                                         <br>OPTIONAL (0xFFFFFFFF = Not Present/Unknown #K_gnsUNKNOWN_U32). */
    int32_t v_RefDeltaCellTime;  /*!< The Delta time (microseconds) between a cellular modem
                                    synchronization pulse which is sent to the GPS
                                    baseband chip and the Reference Cellular Time
                                    (UTRAN/GSM) provided in the Reference Time Assistance message.
                                    <br>OPTIONAL (0x7FFFFFFF = Not Present/Unknown #K_gnsUNKNOWN_S32). */
    uint32_t  v_RefCellTimeUnc;   /*!< Uncertainty in the Reference Time determined via the synchronisation pulse (microseconds).
                                    <br>This includes the reference cell time uncertainty and any further uncertainties added in generating the pulse.
                                    <br>OPTIONAL (4294967295 = 0xFFFFFFFF = Not Present/Unknown #K_gnsUNKNOWN_U32). */
    uint8_t  v_NrOfSats;          /*!< Number of valid Satellites records present in a_AcqElement. */
    s_gnsCP_AcquisElement a_AcqElement[ K_gnsCP_MAX_SAT_NR ];   /*!< Acquisition Assistance Elements for each satellite */
} s_gnsCP_AcquisAssist;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_AlmanacElm
 *  \brief  A-GPS Control Plane Almanac Element Assistance Data for a single satellite.
 */
typedef struct {
    uint8_t  v_SatId;            /*!< Satellite Id.  INTEGER (0..63).
                                    <br>0-31 indicating GPS satellites 1-32, 32-63 indicating augmentation systems.
                                    <br>Value as per the RRC and RRLP specifications.*/
    uint8_t  v_DataId;           /**<Data Id (Only Present in RRC)
                                                255 - Unknown \ref Type_of_UNKNOWN*/
    uint8_t  v_AlmanacWNA;       /*!< Almanac Reference Week Number.  INTEGER (0..255).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.*/
    uint16_t v_AlmanacE;         /*!< Eccentricity.  INTEGER (0..65535).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.*/
    uint8_t  v_AlmanacToa;       /*!< Reference Time of Almanac.  INTEGER (0..255).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.*/
    uint8_t  v_AlmanacSVhealth;  /*!< Satellite Health.  INTEGER (0..255).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.*/
    int16_t v_AlmanacDeltai;    /*!< Ksii - Delta Inclination Angle.  INTEGER (-32768..32767).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.*/
    uint32_t v_AlmanacAsqrt;     /*!< Square-root of the Semi-Major Axis.  INTEGER (0..16777215).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.*/
    int32_t v_AlmanacOmega0;    /*!< Longitude of Ascending Node of Orbit Plane at Weekly Epoch.  INTEGER (-8388608..8388607).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.*/
    int32_t v_AlmanacOmega;     /*!< Argument of Perigee.  INTEGER (-8388608..8388607).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.*/
    int32_t v_AlmanacM0;        /*!< Mean Anomaly at Reference Time.  INTEGER (-8388608..8388607).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.*/
    int16_t v_AlmanacOmegaDot;  /*!< Longitude of Ascending Node of Orbit Plane at Weekly Epoch Range.  INTEGER (-32768..32767).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.*/
    int16_t v_AlmanacAF0;       /*!< Satellite Clock Bias.  INTEGER (-1024..1023).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.*/
    int16_t v_AlmanacAF1;       /*!< Satellite Clock Drift.  INTEGER (-1024..1023).
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications.*/
} s_gnsCP_AlmanacElm;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_GlobalHealth
 *  \brief  A-GPS Control Plane Global Health Assistance data.
 *          This is specified in page 25 of the GPS Navigation Message subframes 4 and 5.
 *          This data is available from RRC, but not from RRLP.
 *  \note   Macros used: \ref K_gnsCP_SV_HEALTH_LENGTH.
 */
typedef struct {
    uint8_t  a_SvGlobalHealth[ K_gnsCP_SV_HEALTH_LENGTH ];   /*!< First 45.5 Bytes shall store 364 bits.
                                                                Value as per the RRC specification. */
} s_gnsCP_GlobalHealth;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCPGsmCellTime
 *  \brief  tructure to define GSM cell time..
 */
typedef struct {

     
     uint32_t   v_FrameNumber;      /*!<  frame_number    Value range: 0 to 2715647 */
     uint16_t   v_QBitNumber;        /*!<   Bit number (in Quarterbits) Valid Range [0 to 625]. Represent the quadra bit value. Set to 0xFFFF if Unknown*/
     uint16_t   v_BCCHCarrier;      /*!<   bcch_carrier     Valid Rane: [0 to 1023] , Set to 0xFFFF if Unknown*/
     uint8_t    v_TimeSlot;         /*!<   time_slot   Valid Range [0 to 7],  Set to 0xFF if Unknown*/
     uint8_t    v_TimingAdvance;    /*!<   Valid Range [0 to 63] , Set to 0xFF if Unknown*/
     uint8_t    v_BSIC;             /*!<   bsic  Value range: 0 to 63, Set to 0xFF if Unknown*/
                                                                        
 } s_gnsCPGsmCellTime;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCPWcdmaFddCellTime
 *  \brief  tructure to define Wcdma Fdd cell time..
 */
typedef struct {

    uint32_t   v_CellFrameTimeLS; /*!<  Least Significant bits of CellFrametime.
                                                          Valid Range: [0...uint32_t max*/
    uint16_t   v_CellFrameTimeMS; /*!< Most significant bits of CellFrametime 
                                                           Valid Range: [0...1023]*/
    uint16_t   v_Sfn;       /*!<  Serving Frame number of the Basestation when SFN was measured before Pulse Reference time is injected.
                                                \verbatim
                                                     Valid Range: 
                                                     0...4095: Valid SFN
                                                     0xFFFF  : Unknown SFN 
                                                \endverbatim */
    uint16_t   v_Psc;        /*!<  PSC used for SFN timing 
                                                   valid range [0...511] according to  TS25 213
                                                   0xFFFF : Unknown Primary Scrambling code*/
    uint16_t   v_RoundTripTime; /*!<  Round trip time Set to 0xFFFF if Unknown*/

} s_gnsCPWcdmaFddCellTime;


/*!
 *  \union u_gnsCP_CellTime
*  \brief  This union defines the Cell Time in Cellular frame format.
 */
typedef union
    {
  s_gnsCPGsmCellTime v_GsmCellTime;  /*!< Structure to define teh Gsm cell time This field will be udpated only if /ref e_gns_RATType 
                                                                 indicate #E_gns_RAT_TYPE_GSM*/
  s_gnsCPWcdmaFddCellTime v_WcdmaFddCellTime;/*!< Structure to define the Wcdma Fdd cell time This field will be udpated only if /ref e_gns_RATType 
                                                                 indicate #E_gns_RAT_TYPE_WCDMA_FDD*/
    }u_gnsCP_CellTime; 



/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_RefTime
 *  \brief  A-GPS Control Plane Reference Time Assistance data.
 *  \note   Macros used: \ref K_gnsCP_MAX_SAT_NR.
 */
typedef struct {
    uint32_t v_GpsTow;               /*!< GPS Time of Week (milliseconds).  INTEGER (0..604799999).
                                        <br>Value as per the RRC specification.
                                        <br>Note that the value received from RRLP needs to be multiplied by 80 to convert to this range.
                                        <br>OPTIONAL (0xFFFFFFFF = Not Present/Unknown #K_gnsUNKNOWN_U32). */
    uint32_t v_GpsTowUnc;            /*!< Uncertainty in the provided GPS Time of Week (microseconds). 
                                                              <br>OPTIONAL (0xFFFFFFFF = Not Present/Unknown #K_gnsUNKNOWN_U32). */
    int32_t v_RefDeltaCellTime;     /*!< The Delta time (microseconds) between a cellular modem
                                        synchronization pulse which is sent to the GPS
                                        baseband chip and the Reference Cellular Time
                                        (UTRAN/GSM) provided in the Reference Time Assistance message.
                                        <br>OPTIONAL (0x7FFFFFFF = Not Present/Unknown #K_gnsUNKNOWN_S32). */
    uint16_t v_GpsWeek;              /*!< GPS Week Number.  INTEGER (0..1022). */
    uint32_t  v_RefCellTimeUnc;       /*!< Uncertainty in the Reference Time determined via the synchronisation pulse (microseconds).
                                        <br>This includes the reference cell time uncertainty and any further uncertainties added in generating the pulse.
                                      OPTIONAL (4294967295 = 0xFFFFFFFF = Not Present/Unknown #K_gnsUNKNOWN_U32). */
    int8_t  v_UtranGpsDriftRate;    /*!< (UTRAN - GPS) time frame drift rate.  This is only available for UTRAN from RRC.
                                      OPTIONAL (127 = 0x7F = Not Present/Unknown #K_gnsUNKNOWN_S8). */
    uint8_t  v_NumTowAssist;         /*!< Number of TOW Assist records in a_TowAssist. */
    s_gnsCP_TowAssist  a_TowAssist[ K_gnsCP_MAX_SAT_NR ];   /*!< GPS Time of Week Assistance elements for each satellite. */
    e_gns_RATType v_CellTimeType;
    
    u_gnsCP_CellTime  v_CellTime;
} s_gnsCP_RefTime;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_IonoModel
 *  \brief  A-GPS Control Plane Ionospheric Model Assistance Information Elements.
 */
typedef struct {
    int8_t  v_Alpha0;           /*!< Ionospheric Model parameter alpha0.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int8_t  v_Alpha1;           /*!< Ionospheric Model parameter alpha1.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int8_t  v_Alpha2;           /*!< Ionospheric Model parameter alpha2.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int8_t  v_Alpha3;           /*!< Ionospheric Model parameter alpha3.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int8_t  v_Beta0;            /*!< Ionospheric Model parameter beta0.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int8_t  v_Beta1;            /*!< Ionospheric Model parameter beta1.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int8_t  v_Beta2;            /*!< Ionospheric Model parameter beta2.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int8_t  v_Beta3;            /*!< Ionospheric Model parameter beta3.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
} s_gnsCP_IonoModel;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_UtcModel
 *  \brief  A-GPS Control Plane UTC Model Assistance Information Elements.
 */
typedef struct {
    int32_t v_UtcA1;            /*!< UTC Model parameter A1.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int32_t v_UtcA0;            /*!< UTC Model parameter A0.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint8_t  v_UtcTot;           /*!< UTC model - reference time of week.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint8_t  v_UtcWnT;           /*!< UTC model - reference week number.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int8_t  v_UtcDeltaTLs;      /*!< UTC model - time difference due to leap seconds before event.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    uint8_t  v_UtcWnLsf;         /*!< UTC model - week number when next leap second event occurs
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int8_t  v_UtcDn;            /*!< UTC model - day of week when next leap second event occurs [range 0..6].
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
    int8_t  v_UtcDeltaTLsf;     /*!< UTC model - time difference due to leap seconds after event.
                                    <br>Value as per the RRC, RRLP and GPS-ICD-200 specifications. */
} s_gnsCP_UtcModel;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_RTIntegrity
 *  \brief  A-GPS Control Plane Real Time Integrity Assistance data.
 *  \note   Macros used: \ref K_gnsCP_MAX_SAT_NR.
 */
typedef struct {
    uint8_t  v_NrOfSats;                         /*!< Number of valid Satellites Id's records in a_BadSatID. */
    uint8_t  a_BadSatId[ K_gnsCP_MAX_SAT_NR ];   /*!< List of Bad Satellite Id's. \ref K_gnsCP_MAX_SAT_NR. */
} s_gnsCP_RTIntegrity;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_SatDgpsData
 *  \brief  A-GPS Control Plane Differential GPS Assistance elements for a single satellite.
 */
typedef struct {
    uint8_t  v_SatId;            /*!< Satellite Id.  INTEGER (0..63).
                                    <br>0-31 indicating GPS satellites 1-32, 32-63 indicating augmentation systems.
                                    <br>Value as per the RRC and RRLP specifications. */
    uint8_t  v_Iode;             /*!< IODE - Issue of Data Ephemeris the DGPS Corrections were computed with.  INTEGER 0..255).
                                    <br>Value as per the RRC, RRLP and RTCM-SC-104 specifications. */
    uint8_t  v_Udre;             /*!< User Differential Range Error.  INTEGER (0..4).
                                    <br>Value as per the RRC, RRLP and RTCM-SC-104 specifications. */
    int8_t  v_Rrc;              /*!< Pseudorange Rate Correction (0.032 m/s).  INTEGER (-128..127).
                                    <br>Represents -4.064 .. 4.064 m/s.
                                    <br>Value as per the RRC, RRLP and RTCM-SC-104 specifications. */
    int16_t v_Prc;              /*!< Pseudorange Correction (0.32 m).  INTEGER (-2047..2047).
                                    <br>Represents -655.04 .. 655.04 m.
                                    <br>Value as per the RRC, RRLP and RTCM-SC-104 specifications. */
} s_gnsCP_SatDgpsData;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_DGpsCorr
 *  \brief  A-GPS Control Plane Differential GPS Correction Assistance data.
 *  \note   Macros used: \ref K_gnsCP_MAX_SAT_NR.
 */
typedef struct {
    uint32_t v_GpsTow;           /*!< DGPS Corrections Reference Time Of Week (seconds).  INTEGER (0..604799).
                                    <br>Value as per the RRC, RRLP and RTCM-SC-104 specifications.
                                    <br>OPTIONAL (0xFFFFFFFF = Not Present/Unknown #K_gnsUNKNOWN_U32). */
    uint8_t  v_Health;           /*!< DGPS Reference Station Health status.
                                    <br>Value as per the RRC, RRLP and RTCM-SC-104 specifications. */
    uint8_t  v_NrOfSats;         /*!< Number of Satellites DGPS Corrections provided in a_DgpsList. */
    s_gnsCP_SatDgpsData a_DgpsList[ K_gnsCP_MAX_SAT_NR ];   /*!< DGPS Corrections list. */
} s_gnsCP_DGpsCorr;

/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_Abort
 *  \brief  A-GPS Control Plane Session Abort requested by network
 */
typedef struct
{
    uint8_t    v_SessionId;    /*!< Session ID used when PosInstruct was issued */
} s_gnsCP_Abort;

/*======================= Parameter description SIGNALS FROM GPS ========================*/


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_MsrElement
 *  \brief  A-GPS Control Plane Raw Measurement result element for a single satellite.
 */
typedef struct {
    uint8_t  v_SatId;            /*!< Satellite Id.  INTEGER (0..63).
                                    <br>0-31 indicating GPS satellites 1-32, 32-63 indicating augmentation systems.
                                    <br>Value as per the RRC and RRLP specifications. */
    uint8_t  v_CNo;             /*!< Satellite Signal to Noise Ratio (dBHz).  INTEGER (0..63).
                                    <br>Value as per the RRC and RRLP specifications. */
    int16_t v_Doppler;          /*!< Satellite Doppler (0.2 Hz).  INTEGER (-32768..32767).
                                    <br>Represents -6553.6 .. 6553.4 Hz.
                                    <br>Value as per the RRC and RRLP specifications. */
    uint16_t v_WholeChips;       /*!< Satellite Code Phase Whole Chips (C/A Chips).  INTEGER (0..1022).
                                    <br>Value as per the RRC and RRLP specifications. */
    uint16_t v_FracChips;        /*!< Satellite Code Phase Fractional Chips (2^-10 chips).  INTEGER (0..1023).
                                    <br>Value as per the RRC and RRLP specifications. */
    uint8_t  v_MpathIndic;       /*!< Pseudorange Multipath Error Indicator.  INTEGER (0..3).
                                    <br>Represents { Not measured, Low (<5m), Medium (<43m), High (>43m) }.
                                    <br>Value as per the RRC and RRLP specifications. */
    uint8_t  v_PseudoRmsErr;     /*!< Pseudorange RMS Error.  INTEGER (0..63).
                                    <br>Consists of a 3 bit Mantissa 'x' & 3 bit Exponent 'y',
                                    where RMS Error =  0.5 * (1 + x/8) * 2^y  metres.
                                    <br>Value as per the RRC and RRLP specifications. */
} s_gnsCP_MsrElement;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_GpsMeasurements
 *  \brief  Raw GPS measurements
 *  \note   Macros used: \ref K_gnsCP_MAX_SAT_NR.
 */
typedef struct {
    uint32_t                v_GpsTow;               /*!< Measurement GPS Time of Week (milliseconds).  INTEGER (0..604799999).
                                                    <br>Value as per the RRC specification.
                                                    <br>OPTIONAL (0xFFFFFFFF = Not Present/Unknown #K_gnsUNKNOWN_U32). */
    int32_t                 v_RefDeltaCellTime;     /*!< The Delta time (microseconds) between the last seen cellular modem
                                                        synchronization pulse which is sent to the GPS baseband chip
                                                        and the Measurement GPS Time of Week provided in this message.
                                                        <br>OPTIONAL (0x7FFFFFFF = Not Present/Unknown #K_gnsUNKNOWN_S32), ie no pulse received. */
    uint8_t                 v_GpsTowUnc;            /*!<GPS Reference Time Uncertainty in microseconds. 
                                                                                           Note that this field needs to be considered only when GPS-GSM time relationship is unknown
                                                                                          0xFF - Unknown ref Type_of_UNKNOWN
                                                                                         <br> Value as per RRLP specification (0-127) */
    uint8_t                 v_DeltaTow;           /*!< The difference in milliseconds between the GPS TOW reported in the GPS Measurement Information
                                                                                             and the millisecond part of the first SV in the list reported.
                                                                                                   <br> Valid range 0-127. Value interpretation:ms
                                                       <br>OPTIONAL (0xFF = Not Present/Unknown #K_gnsUNKNOWN_U8). */

    e_gnsCP_Status          v_Status;               /*!< GPS Measurements Validity Status. */
    uint8_t                 v_NrOfSats;             /*!< Number of Satellites Raw GPS Measurements are provided for below. */
    s_gnsCP_MsrElement a_MsrElement[ K_gnsCP_MAX_SAT_NR ];  /*!< GPS Raw Measurement results for each satellite. */
} s_gnsCP_GpsMeasurements;


/*----------------------------------------------------------------------*/
/*!
 *  \struct s_gnsCP_GpsPosition
 *  \brief  A-GPS Control Plane Position estimate in GAD (Geographical Area Description) format.
 */
typedef struct {
    e_gnsCP_Status                   v_Status;           /*!< Position Validity Status. */
    e_gnsCP_FixType                  v_FixType;          /*!< Fix Type (2D/3D).
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    e_gnsCP_Shape                    v_ShapeType;        /*!< Shape of the Position.
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications.
        \verbatim
            E_gnsCP_SHAPE_E_PT                  =  0    "Shape Type: Ellipsoid Point."
            E_gnsCP_SHAPE_E_PT_UNC_CIRCLE       =  1    "Shape Type: Ellipsoid Point with Uncertainty Circle."
            E_gnsCP_SHAPE_E_PT_UNC_ELLIP        =  3    "Shape Type: Ellipsoid Point with Uncertainty Eclipse."
            E_gnsCP_SHAPE_E_PT_ALT              =  8    "Shape Type: Ellipsoid Point with Altitude."
            E_gnsCP_SHAPE_E_PT_ALT_UNC_ELLIP    =  9    "Shape Type: Ellipsoid Point with Altitude and Uncertainty Eclipse."
            E_gnsCP_SHAPE_E_ARC                 = 10    "Shape Type: Ellipsoid Arc."
        \endverbatim
        */
    e_gnsCP_LatitudeSign             v_SignOfLat;        /*!< Sign of the Latitude (North/South).
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    uint32_t                         v_GpsTow;           /*!< Measurement GPS Time of Week (milliseconds).  INTEGER (0..604799999).
                                                            <br>Value as per the RRC specification.
                                                            <br>OPTIONAL (0xFFFFFFFF = Not Present/Unknown #K_gnsUNKNOWN_U32). */
    uint32_t                         v_GpsTowUnc;        /*!<GPS Reference Time Uncertainty in microseconds
                                                            <br>0xFFFFFFFF - Unknown \ref Type_of_UNKNOWN #K_gnsUNKNOWN_U32
                                                            <br>Note that the value destined for RRLP needs to be modulo 144000. */
    int32_t                          v_RefDeltaCellTime; /*!< The Delta time (microseconds) between the last seen cellular modem
                                                            synchronization pulse which is sent to the GPS baseband chip
                                                            and the Measurement GPS Time of Week provided in this message.
                                                            <br>OPTIONAL (0x7FFFFFFF = Not Present/Unknown #K_gnsUNKNOWN_S32),  ie no pulse received.
                                                            \note v_RefDeltaCellTime is not used in reference position but used in position returned. */
    uint32_t                         v_Latitude;         /*!< Latitude (90/2^23 degrees).  INTEGER (0..8388607).  Represents 0..90 degrees.
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    int32_t                          v_Longitude;        /*!< Longitude (180/2^23 degrees).  INTEGER (-8388608..8388607).   Represents -180..+180 degrees.
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    uint16_t                         v_Altitude;         /*!< Altitude (metres).  INTEGER (0..32767).
                                                            <br>Only valid for Shape Types:
                                                                - #E_gnsCP_SHAPE_E_PT_ALT,
                                                                - #E_gnsCP_SHAPE_E_PT_ALT_UNC_ELLIP.
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    uint8_t                          v_UncertSemiMajor;  /*!< Uncertainty Semi-Major axis.  INTEGER (0..127).
                                                            <br>Only valid for Shape Types:
                                                                - #E_gnsCP_SHAPE_E_PT_UNC_CIRCLE,
                                                                - #E_gnsCP_SHAPE_E_PT_UNC_ELLIP,
                                                                - #E_gnsCP_SHAPE_E_PT_ALT_UNC_ELLIP,
                                                                - #E_gnsCP_SHAPE_E_ARC.
                                                            <br>Shape #E_gnsCP_SHAPE_E_PT_UNC_CIRCLE: Major Axis shall store Uncertainty Code.
                                                            <br>Shape #E_gnsCP_SHAPE_E_ARC: Major Axis shall be interpreted as Uncertainty Radius.
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    uint8_t                          v_UncertSemiMinor;  /*!< Uncertainty Semi-Minor axis.  INTEGER (0..127).
                                                            <br>Only valid for Shape Types:
                                                                - #E_gnsCP_SHAPE_E_PT_UNC_ELLIP,
                                                                - #E_gnsCP_SHAPE_E_PT_ALT_UNC_ELLIP.
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    uint8_t                          v_AxisBearing;      /*!< Bearing of the Uncertainty Semi-Major axis.  INTEGER (0..179).
                                                            <br>Only valid for Shape Types:
                                                                - #E_gnsCP_SHAPE_E_PT_UNC_ELLIP,
                                                                - #E_gnsCP_SHAPE_E_PT_ALT_UNC_ELLIP.
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    e_gnsCP_AltitudeDirection        v_DirectOfAlt;      /*!< Direction of the Altitude (Height/Depth).
                                                            <br>Only valid values:
                                                                - #E_gnsCP_ALT_DIRECT_HEIGHT,
                                                                - #E_gnsCP_ALT_DIRECT_DEPTH.
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    uint8_t                          v_UncertAltitude;   /*!< Altitude Uncertainty.   INTEGER (0..127).
                                                            <br>Only valid for Shape Type:
                                                                - #E_gnsCP_SHAPE_E_PT_ALT_UNC_ELLIP.
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    uint8_t                          v_Confidence;       /*!< Position Confidence (%).  INTEGER (1..100).
                                                            <br>Only valid for Shape Types:
                                                                - #E_gnsCP_SHAPE_E_PT_UNC_CIRCLE,
                                                                - #E_gnsCP_SHAPE_E_PT_UNC_ELLIP,
                                                                - #E_gnsCP_SHAPE_E_PT_ALT_UNC_ELLIP,
                                                                - #E_gnsCP_SHAPE_E_ARC.
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    uint8_t                          v_IncludedAngle;    /*!< Ellipsoid Arc Included Angle.  INTEGER (0..255).
                                                            <br>Only valid for Shape Type:
                                                                - #E_gnsCP_SHAPE_E_ARC.
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    uint8_t                          v_OffsetAngle;      /*!< Ellipsoid Arc Offset Angle.  INTEGER (0..255).
                                                            <br>Only valid for Shape Type:
                                                                - #E_gnsCP_SHAPE_E_ARC.
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    uint16_t                         v_InnerRadius;      /*!< Ellipsoid Arc Inner Radius.  INTEGER (0..65535).
                                                            <br>Only valid for Shape Type:
                                                                - #E_gnsCP_SHAPE_E_ARC.
                                                            <br>Value as per the GAD format used in the RRC and RRLP specifications. */
    uint16_t                         v_GpsTowSubms;      /*!< Measurement GPS Time of Week Sub-millisecond part. .
                                                 Valid range 0-9999
                                                 Value interpretation:Tow in ms=GPS TOW+0.0001*GPSTOW
                                                 Value not present:0xFFFF*/
} s_gnsCP_GpsPosition;

/*!
 *  \struct s_gnsCP_GpsPosnResult
 *  \brief  AGPS CP Position Solution to network
 */
typedef struct
{
    uint8_t                            v_SessionId;    /*!< Session ID used when PosInstruct was issued */
    uint8_t                          v_FinalReport;  /*!< 1 - Indicates final report for a session, 0 - indicates more reports  to follow */
    s_gnsCP_GpsPosition            v_Position;        /*!< A-GPS Control Plane Position estimate in GAD (Geographical Area Description) format. */
    e_gns_RATType           v_CellTimeType;  /*!< Type of RAT or Cell information.Refer e_gns_RATType */
    u_gnsCP_CellTime        v_CellTime;      /*!<A-GPS Control Plane Cell time information  */
} s_gnsCP_GpsPosnResult;

/*!
 *  \struct s_gnsCP_GpsMeasResult
 *  \brief  AGPS CP Measurement Solution to network
 */
typedef struct
{
    uint8_t                            v_SessionId;    /*!< Session ID used when PosInstruct was issued */
    uint8_t                          v_FinalReport;  /*!< 1 - Indicates final report for a session, 0 - indicates more reports to follow */
    s_gnsCP_GpsMeasurements        v_Measurements;    /*!< Raw GPS measurements */
    e_gns_RATType                 v_CellTimeType;  /*!< Type of RAT or Cell information.Refer e_gns_RATType */
    u_gnsCP_CellTime              v_CellTime;      /*!<A-GPS Control Plane Cell time information  */
} s_gnsCP_GpsMeasResult;

/*!
 *  \struct s_gnsCP_GpsAbortCnf
 *  \brief  A-GPS Control Plane Session Abort requested by network succesfully handled
 */
typedef struct
{
    uint8_t    v_SessionId;    /*!< Session ID used when PosInstruct was issued */
} s_gnsCP_GpsAbortCnf;


/*!
    \struct s_gnsCP_3rdPartyClient
    \brief 3rd party client information to which position need to be forwarded
*/

typedef struct {
    uint8_t             v_ClientIdTonNpi;         /*!<  Client ID Type of Number and number Plan identification. Refer 3GPP TS 24.008, section 10.5.4.7. */
    uint8_t             v_ClientIdConfig;         /*!< Indicates if v_TonNpi has been populated.  */
    uint8_t             v_ClientIdPhoneNumber[K_gnsCP_MAX_PHONE_NUM_LENGTH];  /*!< Client ID phone number */
    uint8_t             v_ClientIdPhoneNumberLen;  /*!<  Indicates the length of v_ClientIdPhoneNumber. If this variable is set to 0, indicates self location requested*/
    uint8_t             v_MlcNumTonNpi;              /*!<  MLC Number Type of Number and number Plan identification. Refer 3GPP TS 24.008, section 10.5.4.7. */
    uint8_t             v_MlcNumConfig;              /*!<  Indicates if v_TonNpi has been populated.  */
    uint8_t             v_MlcNumPhoneNumber[K_gnsCP_MAX_PHONE_NUM_LENGTH];    /**<MLC Phone Number */
    uint8_t             v_MlcNumPhoneNumberLen;    /*!<  Indicates the length of v_MlcNumPhoneNumber. If this variable is set to 0, indicates self location requested */

}s_gnsCP_3rdPartyClient;

/*!
    \struct s_gnsCP_QoP
    \brief Quality of Position Required by Application
*/

typedef struct {
   uint8_t          v_Horacc    ;   /*!< Horizontal accuracy       INTEGER (0..127).*/
   int8_t           v_Veracc    ;   /*!< Vertical accuracy         INTEGER (0..127)   OPTIONAL (-1 = Not present).*/
   int32_t          v_Delay     ;   /*!< Maximum age of location[seconds]   INTEGER (0..65535) OPTIONAL (-1 = Not present).*/
   e_gns_DelayType  v_MaxLocAge ;   /*!< Type of Delay. lowdelay = 0, delaytolerant = 1.*/
}s_gnsCP_QoP;


/*!
 *  \struct s_gnsCP_GpsNotifyRsp
 *  \brief Notification response
 */
typedef struct
{
    uint32_t                v_SessionId;    /*!< Session ID used when Notification was issued */
    e_gnsCP_UserResponse    v_UserResponse;    /*!<  */
} s_gnsCP_GpsNotifyRsp;



/*!
 *  \struct s_gnsCP_GpsMolrStartInd
 *  \brief  AGPS MOLR Location Request Indication to network
 */
typedef struct
{
    uint8_t                    v_SessionId;         /*!< Session ID used when PosInstruct was issued */
    uint8_t                    v_Option;            /*!<OptionService Type
                                                                                                                Bit 0 - Assist Request
                                                                                                                Bit 1 - Quality of Position Request
                                                                                                                Bit 2 - Serviec Type
                                                                                                                Bit 3 - 3rd Party*/
    e_gnsCP_MolrType           v_MolrType;          /*!< MOLR Type 0-MOAR, 1-MOLR, 2-MOLF  \ref e_gnsCP_MolrType */
    s_gnsCP_GpsAidRequest      v_GpsAidRequest;     /**< Valid for MOAR */
    s_gnsCP_QoP                v_QopRequest;        /**< Valid for MOLR & MOLF*/
    e_gnsCP_LcsServiceType     v_ServiceType;       /*!<OPTIONAL:Service Type \ref e_gnsCP_ServiceType */
    s_gnsCP_3rdPartyClient     v_3rdPartyRequest;   /**< Valid for MOLF*/
    uint16_t                    v_SupportedGADShapesMask;  /*!< Bitmask indicates supported GAD shapes */

 } s_gnsCP_GpsMolrStartInd;


 /*!
  *  \struct s_gnsCP_GpsMolrStopInd
  *  \brief  AGPS MOLR Location Request Indication to network
  */
 typedef struct
 {
     uint8_t                     v_SessionId;    /*!< Session ID used when PosInstruct was issued */
     e_gnsCP_MolrErrorReason     v_ErrorReason;  /*!< Error Reason  \ref e_gnsCP_MolrErrorReason */
     e_gnsCP_MolrType            v_MolrType;     /*!< MOLR Type 0-MOAR, 1-MOLR, 2-MOLF  \ref e_gnsCP_MolrType */
 } s_gnsCP_GpsMolrStopInd;

  /*!
   *  \struct s_gnsCP_GpsPosCapabilityInd
  *  \brief  Update Positioning Capability to network
  */
 typedef struct
 {
     uint8_t                    v_Dummy;    /*!< Dummy data */
 } s_gnsCP_GpsPosCapabilityInd;



/*! \union  u_gnsCP_MsgData
 *  \brief  Union which defines the data associated with \ref e_gnsCP_MsgType
 */
typedef union
{
    s_gnsCP_GpsAidRequest          v_GnsCpGpsAidRequest;     /*!< Corresponds to message type #s_gnsCP_GpsAidRequest. */
    s_gnsCP_GpsPosnResult         v_GnsCPGpsPosnResult;    /*!< Corresponds to message type #s_gnsCP_GpsPosnResult. */
    s_gnsCP_GpsMeasResult         v_GnsCPGpsMeasResult;    /*!< Corresponds to message type #s_gnsCP_GpsMeasResult. */
    s_gnsCP_GpsAbortCnf            v_GnsCPGpsAbortCnf;        /*!< Corresponds to message type #s_gnsCP_GpsAbortCnf */
    s_gnsCP_GpsNotifyRsp        v_GnsCpGpsNotifyRsp;    /*!< Corresponds to message type #s_gnsCP_GpsNotifyRsp*/
    s_gnsCP_GpsMolrStartInd      v_GnsCpGpsMolrStartInd;     /*!< Corresponds to message type #s_gnsCP_GpsMolrStartInd. */
    s_gnsCP_GpsMolrStopInd         v_GnsCPGpsMolrStopInd;    /*!< Corresponds to message type #s_gnsCP_GpsMolrStopInd. */
    s_gnsCP_GpsPosCapabilityInd v_GnsCPGpsPosCapabilityInd; /*!< Corresponds to message type #s_gnsCP_GpsPosCapabilityInd. */
} u_gnsCP_MsgData ;
/*@}*/

/*!
 *  \struct s_gnsCP_LocNotification
 *  \brief Location notification identify and type
 */

typedef struct {
    uint8_t                      v_MtlrTypeVerify;   /* e_gnsCP_VerificationType */
    uint8_t                      v_NotificationType; /* e_gnsCP_NotificationType*/
    e_gnsCP_LocNotificationType  v_LocationType;     /* e_gnsCP_LocNotificationType  
                                                                                                     00-CURRENT
                                                                                                     01-CURRENT_OR_LAST_KNOWN
                                                                                                     02-INITIAL
                                                                                                     10-ACTIVATE_DEFERRED_LOC 
                                                                                                     11-CANCEL_DEFERRED_LOC    */
    uint8_t                 v_LcsServiceTypeId;
    uint8_t                 v_TonNpi;
    uint8_t                 v_TonNpiConfig;
    int8_t                 a_PhoneNumber[K_gnsCP_MAX_PHONE_NUM_LENGTH];
    int8_t                 a_ClientName[K_gnsCP_MAX_CLIENT_NAME_LENGTH];
    uint8_t                 v_ClientNameAlphabet;
    uint8_t                 v_ClientNameLen;
    int8_t                 a_RequestorId[K_gnsCP_MAX_REQUESTOR_ID_LENGTH];
    uint8_t                 v_RequestorIdAlphabet;
    uint8_t                 v_RequestorIdLen;
    int8_t                 a_CodeWord[K_gnsCP_MAX_CODE_WORD_LENGTH];
    uint8_t                 v_CodeWordAlphabet;
    uint8_t                 v_CodeWordLen;
} s_gnsCP_LocNotification;



/*@} group Location_gnsCP_Structures */


#endif /* GNS_CP_TYPEDEFS_H */
