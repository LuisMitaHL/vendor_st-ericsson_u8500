//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename os_config.c
//
// $Header: X:/MKS Projects/prototype/prototype/os_abstraction/rcs/os_config.c 1.59 2009/01/19 12:32:28Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup os_abstraction
///
/// \file
/// \brief
///     Config retrieval implementation.
///
/// Various config options are required and can be obtained by using these
/// calls.
///
//*************************************************************************

#include "os_config.h"
#include "stdio.h"
#include "string.h"
#include "GN_SUPL_api.h"
#include "os_assert.h"

BL os_Copy_wcdmaCellInfo( s_wcdmaCellInfo *p_WCDMA_dst, s_wcdmaCellInfo *p_WCDMA_src );

/// Maximum line length allowed for in SUPL Config file.
#define MAX_LINE_LENGTH 512
/// Storage allocation for MSISDN.
#define MSISDN_LEN 17
/// Storage allocation for IMSI.
#define IMSI_LEN 17
/// Storage allocation for FQDN.
#define FQDN_MAX  257
/// Default timer value for User Timer 1 in seconds.
#define USER_TIMER_1_DEFAULT 10
/// Default timer value for User Timer 2 in seconds.
#define USER_TIMER_2_DEFAULT 10
/// Default timer value for User Timer 3 in seconds.
#define USER_TIMER_3_DEFAULT 10

/// Structure to contain configuration options.
static struct {
   struct {
      s_CFG_IpAddress SLP_IP_Address;  // = {"127.0.0.1", 7275};
      s_CFG_IpAddress Push_IP_Address;  // = {"127.0.0.1", 7277};
      U2 User_Timer_1_Duration;
      U2 User_Timer_2_Duration;
      U2 User_Timer_3_Duration;
      CH *p_SLP_FQDN;
      U1 *p_MSISDN;  // = {0x44,0x77,0x84,0x11,0x03,0x35,0xff,0xff};
      U1 *p_IMSI;      // = {};
      struct {
         U1 Major;
         U1 Minor;
         U1 Service_Indicator;
      } Version;
      s_LocationId LocationId;
      struct {
         U1 horacc;
         I1 veracc;
         I4 maxLocAge;
         I1 delay;
         U1 *p_horacc;
         I1 *p_veracc;
         I4 *p_maxLocAge;
         I1 *p_delay;
      } QOP;
      BL Notification_Reject;
      CH *p_Server_Key_Filename;
      CH *p_Server_Cert_Filename;
      CH *p_CA_Cert_Filename;
   } SUPL;
   struct {
      U1 MsrResponseTime;
   } RRLP;
} OS_Config_Entries;


//*****************************************************************************
/// \brief
///      Function to read config file.
/// \returns
///      Count of number of lines read.
//*****************************************************************************
int supl_config_read( char *p_file_name, char * p_buffer, int buffer_size, char *p_lines[], int line_size )
{
   errno_t status;
   FILE *config_file;

   int line_count = 0;   
   char line_terminator[] = {0x0d, 0x0a, 0};
   char * line_end;

   status = fopen_s( &config_file, p_file_name, "r" );

   if ( status != 0 )
   {
      return line_count;
   }
   fread( p_buffer, 1, buffer_size, config_file );

   fclose( config_file );

   line_end = p_buffer;
   p_lines[line_count] = line_end;
   for (;;) {
      line_end = strpbrk( line_end, line_terminator );
      if ( line_end == NULL || line_end > p_buffer + buffer_size )
         break; // past end of buffer, jump out of loop
      *line_end = '\0';
      line_end++;
      p_lines[++line_count] = line_end;
      if ( line_count >= line_size )
         break; // too many lines.
   }
   return line_count;
}

//*****************************************************************************
/// \brief
///      Function to get the msisdn from the config.
/// \returns
///      Pointer to msisdn.
//*****************************************************************************
U1 * os_config_get_SUPL_msisdn( void )
{
   return OS_Config_Entries.SUPL.p_MSISDN;
}

//*****************************************************************************
/// \brief
///      Function to set the msisdn in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_msisdn( U1 *p_MSISDN )
{
   // Example conversions.
   // "447748113053"
   // {0x44, 0x77, 0x84, 0x11, 0x03, 0x35, 0xff, 0xff}
   // "1234567890123456"
   // {0x21, 0x43, 0x65, 0x87, 0x09, 0x21, 0x43, 0x65}

//   U1 Msisdn_Length = (U1) strlen( p_MSISDN );
   OS_Config_Entries.SUPL.p_MSISDN = GN_Calloc( 1, MSISDN_LEN );
   strncpy( OS_Config_Entries.SUPL.p_MSISDN, p_MSISDN, MSISDN_LEN );

}

//*****************************************************************************
/// \brief
///      Function to parse the msisdn in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
os_config_parse_SUPL_msisdn( CH *p_MSISDN )
{
   if ( p_MSISDN != NULL )
   {
      os_config_set_SUPL_msisdn( p_MSISDN );
   }
   else
   {
      printf( "Error parsing SUPL_msisdn.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to get the imsi from the config.
/// \returns
///      Pointer to imsi.
//*****************************************************************************
U1 * os_config_get_SUPL_imsi( void )
{
   return OS_Config_Entries.SUPL.p_IMSI;
}

//*****************************************************************************
/// \brief
///      Function to set the imsi in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_imsi( U1 *p_IMSI )
{
   // Example conversions.
   // "447748113053"
   // {0x44, 0x77, 0x84, 0x11, 0x03, 0x35, 0xff, 0xff}
   // "1234567890123456"
   // {0x21, 0x43, 0x65, 0x87, 0x09, 0x21, 0x43, 0x65}

//   U1 Msisdn_Length = (U1) strlen( p_MSISDN );

   OS_Config_Entries.SUPL.p_IMSI = GN_Calloc( 1, IMSI_LEN );
   strncpy( OS_Config_Entries.SUPL.p_IMSI, p_IMSI, IMSI_LEN );

}

//*****************************************************************************
/// \brief
///      Function to parse the imsi in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
os_config_parse_SUPL_imsi( CH *p_IMSI )
{
   if ( p_IMSI != NULL )
   {
      os_config_set_SUPL_imsi( p_IMSI );
   }
   else
   {
      printf( "Error parsing SUPL_imsi.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to get the address of the SUPL server to connect to.
///
/// \returns 
///      Pointer to a string containing the SUPL server address.
//*****************************************************************************
s_CFG_IpAddress *os_config_get_SUPL_server_address( void )
{
   return &OS_Config_Entries.SUPL.SLP_IP_Address;
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL server address in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_server_address( e_TcpIp_AddressType TcpIpAddressType, CH *p_ip_address, U2 Port )
{
   strcpy( OS_Config_Entries.SUPL.SLP_IP_Address.Address, p_ip_address );
   OS_Config_Entries.SUPL.SLP_IP_Address.Port = Port;
   OS_Config_Entries.SUPL.SLP_IP_Address.TcpIp_AddressType = TcpIpAddressType;
}

//*****************************************************************************
/// \brief
///      Function to parse the SUPL server address in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_server_address( CH *p_value )
{
   char *p_ipaddress;
   char *p_port;

   if ( p_value != NULL )
   {
      // Parse value into ipaddress/port.
      p_ipaddress = strtok( p_value, " \t:" );
      if ( p_ipaddress != NULL )
      {
         p_port = strtok( NULL, " \t:" );
         if ( p_ipaddress != NULL )
         {
            unsigned Port;
            sscanf( p_port, "%d", &Port );
            os_config_set_SUPL_server_address( IPV4_Address, p_ipaddress, Port );
         }
         else
         {
            printf( "Error parsing SUPL_server_address.\n" );
         }
      }
      else
      {
         printf( "Error parsing SUPL_server_address.\n" );
      }
   }
   else
   {
      printf( "Error parsing SUPL_server_address.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to get the SUPL push address to connect to.
///
/// \returns 
///      Pointer to a string containing the SUPL push address.
//*****************************************************************************
s_CFG_IpAddress *os_config_get_SUPL_push_address( void )
{
   return &OS_Config_Entries.SUPL.Push_IP_Address;
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL push address in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_push_address( CH ip_address[16], U2 Port )
{
   strcpy( OS_Config_Entries.SUPL.Push_IP_Address.Address, ip_address );
   OS_Config_Entries.SUPL.Push_IP_Address.Port = Port;
}

//*****************************************************************************
/// \brief
///      Function to parse the SUPL push address in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_push_address( CH *p_value )
{
   char *p_ipaddress;
   char *p_port;

   if ( p_value != NULL )
   {
      // Parse value into ipaddress/port.
      p_ipaddress = strtok( p_value, " \t:" );
      if ( p_ipaddress != NULL )
      {
         p_port = strtok( NULL, " \t:" );
         if ( p_ipaddress != NULL )
         {
            unsigned Port;
            sscanf( p_port, "%d", &Port );
            os_config_set_SUPL_push_address( p_ipaddress, Port );
         }
         else
         {
            printf( "Error parsing SUPL_push_address.\n" );
         }
      }
      else
      {
         printf( "Error parsing SUPL_push_address.\n" );
      }
   }
   else
   {
      printf( "Error parsing SUPL_push_address.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to get the SUPL server FQDN to connect to.
///
/// \returns 
///      Pointer to a string containing the SUPL fully qualified domain name.
//*****************************************************************************
CH *os_config_get_SUPL_server_fqdn( void )
{
   return OS_Config_Entries.SUPL.p_SLP_FQDN;
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL server FQDN in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_server_fqdn( CH *p_FQDN )
{
   OS_Config_Entries.SUPL.p_SLP_FQDN = GN_Calloc( 1, FQDN_MAX );
   strcpy( OS_Config_Entries.SUPL.p_SLP_FQDN, p_FQDN );
}

//*****************************************************************************
/// \brief
///      Function to parse the SUPL server FQDN in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_server_fqdn( CH *p_FQDN )
{
   if ( p_FQDN != NULL )
   {
      os_config_set_SUPL_server_fqdn( p_FQDN );
   }
   else
   {
      printf( "Error parsing SUPL_server_fqdn.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL Location ID Type in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_LocationIdType( e_CellInfoType LocationIdType )
{
   OS_Config_Entries.SUPL.LocationId.Type = LocationIdType;
}

//*****************************************************************************
/// \brief
///      Function to get the SUPL Location ID Type.
///
/// \returns 
///      Pointer to a string containing the SUPL Location ID Type.
//*****************************************************************************
e_CellInfoType os_config_get_SUPL_LocationIdType( void )
{
   return OS_Config_Entries.SUPL.LocationId.Type;
}

//*****************************************************************************
/// \brief
///      Function to get the SUPL Location ID.
///
/// \returns 
///      Pointer to a string containing the SUPL Location ID.
//*****************************************************************************
void os_config_get_SUPL_LocationId( s_LocationId *p_LocationId )
{
      p_LocationId->Status = CIS_current;

      p_LocationId->Type = os_config_get_SUPL_LocationIdType();

      switch ( p_LocationId->Type )
      {
      case CIT_gsmCell:    // Cell information is from a GSM network.
         os_config_get_SUPL_GSM_LocationId( &p_LocationId->of_type.gsmCellInfo );
         break;
      case CIT_wcdmaCell:  // Cell information is from a WCDMA network.
         os_config_get_SUPL_WCDMA_LocationId( &p_LocationId->of_type.wcdmaCellInfo );
         if ( p_LocationId->of_type.wcdmaCellInfo.p_FrequencyInfo != NULL )
         {
            switch ( p_LocationId->of_type.wcdmaCellInfo.p_FrequencyInfo->modeSpecificInfoType )
            {
            case fdd:                 // Frequency Division Duplexed mode
               #if 1 // forcibly add some measurements to wcdma for testing.
                  p_LocationId->of_type.wcdmaCellInfo.MeasuredResultCount = 1 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList = GN_Calloc( 1, sizeof( s_MeasuredResult ) ) ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].FrequencyInfoPresent = TRUE ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].FrequencyInfo.modeSpecificInfoType = fdd ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].FrequencyInfo.of_type.fdd.uarfcn_DL = 10689 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].FrequencyInfo.of_type.fdd.uarfcn_UL = -1 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].utra_CarrierRSSI = 28;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].CellMeasuredResultCount = 1 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList = GN_Calloc( p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].CellMeasuredResultCount, sizeof( s_CellMeasuredResult ) );
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].cellIdentity = p_LocationId->of_type.wcdmaCellInfo.refUC ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].modeSpecificInfoType = fdd;
                  // p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.fdd.PrimaryScramblingCode = p_LocationId->of_type.wcdmaCellInfo.primaryScramblingCode ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.fdd.PrimaryScramblingCode = 90 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.fdd.cpich_Ec_N0 = -1 ; // 46 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.fdd.cpich_RSCP = -1 ; // 64 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.fdd.pathloss = 101 ;

                  //p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[1].cellIdentity = -1 ;
                  //p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[1].modeSpecificInfoType = fdd;
                  //p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[1].of_type.fdd.PrimaryScramblingCode = 91 ;
                  //p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[1].of_type.fdd.cpich_Ec_N0 = 25 ;
                  //p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[1].of_type.fdd.cpich_RSCP = 0 ;
                  //p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[1].of_type.fdd.pathloss = 117 ;

                  //p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[2].cellIdentity = -1 ;
                  //p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[2].modeSpecificInfoType = fdd;
                  //p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[2].of_type.fdd.PrimaryScramblingCode = 91 ;
                  //p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[2].of_type.fdd.cpich_Ec_N0 = 25 ;
                  //p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[2].of_type.fdd.cpich_RSCP = 0 ;
                  //p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[2].of_type.fdd.pathloss = 117 ;
               #endif
               break;
            case tdd:                 // Time Division Duplexed mode
                  p_LocationId->of_type.wcdmaCellInfo.MeasuredResultCount = 1 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList = GN_Calloc( 1, sizeof( s_MeasuredResult ) ) ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].FrequencyInfoPresent = TRUE ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].FrequencyInfo.modeSpecificInfoType = tdd ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].FrequencyInfo.of_type.tdd.uarfcn_Nt = 10689 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].utra_CarrierRSSI = 28;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].CellMeasuredResultCount = 1 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList = GN_Calloc( p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].CellMeasuredResultCount, sizeof( s_CellMeasuredResult ) );
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].cellIdentity = p_LocationId->of_type.wcdmaCellInfo.refUC ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].modeSpecificInfoType = tdd;
                  // p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.tdd.PrimaryScramblingCode = p_LocationId->of_type.wcdmaCellInfo.primaryScramblingCode ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.tdd.cellParametersID = 5 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.tdd.pathloss = 46 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.tdd.proposedTGSN = -1 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.tdd.primaryCCPCH_RSCP = -1 ;

                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.tdd.timeslotISCP_Count = 5 ;

                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.tdd.p_TimeslotISCP_List = GN_Calloc( 1, 5 );
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.tdd.p_TimeslotISCP_List[0] = 5 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.tdd.p_TimeslotISCP_List[1] = 4 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.tdd.p_TimeslotISCP_List[2] = 3 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.tdd.p_TimeslotISCP_List[3] = 2 ;
                  p_LocationId->of_type.wcdmaCellInfo.p_MeasuredResultsList[0].p_CellMeasuredResultsList[0].of_type.tdd.p_TimeslotISCP_List[4] = 1 ;
               break;
            }
         }
         break;
      case CIT_cdmaCell:   // Cell information is from a CDMA network.
         os_config_get_SUPL_CDMA_LocationId( &p_LocationId->of_type.cdmaCellInfo );
         break;
      }
}


//*****************************************************************************
/// \brief
///      Function to set the SUPL GSM Location ID in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_GSM_LocationId( s_gsmCellInfo *p_GSM_LocationId )
{
   s_gsmCellInfo *p_GSM_src, *p_GSM_dst;

   os_config_set_SUPL_LocationIdType( CIT_gsmCell );

   p_GSM_src = p_GSM_LocationId;
   p_GSM_dst = &OS_Config_Entries.SUPL.LocationId.of_type.gsmCellInfo;

   p_GSM_dst->refMCC    = p_GSM_src->refMCC;
   p_GSM_dst->refMNC    = p_GSM_src->refMNC;
   p_GSM_dst->refLAC    = p_GSM_src->refLAC;
   p_GSM_dst->refCI     = p_GSM_src->refCI;
   p_GSM_dst->tA        = p_GSM_src->tA;
   p_GSM_dst->NMRCount  = p_GSM_src->NMRCount;

   if ( p_GSM_src->NMRCount > 0 )
   {
      p_GSM_dst->p_NMRList = GN_Calloc( p_GSM_src->NMRCount, sizeof( s_NMRElement ) );
      memcpy( p_GSM_dst->p_NMRList, p_GSM_src->p_NMRList, p_GSM_src->NMRCount * sizeof( s_NMRElement ) );
   }
}

//*****************************************************************************
/// \brief
///      Function to get the current GSM Location.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_get_SUPL_GSM_LocationId (
   s_gsmCellInfo *p_GSM_LocationId
)
{
   s_gsmCellInfo *p_GSM_src, *p_GSM_dst;

   p_GSM_src = &OS_Config_Entries.SUPL.LocationId.of_type.gsmCellInfo;
   p_GSM_dst = p_GSM_LocationId;

   p_GSM_dst->refMCC    = p_GSM_src->refMCC;
   p_GSM_dst->refMNC    = p_GSM_src->refMNC;
   p_GSM_dst->refLAC    = p_GSM_src->refLAC;
   p_GSM_dst->refCI     = p_GSM_src->refCI;
   p_GSM_dst->tA        = p_GSM_src->tA;
   p_GSM_dst->NMRCount  = p_GSM_src->NMRCount;

   if ( p_GSM_src->NMRCount > 0 )
   {
      p_GSM_dst->p_NMRList = GN_Calloc( p_GSM_src->NMRCount, sizeof( s_NMRElement ) );
      memcpy( p_GSM_dst->p_NMRList, p_GSM_src->p_NMRList, p_GSM_src->NMRCount * sizeof( s_NMRElement ) );
   }
   else
   {
      p_GSM_LocationId->p_NMRList = NULL;
   }

}

//*****************************************************************************
/// \brief
///      Function to parse the GSM Location ID in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_GSM_LocationId( CH *p_value )
{
   s_gsmCellInfo GSM_LocationId;

   struct {
      int aRFCN;
      int bsic;
      int rxLev;
   } NMR[15];

   // Preset NMR and tA to -1 so we know if they are not extracted when the cellid is parsed.
   memset( NMR, 0xff, sizeof NMR );
   GSM_LocationId.tA = -1;

   GSM_LocationId.p_NMRList = NULL;

   if ( p_value != NULL )
   {
      int NMRCount;
      sscanf( p_value, "%d:%d:%d:%d:%d:{%d,%d,%d}{%d,%d,%d}{%d,%d,%d}{%d,%d,%d}{%d,%d,%d}{%d,%d,%d}{%d,%d,%d}{%d,%d,%d}{%d,%d,%d}{%d,%d,%d}{%d,%d,%d}{%d,%d,%d}{%d,%d,%d}{%d,%d,%d}{%d,%d,%d}",
         &GSM_LocationId.refMCC, &GSM_LocationId.refMNC,
         &GSM_LocationId.refLAC, &GSM_LocationId.refCI, &GSM_LocationId.tA,
         &NMR[0].aRFCN, &NMR[0].bsic, &NMR[0].rxLev,
         &NMR[1].aRFCN, &NMR[1].bsic, &NMR[1].rxLev,
         &NMR[2].aRFCN, &NMR[2].bsic, &NMR[2].rxLev,
         &NMR[3].aRFCN, &NMR[3].bsic, &NMR[3].rxLev,
         &NMR[4].aRFCN, &NMR[4].bsic, &NMR[4].rxLev,
         &NMR[5].aRFCN, &NMR[5].bsic, &NMR[5].rxLev,
         &NMR[6].aRFCN, &NMR[6].bsic, &NMR[6].rxLev,
         &NMR[7].aRFCN, &NMR[7].bsic, &NMR[7].rxLev,
         &NMR[8].aRFCN, &NMR[8].bsic, &NMR[8].rxLev,
         &NMR[9].aRFCN, &NMR[9].bsic, &NMR[9].rxLev,
         &NMR[10].aRFCN, &NMR[10].bsic, &NMR[10].rxLev,
         &NMR[11].aRFCN, &NMR[11].bsic, &NMR[11].rxLev,
         &NMR[12].aRFCN, &NMR[12].bsic, &NMR[12].rxLev,
         &NMR[13].aRFCN, &NMR[13].bsic, &NMR[13].rxLev,
         &NMR[14].aRFCN, &NMR[14].bsic, &NMR[14].rxLev
         );

      for ( NMRCount = 0; NMRCount < 15; NMRCount++ )
      {
         if ( ( NMR[NMRCount].aRFCN == -1 ) ||
              ( NMR[NMRCount].bsic  == -1 ) ||
              ( NMR[NMRCount].rxLev == -1 ) )
         {
            break;
         }

      }
      GSM_LocationId.NMRCount = NMRCount;
      if ( NMRCount > 0 )
      {
         GSM_LocationId.p_NMRList = GN_Calloc( NMRCount, sizeof( s_NMRElement ) );
         for ( NMRCount = 0 ; NMRCount < GSM_LocationId.NMRCount ; NMRCount++ )
         {
            GSM_LocationId.p_NMRList[NMRCount].aRFCN = NMR[NMRCount].aRFCN;
            GSM_LocationId.p_NMRList[NMRCount].bSIC  = NMR[NMRCount].bsic;
            GSM_LocationId.p_NMRList[NMRCount].rxLev = NMR[NMRCount].rxLev;
         } 
      }
      os_config_set_SUPL_GSM_LocationId( &GSM_LocationId );
      GN_Free( GSM_LocationId.p_NMRList );
   }
   else
   {
      printf( "Error parsing GSM Location Id.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL CDMA Location ID in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_CDMA_LocationId( s_cdmaCellInfo *p_CDMA_LocationId )
{
   s_cdmaCellInfo *p_CDMA_src, *p_CDMA_dst;

   os_config_set_SUPL_LocationIdType( CIT_cdmaCell );

   p_CDMA_src = p_CDMA_LocationId;
   p_CDMA_dst = &OS_Config_Entries.SUPL.LocationId.of_type.cdmaCellInfo;

   p_CDMA_dst->refNID        = (U2) p_CDMA_src->refNID;          // INTEGER(0..65535), -- Network Id
   p_CDMA_dst->refSID        = (U2) p_CDMA_src->refSID;          // INTEGER(0..32767), -- System Id
   p_CDMA_dst->refBASEID     = (U2) p_CDMA_src->refBASEID;       // INTEGER(0..65535), -- Base Station Id
   p_CDMA_dst->refBASELAT    = (U4) p_CDMA_src->refBASELAT;      // INTEGER(0..4194303), -- Base Station Latitude
   p_CDMA_dst->reBASELONG    = (U4) p_CDMA_src->reBASELONG;      // INTEGER(0..8388607), -- Base Station Longitude
   p_CDMA_dst->refREFPN      = (U2) p_CDMA_src->refREFPN;        // INTEGER(0..511), -- Base Station PN Code
   p_CDMA_dst->refWeekNumber = (U2) p_CDMA_src->refWeekNumber;   // INTEGER(0..65535), -- GPS Week Number
   p_CDMA_dst->refSeconds    = (U4) p_CDMA_src->refSeconds;      // INTEGER(0..4194303),-- GPS Seconds
}

//*****************************************************************************
/// \brief Function to get the current GSM Location.
///
//*****************************************************************************
void os_config_get_SUPL_CDMA_LocationId (
   s_cdmaCellInfo *p_CDMA_LocationId
)
{
   s_cdmaCellInfo *p_CDMA_src, *p_CDMA_dst;

   p_CDMA_src = &OS_Config_Entries.SUPL.LocationId.of_type.cdmaCellInfo;
   p_CDMA_dst = p_CDMA_LocationId;

   p_CDMA_dst->refNID        = (U2) p_CDMA_src->refNID;          // INTEGER(0..65535), -- Network Id
   p_CDMA_dst->refSID        = (U2) p_CDMA_src->refSID;          // INTEGER(0..32767), -- System Id
   p_CDMA_dst->refBASEID     = (U2) p_CDMA_src->refBASEID;       // INTEGER(0..65535), -- Base Station Id
   p_CDMA_dst->refBASELAT    = (U4) p_CDMA_src->refBASELAT;      // INTEGER(0..4194303), -- Base Station Latitude
   p_CDMA_dst->reBASELONG    = (U4) p_CDMA_src->reBASELONG;      // INTEGER(0..8388607), -- Base Station Longitude
   p_CDMA_dst->refREFPN      = (U2) p_CDMA_src->refREFPN;        // INTEGER(0..511), -- Base Station PN Code
   p_CDMA_dst->refWeekNumber = (U2) p_CDMA_src->refWeekNumber;   // INTEGER(0..65535), -- GPS Week Number
   p_CDMA_dst->refSeconds    = (U4) p_CDMA_src->refSeconds;      // INTEGER(0..4194303),-- GPS Seconds
}

//*****************************************************************************
/// \brief
///      Function to parse the CDMA Location ID in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_CDMA_LocationId( CH *p_value )
{
   s_cdmaCellInfo CDMA_LocationId;

   U4 refNID;                    // INTEGER(0..65535), -- Network Id
   U4 refSID;                    // INTEGER(0..32767), -- System Id
   U4 refBASEID;                 // INTEGER(0..65535), -- Base Station Id
   U4 refBASELAT;                // INTEGER(0..4194303), -- Base Station Latitude
   U4 reBASELONG;                // INTEGER(0..8388607), -- Base Station Longitude
   U4 refREFPN;                  // INTEGER(0..511), -- Base Station PN Code
   U4 refWeekNumber;             // INTEGER(0..65535), -- GPS Week Number
   U4 refSeconds;                // INTEGER(0..4194303),-- GPS Seconds

   if ( p_value != NULL )
   {
      sscanf( p_value, "%u:%u:%u:%u:%u:%u:%u:%u",
         &refNID,                    // INTEGER(0..65535), -- Network Id
         &refSID,                    // INTEGER(0..32767), -- System Id
         &refBASEID,                 // INTEGER(0..65535), -- Base Station Id
         &refBASELAT,                // INTEGER(0..4194303), -- Base Station Latitude
         &reBASELONG,                // INTEGER(0..8388607), -- Base Station Longitude
         &refREFPN,                  // INTEGER(0..511), -- Base Station PN Code
         &refWeekNumber,             // INTEGER(0..65535), -- GPS Week Number
         &refSeconds                 // INTEGER(0..4194303),-- GPS Seconds
         );
      CDMA_LocationId.refNID        = (U2) refNID;          // INTEGER(0..65535), -- Network Id
      CDMA_LocationId.refSID        = (U2) refSID;          // INTEGER(0..32767), -- System Id
      CDMA_LocationId.refBASEID     = (U2) refBASEID;       // INTEGER(0..65535), -- Base Station Id
      CDMA_LocationId.refBASELAT    = (U4) refBASELAT;      // INTEGER(0..4194303), -- Base Station Latitude
      CDMA_LocationId.reBASELONG    = (U4) reBASELONG;      // INTEGER(0..8388607), -- Base Station Longitude
      CDMA_LocationId.refREFPN      = (U2) refREFPN;        // INTEGER(0..511), -- Base Station PN Code
      CDMA_LocationId.refWeekNumber = (U2) refWeekNumber;   // INTEGER(0..65535), -- GPS Week Number
      CDMA_LocationId.refSeconds    = (U4) refSeconds;      // INTEGER(0..4194303),-- GPS Seconds

      os_config_set_SUPL_CDMA_LocationId( &CDMA_LocationId );
   }
   else
   {
      printf( "Error parsing CDMA Location Id.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL WCDMA Location ID in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_WCDMA_LocationId( s_wcdmaCellInfo *p_WCDMA_LocationId )
{
   s_wcdmaCellInfo *p_WCDMA_src, *p_WCDMA_dst;

   BL status;

   os_config_set_SUPL_LocationIdType( CIT_wcdmaCell );

   p_WCDMA_src = p_WCDMA_LocationId;
   p_WCDMA_dst = &OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo;

   status = os_Copy_wcdmaCellInfo( p_WCDMA_dst, p_WCDMA_src );
}

//*****************************************************************************
/// \brief
///      Function to get the current WCDMA Location.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_get_SUPL_WCDMA_LocationId (
   s_wcdmaCellInfo *p_WCDMA_LocationId
)
{
   s_wcdmaCellInfo *p_WCDMA_src, *p_WCDMA_dst;

   BL status;

   p_WCDMA_src = &OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo;
   p_WCDMA_dst = p_WCDMA_LocationId;

   status = os_Copy_wcdmaCellInfo( p_WCDMA_dst, p_WCDMA_src );

}

//*****************************************************************************
/// \brief
///      Function to parse the WCDMA Location ID in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_WCDMA_LocationId( CH *p_value )
{
   s_wcdmaCellInfo WCDMA_LocationId;
   U4 refMCC;                 // INTEGER(0..999), -- Mobile Country Code
   U4 refMNC;                 // INTEGER(0..999), -- Mobile Network Code
   U4 refUC;                  // INTEGER(0..268435455), -- Cell identity
   I4 primaryScramblingCode;  // INTEGER(0..511) OPTIONAL (-1 = not present)
   
   primaryScramblingCode = -1;
   WCDMA_LocationId.primaryScramblingCode = -1; // INTEGER(0..511) OPTIONAL (-1 = not present).

   WCDMA_LocationId.p_FrequencyInfo = NULL;
   WCDMA_LocationId.p_MeasuredResultsList = NULL;
   WCDMA_LocationId.MeasuredResultCount = 0;

   if ( p_value != NULL )
   {
      sscanf( p_value, "%u:%u:%u:%d",
         &refMCC,                // INTEGER(0..999), -- Mobile Country Code
         &refMNC,                // INTEGER(0..999), -- Mobile Network Code
         &refUC,                 // INTEGER(0..268435455), -- Cell identity
         &primaryScramblingCode  // INTEGER(0..511) OPTIONAL (-1 = not present)
         );

      WCDMA_LocationId.refMCC = refMCC;   // INTEGER(0..999), -- Mobile Country Code
      WCDMA_LocationId.refMNC = refMNC;   // INTEGER(0..999), -- Mobile Network Code
      WCDMA_LocationId.refUC  = refUC;    // INTEGER(0..268435455), -- Cell identity
      WCDMA_LocationId.primaryScramblingCode = primaryScramblingCode;     // INTEGER(0..511) OPTIONAL (-1 = not present)

      os_config_set_SUPL_WCDMA_LocationId( &WCDMA_LocationId );
   }
   else
   {
      printf( "Error parsing WCDMA Location Id.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL WCDMA Frequency Info in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_WCDMA_LocationId_FrequencyInfo( s_wcdmaCellInfo *p_WCDMA_LocationId )
{
   s_wcdmaCellInfo *p_WCDMA_src, *p_WCDMA_dst;

   if ( os_config_get_SUPL_LocationIdType() == CIT_wcdmaCell )
   {
      p_WCDMA_src = p_WCDMA_LocationId;
      p_WCDMA_dst = &OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo;

      // status = os_Copy_wcdmaCellInfo( p_WCDMA_dst, p_WCDMA_src );
      p_WCDMA_dst->p_FrequencyInfo = GN_Calloc( 1, sizeof( s_FrequencyInfo ) );
      p_WCDMA_dst->p_FrequencyInfo->modeSpecificInfoType = p_WCDMA_src->p_FrequencyInfo->modeSpecificInfoType;

      switch ( p_WCDMA_dst->p_FrequencyInfo->modeSpecificInfoType )
      {
      case fdd:                 // Frequency Division Duplexed mode
         p_WCDMA_dst->p_FrequencyInfo->of_type.fdd.uarfcn_DL = p_WCDMA_src->p_FrequencyInfo->of_type.fdd.uarfcn_DL;
         p_WCDMA_dst->p_FrequencyInfo->of_type.fdd.uarfcn_UL = p_WCDMA_src->p_FrequencyInfo->of_type.fdd.uarfcn_UL;
         break;
      case tdd:                 // Time Division Duplexed mode
         p_WCDMA_dst->p_FrequencyInfo->of_type.tdd.uarfcn_Nt = p_WCDMA_src->p_FrequencyInfo->of_type.tdd.uarfcn_Nt;
         break;
      default:
         OS_ASSERT( 0 );
         break;
      }
   }
   else
   {
      printf( "Error parsing WCDMA Location Id frequency info - ignored.\n" );
   }
}

/////*****************************************************************************
///// \brief Function to get the current WCDMA Location.
/////
////*****************************************************************************
//void os_config_get_SUPL_WCDMA_LocationId_FrequencyInfo (
//   s_wcdmaCellInfo *p_WCDMA_LocationId
//)
//{
//   s_wcdmaCellInfo *p_WCDMA_src, *p_WCDMA_dst;
//
//   BL status;
//
//   p_WCDMA_src = &OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo;
//   p_WCDMA_dst = p_WCDMA_LocationId;
//
//   status = os_Copy_wcdmaCellInfo( p_WCDMA_dst, p_WCDMA_src );
//
//}

//*****************************************************************************
/// \brief
///      Function to parse the WCDMA Frequency Info in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_WCDMA_LocationId_FrequencyInfo( CH *p_value )
{
   s_wcdmaCellInfo WCDMA_LocationId;
   U4 Type;
   I4 uarfcn_UL = -1;   // INTEGER(0..16383), OPTIONAL (-1 = Not present).
   U4 uarfcn_DL;        // INTEGER(0..16383)

   if ( p_value != NULL )
   {
      sscanf( p_value, "%u:%u:%d",
            &Type,
            &uarfcn_DL, // or uarfcn-Nt when type is tdd
            &uarfcn_UL
         );

      WCDMA_LocationId.p_FrequencyInfo = GN_Calloc( 1, sizeof( s_FrequencyInfo ) );
      switch ( Type )
      {
      case 0:     // Frequency Division Duplexed mode information
         WCDMA_LocationId.p_FrequencyInfo->modeSpecificInfoType = fdd;
         WCDMA_LocationId.p_FrequencyInfo->of_type.fdd.uarfcn_DL = uarfcn_DL;
         WCDMA_LocationId.p_FrequencyInfo->of_type.fdd.uarfcn_UL = uarfcn_UL;
         break;
      case 1:     // Time Division Duplexed mode information
         WCDMA_LocationId.p_FrequencyInfo->modeSpecificInfoType = tdd;
         WCDMA_LocationId.p_FrequencyInfo->of_type.tdd.uarfcn_Nt = uarfcn_DL;
         break;
      default:
         OS_ASSERT( 0 );
         break;
      }

      os_config_set_SUPL_WCDMA_LocationId_FrequencyInfo( &WCDMA_LocationId );
      GN_Free( WCDMA_LocationId.p_FrequencyInfo );
   }
   else
   {
      printf( "Error parsing WCDMA Location Id frequence info.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL User Timer 1 in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_UT1( U2 User_Timer_1_Duration )
{
   OS_Config_Entries.SUPL.User_Timer_1_Duration = User_Timer_1_Duration;
}

//*****************************************************************************
/// \brief
///      Function to get the User Timer 1 from the config.
/// \returns
///      Value of UT1.
//*****************************************************************************
U2 os_config_get_SUPL_UT1( void )
{
   return OS_Config_Entries.SUPL.User_Timer_1_Duration;
}

//*****************************************************************************
/// \brief
///      Function to parse the User Timer 1 in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_UT1( CH *p_SUPL_User_Timer_1_Duration )
{
   int SUPL_User_Timer_1_Duration;

   if ( p_SUPL_User_Timer_1_Duration != NULL )
   {
      sscanf( p_SUPL_User_Timer_1_Duration, "%d", &SUPL_User_Timer_1_Duration );

      os_config_set_SUPL_UT1( SUPL_User_Timer_1_Duration );
   }
   else
   {
      printf( "Error parsing SUPL_User_Timer_1_Duration.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL User Timer 2 in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_UT2( U2 User_Timer_2_Duration )
{
   OS_Config_Entries.SUPL.User_Timer_2_Duration = User_Timer_2_Duration;
}

//*****************************************************************************
/// \brief
///      Function to get the User Timer 2 from the config.
/// \returns
///      Value of UT2.
//*****************************************************************************
U2 os_config_get_SUPL_UT2( void )
{
   return OS_Config_Entries.SUPL.User_Timer_2_Duration;
}

//*****************************************************************************
/// \brief
///      Function to parse the User Timer 2 in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_UT2( CH *p_SUPL_User_Timer_2_Duration )
{
   int SUPL_User_Timer_2_Duration;

   if ( p_SUPL_User_Timer_2_Duration != NULL )
   {
      sscanf( p_SUPL_User_Timer_2_Duration, "%d", &SUPL_User_Timer_2_Duration );

      os_config_set_SUPL_UT2( SUPL_User_Timer_2_Duration );
   }
   else
   {
      printf( "Error parsing SUPL_User_Timer_2_Duration.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL User Timer 3 in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_UT3( U2 User_Timer_3_Duration )
{
   OS_Config_Entries.SUPL.User_Timer_3_Duration = User_Timer_3_Duration;
}

//*****************************************************************************
/// \brief
///      Function to get the User Timer 3 from the config.
/// \returns
///      Value of UT3.
//*****************************************************************************
U2 os_config_get_SUPL_UT3( void )
{
   return OS_Config_Entries.SUPL.User_Timer_3_Duration;
}

//*****************************************************************************
/// \brief
///      Function to parse the User Timer 3 in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_UT3( CH *p_SUPL_User_Timer_3_Duration )
{
   int SUPL_User_Timer_3_Duration;

   if ( p_SUPL_User_Timer_3_Duration != NULL )
   {
      sscanf( p_SUPL_User_Timer_3_Duration, "%d", &SUPL_User_Timer_3_Duration );

      os_config_set_SUPL_UT3( SUPL_User_Timer_3_Duration );
   }
   else
   {
      printf( "Error parsing SUPL_User_Timer_3_Duration.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL Version number in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_version( U1 Major, U1 Minor, U1 Service_Indicator )
{
   OS_Config_Entries.SUPL.Version.Major = Major;
   OS_Config_Entries.SUPL.Version.Minor = Minor;
   OS_Config_Entries.SUPL.Version.Service_Indicator = Service_Indicator;
}

//*****************************************************************************
/// \brief
///      Function to parse the Version in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_version( CH *p_value )
{
   int Major, Minor, Service_Indicator;

   if ( p_value != NULL )
   {
      sscanf( p_value, "%d.%d.%d", &Major, &Minor, &Service_Indicator );
      os_config_set_SUPL_version( (U1) Major, (U1) Minor, (U1) Service_Indicator );
   }
   else
   {
      printf( "Error parsing SUPL_User_Timer_3_Duration.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to get the current version of the SUPL spec supported.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_get_SUPL_version(
   U1 *p_Major,                ///< [out] Major version number of SUPL Spec.
   U1 *p_Minor,                ///< [out] Minor version number.
   U1 *p_Service_Indicator     ///< [out] Service indicator.
)
{
   if( p_Major ) *p_Major = OS_Config_Entries.SUPL.Version.Major;
   if( p_Minor ) *p_Minor = OS_Config_Entries.SUPL.Version.Minor;
   if( p_Service_Indicator ) *p_Service_Indicator = OS_Config_Entries.SUPL.Version.Service_Indicator;
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL QoP Horizontal Accuracy in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_QOP_horacc( U1 SUPL_QOP_horacc )
{
   OS_Config_Entries.SUPL.QOP.horacc = SUPL_QOP_horacc;
   OS_Config_Entries.SUPL.QOP.p_horacc = &OS_Config_Entries.SUPL.QOP.horacc;
}

//*****************************************************************************
/// \brief
///      Function to get the QoP Horizontal Accuracy from the config.
/// \returns
///      Value of QoP Horizontal Accuracy.
//*****************************************************************************
U1 *os_config_get_SUPL_QOP_horacc( void )
{
   return OS_Config_Entries.SUPL.QOP.p_horacc;
}

//*****************************************************************************
/// \brief
///      Function to parse the QoP Horizontal Accuracy in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_QOP_horacc( CH *p_SUPL_QOP_horacc )
{
   int SUPL_QOP_horacc;
   if ( p_SUPL_QOP_horacc != NULL )
   {
      sscanf( p_SUPL_QOP_horacc, "%u", &SUPL_QOP_horacc );

      os_config_set_SUPL_QOP_horacc( SUPL_QOP_horacc );
   }
   else
   {
      printf( "Error parsing SUPL_QOP_horacc.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL QoP Vertical Accuracy in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_QOP_veracc( I1 SUPL_QOP_veracc )
{
   OS_Config_Entries.SUPL.QOP.veracc = SUPL_QOP_veracc;
   OS_Config_Entries.SUPL.QOP.p_veracc = &OS_Config_Entries.SUPL.QOP.veracc;
}

//*****************************************************************************
/// \brief
///      Function to get the QoP Vertical Accuracy from the config.
/// \returns
///      Value of QoP Vertical Accuracy.
//*****************************************************************************
I1 *os_config_get_SUPL_QOP_veracc( void )
{
   return OS_Config_Entries.SUPL.QOP.p_veracc;
}

//*****************************************************************************
/// \brief
///      Function to parse the QoP Vertical Accuracy in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_QOP_veracc( CH *p_SUPL_QOP_veracc )
{
   int SUPL_QOP_veracc;

   if ( p_SUPL_QOP_veracc != NULL )
   {
      sscanf( p_SUPL_QOP_veracc, "%d", &SUPL_QOP_veracc );

      os_config_set_SUPL_QOP_veracc( SUPL_QOP_veracc );
   }
   else
   {
      printf( "Error parsing SUPL_QOP_veracc.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL QoP Maximum Location Age in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_QOP_maxLocAge( I4 SUPL_QOP_maxLocAge )
{
   OS_Config_Entries.SUPL.QOP.maxLocAge = SUPL_QOP_maxLocAge;
   OS_Config_Entries.SUPL.QOP.p_maxLocAge = &OS_Config_Entries.SUPL.QOP.maxLocAge;
}

//*****************************************************************************
/// \brief
///      Function to get the QoP Maximum Location Age from the config.
/// \returns
///      Value of QoP Maximum Location Age.
//*****************************************************************************
I4 *os_config_get_SUPL_QOP_maxLocAge( void )
{
   return OS_Config_Entries.SUPL.QOP.p_maxLocAge;
}

//*****************************************************************************
/// \brief
///      Function to parse the QoP Maximum Location Age in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_QOP_maxLocAge( CH *p_SUPL_QOP_maxLocAge )
{
   int SUPL_QOP_maxLocAge;

   if ( p_SUPL_QOP_maxLocAge != NULL )
   {
      sscanf( p_SUPL_QOP_maxLocAge, "%d", &SUPL_QOP_maxLocAge );
      os_config_set_SUPL_QOP_maxLocAge( SUPL_QOP_maxLocAge );
   }
   else
   {
      printf( "Error parsing SUPL_QOP_maxLocAge.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL QoP Delay in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_QOP_delay( I1 SUPL_QOP_delay )
{
   OS_Config_Entries.SUPL.QOP.delay = SUPL_QOP_delay;
   OS_Config_Entries.SUPL.QOP.p_delay = &OS_Config_Entries.SUPL.QOP.delay;
}

//*****************************************************************************
/// \brief
///      Function to get the QoP Delay from the config.
/// \returns
///      Value of QoP Delay.
//*****************************************************************************
I1 *os_config_get_SUPL_QOP_delay( void )
{
   return OS_Config_Entries.SUPL.QOP.p_delay;
}

//*****************************************************************************
/// \brief
///      Function to parse the QoP Delay in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_QOP_delay( CH *p_SUPL_QOP_delay )
{
   int SUPL_QOP_delay;

   if ( p_SUPL_QOP_delay != NULL )
   {
      sscanf( p_SUPL_QOP_delay, "%d", &SUPL_QOP_delay );

      os_config_set_SUPL_QOP_delay( SUPL_QOP_delay );
   }
   else
   {
      printf( "Error parsing SUPL_QOP_delay.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL Notification Rejection in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_Notification_Reject( BL SUPL_Notification_Reject )
{
   OS_Config_Entries.SUPL.Notification_Reject = SUPL_Notification_Reject;
}

//*****************************************************************************
/// \brief
///      Function to get the Notification Rejection from the config.
/// \returns
///      Value of QoP Notification Rejection.
//*****************************************************************************
BL os_config_get_SUPL_Notification_Reject( void )
{
   return OS_Config_Entries.SUPL.Notification_Reject;
}

//*****************************************************************************
/// \brief
///      Function to parse the Notification Rejection Flag in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_Notification_Reject( CH *p_SUPL_Notification_Reject )
{
   int SUPL_Notification_Reject;

   if ( p_SUPL_Notification_Reject != NULL )
   {
      sscanf( p_SUPL_Notification_Reject, "%d", &SUPL_Notification_Reject );

      // Reject the notification if the config entry is set to 1.
      os_config_set_SUPL_Notification_Reject( SUPL_Notification_Reject == 1 );
   }
   else
   {
      printf( "Error parsing SUPL_Notification_Reject.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to get the QoP Horizontal Accuracy from the config.
/// \returns
///      Value of QoP Horizontal Accuracy.
//*****************************************************************************
CH *os_config_get_SUPL_Server_Key_Filename( void )
{
   return OS_Config_Entries.SUPL.p_Server_Key_Filename;
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL Server Key Filename in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_Server_Key_Filename( CH *p_Server_Key_Filename )
{
   OS_Config_Entries.SUPL.p_Server_Key_Filename = GN_Calloc( 1, (U2) strlen( p_Server_Key_Filename ) + 1 );
   strcpy( OS_Config_Entries.SUPL.p_Server_Key_Filename, p_Server_Key_Filename );
}

//*****************************************************************************
/// \brief
///      Function to parse the Server Key Filename in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_Server_Key_Filename( CH *p_Server_Key_Filename )
{
   if ( p_Server_Key_Filename != NULL )
   {
      os_config_set_SUPL_Server_Key_Filename( p_Server_Key_Filename );
   }
   else
   {
      printf( "Error parsing SUPL_Server_Key_Filename.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to get the Server Certificate Filename from the config.
/// \returns
///      Value of Server Certificate Filename.
//*****************************************************************************
CH *os_config_get_SUPL_Server_Cert_Filename( void )
{
   return OS_Config_Entries.SUPL.p_Server_Cert_Filename;
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL Server Certificate Filename in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_Server_Cert_Filename( CH *p_Server_Cert_Filename )
{
   OS_Config_Entries.SUPL.p_Server_Cert_Filename = GN_Calloc( 1, (U2) strlen( p_Server_Cert_Filename ) + 1 );
   strcpy( OS_Config_Entries.SUPL.p_Server_Cert_Filename, p_Server_Cert_Filename );
}

//*****************************************************************************
/// \brief
///      Function to parse the Server Certificate Filename in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_Server_Cert_Filename( CH *p_Server_Cert_Filename )
{
   if ( p_Server_Cert_Filename != NULL )
   {
      os_config_set_SUPL_Server_Cert_Filename( p_Server_Cert_Filename );
   }
   else
   {
      printf( "Error parsing SUPL_Server_Cert_Filename.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to get the CA Certificate Filename from the config.
/// \returns
///      Value of CA Certificate Filename.
//*****************************************************************************
CH *os_config_get_SUPL_CA_Cert_Filename( void )
{
   return OS_Config_Entries.SUPL.p_CA_Cert_Filename;
}

//*****************************************************************************
/// \brief
///      Function to set the SUPL CA Certificate Filename in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_SUPL_CA_Cert_Filename( CH *p_CA_Cert_Filename )
{
   OS_Config_Entries.SUPL.p_CA_Cert_Filename = GN_Calloc( 1, (U2) strlen( p_CA_Cert_Filename ) + 1 );
   strcpy( OS_Config_Entries.SUPL.p_CA_Cert_Filename, p_CA_Cert_Filename );
}

//*****************************************************************************
/// \brief
///      Function to parse the CA Certificate Filename in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_SUPL_CA_Cert_Filename( CH *p_CA_Cert_Filename )
{
   if ( p_CA_Cert_Filename != NULL )
   {
      os_config_set_SUPL_CA_Cert_Filename( p_CA_Cert_Filename );
   }
   else
   {
      printf( "Error parsing SUPL_CA_Cert_Filename.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to set the RRLP Measurement Response Time in the config.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_set_RRLP_Msr_Response_Time( U1 RRLP_Msr_Response_Time )
{
   OS_Config_Entries.RRLP.MsrResponseTime = RRLP_Msr_Response_Time;
}

//*****************************************************************************
/// \brief
///      Function to get the RRLP Measurement Response Time from the config.
/// \returns
///      Value of RRLP Measurement Response Time.
//*****************************************************************************
U1 os_config_get_RRLP_Msr_Response_Time( void )
{
   return OS_Config_Entries.RRLP.MsrResponseTime;
}

//*****************************************************************************
/// \brief
///      Function to parse the RRLP Measurement Response Time in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_parse_RRLP_Msr_Response_Time( CH *p_RRLP_Msr_Response_Time )
{
   int RRLP_Msr_Response_Time;

   if ( p_RRLP_Msr_Response_Time != NULL )
   {
      sscanf( p_RRLP_Msr_Response_Time, "%d", &RRLP_Msr_Response_Time );

      os_config_set_RRLP_Msr_Response_Time( RRLP_Msr_Response_Time );
   }
   else
   {
      printf( "Error parsing RRLP_Msr_Response_Time.\n" );
   }
}

//*****************************************************************************
/// \brief
///      Function to parse the Entries in the field in the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void supl_parse_config_line( char * config_line )
{
   int Token_Found_In_Line = 0;
   int entry;
   char *p_token;
   char temp_line[MAX_LINE_LENGTH];
   char SUPL_Server_Address[]          = {"SUPL_Server_Address"};
   char SUPL_Push_Address[]            = {"SUPL_Push_Address"};
   char SUPL_Server_FQDN[]             = {"SUPL_Server_FQDN"};
   char SUPL_Set_MSISDN[]              = {"SUPL_Set_MSISDN"};
   char SUPL_Set_IMSI[]                = {"SUPL_Set_IMSI"};
   char SUPL_User_Timer_1_Duration[]   = {"SUPL_User_Timer_1_Duration"};
   char SUPL_User_Timer_2_Duration[]   = {"SUPL_User_Timer_2_Duration"};
   char SUPL_User_Timer_3_Duration[]   = {"SUPL_User_Timer_3_Duration"};
   char SUPL_Version[]                 = {"SUPL_Version"};
   char SUPL_GSM_LocationId[]          = {"SUPL_GSM_LocationId"};
   char SUPL_CDMA_LocationId[]         = {"SUPL_CDMA_LocationId"};
   char SUPL_WCDMA_LocationId[]        = {"SUPL_WCDMA_LocationId"};
   char SUPL_WCDMA_LocationId_FrequencyInfo[]        = {"SUPL_WCDMA_LocationId_FrequencyInfo"};
   char SUPL_QOP_horacc[]              = {"SUPL_QOP_horacc"};
   char SUPL_QOP_veracc[]              = {"SUPL_QOP_veracc"};
   char SUPL_QOP_maxLocAge[]           = {"SUPL_QOP_maxLocAge"};
   char SUPL_QOP_delay[]               = {"SUPL_QOP_delay"};
   char SUPL_Notification_Reject[]     = {"SUPL_Notification_Reject"};
   char SUPL_Server_Key_Filename[]     = {"SUPL_Server_Key_Filename"};
   char SUPL_Server_Cert_Filename[]    = {"SUPL_Server_Cert_Filename"};
   char SUPL_CA_Cert_Filename[]        = {"SUPL_CA_Cert_Filename"};
   char RRLP_Msr_Response_Time[]       = {"RRLP_Msr_Response_Time"};

   char *config_entries[] ={
      SUPL_Server_Address,
      SUPL_Push_Address,
      SUPL_Server_FQDN,
      SUPL_Set_MSISDN,
      SUPL_Set_IMSI,
      SUPL_User_Timer_1_Duration,
      SUPL_User_Timer_2_Duration,
      SUPL_User_Timer_3_Duration,
      SUPL_Version,
      SUPL_GSM_LocationId,
      SUPL_CDMA_LocationId,
      SUPL_WCDMA_LocationId,
      SUPL_WCDMA_LocationId_FrequencyInfo,
      SUPL_QOP_horacc,
      SUPL_QOP_veracc,
      SUPL_QOP_maxLocAge,
      SUPL_QOP_delay,
      SUPL_Notification_Reject,
      SUPL_Server_Key_Filename,
      SUPL_Server_Cert_Filename,
      RRLP_Msr_Response_Time,
      SUPL_CA_Cert_Filename
   };

   int entry_count = sizeof( config_entries )/sizeof( config_entries[0] );

   strncpy( temp_line, config_line, MAX_LINE_LENGTH - 1 );
   temp_line[MAX_LINE_LENGTH - 1] = '\0';
   // Parse line into token/value.
   p_token = strtok( temp_line, " \t=" );
   if ( p_token != 0 )
   {
      for ( entry = 0; entry < entry_count; entry++ )
      {
         if ( ( strcmp( p_token, config_entries[entry] ) ) == 0 )
         {
            // Get the value associated with the token. NULL means use last string.
            char *p_value = strtok( NULL, " \t=" );

            printf( "found %s in: %s\n", config_entries[entry], config_line );

            if ( config_entries[entry] == SUPL_Server_Address )
            {
               os_config_parse_SUPL_server_address( p_value );
            }
            else if ( config_entries[entry] == SUPL_Push_Address )
            {
               os_config_parse_SUPL_push_address( p_value );
            }
            else if ( config_entries[entry] == SUPL_Server_FQDN )
            {
               os_config_parse_SUPL_server_fqdn( p_value );
            }
            else if ( config_entries[entry] == SUPL_Set_MSISDN )
            {
               os_config_parse_SUPL_msisdn( p_value );
            }
            else if ( config_entries[entry] == SUPL_Set_IMSI )
            {
               os_config_parse_SUPL_imsi( p_value );
            }
            else if ( config_entries[entry] == SUPL_User_Timer_1_Duration )
            {
               os_config_parse_SUPL_UT1( p_value );
            }
            else if ( config_entries[entry] == SUPL_User_Timer_2_Duration )
            {
               os_config_parse_SUPL_UT2( p_value );
            }
            else if ( config_entries[entry] == SUPL_User_Timer_3_Duration )
            {
               os_config_parse_SUPL_UT3( p_value );
            }
            else if ( config_entries[entry] == SUPL_Version )
            {
               os_config_parse_SUPL_version( p_value );
            }
            else if ( config_entries[entry] == SUPL_GSM_LocationId )
            {
               os_config_parse_SUPL_GSM_LocationId( p_value );
            }
            else if ( config_entries[entry] == SUPL_CDMA_LocationId )
            {
               os_config_parse_SUPL_CDMA_LocationId( p_value );
            }
            else if ( config_entries[entry] == SUPL_WCDMA_LocationId )
            {
               os_config_parse_SUPL_WCDMA_LocationId( p_value );
            }
            else if ( config_entries[entry] == SUPL_WCDMA_LocationId_FrequencyInfo )
            {
               os_config_parse_SUPL_WCDMA_LocationId_FrequencyInfo( p_value );
            }
            else if ( config_entries[entry] == SUPL_QOP_horacc )
            {
               os_config_parse_SUPL_QOP_horacc( p_value );
            }
            else if ( config_entries[entry] == SUPL_QOP_veracc )
            {
               os_config_parse_SUPL_QOP_veracc( p_value );
            }
            else if ( config_entries[entry] == SUPL_QOP_maxLocAge )
            {
               os_config_parse_SUPL_QOP_maxLocAge( p_value );
            }
            else if ( config_entries[entry] == SUPL_QOP_delay )
            {
               os_config_parse_SUPL_QOP_delay( p_value );
            }
            else if ( config_entries[entry] == SUPL_Notification_Reject )
            {
               os_config_parse_SUPL_Notification_Reject( p_value );
            }
            else if ( config_entries[entry] == SUPL_Server_Key_Filename )
            {
               os_config_parse_SUPL_Server_Key_Filename( p_value );
            }
            else if ( config_entries[entry] == SUPL_Server_Cert_Filename )
            {
               os_config_parse_SUPL_Server_Cert_Filename( p_value );
            }
            else if ( config_entries[entry] == SUPL_CA_Cert_Filename )
            {
               os_config_parse_SUPL_CA_Cert_Filename( p_value );
            }
            else if ( config_entries[entry] == RRLP_Msr_Response_Time )
            {
               os_config_parse_RRLP_Msr_Response_Time( p_value );
            }
            else
            {
               printf( "Unable to find config entry in line:%s\n", config_entries[entry], config_line );
            }
         }
         else Token_Found_In_Line = 0;
      }
   }
}

//*****************************************************************************
/// \brief
///      Function to parse the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_init( void )
{
   s_GN_SUPL_Config GN_SUPL_Config;

   BL status;

   char buffer[6000]; // buffer containing the file.
   char * lines[200];  // table of pointers to individual lines.
   int line_count = 0;
   int line;

   memset( &OS_Config_Entries, 0, sizeof( OS_Config_Entries ) );
   memset( &GN_SUPL_Config, 0, sizeof( GN_SUPL_Config ) );
   // set up some defaults
   os_config_set_SUPL_server_address( IPV4_Address, "127.0.0.1", 7275 );
   os_config_set_SUPL_push_address( "", 7277 );
   //os_config_set_SUPL_msisdn( "" );
   //os_config_set_SUPL_imsi( "" );

   os_config_set_SUPL_UT1( USER_TIMER_1_DEFAULT );
   os_config_set_SUPL_UT2( USER_TIMER_2_DEFAULT );
   os_config_set_SUPL_UT3( USER_TIMER_3_DEFAULT );

   os_config_set_SUPL_version( 1, 0, 0 );

   line_count = supl_config_read( SUPL_CONFIG_FILE_NAME,
                                 buffer,
                                 sizeof( buffer ),
                                 lines,
                                 sizeof( lines ) );
   for ( line = 0 ; line < line_count ; line++ )
   {
      supl_parse_config_line( lines[line] );
   }
   GN_SUPL_Config.p_SET_msisdn          = OS_Config_Entries.SUPL.p_MSISDN;
   GN_SUPL_Config.p_SET_imsi            = OS_Config_Entries.SUPL.p_IMSI;
   GN_SUPL_Config.SLP_IP_Address        = OS_Config_Entries.SUPL.SLP_IP_Address.Address;
   GN_SUPL_Config.SLP_IP_Port           = OS_Config_Entries.SUPL.SLP_IP_Address.Port;
   GN_SUPL_Config.SLP_IP_Type           = IPV4_Address;
   GN_SUPL_Config.User_Timer_1_Duration = OS_Config_Entries.SUPL.User_Timer_1_Duration;
   GN_SUPL_Config.User_Timer_2_Duration = OS_Config_Entries.SUPL.User_Timer_2_Duration;
   GN_SUPL_Config.User_Timer_3_Duration = OS_Config_Entries.SUPL.User_Timer_3_Duration;
   GN_SUPL_Config.PT_agpsSETAssisted    = TRUE;
   GN_SUPL_Config.PT_agpsSETBased       = TRUE;
   GN_SUPL_Config.PT_autonomousGPS      = TRUE;
   GN_SUPL_Config.PT_eCID               = TRUE ;
   status = GN_SUPL_Set_Config( &GN_SUPL_Config );

}

//*****************************************************************************
/// \brief
///      Function to clean up any memory used when parsing the config file.
/// \returns
///      Nothing.
//*****************************************************************************
void os_config_deinit( void )
{
   if ( OS_Config_Entries.SUPL.p_MSISDN != NULL )
   {
      GN_Free( OS_Config_Entries.SUPL.p_MSISDN );
      OS_Config_Entries.SUPL.p_MSISDN = NULL;
   }
   if ( OS_Config_Entries.SUPL.p_IMSI != NULL )
   {
      GN_Free( OS_Config_Entries.SUPL.p_IMSI );
      OS_Config_Entries.SUPL.p_IMSI = NULL;
   }

   // Used on the host side. Do not deallocate until the end of the sequence.
   if ( OS_Config_Entries.SUPL.p_Server_Cert_Filename != NULL )
   {
      GN_Free( OS_Config_Entries.SUPL.p_Server_Cert_Filename );
      OS_Config_Entries.SUPL.p_Server_Cert_Filename = NULL;
   }
   if ( OS_Config_Entries.SUPL.p_Server_Key_Filename != NULL )
   {
      GN_Free( OS_Config_Entries.SUPL.p_Server_Key_Filename );
      OS_Config_Entries.SUPL.p_Server_Key_Filename = NULL;
   }
   if ( OS_Config_Entries.SUPL.p_CA_Cert_Filename != NULL )
   {
      GN_Free( OS_Config_Entries.SUPL.p_CA_Cert_Filename );
      OS_Config_Entries.SUPL.p_CA_Cert_Filename = NULL;
   }
   if ( OS_Config_Entries.SUPL.p_SLP_FQDN != NULL )
   {
      GN_Free( OS_Config_Entries.SUPL.p_SLP_FQDN );
      OS_Config_Entries.SUPL.p_SLP_FQDN = NULL;
   }
   switch ( OS_Config_Entries.SUPL.LocationId.Type )
   {
   case CIT_gsmCell:   // Cell information is from a GSM network.
      if ( OS_Config_Entries.SUPL.LocationId.of_type.gsmCellInfo.p_NMRList != NULL )
      {
         GN_Free( OS_Config_Entries.SUPL.LocationId.of_type.gsmCellInfo.p_NMRList );
         OS_Config_Entries.SUPL.LocationId.of_type.gsmCellInfo.p_NMRList = NULL;
      }
      break;
   case CIT_wcdmaCell: // Cell information is from a WCDMA network.
      if ( OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo.p_FrequencyInfo != NULL )
      {
         GN_Free( OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo.p_FrequencyInfo );
         OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo.p_FrequencyInfo = NULL;
      }
      if ( OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo.p_MeasuredResultsList != NULL )
      {
         if ( OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList != NULL )
         {
            switch ( OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList->modeSpecificInfoType )
            {
            case fdd:                 // Frequency Division Duplexed mode
               break;
            case tdd:                 // Time Division Duplexed mode
               if ( OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList->of_type.tdd.p_TimeslotISCP_List != NULL )
               {
                  GN_Free( OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList->of_type.tdd.p_TimeslotISCP_List );
                  OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList->of_type.tdd.p_TimeslotISCP_List = NULL;
               }
               break;
            }
            GN_Free( OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList );
            OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo.p_MeasuredResultsList->p_CellMeasuredResultsList = NULL;
         }
         GN_Free( OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo.p_MeasuredResultsList );
         OS_Config_Entries.SUPL.LocationId.of_type.wcdmaCellInfo.p_MeasuredResultsList = NULL;
      }
      break;
   case CIT_cdmaCell:  // Cell information is from a CDMA network.
      break;
   }
}
//*****************************************************************************
/// \brief
///      Function to take a copy of the wcdma cell information in a location ID.
///
/// \returns
///      #TRUE if successful.
///      #FALSE if unsuccessful.
//*****************************************************************************
BL os_Copy_wcdmaCellInfo(
   s_wcdmaCellInfo *p_WCDMA_dst, ///< [out] Cell information from a WCDMA network.
   s_wcdmaCellInfo *p_WCDMA_src  ///< [in] Cell information from a WCDMA network.
)
{
   BL Status = TRUE;
   p_WCDMA_dst->refMCC = p_WCDMA_src->refMCC;
   p_WCDMA_dst->refMNC = p_WCDMA_src->refMNC;
   p_WCDMA_dst->refUC  = p_WCDMA_src->refUC;
   p_WCDMA_dst->primaryScramblingCode = p_WCDMA_src->primaryScramblingCode;
   if ( p_WCDMA_src->p_FrequencyInfo != NULL )
   {
      p_WCDMA_dst->p_FrequencyInfo = GN_Calloc( 1, sizeof( s_FrequencyInfo ) );
      p_WCDMA_dst->p_FrequencyInfo->modeSpecificInfoType = p_WCDMA_src->p_FrequencyInfo->modeSpecificInfoType;
      switch ( p_WCDMA_src->p_FrequencyInfo->modeSpecificInfoType )
      {
      case fdd:   // Frequency Division Duplexed mode
         p_WCDMA_dst->p_FrequencyInfo->of_type.fdd.uarfcn_UL = p_WCDMA_src->p_FrequencyInfo->of_type.fdd.uarfcn_UL;
         p_WCDMA_dst->p_FrequencyInfo->of_type.fdd.uarfcn_DL = p_WCDMA_src->p_FrequencyInfo->of_type.fdd.uarfcn_DL;
         break;
      case tdd:   // Time Division Duplexed mode
         p_WCDMA_dst->p_FrequencyInfo->of_type.tdd.uarfcn_Nt = p_WCDMA_src->p_FrequencyInfo->of_type.tdd.uarfcn_Nt;
         break;
      default:
         Status = FALSE;
         break;
      }
   }
   else
   {
      p_WCDMA_dst->p_FrequencyInfo = NULL;
   }

   if ( p_WCDMA_src->MeasuredResultCount > 0 && p_WCDMA_src->p_MeasuredResultsList != NULL )
   {
      U1 ResultCount;

      p_WCDMA_dst->p_MeasuredResultsList = GN_Calloc( p_WCDMA_src->MeasuredResultCount, sizeof( s_MeasuredResult ) );
      for ( ResultCount = 0 ; ResultCount < p_WCDMA_src->MeasuredResultCount ; ResultCount++ )
      {

         s_MeasuredResult* p_MeasuredResult_dst = &p_WCDMA_dst->p_MeasuredResultsList[ResultCount];
         s_MeasuredResult* p_MeasuredResult_src = &p_WCDMA_src->p_MeasuredResultsList[ResultCount];
         
         p_WCDMA_dst->MeasuredResultCount = p_WCDMA_src->MeasuredResultCount;

         p_MeasuredResult_dst->utra_CarrierRSSI        = p_MeasuredResult_src->utra_CarrierRSSI;
         p_MeasuredResult_dst->CellMeasuredResultCount = p_MeasuredResult_src->CellMeasuredResultCount;
         if ( p_MeasuredResult_src->CellMeasuredResultCount != 0 )
         {
            // CellMeasuredResults ::= SEQUENCE {
            //    cellIdentity      INTEGER(0..268435455) OPTIONAL,
            //    modeSpecificInfo
            //       CHOICE {fdd
            //                SEQUENCE {primaryCPICH-Info   PrimaryCPICH-Info,
            //                         cpich-Ec-N0          CPICH-Ec-N0 OPTIONAL,
            //                         cpich-RSCP           CPICH-RSCP OPTIONAL,
            //                         pathloss             Pathloss OPTIONAL},
            //               tdd
            //                SEQUENCE {cellParametersID    CellParametersID,
            //                         proposedTGSN         TGSN OPTIONAL,
            //                         primaryCCPCH-RSCP    PrimaryCCPCH-RSCP OPTIONAL,
            //                         pathloss             Pathloss OPTIONAL,
            //                         timeslotISCP-List    TimeslotISCP-List OPTIONAL}}}

            U1 count;
            p_MeasuredResult_dst->p_CellMeasuredResultsList = GN_Calloc( p_MeasuredResult_src->CellMeasuredResultCount, sizeof( s_CellMeasuredResult ) );
            for ( count = 0 ; count < p_MeasuredResult_dst->CellMeasuredResultCount ; count++ )
            {
               s_CellMeasuredResult* p_CellMeasuredResult_dst = &p_MeasuredResult_dst->p_CellMeasuredResultsList[count] ;
               s_CellMeasuredResult* p_CellMeasuredResult_src = &p_MeasuredResult_src->p_CellMeasuredResultsList[count] ;
               
               p_CellMeasuredResult_dst->cellIdentity         = p_CellMeasuredResult_src->cellIdentity ;
               p_CellMeasuredResult_dst->modeSpecificInfoType = p_CellMeasuredResult_src->modeSpecificInfoType ;
               switch ( p_CellMeasuredResult_src->modeSpecificInfoType )
               {
               case fdd:                 // Frequency Division Duplexed mode
                  p_CellMeasuredResult_dst->of_type.fdd.PrimaryScramblingCode = p_CellMeasuredResult_src->of_type.fdd.PrimaryScramblingCode ;
                  p_CellMeasuredResult_dst->of_type.fdd.cpich_Ec_N0           = p_CellMeasuredResult_src->of_type.fdd.cpich_Ec_N0 ;
                  p_CellMeasuredResult_dst->of_type.fdd.cpich_RSCP            = p_CellMeasuredResult_src->of_type.fdd.cpich_RSCP ;
                  p_CellMeasuredResult_dst->of_type.fdd.pathloss              = p_CellMeasuredResult_src->of_type.fdd.pathloss ;
                  break;
               case tdd:                 // Time Division Duplexed mode
                  p_CellMeasuredResult_dst->of_type.tdd.cellParametersID      = p_CellMeasuredResult_src->of_type.tdd.cellParametersID ;
                  p_CellMeasuredResult_dst->of_type.tdd.proposedTGSN          = p_CellMeasuredResult_src->of_type.tdd.proposedTGSN ;
                  p_CellMeasuredResult_dst->of_type.tdd.primaryCCPCH_RSCP     = p_CellMeasuredResult_src->of_type.tdd.primaryCCPCH_RSCP ;
                  p_CellMeasuredResult_dst->of_type.tdd.pathloss              = p_CellMeasuredResult_src->of_type.tdd.pathloss ;
                  p_CellMeasuredResult_dst->of_type.tdd.timeslotISCP_Count    = p_CellMeasuredResult_src->of_type.tdd.timeslotISCP_Count ;
                  if ( p_CellMeasuredResult_src->of_type.tdd.timeslotISCP_Count > 0 )
                  {
                     U1 TimeslotISCPCount ;

                     p_CellMeasuredResult_dst->of_type.tdd.p_TimeslotISCP_List = GN_Calloc( p_CellMeasuredResult_src->of_type.tdd.timeslotISCP_Count, sizeof (U1) );
                     for ( TimeslotISCPCount = 0 ; TimeslotISCPCount < p_CellMeasuredResult_src->of_type.tdd.timeslotISCP_Count ; TimeslotISCPCount++ )
                     {
                        p_CellMeasuredResult_dst->of_type.tdd.p_TimeslotISCP_List[TimeslotISCPCount] = p_CellMeasuredResult_src->of_type.tdd.p_TimeslotISCP_List[TimeslotISCPCount] ;
                     }
                  }
                  else
                  {
                     p_CellMeasuredResult_dst->of_type.tdd.p_TimeslotISCP_List = NULL;
                  }
                  break;
               default:
                  Status = FALSE;
                  break;
               }
            }
         }
         p_MeasuredResult_dst->FrequencyInfoPresent = p_MeasuredResult_src->FrequencyInfoPresent;
         if ( p_MeasuredResult_src->FrequencyInfoPresent )
         {
            p_MeasuredResult_dst->FrequencyInfo.modeSpecificInfoType = p_MeasuredResult_src->FrequencyInfo.modeSpecificInfoType;
            switch ( p_WCDMA_src->p_MeasuredResultsList->FrequencyInfo.modeSpecificInfoType )
            {
            case fdd:   // Frequency Division Duplexed mode
               p_MeasuredResult_dst->FrequencyInfo.of_type.fdd.uarfcn_UL = p_MeasuredResult_src->FrequencyInfo.of_type.fdd.uarfcn_UL;
               p_MeasuredResult_dst->FrequencyInfo.of_type.fdd.uarfcn_DL = p_MeasuredResult_src->FrequencyInfo.of_type.fdd.uarfcn_DL;
               break;
            case tdd:   // Time Division Duplexed mode
               p_MeasuredResult_dst->FrequencyInfo.of_type.tdd.uarfcn_Nt = p_MeasuredResult_src->FrequencyInfo.of_type.tdd.uarfcn_Nt;
               break;
            default:
               Status = FALSE;
               break;
            }
         }
      }
   }
   else
   {
      p_WCDMA_dst->p_MeasuredResultsList = NULL;
      p_WCDMA_dst->MeasuredResultCount = 0;
   }

   return Status;
}
