//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_log.c
//
//*************************************************************************
///
/// \ingroup supl_module
///
/// \file
/// \brief
///      Handles logging of supl modules to the host system
///
///
///
//*************************************************************************
#include <string.h>  //memset
#include <stdio.h>
#include <stdarg.h>
#if AGPS_LINUX_FTR
#include <time.h>
#endif
#include "GN_GPS_api.h"
#include "GN_SUPL_api.h"
#include "supl_instance.h"
#include "supl_log.h"
#include "supl_config.h"
#include "GAD_Conversions.h"
#include "supl_helper.h"

#ifdef AGPS_LINUX_FTR
#define _vsnprintf   vsnprintf  // _vsnprintf is private static in linux c runtime
#endif

//*****************************************************************************
/// \brief
///      Logs SUPL Event messages.
///
///      Sends 'C' formatted debug Log messages to the
///      GN_GPS_Event_Log channel and appends 'CR' and 'LF.
///
/// \note
///      The total message length must be less than 250 characters long.
/// \returns
///      Nothing.
//*****************************************************************************
void GN_SUPL_Log( const char *format, ... )
{
   CH       buff[256];
   CH       *formattedLine = buff;
   U2       prefixSize = 0;
   CH       postFix[] = "\x0d\x0a";
   va_list  arg_list;
   I2       i;
   I2       buffSize = sizeof( buff );
   I2       ret;

   // Add a header of "~ OS_Time_ms " to distinguish these Host Wrapper Event
   // Logs from the GN_GPS_Lib internal Event Logs.
   i = sprintf( buff, "@ %6u ", GN_GPS_Get_OS_Time_ms() );

   prefixSize = i;

   buffSize -= prefixSize ; // Take away the size of the timestamp from the available buffer size.
   buffSize -= sizeof( postFix ) ; // Allow 3 extra to add trailing CR/LF at the end.

   // Format the user string.
   va_start( arg_list, format );
   ret = _vsnprintf( ( formattedLine + i ), buffSize, format, arg_list );
   va_end( arg_list );

   if ( ret >= 0 &&        // if no error and...
        ret <  buffSize )  // if the size of the generated string is less than the available size
   {
      // identify the end of the current string.
      i = i + ret ;
   }
   else if ( ( ret + (I2) sizeof( postFix ) ) > buffSize )

   {
      // The amount required was greater than the available buffer, so we make a buffer big enough.
      buffSize = prefixSize + ret + sizeof( postFix );

      // if the size of the generated string would be greater than the available size
      formattedLine = GN_Calloc( 1,  buffSize );

      if ( formattedLine != NULL )
      {
         // copy over prefix
         strncpy( formattedLine, buff, prefixSize );

         buffSize -= prefixSize ; // Take away the size of the timestamp from the available buffer size.
         buffSize -= sizeof( postFix ) ; // Allow 3 extra to add trailing CR/LF at the end.

         // Format the user string.
         va_start( arg_list, format );
         ret = _vsnprintf( ( formattedLine + i ), buffSize, format, arg_list );
         va_end( arg_list );
         if ( ret >= 0 &&     // if no error and...
              ret < buffSize ) // if the size of the generated string is less than the available size
         {
            // identify the end of the current string.
            i = i + ret ;
         }
      }
      else // GN_Calloc failed
      {
         formattedLine = buff;
         ret = -1;
      }
   }
   if ( ret < 0 ||      // if _vsnprintf or GN_Calloc could not be made to work properly dump an error.
        ret > buffSize )
   {
      // If an error occured
      strcpy( formattedLine + prefixSize, "GN_SUPL_Log:--- Error formatting log output. ---" );
      i = (I2) strlen( formattedLine );
   }

   // Append a <CR><LF> & NULL Terminate
   formattedLine[i]   = postFix[0];
   formattedLine[i+1] = postFix[1];
   formattedLine[i+2] = postFix[2];

   // Send the String to the GloNav Event Log channel.
   (void)GN_SUPL_Write_Event_Log( (U2) ( i + 2 ), (CH*) formattedLine );

   if ( formattedLine != buff )
   {
      // we must have allocated the buffer so free it.
      GN_Free( formattedLine );
   }

   return;
}

void* GN_SUPL_GetHandleFromRRLPHandle( void* handle )
{
    s_SUPL_Instance *p_Instance;
    p_Instance = SUPL_Instance_From_POS_Handle( handle );

    if( NULL != p_Instance )
    {
        return p_Instance->Handle;
    }
    else return NULL;
}


#ifdef SUPL_CMCC_LOGGING_ENABLE
static BL IsLoggingAllowed( void *handle )
{
    s_SUPL_Instance *p_Instance = SUPL_Instance_From_Handle( handle );

    if( NULL != p_Instance )
    {
        return ( !p_Instance->p_SUPL->PrivacyMode );
    }
    else
    {
        GN_SUPL_Log("Invalid Handle : %p" , handle );
        return FALSE;
    }
}
#endif /* SUPL_CMCC_LOGGING_ENABLE */


/*
CMCC Appendix B Log Format
The log information is used for customer service.
Each session log ‘entry contains multiple lines, and the format of each line is as follows:
LINE ::= [UTC_TIME]CODE:(TEXT_LOG)+ (NOTES)*

-The format of UTC_TIME is yyyymmddhhsss.ss (e.g. 2006101023300.10 denotes 23:06:6, 10th, October, 2006. Note:“[“ and “]” is the content of the log, see example.
-The CODE format is 0xHHHHHHHH (4 bytes). CODE defines 0. Each H is a number of 16 number system ranging from 0 to F. Note: “:” is the log content, see example.
-The TEXT_LOG is optional and parameters are differentiated by commas. Note: () out of TEXT_LOG + non-log content, denoting one or more TEXT_LOG elements.
-NOTES are explanation texts beginning from “#. They may also be “meaning” field in the table. Note: ()* non-log content, denoting 0~1 element.

Table 10-1 CODE definition and TEXT LOG
CODE    Notes    Explanations    Allowed TEXT_LOG
The first 4 bytes of CODE denote the log type:
“0” denotes session status update
“1” denotes warning (warning, AGPS session resume)
“2” denotes errors (errors, AGPS session may end)

The last 4 bytes of CODE denotes the log source:
“0” denotes correlation with AGPS software
“1” denotes correlation with AGPS platform
“2” denotes correlation with OTA messages (SUPL/RRLP)
“3” denoted correlation with GPS chips
Other values are vendor specific origins*

So definable tables.

CODE    Notes    Explanations    Allowed TEXT_LOG

Status Updates (0x0HHHHHHHHHHHHHHH)
0x00000000    A new AGPS Session is started    New AGPS session starts    local_session_num


0x00000001    An AGPS session has ended    AGPS session ends    local_session_num
0x00000002    Session QoP (Single or Periodic)    Session QoP (single positioning or periodic positioning)    local_session_num, hor_acc(in meters), ver_acc (in meters), resp_time(in seconds), num_fixes, time_btw_fixes
0x00000003    Position Result    Positioning results    local_session_num, Time_stamp_of_result, lat, long, unc_maj, unc_min, orientation, height, height_unc
0x00000004    Eph Aiding Sent to GPS    Assistant information sent to the GPS acceptor    local_session_num, num_sat, gps_week, toe, sat_num1, sat_num2, …., sat_numN
0x00000005    AGPS SW version numbers    AGPS software version number    local_session_num, version_text_string1,  version_text_string2,  ...
0x02000000    OTA message type    OTA message type    local_session_num, SUPL_version,  {SUPL_INIT|SUPL_POS|SUPL_POS_INIT|SUPL_START|SUPL_RESPONSE|SUPL_POS|SUPL_END}


Warning Message (0x1HHHHHHHHHHHHHHH)
0x10000000    SET-Initiated session,  no network connection,  default to standalone.    Positioning session initiated by the terminal, network unable to be connected, switched to autonomous positioning mode.    local_session_num
0x10000000    Unknown OTA message received, discard.    Unknown OTA message, not to be processed    local_session_num
0x10000000    Unknown GPS message received, discard.    Unknown GPS message, not to be processed    local_session_num
0x13000000    GPS cannot produce a position within resp_time.    GPS cannot calculate the location in the requested resp_time.    local_session_num


Error Message (0x2HHHHHHHHHHHHHHH)
(Sent when the session cannot go on anymore)
0x20000000    Unexpected OTA message received. FSM out-of-sync.    OTA message incorrect. FSM not synchronous    local_session_num
0x20000001    SET-Initiated Session: wrong input configuration    Positioning session initiated by the terminal, network unable to be connected, switched to autonomous positioning mode.    local_session_num
0x21000000    Network Connection Failure    Network connection failed    local_session_num
0x21000001    Server connection failure    Server connection failed    local_session_num
0x21000002    Connection to GPS lost during AGPS session,  or at GPS reset.    In a AGPS session or GPS failed to restart GPS acceptor connection    local_session_num
0x21000003    TLS Failure during SUPL    TLS failed    local_session_num
0x22000000    SUPL END Status Code (When error occurs)    SUPL END message containing error status code received    local_session_num, SUPL_Status_Code
0x22000001    OTA Message Time-Out    OTA message (SUPL message) overtime    local_session_num
0x22000002    OTA Message Decoding Error    OTA message (SUPL message) decoding wrong    local_session_num
0x23000000    GPS Message Time-Out    Message sent by GPS acceptor overtime    local_session_num


Note*: let terminal manufacturers to communicate with operator to insure the error codes not conflict with each other.
Example:
[2006101023300.10] 0x00000000: 2,                                                            # session starts
[2006101023300.10] 0x00000005: 2,LPL-3.0.26a,SLC-3.3.1b,     # version numbers
[2006101023301.10] 0x00000002: 2,100,300,60,1,0,                                   # QoP
[2006101023302.10] 0x02000000: 2,1.0,SUPL_START
[2006101023310.10] 0x02000000: 2,1.0,SUPL_RESPONSE
[2006101023311.10] 0x02000000: 2,1.0,SUPL_POS_INIT
[2006101023315.10] 0x02000000: 2,1.0,SUPL_POS
[2006101023316.10] 0x00000004: 2,8,1440,450,2,5,7,9,12,15,21,28,         # Eph Aiding to GPS
[2006101023318.10] 0x00000004: 2, 2006101023316.11,37.374260,-121.911520,20,20,115,52,20,                                                       # position
[2006101023315.10] 0x02000000: 2,1.0,SUPL_POS
[2006101023315.10] 0x02000000: 2,1.0,SUPL_END
[2006101023315.10] 0x00000001: 2,                       # session end

*/
#ifdef SUPL_CMCC_LOGGING_ENABLE

/* Status Logs */
#define GN_SUPL_LOG_CMCC_AGPS_SESSION_STARTED    0x00000000
#define GN_SUPL_LOG_CMCC_AGPS_SESSION_ENDED      0x00000001
#define GN_SUPL_LOG_CMCC_AGPS_SESSION_QOP        0x00000002
#define GN_SUPL_LOG_CMCC_AGPS_SESSION_POS_RESULT 0x00000003
#define GN_SUPL_LOG_CMCC_AGPS_AIDING_RECVD       0x00000004
#define GN_SUPL_LOG_CMCC_AGPS_SW_VERSION         0x00000005
#define GN_SUPL_LOG_CMCC_SUPL_NETWORK_CONNECT    0x01000000
#define GN_SUPL_LOG_CMCC_SUPL_SERVER_CONNECT     0x01000001
#define GN_SUPL_LOG_CMCC_SUPL_MSG_TYPE           0x02000000

/* Warning Logs */
#define GN_SUPL_LOG_CMCC_WARNING                 0x10000000
#define GN_SUPL_LOG_CMCC_POS_FAILURE             0x13000000


/* Error Logs */
#define GN_SUPL_LOG_CMCC_ERROR_WRONG_SUPL_MSG    0x20000000
#define GN_SUPL_LOG_CMCC_ERROR_SI_CONFIG         0x20000001
#define GN_SUPL_LOG_CMCC_ERROR_NO_NETWORK        0x21000000
#define GN_SUPL_LOG_CMCC_ERROR_NO_SERVER         0x21000001
#define GN_SUPL_LOG_CMCC_ERROR_SERVER_DISCONNECT 0x21000003
#define GN_SUPL_LOG_CMCC_ERROR_SUPL_END_STATUS   0x22000000
#define GN_SUPL_LOG_CMCC_ERROR_SUPL_TIMEOUT      0x22000001
#define GN_SUPL_LOG_CMCC_ERROR_SUPL_DECODE       0x22000002

#endif /* #ifdef SUPL_CMCC_LOGGING_ENABLE */

char*    PosMethodAsString[] =
{
    "NI MSA" , //agpsSETassisted1 0
    "NI MSB" , //agpsSETbased1 1
    "NI MSAPref" , //agpsSETassistedpref1 2
    "NI MSBPref" , //agpsSETbasedpref1 3
    "NI Autonomous" , //autonomousGPS1 4
    "NI AFLT" , //aFLT1 5
    "NI ECellId" , //eCID1 6
    "NI EOTD" , //eOTD1 7
    "NI OTDOA" , //oTDOA1 8
    "NI No Position" //noPosition1 9
};

char*  PrefPosMethodAsString[] =
{
    "SI MSA" , //agpsSETassistedPreferred1 0
    "SI MSB" , //agpsSETBasedPreferred1 1
    NULL , //noPreference1 2
};

static void GN_SUPL_Log_SuplMessage( void* handle , void  *p_PDU , BL encode )
{
   char     *PDUType = "Undetermined";
   char        *PDUTypeCMCC = "Undetermined";
   char     *ExtraLoggingCMCC = NULL;

   e_GN_UlpMessageType suplMessage = supl_PDU_Get_MessageType(p_PDU);

   handle = handle;

   // Just for logging the PDU type encoded.
   switch ( suplMessage )
   {
   case GN_UlpMessage_PR_msSUPLINIT:
   {
      e_GN_PosMethod posMethod;

      PDUType = "msSUPLINIT";
      PDUTypeCMCC = "SUPL_INIT";

      supl_PDU_Get_SUPLINIT_PosMethod(p_PDU , &posMethod );

      ExtraLoggingCMCC = PosMethodAsString[ posMethod ];
   }
   break;
   case GN_UlpMessage_PR_msSUPLSTART:
   {
      PDUType = "msSUPLSTART";
      PDUTypeCMCC = "SUPL_START";
      ExtraLoggingCMCC = PrefPosMethodAsString[ supl_PDU_Get_SUPLSTART_PreferredMethod(p_PDU) ];
   }
   break;
   case GN_UlpMessage_PR_msSUPLRESPONSE: PDUType = "msSUPLRESPONSE";    PDUTypeCMCC = "SUPL_RESPONSE";break;
   case GN_UlpMessage_PR_msSUPLPOSINIT:  PDUType = "msSUPLPOSINIT";    PDUTypeCMCC = "SUPL_POS_INIT";break;
   case GN_UlpMessage_PR_msSUPLPOS:      PDUType = "msSUPLPOS";        PDUTypeCMCC = "SUPL_POS";break;
   case GN_UlpMessage_PR_msSUPLEND:      PDUType = "msSUPLEND";        PDUTypeCMCC = "SUPL_END";break;
   case GN_UlpMessage_PR_msSUPLAUTHREQ:  PDUType = "msSUPLAUTHREQ";    PDUTypeCMCC = "SUPL_AUTH_REQ";break;
   case GN_UlpMessage_PR_msSUPLAUTHRESP: PDUType = "msSUPLAUTHRESP";    PDUTypeCMCC = "SUPL_AUTH_RESP";break;
   /* SUPL-2.0 Specific Messages */
   /* The CMCC specific logging is just a place holder. CMCC has not defined any as of now */
   case GN_UlpMessage_PR_NOTHING:                  PDUType = "Unknown";                PDUTypeCMCC = "Unknown";break;
   case GN_UlpMessage_PR_msSUPLNOTIFY:             PDUType = "msSUPLNOTIFY";          PDUTypeCMCC = "SUPL_NOTIFY";break;
   case GN_UlpMessage_PR_msSUPLNOTIFYRESPONSE:     PDUType = "msSUPLNOTIFYRESPONSE";PDUTypeCMCC = "SUPL_NOTIFY_RESPONSE";break;
   case GN_UlpMessage_PR_msSUPLREPORT:             PDUType = "msSUPLREPORT";          PDUTypeCMCC = "SUPL_REPORT";break;
   case GN_UlpMessage_PR_msSUPLTRIGGEREDSTART:     PDUType = "msSUPLTRIGGEREDSTART";PDUTypeCMCC = "SUPL_TRIGGERED_START";break;
   case GN_UlpMessage_PR_msSUPLTRIGGEREDRESPONSE:  PDUType = "msTRIGGEREDRESPONSE";    PDUTypeCMCC = "SUPL_TRIGGERED_RESPONSE";break;
   case GN_UlpMessage_PR_msSUPLTRIGGEREDSTOP:      PDUType = "msSUPLTRIGGEREDSTOP";    PDUTypeCMCC = "SUPL_TRIGGERED_STOP";break;
   case GN_UlpMessage_PR_msSUPLSETINIT:            PDUType = "msSUPLSETINIT";          PDUTypeCMCC = "SUPL_SETINIT";break;
   }

   if( encode )
      GN_SUPL_Log( "GN_SUPL_asn1_encode:    --- [label=\"Encode SUPL PDU %s\"];", PDUType );
   else
      GN_SUPL_Log( "GN_SUPL_asn1_decode:    --- [label=\"Decode SUPL PDU %s\"];", PDUType );

#ifdef SUPL_CMCC_LOGGING_ENABLE
   if( IsLoggingAllowed(handle) )
   {
      U1       majorVersion,MinorVersion,serviceIndicator;
      char     textLog[64] = "";

      supl_PDU_Get_Version(p_PDU,&majorVersion,&MinorVersion,&serviceIndicator );

      sprintf(textLog,"%d,%d.%d.%d,%s",(int)handle,majorVersion,MinorVersion,serviceIndicator,PDUTypeCMCC);
      GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_SUPL_MSG_TYPE , textLog , ExtraLoggingCMCC );
   }
#endif
}

void GN_SUPL_Log_SuplMessageDecoded( void* handle , void* p_PDU )
{
    GN_SUPL_Log_SuplMessage( handle , p_PDU , FALSE );
}

void GN_SUPL_Log_SuplMessageEncoded( void* handle ,  void* p_PDU )
{
    GN_SUPL_Log_SuplMessage( handle , p_PDU , TRUE );
}

void GN_SUPL_Log_QoP(void* handle , s_GN_QoP* p_GN_QoP )
{
    handle   = handle;
    p_GN_QoP = p_GN_QoP;

#ifdef SUPL_CMCC_LOGGING_ENABLE
    if( IsLoggingAllowed(handle) )
    {
      /* First check if QoP is present and set to the appropriate values */
      if( p_GN_QoP != NULL )
      {
         CH  textLog[128];
         I4  horacc = -1 , veracc = -1 , delay = -1;

         /* If a particular field has no value, then zero is assumed */

         if( p_GN_QoP->p_horacc != NULL )
         {
            /* hor_acc_in_m = 10*( 1.1^k -1 ) */
            horacc = (I4)10*( pow( 1.1 , *p_GN_QoP->p_horacc ) - 1 );
         }
         if( p_GN_QoP->p_veracc != NULL )
         {
            /* ver_acc_in_m = 45*( 1.025^k - 1 ) */
            veracc = (I4)45*( pow(1.025,*p_GN_QoP->p_veracc) - 1 );
         }
         if( p_GN_QoP->p_delay != NULL )
         {
            delay = (I4)pow( 2 , *p_GN_QoP->p_delay );
         }
           /* local_session_num, hor_acc(in meters), ver_acc (in meters), resp_time(in seconds), num_fixes, time_btw_fixes */
           sprintf( textLog , "%d,%ld,%ld,%ld,1,0",
                   (int)handle,
                   horacc,
                   veracc,
                   delay);

           GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_AGPS_SESSION_QOP , textLog , "QoP" );
      }
    }
#endif
}

void GN_SUPL_Log_SuplEndStatus( void* handle , e_GN_StatusCode statusCode )
{
    CH *p_StatusCode;

    handle   = handle;

    switch ( statusCode )
    {
        case GN_StatusCode_unspecified:                p_StatusCode = "unspecified";                break;
        case GN_StatusCode_systemFailure:              p_StatusCode = "systemFailure";              break;
        case GN_StatusCode_unexpectedMessage:          p_StatusCode = "unexpectedMessage";          break;
        case GN_StatusCode_protocolError:              p_StatusCode = "protocolError";              break;
        case GN_StatusCode_dataMissing:                p_StatusCode = "dataMissing";                break;
        case GN_StatusCode_unexpectedDataValue:        p_StatusCode = "unexpectedDataValue";        break;
        case GN_StatusCode_posMethodFailure:           p_StatusCode = "posMethodFailure";           break;
        case GN_StatusCode_posMethodMismatch:          p_StatusCode = "posMethodMismatch";          break;
        case GN_StatusCode_posProtocolMismatch:        p_StatusCode = "posProtocolMismatch";        break;
        case GN_StatusCode_targetSETnotReachable:      p_StatusCode = "targetSETnotReachable";      break;
        case GN_StatusCode_versionNotSupported:        p_StatusCode = "versionNotSupported";        break;
        case GN_StatusCode_resourceShortage:           p_StatusCode = "resourceShortage";           break;
        case GN_StatusCode_invalidSessionId:           p_StatusCode = "invalidSessionId";           break;
        case GN_StatusCode_nonProxyModeNotSupported:   p_StatusCode = "nonProxyModeNotSupported";   break;
        case GN_StatusCode_proxyModeNotSupported:      p_StatusCode = "proxyModeNotSupported";      break;
        case GN_StatusCode_positioningNotPermitted:    p_StatusCode = "positioningNotPermitted";    break;
        case GN_StatusCode_authNetFailure:             p_StatusCode = "authNetFailure";             break;
        case GN_StatusCode_authSuplinitFailure:        p_StatusCode = "authSuplinitFailure";        break;
        case GN_StatusCode_consentDeniedByUser:        p_StatusCode = "consentDeniedByUser";        break;
        case GN_StatusCode_consentGrantedByUser:       p_StatusCode = "consentGrantedByUser";       break;
        case GN_StatusCode_ver2_sessionStopped:        p_StatusCode = "ver2_sessionStopped";        break;
        default:                   p_StatusCode = "*** statusCode value not recognised ***";        break;
    }
    GN_SUPL_Log( "GN_SUPL_asn1_encode: SUPL-END with statusCode: %s", p_StatusCode );

#ifdef SUPL_CMCC_LOGGING_ENABLE
    /* local_session_num, SUPL_Status_Code */
    if( IsLoggingAllowed(handle) )
    {
        CH textLog[24];
        CH notes[48];

        sprintf( textLog , "%d,%d" , (int)handle , statusCode );
        if( statusCode == GN_StatusCode_unspecified )
        {
            sprintf( notes , "SUPLEND" );
        }
        else
        {
            sprintf( notes , "SUPLEND error code %s" , p_StatusCode );
        }
        GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_ERROR_SUPL_END_STATUS , textLog , notes );
    }
#endif
}

#ifdef SUPL_CMCC_LOGGING_ENABLE
void GN_SUPL_Log_CMCC_PosReport( void* handle ,s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data , U4 msTTFF )
{
    CH                  textLog[512];
    CH                  notesLog[256];
    /* s_GN_GPS_UTC_Data   p_UTC; */
    /* BL                  bUtcReady; */
    I2                  altitude = 0;
    R4                  TempLat, TempLong;
    U2                    orientation = 0;
    s_GN_GPS_Time_Data  timeData;


    // Calculate Latitude in degrees.
    TempLat = (R4) ( p_GN_AGPS_GAD_Data->latitude & GAD_LATITUDE_RANGE_MASK ) /
                       GAD_LATITUDE_SCALING_FACTOR;
    TempLat *= (R4) GAD_LATITUDE_DEGREES_IN_RANGE *
                       ( p_GN_AGPS_GAD_Data->latitudeSign == 0 ? 1 : -1 );

    // Calculate Longitude in degrees.
    TempLong = (R4) ( p_GN_AGPS_GAD_Data->longitude / (R4) GAD_LONGITUDE_SCALING_FACTOR );
    TempLong *= (R4) GAD_LONGITUDE_DEGREES_IN_RANGE;

    /* bUtcReady = GN_GPS_Read_UTC(&p_UTC); */

   (void)GN_GPS_Get_Time_Data( &timeData );

    if( p_GN_AGPS_GAD_Data->altitudeDirection != -1 )
    {
        altitude = p_GN_AGPS_GAD_Data->altitudeDirection ? (-1*p_GN_AGPS_GAD_Data->altitude) : p_GN_AGPS_GAD_Data->altitude ;
    }

    if( p_GN_AGPS_GAD_Data->orientationMajorAxis != 255 )
    {
        orientation = 2*p_GN_AGPS_GAD_Data->orientationMajorAxis;
    }
    /* local_session_num, Time_stamp_of_result, lat, long, unc_maj, unc_min, orientation, height, height_unc */
    /* Time stamp format : sprintf(strCmccLog, "%04d%02d%02d%02d%02d%02d.%02d",
                p_UTC.Year, p_UTC.Month, p_UTC.Day, p_UTC.Hours, p_UTC.Minutes,
                p_UTC.Seconds, p_UTC.Milliseconds
                ); */
        sprintf( textLog , "%d,%04d%02d%02d%02d%02d%02d.%02d,%.06lf,%.06lf,%u,%d",
                        (int)handle ,
/*                        p_UTC.Year, p_UTC.Month, p_UTC.Day, p_UTC.Hours,
                        p_UTC.Minutes,p_UTC.Seconds, p_UTC.Milliseconds, */
                  timeData.Year , timeData.Month, timeData.Day, timeData.Hours,
                        timeData.Minutes,timeData.Seconds, timeData.Milliseconds ,
                        TempLat, TempLong,
                        orientation , altitude );

    sprintf( notesLog , "Position(Timestamp, lat, lon, orientation, height)  time to fix : %d ms" , msTTFF );

    GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_AGPS_SESSION_POS_RESULT , textLog , notesLog );
}

#endif



void GN_SUPL_Log_SessionStarted( void* handle )
{
    GN_SUPL_Log("GN_SUPL:    --- [label=\"Handle Created %p\"];", handle );
#ifdef SUPL_CMCC_LOGGING_ENABLE
    if( IsLoggingAllowed(handle) )
    {
        CH buff[276] = {0};
        CH *p_Version = NULL;

        /*Handle copied to buffer*/
        sprintf( buff, "%d" , (int)handle );

        /*Session started string provided for CMCC logging*/

        /*Platform version is obtained here*/
        p_Version = supl_config_get_platform_version();

        /*Check whether platform version available or not*/
        if( NULL != p_Version )
        {
            /*Platform version available: Copy handle and platform version to buffer*/
            sprintf( buff , "%d, %s", (int)handle,  p_Version );

            /*Platform version string provided for CMCC logging*/
            GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_AGPS_SW_VERSION, buff, "version numbers" );
        }
        else
        {
            /*Platform version not available: Copy handle only to buffer*/
            sprintf(buff, "%d, No Version", (int)handle );

            /*Platform version not available string is provided for CMCC logging*/
            GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_AGPS_SW_VERSION , buff  , "version numbers" );
        }
    }
#endif
}

void GN_SUPL_Log_SessionEnded( void* handle  )
{
    GN_SUPL_Log( "GN_SUPL:    --- [label=\"Handle Deleted %p\"];", handle );
#ifdef SUPL_CMCC_LOGGING_ENABLE
    if( IsLoggingAllowed(handle) )
    {
        CH local_session[20];

        sprintf( local_session , "%d" , (int)handle );
     }
#endif
}

void GN_SUPL_Log_PduIgnored( void* handle )
{
    GN_SUPL_Log( "GN_SUPL:    --- [label=\"PDU Ignored %p\"];", handle );
#ifdef SUPL_CMCC_LOGGING_ENABLE
    if( IsLoggingAllowed(handle) )
    {
        CH local_session[20];
        sprintf( local_session , "%d" , (int)handle );
        GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_WARNING , local_session , "PDU Ignored" );
    }
#endif
}

void GN_SUPL_Log_NoNetworkResource( void *handle )
{
    GN_SUPL_Log( "GN_SUPL:    --- [label=\"PDP Attach Failed %p\"];", handle );
}

void GN_SUPL_Log_ServerDisconnected( void *handle )
{
    GN_SUPL_Log( "GN_SUPL:    --- [label=\"Server Disconnected%p\"];", handle );
}

void GN_SUPL_Log_ServerConnected( void *handle )
{
    GN_SUPL_Log( "GN_SUPL:    --- [label=\"PDP Attach Done%p\"];", handle );
    GN_SUPL_Log( "GN_SUPL:    --- [label=\"Server Connected%p\"];", handle );
}

void GN_SUPL_Log_ServerNotFound( void *handle )
{
    GN_SUPL_Log( "GN_SUPL:    --- [label=\"PDP Attach Done%p\"];", handle );
    GN_SUPL_Log( "GN_SUPL:    --- [label=\"Server Not Found%p\"];", handle );
}

void GN_SUPL_Log_MsgTimerExpiry( void *handle , char* timerName )
{
    handle    = handle;
    timerName = timerName;

#ifdef SUPL_CMCC_LOGGING_ENABLE
    if( IsLoggingAllowed(handle) )
    {
        CH local_session[24];
        CH local_notes[48];
        sprintf( local_session , "%d" , (int)handle );
        sprintf( local_notes , "SUPL Timer %s Expired" , timerName );
        GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_ERROR_SUPL_TIMEOUT , local_session , local_notes );
    }
#endif
}

void GN_SUPL_Log_MsgDecodeFailed( void* handle )
{
   handle = handle;
   GN_SUPL_Log("GN_SUPL_asn1_decode:    --- [label=\"Failed to decode SUPL PDU%p\"];", handle);
#ifdef SUPL_CMCC_LOGGING_ENABLE
    if( IsLoggingAllowed(handle) )
    {
        CH local_session[20];
        sprintf( local_session , "%d" , (int)handle );
        GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_ERROR_SUPL_DECODE , local_session , "SUPL Decode Failure" );
    }
#endif
}

void GN_SUPL_Log_MsgOutOfOrder( void* handle , void *p_PDU )
{
   char  *PDUType     = "Undetermined";
   char  *PDUTypeCMCC = "Undetermined";
   e_GN_UlpMessageType suplMessage;

   handle = handle;

   suplMessage = supl_PDU_Get_MessageType( p_PDU );

   // Just for logging the PDU type encoded.
   switch ( suplMessage )
   {
       case GN_UlpMessage_PR_msSUPLINIT:     PDUType = "msSUPLINIT";       PDUTypeCMCC = "SUPL_INIT";  break;
       case GN_UlpMessage_PR_msSUPLSTART:    PDUType = "msSUPLSTART";      PDUTypeCMCC = "SUPL_START";  break;
       case GN_UlpMessage_PR_msSUPLRESPONSE: PDUType = "msSUPLRESPONSE";   PDUTypeCMCC = "SUPL_RESPONSE";break;
       case GN_UlpMessage_PR_msSUPLPOSINIT:  PDUType = "msSUPLPOSINIT";    PDUTypeCMCC = "SUPL_POS_INIT";break;
       case GN_UlpMessage_PR_msSUPLPOS:      PDUType = "msSUPLPOS";        PDUTypeCMCC = "SUPL_POS";break;
       case GN_UlpMessage_PR_msSUPLEND:      PDUType = "msSUPLEND";        PDUTypeCMCC = "SUPL_END";break;
       case GN_UlpMessage_PR_msSUPLAUTHREQ:  PDUType = "msSUPLAUTHREQ";    PDUTypeCMCC = "SUPL_AUTH_REQ";break;
       case GN_UlpMessage_PR_msSUPLAUTHRESP: PDUType = "msSUPLAUTHRESP";   PDUTypeCMCC = "SUPL_AUTH_RESP";break;
      /* SUPL-2.0 Specific Messages */
      /* The CMCC specific logging is just a place holder. CMCC has not defined any as of now */
      case GN_UlpMessage_PR_NOTHING:                  PDUType = "Unknown";                PDUTypeCMCC = "Unknown";break;
      case GN_UlpMessage_PR_msSUPLNOTIFY:             PDUType = "msSUPLNOTIFY";           PDUTypeCMCC = "SUPL_NOTIFY";break;
      case GN_UlpMessage_PR_msSUPLNOTIFYRESPONSE:     PDUType = "msSUPLNOTIFYRESPONSE";   PDUTypeCMCC = "SUPL_NOTIFY_RESPONSE";break;
      case GN_UlpMessage_PR_msSUPLREPORT:             PDUType = "msSUPLREPORT";           PDUTypeCMCC = "SUPL_REPORT";break;
      case GN_UlpMessage_PR_msSUPLTRIGGEREDSTART:     PDUType = "msSUPLTRIGGEREDSTART";   PDUTypeCMCC = "SUPL_TRIGGERED_START";break;
      case GN_UlpMessage_PR_msSUPLTRIGGEREDRESPONSE:  PDUType = "msTRIGGEREDRESPONSE";    PDUTypeCMCC = "SUPL_TRIGGERED_RESPONSE";break;
      case GN_UlpMessage_PR_msSUPLTRIGGEREDSTOP:      PDUType = "msSUPLTRIGGEREDSTOP";    PDUTypeCMCC = "SUPL_TRIGGERED_STOP";break;
      case GN_UlpMessage_PR_msSUPLSETINIT:            PDUType = "msSUPLSETINIT";          PDUTypeCMCC = "SUPL_SETINIT";break;
   }

   GN_SUPL_Log( "SUPL Msg Out of Order %s" , PDUType );

#ifdef SUPL_CMCC_LOGGING_ENABLE
    /* local_session_num, SUPL_Status_Code */
    if( IsLoggingAllowed(handle) )
    {
        CH textLog[24];
        CH notes[64];

        sprintf( textLog , "%d" , (int)handle );
        sprintf( notes , "SUPL Msg Out of Order %s" , PDUTypeCMCC );
        GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_ERROR_WRONG_SUPL_MSG , textLog , notes );
    }
#endif

}





void GN_SUPL_Log_CMCC_CachePosReport( void* handle ,s_GN_Position *p_GN_Position , U4 msTTFF )
{
    handle        = handle;
    p_GN_Position = p_GN_Position;
    msTTFF        = msTTFF;

#ifdef SUPL_CMCC_LOGGING_ENABLE
    {
        CH                  textLog[512];
        CH                  notesLog[256];
        s_GN_GPS_UTC_Data   p_UTC = {0};
        BL                  bUtcReady = 1;
        I2                  altitude = 0;
        R4                  TempLat, TempLong;
        U2                  orientation = 0;
#if AGPS_LINUX_FTR
        U4                 seconds = 0;
        struct timeval current = {0};
#endif

        // Calculate Latitude in degrees.
        TempLat = (R4) ( p_GN_Position->latitude & GAD_LATITUDE_RANGE_MASK ) /
                           GAD_LATITUDE_SCALING_FACTOR;
        TempLat *= (R4) GAD_LATITUDE_DEGREES_IN_RANGE *
                           ( p_GN_Position->latitudeSign == 0 ? 1 : -1 );

        // Calculate Longitude in degrees.
        TempLong = (R4) ( p_GN_Position->longitude / (R4) GAD_LONGITUDE_SCALING_FACTOR );
        TempLong *= (R4) GAD_LONGITUDE_DEGREES_IN_RANGE;

#if AGPS_LINUX_FTR
        if( 0 == gettimeofday( &current , NULL ) )
        {
           seconds = current.tv_sec;

           GN_CTime_To_YMDHMS((U4)seconds,&p_UTC.Year,&p_UTC.Month,&p_UTC.Day,&p_UTC.Hours,&p_UTC.Minutes,
                                 &p_UTC.Seconds);
        }
        else
        {
          bUtcReady = 0;
        }
#else
        bUtcReady = GN_GPS_Read_UTC(&p_UTC);
#endif

        if( p_GN_Position->altitudeInfoPresent )
        {
              if( p_GN_Position->altitudeDirection != -1 )
              {
                    altitude = (p_GN_Position->altitude * GAD_ALTITUDE_RANGE_MASK ) * ( (p_GN_Position ->altitudeDirection == 0) ? 1 : -1 );
              }
        }

        if( p_GN_Position->orientationMajorAxis != 255 )
        {
            orientation = 2 * p_GN_Position->orientationMajorAxis;
        }

        /* local_session_num, Time_stamp_of_result, lat, long, unc_maj, unc_min, orientation, height, height_unc */
        /* Time stamp format : sprintf(strCmccLog, "%04d%02d%02d%02d%02d%02d.%02d",
                    p_UTC.Year, p_UTC.Month, p_UTC.Day, p_UTC.Hours, p_UTC.Minutes,
                    p_UTC.Seconds, p_UTC.Milliseconds
                    ); */
            sprintf( textLog , "%d,%04d%02d%02d%02d%02d%02d.%02d,%.06lf,%.06lf,%u,%d",
                            (int)handle ,
                            p_UTC.Year, p_UTC.Month, p_UTC.Day, p_UTC.Hours,
                            p_UTC.Minutes,p_UTC.Seconds, p_UTC.Milliseconds,
                            TempLat, TempLong,
                            orientation , altitude );

        sprintf( notesLog , "Position(Timestamp, lat, lon, orientation, height)  time to fix : %d ms" , msTTFF );

        GN_SUPL_Write_Event_Log_CMCC( GN_SUPL_LOG_CMCC_AGPS_SESSION_POS_RESULT , textLog , notesLog );
    }
#endif  /* SUPL_CMCC_LOGGING_ENABLE */
}


void GN_SUPL_Log_CachePosReport( void* handle , s_GN_Position *p_GN_Position , U4 msTTFF )
{
    handle        = handle;
    p_GN_Position = p_GN_Position;
    msTTFF        = msTTFF;

#ifdef SUPL_CMCC_LOGGING_ENABLE
    {
        s_SUPL_Instance *p_Instance = (s_SUPL_Instance *)handle;

        if( !p_Instance->p_SUPL->PrivacyMode )
        {
            GN_SUPL_Log_CMCC_CachePosReport( (void *)p_Instance->Handle, p_GN_Position , msTTFF );
        }
    }
#endif
}




