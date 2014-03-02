
//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_config.h
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/supl_config.h 1.18 2009/01/13 14:10:54Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_module
///
/// \file
/// \brief
///      SUPL configuration setting and retrieval declarations.
///
///      Various config options are required and can be obtained by using these
///      calls.
///
//*************************************************************************

#ifndef SUPL_CONFIG_H
#define SUPL_CONFIG_H

#include "GN_SUPL_api.h"
#include "gps_ptypes.h"
#include "supl_instance.h"

/// \brief Size in bytes for storing an MSISDN.
/// <pre>
////     msisdn      OCTET STRING(SIZE (8)),
/// </pre>
#define MSISDN_STORE_LEN   8

/// \brief Size in bytes for storing an MDN.
/// <pre>
////     mdn         OCTET STRING(SIZE (8)),
/// </pre>
#define MDN_STORE_LEN      8

/// \brief Size in bytes for storing an MIN.
/// <pre>
////     min         BIT STRING(SIZE (34)), -- coded according to TIA-553
/// </pre>
#define MIN_STORE_LEN      ( ( 34 / 8 ) + 1 )     // 34 bit coded according to TIA-553

/// \brief Size in bytes for storing an IMSI.
/// <pre>
////     imsi        OCTET STRING(SIZE (8)),
/// </pre>
#define IMSI_STORE_LEN     8

#define PLATFORM_VERSION_LEN    255

#define MAX_ESLP_ADDRESS_LEN    255

#define MAX_ESLP_ADDRESS         10

#define SUPL_UT_BUFFER_TIME     500

/// \brief Maximum allowable length for a FQDN.
/// <pre>
///   FQDN ::=
///      VisibleString(FROM ("a".."z" | "A".."Z" | "0".."9" | ".-"))(SIZE (1..255))
/// </pre>
#define FQDN_MAX           255

/// The CFG IP Address contains the data specific to the NET Adapter in messages.
typedef struct {
   e_TcpIp_AddressType  TcpIp_AddressType;      ///< Type of IP Address: IPV4, IPV6 or FQDN(Comms_Open_Request).
   CH                   Address[FQDN_MAX + 1];  ///< IP address.
   U2                   Port;                   ///< Port number.
} s_SUPL_IpAddress;

e_SetIdType supl_config_get_SUPL_SetIdType( void );

void supl_config_set_SUPL_msisdn( CH *p_MSISDN );
U1*  supl_config_get_SUPL_msisdn( void );

void supl_config_set_SUPL_mdn( CH *p_MDN );
U1*  supl_config_get_SUPL_mdn( void );

void supl_config_set_SUPL_imsi( CH *p_IMSI );
U1*  supl_config_get_SUPL_imsi( void );

void supl_config_set_SUPL_agpsSETassisted( BL agpsSETassisted );
BL   supl_config_get_SUPL_agpsSETassisted( void );

void supl_config_set_SUPL_agpsSETBased( BL agpsSETBased );
BL   supl_config_get_SUPL_agpsSETBased( void );

void supl_config_set_SUPL_autonomousGPS( BL autonomousGPS );
BL   supl_config_get_SUPL_autonomousGPS( void );

void supl_config_set_SUPL_eCID( BL eCID );
BL   supl_config_get_SUPL_eCID( void );

s_SUPL_IpAddress* supl_config_get_SUPL_SLP_address( void );
void              supl_config_set_SUPL_SLP_address( e_TcpIp_AddressType TcpIp_AddressType, CH *p_ip_address, U2 Port );//con33_3

s_SUPL_IpAddress* supl_config_get_SUPL_ESLP_address( void );
void              supl_config_set_SUPL_ESLP_address( e_TcpIp_AddressType TcpIp_AddressType, CH *p_ip_address, U2 Port );//con33_3

void supl_config_set_SUPL_UT1( U2 User_Timer_1_Duration );
U2   supl_config_get_SUPL_UT1( void );
void supl_config_set_SUPL_UT2( U2 User_Timer_2_Duration );
U2   supl_config_get_SUPL_UT2( void );
void supl_config_set_SUPL_UT3( U2 User_Timer_3_Duration );
U2   supl_config_get_SUPL_UT3( void );

void supl_config_set_SUPL_version( U1 Major, U1 Minor, U1 Service_Indicator );
void supl_config_get_SUPL_version( U1 *p_Major, U1 *p_Minor, U1 *p_Service_Indicator );
//
//void os_config_set_SUPL_GSM_LocationId( U2 MCC, U2 MNC, U2 LAC, U2 CI );
//void os_config_get_SUPL_GSM_LocationId( U2 *p_MCC, U2 *p_MNC, U2 *p_LAC, U2 *p_CI );
//
//void os_config_set_SUPL_QOP_horacc( U1 SUPL_QOP_horacc );
//U1*  os_config_get_SUPL_QOP_horacc( void );
//
//void os_config_set_SUPL_QOP_veracc( U1 SUPL_QOP_veracc );
//U1*  os_config_get_SUPL_QOP_veracc( void );
//
//void os_config_set_SUPL_QOP_maxLocAge( U2 SUPL_QOP_maxLocAge );
//U2*  os_config_get_SUPL_QOP_maxLocAge( void);
//
//void os_config_set_SUPL_QOP_delay( U1 SUPL_QOP_delay );
//U1*  os_config_get_SUPL_QOP_delay( void );
//
//void os_config_set_RRLP_Msr_Response_Time( U1 RRLP_Msr_Response_Time );
//U1   os_config_get_RRLP_Msr_Response_Time( void );
//
//void os_config_set_SUPL_Server_Key_Filename( CH *p_Server_Key_Filename );
//CH*  os_config_get_SUPL_Server_Key_Filename( void );
//
//void os_config_set_SUPL_Server_Cert_Filename( CH *p_Server_Cert_Filename );
//CH*  os_config_get_SUPL_Server_Cert_Filename( void );
//
//void os_config_set_SUPL_CA_Cert_Filename( CH *p_CA_Cert_Filename );
//CH*  os_config_get_SUPL_CA_Cert_Filename( void );

void supl_config_set_platform_version( CH* p_PlatformVersion );
CH* supl_config_get_platform_version();

void supl_config_set_Periodic_Trigger( BL Periodic_Trigger );
BL supl_config_get_Periodic_Trigger( void );

void supl_config_set_AreaEvent_Trigger( BL AreaEvent_Trigger );
BL supl_config_get_AreaEvent_Trigger( void );

void supl_config_set_Geographic_Area_Ellipse( BL Geographic_Area_Ellipse );
BL supl_config_get_Geographic_Area_Ellipse( void );

void supl_config_set_Geographic_Area_Polygon( BL Geographic_Area_Polygon );
BL supl_config_get_Geographic_Area_Polygon( void );

void supl_config_set_Max_Area_ID_Lists( U1 Max_Area_ID_Lists );
U1 supl_config_get_Max_Area_ID_Lists( void );

void supl_config_set_Max_Area_ID_Per_List( U1 Max_Area_ID_Per_List );
U1 supl_config_get_Max_Area_ID_Per_List( void );

void supl_config_set_Max_Num_Session( U1 Max_Num_Session );
U1 supl_config_get_Max_Num_Session( void );

void supl_config_set_Max_Geographic_Areas( U1 Max_Geographic_Areas );
U1 supl_config_get_Max_Geographic_Areas( void );

void supl_config_set_Reporting_Mode_Real_Time( BL Reporting_Mode_Real_Time );
BL supl_config_get_Reporting_Mode_Real_Time( void );

void supl_config_set_Reporting_Mode_Quasi_Real_Time( BL Reporting_Mode_Quasi_Real_Time );
BL supl_config_get_Reporting_Mode_Quasi_Real_Time( void );

void supl_config_set_Batch_Report_Position( BL Batch_Report_Position );
BL supl_config_get_Batch_Report_Position( void );

void supl_config_set_Reporting_Mode_Batch( BL Reporting_Mode_Batch );
BL supl_config_get_Reporting_Mode_Batch( void );

void supl_config_set_Batch_Report_Measurements( BL Batch_Report_Measurements );
BL supl_config_get_Batch_Report_Measurements( void );

void supl_config_set_Min_Report_Interval( U2 Min_Report_Interval );
U2 supl_config_get_Min_Report_Interval( void );

void supl_config_set_Max_Report_Interval( U2 Max_Report_Interval );
U2 supl_config_get_Max_Report_Interval( void );

void supl_config_set_Max_Positions_In_Batch( U1 Max_Positions_In_Batch );
U1 supl_config_get_Max_Positions_In_Batch( void );

void supl_config_set_Max_Measurement_In_Batch( U1 Max_Measurement_In_Batch );
U1 supl_config_get_Max_Measurement_In_Batch( void );
CH *supl_config_get_SUPL_eslp_whitelist_entry( U1 index );

void supl_config_set_SUPL_eslp_whitelist_entry( CH* s_ESlpAddr , U1 index );

void supl_config_set_SUPL_UT5(U2 User_Timer_5_Duration );
void supl_config_set_SUPL_UT6(U2 User_Timer_6_Duration );
void supl_config_set_SUPL_UT7(U2 User_Timer_7_Duration );
void supl_config_set_SUPL_UT8(U2 User_Timer_8_Duration );
void supl_config_set_SUPL_UT10(U2 User_Timer_10_Duration );

U2 supl_config_get_SUPL_UT5( void );
U2 supl_config_get_SUPL_UT7( void );
U2 supl_config_get_SUPL_UT8( void );
U2 supl_config_get_SUPL_UT9( void );

void supl_config_set_SUPL_UT9(U2 User_Timer_9_Duration);
#endif   // SUPL_CONFIG_H
