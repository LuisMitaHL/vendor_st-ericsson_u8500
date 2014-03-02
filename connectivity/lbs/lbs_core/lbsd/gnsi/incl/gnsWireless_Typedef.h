/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Sabdharishi
 *  Email : sabdharishi.natarajan@stericsson.com
 *****************************************************************************/
#ifndef GNS_WIRELESS_INFO_TYPEDEFS_H
#define GNS_WIRELESS_INFO_TYPEDEFS_H

typedef enum
{
    E_gnsWIRELESS_INFO_REQ  /*!< Mobile Info Updates Requested. Data defined by \ref e_gnsWIRELESS_MsgType */
}e_gnsWIRELESS_MsgType;





/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsWIRELESS_NwInfoValidity
 *  \brief  List of constant used to indicate validity of different wireless network info like WLAN, WiFi
 */
typedef enum{
    E_gnsWIRELESS_WLAN_FIELD = 0x100,/*WLAN Field*/
    E_gnsWIRELESS_WIMAX_FIELD = 0x200/*WIMAX Field*/

} e_gnsWIRELESS_NwInfoValidity;


/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsWIRELESS_AccessPointType
 *  \brief  List of constant used to indicate The type of Wireless network used like WLAN and WIMAX
 */
typedef enum{
    E_gnsWIRELESS_WLAN                = 0,        /*!<To indicate access point information present is that of WLAN. */
    E_gnsWIRELESS_WIMAX               = 1,        /*!< To indicate access point information present is that of WIMAX. */
    E_gnsWIRELESS_UNKNWON_TYPE        = 0xFF      /*!< Unknown Wireless network type */
} e_gnsWIRELESS_AccessPointType;


/*----------------------------------------------------------------------*/
/*!
 *  \enum   e_gnsWIRELESS_AccessPointStatus
 *  \brief  List of constant used to indicate The status of the Accesspoint connection
 */
typedef enum{
    E_gnsWIRELESS_ACCESSPOINT_STALE    = 0,        /*!<Access Point Information is Stale (Last Known Access Point Information)*/
    E_gnsWIRELESS_ACCESSPOINT_CURRENT  = 1,        /*!<Indicates the Access point information is of current serving access point */
    E_gnsWIRELESS_ACCESSPOINT_UNKNWON  = 0xFF      /*!< Indicates the Access point information validity is unknown */
} e_gnsWIRELESS_AccessPointStatus;


/*----------------------------------------------------------------------*/
/*! \struct s_gnsWIRELESS_WlanAPInfo
 *  \brief  Structure which defines the mobile information for WLAN Info.
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
} s_gnsWIRELESS_WlanAPInfo;


/*----------------------------------------------------------------------*/
/*! \union  u_gnsWIRELESS_APInfo
 *  \brief  Union which defines the Access Point Information.
 */
typedef union
{
    s_gnsWIRELESS_WlanAPInfo           v_WlanApInfo;              /*!< Access Point Information of the WLAN*/
    /*This structure can be extended for adding WIMAX and other wireless network Info*/
} u_gnsWIRELESS_APInfo;



/*! \struct  s_gnsWIRELESS_NetworkInfoReq
 *  \brief  Structure which defines the data associated with \ref E_gnsWIRELESS_INFO_REQ
 */
typedef struct
{
    uint32_t v_RequestedFields; /* \ref  e_gnsSUPL_MobileInfoValidity*/
} s_gnsWIRELESS_NetworkInfoReq;



/*! \union  u_gnsWIRELESS_MsgData
 *  \brief  Union which defines the data associated with \ref e_gnsWIRELESS_MsgType
 */
typedef union
{
    s_gnsWIRELESS_NetworkInfoReq        v_GnsWirelessNetWorkInfoReq; /*!< Corresponds to message type#E_gnsWIRELESS_INFO_REQ */
}u_gnsWIRELESS_MsgData;



/*----------------------------------------------------------------------*/
/*! \struct s_gnsWIRELESS_NetworkInfo
 *  \brief  Structure which defines mobile information.
 */
typedef struct
{
    e_gnsWIRELESS_AccessPointType       v_AccessPointType;          /*!< AccessPointType type (WLAN, WIMAX).  Defines which type of  wireless network is going to be used
                                                                                                                     for performing the SUPL session */
    e_gnsWIRELESS_AccessPointStatus     v_AccessPointStatus;        /*!< AccessPoint connection status  */
    u_gnsWIRELESS_APInfo                v_AccessPointInfo;          /*!< AccessPoint Information of WLAN, WIMAX  */
} s_gnsWIRELESS_NetworkInfo;


#endif /* GNS_WIRELESS_INFO_TYPEDEFS_H */


