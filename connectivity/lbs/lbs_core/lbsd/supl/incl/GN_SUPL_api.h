/*
 *  Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson SA.
 */

#ifndef GN_SUPL_API_H
#define GN_SUPL_API_H
#ifdef __cplusplus
   extern "C" {
#endif

//****************************************************************************
// File level documentation
/// \file
/// \brief
///      LBS SUPL External Interface API Header
//
//-----------------------------------------------------------------------------
/// \ingroup    LBS SUPL
//
/// \defgroup   GN_SUPL_api_I  LBS SUPL - Internally Implemented SUPL API.
//
/// \brief
///      LBS SUPL - SUPL API definitions for functions implemented internally.
//
/// \details
///      LBS SUPL - GPS SUPL API definitions for the enumerated
///      data types structures and functions provided in the library and can be
///      called by the host software (ie inward called).
/// \addtogroup GN_SUPL_api_I
//
//-----------------------------------------------------------------------------
/// \ingroup      LBS SUPL
//
/// \defgroup     GN_SUPL_api_H  LBS SUPL - Host Implemented SUPL API.
//
/// \brief
///      LBS SUPL - SUPL API definitions for functions implemented by the host.
//
/// \details
///      LBS SUPL - SUPL API definitions for the enumerated data
///      types structures and functions called by the library and must be
///      implemented by the host software to suit the target platform OS and
///      hardware configuration (ie outward called).
///      All of these functions must be implemented in order to link a final
///      solution, even if only with a stub "{ return( 0 ); }" so say that the
///      particular action requested by the library is not supported.
/// \note
///      The functions in this group must be implemented in the
///      supporting code and linked with the library for the GPS to function
///      correctly.
/// \addtogroup GN_SUPL_api_H
//
//*****************************************************************************

#include "gps_ptypes.h"
#include "GN_Status.h"
#include "GN_GPS_api.h"
#include "GN_AGPS_api.h"

//*****************************************************************************
/// \addtogroup GN_SUPL_api_I
/// \{


//*****************************************************************************
/// \brief
///      Type of IP address passed.
/// \details
///      Enum to indicate the type of IP address passed.
typedef enum TcpIp_AddressType
{
   IP_None,                         ///< Connect to predefined IP address.
   IPV4_Address,                    ///< IP address is of type IPV4.
   IPV6_Address,                    ///< IP address is of type IPV6.
   FQDN_Address                     ///< IP address is of type FQDN.
} e_TcpIp_AddressType;              // Type of IP address passed.


//*****************************************************************************
/// \brief
///      GN SUPL Core Library Configuration Data.
/// \details
///      GN SUPL Core Library Configuration Data for items that can be changed
///      at run-time. A NULL pointer indicates an optional parameter is not present.
///      IP addresses can be in one of the following notations:
///      <ul>
///         <li>IPv4 address in Dot-decimal notation.</li>
///         <li>IPv6 address in IPv6 notation.</li>
///      </ul>
/// \note
///      At least one of the Optional SET types (msisdn, mdn, min, imsi, nai or
///      IP address) must be set. If more than one of the SET types is present
///      it is undefind which will be used.
typedef struct // GN_SUPL_Config
{
   e_TcpIp_AddressType SLP_IP_Type; ///< Type of IP address in SLP_IP_Address.
   CH *SLP_IP_Address;              ///< SLP IP Address.
   U2 SLP_IP_Port;                  ///< SLP IP Port (default should be 7275).
   U2 User_Timer_1_Duration;        ///< SUPL UT1 value in seconds (default should be 10).
   U2 User_Timer_2_Duration;        ///< SUPL UT2 value in seconds (default should be 10).
   U2 User_Timer_3_Duration;        ///< SUPL UT3 value in seconds (default should be 10).
   CH *p_SET_msisdn;                ///< Optional MSISDN.
   CH *p_SET_mdn;                   ///< Optional MDN (34 bit coded according to TIA-553).
   CH *p_SET_min;                   ///< Optional MIN.
   CH *p_SET_imsi;                  ///< Optional IMSI.
   CH *p_SET_nai;                   ///< Optional NAI.
   e_TcpIp_AddressType SET_IP_Type; ///< Type of IP address in SET_IP_Address.
   CH *SET_IP_Address;              ///< Optional SET IP Address.
   BL PT_agpsSETAssisted;           ///< Positioning Technology AGPS SET Assisted is supported if TRUE.
   BL PT_agpsSETBased;              ///< Positioning Technology AGPS SET Based is supported if TRUE.
   BL PT_autonomousGPS;             ///< Positioning Technology Autonomous GPS is supported if TRUE.
   BL PT_eCID;                      ///< Positioning Technology Enhanced Cell ID is supported if TRUE.
   CH *p_PlatfromVersion;            ///< Platfrom Version to be used by SUPL library for the logging
} s_GN_SUPL_Config;                 // GN SUPL Core Library Configuration Data


//*****************************************************************************
/// \brief
///      Type of Third party ID.
/// \details
///      Enum to indicate the type of third party id passed.

typedef enum{
    thirdparty_id_nothing=0,            /**< Third Party Information is not present */
    thirdparty_id_logicalName,          /**< Describes the third party through its logical name */
    thirdparty_id_msisdn,               /**< Describes the third party through its Mobile Station International Subscriber Directory Number (MSISDN) */
    thirdparty_id_emailaddr,            /**< Describes the third party through its Email address */ 
    thirdparty_id_sip_uri,              /**< Describes the third party through its Session Initiation Protocol (SIP) URI */
    thirdparty_id_ims_public_identity,  /**< Describes the third party through its IP Multimedia Subsystem (IMS) public ID */
    thirdparty_id_min,                  /**< Describes the third party through its Mobile Identification Number (MIN) */
    thirdparty_id_mdn,                  /**< Describes the third party through its Mobile Directory Number (MDN)*/
    thirdparty_id_uri,                  /**< Describes the third party through its Uniform Resource Identifier (URI) */
    thirdparty_id_max,

}e_thirdparty_id;



//*****************************************************************************
/// \brief
///      GN SUPL Core Library Extended Configuration Data.
/// \details
///      GN SUPL Core Library Extended Configuration Data for items that can be changed
///      at run-time. 
/// \note
///      If this API is not called, the default SUPL version supported is SUPLv1.0

typedef struct
{
   U1 Major_Version;                   ///< Specifies the Major version to emulate. Range [1..2]. If 1, will act as a SUPLv1.0 client. 
                                       ///< In case of 2, will act as a v2.0 client. SetConfig will return with failure if the SUPL library 
                                       ///< internally cannot support the requested major version
   e_TcpIp_AddressType ESLP_IP_Type;    ///< Type of IP address in SLP_IP_Address.
   CH *ESLP_IP_Address;                 ///< SLP IP Address.
   U2 ESLP_IP_Port;                     ///< SLP IP Port (default should be 7275).
                                    
   /* General configuration for reporting data to the SUPL server */
   BL Reporting_Mode_Real_Time;        ///< Reporting Mode Real Time is supported if TRUE
   BL Reporting_Mode_Quasi_Real_Time;  ///< Reporting Mode Quasi Real Time is supported if TRUE
   BL Reporting_Mode_Batch;            ///< Reporting Mode Batch is supported if TRUE
   U2 Min_Report_Interval;             ///< Minimum interval is seconds between reports. Range [1..50000]
   U2 Max_Report_Interval;             ///< Minimum interval is seconds between reports. Range [1..50000] -1 not defined

   /* Batch & Quasi Real Time Reporting Configuration */
   BL Batch_Report_Position;           ///< Position Reporting in a Batch is supported if TRUE
   BL Batch_Report_Measurements;       ///< Measurement Reporting in a Batch is supported if TRUE
   U1 Max_Positions_In_Batch;          ///< Maximum number of Positions reported in a batch. Range [1..100] -1 not defined
   U1 Max_Measurement_In_Batch;        ///< Maximum number of Measurements reported in a batch. Range [1..100] -1 not defined
                                    
   /* Periodic Trigger related configuration */
   BL Periodic_Trigger;                ///< Periodic triggers are supported if TRUE
   

   /* Area Event Trigger related configuration */
   BL Area_Event_Trigger;              ///< Area Event triggers are supported if TRUE
   BL Geographic_Area_Ellipse;         ///< Ellipse Shaped area for Area Event triggers are supported if TRUE
   BL Geographic_Area_Polygon;         ///< Polygon Shaped area for Area Event triggers are supported if TRUE
   U1 Max_Geographic_Areas;            ///< Maximum number of Geographic Areas for Area Event Triggers. Range [1..10] -1 not defined
   U1 Max_Area_ID_Lists;               ///< Maximum number of AreaID Lists for Area Event Triggers. Range [1..10] -1 not defined
   U1 Max_Area_ID_Per_List;            ///< Maximum number of AreaID Per AreaIDLists for Area Event Triggers. Range [1..10] -1 not defined

   /* User Timer configuration */
   U2 User_Timer_5_Duration;        ///< SUPL UT5 value in seconds (default should be 10).
   U2 User_Timer_6_Duration;        ///< SUPL UT6 value in seconds (default should be 10).
   U2 User_Timer_7_Duration;        ///< SUPL UT7 value in seconds (default should be 10).
   U2 User_Timer_8_Duration;        ///< SUPL UT8 value in seconds (default should be 10).
   U2 User_Timer_9_Duration;        ///< SUPL UT9 value in seconds (default should be 60).
   U2 User_Timer_10_Duration;        ///< SUPL UT10 value in seconds (default should be 60).

   /* Number of sessions reported as supported to the SUPL server */
   U1 Max_Num_Session;                 ///< Maximum SUPL sessions allowed. range [1..5]
   CH *p_ESlp_WhiteList[10];       ///<List of ESLP Address which is provisioned max support is 10>
   
} s_GN_SUPL_Extd_Config;


//*****************************************************************************
/// \brief
///      GN SUPL Core Position Request QoP Data.
/// \details
///      GN SUPL Core Library Quality of Position data for a Position Request.
/// \note
///     See 3GPP TS 23.032 for a full description of the GAD (Geographical Area
///     Description) Horizontal and Vertical Accuracy fields.
typedef struct // GN_SUPL_QoP
{
   U1 horacc;           ///< Horizontal accuracy       INTEGER (0..127).
   I1 veracc;           ///< Vertical accuracy         INTEGER (0..127)   OPTIONAL (-1 = Not present).
   I4 maxLocAge;        ///< Maximum age of location   INTEGER (0..65535) OPTIONAL (-1 = Not present).
   I1 delay;            ///< Maximum permissible delay INTEGER (0..7)     OPTIONAL (-1 = Not present).
} s_GN_SUPL_QoP;        // GN SUPL Core Library QoP Data


//*****************************************************************************
/// \brief
///      Status of Cell information.
/// \details
///      Enum to indicate the status of the cell information.
typedef enum
{
   CIS_NONE,         ///< Cell information is not yet available.
   CIS_stale,        ///< Cell information is stale.
   CIS_current,      ///< Cell information is current.
   CIS_unknown       ///< Cell information is unknown.
} e_CellInfoStatus;  // Status of cell information provided.


//*****************************************************************************
/// \brief
///      Type of Cell information.
/// \details
///      Enum to indicate the type of the cell information.
typedef enum CellInfoType
{
   CIT_gsmCell,   ///< Cell information is from a GSM network.
   CIT_wcdmaCell, ///< Cell information is from a WCDMA network.
   CIT_cdmaCell,   ///< Cell information is from a CDMA network.   
   CIT_AccessPoint_WLAN,         ///< Cell information is not yet available.
   CIT_AccessPoint_WIMAX,        ///< Cell information is stale.
   CIT_AccessPoint_UNKNOWN
} e_CellInfoType; // Type of cell information provided.

//*****************************************************************************
/// \brief
///      Type of Area Id information.
/// \details
///      Enum to indicate the type of the area id information.
typedef enum 
{
   GN_AreaId_gSM,   ///< Area Id is from a GSM network.
   GN_AreaId_wCDMA, ///< Area Id is from a WCDMA network.
   GN_AreaId_cDMA   ///< Area Id is from a CDMA network.
} e_GN_SUPL_V2_AreaIdInfoType; // Type of Area Id information provided.


//*****************************************************************************
/// \brief
///      Network Measurement Report from a GSM network.
/// \details
///      Single entry of the Network Measurement Report from a GSM network.
typedef struct
{
   U2 aRFCN;            ///< INTEGER(0..1023),
   U1 bSIC;             ///< INTEGER(0..63),
   U1 rxLev;            ///< INTEGER(0..63),
} s_NMRElement;         // Network Measurement Report information.


//*****************************************************************************
/// \brief
///      Cell information from a GSM network.
/// \details
///      Cell information and Network Measurement Reports from a GSM network.
typedef struct
{
   U2 refMCC;           ///< INTEGER(0..999), -- Mobile Country Code
   U2 refMNC;           ///< INTEGER(0..999), -- Mobile Network Code
   U2 refLAC;           ///< INTEGER(0..65535), -- Location area code
   U2 refCI;            ///< INTEGER(0..65535), -- Cell identity
   I2 tA;               ///< INTEGER(0..255) OPTIONAL, --Timing Advance. (-1 = Not present).
   U1 NMRCount;         ///< Number of NMR entries in NMRList.
   s_NMRElement*
      p_NMRList;        ///<  OPTIONAL (Present if NMRCount > 0) Pointer to an array of NMRElements.
} s_gsmCellInfo;

//*****************************************************************************
/// \brief
///      Area Id information from a GSM network.
/// \details
///      Area Id information and Network Measurement Reports from a GSM network.
typedef struct
{
   U2 refMCC;           ///< INTEGER(0..999), -- Mobile Country Code
   U2 refMNC;           ///< INTEGER(0..999), -- Mobile Network Code
   U2 refLAC;           ///< INTEGER(0..65535), -- Location area code
   U2 refCI;            ///< INTEGER(0..65535), -- Cell identity
} s_GN_SUPL_V2_gSMAreaIdType;


//*****************************************************************************
/// \brief
///      Type of mode specific information for a WCDMA Network.
/// \details
///      Enumeration to choose between Frequency or Time Division Duplexed mode
///      for a WCDMA Network.
typedef enum {
   fdd,                 ///< Frequency Division Duplexed mode
   tdd                  ///< Time Division Duplexed mode
} e_modeSpecificInfo;


//*****************************************************************************
/// \brief
///      Frequency Division Duplexed information for a WCDMA network.
/// \details
///      Frequency Division Duplexed information for a WCDMA network.
typedef struct
{
   I2 uarfcn_UL;        ///< INTEGER(0..16383), OPTIONAL (-1 = Not present).
   U2 uarfcn_DL;        ///< INTEGER(0..16383)
} s_FrequencyInfoFDD;


//*****************************************************************************
/// \brief
///      Time Division Duplexed information from a WCDMA network.
/// \details
///      Time Division Duplexed information from a WCDMA network.
typedef struct
{
   U2 uarfcn_Nt;        ///< INTEGER(0..16383)
} s_FrequencyInfoTDD;


//*****************************************************************************
/// \brief
///      Frequency information from a WCDMA network.
/// \details
///      Choice of Frequency or Time Division Duplexed information from a
///      WCDMA network.
typedef struct
{
   e_modeSpecificInfo modeSpecificInfoType;  ///< Choice between FDD or TDD information.
   union {
      s_FrequencyInfoFDD fdd;    ///< Frequency Division Duplexed mode information
      s_FrequencyInfoTDD tdd;    ///< Time Division Duplexed mode information
   } of_type; ///< Union of either #s_FrequencyInfoFDD or #s_FrequencyInfoTDD frequency information.
} s_FrequencyInfo;


//*****************************************************************************
/// \brief
///      Cell measurements from the Frequency Division Duplexed domain in a WCDMA network.
/// \details
///      Cell measurements from the Frequency Division Duplexed domain in a WCDMA network.
typedef struct
{
   U2 PrimaryScramblingCode;     ///< INTEGER(0..511).
   I1 cpich_Ec_N0;               ///< INTEGER(0..63) OPTIONAL (-1 = Not Present). Values above 49 are spare.
   I1 cpich_RSCP;                ///< INTEGER(0..127) OPTIONAL (-1 = Not Present). Values above 91 are spare.
   U1 pathloss;                  ///< INTEGER(46..173) OPTIONAL (0 = Not Present). Values above 158 are spare.
} s_MeasuredResultFDD;


//*****************************************************************************
/// \brief
///      Cell measurements from the Time Division Duplexed domain from a WCDMA network.
/// \details
///      Cell measurements from the Time Division Duplexed domain from a WCDMA network.
typedef struct
{
   U1 cellParametersID;          ///< INTEGER(0..127).
   I1 proposedTGSN;              ///< INTEGER(0..14) OPTIONAL (-1 = Not Present).
   I1 primaryCCPCH_RSCP;         ///< INTEGER(0..127) OPTIONAL (-1 = Not Present).
   U1 pathloss;                  ///< INTEGER(46..173) OPTIONAL (0 = Not Present). Values above 158 are spare.
   U1 timeslotISCP_Count;        ///< Number of entries in p_TimeslotISCP_List.
   U1 *p_TimeslotISCP_List;      ///< OPTIONAL (Present if timeslotISCP_Count > 0) Pointer to an array of U1.
} s_MeasuredResultTDD;


//*****************************************************************************
/// \brief
///      Measured result from a network cell in a WCDMA network.
/// \details
///      Measured result from a network cell in a WCDMA network.
typedef struct
{
   I4 cellIdentity;              ///< INTEGER(0..268435455) OPTIONAL (-1 = Not present).
   e_modeSpecificInfo modeSpecificInfoType;  ///< Choice between FDD or TDD information.
   union {
     s_MeasuredResultFDD  fdd;   ///< Frequency Division Duplexed mode Measured Result
     s_MeasuredResultTDD  tdd;   ///< Time Division Duplexed mode Measured Result
   } of_type; ///< Union of either #s_MeasuredResultFDD or #s_MeasuredResultTDD result.
} s_CellMeasuredResult;


//*****************************************************************************
/// \brief
///      Measured Results from a WCDMA network.
/// \details
///      Set of measured results from a WCDMA network.
typedef struct
{
   BL FrequencyInfoPresent;      ///< TRUE if FrequencyInfo is present.
   s_FrequencyInfo
      FrequencyInfo;             ///< OPTIONAL (Present if FrequencyInfo == TRUE).
   I1 utra_CarrierRSSI;          ///< INTEGER(0..127) OPTIONAL, (-1 = Not present).
   U1 CellMeasuredResultCount;   ///< Count of MeasuredResults up to (maxCellMeas INTEGER ::= 32)
   s_CellMeasuredResult*
      p_CellMeasuredResultsList; ///< OPTIONAL (Present if CellMeasuredResultCount > 0) Pointer to an array of s_CellMeasuredResult.
} s_MeasuredResult;


/*----------------------------------------------------------------------*/
/*! \struct s_GN_SUPL_V2_WLANAPInfo
 *  \brief  Structure which defines the mobile information for WLAN Info.
 */
typedef struct
{
    U2  v_MS_Addr;                      /*!<Media Access Control address of WLAN access point (most significant 16 bits)*/
    U4  v_LS_Addr;                      /*!<Media Access Control address of WLAN access point (least significant 32 bits)*/
    I2  v_TransmitPower;                /*!<AP Transmit power in dBm. Optional parameter. INTEGER (-127...128). Invalid Value 0X7FFF*/
    I2  v_AntennaGain;                  /*!<AP antenna gain in dBi,Optional parameter.  INTEGER (-127...128). Invalid Value 0X7FFF*/        
    I2  v_SNR;                          /*!<AP Signal to Noise received at the SET in dB,Optional parameter.  INTEGER (-127...128). Invalid Value 0X7FFF*/
    U1  v_DeviceType;                   /*!<ENUM, 802.11a device or 802.11b device or 802.11g device or Unknown*/
    I2  v_SignalStrength;               /*!<AP signal strength received at the SET in dBm. INTEGER (-127...128).*/              
    U2  v_Channel;                      /*!<AP channel/frequency of Tx/Rx.*/               
    U4  v_RTDValue;                     /*!<Measured Round Trip Delay value between the SET and AP.*/
    U1  v_RTDUnits;                     /*!<Units for Round Trip Delay value and Round Trip Delay accuracy*/
    U2  v_Accuracy;                     /*!<Round Trip Delay standard deviation in relative units. INTEGER (0...255).*/
    I2  v_SETTransmitPower;             /*!<SET Transmit power in dBm.*/
    I2  v_SETAntennaGain;               /*!<SET antenna gain in dBi*/
    I2  v_SETSNR;                       /*!<SET Signal to Noise received at the AP in dB. */
    I2  v_SETSignalStrength;            /*!<SET signal strength received at the AP in dBm.*/
} s_GN_SUPL_V2_WLANAPInfo;

/*----------------------------------------------------------------------*/
/*! \union  s_GN_SUPL_V2_AccessPointInfo
 *  \brief  Union which defines the Access Point Information.
 */
typedef union
{
    s_GN_SUPL_V2_WLANAPInfo           WlanAccessPointInfo;              /*!< Access Point Information of the WLAN*/
    /*This structure can be extended for adding WIMAX and other wireless network Info*/
} s_GN_SUPL_V2_AccessPointInfo;

/*----------------------------------------------------------------------*/
/*! \struct s_GN_SUPL_V2_WirelessNetworkInfo
 *  \brief  Structure which defines mobile information.
 */
typedef struct
{
    s_GN_SUPL_V2_AccessPointInfo       AccessPointInfo;          /*!< AccessPoint Information of WLAN, WIMAX  */
} s_GN_SUPL_V2_WirelessNetworkInfo;

//*****************************************************************************
/// \brief
///      Cell information from a WCDMA network.
/// \details
///      Cell information from a WCDMA network.
typedef struct
{
   U2 refMCC;                    ///< INTEGER(0..999), -- Mobile Country Code
   U2 refMNC;                    ///< INTEGER(0..999), -- Mobile Network Code
   U4 refUC;                     ///< INTEGER(0..268435455), -- Cell identity
   s_FrequencyInfo*
      p_FrequencyInfo;           ///< FrequencyInfo OPTIONAL
   I2 primaryScramblingCode;     ///< INTEGER(0..511) OPTIONAL (-1 = not present).,
   U1 MeasuredResultCount;       ///< maxFreq INTEGER ::= 8
   s_MeasuredResult*
      p_MeasuredResultsList;     ///< OPTIONAL (Present if MeasuredResultCount > 0). Pointer to array of s_MeasuredResult.
} s_wcdmaCellInfo;

//*****************************************************************************
/// \brief
///      Area Id information from a WCDMA network.
/// \details
///      Area Id information from a WCDMA network.
typedef struct
{
   U2 refMCC;                    ///< INTEGER(0..999), -- Mobile Country Code
   U2 refMNC;                    ///< INTEGER(0..999), -- Mobile Network Code
   U4 refUC;                     ///< INTEGER(0..268435455), -- Cell identity
   U2 refLAC;                    ///< INTEGER(0..65535), -- Location area code
} s_GN_SUPL_V2_wCDMAAreaIdType;

//*****************************************************************************
/// \brief
///      Cell information from a CDMA network.
/// \details
///      Cell information from a CDMA network.
typedef struct
{
   U2 refNID;                    ///< INTEGER(0..65535), -- Network Id
   U2 refSID;                    ///< INTEGER(0..32767), -- System Id
   U2 refBASEID;                 ///< INTEGER(0..65535), -- Base Station Id
   U4 refBASELAT;                ///< INTEGER(0..4194303), -- Base Station Latitude
   U4 reBASELONG;                ///< INTEGER(0..8388607), -- Base Station Longitude
   U2 refREFPN;                  ///< INTEGER(0..511), -- Base Station PN Code
   U2 refWeekNumber;             ///< INTEGER(0..65535), -- GPS Week Number
   U4 refSeconds;                ///< INTEGER(0..4194303),-- GPS Seconds
} s_cdmaCellInfo;

//*****************************************************************************
/// \brief
///      Area Id information from a CDMA network.
/// \details
///      Area Id information from a CDMA network.
typedef struct
{
   U2 refNID;                    ///< INTEGER(0..65535), -- Network Id
   U2 refSID;                    ///< INTEGER(0..32767), -- System Id
   U2 refBASEID;                 ///< INTEGER(0..65535), -- Base Station Id
} s_GN_SUPL_V2_cDMAAreaIdType;

//*****************************************************************************
/// \brief
///      Location Id definition.
/// \details
///      Location Id from a network of one of the supported types.
typedef struct
{ //
   e_CellInfoStatus  Status;     ///< Indicates the validity of the Location Id.
   e_CellInfoType    Type;       ///< Indicates type to use for the union.
   union {
      s_gsmCellInfo     gsmCellInfo;    ///< Cell information is from a GSM network.
      s_wcdmaCellInfo   wcdmaCellInfo;  ///< Cell information is from a WCDMA network.
      s_cdmaCellInfo    cdmaCellInfo;   ///< Cell information is from a CDMA network.
      s_GN_SUPL_V2_WirelessNetworkInfo   wirelessNetworkInfo;     ///<Stores the AccessPoint Information of WLAN, WIMAX networks
   } of_type; ///< Union of either #s_gsmCellInfo, #s_wcdmaCellInfo or #s_cdmaCellInfo result.
} s_LocationId;

//*****************************************************************************
/// \brief
///      e_GN_SUPL_DebugEvents Definition
/// \details
///      Debug Events reported by SUPL library.

typedef enum
{
    SUPL_PosRequest,  ///< A position request has started
    SUPL_PosResponse, ///< A position response has been reported to network
    SUPL_PosAbort,    ///< Positioning Session aborted by network
    SUPL_PosReport    ///< Position report returned to network
} e_GN_SUPL_DebugEvents;

//*****************************************************************************
/// \brief
///      e_GN_SUPL_DebugEvents Definition
/// \details
///      The position returned to network by SUPL library in case of a succesful
///      positioning session.
typedef struct
{
    R8    Latitude;  ///<  latitude
    R8  Longitude; ///<  longitude
    R4  Altitude;  ///<  altitude from +32767 to -32767
} s_GN_SUPL_DebugPosReport;

//*****************************************************************************
/// \brief
///      u_GN_SUPL_DebugEventPayload Definition
/// \details
///      Contains all payloads associated with different debug events.
typedef union
{
    s_GN_SUPL_DebugPosReport  posReport; ///< Positon reported to network
} u_GN_SUPL_DebugEventPayload;

//*****************************************************************************
/// \brief
///      u_GN_SUPL_DebugEventPayload Definition
/// \details
///      Contains all payloads associated with different debug events.

typedef enum
{
    GN_TriggeredModeNone,
    GN_TriggeredModePeriodic,
    GN_TriggeredModeAreaEvent
} e_GN_SUPL_V2_TriggeredMode;

//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_PeriodicTrigger Definition
/// \details
///      Contains Periodic Trigger Parameters for Notifying SUPL when the event is triggered.

typedef struct
{
    U4  NumberOfFixes;       ///<  Number of the fixes requested for the periodic trigger session
    U4  IntervalBetweenFixes;///< Time Interval between the fixes
    U4  StartTime;             ///<  Start time of the periodic trigger session
} s_GN_SUPL_V2_PeriodicTrigger;

//*****************************************************************************
/// \brief
///      e_GN_SUPL_V2_AreaEventType Definition
/// \details
///      Contains Area Event Type.

typedef enum
{
    GN_AreaEventType_EnteringArea,   ///< Defines Area Event as Entering
    GN_AreaEventType_InsideArea,     ///< Defines Area Event as Inside
    GN_AreaEventType_OutsideArea,    ///< Defines Area Event as Outside
    GN_AreaEventType_LeavingArea     ///< Defines Area Event as leaving
} e_GN_SUPL_V2_AreaEventType;

/// \brief
///      Structure to store the UTC and GAD position data for a cell.
/// \details
///      The UTC and GAD position data of each cell is stored and updated in the database

typedef struct
{
 s_GN_GPS_UTC_Data utc; ///< UTC Time \ ref s_GN_GPS_UTC_Data
 s_GN_AGPS_GAD_Data gad;///<GAD Position data \ ref s_GN_AGPS_GAD_Data
}s_GN_SUPL_PVTData;
//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_RepeatedReportingParams Definition
/// \details
///      Contains Reporting Criteria for Periodic or Events Trigger.


typedef struct
{
    U4 minimumIntervalTime;     ///< Minimum interval time between reports,  0-Indicates Not set
    U4 maximumNumberOfReports; ///< Maximum number of reports that is requested in the Triggered session, 0-Indicates Not set
} s_GN_SUPL_V2_RepeatedReportingParams;


//*****************************************************************************
/// \brief
///      e_GN_SUPL_V2_GeographicalTargetAreaType Definition
/// \details
///      Contains type of geographical shape definition.
typedef enum
{
    GN_GeographicalTargetArea_Circlar,   ///< Defines the Target Area Type as Circle
    GN_GeographicalTargetArea_Ellipse,   ///< Defines the Target Area Type as Ellipse  
    GN_GeographicalTargetArea_Polygon    ///< Defines the Target Area Type as Polygon
} e_GN_SUPL_V2_GeographicalTargetAreaType;



//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_Coordinate Definition
/// \details
///      Define latitude longitude of a point in GAD specification.
typedef struct
{
    U1 latitudeSign;           ///< Latitude sign [0=North, 1=South]
    U4 latitude;               ///< Latitude      [range 0..8388607 for 0..90 degrees]
    I4 longitude;              ///< Longitude     [range -8388608..8388607 for -180..+180 degrees]
} s_GN_SUPL_V2_Coordinate;
//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_CircularArea Definition
/// \details
///      Contains parameters to define a circular area.
typedef struct
{
    s_GN_SUPL_V2_Coordinate   center;    ///< Indicates the Center position of the circle
    U4                        radius;    ///< Radius of the circle in meters. [range 1..1000000m].
    U4                        radiusMin; ///< Hysteresis Minimum Radius of the circle in meters [range 0..1000000m]
    U4                        radiusMax; ///< Hysteresis Maximum Radius of the circle in meters [range 0..1500000m] 
} s_GN_SUPL_V2_CircularArea;


//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_EllipticalArea Definition
/// \details
///      Contains parameters to define a elliptical area.
typedef struct
{
    s_GN_SUPL_V2_Coordinate   center;          ///< Indicates the Center position of the Ellipse
    U4                        semiMajor;       ///< Semi Major of Ellipse in meters. [range 1..1000000m].
    U4                        semiMajorMax;    ///< Hysteresis Minimum Semi Major of Ellipse in meters [range 0..1000000m]
    U4                        semiMajorMin;    ///< Hysteresis Maximum Semi Major of Ellipse in meters [range 0..1500000m] 
    U4                        semiMinor;       ///< Semi Minor of Ellipse in meters. [range 1..1000000m].
    U4                        semiMinorMax;    ///< Hysteresis Minimum Semi Minor of Ellipse in meters [range 0..1000000m]
    U4                        semiMinorMin;    ///< Hysteresis Maximum Semi Minor of Ellipse in meters [range 0..1500000m] 
    U1                        semiMinorAngle;  ///< Semi Minor Angle [range 0.. 179]
} s_GN_SUPL_V2_EllipticalArea;
//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_PolygonArea Definition
/// \details
///      Contains parameters to define a polygon area.
typedef struct
{
    s_GN_SUPL_V2_Coordinate* coordinates;       ///< Contains coordinates of all points of  Polygon
    U1                       numOfPoints;       ///< Number of points in polygon. [range 3..15].
    U4                       polygonHysteresis; ///< Hysteresis of polygon in meters. [range 0..100000m].
} s_GN_SUPL_V2_PolygonArea;


//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_GeographicTargetArea Definition
/// \details
///      Contains Geographical Target Area Parameters.
typedef struct
{
    e_GN_SUPL_V2_GeographicalTargetAreaType    shapeType; ///< Indicates the shape of Target Area
    union {
          s_GN_SUPL_V2_CircularArea   circularArea;       ///< Defines the shapeType as Circle
          s_GN_SUPL_V2_EllipticalArea ellipticalArea;     ///< Defines the shapeType as Ellipse
          s_GN_SUPL_V2_PolygonArea    polygonArea;        ///< Defines the shapeType as Polygon
          } choice; 
} s_GN_SUPL_V2_GeographicTargetArea;

//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_AreaIdSet Definition
/// \details
///      Contains Geographical Target Area Parameters.
typedef struct
{
    e_GN_SUPL_V2_AreaIdInfoType    AreaIdType; ///< Indicates the shape of Target Area
    union {
          s_GN_SUPL_V2_gSMAreaIdType    gsmAreaIdInfo;       ///< Area Id is from a GSM network.
          s_GN_SUPL_V2_wCDMAAreaIdType  wcdmaAreaIdInfo ;    ///< Area Id is from a WCDMA network.
          s_GN_SUPL_V2_cDMAAreaIdType   cdmaAreaIdInfo ;     ///< Area Id is from a CDMA network.
          } choice; 
} s_GN_SUPL_V2_AreaIdSet;

typedef struct
{
    U1                          NumAreaId;      ///< Describes the number of Area Id */
    s_GN_SUPL_V2_AreaIdSet*     s_AreaIdSetParms;       ///<  gives the lists of area ids */
}s_GN_SUPL_V2_AreaIdLists;

//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_AreaEventTrigger Definition
/// \details
///      Contains Area Event Trigger Parameters.
typedef struct
{
    e_GN_SUPL_V2_AreaEventType              areaEventType;               ///<Indicates the Type of Area Event occuring
    BL                                      locationEstimateRequested;   ///<Indicates whether location estimate is requested 
    s_GN_SUPL_V2_RepeatedReportingParams    repeatedReportingParams;     ///<Indicates the Minimum Interval time and No. of reports required
    U4                                      startTime;                   ///< Start Time. In seconds. Set to 0XFFFFFFFF if not present
    U4                                      stopTime;                    ///< Stop Time. In seconds. Set to 0XFFFFFFFF if not present
    s_GN_SUPL_V2_GeographicTargetArea*      geographicTargetAreaParams;  ///<Indicates the Area Type(Circle,Ellipse or Polygon) 
    U1                                      geographicTargetAreaCount;   ///<Indicates the number of target areas
    s_GN_SUPL_V2_AreaIdLists*               AreaIdListsParams;           ///<Indicates the AreaIdList parameters
    U1                                      AreaIdListsCount;            ///<Indicates the number of target areas
} s_GN_SUPL_V2_AreaEventTrigger;


//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_Triggers Definition
/// \details
///      Contains Trigger events required fro SUPL session. This is SUPL V2 addtion.


typedef struct
{
    e_GN_SUPL_V2_TriggeredMode     triggeredMode;   ///< Mode of the Trigger session either area or periodic \ref e_GN_SUPL_V2_TriggeredMode
    s_GN_SUPL_V2_PeriodicTrigger   periodicTrigger; ///< Contains Periodic Trigger Parameters \ref s_GN_SUPL_V2_PeriodicTrigger
    s_GN_SUPL_V2_AreaEventTrigger  areaEventTrigger;///< Contains ARea Trigger Parameters \ref s_GN_SUPL_V2_AreaEventTrigger
} s_GN_SUPL_V2_Triggers;


//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_ThirdPartyInfoDefinition
/// \details
///      Contains information to identify a third party entity with which position information can be shared.
typedef struct
{
    e_thirdparty_id thirdPartyId;                 /// Third Party ID
    CH*             thirdPartyIdName;            /// Third Party Id string */
}s_GN_SUPL_V2_ThirdPartyInfo;



//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_ApplicationInfo
/// \details
///      Contains information of application requesting for positiong service.
typedef struct
{
    U1    applicationIDInfoPresence;       /// Application Id Info Present */
    CH*   applicationProvider;             /// Application Provider string */  
    CH*   appName;                         /// Application Name string */
    CH*   appVersion;                      /// Application Version string */
}s_GN_SUPL_V2_ApplicationInfo;


//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_SupportedWLANInfo
/// \details
///      Contains information of Supported WLAN.
typedef struct
{
    BL     apTP;
    BL     apAG;
    BL     apSN;
    BL     apDevType;
    BL     apRSSI;
    BL     apChanFreq;
    BL     apRTD;
    BL     setTP;
    BL     setAG;
    BL     setSN;
    BL     setRSSI;
    BL     apRepLoc;
} s_GN_SUPL_V2_SupportedWLANInfo;


//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_SupportedWCDMAInfo
/// \details
///      Contains information of Supported WCDMA.
typedef struct
{
    BL     mRL;
} s_GN_SUPL_V2_SupportedWCDMAInfo;


//*****************************************************************************
/// \brief
///      s_GN_SUPL_V2_SupportedNetworkInfo
/// \details
///      Contains information of Supported Network.
typedef struct
{
    BL     wLAN;
    s_GN_SUPL_V2_SupportedWLANInfo    *supportedWLANInfo    /* OPTIONAL */;
    BL     gSM;
    BL     wCDMA;
    s_GN_SUPL_V2_SupportedWCDMAInfo    *supportedWCDMAInfo    /* OPTIONAL */;
    BL     cDMA;
    BL     hRDP;
    BL     uMB;
    BL     lTE;
    BL     wIMAX;
    BL     historic;
    BL     nonServing;
    BL     uTRANGPSReferenceTime;
    BL     uTRANGANSSReferenceTime;
}s_GN_SUPL_V2_SupportedNetworkInfo;


//*****************************************************************************
/// \brief
///      Initialises the SUPL_Handler.
/// \details
///      Sets up queues, instance data and memory allocations required.
///      <p> Called once at startup by the host to initialise the SUPL handler.
/// \returns
///      Nothing.
void GN_SUPL_Handler_Init( void );


//*****************************************************************************
/// \brief
///      DeInitialises the SUPL_Handler.
/// \details
///      Cleans up queues, instance data and memory allocations used.
///      <p> Called once at shutdown by the host to deinitialise the SUPL handler.
/// \note
///      #GN_SUPL_Handler_DeInit() will not attempt to close down any
///      connections to prevent the risk of deadlock.
/// \returns
///      Nothing.
void GN_SUPL_Handler_DeInit( void );


//*****************************************************************************
/// \brief
///      SUPL_Handler process block.
/// \details
///      Called in the main processing loop to perform the SUPL related
///      processing functions.
///      <p> This module does the main SUPL processing is exercised to
///      enable the SUPL Handler to process SUPL related events.
/// \returns
///      Nothing.
void GN_SUPL_Handler( void );


//*****************************************************************************
/// \brief
///      SUPL outgoing connection indication.
/// \details
///      Confirms a connection for SUPL to an external network entity.
///      <p> This function is called in response to a GN_SUPL_Connect_Req_Out()
///      the GN_SUPL_Connect_Ind_In()::Handle must be set to the
///      value provided in GN_SUPL_Connect_Req_Out()::Handle.
///      <H3>p_Status values passed in:</H3>
///      <ul>
///         <li><var> GN_SUCCESS \ ref e_GN_Status
///            p_Status should be set to this value when the GN_SUPL_Connect_Req_Out()
///            was successful.</li>
///         <li><var> GN_ERR_NO_RESOURCE \ ref e_GN_Status
///            p_Status should be set to this value when the PDP attach failed</li>
///         <li><var> GN_ERR_CONN_TIMEOUT \ ref e_GN_Status
///            p_Status should be set to this value when the server could not be found.</li>
///         <li><var> GN_ERR_CONN_REJECTED \ ref e_GN_Status
///            p_Status should be set to this value when the connection to the server was refused.
///               Example : when the TLS Handshake failed.</li>
///      </ul>
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var> GN_SUCCESS \ ref e_GN_Status
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var> GN_ERR_HANDLE_INVALID \ ref e_GN_Status
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the library software.</li>
///         <li><var> GN_ERR_NO_RESOURCE \ ref e_GN_Status
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \attention
///      The caller of GN_SUPL_Connect_Ind_In() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_LocationId.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Connect_Ind_In(
   void*          Handle,        ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,      ///< [in/out] Status of disconnect and to be checked when return flag indicates failure.
   s_LocationId*  p_LocationId   ///< [in] Location ID required for the SUPL exchange.
);

//*****************************************************************************
/// \brief
///      SUPL Delete_LocationID_Data.
/// \details
///      SUPL Delete_LocationID_Data.
/// \returns
///      Flag to indicate success or failure of the occurence of event.
/// \retval #TRUE Flag indicating Area Event has occured
/// \retval #FALSE Flag indicatingArea Event has not occured and reporting is not required.
BL GN_SUPL_Delete_LocationID_Data_Ind_In( void );


//*****************************************************************************
/// \brief
///      SUPL incoming disconnection indication.
/// \details
///      Confirms disconnection for an existing SUPL connection.
///      <p> This function is called in response to a
///      GN_SUPL_Disconnect_Req_Out() and the Handle is set to the Handle
///      from the GN_SUPL_Disconnect_Req_Out() call or is called when a
///      connection spontaneously terminates when the Handle is set to the
///      Handle in the GN_SUPL_Connect_Req_Out() request.
///      <H3>p_Status values passed in:</H3>
///      <ul>
///         <li><var> GN_SUCCESS \ ref e_GN_Status
///            p_Status should be set to this value when the GN_SUPL_Disconnect_Req_Out()
///            was successful.</li>
///         <li><var> GN_ERR_CONN_SHUTDOWN \ ref e_GN_Status
///            p_Status should be set to this value when the connection
///            associated with the Handle was spontaneously shut down.</li>
///      </ul>
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var> GN_SUCCESS \ ref e_GN_Status
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var> GN_ERR_HANDLE_INVALID \ ref e_GN_Status
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the library software.</li>
///         <li><var> GN_ERR_NO_RESOURCE \ ref e_GN_Status
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \attention
///      The caller of #GN_SUPL_Disconnect_Ind_In() must manage (e.g. alloc and free)
///      the memory referenced by p_Status.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Disconnect_Ind_In(
   void*          Handle,     ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status    ///< [in/out] Status of disconnect and to be checked when return flag indicates failure.
);


//*****************************************************************************
/// \brief
///      SUPL Push delivery in.
/// \details
///      Delivers a SUPL Push from an SMS or MMS.
///      <p> This function is called in response to a SUPL-INIT being received
///      via an SMS or MMS and is then delivered to the SUPL_Handler.
///      <p> A Handle will be created by the SUPL subsystem and placed
///      in the location pointed at by p_NewHandle.
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var> GN_SUCCESS \ ref e_GN_Status
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var> GN_ERR_NO_RESOURCE \ ref e_GN_Status
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
///      \anchor VER_Generation
/// <H3>VER Hash Generation</H3>
///      <p> For Proxy mode SUPL-POS-INIT must contain a verification field (VER) which
///      is an HMAC (Hash Message Authentication Code) which is generated as below:
///      <ul>
///         <li> VER = H(H-SLP XOR opad, H(H-SLP XOR ipad, SUPL INIT)) </li>
///         <li> H = Hash function (SHA-1). </li>
///         <li> H-SLP = FQDN of the SLP. </li>
///         <li> SUPL-INIT = SUPL-INIT received in the PUSH. </li>
///         <li> IPAD (Inner PAD) = Array of 0x36, length of H-SLP. </li>
///         <li> OPAD (Outer PAD) = Array of 0x5c, length or H-SLP. </li>
///      </ul>
/// \attention
///      The caller of GN_SUPL_Push_Delivery_In() must manage (e.g. alloc and free)
///      the memory referenced by p_NewHandle, p_Status, p_PDU and p_VER.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Push_Delivery_In(
   void**         p_NewHandle,///< [out] Opaque Handle Filled in by SUPL.
   e_GN_Status*   p_Status,   ///< [out] Status of disconnect and to be checked when return flag indicates failure.
   U2             PDU_Size,   ///< [in] Size of data at p_PDU in bytes.
   U1*            p_PDU,      ///< [in] Pointer to data.
   U2             VER_Size,   ///< [in] Size of \ref VER_Generation "hash" at p_VER in bytes.
   U1*            p_VER       ///< [in] Pointer to \ref VER_Generation "hash".
);


//*****************************************************************************
/// \brief
///      SUPL PDU delivery in.
/// \details
///      Delivers a SUPL PDU from an existing network connection.
///      <p> This function is called in response to data being sent over a
///      TCP/IP connection set up by a GN_SUPL_Connect_Req_Out()
///      function call.
///      <p> The Handle is set to the Handle from the
///      GN_SUPL_Connect_Req_Out() call.
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var> GN_SUCCESS \ ref e_GN_Status
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var> GN_ERR_HANDLE_INVALID \ ref e_GN_Status
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the library software.</li>
///         <li><var> GN_ERR_NO_RESOURCE \ ref e_GN_Status
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \attention
///      The caller of #GN_SUPL_PDU_Delivery_In() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_PDU.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_PDU_Delivery_In(
   void*          Handle,     ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,   ///< [out] Status of PDU delivery and to be checked when return flag indicates failure.
   U2             PDU_Size,   ///< [in] Size of data at p_PDU in bytes.
   U1*            p_PDU       ///< [in] Pointer to data.
);


//*****************************************************************************
/// \brief
///      SUPL notification response in.
/// \details
///      Provides a response from the user notification.
///      <p> This function is called in response to a notification request
///      sent in GN_SUPL_Notification_Req_Out() required by the SUPL
///      exchange.
///      <p> The Handle is set to the Handle from the
///      GN_SUPL_Notification_Req_Out() call.
///      <H3>p_Status values passed in:</H3>
///      <ul>
///         <li><var> GN_SUCCESS \ ref e_GN_Status
///            p_Status should be set to this value when the GN_SUPL_Disconnect_Req_Out()
///            was successful.</li>
///         <li><var> GN_ERR_NO_RESOURCE \ ref e_GN_Status
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var> GN_SUCCESS \ref e_GN_Status
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var> GN_ERR_HANDLE_INVALID \ ref e_GN_Status
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the library software.</li>
///         <li><var> GN_ERR_NO_RESOURCE \ ref e_GN_Status
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \attention
///      The caller of GN_SUPL_Notification_Rsp_In() must manage (e.g. alloc and free)
///      the memory referenced by p_Status.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Notification_Rsp_In(
   void*          Handle,        ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,      ///< [in/out] Status of notification and to be checked when return flag indicates failure.
   BL    Notification_Accepted   ///< [in] Flag to indicate the user response to the indication.
                                 /// <ul>
                                 ///   <li>#TRUE = Accepted by user.</li>
                                 ///   <li>#FALSE = Rejected by user.</li>
                                 /// </ul>
);


//*****************************************************************************
/// \brief
///      SUPL position request in.
/// \details
///      Requests a position from the SUPL subsystem.
///      <p> This function is called to obtain a position from the SUPL
///      subsystem and is likely to initiate a Mobile Originated supl
///      sequence depending on QoP parameters.
///      <p> A Handle will be created by the SUPL subsystem and placed
///      in the location pointed at by p_NewHandle.
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var> GN_SUCCESS \ ref e_GN_Status
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var> GN_ERR_NO_RESOURCE \ ref e_GN_Status
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \attention
///      The caller of GN_SUPL_Position_Req_In() must manage (e.g. alloc and free)
///      the memory referenced by p_NewHandle, p_Status and p_SUPL_QoP.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Position_Req_In(
   void**         p_NewHandle,   ///< [out] Opaque Handle Filled in by SUPL.
   e_GN_Status*   p_Status,      ///< [out] Status of notification and to be checked when return flag indicates failure.
   s_GN_SUPL_QoP* p_SUPL_QoP     ///< [in] QoP criteria. NULL pointer if no QoP criteria set.
);

//*****************************************************************************
/// \brief
///      SUPL position request in.
/// \details
///      Requests a position from the SUPL subsystem.
///      <p>  This function is used to populate Third Party Info on to the SUPL instance for Set Initiated SUPL session.
///      <p> This function posts an event event_SUPL_V2_Third_Party_info_Request to populate the third party id info 
///             to the supl instance
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \attention
///      The caller of #GN_SUPL_Position_Req_In() must manage (e.g. alloc and free)
///      the memory referenced by p_NewHandle, p_Status and p_SUPL_QoP.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.

BL GN_SUPL_Start_ThirdParty_Location_Transfer(
   void*         p_NewHandle,   ///< [out] Opaque Handle Filled in by SUPL.
   s_GN_SUPL_V2_ThirdPartyInfo *tp_info,   
   e_GN_Status*   p_Status  ///< [out] Status of notification and to be checked when return flag indicates failure.
);
//*****************************************************************************
/// \brief
///      SUPL position request in.
/// \details
///      Requests a position from the SUPL subsystem.
///      <p> This function is used to populate Application Id Info on to the SUPL instance for Set Initiated SUPL session.
///      <p> This function posts an event event_SUPL_V2_ApplicationId_info_Request to populate the Appid info 
///             to the supl instance
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \attention
///      The caller of #GN_SUPL_Position_Req_In() must manage (e.g. alloc and free)
///      the memory referenced by p_NewHandle, p_Status and p_SUPL_QoP.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.

BL GN_SUPL_ApplicationID_Info_In(
   void*         p_NewHandle,   ///< [out] Opaque Handle Filled in by SUPL.
   s_GN_SUPL_V2_ApplicationInfo  *p_SUPL_ApplicationIDInfo,   
   e_GN_Status*   p_Status  ///< [out] Status of notification and to be checked when return flag indicates failure.
);

//*****************************************************************************
/// \brief
///      Mobile Info indication.
/// \details
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, Status will be set to True.
/// \retval #FALSE Flag indicating failure , Status will be set to False.

BL GN_SUPL_Mobile_Info_Ind_In(
     s_LocationId*  p_SUPL_LocationId   ///< [in] Location ID required for the SUPL exchange.
);

//*****************************************************************************
/// \brief
///      GN SUPL API Function to request for location of a target SET.
/// \details
///      GN SUPL APIFunction to request for location of a target SET.
///      <p> Typically this function is called the once after #GN_SUPL_Handler_Init()
///      and before entering the main #GN_SUPL_Handler() processing loop.
/// \returns
///      Flag to indicate whether the new configuration settings were accepted.
/// \retval #TRUE if accepted.
/// \retval #FALSE if not accepted.
BL GN_SUPL_ThirdPartyPosition_Req_In(
    void**         p_NewHandle,   ///< [out] Opaque Handle Filled in by SUPL.
    e_GN_Status*   p_Status,      ///< [out] Status of notification and to be checked when return flag indicates failure.
    s_GN_SUPL_QoP* p_SUPL_QoP,    ///< [in] QoP criteria. NULL pointer if no QoP criteria set.
    s_GN_SUPL_V2_ThirdPartyInfo  *tp_info        ///<[in]. Target SET i.e third party's info
);


//*****************************************************************************
/// \brief
///      GN SUPL API Function to Set or change the SUPL Library Configuration data.
/// \details
///      GN SUPL API Function to Set or change the SUPL Library Configuration data.
///      <p> Typically this function is called the once after GN_SUPL_Handler_Init()
///      and before entering the main GN_SUPL_Handler() processing loop.
///      This configuration will fail if the major version requested is not supported by
///      supl library
/// \returns
///      Flag to indicate whether the new configuration settings were accepted.
/// \retval TRUE if accepted.
/// \retval FALSE if not accepted.
BL GN_SUPL_Set_Config(
   s_GN_SUPL_Config* p_Config    ///< [in] Pointer to where the SUPL Library can get the Configuration data from.
);
//*****************************************************************************
/// \brief
///      GN SUPL API Function to Set or change the SUPL Library Extended Configuration data.
/// \details
///      GN SUPL API Function to Set or change the SUPL Library Extended Configuration data.
///      <p> Typically this function is called the once after GN_SUPL_Handler_Init()
///      and before entering the main GN_SUPL_Handler() processing loop.
///      This configuration will fail if the major version requested is not supported by
///      supl library
/// \returns
///      Flag to indicate whether the new configuration settings were accepted.
/// \retval TRUE if accepted.
/// \retval FALSE if not accepted.
BL GN_SUPL_Extd_Set_Config(
   s_GN_SUPL_Extd_Config* p_Config    ///< [in] Pointer to where the SUPL Library can get the Configuration data from.
);


//*****************************************************************************

/* +LMSqc38060 */
//*****************************************************************************



/// \brief
///      GN_SUPL_Abort - Aborting the supl session as part of GPS stop request
/// \details
/// \note
/// \attention
/// \returns
///      Flag to indicate success or failure of the posting of the request.

void GN_SUPL_Abort(
   void*          Handle  ///< [in] Opaque Handle used to coordinate requests.
);

/// \brief
///      GN_SUPL_Trigger_End_Ind_In - End the Trieggered supl session when the stop time is reached or when the maximum
///     number of reports has been reached
/// \details
/// \note
/// \attention
/// \returns
///      Flag to indicate success or failure of the posting of the request.

void GN_SUPL_Trigger_End_Ind_In(
   void*          Handle  ///< [in] Opaque Handle used to coordinate requests.
);



//**
/* -LMSqc38060 */


/// \brief
///      GN_SUPL_Trigger_Ind_In
/// \details
///      This function is used to indicate to SUPL library that a trigger has occured for a particular session.
///      Following this indication, SUPL library will start a positioning session with the SUPL server to report
///      either the event or the position based on the request from the server.
/// \note
/// \attention
/// \returns
///      Flag to indicate success or failure of the posting of the request.

BL GN_SUPL_Trigger_Ind_In(
   void*          Handle,       ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status      ///< [out] Status of notification and to be checked when return flag indicates failure.
);


/// \brief
///      GN_SUPL_TriggerPeriodic_Req_In
/// \details
///      This function is used to trigger periodic session for Set Initiated SUPL session.
/// \note
/// \attention
/// \returns
///      Flag to indicate success or failure of the posting of the request.

BL GN_SUPL_TriggerPeriodic_Req_In(
    void*           Handle,       ///< [in] Opaque Handle used to coordinate requests.
    s_GN_SUPL_V2_Triggers* p_Trigger,       ///< [in] Pointer to Trigger Events.
    e_GN_Status*   p_Status   ///< [out] Status of PDU delivery and to be checked when return flag indicates failure.

);



/// \}

//*****************************************************************************
/// \addtogroup GN_SUPL_api_H
/// \{


//*****************************************************************************
/// \brief
///      SUPL connection request parameters.
/// \details
///      Structure to indicate the destination of a connection to be set up
///      for the SUPL subsystem.
typedef struct // GN_SUPL_Connect_Req_Parms
{
   e_TcpIp_AddressType TcpIp_AddressType; ///< Type of IP address passed.
   CH*   p_TcpIp_Address;        ///< IP address in string format.
   U2    Port;                   ///< Port number as an unsigned integer.
} s_GN_SUPL_Connect_Req_Parms;


//*****************************************************************************
/// \brief
///      Format of Notification Identifier.
/// \details
///      Enum to indicate the format of the notification identifier passed.
typedef enum // GN_SUPL_Notify_Fmt_Ind
{
  NOTIFY_noFormat,               ///< No format specified.
  NOTIFY_logicalName,            ///< Identifier contains a Name.
  NOTIFY_e_mailAddress,          ///< Identifier contains an Email Address.
  NOTIFY_msisdn,                 ///< Identifier contains an MSISDN.
  NOTIFY_url,                    ///< Identifier contains a URL.
  NOTIFY_sipUrl,                 ///< Identifier contains a SIP URL.
  NOTIFY_min,                    ///< Identifier contains an MIN.
  NOTIFY_mdn,                    ///< Identifier contains an MDN.
  NOTIFY_imsPublicIdentity       ///< Identifier contains an IMS Public Identity.
} e_GN_SUPL_Notify_Fmt_Ind;      ///< Format of notification identifier passed.


//*****************************************************************************
/// \brief
///      Type of Notification.
/// \details
///      Enum to indicate the type of the notification passed.
typedef enum // GN_SUPL_Notify_Type
{
   NOTIFY_NONE,                  ///< Provide no notification to the user but indicate permission or rejection of request.
   NOTIFY_ONLY,                  ///< Provide notification to the user.
   NOTIFY_ALLOWED_ON_TIMEOUT,    ///< Provide confirmation request which will default to being allowed if no response.
   NOTIFY_DENIED_ON_TIMEOUT,     ///< Provide confirmation request which will default to being disallowed if no response.
   NOTIFY_PRIVACY_OVERRIDE       ///< Provide notification that a privacy overide supl request has been received and no logging should take place.
} e_GN_SUPL_Notify_Type;         ///< Type of notification passed.


//*****************************************************************************
/// \brief
///      Encoding used in Notification Identifier.
/// \details
///      Enum to indicate the encoding of the requestor id or client name passed
///      in the notification identifier.
/// \note
///      ENCODING_gsmDefault refers to the 7-bit default alphabet and the SMS
///      packing specified in [3GPP 23.038]
typedef enum // e_GN_SUPL_Encoding_Type
{
   ENCODING_NONE,                ///< No encoding type specified in SUPL-INIT.
   ENCODING_ucs2,                ///< Notification identifier encoded using UCS2.
   ENCODING_gsmDefault,          ///< Notification identifier encoded using GSM Default.
   ENCODING_utf8                 ///< Notification identifier encoded using UTF8.
} e_GN_SUPL_Encoding_Type;       ///< Encoding used in the notification identifier.


//*****************************************************************************
/// \brief
///      SUPL notification indication parameters.
/// \details
///      Structure to indicate that a notification has been requested in a
///      SUPL-INIT.
typedef struct // GN_SUPL_Notification_Parms
{
   e_GN_SUPL_Notify_Type      GN_SUPL_Notify_Type;    ///< Notification type passed.
   e_GN_SUPL_Encoding_Type    GN_SUPL_Encoding_Type;  ///< Encoding used in the Notification Identifier.
   U1*                        RequestorId;            ///< Requestor Identifier.
   U1                         RequestorIdLen;         ///< Length of Requestor Identifier in bytes (maxReqLength = 50)
   e_GN_SUPL_Notify_Fmt_Ind   RequestorIdType;        ///< Format of Requestor Identifier passed.
   U1*                        ClientName;             ///< Name of the Client.
   U1                         ClientNameLen;          ///< Length of Client Name in bytes (maxClientLength = 50)
   e_GN_SUPL_Notify_Fmt_Ind   ClientNameType;         ///< Format of Client Name passed.
} s_GN_SUPL_Notification_Parms;


//-----------------------------------------------------------------------------
/// \brief
///      GN SUPL Position Solution data.
/// \details
///      GN SUPL Position Solution data.
///      This data is sufficient to cover the position passed between the SUPL
///      library and the SLP. This is also used to present the final SUPL solution
///      to the user.
typedef struct // GN_SUPL_Pos_Data
{
   // Optional items
   BL AltitudeInfo_Valid;     ///< Altitude fields valid: Altitude and V_AccEst.
   BL HorizontalVel_Valid;    ///< Horizontal Velocity fields valid: HorizontalVel and Bearing.
   BL VerticalVel_Valid;      ///< Vertical Velocity field valid: VerticalVel.
   BL H_Acc_Valid;            ///< Horizontal Uncertainty fields valid: H_AccMaj, H_AccMin and H_AccMajBrg.
   BL HVel_AccEst_Valid;      ///< Horizontal Velocity uncertainty field valid: HVel_AccEst.
   BL VVel_AccEst_Valid;      ///< Vertical Velocity uncertainty field valid: VVel_AccEst.
   BL Confidence_Valid;       ///< Confidence field valid: Confidence.
   // Time
   U2 Year;                   ///< UTC Year A.D.                     [eg 2007].
   U2 Month;                  ///< UTC Month into the year           [range 1..12].
   U2 Day;                    ///< UTC Days into the month           [range 1..31].
   U2 Hours;                  ///< UTC Hours into the day            [range 0..23].
   U2 Minutes;                ///< UTC Minutes into the hour         [range 0..59].
   U2 Seconds;                ///< UTC Seconds into the hour         [range 0..59].
   U2 Milliseconds;           ///< UTC Milliseconds into the second  [range 0..999].
   // Position
   R8 Latitude;               ///< WGS84 Latitude  [degrees, positive North].
   R8 Longitude;              ///< WGS84 Longitude [degrees, positive East].
   R4 Altitude;               ///< Altitude above the WGS84 Ellipsoid [m].
   // Velocity
   R4 HorizontalVel;          ///< 2-dimensional Horizontal Velocity [m/s].
   R4 Bearing;                ///< 2-dimensional Bearing [degrees].
   R4 VerticalVel;            ///< Vertical Velocity [m/s, positive Up].
   // Accuracy Estimates
   R4 V_AccEst;               ///< Vertical Accuracy estimate [m].
   R4 H_AccMaj;               ///< Horizontal Error ellipse semi-major axis [m].
   R4 H_AccMin;               ///< Horizontal Error ellipse semi-minor axis [m].
   R4 H_AccMajBrg;            ///< Bearing of the Horizontal Error ellipse semi-major axis [degrees].
   R4 HVel_AccEst;            ///< 2-dimensional Horizontal Velocity accuracy estimate [m/s].
   R4 VVel_AccEst;            ///< 2-dimensional Vertical Velocity accuracy estimate [m/s].
   U1 Confidence;             ///< Confidence of the solution in a percentage where RMS 1-sigma equals (67%) range [1..100].

} s_GN_SUPL_Pos_Data;          // GN SUPL Position solution data


//*****************************************************************************
/// \brief
///      SUPL outgoing connection request.
/// \details
///      Requests a connection for SUPL to an external network entity.
///      <p> The Handle is set by the SUPL Handler or is set to Handle
///      provided by the GN_SUPL_Push_Delivery_In() call.
///      <p> Following the GN_SUPL_Connect_Req_Out() the SUPL subsystem waits
///      for a GN_SUPL_Connect_Ind_In() when the connection to the SLP is
///      established.
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var> GN_SUCCESS \ ref e_GN_Status
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var> GN_ERR_NO_RESOURCE \ ref e_GN_Status
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the LBS SUPL library.
/// \attention
///      The caller of GN_SUPL_Connect_Req_Out() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_SUPL_Connect_Req_Parms.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Connect_Req_Out(
   void*          Handle,        ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,      ///< [out] Status of connection request and to be checked when return flag indicates failure.
   s_GN_SUPL_Connect_Req_Parms*
      p_SUPL_Connect_Req_Parms   ///< [in] Details of connection.
);


//*****************************************************************************
/// \brief
///      SUPL disconnection request.
/// \details
///      Requests a disconnection for an existing SUPL connection.
///      <p> The Handle is set to the Handle from the
///      GN_SUPL_Connect_Req_Out() call.
///      <H3>p_Status values passed in:</H3>
///      <ul>
///         <li><var>GN_SUCCESS \ ref e_GN_Status
///            p_Status should be set to this value when the GN_SUPL_Disconnect_Req_Out()
///            was successful.</li>
///         <li><var>GN_ERR_CONN_SHUTDOWN \ ref e_GN_Status
///            p_Status should be set to this value when the connection
///            associated with the Handle was spontaneously shut down.</li>
///      </ul>
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var> GN_SUCCESS \ref e_GN_Status
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var> GN_ERR_HANDLE_INVALID \ ref e_GN_Status
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the host software.</li>
///         <li><var> GN_ERR_NO_RESOURCE \ ref e_GN_Status
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///         <li><var> GN_ERR_CONN_SHUTDOWN \ ref e_GN_Status
///            p_Status should be set to this value when the connection
///            associated with the handle is no longer available.</li>
///      </ul>
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the LBS SUPL Library.
/// \attention
///      The caller of #GN_SUPL_Disconnect_Req_Out() must manage (e.g. alloc and free)
///      the memory referenced by p_Status.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Disconnect_Req_Out(
   void*          Handle,     ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status    ///< [in/out] Status of disconnect and to be checked when return flag indicates failure.
);


//*****************************************************************************
/// \brief
///      SUPL PDU delivery out.
/// \details
///      Delivers a SUPL PDU to an existing network connection.
///      <p> The Handle is set to the Handle from the
///      GN_SUPL_Connect_Req_Out() call.
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var> GN_SUCCESS e_GN_Status
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var> GN_ERR_HANDLE_INVALID \ ref e_GN_Status
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the host software.</li>
///         <li><var> GN_ERR_POINTER_INVALID \ ref e_GN_Status
///            p_Status should be set to this value when the p_PDU is NULL.</li>
///         <li><var> GN_ERR_PARAMETER_INVALID \ ref e_GN_Status
///            p_Status should be set to this value when the PDU_Size is 0.</li>
///         <li><var> GN_ERR_NO_RESOURCE \ ref e_GN_Status
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///         <li><var> GN_ERR_CONN_SHUTDOWN \ ref e_GN_Status
///            p_Status should be set to this value when the connection
///            associated with the handle is no longer available.</li>
///      </ul>
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the LBS SUPL Library.
/// \attention
///      The caller of GN_SUPL_PDU_Delivery_Out() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_PDU.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_PDU_Delivery_Out(
   void*          Handle,     ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,   ///< [out] Status of PDU delivery and to be checked when return flag indicates failure.
   U2             PDU_Size,   ///< [in] Size of data at p_PDU in bytes.
   U1*            p_PDU       ///< [in] Pointer to data.
);


//*****************************************************************************
/// \brief
///      SUPL notification request out.
/// \details
///      Indicates the handset user should be notified of a location request.
///      Depending on the parameters in the SUPL exchange the user may be
///      given the option to reject the request.
///      <p> The Handle is set to the Handle from the
///      GN_SUPL_Push_Delivery_In() call as a notification will only originate from
///      a SUPL-INIT.
///      <p> The host will respond with GN_SUPL_Notification_Rsp_In() indicating
///      whether the positioning sequence may proceed. If no response is available
///      from the user the Host will respond after a suitable time has elapsed.
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var> GN_SUCCESS \ ref e_GN_Status
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var> GN_ERR_HANDLE_INVALID  \ ref e_GN_Status
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the host software.</li>
///         <li><var>GN_ERR_POINTER_INVALID  \ ref e_GN_Status
///            p_Status should be set to this value when the p_SUPL_Notification_Parms
///            is NULL.</li>
///         <li><var> GN_ERR_PARAMETER_INVALID  \ ref e_GN_Status
///            p_Status should be set to this value when the PDU_Size is 0.</li>
///         <li><var> GN_ERR_NO_RESOURCE  \ ref e_GN_Status
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the LBS SUPL Library.
/// \attention
///      The caller of GN_SUPL_Notification_Req_Out() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_SUPL_Notification_Parms.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval
///     TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS
/// \retval
///     FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Notification_Req_Out(
   void*          Handle,           ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,         ///< [out] Status of disconnect and to be checked when return flag indicates failure.
   s_GN_SUPL_Notification_Parms*
      p_SUPL_Notification_Parms     ///< [in] Pointer to a set of notification parameters.
);


//*****************************************************************************
/// \brief
///      SUPL position response out.
/// \details
///      Provides the Position calculated by the SUPL subsystem to the host.
///      <p> The Handle is set to the Handle from the
///      GN_SUPL_Push_Delivery_In() or a GN_SUPL_Position_Req_In().
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var> GN_SUCCESS \ref e_GN_Status
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var> GN_ERR_HANDLE_INVALID \ ref e_GN_Status
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the host software.</li>
///         <li><var> GN_ERR_POINTER_INVALID \ ref e_GN_Status
///            p_Status should be set to this value when the p_GN_SUPL_Pos_Data
///            is NULL.</li>
///         <li><var> GN_ERR_NO_RESOURCE \ ref e_GN_Status
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the LBS SUPL.
/// \attention
///      The caller of GN_SUPL_Position_Resp_Out() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_GN_SUPL_Pos_Data.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval 
///      TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval 
///     FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_Position_Resp_Out(
   void*          Handle,     ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*   p_Status,   ///< [out] Status of disconnect and to be checked when return flag indicates failure.
   s_GN_SUPL_Pos_Data*
      p_GN_SUPL_Pos_Data      ///< [in] Data structure populated containing the position solution.
);

//*****************************************************************************
/// \brief
///      SUPL Third Party position response out.
/// \details
///      Provides the Position of the third party to the host.
///      <p> The Handle is set to the Handle from the
///      #GN_SUPL_Push_Delivery_In() or a #GN_SUPL_Position_Req_In().
///      <H3>p_Status values returned:</H3>
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_HANDLE_INVALID \copydoc GN_ERR_HANDLE_INVALID</var>\n
///            p_Status should be set to this value when the Handle
///            passed was not recognised by the host software.</li>
///         <li><var>#GN_ERR_POINTER_INVALID \copydoc GN_ERR_POINTER_INVALID</var>\n
///            p_Status should be set to this value when the p_GN_SUPL_Pos_Data
///            is NULL.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources on the host to perform the requested function.</li>
///      </ul>
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the GloNav Library.
/// \attention
///      The caller of #GN_SUPL_Position_Resp_Out() must manage (e.g. alloc and free)
///      the memory referenced by p_Status and p_GN_SUPL_Pos_Data.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success, *p_Status will be set to GN_SUCCESS.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_SUPL_ThirdPartyPosition_Resp_Out(
   void*                Handle,     ///< [in] Opaque Handle used to coordinate requests.
   e_GN_Status*         p_Status,   ///< [out] Status of disconnect and to be checked when return flag indicates failure.
   s_GN_SUPL_Pos_Data*  p_GN_SUPL_Pos_Data      ///< [in] Data structure populated containing the third party position solution.
);
///      GN SUPL API Function to request the Host software to Write the SUPL Library's
///      Event Log data to the Host's chosen debug interface.
/// \details
///      GN SUPL API Function to request the Host software to Write the SUPL Library's
///      Event Log data to the Host's chosen debug interface.
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the SUPL Library.
/// \returns
///      The number of bytes actually written by the host software.  If this is
///      less than the number of bytes requested to be written, then it is
///      assumed that this is because the host side cannot physically handle
///      any more data at this time, and will be buffered internally.
U2 GN_SUPL_Write_Event_Log(
   U2  num_bytes,             ///< [in] Available number of bytes to Write in this API call.
   CH* p_Event_Log            ///< [in] Pointer to where the Host software can get the Event Log data from.
);

//*****************************************************************************
/// \brief
///      GN SUPL API Function to request the Host software to Write the SUPL Library's
///      Event Log data to the CMCC's chosen debug interface.
/// \details
///      GN SUPL API Function to request the Host software to Write the SUPL Library's
///      Event Log data to the CMCC's chosen debug interface. The current implementation
///      is ad-hoc. In future, a more generic implementation should be done and the host
///      software should manage any CMCC specific logging
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the SUPL Library.
/// \returns
///      void
void GN_SUPL_Write_Event_Log_CMCC(
   U4    code,        ///< [in] CMCC defined code to be output
   char* textLog,    ///< [in] Pointer to where the Host software can get the Log data from.
   char* notes        ///< [in] Pointer to where the CMCC specific Host software can get the notes from.
);

//*****************************************************************************
/// \brief
///      GN SUPL API Function to report debug events related to SUPL positioning
///      sessions with the network.
/// \details
///      GN SUPL API Function to report debug events related to SUPL positioning
///      sessions with the network.
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the SUPL Library.
/// \returns
///      void.
void GN_SUPL_Debug_Event(
   e_GN_SUPL_DebugEvents events,            ///< [in] event in the positioning procedure
   u_GN_SUPL_DebugEventPayload  *payload    ///< [in] data associated with the positioning event
);

//*****************************************************************************
/// \brief
///      Calloc definition for LBS SUPL library use.
/// \details
///      Platform implementation of the calloc() function returning a pointer
///      to a block of memory of (Number * Size) number of bytes.
///      The allocated memory area must be initialised to 0.
///      <p> If either of the parameters is 0 #GN_Calloc() will return NULL.
///      <p> The memory can subsequently be deallocated using GN_Free().
/// \returns
///      Pointer to memory or NULL.
/// \retval
///      NULL indicates an error occured while allocating memory.
/// \retval
///      Non_Null values are pointers to a block of successfully allocated memory.
void * GN_Calloc(
   U2 Number_Of_Elements,  ///< [in] Number of elements to allocate.
   U2 Size_Of_Elements     ///< [in] Size of elements to allocate.
);


//*****************************************************************************
/// \brief
///      Free definition for LBS SUPL library use.
/// \details
///      Platform implementation of the free() function taking a pointer
///      to a block of memory to deallocate.
///      <p> The memory to deallocate will have been allocated by the GN_Calloc()
///      routine.
void GN_Free(
   void * p_mem  ///< [in] Pointer to the memory to deallocate.
);


/// \brief
///      GN_SUPL_TriggerPeriodic_Req_Out - Indication from SUPL library that a periodic trigger is requested
/// \details
///      This function is used to indicate to the host that it has to generate events and call the GN_SUPL_Trigger_Ind_In
///      function. The SUPL library depends upon the host to call the trigger function at the correct time.
///      It does not do any validation of the trigger w.r.t whether the trigger is at the correct time.
///      The host should therefore take care to trigger at the correct time.
/// \note
/// \attention
///      Host should call the GN_SUPL_Trigger_Ind_In to generate triggers required for this function
/// \returns
///      void

void GN_SUPL_TriggerPeriodic_Req_Out(
   void*          Handle,                 ///< [in] Opaque Handle used to coordinate requests.
   U4             NumberOfFixesRequested, ///< [in] range 1..8639999. Number of fixes*PeriodicInterval < 100 Days
   U4             PeriodicInterval,       ///< [in] Unit of Seconds . range 1..8639999. Number of fixes*PeriodicInterval < 100 Days
   U4             StartTime               ///< [in] Unit of Seconds . range 0..2678400. Relative to time of function call
);

/// \brief
///      GN_SUPL_TriggerAreaEvent_Req_Out - Indication from SUPL library that a Event trigger is requested
/// \details
///      This function is used to indicate to the host that it has to generate events and call the GN_SUPL_Trigger_Ind_In
///      function. The SUPL library depends upon the host to call the trigger function at the correct time.
///      It does not do any validation of the trigger w.r.t whether the trigger is at the correct time.
///      The host should therefore take care to trigger at the correct time.
/// \note
/// \attention
///      Host should call the GN_SUPL_Trigger_Ind_In to generate triggers required for this function
/// \returns
///      Void

void GN_SUPL_TriggerAreaEvent_Req_Out(
   void*          Handle,                            ///< [in] Opaque Handle used to coordinate requests.
   s_GN_SUPL_V2_AreaEventTrigger* AreaEventParams     /// < [in] Pointer to a set of notification parameters.
);

/// \}

#ifdef __cplusplus
   }     // extern "C"
#endif

#endif   // GN_SUPL_API_H
