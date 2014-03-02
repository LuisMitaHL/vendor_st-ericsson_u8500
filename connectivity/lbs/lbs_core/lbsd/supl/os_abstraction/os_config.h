//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename os_config.h
//
// $Header: X:/MKS Projects/prototype/prototype/os_abstraction/rcs/os_config.h 1.30 2009/01/13 14:09:39Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup os_abstraction
///
/// \file
/// \brief
///      Config retrieval declarations.
///
///      Various config options are required and can be obtained by using these
///      calls.
///
//*************************************************************************

#ifndef OS_CONFIG_H
#define OS_CONFIG_H

#include "gps_ptypes.h"
#include "GN_SUPL_api.h"
//#include "net_messages.h"

/// Filename of the configurations.
#define SUPL_CONFIG_FILE_NAME "SUPL_config.txt"
/// The CFG IP Address contains the data specific to the NET Adapter in messages.
typedef struct {
   e_TcpIp_AddressType TcpIp_AddressType; ///< Type of IP Address: IPV4, IPV6 or FQDN(Comms_Open_Request).
   char            Address[255 + 1];      ///< IP address.
   ///\todo replace magic numbers with definition for length of FQDN address.
   unsigned        Port;                  ///< Port number.
} s_CFG_IpAddress;

void  os_config_init( void );
void  os_config_deinit( void );

s_CFG_IpAddress *os_config_get_SUPL_server_address( void );
//void os_config_set_SUPL_server_address(   CH ip_address[16], U2 Port );

s_CFG_IpAddress *os_config_get_SUPL_push_address( void );
void  os_config_set_SUPL_push_address(          CH ip_address[16], U2 Port );

CH    *os_config_get_SUPL_server_fqdn(          void );
void  os_config_set_SUPL_server_fqdn(           CH *p_FQDN );

//void  os_config_set_SUPL_msisdn(                U1 *p_MSISDN );
//U1    *os_config_get_SUPL_msisdn(               void );
//
//void  os_config_set_SUPL_UT1(                   U2 User_Timer_1_Duration );
//U2    os_config_get_SUPL_UT1(                   void );
//void  os_config_set_SUPL_UT2(                   U2 User_Timer_2_Duration );
//U2    os_config_get_SUPL_UT2(                   void );
//void  os_config_set_SUPL_UT3(                   U2 User_Timer_3_Duration );
//U2    os_config_get_SUPL_UT3(                   void );

void  os_config_set_SUPL_version(               U1 Major,    U1 Minor,    U1 Service_Indicator );
void  os_config_get_SUPL_version(               U1 *p_Major, U1 *p_Minor, U1 *p_Service_Indicator );

void  os_config_get_SUPL_LocationId(            s_LocationId *p_LocationId );

void  os_config_set_SUPL_LocationIdType(        e_CellInfoType LocationIdType );
e_CellInfoType os_config_get_SUPL_LocationIdType( void );

void  os_config_set_SUPL_GSM_LocationId(        s_gsmCellInfo     *p_GSM_LocationId );
void  os_config_get_SUPL_GSM_LocationId(        s_gsmCellInfo     *p_GSM_LocationId );

void  os_config_set_SUPL_CDMA_LocationId(       s_cdmaCellInfo    *p_CDMA_LocationId );
void  os_config_get_SUPL_CDMA_LocationId(       s_cdmaCellInfo    *p_CDMA_LocationId );

void  os_config_set_SUPL_WCDMA_LocationId(      s_wcdmaCellInfo   *p_WCDMA_LocationId );
void  os_config_get_SUPL_WCDMA_LocationId(      s_wcdmaCellInfo   *p_WCDMA_LocationId );

void  os_config_set_SUPL_QOP_horacc(            U1 SUPL_QOP_horacc);
U1    *os_config_get_SUPL_QOP_horacc(           void );

void  os_config_set_SUPL_QOP_veracc(            I1 SUPL_QOP_veracc);
I1    *os_config_get_SUPL_QOP_veracc(           void );

void  os_config_set_SUPL_QOP_maxLocAge(         I4 SUPL_QOP_maxLocAge);
I4    *os_config_get_SUPL_QOP_maxLocAge(        void );

void  os_config_set_SUPL_QOP_delay(             I1 SUPL_QOP_delay);
I1    *os_config_get_SUPL_QOP_delay(            void );

void  os_config_set_SUPL_Notification_Reject(   BL SUPL_Notification_Reject);
BL    os_config_get_SUPL_Notification_Reject(   void );

void  os_config_set_RRLP_Msr_Response_Time(     U1 RRLP_Msr_Response_Time);
U1    os_config_get_RRLP_Msr_Response_Time(     void );

void  os_config_set_SUPL_Server_Key_Filename(   CH *p_Server_Key_Filename);
CH    *os_config_get_SUPL_Server_Key_Filename(  void );

void  os_config_set_SUPL_Server_Cert_Filename(  CH *p_Server_Cert_Filename);
CH    *os_config_get_SUPL_Server_Cert_Filename( void );

void  os_config_set_SUPL_CA_Cert_Filename(      CH *p_CA_Cert_Filename);
CH    *os_config_get_SUPL_CA_Cert_Filename(     void );

#endif // OS_CONFIG_H
