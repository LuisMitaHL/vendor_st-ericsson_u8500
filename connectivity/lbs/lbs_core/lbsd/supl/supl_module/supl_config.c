//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_config.c
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/supl_config.c 1.14 2009/01/13 14:10:53Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_module
///
/// \file
/// \brief
///      SUPL configuration setting and retrieval implementation.
///
///      Various config options are required and can be obtained by using these
///      calls.
///
//*************************************************************************

#include "supl_config.h"
#include "stdio.h"
#include "string.h"
#include "GN_SUPL_api.h"
#include "supl_instance.h"
#include "supl_log.h"

/// Macro definition to control logging of supl_config entries
#define SUPL_CFG_LOGGING

/// Default value of User Timer 1
#define USER_TIMER_1_DEFAULT 10000
/// Default value of User Timer 2
#define USER_TIMER_2_DEFAULT 10000
/// Default value of User Timer 3
#define USER_TIMER_3_DEFAULT 10000

/// Internal structure for holding the current setting of the SUPL Config entries.
static struct {
   e_SetIdType SetIdType;           ///< Type of SET Id configured.
   U1 MSISDN[MSISDN_STORE_LEN];     ///< Store for MSISDN for use in SET Id.
   U1 MDN[MDN_STORE_LEN];           ///< Store for MDN for use in SET Id.
   U1 IMSI[IMSI_STORE_LEN];         ///< Store for IMSI for use in SET Id.
   s_SUPL_IpAddress SLP_IP_Address; ///< IP address of SLP to connect to.
   s_SUPL_IpAddress ESLP_IP_Address; ///< IP address of SLP to connect to.

   U2 User_Timer_1_Duration;        ///< User timer 1 duration (ms).
   U2 User_Timer_2_Duration;        ///< User timer 2 duration (ms).
   U2 User_Timer_3_Duration;        ///< User timer 3 duration (ms).
   U2 User_Timer_5_Duration;        ///< User timer 5 duration (ms).
   U2 User_Timer_6_Duration;        ///< User timer 6 duration (ms).
   U2 User_Timer_7_Duration;        ///< User timer 7 duration (ms).
   U2 User_Timer_8_Duration;        ///< User timer 8 duration (ms).
   U2 User_Timer_9_Duration;        ///< User timer 9 duration (ms).
   U2 User_Timer_10_Duration;        ///< User timer 10 duration (ms).

   struct {
      U1 Major;                     ///< Major part of version.
      U1 Minor;                     ///< Minor part of version.
      U1 Service_Indicator;         ///< Service Indicator part of version.
   } Version;                       ///< Version number to use.
   BL agpsSETAssisted;              ///< Positioning Technology AGPS SET Assisted is supported if TRUE.
   BL agpsSETBased;                 ///< Positioning Technology AGPS SET Based is supported if TRUE.
   BL autonomousGPS;                ///< Positioning Technology Autonomous GPS is supported if TRUE.
   BL eCID;                         ///< Positioning Technology eCID is supported if TRUE.
   CH PlatformVersion[PLATFORM_VERSION_LEN]; ///< String representing platfrom version

   /* Beginning of SUPLv2.0 specific configuration */
   BL Periodic_Trigger;             ///< Periodic triggers are supported if TRUE
   BL Area_Event_Trigger;           ///< Area Event triggers are supported if TRUE
   BL Geographic_Area_Ellipse;      ///< Ellipse Shaped area for Area Event triggers are supported if TRUE
   BL Geographic_Area_Polygon;      ///< Polygon Shaped area for Area Event triggers are supported if TRUE
   U1 Max_Geographic_Areas;         ///< Maximum number of Geographic Areas for Area Event Triggers. Range [0..10] -1 not defined
   U1 Max_Area_ID_Lists;            ///< Maximum number of AreaID Lists for Area Event Triggers. Range [0..10] -1 not defined
   U1 Max_Area_ID_Per_List;         ///< Maximum number of AreaID Per AreaIDLists for Area Event Triggers. Range [0..10] -1 not defined
   U1 Max_Num_Session;              ///< Maximum SUPL sessions allowed. range 1..5.
   BL Reporting_Mode_Real_Time;
   BL Reporting_Mode_Quasi_Real_Time;
   BL Reporting_Mode_Batch;     
   U2 Min_Report_Interval;
   U2 Max_Report_Interval;
   BL Batch_Report_Position;
   BL Batch_Report_Measurements;
   U1 Max_Positions_In_Batch;
   U1 Max_Measurement_In_Batch;
   CH p_ESlp_WhiteList[MAX_ESLP_ADDRESS][MAX_ESLP_ADDRESS_LEN];      ///<List of ESLP Address which is provisioned max support is 10>
} SUPL_Config_Entries;

//*****************************************************************************
/// \brief
///      Function to SET ID type.
/// \returns
///      Type of the SET ID configured.
//*****************************************************************************
e_SetIdType supl_config_get_SUPL_SetIdType( void )
{
   return SUPL_Config_Entries.SetIdType;
}

//*****************************************************************************
/// \brief
///      Function to set the MSISDN to use as the SET ID.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_msisdn(
   CH *p_MSISDN   ///< Pointer to character string containing the MSISDN.
)
{
   // Example conversions.
   // "447748113053"
   // {0x44, 0x77, 0x84, 0x11, 0x03, 0x35, 0xff, 0xff}
   // "1234567890123456"
   // {0x21, 0x43, 0x65, 0x87, 0x09, 0x21, 0x43, 0x65}
   U1 i;

   U1 Msisdn_Length;
   U1 *dst;

   if ( p_MSISDN != NULL )
   {
      Msisdn_Length = (U1) strlen( p_MSISDN );
      dst = SUPL_Config_Entries.MSISDN;
      SUPL_Config_Entries.SetIdType = GN_SETId_PR_msisdn;
      for ( i = 0 ; i < Msisdn_Length ; i++ )
      {
         U1 dst_index = i / 2;
         BL low_nibble = ( i % 2 ) == 0; // If the index is divisible by 2, it goes into the low nibble.
         if ( low_nibble )
         {
            SUPL_Config_Entries.MSISDN[dst_index] = ( p_MSISDN[i] - '0' ) & 0x0f;
         }
         else
         {
            SUPL_Config_Entries.MSISDN[dst_index] |= ( ( ( p_MSISDN[i] - '0' ) << 4 ) & 0xf0 );
         }
      }

      for ( i = Msisdn_Length ; i < ( MSISDN_STORE_LEN * 2 ) ; i++ )
      {
         U1 dst_index = i / 2;
         BL low_nibble = ( i % 2 ) == 0; // If the index is divisible by 2, it goes into the low nibble.
         if ( low_nibble )
         {
            SUPL_Config_Entries.MSISDN[dst_index] = 0x0f;
         }
         else
         {
            SUPL_Config_Entries.MSISDN[dst_index] |= 0xf0;
         }
      }
   }
}

//*****************************************************************************
/// \brief
///      Function to get the MSISDN to use as the SET ID.
/// \returns
///      Pointer to the MSISDN.
//*****************************************************************************
U1 * supl_config_get_SUPL_msisdn( void )
{
   return SUPL_Config_Entries.MSISDN;
}

//*****************************************************************************
/// \brief
///      Function to set the MDN to use as the SET ID.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_mdn(
   CH *p_MDN      ///< Pointer to character string containing the MDN.
)
{
   if ( p_MDN != NULL )
   {
      memcpy( SUPL_Config_Entries.MDN, p_MDN, MDN_STORE_LEN );
      SUPL_Config_Entries.SetIdType = GN_SETId_PR_mdn;
   }
}

//*****************************************************************************
/// \brief
///      Function to get the MDN to use as the SET ID.
/// \returns
///      Pointer to the MDN.
//*****************************************************************************
U1 * supl_config_get_SUPL_mdn( void )
{
   return SUPL_Config_Entries.MDN;
}

//*****************************************************************************
/// \brief
///      Function to set the IMSI to use as the SET ID.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_imsi(
   CH *p_IMSI     ///< Pointer to character string containing the IMSI.
)
{
   // Example conversions.
   // "447748113053"
   // {0x44, 0x77, 0x84, 0x11, 0x03, 0x35, 0xff, 0xff}
   // "1234567890123456"
   // {0x21, 0x43, 0x65, 0x87, 0x09, 0x21, 0x43, 0x65}
   U1 i;

   U1 IMSI_Length;
   U1 *dst;

   if ( p_IMSI != NULL )
   {
      IMSI_Length = (U1) strlen( p_IMSI );
      dst = SUPL_Config_Entries.IMSI;

      SUPL_Config_Entries.SetIdType = GN_SETId_PR_imsi;

      for ( i = 0 ; i < IMSI_Length ; i++ )
      {
         U1 dst_index = i / 2;
         BL low_nibble = ( i % 2 ) == 0; // If the index is divisible by 2, it goes into the low nibble.
         if ( low_nibble )
         {
            SUPL_Config_Entries.IMSI[dst_index] = ( p_IMSI[i] - '0' ) & 0x0f;
         }
         else
         {
            SUPL_Config_Entries.IMSI[dst_index] |= ( ( ( p_IMSI[i] - '0' ) << 4 ) & 0xf0 );
         }
      }

      for ( i = IMSI_Length ; i < ( IMSI_STORE_LEN * 2 ) ; i++ )
      {
         U1 dst_index = i / 2;
         BL low_nibble = ( i % 2 ) == 0; // If the index is divisible by 2, it goes into the low nibble.
         if ( low_nibble )
         {
            SUPL_Config_Entries.IMSI[dst_index] = 0x0f;
         }
         else
         {
            SUPL_Config_Entries.IMSI[dst_index] |= 0xf0;
         }
      }
   }
}

//*****************************************************************************
/// \brief
///      Function to get the IMSI to use as the SET ID.
/// \returns
///      Pointer to the IMSI.
//*****************************************************************************
U1 * supl_config_get_SUPL_imsi( void )
{
   return SUPL_Config_Entries.IMSI;
}

//*****************************************************************************
/// \brief
///      Function to get the address of the supl server to connect to.
/// \returns
///      pointer to a string containing the supl server address.
//*****************************************************************************
s_SUPL_IpAddress *supl_config_get_SUPL_SLP_address( void )
{
   #ifdef SUPL_CFG_LOGGING
      GN_SUPL_Log( "GN_SUPL_Config_Set: Retrieving SLP IPAddress == %s, Port == %u",
         SUPL_Config_Entries.SLP_IP_Address.Address,
         SUPL_Config_Entries.SLP_IP_Address.Port );
   #endif

   return &SUPL_Config_Entries.SLP_IP_Address;
}

//*****************************************************************************
/// \brief
///      Function to set the address of the supl server to connect to.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_SLP_address(
   e_TcpIp_AddressType TcpIp_AddressType, ///< Type of IP Address: IPV4, IPV6 or FQDN(Comms_Open_Request).
   CH *p_ip_address,                      ///< IP address of server.
   U2 Port                                ///< Port number.
)
{
   SUPL_Config_Entries.SLP_IP_Address.TcpIp_AddressType = TcpIp_AddressType;
   if ( TcpIp_AddressType != IP_None )
   {
      strcpy( SUPL_Config_Entries.SLP_IP_Address.Address, p_ip_address );
      SUPL_Config_Entries.SLP_IP_Address.Port = Port; 
      #ifdef SUPL_CFG_LOGGING
         GN_SUPL_Log( "GN_SUPL_Config_Set: Set SLP IPAddress == %s, Port == %u",
            SUPL_Config_Entries.SLP_IP_Address.Address,
            SUPL_Config_Entries.SLP_IP_Address.Port );
      #endif
   }
   else
   {
      SUPL_Config_Entries.SLP_IP_Address.Address[0] = '\0';
      SUPL_Config_Entries.SLP_IP_Address.Port = 0;
   }
}

//*****************************************************************************
/// \brief
///      Function to get the address of the Eslp supl server to connect to.
/// \returns
///      pointer to a string containing the supl server address.
//*****************************************************************************
s_SUPL_IpAddress *supl_config_get_SUPL_ESLP_address( void )
{
   #ifdef SUPL_CFG_LOGGING
      GN_SUPL_Log( "GN_SUPL_Config_Set: Retrieving ESLP IPAddress == %s, Port == %u",
         SUPL_Config_Entries.ESLP_IP_Address.Address,
         SUPL_Config_Entries.ESLP_IP_Address.Port );
   #endif

   return &SUPL_Config_Entries.ESLP_IP_Address;
}

//*****************************************************************************
/// \brief
///      Function to set the address of the Eslp supl server to connect to.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_ESLP_address(
   e_TcpIp_AddressType TcpIp_AddressType, ///< Type of IP Address: IPV4, IPV6 or FQDN(Comms_Open_Request).
   CH *p_ip_address,                      ///< IP address of server.
   U2 Port                                ///< Port number.
)
{
   SUPL_Config_Entries.ESLP_IP_Address.TcpIp_AddressType = TcpIp_AddressType;
   if ( TcpIp_AddressType != IP_None )
   {
      strcpy( SUPL_Config_Entries.ESLP_IP_Address.Address, p_ip_address );
      SUPL_Config_Entries.ESLP_IP_Address.Port = Port; 
      #ifdef SUPL_CFG_LOGGING
         GN_SUPL_Log( "GN_SUPL_Config_Set: Set ESLP IPAddress == %s, Port == %u",
            SUPL_Config_Entries.ESLP_IP_Address.Address,
            SUPL_Config_Entries.ESLP_IP_Address.Port );
      #endif
   }
   else
   {
      SUPL_Config_Entries.ESLP_IP_Address.Address[0] = '\0';
      SUPL_Config_Entries.ESLP_IP_Address.Port = 0;
   }
}


//*****************************************************************************
/// \brief
///      Function to set the configured value of User Timer 1 in milliseconds.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_UT1(
   U2 User_Timer_1_Duration   ///< Value of User Timer 1 in milliseconds.
)
{
   SUPL_Config_Entries.User_Timer_1_Duration = User_Timer_1_Duration + SUPL_UT_BUFFER_TIME ;
}

//*****************************************************************************
/// \brief
///      Function to get the configured value of User Timer 1 in milliseconds.
/// \returns
///      Value of UT1 in milliseconds.
//*****************************************************************************
U2 supl_config_get_SUPL_UT1( void )
{
   return SUPL_Config_Entries.User_Timer_1_Duration;
}

//*****************************************************************************
/// \brief
///      Function to set the configured value of User Timer 2 in milliseconds.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_UT2(
   U2 User_Timer_2_Duration      ///< Value of User Timer 2 in milliseconds.
)
{
   SUPL_Config_Entries.User_Timer_2_Duration = User_Timer_2_Duration + SUPL_UT_BUFFER_TIME ;
}

//*****************************************************************************
/// \brief
///      Function to get the configured value of User Timer 2 in milliseconds.
/// \returns
///      Value of UT2 in milliseconds.
//*****************************************************************************
U2 supl_config_get_SUPL_UT2( void )
{
   return SUPL_Config_Entries.User_Timer_2_Duration;
}

//*****************************************************************************
/// \brief
///      Function to set the configured value of User Timer 3 in milliseconds.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_UT3(
   U2 User_Timer_3_Duration      ///< Value of User Timer 3 in milliseconds.
)
{
   SUPL_Config_Entries.User_Timer_3_Duration = User_Timer_3_Duration + SUPL_UT_BUFFER_TIME ;
}

//*****************************************************************************
/// \brief
///      Function to get the configured value of User Timer 3 in milliseconds.
/// \returns
///      Value of UT3 in milliseconds.
//*****************************************************************************
U2 supl_config_get_SUPL_UT3( void )
{
   return SUPL_Config_Entries.User_Timer_3_Duration;
}

//*****************************************************************************
/// \brief
///      Function to set the configured value of User Timer 5 in milliseconds.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_UT5(
   U2 User_Timer_5_Duration      ///< Value of User Timer 5 in milliseconds.
)
{
   SUPL_Config_Entries.User_Timer_5_Duration = User_Timer_5_Duration + SUPL_UT_BUFFER_TIME ;
}

//*****************************************************************************
/// \brief
///      Function to get the configured value of User Timer 5 in milliseconds.
/// \returns
///      Value of UT5 in milliseconds.
//*****************************************************************************
U2 supl_config_get_SUPL_UT5( void )
{
   return SUPL_Config_Entries.User_Timer_5_Duration;
}

//*****************************************************************************
/// \brief
///      Function to set the configured value of User Timer 6 in milliseconds.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_UT6(
   U2 User_Timer_6_Duration      ///< Value of User Timer 6 in milliseconds.
)
{
   SUPL_Config_Entries.User_Timer_6_Duration = User_Timer_6_Duration + SUPL_UT_BUFFER_TIME ;
}

//*****************************************************************************
/// \brief
///      Function to get the configured value of User Timer 6 in milliseconds.
/// \returns
///      Value of UT6 in milliseconds.
//*****************************************************************************
U2 supl_config_get_SUPL_UT6( void )
{
   return SUPL_Config_Entries.User_Timer_6_Duration;
}

//*****************************************************************************
/// \brief
///      Function to set the configured value of User Timer 7 in milliseconds.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_UT7(
   U2 User_Timer_7_Duration      ///< Value of User Timer 7 in milliseconds.
)
{
   SUPL_Config_Entries.User_Timer_7_Duration = User_Timer_7_Duration + SUPL_UT_BUFFER_TIME ;
}

//*****************************************************************************
/// \brief
///      Function to get the configured value of User Timer 7 in milliseconds.
/// \returns
///      Value of UT7 in milliseconds.
//*****************************************************************************
U2 supl_config_get_SUPL_UT7( void )
{
   return SUPL_Config_Entries.User_Timer_7_Duration;
}

//*****************************************************************************
/// \brief
///      Function to set the configured value of User Timer 8 in milliseconds.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_UT8(
   U2 User_Timer_8_Duration      ///< Value of User Timer 8 in milliseconds.
)
{
   SUPL_Config_Entries.User_Timer_8_Duration = User_Timer_8_Duration + SUPL_UT_BUFFER_TIME ;
}

//*****************************************************************************
/// \brief
///      Function to get the configured value of User Timer 8 in milliseconds.
/// \returns
///      Value of UT8 in milliseconds.
//*****************************************************************************
U2 supl_config_get_SUPL_UT8( void )
{
   return SUPL_Config_Entries.User_Timer_8_Duration;
}

//*****************************************************************************
/// \brief
///      Function to set the configured value of User Timer 60 secs.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_UT9(
   U2 User_Timer_9_Duration      ///< Value of User Timer 3 in milliseconds.
)
{
   SUPL_Config_Entries.User_Timer_9_Duration = User_Timer_9_Duration + SUPL_UT_BUFFER_TIME ;
}

//*****************************************************************************
/// \brief
///      Function to get the configured value of User Timer 9 in milliseconds.
/// \returns
///      Value of UT3 in milliseconds.
//*****************************************************************************
U2 supl_config_get_SUPL_UT9( void )
{
   return SUPL_Config_Entries.User_Timer_9_Duration;
}

//*****************************************************************************
/// \brief
///      Function to set the configured value of User Timer 10 in milliseconds.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_UT10(
   U2 User_Timer_10_Duration   ///< Value of User Timer 10 in milliseconds.
)
{
   SUPL_Config_Entries.User_Timer_10_Duration = User_Timer_10_Duration + SUPL_UT_BUFFER_TIME ;
}

//*****************************************************************************
/// \brief
///      Function to get the configured value of User Timer 10 in milliseconds.
/// \returns
///      Value of UT10 in milliseconds.
//*****************************************************************************
U2 supl_config_get_SUPL_UT10( void )
{
   return SUPL_Config_Entries.User_Timer_10_Duration;
}

//*****************************************************************************
/// \brief
///      Function to set a flag indicating whether AGPS SET assisted
///      positioning is supported.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_agpsSETassisted( BL agpsSETAssisted )
{
   /// Positioning Technology agpsSETassisted is supported if TRUE.
   SUPL_Config_Entries.agpsSETAssisted = agpsSETAssisted ;
}

//*****************************************************************************
/// \brief
///      Function to retrieve a flag to indicate if AGPS SET assisted
///      positioning is supported.
/// \returns
///      Flag to indicate whether AGPS SET Assisted positioning is supported.
/// \retval
///      #TRUE if AGPS SET assisted positioning is supported.
///      #FALSE if AGPS SET assisted positioning is not supported.
//*****************************************************************************
BL supl_config_get_SUPL_agpsSETassisted( void )
{
   return SUPL_Config_Entries.agpsSETAssisted ;
}

//*****************************************************************************
/// \brief
///      Function to set a flag indicatin whether AGPS SET based
///      positioning is supported.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_agpsSETBased( BL agpsSETBased )
{
   /// Positioning Technology agpsSETBased is supported if TRUE.
   SUPL_Config_Entries.agpsSETBased = agpsSETBased ;
}

//*****************************************************************************
/// \brief
///      Function to retrieve a flag to indicate if AGPS SET based
///      positioning is supported.
/// \returns
///      Flag to indicate whether AGPS SET based positioning is supported.
/// \retval
///      #TRUE if AGPS SET based positioning is supported.
///      #FALSE if AGPS SET based positioning is not supported.
//*****************************************************************************
BL supl_config_get_SUPL_agpsSETBased( void )
{
   return SUPL_Config_Entries.agpsSETBased ;
}

//*****************************************************************************
/// \brief
///      Function to set a flag indicatin whether Autonomous GPS
///      positioning is supported.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_autonomousGPS( BL autonomousGPS )
{
   /// Positioning Technology autonomousGPS is supported if TRUE.
   SUPL_Config_Entries.autonomousGPS = autonomousGPS ;
}

//*****************************************************************************
/// \brief
///      Function to retrieve a flag to indicate if Autonomous GPS
///      positioning is supported.
/// \returns
///      Flag to indicate whether Autonomous GPS positioning is supported.
/// \retval
///      #TRUE if Autonomous GPS positioning is supported.
///      #FALSE if Autonomous GPS positioning is not supported.
//*****************************************************************************
BL supl_config_get_SUPL_autonomousGPS( void )
{
   return SUPL_Config_Entries.autonomousGPS ; 
}

//*****************************************************************************
/// \brief
///      Function to set a flag indicatin whether Enhanced Cell ID
///      positioning is supported.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_config_set_SUPL_eCID( BL eCID )
{
   /// Positioning Technology eCID is supported if TRUE.
   SUPL_Config_Entries.eCID = eCID ;
}

//*****************************************************************************
/// \brief
///      Function to retrieve a flag to indicate if Enhanced Cell ID
///      positioning is supported.
/// \returns
///      Flag to indicate whether Enhanced Cell ID positioning is supported.
/// \retval
///      #TRUE if Enhanced Cell ID positioning is supported.
///      #FALSE if Enhanced Cell ID positioning is not supported.
//*****************************************************************************
BL supl_config_get_SUPL_eCID( void )
{
   return SUPL_Config_Entries.eCID ;
}

//*****************************************************************************
/// \brief
///      Function to set the current version of the SUPL spec supported.
//*****************************************************************************
void supl_config_set_SUPL_version(
   U1 Major,                ///< [in] Major version number of SUPL Spec.
   U1 Minor,                ///< [in] Minor version number.
   U1 Service_Indicator     ///< [in] Service indicator.
)
{
   SUPL_Config_Entries.Version.Major = Major;
   SUPL_Config_Entries.Version.Minor = Minor;
   SUPL_Config_Entries.Version.Service_Indicator = Service_Indicator;
}


//*****************************************************************************
/// \brief
///      Function to get the current version of the SUPL spec supported.
//*****************************************************************************
void supl_config_get_SUPL_version(
   U1 *p_Major,                ///< [out] Major version number of SUPL Spec.
   U1 *p_Minor,                ///< [out] Minor version number.
   U1 *p_Service_Indicator     ///< [out] Service indicator.
)
{
   if( NULL != p_Major ) *p_Major = SUPL_Config_Entries.Version.Major;
   if( NULL != p_Minor ) *p_Minor = SUPL_Config_Entries.Version.Minor;
   if( NULL != p_Service_Indicator ) *p_Service_Indicator = SUPL_Config_Entries.Version.Service_Indicator;
}

void supl_config_set_platform_version( CH* p_PlatformVersion )
{
    if( NULL != p_PlatformVersion )
    {
        int len = strlen( p_PlatformVersion );

        if( len < PLATFORM_VERSION_LEN )
        {
            memcpy( SUPL_Config_Entries.PlatformVersion , p_PlatformVersion , len + 1 );
        }    
        else
        {
            memcpy( SUPL_Config_Entries.PlatformVersion , p_PlatformVersion , PLATFORM_VERSION_LEN - 2 );
            SUPL_Config_Entries.PlatformVersion[ PLATFORM_VERSION_LEN - 1 ] = '\0';  
        }
    }
}

CH* supl_config_get_platform_version()
{
    return SUPL_Config_Entries.PlatformVersion;
}

/*

BL Periodic_Trigger;             ///< Periodic triggers are supported if TRUE
   BL Area_Event_Trigger;           ///< Area Event triggers are supported if TRUE
   BL Geographic_Area_Ellipse;      ///< Ellipse Shaped area for Area Event triggers are supported if TRUE
   BL Geographic_Area_Polygon;      ///< Polygon Shaped area for Area Event triggers are supported if TRUE
   U1 Max_Geographic_Areas;         ///< Maximum number of Geographic Areas for Area Event Triggers. Range [0..10] -1 not defined
   U1 Max_Area_ID_Lists;            ///< Maximum number of AreaID Lists for Area Event Triggers. Range [0..10] -1 not defined
   U1 Max_Area_ID_Per_List;         ///< Maximum number of AreaID Per AreaIDLists for Area Event Triggers. Range [0..10] -1 not defined
   U1 Max_Num_Session;              ///< Maximum SUPL sessions allowed. range 1..5.

   */

//*****************************************************************************
/// \brief
///      Function to set the support for Periodic Trigger
//*****************************************************************************
void supl_config_set_Periodic_Trigger( BL Periodic_Trigger )
{
   SUPL_Config_Entries.Periodic_Trigger = Periodic_Trigger;
}


//*****************************************************************************
/// \brief
///      Function to get the support for Periodic Trigger
//*****************************************************************************
BL supl_config_get_Periodic_Trigger( void )
{
   return SUPL_Config_Entries.Periodic_Trigger;
}

//*****************************************************************************
/// \brief
///      Function to set the support for AreaEvent_Trigger
//*****************************************************************************
void supl_config_set_AreaEvent_Trigger( BL AreaEvent_Trigger )
{
   SUPL_Config_Entries.Area_Event_Trigger = AreaEvent_Trigger;
}


//*****************************************************************************
/// \brief
///      Function to get the support for AreaEvent_Trigger
//*****************************************************************************
BL supl_config_get_AreaEvent_Trigger( void )
{
   return SUPL_Config_Entries.Area_Event_Trigger;
}

//*****************************************************************************
/// \brief
///      Function to set the support for Geographic_Area_Ellipse
//*****************************************************************************
void supl_config_set_Geographic_Area_Ellipse( BL Geographic_Area_Ellipse )
{
   SUPL_Config_Entries.Geographic_Area_Ellipse = Geographic_Area_Ellipse;
}

//*****************************************************************************
/// \brief
///      Function to get the support for Geographic_Area_Ellipse
//*****************************************************************************
BL supl_config_get_Geographic_Area_Ellipse( void )
{
   return SUPL_Config_Entries.Geographic_Area_Ellipse;
}

//*****************************************************************************
/// \brief
///      Function to set the support for Geographic_Area_Polygon
//*****************************************************************************
void supl_config_set_Geographic_Area_Polygon( BL Geographic_Area_Polygon )
{
   SUPL_Config_Entries.Geographic_Area_Polygon = Geographic_Area_Polygon;
}

//*****************************************************************************
/// \brief
///      Function to get the support for Geographic_Area_Polygon
//*****************************************************************************
BL supl_config_get_Geographic_Area_Polygon( void )
{
   return SUPL_Config_Entries.Geographic_Area_Polygon;
}

//*****************************************************************************
/// \brief
///      Function to set the maximum number of Area ID lists supported.
//*****************************************************************************
void supl_config_set_Max_Area_ID_Lists( U1 Max_Area_ID_Lists )
{
   SUPL_Config_Entries.Max_Area_ID_Lists = Max_Area_ID_Lists;
}


//*****************************************************************************
/// \brief
///      Function to get the  maximum number of Area ID lists supported.
//*****************************************************************************
U1 supl_config_get_Max_Area_ID_Lists( void )
{
   return SUPL_Config_Entries.Max_Area_ID_Lists;
}

//*****************************************************************************
/// \brief
///      Function to set the maximum number of Area IDs per Area ID List supported.
//*****************************************************************************
void supl_config_set_Max_Area_ID_Per_List( U1 Max_Area_ID_Per_List )
{
   SUPL_Config_Entries.Max_Area_ID_Per_List= Max_Area_ID_Per_List;
}


//*****************************************************************************
/// \brief
///      Function to get the  maximum number of Area IDs per Area ID List supported.
//*****************************************************************************
U1 supl_config_get_Max_Area_ID_Per_List( void )
{
   return SUPL_Config_Entries.Max_Area_ID_Per_List;
}


//*****************************************************************************
/// \brief
///      Function to set the maximum number of SUPL sesisons allowed
//*****************************************************************************
void supl_config_set_Max_Num_Session( U1 Max_Num_Session )
{
   SUPL_Config_Entries.Max_Num_Session = Max_Num_Session;
}


//*****************************************************************************
/// \brief
///      Function to get the  maximum number of SUPL sessions allowed
//*****************************************************************************
U1 supl_config_get_Max_Num_Session( void )
{
   return SUPL_Config_Entries.Max_Num_Session;
}


//*****************************************************************************
/// \brief
///      Function to set the maximum number of Geographic areas supported.
//*****************************************************************************
void supl_config_set_Max_Geographic_Areas( U1 Max_Geographic_Areas )
{
   SUPL_Config_Entries.Max_Geographic_Areas = Max_Geographic_Areas;
}


//*****************************************************************************
/// \brief
///      Function to get the  maximum number of Geographic areas supported.
//*****************************************************************************
U1 supl_config_get_Max_Geographic_Areas( void )
{
   return SUPL_Config_Entries.Max_Geographic_Areas;
}


//*****************************************************************************
/// \brief
///      Function to set whether Real Time Mode is supported
//*****************************************************************************
void supl_config_set_Reporting_Mode_Real_Time( BL Reporting_Mode_Real_Time )
{
   SUPL_Config_Entries.Reporting_Mode_Real_Time = Reporting_Mode_Real_Time;
}


//*****************************************************************************
/// \brief
///      Function to get whether Real Time Mode is supported
//*****************************************************************************
BL supl_config_get_Reporting_Mode_Real_Time( void )
{
   return SUPL_Config_Entries.Reporting_Mode_Real_Time;
}

//*****************************************************************************
/// \brief
///      Function to set whether Quasi Real Time Mode is supported
//*****************************************************************************
void supl_config_set_Reporting_Mode_Quasi_Real_Time( BL Reporting_Mode_Quasi_Real_Time )
{
   SUPL_Config_Entries.Reporting_Mode_Quasi_Real_Time = Reporting_Mode_Quasi_Real_Time;
}


//*****************************************************************************
/// \brief
///      Function to get whether Quasi Real Time Mode is supported
//*****************************************************************************
BL supl_config_get_Reporting_Mode_Quasi_Real_Time( void )
{
   return SUPL_Config_Entries.Reporting_Mode_Quasi_Real_Time;
}

//*****************************************************************************
/// \brief
///      Function to set whether Position can be reported as part of Batch Reporting
//*****************************************************************************
void supl_config_set_Batch_Report_Position( BL Batch_Report_Position )
{
   SUPL_Config_Entries.Batch_Report_Position = Batch_Report_Position;
}


//*****************************************************************************
/// \brief
///      Function to get whether Position can be reported as part of Batch Reporting
//*****************************************************************************
BL supl_config_get_Batch_Report_Position( void )
{
   return SUPL_Config_Entries.Batch_Report_Position;
}

//*****************************************************************************
/// \brief
///      Function to set whether Batch reporting is supported
//*****************************************************************************
void supl_config_set_Reporting_Mode_Batch( BL Reporting_Mode_Batch )
{
   SUPL_Config_Entries.Reporting_Mode_Batch = Reporting_Mode_Batch;
}


//*****************************************************************************
/// \brief
///      Function to get whether Batch reporting is supported
//*****************************************************************************
BL supl_config_get_Reporting_Mode_Batch( void )
{
   return SUPL_Config_Entries.Reporting_Mode_Batch;
}

//*****************************************************************************
/// \brief
///      Function to set whether Measurements can be reported as part of Batch Reporting
//*****************************************************************************
void supl_config_set_Batch_Report_Measurements( BL Batch_Report_Measurements )
{
   SUPL_Config_Entries.Batch_Report_Measurements = Batch_Report_Measurements;
}


//*****************************************************************************
/// \brief
///      Function to get whether Measurements can be reported as part of Batch Reporting
//*****************************************************************************
BL supl_config_get_Batch_Report_Measurements( void )
{
   return SUPL_Config_Entries.Batch_Report_Measurements;
}

//*****************************************************************************
/// \brief
///      Function to set the minimum interval between SUPL reports to server
//*****************************************************************************
void supl_config_set_Min_Report_Interval( U2 Min_Report_Interval )
{
   SUPL_Config_Entries.Min_Report_Interval = Min_Report_Interval;
}


//*****************************************************************************
/// \brief
///      Function to get the minimum interval between SUPL reports to server
//*****************************************************************************
U2 supl_config_get_Min_Report_Interval( void )
{
   return SUPL_Config_Entries.Min_Report_Interval;
}



//*****************************************************************************
/// \brief
///      Function to set the maximum interval between SUPL reports to server
//*****************************************************************************
void supl_config_set_Max_Report_Interval( U2 Max_Report_Interval )
{
   SUPL_Config_Entries.Max_Report_Interval = Max_Report_Interval;
}


//*****************************************************************************
/// \brief
///      Function to get the maximum interval between SUPL reports to server
//*****************************************************************************
U2 supl_config_get_Max_Report_Interval( void )
{
   return SUPL_Config_Entries.Max_Report_Interval;
}


//*****************************************************************************
/// \brief
///      Function to set the maximum allowed positions in a batch of reports
//*****************************************************************************
void supl_config_set_Max_Positions_In_Batch( U1 Max_Positions_In_Batch )
{
   SUPL_Config_Entries.Max_Positions_In_Batch = Max_Positions_In_Batch;
}


//*****************************************************************************
/// \brief
///      Function to get the maximum allowed positions in a batch of reports
//*****************************************************************************
U1 supl_config_get_Max_Positions_In_Batch( void )
{
   return SUPL_Config_Entries.Max_Positions_In_Batch;
}

//*****************************************************************************
/// \brief
///      Function to set the maximum allowed measurements in a batch of reports
//*****************************************************************************
void supl_config_set_Max_Measurement_In_Batch( U1 Max_Measurement_In_Batch )
{
   SUPL_Config_Entries.Max_Measurement_In_Batch = Max_Measurement_In_Batch;
}


//*****************************************************************************
/// \brief
///      Function to get themaximum allowed measurements in a batch of reports
//*****************************************************************************
U1 supl_config_get_Max_Measurement_In_Batch( void )
{
   return SUPL_Config_Entries.Max_Measurement_In_Batch;
}

//*****************************************************************************
/// \brief
///      Function to set the address of E-SLP server to connect to.
//*****************************************************************************
void supl_config_set_SUPL_eslp_whitelist_entry( CH* s_ESlpAddr , U1 index )
{
   if( s_ESlpAddr != NULL )
   {
   
      int len = strlen( s_ESlpAddr );
   
      memcpy( SUPL_Config_Entries.p_ESlp_WhiteList[index] , s_ESlpAddr , len + 1 );
   
   }
}

//*****************************************************************************
/// \brief
///      Function to get the address of E-SLP server to connect to.
/// \returns
///      pointer to a string containing the E-SLP server address.
//*****************************************************************************
CH *supl_config_get_SUPL_eslp_whitelist_entry( U1 index )
{
   return SUPL_Config_Entries.p_ESlp_WhiteList[index] ;
}



