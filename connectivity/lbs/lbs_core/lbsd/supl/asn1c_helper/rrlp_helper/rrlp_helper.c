//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
//
// Filename rrlp_helper.c
//
// $Header: X:/MKS Projects/prototype/prototype/asn1_telelogic/rrlp_helper/rcs/rrlp_helper.c 1.55 2009/01/16 14:41:56Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_helper
///
/// \file
/// \brief
///      ASN.1 helper routines implementation.
///
///      Access routines specifically for using the SUPL encode/decode routines
///      in a way useful for the rrlp_agent.
///
//*************************************************************************

#include "os_assert.h"
#include "rrlp_helper.h"
#include "pdu_defs.h"
#include "os_config.h"
#include "GN_GPS_api.h"
#include "GN_AGPS_api.h"
#include "GAD_helper.h"
#include "assist_helper.h"
#include "RRLP_PDU.h"
#include "rrlp_asn1_macros.h"
#include "rrlp_asn1.h"


//PDU
// RRLP_PDU_t RRLP_ASN1_pdu_t;

#define RRLP_NAVIGATION_MODEL_LIMIT 16 ///< From RRLP-Components Size of SeqOfNavModelElement


typedef RRLP_PositionInstruct_t PositionInstruct;
typedef RRLP_GPS_AssistData_t GPS_AssistData;
//typedef ErrorCodes_t ErrorCodes;
void GN_RRLP_Log( const char *format, ... );

#ifdef SUPL_SERVER_FTR
BL rrlp_PDU_Make_GPS_AssistData( RRLP_GPS_AssistData_t *p_GPS_AssistData, s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data, s_GN_RequestedAssistData *p_GN_RequestedAssistData);
#endif

//*****************************************************************************
/// \brief
///      GN GPS API Function to request a copy the latest GPS Navigation solution data.
/// \details
///      GN GPS API Function to request the latest GPS Navigation solution data.
///      The data provided includes everything normally made available in the
///      the commonly used NMEA 183 format GPS sentences.
///      <p> This function should not be used to poll for navigation data.
///      Polling for data should be done with #GN_GPS_Get_Nav_Data which has
///      mechanisms in place to reduce overhead if no new position solution is
///      available.
///      <p> The data returned is double buffered so it is safe to call this
///      function from any OS task or thread.
/// \returns
///      Flag to indicate if there is valid GPS Navigation solution data.
/// \retval #TRUE if a GPS Navigation solution data is available.
/// \retval #FALSE if a GPS Navigation solution data is not available.
//*****************************************************************************
BL GN_GPS_Get_Nav_Data_Copy(
   s_GN_GPS_Nav_Data* p_Nav_Data    ///< [in] Pointer to where the GPS Library should write the Nav Data to.
);


//*****************************************************************************
/// \brief
///      Function for logging a raw PDU Buffer Store.
//*****************************************************************************
void rrlp_PDU_Buffer_Store_Log
(
   CH                   *Log_Prefix,         ///< Prefix of log message.
   s_PDU_Buffer_Store   *p_PDU_Buffer_Store  ///< Address of the PDU_Buffer_Store pointer to free.
)
{
   if ( p_PDU_Buffer_Store != NULL )
   {
      if ( p_PDU_Buffer_Store->PDU_Encoded.p_PDU_Data != NULL &&
           p_PDU_Buffer_Store->PDU_Encoded.Length     != 0       )
      {
         CH* PDU_String ;
         CH* String_Ptr ;
         U2  i ;

         /// Allocate 3 characters to display each of the bytes and a space plus one string terminator.
         PDU_String = GN_Calloc( 1, p_PDU_Buffer_Store->PDU_Encoded.Length * 3 + 1 ) ;

         String_Ptr = PDU_String ;

         /// Loop around for each character and make a printable string of the buffer contents.
         for ( i = 0 ; i < p_PDU_Buffer_Store->PDU_Encoded.Length ; i++ )
         {
            String_Ptr += sprintf( String_Ptr, " %02x", p_PDU_Buffer_Store->PDU_Encoded.p_PDU_Data[i] );
         }

         /// Print out the buffer contents proceded by the prefix and length.
         GN_RRLP_Log( "%s:PDU_Buffer_Store Length: %5u, Data:%s",
            Log_Prefix,
            p_PDU_Buffer_Store->PDU_Encoded.Length,
            PDU_String );

         GN_Free( PDU_String );
      }
      else
      {
         GN_RRLP_Log( "%s:PDU_Buffer_Store Empty",
            Log_Prefix );
      }
   }
   else
   {
      GN_RRLP_Log( "%s:PDU_Buffer_Store Invalid",
         Log_Prefix );
   }
}


//*****************************************************************************
/// \brief
///      Brute force logging of ephemeris in GloNAV elemental format.
///
/// \returns
///      Nothing.
//*****************************************************************************
void GN_AGPS_Log_Eph_El
(
   s_GN_AGPS_Eph_El *p_GN_AGPS_Eph_El  ///< [in] Ephemeris in GloNAV elemental format.
)
{
   #ifdef PDU_LOGGING
      #if PDU_LOGGING == PDU_LOGGING_LEV_DBG
         GN_RRLP_Log( "SatID: %2u, CodeOnL2: %1u, URA: %2u, SVHealth: %#02x, FitIntFlag: %1u, AODA: %2u, L2Pflag: %1d, TGD: %3d, af2: %3d, Week: %4u, toc: %5u, toe: %5u, IODC: %4u",
            p_GN_AGPS_Eph_El->SatID,                  // Satellite ID (PRN) Number                :  6 bits [1..32]
            p_GN_AGPS_Eph_El->CodeOnL2,               // C/A or P on L2                           :  2 bits [0..3]
            p_GN_AGPS_Eph_El->URA,                    // User Range Accuracy Index                :  4 bits [0..15]
            p_GN_AGPS_Eph_El->SVHealth,               // Satellite Health Bits                    :  6 bits [0..63]
            p_GN_AGPS_Eph_El->FitIntFlag,             // Fit Interval Flag                        :  1 bit  [0=4hrs, 1=6hrs]
            p_GN_AGPS_Eph_El->AODA,                   // Age Of Data Offset                       :  5 bits [x 900 sec]
            p_GN_AGPS_Eph_El->L2Pflag,                // L2 P Data Flag                           :  1 bit  [0..1]
            p_GN_AGPS_Eph_El->TGD,                    // Total Group Delay                        :  8 bits [x 2^-31 sec]
            p_GN_AGPS_Eph_El->af2,                    // SV Clock Drift Rate                      :  8 bits [x 2^-55 sec/sec2]
            p_GN_AGPS_Eph_El->Week,                   // GPS Reference Week Number                : 10 bits [0..1023]
            p_GN_AGPS_Eph_El->toc,                    // Clock Reference Time of Week             : 16 bits [x 2^4 sec]
            p_GN_AGPS_Eph_El->toe,                    // Ephemeris Reference Time of Week         : 16 bits [x 2^4 sec]
            p_GN_AGPS_Eph_El->IODC );                 // Issue Of Data Clock                      : 10 bits [0..1023]
         GN_RRLP_Log( "SatID: %2u, af1: %6d, dn: %6d, IDot: %6d, Crs: %6d, Crc: %6d, Cus: %6d, Cuc: %6d, Cis: %6d, Cic: %6d, af0: %11d",
            p_GN_AGPS_Eph_El->SatID,                  // Satellite ID (PRN) Number                :  6 bits [1..32]
            p_GN_AGPS_Eph_El->af1,                    // SV Clock Drift                           : 16 bits [x 2^-43 sec/sec]
            p_GN_AGPS_Eph_El->dn,                     // Delta n                                  : 16 bits [x 2^-43 semi-circles/sec]
            p_GN_AGPS_Eph_El->IDot,                   // Rate of Inclination Angle                : 14 bits [x 2^-43 semi-circles/sec]
            p_GN_AGPS_Eph_El->Crs,                    // Coefficient-Radius-sine                  : 16 bits [x 2^-5 meters]
            p_GN_AGPS_Eph_El->Crc,                    // Coefficient-Radius-cosine                : 16 bits [x 2^-5 meters]
            p_GN_AGPS_Eph_El->Cus,                    // Coefficient-Argument_of_Latitude-sine    : 16 bits [x 2^-29 radians]
            p_GN_AGPS_Eph_El->Cuc,                    // Coefficient-Argument_of_Latitude-cosine  : 16 bits [x 2^-29 radians]
            p_GN_AGPS_Eph_El->Cis,                    // Coefficient-Inclination-sine             : 16 bits [x 2^-29 radians]
            p_GN_AGPS_Eph_El->Cic,                    // Coefficient-Inclination-cosine           : 16 bits [x 2^-29 radians]
            p_GN_AGPS_Eph_El->af0 );                  // SV Clock Bias                            : 22 bits [x 2^-31 sec]
         GN_RRLP_Log( "SatID: %2d, M0: %11d, e: %10u, APowerHalf: %10u, Omega0: %11d,  i0: %11d, w: %11d, OmegaDot: %11d",
            p_GN_AGPS_Eph_El->SatID,                  // Satellite ID (PRN) Number                :  6 bits [1..32]
            p_GN_AGPS_Eph_El->M0,                     // Mean Anomaly                             : 32 bits [x 2^-31 semi-circles]
            p_GN_AGPS_Eph_El->e,                      // Eccentricity                             : 32 bits [x 2^-33]
            p_GN_AGPS_Eph_El->APowerHalf,             // (Semi-Major Axis)^1/2                    : 32 bits [x 2^-19 metres^1/2]
            p_GN_AGPS_Eph_El->Omega0,                 // Longitude of the Ascending Node          : 32 bits [x 2^-31 semi-circles]
            p_GN_AGPS_Eph_El->i0,                     // Inclination angle                        : 32 bits [x 2^-31 semi-circles]
            p_GN_AGPS_Eph_El->w,                      // Argument of Perigee                      : 32 bits [x 2^-31 meters]
            p_GN_AGPS_Eph_El->OmegaDot );             // Rate of Right Ascension                  : 24 bits [x 2^-43 semi-circles/sec]
      #endif
   #endif
}


//*****************************************************************************
/// \brief
///      Brute force logging of almanac in GloNAV elemental format.
///
/// \returns
///      Nothing.
//*****************************************************************************
void GN_AGPS_Log_Alm_El
(
   s_GN_AGPS_Alm_El *p_GN_AGPS_Alm_El  ///< [in] Almanac in GloNAV elemental format.
)
{
   #ifdef PDU_LOGGING
      #if PDU_LOGGING == PDU_LOGGING_LEV_DBG
         GN_RRLP_Log( "SatID:%2d, WNa:%3d, SVHealth:%3d, toa:%3d, af0:%6d, af1:%6d, e:%5d, delta_I:%6d, OmegaDot:%6d, APowerHalf:%8d, Omega0:%8d, w:%8d , M0:%8d",
            p_GN_AGPS_Alm_El->SatID,
            p_GN_AGPS_Alm_El->WNa,
            p_GN_AGPS_Alm_El->SVHealth,
            p_GN_AGPS_Alm_El->toa,
            p_GN_AGPS_Alm_El->af0,
            p_GN_AGPS_Alm_El->af1,
            p_GN_AGPS_Alm_El->e,
            p_GN_AGPS_Alm_El->delta_I,
            p_GN_AGPS_Alm_El->OmegaDot,
            p_GN_AGPS_Alm_El->APowerHalf,
            p_GN_AGPS_Alm_El->Omega0,
            p_GN_AGPS_Alm_El->w,
            p_GN_AGPS_Alm_El->M0 );
      #endif
   #endif
}


//*****************************************************************************
/// \brief
///      Brute force logging of Acquisition Assist in GloNAV elemental format.
///
/// \returns
///      Nothing.
//*****************************************************************************
void GN_AGPS_Log_Acq_Ass
(
   s_GN_AGPS_Acq_Ass *p_GN_AGPS_Acq_Ass  ///< [in] Acquisition Assist in GloNAV elemental format.
)
{
   #ifdef PDU_LOGGING
#if PDU_LOGGING == PDU_LOGGING_LEV_DBG
         U1 Index;
         for (Index = 0 ; Index < p_GN_AGPS_Acq_Ass->Num_AA ; Index++)
         {
            GN_RRLP_Log( "GN_AGPS_Log_Acq_Ass: Ref_GPS_TOW %9lu, AA %2d of %2d: SatID: %2u, Doppler_0: %5d, Doppler_1: %2u, Doppler_Unc: %u, Code_Phase: %4u, Int_Code_Phase: %2u, GPS_Bit_Num: %u, Code_Window: %2u, Azimuth: %6.2f, Elevation: %5.2f",
               p_GN_AGPS_Acq_Ass->Ref_GPS_TOW,
               Index + 1, p_GN_AGPS_Acq_Ass->Num_AA,        // Count of Total.
               p_GN_AGPS_Acq_Ass->AA[Index].SatID,          //   U1 SatID;             // Satellite ID (PRN) Number  [range 1..32]
               p_GN_AGPS_Acq_Ass->AA[Index].Doppler_0,      //   I2 Doppler_0;         // Satellite Doppler 0th order term [range -2048..2047 x 2.5 Hz]
               p_GN_AGPS_Acq_Ass->AA[Index].Doppler_1,      //   U1 Doppler_1;         // Satellite Doppler 1st order term [range 0..63 x 1/42 Hz/s], offset by -42 to give values in the range -1.0 .. 0.5 Hz
               p_GN_AGPS_Acq_Ass->AA[Index].Doppler_Unc,    //   U1 Doppler_Unc;       // Satellite Doppler Uncertainty [range 0..4], representing +/- { <=200, <=100, <=50, <=25, <=12.5 } Hz
               p_GN_AGPS_Acq_Ass->AA[Index].Code_Phase,     //   U2 Code_Phase;        // Satellite Code Phase [range 0..1022 chips]
               p_GN_AGPS_Acq_Ass->AA[Index].Int_Code_Phase, //   U1 Int_Code_Phase;    // Satellite Integer Code Phase since the last GPS bit edge boundary [range 0..19 ms]
               p_GN_AGPS_Acq_Ass->AA[Index].GPS_Bit_Num,    //   U1 GPS_Bit_Num;       // Satellite GPS Bit Number (modulo 4) relative to #Ref_GPS_TOW
               p_GN_AGPS_Acq_Ass->AA[Index].Code_Window,    //   U1 Code_Window;       // \brief  Satellite Code Phase Search Window [range 0..15], representing
                                                                                       //          +/- { 512, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 196 } chips
               p_GN_AGPS_Acq_Ass->AA[Index].Azimuth   * 11.25,      //   U1 Azimuth;           // Satellite Azimuth   [range 0..31 x 11.25 degrees]
               p_GN_AGPS_Acq_Ass->AA[Index].Elevation * 11.25 );    //   U1 Elevation;         // Satellite Elevation [range 0..7  x 11.25 degrees]
         }
      #endif
   #endif
}


//*****************************************************************************
/// \brief
///      Brute force logging of AGPS Measurements in GloNAV elemental format.
///
/// \returns
///      Nothing.
//*****************************************************************************
void GN_AGPS_Log_Meas
(
   s_GN_AGPS_Meas *p_GN_AGPS_Meas   ///< [in] AGPS Measurements in GloNAV format.
)
{
   #ifdef PDU_LOGGING
      #if PDU_LOGGING == PDU_LOGGING_LEV_DBG
         U1 Index;
         CH LogMessage[1000];
         CH *p_LogIndex = LogMessage;
         U1 SatID;

         //p_LogIndex += sprintf( p_LogIndex, "GN_AGPS_Log_AGPS_Meas: Meas_GPS_TOW: %9u, EFSP_dT_us: %9d, Quality: %3u, Num_Meas: %2u, ",
         //                        p_GN_AGPS_Meas->Meas_GPS_TOW, // U4 Meas_GPS_TOW;           // Measurement GPS Time of Week [range 0..604799999 ms]
         //                        p_GN_AGPS_Meas->EFSP_dT_us,   // I4 EFSP_dT_us;             // Delta-Time [us] to the last received External Frame Sync Pulse, 0x7FFFFFFF = No pulse received
         //                        p_GN_AGPS_Meas->Quality,      // U1 Quality;                // A quality metric [range 0(lowest) ..255(highest)] for this message set.
         //                        p_GN_AGPS_Meas->Num_Meas );   // U1 Num_Meas;               // Number of measurement elements to follow [range 0..16]
         p_LogIndex += sprintf( p_LogIndex, "GN_AGPS_Log_AGPS_Meas: TOW: %9u, Qual: %3u, Num: %2u, {PRN,SNR,PR_RMS_Err}: ",
                                 p_GN_AGPS_Meas->Meas_GPS_TOW, // U4 Meas_GPS_TOW;           // Measurement GPS Time of Week [range 0..604799999 ms]
                                 p_GN_AGPS_Meas->Quality,      // U1 Quality;                // A quality metric [range 0(lowest) ..255(highest)] for this message set.
                                 p_GN_AGPS_Meas->Num_Meas );   // U1 Num_Meas;               // Number of measurement elements to follow [range 0..16]
         for ( SatID = 1 ; SatID <= 32 ; SatID++ )
         {
            for ( Index = 0 ; Index < p_GN_AGPS_Meas->Num_Meas ; Index++ )
            {
               if ( SatID == p_GN_AGPS_Meas->Meas[Index].SatID )
               {
                  //p_LogIndex += sprintf( p_LogIndex, "PRN: %2u, SNR: %2u, Doppler: %6d, Whole_Chips: %4u, Fract_Chips: %4u, MPath_Ind: %1u, PR_RMS_Err: %2u, ",
                  //   p_GN_AGPS_Meas->Meas[Index].SatID,        // U1 SatID;                  // Satellite ID (PRN) Number  [range 1..32]
                  //   p_GN_AGPS_Meas->Meas[Index].SNR,          // U1 SNR;                    // Satellite Signal to Noise Ratio [range 0..63 dBHz]
                  //   p_GN_AGPS_Meas->Meas[Index].Doppler,      // I2 Doppler;                // Satellite Doppler [range -32768..32767 x 0.2 Hz]
                  //   p_GN_AGPS_Meas->Meas[Index].Whole_Chips,  // U2 Whole_Chips;            // Satellite Code Phase Whole Chips [range 0..1022 chips]
                  //   p_GN_AGPS_Meas->Meas[Index].Fract_Chips,  // U2 Fract_Chips;            // Satellite Code Phase Fractional Chips [range 0..1023 x 2^-10 chips]
                  //   p_GN_AGPS_Meas->Meas[Index].MPath_Ind,    // U1 MPath_Ind;              // \brief  Pseudorange Multipath Error Indicator [range 0..3], representing
                  //                                             //                            //         { Not measured, Low (<5m), Medium (<43m), High (>43m) }
                  //   p_GN_AGPS_Meas->Meas[Index].PR_RMS_Err ); // U1 PR_RMS_Err;             // \brief  Pseudorange RMS Error [range 0..63] consisting of a 3 bit Mantissa 'x' & 3 bit Exponent 'y',
                  p_LogIndex += sprintf( p_LogIndex, "{%2u,%2u,%2u} ",
                     p_GN_AGPS_Meas->Meas[Index].SatID,        // U1 SatID;                  // Satellite ID (PRN) Number  [range 1..32]
                     p_GN_AGPS_Meas->Meas[Index].SNR,          // U1 SNR;                    // Satellite Signal to Noise Ratio [range 0..63 dBHz]
                     p_GN_AGPS_Meas->Meas[Index].PR_RMS_Err ); // U1 PR_RMS_Err;             // \brief  Pseudorange RMS Error [range 0..63] consisting of a 3 bit Mantissa 'x' & 3 bit Exponent 'y',
                                                               //                            //         where RMS Error =  0.5 * (1 + x/8) * 2^y  metres.
               }
            }
         }
         GN_RRLP_Log( LogMessage );
         // printf_stub("%s\n",LogMessage);
      #endif
   #endif
}


//*****************************************************************************
/// \brief
///      Encodes an RRLP structure prepared for the ASN.1 compiler into a
///      #s_PDU_Buffer_Store
///
/// \returns
///      Flag to indicate whether encoding the PDU was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
//*****************************************************************************
BL rrlp_PDU_Encode
(
   void                 *p_PDU_Src, ///< [in] Pointer to generated RRLP structure
                                    ///   PDU_TTCN_KEYWORD. Passed in as (void *) to hide
                                    ///   implementation.
   s_PDU_Buffer_Store   *p_PDU_Buf  ///< [out] Store containing encoded PDU
)
{
   uint8_t *encodedASN1;          /*  Data stream to hold the encoded message */
   uint32_t encodedStringLength; /*  To hold the length of the encoded message */

   RRLP_PDU_t  *p_CastPDU = p_PDU_Src;

   char * p_PDUType;
   switch ( p_CastPDU->component.present )
   {
   case RRLP_RRLP_Component_PR_msrPositionReq:    p_PDUType = "msrPositionReq"     ; break ;
   case RRLP_RRLP_Component_PR_msrPositionRsp:    p_PDUType = "msrPositionRsp"     ; break ;
   case RRLP_RRLP_Component_PR_assistanceData:    p_PDUType = "assistanceData"     ; break ;
   case RRLP_RRLP_Component_PR_assistanceDataAck: p_PDUType = "assistanceDataAck"  ; break ;
   case RRLP_RRLP_Component_PR_protocolError:     p_PDUType = "protocolError"      ; break ;
   default:                                  p_PDUType = "Undetermined"       ; break ;
   }
   GN_RRLP_Log( "GN_RRLP_asn1_encode:    --- [label=\"Encode RRLP PDU %s\"];", p_PDUType );

   // asn1_init_encode_buffer() is used to initialize the static encode buffer
   // required for encoding any message   into ASN.1, has to be called before encoding.
   //asn1_init_encode_buffer();  need to be check-raghu

   //retValue = asn1_encode_pdu_ttcn_keyword( &encodedASN1, &encodedStringLength, p_CastPDU );
   RRLP_EncodeMessage(p_CastPDU, &encodedASN1, &encodedStringLength);

   // Free work space initialised and allocated by asn1_init_encode_buffer().
  // asn1_free_encode_buffer(); //need to be check-raghu

   p_PDU_Buf->PDU_Encoded.p_PDU_Data   = (U1*) encodedASN1;
   p_PDU_Buf->PDU_Encoded.Length       = (U2) encodedStringLength;

   if ( encodedStringLength > 0 )
   {
      rrlp_PDU_Buffer_Store_Log( "rrlp_PDU_Encode", p_PDU_Buf );

      return TRUE;
   }
   else
   {
      GN_RRLP_Log( "GN_RRLP_asn1_encode: Failed ");
      return( FALSE );
   }
}


//*****************************************************************************
/// \brief
///      Decodes an RRLP structure prepared for the ASN.1 compiler from a
///      #s_PDU_Buffer_Store
///
/// \returns
///      Flag to indicate whether decoding the PDU was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
//*****************************************************************************
BL rrlp_PDU_Decode
(
   s_PDU_Buffer_Store   *p_PDU_Buf,    ///< [in] Store containing encoded PDU
   void                 **p_p_PDU_Dest,///< [out] Pointer to generated RRLP structure
                                       ///   pointer PDU_TTCN_KEYWORD. Passed in as (void **)
                                       ///   to hide implementation.
   GN_RRLP_ErrorCodes   *p_GN_RRLP_ErrorCodes /// [out] Status of the decode
)
{
   uint8_t   *encodedASN1;        /*  Data stream to hold the encoded message */
   uint32_t encodedStringLength; /*  To hold the length of the encoded message */
   uint32_t retValue;            /*  To hold the return value indicating success / failure of encode / decode operation  */

   RRLP_PDU_t **p_p_CastPDU = (RRLP_PDU_t**) p_p_PDU_Dest;
   RRLP_PDU_t *p_CastPDU = NULL;

   //*p_p_CastPDU = GN_Calloc( 1, sizeof( RRLP_PDU_t) );

   rrlp_PDU_Buffer_Store_Log ("rrlp_PDU_Decode", p_PDU_Buf );
   encodedASN1          = (uint8_t *) p_PDU_Buf->PDU_Encoded.p_PDU_Data;
   encodedStringLength  = (uint32_t) p_PDU_Buf->PDU_Encoded.Length;

   retValue = RRLP_DecodeMessage(encodedStringLength, encodedASN1, &p_CastPDU);

   ASN1_SET(p_p_CastPDU) = p_CastPDU;

  // retValue = asn1_decode_pdu_ttcn_keyword( encodedASN1, encodedStringLength, *p_p_CastPDU );

   if ( retValue)// == 0 )
   {
      char * p_PDUType;
      switch ( (*p_p_CastPDU)->component.present )
      {
      case RRLP_RRLP_Component_PR_msrPositionReq:    p_PDUType = "msrPositionReq"     ; break ;
      case RRLP_RRLP_Component_PR_msrPositionRsp:    p_PDUType = "msrPositionRsp"     ; break ;
      case RRLP_RRLP_Component_PR_assistanceData:    p_PDUType = "assistanceData"     ; break ;
      case RRLP_RRLP_Component_PR_assistanceDataAck: p_PDUType = "assistanceDataAck"  ; break ;
      case RRLP_RRLP_Component_PR_protocolError:     p_PDUType = "protocolError"      ; break ;
      default:                p_PDUType = "Undetermined"       ; break ;
      }
      GN_RRLP_Log( "GN_RRLP_asn1_decode:    --- [label=\"Decode RRLP PDU %s\"];", p_PDUType );
      *p_GN_RRLP_ErrorCodes = GN_RRLP_ErrorCodes_NONE;
      return TRUE;
   }
   else
   {
      switch ( retValue )
      {
         default:
            *p_GN_RRLP_ErrorCodes = GN_RRLP_ErrorCodes_unDefined;
            break;
      }

      return FALSE;
   }

}


//*****************************************************************************
/// \brief
///      Allocates storage for RRLP_PDU_t in a way that hides the implementation.
///
/// \returns
///      Pointer to storage for RRLP_PDU_t.
//*****************************************************************************
void *rrlp_PDU_Alloc( void )
{
   RRLP_PDU_t *p_TempPDU;

   p_TempPDU = GN_Calloc( 1, sizeof( RRLP_PDU_t) ); /* not malloc! */
   if( ! p_TempPDU ) {
      GN_RRLP_Log( "GN_Calloc() failed" );
      return NULL;
   }
   return ( (void*) p_TempPDU );
}


//*****************************************************************************
/// \brief
///      Frees storage for PDU_TTCN_KEYWORD in a way that hides the implementation.
///
/// \returns
///      nothing.
//*****************************************************************************
void rrlp_PDU_Free
(
   void *p_PDU ///< [in] Pointer to storage for PDU_TTCN_KEYWORD.
)
{
   RRLP_PDU_t *p_TempPDU = p_PDU;

   if ( p_TempPDU != NULL )
   {
       RRLP_FreePDU(p_TempPDU);
   }
}


//*****************************************************************************
/// \brief
///      Function to identify the contents of the decoded RRLP PDU.
///
/// \returns
///      enum indicating the RRLP message type #e_GN_RRLP_Component_PR
//*****************************************************************************
e_GN_RRLP_Component_PR rrlp_PDU_Get_MessageType
(
   void *p_ThisPDU   ///< [in] Pointer to storage for PDU_TTCN_KEYWORD.
)
{
   RRLP_PDU_t *p_PDU_Cast = p_ThisPDU;

   // RRLP-Component ::= CHOICE {
   //    msrPositionReq       MsrPosition-Req,
   //    msrPositionRsp       MsrPosition-Rsp,
   //    assistanceData       AssistanceData,
   //    assistanceDataAck    NULL,
   //    protocolError        ProtocolError,
   //    ...
   // }

   switch( p_PDU_Cast->component.present )
   {
   case RRLP_RRLP_Component_PR_msrPositionReq:    return GN_RRLP_Component_PR_msrPositionReq;     break;
   case RRLP_RRLP_Component_PR_msrPositionRsp:    return GN_RRLP_Component_PR_msrPositionRsp;     break;
   case RRLP_RRLP_Component_PR_assistanceData:    return GN_RRLP_Component_PR_assistanceData;     break;
   case RRLP_RRLP_Component_PR_assistanceDataAck: return GN_RRLP_Component_PR_assistanceDataAck;  break;
   case RRLP_RRLP_Component_PR_protocolError:     return GN_RRLP_Component_PR_protocolError;      break;
   default:
      return GN_RRLP_Component_PR_NOTHING;
      break;
   }
   return GN_RRLP_Component_PR_NOTHING;
}


//*****************************************************************************
/// \brief
///      Populates an rrlp pdu structure with assistance data.
///
/// \returns
///      Pointer to storage for RRLP_PDU_t.
//*****************************************************************************
#ifdef SUPL_SERVER_FTR
void *rrlp_PDU_Make_assistanceData
(
   U2                         ReferenceNumber,  ///< [in] Current instance data used to
                                                ///   populate the assistance data.
   s_GN_AGPS_GAD_Data         *p_GN_AGPS_GAD_Data,
   s_GN_RequestedAssistData   *p_GN_RequestedAssistData
)
{
   RRLP_PDU_t *p_TempPDU = rrlp_PDU_Alloc();
   BL status;

   // PDU ::= SEQUENCE {
   //    referenceNumber      INTEGER (0..7),
   //    component            RRLP-Component
   //}

   p_TempPDU->referenceNumber = (ReferenceNumber) % 8;

   // RRLP-Component ::= CHOICE {
   //    msrPositionReq       MsrPosition-Req,
   //    msrPositionRsp       MsrPosition-Rsp,
   //    assistanceData       AssistanceData,
   //    assistanceDataAck    NULL,
   //    protocolError        ProtocolError,
   //    ...
   //}

   p_TempPDU->component.present = RRLP_RRLP_Component_PR_assistanceData;
   //p_TempPDU->component.choice.assistanceData.gps_AssistData.U.assistanceData.gps_AssistData2Present = SDL_True;//need to be check- raghu

   status = rrlp_PDU_Make_GPS_AssistData(
       p_TempPDU->component.choice.assistanceData.gps_AssistData,
      p_GN_AGPS_GAD_Data,
      p_GN_RequestedAssistData );

   return p_TempPDU;
}
#endif

//*****************************************************************************
/// \brief
///      Populates an rrlp pdu structure with an assistance data acknowledgement.
///
/// \returns
///      Pointer to storage for PDU_TTCN_KEYWORD.
//*****************************************************************************
void *rrlp_PDU_Make_assistanceDataAck
(
   U2 ReferenceNumber   ///< [in] Current instance data used to
                        ///   populate the assistance data
                        ///   acknowledgement.
)
{
   RRLP_PDU_t *p_TempPDU = rrlp_PDU_Alloc();

   // PDU ::= SEQUENCE {
   //    referenceNumber      INTEGER (0..7),
   //    component            RRLP-Component
   //}

   p_TempPDU->referenceNumber = (ReferenceNumber) % 8;

   // RRLP-Component ::= CHOICE {
   //    msrPositionReq       MsrPosition-Req,
   //    msrPositionRsp       MsrPosition-Rsp,
   //    assistanceData       AssistanceData,
   //    assistanceDataAck    NULL,
   //    protocolError        ProtocolError,
   //    ...
   //}

   p_TempPDU->component.present = RRLP_RRLP_Component_PR_assistanceDataAck;

   return p_TempPDU;
}


//*****************************************************************************
/// \brief
///      Populates an rrlp pdu structure with an measure position request.
///
/// \returns
///      Pointer to storage for RRLP_PDU_t.
//*****************************************************************************
void *rrlp_PDU_Make_msrPositionReq
(
   U2 ReferenceNumber,  ///< [in] Current instance data used to
                        ///   populate the measure pos. request.
   s_GN_RRLP_MethodType *p_GN_RRLP_MethodType
)
{
   RRLP_PDU_t *p_TempPDU = rrlp_PDU_Alloc();

   PositionInstruct *p_PositionInstruct;

   // PDU ::= SEQUENCE {
   //    referenceNumber      INTEGER (0..7),
   //    component            RRLP-Component
   //}

   p_TempPDU->referenceNumber = (ReferenceNumber) % 8;

   // RRLP-Component ::= CHOICE {
   //    msrPositionReq       MsrPosition-Req,
   //    msrPositionRsp       MsrPosition-Rsp,
   //    assistanceData       AssistanceData,
   //    assistanceDataAck    NULL,
   //    protocolError        ProtocolError,
   //    ...
   //}

   p_TempPDU->component.present = RRLP_RRLP_Component_PR_msrPositionReq;
   p_PositionInstruct = &p_TempPDU->component.choice.msrPositionReq.positionInstruct;

   switch( p_GN_RRLP_MethodType->methodType )
   {
   case GN_msAssisted:     // msAssisted     AccuracyOpt,   -- accuracy is optional
      p_PositionInstruct->methodType.present = RRLP_MethodType_PR_msAssisted;
      if ( p_GN_RRLP_MethodType->accuracy != -1 )
      {
          *p_PositionInstruct->methodType.choice.msAssisted.accuracy     = p_GN_RRLP_MethodType->accuracy;
        // p_PositionInstruct->methodType.U.msAssisted.accuracyPresent = SDL_True;  //need to be check
      }
      break;
   case GN_msBased:        // msBased        Accuracy,      -- accuracy is mandatory
       p_PositionInstruct->methodType.present          = RRLP_MethodType_PR_msBased;
       p_PositionInstruct->methodType.choice.msBased        = p_GN_RRLP_MethodType->accuracy;
      break;
   case GN_msBasedPref:    // msBasedPref    Accuracy,      -- accuracy is mandatory
       p_PositionInstruct->methodType.present          = RRLP_MethodType_PR_msBasedPref;
       p_PositionInstruct->methodType.choice.msAssistedPref    = p_GN_RRLP_MethodType->accuracy;
      break;
   case GN_msAssistedPref: // msAssistedPref Accuracy       -- accuracy is mandatory
       p_PositionInstruct->methodType.present          = RRLP_MethodType_PR_msAssistedPref;
       p_PositionInstruct->methodType.choice.msAssistedPref = p_GN_RRLP_MethodType->accuracy;
      break;
   default:
       break;
   }
   RRLP_ASN1_ENUM_SET(p_PositionInstruct->positionMethod,RRLP_PositionMethod_gps);

   /// \todo Calculate value for measurement response time. (t in seconds is pow(2, measureResponseTime))
   p_PositionInstruct->measureResponseTime = 4;

   RRLP_ASN1_ENUM_SET(p_PositionInstruct->useMultipleSets,RRLP_UseMultipleSets_oneSet);

   return p_TempPDU;
}

//*****************************************************************************
/// \brief
///      Populates an rrlp pdu structure with an measure position response.
///
/// \returns
///      Pointer to storage for RRLP_PDU_t.
//*****************************************************************************
void *rrlp_PDU_Make_msrPositionRsp
(
   U2 ReferenceNumber   ///< [in] Current instance data used to
                        ///   populate the measure pos. response.
)
{
   RRLP_PDU_t *p_TempPDU = rrlp_PDU_Alloc();

   // PDU ::= SEQUENCE {
   //    referenceNumber      INTEGER (0..7),
   //    component            RRLP-Component
   // }

   p_TempPDU->referenceNumber = (ReferenceNumber) % 8;

   // RRLP-Component ::= CHOICE {
   //    msrPositionReq       MsrPosition-Req,
   //    msrPositionRsp       MsrPosition-Rsp,
   //    assistanceData       AssistanceData,
   //    assistanceDataAck    NULL,
   //    protocolError        ProtocolError,
   //    ...
   // }

   p_TempPDU->component.present = RRLP_RRLP_Component_PR_msrPositionRsp;

   return p_TempPDU;
}



//*****************************************************************************
/// \brief
///      Populates an rrlp pdu structure with a protocol error.
///
/// \returns
///      Pointer to storage for RRLP_PDU_t.
//*****************************************************************************
void *rrlp_PDU_Make_protocolError
(
   U2                   ReferenceNumber,  ///< [in] Reference number to populate
                                          ///   the protocol error PDU.
   GN_RRLP_ErrorCodes   ErrorCode         ///< [in]
)
{
   RRLP_PDU_t *p_TempPDU = rrlp_PDU_Alloc();
   RRLP_ErrorCodes_t *p_ErrorCodes = &p_TempPDU->component.choice.protocolError.errorCause;//need to check-raghu

   // PDU ::= SEQUENCE {
   //    referenceNumber      INTEGER (0..7),
   //    component            RRLP-Component.
   //}

   p_TempPDU->referenceNumber = (ReferenceNumber) % 8;

   // RRLP-Component ::= CHOICE {
   //    msrPositionReq       MsrPosition-Req,
   //    msrPositionRsp       MsrPosition-Rsp,
   //    assistanceData       AssistanceData,
   //    assistanceDataAck    NULL,
   //    protocolError        ProtocolError,
   //    ...
   //}

   p_TempPDU->component.present = RRLP_RRLP_Component_PR_protocolError;

   // -- Protocol Error Causes
   // ErrorCodes ::= ENUMERATED {
   //    unDefined (0),
   //    missingComponet (1),
   //    incorrectData (2),
   //    missingIEorComponentElement (3),
   //    messageTooShort (4),
   //    unknowReferenceNumber (5),
   //    ...
   // }

   switch( ErrorCode )
   {
   case GN_RRLP_ErrorCodes_unDefined:
       RRLP_ASN1_ENUM_SET_CHK(p_ErrorCodes, RRLP_ErrorCodes_unDefined);
      break;
   case GN_RRLP_ErrorCodes_missingComponet:
      RRLP_ASN1_ENUM_SET_CHK(p_ErrorCodes, RRLP_ErrorCodes_missingComponet);
      break;
   case GN_RRLP_ErrorCodes_incorrectData:
      RRLP_ASN1_ENUM_SET_CHK(p_ErrorCodes,RRLP_ErrorCodes_incorrectData);
      break;
   case GN_RRLP_ErrorCodes_missingIEorComponentElement:
      RRLP_ASN1_ENUM_SET_CHK(p_ErrorCodes, RRLP_ErrorCodes_missingIEorComponentElement);
      break;
   case GN_RRLP_ErrorCodes_messageTooShort:
      RRLP_ASN1_ENUM_SET_CHK(p_ErrorCodes, RRLP_ErrorCodes_messageTooShort);
      break;
   case GN_RRLP_ErrorCodes_unknowReferenceNumber:
      RRLP_ASN1_ENUM_SET_CHK(p_ErrorCodes, RRLP_ErrorCodes_unknowReferenceNumber);
      break;
   default:
       break;
   }
   return p_TempPDU;
}

//*****************************************************************************
/// \brief
///      Extracts utcModel from RRLP pdu and converts it to the #s_GN_AGPS_UTC
///      format.
///
/// \returns
///      Flag to indicate whether UTC Model was available.
/// \retval #TRUE if available.
/// \retval #FALSE if unavailable.
//*****************************************************************************

BL rrlp_PDU_Get_Assist_utcModel
(
   GPS_AssistData *p_GPS_AssistData,      ///< [in] UTC Model within delivered PDU.
   s_GN_AGPS_UTC  *p_GN_AGPS_UTC          ///< [out] Glonav format UTC model.
)
{
   RRLP_UTCModel_t  *p_utcModel;

   if ( p_GPS_AssistData != NULL )
   {
       if ( p_GPS_AssistData->controlHeader.utcModel != NULL)
      {
         // -- Universal Time Coordinate Model
         // UTCModel ::= SEQUENCE {
         p_utcModel = p_GPS_AssistData->controlHeader.utcModel;

         //
         // U4 ZCount;                 // Reference Z-Count the Almanac Subframe was decoded at.  (See \ref Note_ZCount "Note 1").
         p_GN_AGPS_UTC->ZCount   = 0; // Set Zcount to zero if unknown.

         // I4 A1;                     // UTC model - parameter A1               [x 2^-50 seconds/second]
         //    utcA1          INTEGER (-8388608..8388607),
         p_GN_AGPS_UTC->A1       = p_utcModel->utcA1;

         // I4 A0;                     // UTC model - parameter A0               [x 2^-30 seconds]
         //    utcA0          INTEGER (-2147483648..2147483647),
         p_GN_AGPS_UTC->A0       = (I4) p_utcModel->utcA0;

         // U1 Tot;                    // UTC model - reference time of week     [x 2^12 seconds]
         //    utcTot         INTEGER (0..255),
         p_GN_AGPS_UTC->Tot      = (U1) p_utcModel->utcTot;

         // U1 WNt;                    // UTC model - reference week number      [weeks]
         //    utcWNt         INTEGER (0..255),
         p_GN_AGPS_UTC->WNt      = (U1) p_utcModel->utcWNt;

         // I1 dtLS;                   // UTC model - time difference due to leap seconds before event  [seconds]
         //    utcDeltaTls    INTEGER (-128..127),
         p_GN_AGPS_UTC->dtLS     = (I1) p_utcModel->utcDeltaTls;

         // U1 WNLSF;                  // UTC model - week number when next leap second event occurs    [weeks]
         //    utcWNlsf       INTEGER (0..255),
         p_GN_AGPS_UTC->WNLSF    = (U1) p_utcModel->utcWNlsf;

         // U1 DN;                     // UTC model - day of week when next leap second event occurs    [days]
         //    utcDN          INTEGER (-128..127),
         p_GN_AGPS_UTC->DN       = (U1) p_utcModel->utcDN;

         // I1 dtLSF;                  // UTC model - time difference due to leap seconds after event   [seconds]
         //    utcDeltaTlsf   INTEGER (-128..127)
         p_GN_AGPS_UTC->dtLSF    = (I1) p_utcModel->utcDeltaTlsf;

         // }

         #ifdef PDU_LOGGING
            #if PDU_LOGGING == PDU_LOGGING_LEV_DBG
               GN_RRLP_Log( "rrlp_PDU_Get_Assist_utcModel: ZCount = %u, A1 = %d, A0 = %d, Tot = %u, WNt = %u, dtLS = %d, WNLSF = %u, DN = %u, dtLSF = %d",
                  p_GN_AGPS_UTC->ZCount,  // U4 ZCount;  // Reference Z-Count the Almanac Subframe was decoded at.  (See \ref Note_ZCount "Note 1").
                  p_GN_AGPS_UTC->A1,      // I4 A1;      // UTC model - parameter A1               [x 2^-50 seconds/second]
                  p_GN_AGPS_UTC->A0,      // I4 A0;      // UTC model - parameter A0               [x 2^-30 seconds]
                  p_GN_AGPS_UTC->Tot,     // U1 Tot;     // UTC model - reference time of week     [x 2^12 seconds]
                  p_GN_AGPS_UTC->WNt,     // U1 WNt;     // UTC model - reference week number      [weeks]
                  p_GN_AGPS_UTC->dtLS,    // I1 dtLS;    // UTC model - time difference due to leap seconds before event  [seconds]
                  p_GN_AGPS_UTC->WNLSF,   // U1 WNLSF;   // UTC model - week number when next leap second event occurs    [weeks]
                  p_GN_AGPS_UTC->DN,      // U1 DN;      // UTC model - day of week when next leap second event occurs    [days]
                  p_GN_AGPS_UTC->dtLSF ); // I1 dtLSF;   // UTC model - time difference due to leap seconds after event   [seconds]
            #endif
         #endif

         return TRUE;
      }
   }
   return FALSE;
}


//*****************************************************************************
/// \brief
///      Extracts Reference Time from RRLP pdu and converts it to the
///      #s_GN_AGPS_Ref_Time format.
///
/// \returns
///      Flag to indicate whether Reference Time was available.
/// \retval #TRUE if available.
/// \retval #FALSE if unavailable.
//*****************************************************************************
BL rrlp_PDU_Get_Assist_referenceTime
(
   GPS_AssistData       *p_GPS_AssistData,   ///< [in] Reference Time within delivered PDU.
   s_GN_AGPS_Ref_Time   *p_GN_AGPS_Ref_Time, ///< [out] Glonav format Reference Time.
   U4                   PDU_TimeStamp        ///< [in] Time of arrival of PDU in OS_Time_ms
)
{
   // ReferenceTime ::= SEQUENCE {
   //    gpsTime           GPSTime,
   //    gsmTime           GSMTime           OPTIONAL,
   //    gpsTowAssist      GPSTOWAssist      OPTIONAL
   // }

   // -- GPS Time includes week number and time-of-week (TOW)
   // GPSTime ::= SEQUENCE {
   //    gpsTOW23b         GPSTOW23b,
   //    gpsWeek           GPSWeek
   // }

   RRLP_ReferenceTime_t  *p_referenceTime;

   if ( p_GPS_AssistData != NULL )
   {
       if ( p_GPS_AssistData->controlHeader.referenceTime != NULL )
      {
          p_referenceTime = p_GPS_AssistData->controlHeader.referenceTime;

         // U4 TOW_ms;                 // Reference GPS Time of Week [milliseconds].
         // -- GPSTOW, range 0-604799.92, resolution 0.08 sec, 23-bit presentation
         // GPSTOW23b ::= INTEGER (0..7559999)
         p_GN_AGPS_Ref_Time->TOW_ms = (U4) ( p_referenceTime->gpsTime.gpsTOW23b * (U4) ( 1000.0 * 0.08 ) );

         // U2 WeekNo;                 // Reference GPS Week Number.  If known, including the GPS 1024 week rollovers.
         // -- GPS week number
         // GPSWeek ::= INTEGER (0..1023)
         p_GN_AGPS_Ref_Time->WeekNo = (U2) p_referenceTime->gpsTime.gpsWeek;

         // U2 RMS_ms;                 // Reference GPS Time RMS Accuracy [milliseconds]
         p_GN_AGPS_Ref_Time->RMS_ms = 1000; /// \todo Reasess: Assuming 1s rms equiv 3 sec uncertainty.

         // U4 OS_Time_ms;             // OS Time [milliseconds] when the Reference GPS Time was obtained.
         p_GN_AGPS_Ref_Time->OS_Time_ms = PDU_TimeStamp; /// Timestamp of the PDU reception.

         #ifdef PDU_LOGGING
            #if PDU_LOGGING == PDU_LOGGING_LEV_DBG
               GN_RRLP_Log( "rrlp_PDU_Get_Assist_referenceTime: WeekNo: %u, GPS Tow23b: %u, GPS_Tow(ms): %u",
                  p_referenceTime->gpsTime.gpsWeek,
                  p_referenceTime->gpsTime.gpsTOW23b,
                  p_GN_AGPS_Ref_Time->TOW_ms );
            #endif
         #endif
         return TRUE;
      }
   }
   return FALSE;
}


//*****************************************************************************
/// \brief
///      Extracts Reference Location from RRLP pdu and converts it to the
///      #s_GN_AGPS_Ref_Pos format.
///
/// \returns
///      Flag to indicate whether Reference Location was available.
/// \retval #TRUE if available.
/// \retval #FALSE if unavailable.
//*****************************************************************************
BL rrlp_PDU_Get_Assist_refLocation
(
   GPS_AssistData     *p_GPS_AssistData,     ///< [in] Reference Location within
                                             ///   delivered PDU.
   s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data    ///< [out] Glonav format Reference
                                             ///   Location.
)
{
   // -- Reference Location IE
   // RefLocation ::= SEQUENCE {
   //    threeDLocation       Ext-GeographicalInformation
   // }

   s_GAD_Message  GAD_Message;
   RRLP_RefLocation_t    *p_refLocation;

   if ( p_GPS_AssistData != NULL )
   {
       if ( p_GPS_AssistData->controlHeader.refLocation != NULL )
      {
         p_refLocation = p_GPS_AssistData->controlHeader.refLocation;
         if ( p_refLocation->threeDLocation.size != 0 )
         {
            CH MessageText[ 14 * 3 + 1 ] = "";
            CH *MessagePtr = MessageText;
            U1 i;

            GAD_Message.buflen = p_refLocation->threeDLocation.size;
            memcpy ( &GAD_Message.message, p_refLocation->threeDLocation.buf, GAD_Message.buflen );

            for ( i = 0 ; i < GAD_Message.buflen && i < sizeof(GAD_Message.message) ; i++ )
            {
               MessagePtr += sprintf( MessagePtr, " %02X", GAD_Message.message[i] );
            }
            GN_RRLP_Log( "rrlp_PDU_Get_Assist_refLocation:GAD_Message Length: %d, Contents:%s",
                         GAD_Message.buflen,
                         MessageText );

            return GN_AGPS_GAD_Data_From_GAD_Message( &GAD_Message, p_GN_AGPS_GAD_Data );
         }
      }
   }
   return FALSE;
}


//*****************************************************************************
/// \brief
///      Extracts Ionospheric Model from RRLP pdu and converts it to the
///      #s_GN_AGPS_Ion format.
///
/// \returns
///      Flag to indicate whether Ionospheric Model was available.
/// \retval #TRUE if available.
/// \retval #FALSE if unavailable.
//*****************************************************************************
BL rrlp_PDU_Get_Assist_ionosphericModel
(
   GPS_AssistData *p_GPS_AssistData,         ///< [in] Ionospheric Model within
                                             ///   delivered PDU.
   s_GN_AGPS_Ion  *p_GN_AGPS_Ion             ///< [out] Glonav format
                                             ///   Ionospheric Model.
)
{

   RRLP_IonosphericModel_t *p_ionosphericModel;

   if ( p_GPS_AssistData != NULL )
   {
      // -- Ionospheric Model IE
      // IonosphericModel ::= SEQUENCE {
       if ( p_GPS_AssistData->controlHeader.ionosphericModel != NULL)
      {
         p_ionosphericModel = p_GPS_AssistData->controlHeader.ionosphericModel;//need to check -raghu for all kind of this assisgnments

         // U4 ZCount;                 // Reference Z-Count the Almanac Subframe was decoded at.  (See \ref Note_ZCount "Note 1").
         p_GN_AGPS_Ion->ZCount = 0;

         // I1 a0;                     // Klobuchar - alpha 0   : 8 bits  [x 2^-30      seconds]
         //    alfa0       INTEGER (-128..127),
         p_GN_AGPS_Ion->a0 = (I1) p_ionosphericModel->alfa0;

         // I1 a1;                     // Klobuchar - alpha 1   : 8 bits  [x 2^-27/PI   seconds/semi-circle]
         //    alfa1       INTEGER (-128..127),
         p_GN_AGPS_Ion->a1 = (I1) p_ionosphericModel->alfa1;

         // I1 a2;                     // Klobuchar - alpha 2   : 8 bits  [x 2^-24/PI^2 seconds/semi-circle^2]
         //    alfa2       INTEGER (-128..127),
         p_GN_AGPS_Ion->a2 = (I1) p_ionosphericModel->alfa2;

         // I1 a3;                     // Klobuchar - alpha 3   : 8 bits  [x 2^-24/PI^3 seconds/semi-circle^3]
         //    alfa3       INTEGER (-128..127),
         p_GN_AGPS_Ion->a3 = (I1) p_ionosphericModel->alfa3;

         // I1 b0;                     // Klobuchar - beta 0    : 8 bits  [x 2^11       seconds]
         //    beta0       INTEGER (-128..127),
         p_GN_AGPS_Ion->b0 = (I1) p_ionosphericModel->beta0;

         // I1 b1;                     // Klobuchar - beta 1    : 8 bits  [x 2^14/PI    seconds/semi-circle]
         //    beta1       INTEGER (-128..127),
         p_GN_AGPS_Ion->b1 = (I1) p_ionosphericModel->beta1;

         // I1 b2;                     // Klobuchar - beta 2    : 8 bits  [x 2^16/PI^2  seconds/semi-circle^2]
         //    beta2       INTEGER (-128..127),
         p_GN_AGPS_Ion->b2 = (I1) p_ionosphericModel->beta2;

         // I1 b3;                     // Klobuchar - beta 3    : 8 bits  [x 2^16/PI^3  seconds/semi-circle^3]
         //    beta3       INTEGER (-128..127)
         p_GN_AGPS_Ion->b3 = (I1) p_ionosphericModel->beta3;

         //}
         return TRUE;
      }
   }
   return FALSE;
}

/* The below function need to be check -Raghu*/

//*****************************************************************************
/// \brief
///      Extracts Acquisition Assistance from RRLP pdu and converts it to the
///      #s_GN_AGPS_Acq_Ass format.
///
/// \returns
///      Flag to indicate whether Acquisition Assistance was available.
/// \retval #TRUE if available.
/// \retval #FALSE if unavailable.
//*****************************************************************************
BL rrlp_PDU_Get_Assist_acquisAssist
(
   GPS_AssistData    *p_GPS_AssistData,      ///< [in] Ionospheric Model within
                                             ///   delivered PDU.
   s_GN_AGPS_Acq_Ass *p_GN_AGPS_Acq_Ass      ///< [out] Glonav format
                                             ///   Acquisition Assistance.
)
{
   U1 AcqAssistCount;

   if ( p_GPS_AssistData != NULL )
   {
       if ( p_GPS_AssistData->controlHeader.acquisAssist != NULL )
      {
         RRLP_AcquisAssist_t  *p_acquisAssist = p_GPS_AssistData->controlHeader.acquisAssist;

         RRLP_AcquisElement_t *AcquisElement = *p_GPS_AssistData->controlHeader.acquisAssist->acquisList.list.array;

         // gpsTOW is in 80ms units.
         p_GN_AGPS_Acq_Ass->Ref_GPS_TOW = p_acquisAssist->timeRelation.gpsTOW * 80;
         /// \todo work out what to do with the 'time relation' field if present.

         for ( AcqAssistCount = 0;
             AcqAssistCount < p_acquisAssist->acquisList.list.count &&
                  AcqAssistCount < 16 && (AcquisElement != NULL); // number of s_GN_AGPS_AA_El elements in s_GN_AGPS_Acq_Ass
               AcqAssistCount++ )
         {
             AcquisElement = p_GPS_AssistData->controlHeader.acquisAssist->acquisList.list.array[AcqAssistCount];
            // U1 SatID;                  // Satellite ID (PRN) Number  [range 1..32]
            // svid              INTEGER (0..63)   -- identifies satellite,
             p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].SatID = (U1)AcquisElement->svid +1; //SeqOfAcquisElement->list.array[AcqAssistCount]->svid + 1; //p_AcquisElement->Data.svid + 1;

            // -- Doppler 0th order term,
            // -- -5120.0 - 5117.5 Hz (= -2048 - 2047 with 2.5 Hz resolution)
            // doppler0                INTEGER (-2048..2047),
            // I2 Doppler_0;              // Satellite Doppler 0th order term [range -2048..2047 x 2.5 Hz]
             p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].Doppler_0 = (I2)AcquisElement->doppler0;//SeqOfAcquisElement->list.array[AcqAssistCount]->doppler0;//p_AcquisElement->Data.doppler0;

            // addionalDoppler         AddionalDopplerFields   OPTIONAL,
             if ( AcquisElement->addionalDoppler != NULL ) //p_AcquisElement->Data.addionalDopplerPresent == SDL_True )
            {
               // -- Doppler 1st order term, -1.0 - +0.5 Hz/sec
               // -- (= -42 + (0 to 63) with 1/42 Hz/sec. resolution)
               // doppler1             INTEGER (0..63),
               // U1 Doppler_1;              // Satellite Doppler 1st order term [range 0..63 x 1/42 Hz/s], offset by -42 to give values in the range -1.0 .. 0.5 Hz
                p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].Doppler_1 = (U1) AcquisElement->addionalDoppler->doppler1;  //p_AcquisElement->Data.addionalDoppler.doppler1;

               // dopplerUncertainty   INTEGER (0..7)
               // U1 Doppler_Unc;            // Satellite Doppler Uncertainty [range 0..4], representing +/- { <=200, <=100, <=50, <=25, <=12.5 } Hz
                p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].Doppler_Unc = (U1)AcquisElement->addionalDoppler->dopplerUncertainty; //p_AcquisElement->Data.addionalDoppler.dopplerUncertainty;
            }
            else
            {
               // If #Doppler_1 and #Doppler_Unc are unknown (ie they were not provided)
               // then they should both be set to zero.
               p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].Doppler_1 = 0;
               p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].Doppler_Unc = 0;
            }

            // codePhase               INTEGER (0..1022),   -- Code Phase
            // U2 Code_Phase;             // Satellite Code Phase [range 0..1022 chips]
            p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].Code_Phase = (U2)AcquisElement->codePhase; //p_AcquisElement->Data.codePhase;

            // intCodePhase            INTEGER (0..19),  -- Integer Code Phase
            // U1 Int_Code_Phase;         // Satellite Integer Code Phase since the last GPS bit edge boundary [range 0..19 ms]
            p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].Int_Code_Phase = (U1)AcquisElement->intCodePhase; // p_AcquisElement->Data.intCodePhase;

            // gpsBitNumber            INTEGER (0..3),   -- GPS bit number
            // U1 GPS_Bit_Num;            // Satellite GPS Bit Number (modulo 4) relative to #Ref_GPS_TOW
            p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].GPS_Bit_Num = (U1)AcquisElement->gpsBitNumber;  //p_AcquisElement->Data.gpsBitNumber;

            // codePhaseSearchWindow   INTEGER (0..15),  -- Code Phase Search Window
            // U1 Code_Window;            // \brief  Satellite Code Phase Search Window [range 0..15], representing
            //                            ///          +/- { 512, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96, 128, 196 } chips
            p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].Code_Window = (U1)AcquisElement->codePhaseSearchWindow; //p_AcquisElement->Data.codePhaseSearchWindow;

            // addionalAngle           AddionalAngleFields     OPTIONAL
            if ( AcquisElement->addionalAngle != NULL ) //p_AcquisElement->Data.addionalAnglePresent )
            {
               // -- azimuth angle, 0 - 348.75 deg (= 0 - 31 with 11.25 deg resolution)
               // azimuth              INTEGER (0..31),
               // U1 Azimuth;                // Satellite Azimuth   [range 0..31 x 11.25 degrees]
                p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].Azimuth = (U1)AcquisElement->addionalAngle->azimuth; //p_AcquisElement->Data.addionalAngle.azimuth;
               // -- elevation angle, 0 - 78.75 deg (= 0 - 7 with 11.25 deg resolution)
               // elevation            INTEGER (0..7)
               // U1 Elevation;              // Satellite Elevation [range 0..7  x 11.25 degrees]
                p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].Elevation = (U1)AcquisElement->addionalAngle->elevation;  //p_AcquisElement->Data.addionalAngle.elevation;
            }
            else
            {
               // If #Azimuth and #Elevation are unknown (ie they were not provided)
               // then they should both be set to zero.
               p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].Azimuth = 0;
               p_GN_AGPS_Acq_Ass->AA[AcqAssistCount].Elevation = 0;
            }
         }

               p_GN_AGPS_Acq_Ass->Num_AA = p_acquisAssist->acquisList.list.count; //p_acquisAssist->acquisList.Length;
         #ifdef PDU_LOGGING
            #if PDU_LOGGING == PDU_LOGGING_LEV_DBG
               GN_AGPS_Log_Acq_Ass( p_GN_AGPS_Acq_Ass );
            #endif
         #endif
         return TRUE;
      }
   }
   return FALSE;
}


//*****************************************************************************
/// \brief
///      Extracts Real Time Integrity from RRLP pdu and converts it to the
///      Glonav format.
///
/// \returns
///      Flag to indicate whether Real Time Integrity was available.
/// \retval #TRUE if available.
/// \retval #FALSE if unavailable.
//*****************************************************************************
BL rrlp_PDU_Get_Assist_realTimeIntegrity
(
   GPS_AssistData *p_GPS_AssistData,   ///< [in] Real Time Integrity within
                                       ///   delivered PDU.
   U1             *p_Num_Bad_SV,       ///< [out] Number of "Bad" satellites in the list.
   U1             Bad_SV_List[]        ///< [out] List of "Bad" Satellites ID (PRN) Numbers [range 1..32].
)
{
   // -- Real-Time Integrity
   // -- number of bad satellites can be read from this element
   // SeqOf-BadSatelliteSet ::= SEQUENCE (SIZE(1..16)) OF SatelliteID

   // SatelliteID ::= INTEGER (0..63)  -- identifies satellite

   U1 RealTimeIntegrityCount;

   if ( p_GPS_AssistData != NULL )
   {
       if ( p_GPS_AssistData->controlHeader.realTimeIntegrity != NULL )
      {
         RRLP_SeqOf_BadSatelliteSet_t      *p_SeqOf_BadSatelliteSet = p_GPS_AssistData->controlHeader.realTimeIntegrity;
        // SeqOf_BadSatelliteSet_yptr p_BadSatelliteSetElement = p_SeqOf_BadSatelliteSet->First;

         RRLP_SatelliteID_t  *p_Rti = (RRLP_SatelliteID_t *)p_SeqOf_BadSatelliteSet->list.array;

         *p_Num_Bad_SV = 0;

         for ( RealTimeIntegrityCount = 0;
             RealTimeIntegrityCount < p_SeqOf_BadSatelliteSet->list.count &&
                  RealTimeIntegrityCount < 16 && (p_Rti != NULL); // number of SatelliteID in SeqOf-BadSatelliteSet
               RealTimeIntegrityCount++ )
         {
            // U4 SV_Health_Mask          // Satellite Health bit Mask, where bits 0..31 are for SV ID's 1..32, and 1=Healthy, 0=Unhealthy.
            // SatelliteID ::= INTEGER (0..63)  -- identifies satellite
            // SatelliteID can be from 0..63, make sure it's within range for SV_Health_Mask i.e. 0..31.
            if ( *p_Rti < 32 )   //need to be check -raghu
            {
               Bad_SV_List[*p_Num_Bad_SV] =(U1) (*p_Rti + 1);
               (*p_Num_Bad_SV)++;
            }
            // Point to the next element to process.
            p_Rti++;
            //p_BadSatelliteSetElement = p_BadSatelliteSetElement->Suc;
         }
         return TRUE;
      }
   }
   return FALSE;
}


//*****************************************************************************
/// \brief
///      Extracts a single Almanac Element from RRLP pdu and converts it to the
///      #s_GN_AGPS_Alm_El format.
///
/// \returns
///      Flag to indicate whether Almanac Element was available.
/// \retval #TRUE if available.
/// \retval #FALSE if unavailable.
//*****************************************************************************
BL rrlp_PDU_Get_Assist_almanacElement_El
(
   RRLP_AlmanacElement_t    *p_AlmanacElement,   ///< [in] Almanac Element within
                                          ///   delivered PDU.
   s_GN_AGPS_Alm_El  *p_GN_AGPS_Alm_El    ///< [out] Glonav format Almanac Element.
)
{
   // The Almanac week number is not available in the individual elements, this is set elsewhere.
   // U1 WNa;                    // Almanac Reference Week             :  8 bits  [0..255]

   // U1 SatID;                  // Satellite ID                       :  6 bits  [1..32]
   // satelliteID       INTEGER (0..63)   -- identifies satellite,
   p_GN_AGPS_Alm_El->SatID       = (U1) p_AlmanacElement->satelliteID + 1; // Count from 1-32 iso 0-31

   // U2 e;                      // Eccentricity                       : 16 bits  [x 2^-21]
   // almanacE          INTEGER (0..65535),
   p_GN_AGPS_Alm_El->e           = (U2) p_AlmanacElement->almanacE ;

   // U1 toa;                    // Reference Time of Week             :  8 bits  [x 2^12 sec]
   // alamanacToa       INTEGER (0..255),
   p_GN_AGPS_Alm_El->toa         = (U1) p_AlmanacElement->alamanacToa ;

   // I2 delta_I;                // Delta_Inclination Angle            : 16 bits  [x 2^-19 semi-circles]
   // almanacKsii       INTEGER (-32768..32767),
   p_GN_AGPS_Alm_El->delta_I     = (I2) p_AlmanacElement->almanacKsii ;

   // I2 OmegaDot;               // Rate of Right Ascension            : 16 bits  [x 2^-38 semi-circles/sec]
   // almanacOmegaDot   INTEGER (-32768..32767),
   p_GN_AGPS_Alm_El->OmegaDot    = (I2) p_AlmanacElement->almanacOmegaDot ;

   // U1 SVHealth;               // Satellite Health Bits              :  8 bits  [0..255]
   // almanacSVhealth   INTEGER (0..255),
   p_GN_AGPS_Alm_El->SVHealth    = (U1) p_AlmanacElement->almanacSVhealth ;

   // U4 APowerHalf;             // (Semi-Major Axis)^1/2              : 24 bits  [x 2^-11 meters^1/2]
   // almanacAPowerHalf INTEGER (0..16777215),
   p_GN_AGPS_Alm_El->APowerHalf  = (U4) p_AlmanacElement->almanacAPowerHalf ;

   // I4 Omega0;                 // Longitude of the Ascending Node    : 24 bits  [x 2^-23 semi-circles]
   // almanacOmega0     INTEGER (-8388608..8388607),
   p_GN_AGPS_Alm_El->Omega0      = (I4) p_AlmanacElement->almanacOmega0 ;

   // I4 w;                      // Argument of Perigee                : 24 bits  [x 2^-23 semi-circles]
   // almanacW          INTEGER (-8388608..8388607),
   p_GN_AGPS_Alm_El->w           = (I4) p_AlmanacElement->almanacW ;

   // I4 M0;                     // Mean Anomaly                       : 24 bits  [x 2^-23 semi-circles]
   // almanacM0         INTEGER (-8388608..8388607),
   p_GN_AGPS_Alm_El->M0          = (I4) p_AlmanacElement->almanacM0 ;

   // I2 af0;                    // SV Clock Bias                      : 11 bits  [x 2^-20 seconds]
   // almanacAF0        INTEGER (-1024..1023),
   p_GN_AGPS_Alm_El->af0         = (I2) p_AlmanacElement->almanacAF0 ;

   // I2 af1;                    // SV Clock Drift                     : 11 bits  [x 2^-38 sec/sec]
   // almanacAF1        INTEGER (-1024..1023)
   p_GN_AGPS_Alm_El->af1         = (I2) p_AlmanacElement->almanacAF1 ;

   return TRUE;
}


//*****************************************************************************
/// \brief
///      Extracts a single NavModel Element from RRLP pdu and converts it to the
///      #s_GN_AGPS_Eph_El format.
///
/// \returns
///      Flag to indicate whether NavModel Element was available.
/// \retval #TRUE if available.
/// \retval #FALSE if unavailable.
//*****************************************************************************
BL rrlp_PDU_Get_Assist_navModelElement_El
(
   RRLP_NavModelElement_t   *p_NavModelElement,  ///< [in] NavModel Element within
                                          ///   delivered PDU.
   s_GN_AGPS_Eph_El  *p_GN_AGPS_Eph_El    ///< [out] Glonav format
                                          ///   NavModel Element.
)
{
   RRLP_UncompressedEphemeris_t *p_UncompressedEphemeris;

   switch ( p_NavModelElement->satStatus.present)
   {
   case RRLP_SatStatus_PR_newSatelliteAndModelUC:
       p_UncompressedEphemeris = &p_NavModelElement->satStatus.choice.newSatelliteAndModelUC;
      break;
   case RRLP_SatStatus_PR_newNaviModelUC:
       p_UncompressedEphemeris = &p_NavModelElement->satStatus.choice.newNaviModelUC;
      break;
   case RRLP_SatStatus_PR_oldSatelliteAndModel:
   default:
      return FALSE;
      break;
   }
   // Satellite id is not available in the individual elements, this is set elsewhere.
   // U1 SatID;                  // Satellite ID (PRN)                       :  6 bits [1..32]

   // GPS Reference Week number is not available in the individual elements, this is set elsewhere.
   // U2 Week;                   // GPS Reference Week Number                : 10 bits [0..1023]

   // U1 CodeOnL2;               // C/A or P on L2                           :  2 bits [0..3]
   // ephemCodeOnL2  INTEGER (0..3),
   p_GN_AGPS_Eph_El->CodeOnL2       = (U1) p_UncompressedEphemeris->ephemCodeOnL2;

   // U1 URA;                    // User Range Accuracy Index                :  4 bits [0..15]
   // ephemURA    INTEGER (0..15),
   p_GN_AGPS_Eph_El->URA            = (U1) p_UncompressedEphemeris->ephemURA;

   // U1 SVHealth;               // Satellite Health Bits                    :  6 bits [0..63]
   // ephemSVhealth     INTEGER (0..63),
   p_GN_AGPS_Eph_El->SVHealth       = (U1) p_UncompressedEphemeris->ephemSVhealth;

   // U2 IODC;                   // Issue Of Data Clock                      : 10 bits [0..1023]
   // ephemIODC         INTEGER  (0..1023),
   p_GN_AGPS_Eph_El->IODC           = (U2) p_UncompressedEphemeris->ephemIODC;

   // I1 L2Pflag;                // L2 P Data Flag                           :  1 bit  [0..1]
   // ephemL2Pflag      INTEGER (0..1),
   p_GN_AGPS_Eph_El->L2Pflag        = (I1) p_UncompressedEphemeris->ephemL2Pflag;

   // ephemSF1Rsvd      ,
   //          reserved1      INTEGER (0..8388607),   -- 23-bit field
   //          reserved2      INTEGER (0..16777215),  -- 24-bit field
   //          reserved3      INTEGER (0..16777215),  -- 24-bit field
   //          reserved4      INTEGER (0..65535)      -- 16-bit field

   // I1 TGD;                    // Total Group Delay                        :  8 bits [x 2^-31 sec]
   // ephemTgd          INTEGER (-128..127),
   p_GN_AGPS_Eph_El->TGD            = (I1) p_UncompressedEphemeris->ephemTgd;

   // U2 toc;                    // Clock Reference Time of Week             : 16 bits [x 2^4 sec]
   // ephemToc          INTEGER (0..37799),
   p_GN_AGPS_Eph_El->toc            = (U2) p_UncompressedEphemeris->ephemToc;

   // I1 af2;                    // SV Clock Drift Rate                      :  8 bits [x 2^-55 sec/sec2]
   // ephemAF2          INTEGER (-128..127),
   p_GN_AGPS_Eph_El->af2            = (I1) p_UncompressedEphemeris->ephemAF2;

   // I2 af1;                    // SV Clock Drift                           : 16 bits [x 2^-43 sec/sec]
   // ephemAF1          INTEGER (-32768..32767),
   p_GN_AGPS_Eph_El->af1            = (I2) p_UncompressedEphemeris->ephemAF1;

   // I4 af0;                    // SV Clock Bias                            : 22 bits [x 2^-31 sec]
   // ephemAF0          INTEGER (-2097152..2097151),
   p_GN_AGPS_Eph_El->af0            = (I4) p_UncompressedEphemeris->ephemAF0;

   // I2 Crs;                    // Coefficient-Radius-sine                  : 16 bits [x 2^-5 meters]
   // ephemCrs          INTEGER (-32768..32767),
   p_GN_AGPS_Eph_El->Crs            = (I2) p_UncompressedEphemeris->ephemCrs;

   // I2 dn;                     // Delta n                                  : 16 bits [x 2^-43 semi-circles/sec]
   // ephemDeltaN       INTEGER (-32768..32767),
   p_GN_AGPS_Eph_El->dn             = (I2) p_UncompressedEphemeris->ephemDeltaN;

   // I4 M0;                     // Mean Anomaly                             : 32 bits [x 2^-31 semi-circles]
   // ephemM0           INTEGER (-2147483648..2147483647),
   p_GN_AGPS_Eph_El->M0             = (I4) p_UncompressedEphemeris->ephemM0;

   // I2 Cuc;                    // Coefficient-Argument_of_Latitude-cosine  : 16 bits [x 2^-29 radians]
   // ephemCuc          INTEGER (-32768..32767),
   p_GN_AGPS_Eph_El->Cuc            = (I2) p_UncompressedEphemeris->ephemCuc;

   // U4 e;                      // Eccentricity                             : 32 bits [x 2^-33]
   // ephemE            INTEGER (0..4294967295),
   p_GN_AGPS_Eph_El->e              = (U4) p_UncompressedEphemeris->ephemE ; //Removed this addition asper the data type + 2147483648;

   // I2 Cus;                    // Coefficient-Argument_of_Latitude-sine    : 16 bits [x 2^-29 radians]
   // ephemCus          INTEGER (-32768..32767),
   p_GN_AGPS_Eph_El->Cus            = (I2) p_UncompressedEphemeris->ephemCus;

   // U4 APowerHalf;             // (Semi-Major Axis)^1/2                    : 32 bits [x 2^-19 metres^1/2]
   // ephemAPowerHalf   INTEGER (0..4294967295),
   p_GN_AGPS_Eph_El->APowerHalf     = (U4) p_UncompressedEphemeris->ephemAPowerHalf; //Removed this addition asper the data type + 2147483648;

   // U2 toe;                    // Ephemeris Reference Time of Week         : 16 bits [x 2^4 sec]
   // ephemToe          INTEGER (0..37799),
   p_GN_AGPS_Eph_El->toe            = (U2) p_UncompressedEphemeris->ephemToe;

   // U1 FitIntFlag;             // Fit Interval Flag                        :  1 bit  [0=4hrs, 1=6hrs]
   // ephemFitFlag      INTEGER (0..1),
   p_GN_AGPS_Eph_El->FitIntFlag     = (U1) p_UncompressedEphemeris->ephemFitFlag;

   // U1 AODA;                   // Age Of Data Offset                       :  5 bits [x 900 sec]
   // ephemAODA         INTEGER (0..31),
   p_GN_AGPS_Eph_El->AODA           = (U1) p_UncompressedEphemeris->ephemAODA;

   // I2 Cic;                    // Coefficient-Inclination-cosine           : 16 bits [x 2^-29 radians]
   // ephemCic          INTEGER (-32768..32767),
   p_GN_AGPS_Eph_El->Cic            = (I2) p_UncompressedEphemeris->ephemCic;

   // I4 Omega0;                 // Longitude of the Ascending Node          : 32 bits [x 2^-31 semi-circles]
   // ephemOmegaA0      INTEGER (-2147483648..2147483647),
   p_GN_AGPS_Eph_El->Omega0         = (I4) p_UncompressedEphemeris->ephemOmegaA0;

   // I2 Cis;                    // Coefficient-Inclination-sine             : 16 bits [x 2^-29 radians]
   // ephemCis          INTEGER (-32768..32767),
   p_GN_AGPS_Eph_El->Cis            = (I2) p_UncompressedEphemeris->ephemCis;

   // I4 i0;                     // Inclination angle                        : 32 bits [x 2^-31 semi-circles]
   // ephemI0           INTEGER (-2147483648..2147483647),
   p_GN_AGPS_Eph_El->i0             = (I4) p_UncompressedEphemeris->ephemI0;

   // I2 Crc;                    // Coefficient-Radius-cosine                : 16 bits [x 2^-5 meters]
   // ephemCrc          INTEGER (-32768..32767),
   p_GN_AGPS_Eph_El->Crc            = (I2) p_UncompressedEphemeris->ephemCrc;

   // I4 w;                      // Argument of Perigee                      : 32 bits [x 2^-31 meters]
   // ephemW            INTEGER (-2147483648..2147483647),
   p_GN_AGPS_Eph_El->w              = (I4) p_UncompressedEphemeris->ephemW;

   // I4 OmegaDot;               // Rate of Right Ascension                  : 24 bits [x 2^-43 semi-circles/sec]
   // ephemOmegaADot    INTEGER (-8388608..8388607),
   p_GN_AGPS_Eph_El->OmegaDot       = (I4) p_UncompressedEphemeris->ephemOmegaADot;

   // I2 IDot;                   // Rate of Inclination Angle                : 14 bits [x 2^-43 semi-circles/sec]
   // ephemIDot         INTEGER (-8192..8191)
   p_GN_AGPS_Eph_El->IDot           = (I2) p_UncompressedEphemeris->ephemIDot;

   return TRUE;
}


//*****************************************************************************
/// \brief
///      Extracts a Measure Response Time from RRLP pdu.
///
/// \returns
///      Flag to indicate whether Measure Response Time was available.
/// \retval #TRUE if available.
/// \retval #FALSE if unavailable.
//*****************************************************************************
BL rrlp_PDU_Get_PositionInstruct_measureResponseTime
(
   void  *p_ThisPDU,       ///< [in] Pointer to storage for PDU_TTCN_KEYWORD.
   U1    *p_ResponseTime,   ///< [out] Pointer to Measure Response Time.
   I1    *p_GADReponseTime ///< [out] Pointer to Measure Response Time in 2^n format.
)
{
   RRLP_PDU_t *p_PDU_Cast = p_ThisPDU;

   switch ( p_PDU_Cast->component.present )
   {
   case RRLP_RRLP_Component_PR_msrPositionReq:
       *p_ResponseTime = 1 << p_PDU_Cast->component.choice.msrPositionReq.positionInstruct.measureResponseTime;
       *p_GADReponseTime = (I1)p_PDU_Cast->component.choice.msrPositionReq.positionInstruct.measureResponseTime;
      return TRUE;
      break;
   case RRLP_RRLP_Component_PR_msrPositionRsp:    return FALSE; break;
   case RRLP_RRLP_Component_PR_assistanceData:    return FALSE; break;
   case RRLP_RRLP_Component_PR_assistanceDataAck: return FALSE; break;
   case RRLP_RRLP_Component_PR_protocolError:     return FALSE; break;
   default:
      OS_ASSERT( 0 );
      return FALSE;
      break;
   }
}


//*****************************************************************************
/// \brief
///      Gets a pointer to GPS_AssistData from RRLP pdu if present.
///
/// \returns
///      Pointer to GPS_AssistData, or NULL if none present in PDU.
//*****************************************************************************
GPS_AssistData *rrlp_PDU_Get_gps_AssistData
(
   void *p_ThisPDU   ///< [in] Pointer to storage for PDU_TTCN_KEYWORD.
)
{
   RRLP_PDU_t *p_PDU_Cast = p_ThisPDU;

   switch ( p_PDU_Cast->component.present )
   {
   case RRLP_RRLP_Component_PR_msrPositionReq:
       return p_PDU_Cast->component.choice.msrPositionReq.gps_AssistData;  //need to veryfy this assisgnment-Raghu
      break;
   case RRLP_RRLP_Component_PR_assistanceData:
      return p_PDU_Cast->component.choice.assistanceData.gps_AssistData;
      break;
   case RRLP_RRLP_Component_PR_msrPositionRsp:
   case RRLP_RRLP_Component_PR_assistanceDataAck:
   case RRLP_RRLP_Component_PR_protocolError:
   default:
      return NULL;
      break;
   }
   return NULL;
}

//need to check the belo function -Raghu
//*****************************************************************************
/// \brief
///      Sequences through the RRLP pdu processing assistance data.
///
/// \returns
///      Nothing.
//*****************************************************************************
void rrlp_PDU_ProcessAssistanceData
(
   void  *handle,
   void  *p_ThisPDU,    ///< [in] Pointer to storage for PDU_TTCN_KEYWORD.
   U4    PDU_TimeStamp  ///< [in] Time of arrival of PDU in OS_Time_ms
)
{
   // -- GPS assistance data IE
   // GPS-AssistData ::= SEQUENCE {
   //    controlHeader     ControlHeader
   // }
   // -- Control header of the GPS assistance data
   // ControlHeader ::= SEQUENCE {
   //
   //    -- Field type Present information
   //   /* RRLP_ControlHeader */
    //  typedef struct ControlHeader {
    //struct ReferenceTime    *referenceTime    /* OPTIONAL */;
    //struct RefLocation    *refLocation    /* OPTIONAL */;
    //struct DGPSCorrections    *dgpsCorrections    /* OPTIONAL */;
    //struct NavigationModel    *navigationModel    /* OPTIONAL */;
    //struct IonosphericModel    *ionosphericModel    /* OPTIONAL */;
    //struct UTCModel    *utcModel    /* OPTIONAL */;
    //struct Almanac    *almanac    /* OPTIONAL */;
    //struct AcquisAssist    *acquisAssist    /* OPTIONAL */;
    //struct SeqOf_BadSatelliteSet    *realTimeIntegrity    /* OPTIONAL */;

    /* Context for parsing across buffer boundaries */
    //asn_struct_ctx_t _asn_ctx;
//} ControlHeader_t;
   // }
   ///\todo Add support for DGPS Corrections.
      //    dgpsCorrections   DGPSCorrections         OPTIONAL,

   RRLP_PDU_t  *p_PDU_Cast = p_ThisPDU;
   BL                status;
   GPS_AssistData    *p_GPS_AssistData;

   s_GN_AGPS_Ref_Time   GN_AGPS_Ref_Time;
   s_GN_AGPS_GAD_Data   GN_AGPS_GAD_Data;
   s_GN_AGPS_UTC        GN_AGPS_UTC;
   s_GN_AGPS_Ion        GN_AGPS_Ion;
   s_GN_AGPS_Alm_El     GN_AGPS_Alm_El;
   s_GN_AGPS_Eph_El     GN_AGPS_Eph_El;
   s_GN_AGPS_Acq_Ass    GN_AGPS_Acq_Ass;
   U1                   Num_Bad_SV;          // Number of "Bad" satellites in the list.
   U1                   Bad_SV_List[32];     // List of "Bad" Satellites ID (PRN) Numbers [range 1..32].

#if defined( SUPL_MSB_NO_ACQ_ASSIST ) || defined( SUPL_USE_PURE_MSA_MODE )
    e_GN_methodType GN_methodType = RRLP_Instance_From_Handle(handle)->p_RRLP->GN_RRLP_MethodType.methodType;
#endif

   p_GPS_AssistData = rrlp_PDU_Get_gps_AssistData( p_ThisPDU );

   if ( p_GPS_AssistData != NULL )
   {
      //    referenceTime     ReferenceTime           OPTIONAL,
      status = rrlp_PDU_Get_Assist_referenceTime( p_GPS_AssistData, &GN_AGPS_Ref_Time, PDU_TimeStamp );
      if ( status )
      {
         status = GN_AGPS_Set_Ref_Time( &GN_AGPS_Ref_Time );
      }

#ifdef SUPL_USE_PURE_MSA_MODE
      if( GN_methodType != GN_msAssisted )
      {
#endif /* SUPL_USE_PURE_MSA_MODE */
#ifndef SUPL_DO_NOT_USE_REF_POSITION  /* this is disabled for some IOTs */
      //    refLocation       RefLocation             OPTIONAL,
      status = rrlp_PDU_Get_Assist_refLocation( p_GPS_AssistData, &GN_AGPS_GAD_Data );
#ifdef SUPL_SET_REF_POS_UNCERTAINTY_25KM
      GN_AGPS_GAD_Data.uncertaintySemiMajor = 93; // 25km uncertainty
      GN_AGPS_GAD_Data.uncertaintySemiMinor = 93; // 25km uncertainty
      GN_AGPS_GAD_Data.confidence = 95;
#endif
      if ( status )
      {
         status = GN_AGPS_Set_GAD_Ref_Pos( &GN_AGPS_GAD_Data );
      }
#endif /* #ifdef SUPL_DO_NOT_USE_REF_POSITION */

      //    utcModel          UTCModel                OPTIONAL,
      status = rrlp_PDU_Get_Assist_utcModel( p_GPS_AssistData, &GN_AGPS_UTC );
      if ( status )
      {
         status = GN_AGPS_Set_UTC( &GN_AGPS_UTC );
      }

      //    ionosphericModel  IonosphericModel        OPTIONAL,
      status = rrlp_PDU_Get_Assist_ionosphericModel( p_GPS_AssistData, &GN_AGPS_Ion );
      if ( status )
      {
         status = GN_AGPS_Set_Ion( &GN_AGPS_Ion );
      }
#ifdef SUPL_USE_PURE_MSA_MODE
      }
#endif /* SUPL_USE_PURE_MSA_MODE */

#ifdef SUPL_MSB_NO_ACQ_ASSIST
      if( GN_methodType != GN_msBased )
      {
#endif /* SUPL_USE_PURE_MSA_MODE */
      //    acquisAssist      AcquisAssist            OPTIONAL,
      status = rrlp_PDU_Get_Assist_acquisAssist( p_GPS_AssistData, &GN_AGPS_Acq_Ass );
      if ( status )
      {
        status = GN_AGPS_Set_Acq_Ass( &GN_AGPS_Acq_Ass );
      }
#ifdef SUPL_MSB_NO_ACQ_ASSIST
      }
#endif /* SUPL_USE_PURE_MSA_MODE */

#ifdef SUPL_USE_PURE_MSA_MODE
      if( GN_methodType != GN_msAssisted )
      {
#endif /* SUPL_USE_PURE_MSA_MODE */
#ifndef DO_NOT_USE_ALMANAC
      //    almanac           Almanac                 OPTIONAL,
          if ( p_GPS_AssistData->controlHeader.almanac != NULL)
      {
         U1 i;
         U1 AlmanacCount;
         RRLP_Almanac_t *p_Almanac;
         RRLP_AlmanacElement_t *p_AlmanacElement;
         p_Almanac = p_GPS_AssistData->controlHeader.almanac;
         p_AlmanacElement = *p_GPS_AssistData->controlHeader.almanac->almanacList.list.array;
         AlmanacCount = p_Almanac->almanacList.list.count ;

/*+LMSqcMultiSUPL*/
     //if(p_GPS_AssistData->controlHeader.almanac.alamanacWNa >= CurrentWeekNum)
         {
/*-LMSqcMultiSUPL*/

            GN_AGPS_Alm_El.WNa = (U1) p_Almanac->alamanacWNa;
             for ( i = 0 ; i < AlmanacCount && p_AlmanacElement != NULL ; i++ )
             {
                 p_AlmanacElement = p_GPS_AssistData->controlHeader.almanac->almanacList.list.array[i];
                if ( rrlp_PDU_Get_Assist_almanacElement_El( p_AlmanacElement, &GN_AGPS_Alm_El ) )
                {
                   #ifdef PDU_LOGGING
                      #if PDU_LOGGING == PDU_LOGGING_LEV_DBG
                         GN_AGPS_Log_Alm_El( &GN_AGPS_Alm_El );
                      #endif
                   #endif
                   status = GN_AGPS_Set_Alm_El( &GN_AGPS_Alm_El );
                }
                //p_NextAlmanacElement = p_NextAlmanacElement->Suc;
                //p_AlmanacElement++;
             }
/*+LMSqcMultiSUPL*/
       }
/*-LMSqcMultiSUPL*/


      }
#endif /* #ifndef DO_NOT_USE_ALMANAC */

      //    navigationModel   NavigationModel-RRLP    OPTIONAL,
          if ( p_GPS_AssistData->controlHeader.navigationModel != NULL )
      {
         U1 i;
         U1 NavigationModelCount;
         RRLP_NavigationModel_t *p_NavigationModel;
         RRLP_NavModelElement_t  *p_NavModelElement;
        // SeqOfNavModelElement_yptr p_NextNavModelElement = p_GPS_AssistData->controlHeader.navigationModel.navModelList.First;
         p_NavigationModel = p_GPS_AssistData->controlHeader.navigationModel;
         p_NavModelElement = (RRLP_NavModelElement_t*)p_NavigationModel->navModelList.list.array;

         GN_AGPS_Eph_El.Week  = 0;
         NavigationModelCount = p_NavigationModel->navModelList.list.count;
         for ( i = 0 ; i < NavigationModelCount && (p_NavModelElement != NULL) ; i++ )
         {
             p_NavModelElement = p_GPS_AssistData->controlHeader.navigationModel->navModelList.list.array[i];
            if ( rrlp_PDU_Get_Assist_navModelElement_El( p_NavModelElement, &GN_AGPS_Eph_El ) != 0 )
            {
                U4 a_EphReservedWords[4];
                GN_AGPS_Eph_El.SatID = (U1) p_NavModelElement->satelliteID + 1;
               #ifdef PDU_LOGGING
                  #if PDU_LOGGING == PDU_LOGGING_LEV_DBG
                     GN_AGPS_Log_Eph_El( &GN_AGPS_Eph_El );
                  #endif
               #endif
               status = GN_AGPS_Set_Eph_El( &GN_AGPS_Eph_El );

                a_EphReservedWords[0] = p_NavModelElement->satStatus.choice.newNaviModelUC.ephemSF1Rsvd.reserved1;
                a_EphReservedWords[1] = p_NavModelElement->satStatus.choice.newNaviModelUC.ephemSF1Rsvd.reserved2;
                a_EphReservedWords[2] = p_NavModelElement->satStatus.choice.newNaviModelUC.ephemSF1Rsvd.reserved3;
                a_EphReservedWords[3] = p_NavModelElement->satStatus.choice.newNaviModelUC.ephemSF1Rsvd.reserved4;

                status = GN_AGPS_Set_Eph_Res(GN_AGPS_Eph_El.SatID, a_EphReservedWords);
            }
         }
      }

      //    realTimeIntegrity SeqOf-BadSatelliteSet   OPTIONAL
      status = rrlp_PDU_Get_Assist_realTimeIntegrity( p_GPS_AssistData, &Num_Bad_SV, Bad_SV_List );
      if ( status )
      {
         status = GN_AGPS_Set_Bad_SV_List( Num_Bad_SV, Bad_SV_List );
      }
#ifdef SUPL_USE_PURE_MSA_MODE
      }
#endif /* SUPL_USE_PURE_MSA_MODE */

      /* @Todo : THis has to be implemented in ASN1 independent manner */
      GN_RRLP_Log_AssistRecvd( handle , p_GPS_AssistData );
   }
}


//*****************************************************************************
/// \brief
///      Adds Reference Time to RRLP GPS_AssistData structure.
///
/// \returns
///      Flag to indicate whether adding the Reference Time was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
//*****************************************************************************
BL rrlp_PDU_Make_Assist_referenceTime
(
   GPS_AssistData *p_GPS_AssistData ///< [out] Assist data where the
                                    ///   Reference Time is to be added.
)
{
   // ReferenceTime ::= SEQUENCE {
   //    gpsTime           GPSTime,
   //    gsmTime           GSMTime           OPTIONAL,
   //    gpsTowAssist      GPSTOWAssist      OPTIONAL
   // }

   // -- GPS Time includes week number and time-of-week (TOW)
   // GPSTime ::= SEQUENCE {
   //    gpsTOW23b         GPSTOW23b,
   //    gpsWeek           GPSWeek
   // }
   U2 GPSWeek;
   U4 GPSTOWMS;

   if ( p_GPS_AssistData != NULL )
   {
      GN_Assist_Get_RefTime( &GPSWeek, &GPSTOWMS );
      /*
      if(p_GPS_AssistData->controlHeader.referenceTime == NULL)
      {
        ReferenceTime_t *p_ReferenceTime = p_GPS_AssistData->controlHeader.referenceTime;
        p_ReferenceTime = (ReferenceTime_t *)
        */  // need to check-Raghu
      //p_GPS_AssistData->controlHeader.referenceTimePresent = SDL_True;

      // -- GPS week number
      // GPSWeek ::= INTEGER (0..1023)
      p_GPS_AssistData->controlHeader.referenceTime->gpsTime.gpsWeek = GPSWeek % 1024;

      // -- GPSTOW, range 0-604799.92, resolution 0.08 sec, 23-bit presentation
      // GPSTOW23b ::= INTEGER (0..7559999)
      p_GPS_AssistData->controlHeader.referenceTime->gpsTime.gpsTOW23b =  GPSTOWMS /(U4) ( 1000 * 0.08 );

      return TRUE;
   }
   return FALSE;
}



//*****************************************************************************
/// \brief
///      Adds Reference Location to RRLP GPS_AssistData structure.
///
/// \returns
///      Flag to indicate whether adding the Reference Location was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
//*****************************************************************************
BL rrlp_PDU_Make_Assist_refLocation
(
   GPS_AssistData       *p_GPS_AssistData,   ///< [out] Assist data where the
                                             ///   Reference Location is to be
                                             ///   added.
   s_GN_AGPS_GAD_Data   *p_GN_AGPS_GAD_Data  ///< [in] Source of assistance data in
                                             ///   GloNAV #s_GN_AGPS_GAD_Data format.
)
{
   // -- Reference Location IE
   // RefLocation ::= SEQUENCE {
   //    threeDLocation       Ext-GeographicalInformation
   // }

   RRLP_RefLocation_t    *p_refLocation;
   s_GAD_Message  GAD_Message = { 14, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } };
   BL             status;

   if ( p_GPS_AssistData != NULL )
   {
      status = GN_GAD_Message_From_AGPS_GAD_Data( p_GN_AGPS_GAD_Data, &GAD_Message );
      if ( status == FALSE )
      {
         return FALSE;
      }

      //p_GPS_AssistData->controlHeader.refLocationPresent = SDL_True;
      p_refLocation = p_GPS_AssistData->controlHeader.refLocation;

      if ( p_refLocation->threeDLocation.buf == NULL )
      {
         p_refLocation->threeDLocation.buf = GN_Calloc( 1, GAD_Message.buflen );
         p_refLocation->threeDLocation.size = GAD_Message.buflen;
      }

      if ( p_refLocation->threeDLocation.size >= GAD_Message.buflen )
      {
         memcpy(
             p_refLocation->threeDLocation.buf,
            &GAD_Message.message,
            GAD_Message.buflen );
        // p_refLocation->threeDLocation.IsAssigned = XASS;
         return TRUE;
      }
   }
   return FALSE;
}


//*****************************************************************************
/// \brief
///      Adds Ionospheric Model to RRLP GPS_AssistData structure.
///
/// \returns
///      Flag to indicate whether adding the Ionospheric Model was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
//*****************************************************************************
BL rrlp_PDU_Make_Assist_ionosphericModel
(
   GPS_AssistData *p_GPS_AssistData    ///< [out] Assist data where the
                                       ///   Ionospheric Model is to be added.
)
{
   RRLP_IonosphericModel_t  *p_ionosphericModel;
   s_GN_AGPS_Ion     GN_AGPS_Ion;
   BL                status;

   status = GN_Assist_Get_Ion( &GN_AGPS_Ion );

   if ( status )
   {
      // -- Ionospheric Model IE
      // IonosphericModel ::= SEQUENCE {

       p_ionosphericModel = p_GPS_AssistData->controlHeader.ionosphericModel;

      //    alfa0       INTEGER (-128..127),
      // I1 a0;                     // Klobuchar - alpha 0   : 8 bits  [x 2^-30      seconds]
      p_ionosphericModel->alfa0 = GN_AGPS_Ion.a0;

      //    alfa1       INTEGER (-128..127),
      // I1 a1;                     // Klobuchar - alpha 1   : 8 bits  [x 2^-27/PI   seconds/semi-circle]
      p_ionosphericModel->alfa1 = GN_AGPS_Ion.a1;

      //    alfa2       INTEGER (-128..127),
      // I1 a2;                     // Klobuchar - alpha 2   : 8 bits  [x 2^-24/PI^2 seconds/semi-circle^2]
      p_ionosphericModel->alfa2 = GN_AGPS_Ion.a2;

      //    alfa3       INTEGER (-128..127),
      // I1 a3;                     // Klobuchar - alpha 3   : 8 bits  [x 2^-24/PI^3 seconds/semi-circle^3]
      p_ionosphericModel->alfa3 = GN_AGPS_Ion.a3;

      //    beta0       INTEGER (-128..127),
      // I1 b0;                     // Klobuchar - beta 0    : 8 bits  [x 2^11       seconds]
      p_ionosphericModel->beta0 = GN_AGPS_Ion.b0;

      //    beta1       INTEGER (-128..127),
      // I1 b1;                     // Klobuchar - beta 1    : 8 bits  [x 2^14/PI    seconds/semi-circle]
      p_ionosphericModel->beta1 = GN_AGPS_Ion.b1;

      //    beta2       INTEGER (-128..127),
      // I1 b2;                     // Klobuchar - beta 2    : 8 bits  [x 2^16/PI^2  seconds/semi-circle^2]
      p_ionosphericModel->beta2 = GN_AGPS_Ion.b2;

      //    beta3       INTEGER (-128..127)
      // I1 b3;                     // Klobuchar - beta 3    : 8 bits  [x 2^16/PI^3  seconds/semi-circle^3]
      p_ionosphericModel->beta3 = GN_AGPS_Ion.b3;

      //p_GPS_AssistData->controlHeader.ionosphericModelPresent = SDL_True;

      return TRUE;
   }
   return status;
}


//*****************************************************************************
/// \brief
///      Adds UTC Model to RRLP GPS_AssistData structure.
///
/// \returns
///      Flag to indicate whether adding the UTC Model was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
//*****************************************************************************
BL rrlp_PDU_Make_Assist_utcModel
(
   GPS_AssistData *p_GPS_AssistData ///< [out] Assist data where the
                                    ///   UTC Model is to be added.
)
{
   RRLP_UTCModel_t       *p_utcModel;
   s_GN_AGPS_UTC  GN_AGPS_UTC;
   BL             status;

   status = GN_Assist_Get_UTC( &GN_AGPS_UTC );

   if ( status )
   {
      // -- Universal Time Coordinate Model
      // UTCModel ::= SEQUENCE {
      p_utcModel = p_GPS_AssistData->controlHeader.utcModel;

      //    utcA1          INTEGER (-8388608..8388607),
      // I4 A1;                     // UTC model - parameter A1               [x 2^-50 seconds/second]
      p_utcModel->utcA1          = GN_AGPS_UTC.A1;

      //    utcA0          INTEGER (-2147483648..2147483647),
      // I4 A0;                     // UTC model - parameter A0               [x 2^-30 seconds]
      p_utcModel->utcA0          = GN_AGPS_UTC.A0;

      //    utcTot         INTEGER (0..255),
      // U1 Tot;                    // UTC model - reference time of week     [x 2^12 seconds]
      p_utcModel->utcTot         = GN_AGPS_UTC.Tot;

      //    utcWNt         INTEGER (0..255),
      // U1 WNt;                    // UTC model - reference week number      [weeks]
      p_utcModel->utcWNt         = GN_AGPS_UTC.WNt;

      //    utcDeltaTls    INTEGER (-128..127),
      // I1 dtLS;                   // UTC model - time difference due to leap seconds before event  [seconds]
      p_utcModel->utcDeltaTls    = GN_AGPS_UTC.dtLS;

      //    utcWNlsf       INTEGER (0..255),
      // U1 WNLSF;                  // UTC model - week number when next leap second event occurs    [weeks]
      p_utcModel->utcWNlsf       = GN_AGPS_UTC.WNLSF;

      //    utcDN          INTEGER (-128..127),
      // U1 DN;                     // UTC model - day of week when next leap second event occurs    [days]
      p_utcModel->utcDN          = GN_AGPS_UTC.DN;

      //    utcDeltaTlsf   INTEGER (-128..127)
      // I1 dtLSF;                  // UTC model - time difference due to leap seconds after event   [seconds]
      p_utcModel->utcDeltaTlsf   = GN_AGPS_UTC.dtLSF;

     // p_GPS_AssistData->controlHeader.utcModelPresent = SDL_True;

      return TRUE;
   }
   return status;
}


//*****************************************************************************
/// \brief
///      Populates a GPS_AssistData structure with a set of ephemerides.
///
/// \returns
///      Flag to indicate whether adding the set of ephemerides was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
//*****************************************************************************
#ifdef SUPL_SERVER_FTR
BL rrlp_PDU_Make_Assist_navigationModel
(
   GPS_AssistData       *p_GPS_AssistData,   ///< [out] Assist data where the
                                             ///   Navigation Model is to be
                                             ///   added.
   s_GN_AGPS_GAD_Data   *p_GN_AGPS_GAD_Data  ///< [in] Source of assistance data in
                                             ///   GloNAV #s_GN_AGPS_GAD_Data format.
)
{
   U1                Valid_SV_Count       = 0;
   BL                NavModel_Available   = FALSE;
   U1                NavModel_Count       = 0;
   s_GN_AGPS_Eph_El  GN_AGPS_Eph_El_List[32];
   U1                SVID;

   //SeqOfNavModelElement_yrec *p_NavModelElement;
   //SeqOfNavModelElement_yptr *p_p_thisNavModel;
   RRLP_NavigationModel_t *p_NavigationModel;
   RRLP_NavModelElement_t  *p_NavModelElement;

   memset( GN_AGPS_Eph_El_List, 0, sizeof( GN_AGPS_Eph_El_List ) );

   GN_Assist_Get_Visible_Eph_List( &GN_AGPS_Eph_El_List[0] );

   // Set this here to remove "possibly used before being set" warnings.
   p_NavigationModel = p_GPS_AssistData->controlHeader.navigationModel;
   //p_NavModelElement = p_NavigationModel->navModelList.list.array;

   for ( SVID = 1 ; SVID <= 32 ; SVID++ )
   {
      if ( GN_AGPS_Eph_El_List[SVID - 1].SatID == SVID )
      {
         RRLP_UncompressedEphemeris_t *p_UncompressedEphemeris;
         if ( ! NavModel_Available )
         {
            // Make sure we don't do this more than once.
            NavModel_Available = TRUE;
           // p_NavModelElement = &p_NavigationModel->navModelList.list.array;
         }

         NavModel_Count++;
         GN_AGPS_Log_Eph_El( &GN_AGPS_Eph_El_List[SVID - 1] );

         p_NavModelElement = (RRLP_NavModelElement_t *) GN_Calloc( 1, sizeof( RRLP_NavModelElement_t ) );
         // U1 SatID;                  // Satellite ID (PRN)                       :  6 bits [1..32]
         p_NavModelElement->satelliteID       = GN_AGPS_Eph_El_List[SVID - 1].SatID - 1;
         p_NavModelElement->satStatus.present = SatStatus_PR_newNaviModelUC;

         p_UncompressedEphemeris = &p_NavModelElement->satStatus.choice.newNaviModelUC;

         // GPS Reference Week number is not available in the individual elements, this is set elsewhere.
         // U2 Week;                   // GPS Reference Week Number                : 10 bits [0..1023]

         // ephemCodeOnL2  INTEGER (0..3),
         // U1 CodeOnL2;               // C/A or P on L2                           :  2 bits [0..3]
         p_UncompressedEphemeris->ephemCodeOnL2    = GN_AGPS_Eph_El_List[SVID - 1].CodeOnL2;

         // ephemURA    INTEGER (0..15),
         // U1 URA;                    // User Range Accuracy Index                :  4 bits [0..15]
         p_UncompressedEphemeris->ephemURA         = GN_AGPS_Eph_El_List[SVID - 1].URA;

         // ephemSVhealth     INTEGER (0..63),
         // U1 SVHealth;               // Satellite Health Bits                    :  6 bits [0..63]
         p_UncompressedEphemeris->ephemSVhealth    = GN_AGPS_Eph_El_List[SVID - 1].SVHealth;

         // ephemIODC         INTEGER  (0..1023),
         // U2 IODC;                   // Issue Of Data Clock                      : 10 bits [0..1023]
         p_UncompressedEphemeris->ephemIODC        = GN_AGPS_Eph_El_List[SVID - 1].IODC;

         // ephemL2Pflag      INTEGER (0..1),
         // I1 L2Pflag;                // L2 P Data Flag                           :  1 bit  [0..1]
         p_UncompressedEphemeris->ephemL2Pflag     = GN_AGPS_Eph_El_List[SVID - 1].L2Pflag;

         // ephemSF1Rsvd      ,
         //          reserved1      INTEGER (0..8388607),   -- 23-bit field
         p_UncompressedEphemeris->ephemSF1Rsvd.reserved1 = 0;

         //          reserved2      INTEGER (0..16777215),  -- 24-bit field
         p_UncompressedEphemeris->ephemSF1Rsvd.reserved2 = 0;

         //          reserved3      INTEGER (0..16777215),  -- 24-bit field
         p_UncompressedEphemeris->ephemSF1Rsvd.reserved3 = 0;

         //          reserved4      INTEGER (0..65535)      -- 16-bit field
         p_UncompressedEphemeris->ephemSF1Rsvd.reserved4 = 0;

         // ephemTgd          INTEGER (-128..127),
         // I1 TGD;                    // Total Group Delay                        :  8 bits [x 2^-31 sec]
         p_UncompressedEphemeris->ephemTgd         = GN_AGPS_Eph_El_List[SVID - 1].TGD;

         // ephemToc          INTEGER (0..37799),
         // U2 toc;                    // Clock Reference Time of Week             : 16 bits [x 2^4 sec]
         p_UncompressedEphemeris->ephemToc         = GN_AGPS_Eph_El_List[SVID - 1].toc;

         // ephemAF2          INTEGER (-128..127),
         // I1 af2;                    // SV Clock Drift Rate                      :  8 bits [x 2^-55 sec/sec2]
         p_UncompressedEphemeris->ephemAF2         = GN_AGPS_Eph_El_List[SVID - 1].af2;

         // ephemAF1          INTEGER (-32768..32767),
         // I2 af1;                    // SV Clock Drift                           : 16 bits [x 2^-43 sec/sec]
         p_UncompressedEphemeris->ephemAF1         = GN_AGPS_Eph_El_List[SVID - 1].af1;

         // ephemAF0          INTEGER (-2097152..2097151),
         // I4 af0;                    // SV Clock Bias                            : 22 bits [x 2^-31 sec]
         p_UncompressedEphemeris->ephemAF0         = GN_AGPS_Eph_El_List[SVID - 1].af0;

         // ephemCrs          INTEGER (-32768..32767),
         // I2 Crs;                    // Coefficient-Radius-sine                  : 16 bits [x 2^-5 meters]
         p_UncompressedEphemeris->ephemCrs         = GN_AGPS_Eph_El_List[SVID - 1].Crs;

         // ephemDeltaN       INTEGER (-32768..32767),
         // I2 dn;                     // Delta n                                  : 16 bits [x 2^-43 semi-circles/sec]
         p_UncompressedEphemeris->ephemDeltaN      = GN_AGPS_Eph_El_List[SVID - 1].dn;

         // ephemM0           INTEGER (-2147483648..2147483647),
         // I4 M0;                     // Mean Anomaly                             : 32 bits [x 2^-31 semi-circles]
         p_UncompressedEphemeris->ephemM0          = GN_AGPS_Eph_El_List[SVID - 1].M0;

         // ephemCuc          INTEGER (-32768..32767),
         // I2 Cuc;                    // Coefficient-Argument_of_Latitude-cosine  : 16 bits [x 2^-29 radians]
         p_UncompressedEphemeris->ephemCuc         = GN_AGPS_Eph_El_List[SVID - 1].Cuc;

         // ephemE            INTEGER (0..4294967295),
         // U4 e;                      // Eccentricity                             : 32 bits [x 2^-33]
         // Adjust for Telelogic ASN.1 encoder problems.
         p_UncompressedEphemeris->ephemE           = (I4) GN_AGPS_Eph_El_List[SVID - 1].e - 2147483648UL;

         // ephemCus          INTEGER (-32768..32767),
         // I2 Cus;                    // Coefficient-Argument_of_Latitude-sine    : 16 bits [x 2^-29 radians]
         p_UncompressedEphemeris->ephemCus         = GN_AGPS_Eph_El_List[SVID - 1].Cus;

         // ephemAPowerHalf   INTEGER (0..4294967295),
         // U4 APowerHalf;             // (Semi-Major Axis)^1/2                    : 32 bits [x 2^-19 metres^1/2]
         // Adjust for Telelogic ASN.1 encoder problems.
         p_UncompressedEphemeris->ephemAPowerHalf  = (I4) GN_AGPS_Eph_El_List[SVID - 1].APowerHalf - 2147483648UL;

         // ephemToe          INTEGER (0..37799),
         // U2 toe;                    // Ephemeris Reference Time of Week         : 16 bits [x 2^4 sec]
         p_UncompressedEphemeris->ephemToe         = GN_AGPS_Eph_El_List[SVID - 1].toe;

         // ephemFitFlag      INTEGER (0..1),
         // U1 FitIntFlag;             // Fit Interval Flag                        :  1 bit  [0=4hrs, 1=6hrs]
         p_UncompressedEphemeris->ephemFitFlag     = GN_AGPS_Eph_El_List[SVID - 1].FitIntFlag;

         // ephemAODA         INTEGER (0..31),
         // U1 AODA;                   // Age Of Data Offset                       :  5 bits [x 900 sec]
         p_UncompressedEphemeris->ephemAODA        = GN_AGPS_Eph_El_List[SVID - 1].AODA;

         // ephemCic          INTEGER (-32768..32767),
         // I2 Cic;                    // Coefficient-Inclination-cosine           : 16 bits [x 2^-29 radians]
         p_UncompressedEphemeris->ephemCic         = GN_AGPS_Eph_El_List[SVID - 1].Cic;

         // ephemOmegaA0      INTEGER (-2147483648..2147483647),
         // I4 Omega0;                 // Longitude of the Ascending Node          : 32 bits [x 2^-31 semi-circles]
         p_UncompressedEphemeris->ephemOmegaA0     = GN_AGPS_Eph_El_List[SVID - 1].Omega0;

         // ephemCis          INTEGER (-32768..32767),
         // I2 Cis;                    // Coefficient-Inclination-sine             : 16 bits [x 2^-29 radians]
         p_UncompressedEphemeris->ephemCis         = GN_AGPS_Eph_El_List[SVID - 1].Cis;

         // ephemI0           INTEGER (-2147483648..2147483647),
         // I4 i0;                     // Inclination angle                        : 32 bits [x 2^-31 semi-circles]
         p_UncompressedEphemeris->ephemI0          = GN_AGPS_Eph_El_List[SVID - 1].i0;

         // ephemCrc          INTEGER (-32768..32767),
         // I2 Crc;                    // Coefficient-Radius-cosine                : 16 bits [x 2^-5 meters]
         p_UncompressedEphemeris->ephemCrc         = GN_AGPS_Eph_El_List[SVID - 1].Crc;

         // ephemW            INTEGER (-2147483648..2147483647),
         // I4 w;                      // Argument of Perigee                      : 32 bits [x 2^-31 meters]
         p_UncompressedEphemeris->ephemW           = GN_AGPS_Eph_El_List[SVID - 1].w;

         // ephemOmegaADot    INTEGER (-8388608..8388607),
         // I4 OmegaDot;               // Rate of Right Ascension                  : 24 bits [x 2^-43 semi-circles/sec]
         p_UncompressedEphemeris->ephemOmegaADot   = GN_AGPS_Eph_El_List[SVID - 1].OmegaDot;

         // ephemIDot         INTEGER (-8192..8191)
         // I2 IDot;                   // Rate of Inclination Angle                : 14 bits [x 2^-43 semi-circles/sec]
         p_UncompressedEphemeris->ephemIDot        = GN_AGPS_Eph_El_List[SVID - 1].IDot;

         p_NavModelElement++;
        // *p_p_thisNavModel = p_NavModelElement;
         //p_p_thisNavModel = &p_NavModelElement->Suc;
         //p_GPS_AssistData->controlHeader.navigationModel.navModelList.Last = p_NavModelElement;
         //p_GPS_AssistData->controlHeader.navigationModel.navModelList.Length = NavModel_Count;
         //p_GPS_AssistData->controlHeader.navigationModel.navModelList.IsAssigned = XASS;
         // At least one element is available so mark as present.
         p_NavigationModel->navModelList.list.count = NavModel_Count;
        // p_GPS_AssistData->controlHeader.navigationModelPresent = SDL_True;
      }
   }
   return TRUE;
}
#endif


//*****************************************************************************
/// \brief
///      Populates a GPS_AssistData structure with a set of Almanacs.
///
/// \returns
///      Flag to indicate whether adding the set of Almanacs was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
//*****************************************************************************
BL rrlp_PDU_Make_Assist_almanac
(
   GPS_AssistData *p_GPS_AssistData    ///< [out] Assist data where the
                                       ///   Almanacs are to be
                                       ///   added.
)
{
   BL                Status;
   U1                SVID;
   U1                Almanac_Count     = 0;
   BL                Almanac_Available = FALSE;
   s_GN_AGPS_Alm_El  GN_AGPS_Alm_El_List[32];

   RRLP_Almanac_t *p_Almanac;
   RRLP_AlmanacElement_t *p_AlmanacElement;
   p_GPS_AssistData->controlHeader.almanac =(RRLP_Almanac_t*)GN_Calloc(1,sizeof(RRLP_Almanac_t));
   p_Almanac = p_GPS_AssistData->controlHeader.almanac;
   Status  = GN_Assist_Get_Alm_List( GN_AGPS_Alm_El_List );
   if ( Status == FALSE ) return FALSE;


   for ( SVID = 1 ; SVID <= 32 ; SVID++ )
   {
      if ( GN_AGPS_Alm_El_List[SVID - 1].SatID == SVID )
      {


         if ( ! Almanac_Available )
         {
            // At least one element is available so mark as present.
            //p_GPS_AssistData->controlHeader.almanacPresent = SDL_True;
            // Make sure we don't do this more than once.
            Almanac_Available = TRUE;
            //p_AlmanacElement = p_Almanac->almanacList.list.array;
            //p_p_thisAlmanac = &p_GPS_AssistData->controlHeader.almanac.almanacList.First;
         }

         p_Almanac->almanacList.list.array[SVID] = GN_Calloc( 1, sizeof( RRLP_AlmanacElement_t ) );

         p_AlmanacElement = p_Almanac->almanacList.list.array[SVID];
         Almanac_Count++;

         #ifdef PDU_LOGGING
            #if PDU_LOGGING == PDU_LOGGING_LEV_DBG
               GN_AGPS_Log_Alm_El( &GN_AGPS_Alm_El_List[SVID - 1] );
            #endif
         #endif

         // The Almanac week number is not available in the individual elements, this is set elsewhere.
         // U1 WNa;                    // Almanac Reference Week             :  8 bits  [0..255]
               p_GPS_AssistData->controlHeader.almanac->alamanacWNa = GN_AGPS_Alm_El_List[SVID - 1].WNa;

         // satelliteID       INTEGER (0..63)   -- identifies satellite,
         // U1 SatID;                  // Satellite ID                       :  6 bits  [1..32]
         p_AlmanacElement->satelliteID       = GN_AGPS_Alm_El_List[SVID - 1].SatID - 1;

         // almanacE          INTEGER (0..65535),
         // U2 e;                      // Eccentricity                       : 16 bits  [x 2^-21]
         p_AlmanacElement->almanacE          = GN_AGPS_Alm_El_List[SVID - 1].e;

         // alamanacToa       INTEGER (0..255),
         // U1 toa;                    // Reference Time of Week             :  8 bits  [x 2^12 sec]
         p_AlmanacElement->alamanacToa       = GN_AGPS_Alm_El_List[SVID - 1].toa;

         // almanacKsii       INTEGER (-32768..32767),
         // I2 delta_I;                // Delta_Inclination Angle            : 16 bits  [x 2^-19 semi-circles]
         p_AlmanacElement->almanacKsii       = GN_AGPS_Alm_El_List[SVID - 1].delta_I;

         // almanacOmegaDot   INTEGER (-32768..32767),
         // I2 OmegaDot;               // Rate of Right Ascension            : 16 bits  [x 2^-38 semi-circles/sec]
         p_AlmanacElement->almanacOmegaDot   = GN_AGPS_Alm_El_List[SVID - 1].OmegaDot;

         // almanacSVhealth   INTEGER (0..255),
         // U1 SVHealth;               // Satellite Health Bits              :  8 bits  [0..255]
         p_AlmanacElement->almanacSVhealth   = GN_AGPS_Alm_El_List[SVID - 1].SVHealth;

         // almanacAPowerHalf INTEGER (0..16777215),
         // U4 APowerHalf;             // (Semi-Major Axis)^1/2              : 24 bits  [x 2^-11 meters^1/2]
         p_AlmanacElement->almanacAPowerHalf = GN_AGPS_Alm_El_List[SVID - 1].APowerHalf;

         // almanacOmega0     INTEGER (-8388608..8388607),
         // I4 Omega0;                 // Longitude of the Ascending Node    : 24 bits  [x 2^-23 semi-circles]
         p_AlmanacElement->almanacOmega0     = GN_AGPS_Alm_El_List[SVID - 1].Omega0;

         // almanacW          INTEGER (-8388608..8388607),
         // I4 w;                      // Argument of Perigee                : 24 bits  [x 2^-23 semi-circles]
         p_AlmanacElement->almanacW          = GN_AGPS_Alm_El_List[SVID - 1].w;

         // almanacM0         INTEGER (-8388608..8388607),
         // I4 M0;                     // Mean Anomaly                       : 24 bits  [x 2^-23 semi-circles]
         p_AlmanacElement->almanacM0         = GN_AGPS_Alm_El_List[SVID - 1].M0;

         // almanacAF0        INTEGER (-1024..1023),
         // I2 af0;                    // SV Clock Bias                      : 11 bits  [x 2^-20 seconds]
         p_AlmanacElement->almanacAF0        = GN_AGPS_Alm_El_List[SVID - 1].af0;

         // almanacAF1        INTEGER (-1024..1023)
         // I2 af1;                    // SV Clock Drift                     : 11 bits  [x 2^-38 sec/sec]
         p_AlmanacElement->almanacAF1        = GN_AGPS_Alm_El_List[SVID - 1].af1;
      }
   }

   p_Almanac->almanacList.list.count = Almanac_Count;
   p_Almanac->almanacList.list.size = sizeof(RRLP_AlmanacElement_t) * Almanac_Count;

   return Almanac_Available;
//#endif //  # if 0 raghu
}


//*****************************************************************************
/// \brief
///      Populates a GPS_AssistData structure with a set of assistance.
///
/// \returns
///      Flag to indicate whether making the assistance data was successful.
/// \retval #TRUE if successful.
/// \retval #FALSE if unsuccessful.
//*****************************************************************************
#ifdef SUPL_SERRVER_FTR
BL rrlp_PDU_Make_GPS_AssistData
(
   GPS_AssistData             *p_GPS_AssistData,         ///< [out] Assist data where the
                                                         ///   Almanacs are to be added.
   s_GN_AGPS_GAD_Data         *p_GN_AGPS_GAD_Data,       ///< [in] Source of assistance data in
                                                         ///   GloNAV #s_GN_AGPS_GAD_Data format.
   s_GN_RequestedAssistData   *p_GN_RequestedAssistData  ///< [in] Requested assistance data to add to the PDU
)
{
   BL status;

   // Deliver reference location whether it's asked for or not.
   status = rrlp_PDU_Make_Assist_refLocation( p_GPS_AssistData, p_GN_AGPS_GAD_Data );

   if ( p_GN_RequestedAssistData->utcModelRequested )
   {
      status = rrlp_PDU_Make_Assist_ionosphericModel( p_GPS_AssistData );
   }

   if ( p_GN_RequestedAssistData->utcModelRequested )
   {
      status = rrlp_PDU_Make_Assist_utcModel(         p_GPS_AssistData );
   }

   if ( p_GN_RequestedAssistData->almanacRequested)
   {
      status = rrlp_PDU_Make_Assist_almanac(          p_GPS_AssistData );
   }

   if ( p_GN_RequestedAssistData->navigationModelRequested )
   {
      status = rrlp_PDU_Make_Assist_navigationModel(  p_GPS_AssistData, p_GN_AGPS_GAD_Data );
   }

   // Deliver reference time whether it's asked for or not.
   // Make reference time last thing to be encoded to minimize the delay.
   status = rrlp_PDU_Make_Assist_referenceTime(       p_GPS_AssistData );

   return TRUE;
}
#endif


//*****************************************************************************
/// \brief
///      Retrieves the RRLP reference number from a decoded PDU.
///
/// \returns
///      Reference Number from RRLP.
//*****************************************************************************
U1 rrlp_PDU_Get_Reference_Number
(
   void *p_PDU_Src               ///< [in] Pointer to generated RRLP structure
                                 ///   PDU_TTCN_KEYWORD. Passed in as (void *) to hide
                                 ///   implementation.
)
{
   RRLP_PDU_t *p_CastPDU = p_PDU_Src;
   return (U1) p_CastPDU->referenceNumber;
}


//*****************************************************************************
/// \brief
///      Sets the RRLP reference number in an unencoded PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void rrlp_PDU_Set_Reference_Number
(
   void  *p_PDU_Src,             ///< [out] Pointer to generated RRLP structure
                                 ///   PDU_TTCN_KEYWORD. Passed in as (void *) to hide
                                 ///   implementation.
   U2    ReferenceNumber         ///< [in] Reference Number
)
{
   RRLP_PDU_t *p_CastPDU = p_PDU_Src;
   // PDU ::= SEQUENCE {
   //    referenceNumber      INTEGER (0..7),
   //    component            RRLP-Component
   //}
   p_CastPDU->referenceNumber = (ReferenceNumber) % 8;
}


//*****************************************************************************
/// \brief
///      Adds the.
///
/// \returns
///      Nothing.
//*****************************************************************************
void rrlp_Add_GPS_MsrSetElement_gps_msrList
(
   RRLP_GPS_MsrSetElement_t *p_GPS_MsrSetElement,   ///< [in/out] Pointer to MsrSetElement within an unencoded PDU
   s_GN_AGPS_Meas    *p_GN_AGPS_Meas         ///< [in] Measurement report data to add to the PDU.
)
{
   //SeqOfGPS_MsrElement_yptr *p_p_GPS_MsrElement;
   //SeqOfGPS_MsrElement_yrec *p_GPS_MsrElement;
   RRLP_GPS_MsrElement_t *p_GPS_MsrElement;
   U1 MsrElementCount;

   // GPS-MsrSetElement ::= SEQUENCE {
   //    refFrame       INTEGER (0..65535)   OPTIONAL,   -- Reference Frame number
   //    gpsTOW         GPSTOW24b,                       -- GPS TOW
   //    -- Note that applicable range for refFrame is 0 - 42431
   //
   //    -- N_SAT can be read from number of elements of gps-msrList
   //
   //    gps-msrList    SeqOfGPS-MsrElement
   //}

   // -- 24 bit presentation for GPSTOW
   // GPSTOW24b ::= INTEGER (0..14399999)
   p_GPS_MsrSetElement->gpsTOW = p_GN_AGPS_Meas->Meas_GPS_TOW % 14400000;
   p_GPS_MsrSetElement->refFrame = NULL; //refframe not supported
   GN_RRLP_Log( "rrlp_Add_GPS_MsrSetElement_gps_msrList:GPSTOWms: %u, gpsTOW24b: %u", p_GN_AGPS_Meas->Meas_GPS_TOW, p_GPS_MsrSetElement->gpsTOW );


   for ( MsrElementCount = 0 ; MsrElementCount < p_GN_AGPS_Meas->Num_Meas ; MsrElementCount++ )
   {

      // Allocate space for this element.
       p_GPS_MsrElement =(RRLP_GPS_MsrElement_t*)GN_Calloc( 1, sizeof( RRLP_GPS_MsrElement_t ) );

      // Fill in the measurement data.
      // satelliteID    SatelliteID,            -- Satellite identifier
      p_GPS_MsrElement->satelliteID     = p_GN_AGPS_Meas->Meas[MsrElementCount].SatID - 1;

      // cNo            INTEGER (0..63),        -- carrier noise ratio
      p_GPS_MsrElement->cNo             = p_GN_AGPS_Meas->Meas[MsrElementCount].SNR;

      // doppler        INTEGER (-32768..32767),   -- doppler, mulltiply by 0.2
      p_GPS_MsrElement->doppler         = p_GN_AGPS_Meas->Meas[MsrElementCount].Doppler;

      // wholeChips     INTEGER (0..1022),         -- whole value of the code phase measurement
      p_GPS_MsrElement->wholeChips      = p_GN_AGPS_Meas->Meas[MsrElementCount].Whole_Chips;

      // fracChips      INTEGER (0..1024),         -- fractional value of the code phase measurement
      //                                           -- a value of 1024 shall not be encoded by the sender
      //                                           -- the receiver shall consider a value of 1024 to be
      //                                           -- invalid data
      p_GPS_MsrElement->fracChips       = p_GN_AGPS_Meas->Meas[MsrElementCount].Fract_Chips;

      // mpathIndic     MpathIndic,                -- multipath indicator
      // notMeasured (0), low (1), medium (2), high (3)
      RRLP_ASN1_ENUM_SET(p_GPS_MsrElement->mpathIndic,p_GN_AGPS_Meas->Meas[MsrElementCount].MPath_Ind);

      // pseuRangeRMSErr INTEGER (0..63)            -- index
      p_GPS_MsrElement->pseuRangeRMSErr = p_GN_AGPS_Meas->Meas[MsrElementCount].PR_RMS_Err;
      GN_RRLP_Log( "rrlp_Add_GPS_MsrSetElement_gps_msrList:SatID: %2u, SNR: %2u, Doppler: %+6d, Whole_Chips: %4u, Fract_Chips: %4u, MPath_Ind: %u",
                   p_GN_AGPS_Meas->Meas[MsrElementCount].SatID,
                   p_GN_AGPS_Meas->Meas[MsrElementCount].SNR,
                   p_GN_AGPS_Meas->Meas[MsrElementCount].Doppler,
                   p_GN_AGPS_Meas->Meas[MsrElementCount].Whole_Chips,
                   p_GN_AGPS_Meas->Meas[MsrElementCount].Fract_Chips,
                   p_GN_AGPS_Meas->Meas[MsrElementCount].MPath_Ind );

      asn_sequence_add( &p_GPS_MsrSetElement->gps_msrList.list , p_GPS_MsrElement );
   }
}


//*****************************************************************************
/// \brief
///      Sets the RRLP GPS Measurement Info into an unencoded Measure Position
///      Response PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void rrlp_PDU_Add_msrPositionRsp_gps_MeasureInfo
(
   void           *p_PDU_Src,    ///< [out] Pointer to generated RRLP structure
                                 ///   RRLP PDU. Passed in as (void *) to hide
                                 ///   implementation.
   U1             NumMsrResults, ///< [in] number of sets of measurements range 1..3
   s_GN_AGPS_Meas GN_AGPS_Meas[3]///< [in] pointer to set of latched measurements.
)
{
   // -- GPS-Measurement information
   // GPS-MeasureInfo ::= SEQUENCE {
   //    -- Measurement info elements
   //    -- user has to make sure that in this element is number of elements
   //    -- defined in reference BTS identity
   //    gpsMsrSetList  SeqOfGPS-MsrSetElement
   //}
   RRLP_PDU_t *p_CastPDU = p_PDU_Src;

  // GPS_MsrSetElement_t p_GPS_MsrSetElement;
   RRLP_SeqOfGPS_MsrSetElement_t *p_SeqOfGPS_MsrSetElement;
   U1 MsrCount;

   // SeqOfGPS-MsrSetElement ::= SEQUENCE (SIZE(1..3)) OF GPS-MsrSetElement

  // p_p_GPS_MsrSetElement = &p_CastPDU->component.choice.msrPositionRsp.gps_MeasureInfo;
   p_CastPDU->component.choice.msrPositionRsp.rel_98_MsrPosition_Rsp_Extension =(RRLP_Rel_98_MsrPosition_Rsp_Extension_t*)GN_Calloc(1, sizeof(RRLP_Rel_98_MsrPosition_Rsp_Extension_t));
   p_CastPDU->component.choice.msrPositionRsp.rel_98_MsrPosition_Rsp_Extension->timeAssistanceMeasurements = (RRLP_GPSTimeAssistanceMeasurements_t*)GN_Calloc(1, sizeof(RRLP_GPSTimeAssistanceMeasurements_t));
   p_CastPDU->component.choice.msrPositionRsp.rel_98_MsrPosition_Rsp_Extension->timeAssistanceMeasurements->gpsReferenceTimeUncertainty = (long*)GN_Calloc(1, sizeof(long));
   *p_CastPDU->component.choice.msrPositionRsp.rel_98_MsrPosition_Rsp_Extension->timeAssistanceMeasurements->gpsReferenceTimeUncertainty = GN_AGPS_Meas[0].Meas_GPS_TOW_Unc ;

   if ( GN_AGPS_Meas[0].Delta_TOW != -1 )
   {
       p_CastPDU->component.choice.msrPositionRsp.rel_98_MsrPosition_Rsp_Extension->timeAssistanceMeasurements->deltaTow = (long*)GN_Calloc(1, sizeof(long));
       *p_CastPDU->component.choice.msrPositionRsp.rel_98_MsrPosition_Rsp_Extension->timeAssistanceMeasurements->deltaTow = GN_AGPS_Meas[0].Delta_TOW ;
   }

   p_CastPDU->component.choice.msrPositionRsp.gps_MeasureInfo = (RRLP_GPS_MeasureInfo_t*)GN_Calloc(1, sizeof(RRLP_GPS_MeasureInfo_t));

   p_SeqOfGPS_MsrSetElement = &p_CastPDU->component.choice.msrPositionRsp.gps_MeasureInfo->gpsMsrSetList;
   for ( MsrCount = 0 ; MsrCount < NumMsrResults ; MsrCount++ )
   {
      RRLP_GPS_MsrSetElement_t  *p_Element;

      p_Element = GN_Calloc( 1, sizeof( RRLP_GPS_MsrSetElement_t ) );

      rrlp_Add_GPS_MsrSetElement_gps_msrList(  p_Element, &GN_AGPS_Meas[MsrCount]   );

      asn_sequence_add( &p_SeqOfGPS_MsrSetElement->list , p_Element );
   }
}


//*****************************************************************************
/// \brief
///      Sets the RRLP Location Info into an unencoded Measure Position
///      Response PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void rrlp_PDU_Add_msrPositionRsp_locationInfo
(
   void                 *p_PDU_Src,          ///< [out] Pointer to generated RRLP structure
                                             ///   PDU_TTCN_KEYWORD. Passed in as (void *) to hide
                                             ///   implementation.
   s_GN_AGPS_GAD_Data   *p_GN_AGPS_GAD_Data, ///< [in] Position in #s_GN_AGPS_GAD_Data format.
   I2                   GAD_Ref_TOW_Subms    ///< [in] GAD Data Reference GPS Time of Week Sub-millisecond part [range 0..9999 x 0.0001 ms,  -1 = Unknown]
)
{
   // -- Location information IE
   // LocationInfo ::= SEQUENCE {
   //    refFrame    INTEGER (0..65535),        -- Reference Frame number
   //    -- If refFrame is within (42432..65535), it shall be ignored by the receiver
   //    -- in that case the MS should provide GPS TOW if available
   //    gpsTOW      INTEGER (0..14399999)   OPTIONAL,   -- GPS TOW
   //    fixType     FixType,
   //    -- Note that applicable range for refFrame is 0 - 42431
   //    -- Possible shapes carried in posEstimate are
   //    -- ellipsoid point,
   //    -- ellipsoid point with uncertainty circle
   //    -- ellipsoid point with uncertainty ellipse
   //    -- ellipsoid point with altitude and uncertainty ellipsoid
   //    posEstimate Ext-GeographicalInformation
   //}

   RRLP_PDU_t *p_CastPDU = p_PDU_Src;
   RRLP_LocationInfo_t **p_p_LocationInfo,*p_LocationInfo;

   // struct RefLocation *p_refLocation;
  // Ext_GeographicalInformation *p_posEstimate;

   s_GAD_Message  GAD_Message = {14, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
   BL status;
   s_GN_GPS_Nav_Data  GN_GPS_SUPL_Nav_Data;    // The latest Nav solution

   GN_GPS_Get_Nav_Data_Copy( &GN_GPS_SUPL_Nav_Data );

   status = GN_GAD_Message_From_AGPS_GAD_Data( p_GN_AGPS_GAD_Data, &GAD_Message );

   //p_CastPDU->component.U.msrPositionRsp.locationInfoPresent = SDL_True;
   p_p_LocationInfo = &p_CastPDU->component.choice.msrPositionRsp.locationInfo;

   ASN1_SET(p_p_LocationInfo) = (RRLP_LocationInfo_t*)GN_Calloc( 1, sizeof(RRLP_LocationInfo_t) );
   p_LocationInfo =ASN1_GET(p_p_LocationInfo);

   //    fixType     FixType,
   // FixType ::= INTEGER {
   //    twoDFix (0),
   //    threeDFix (1)
   // }  (0..1)

   //
   if ( p_GN_AGPS_GAD_Data->altitudeDirection == -1 )
   {
       p_LocationInfo->fixType = RRLP_FixType_twoDFix;
   }
   else
   {
       p_LocationInfo->fixType = RRLP_FixType_threeDFix;
   }

   //    -- Possible shapes carried in posEstimate are
   //    -- ellipsoid point,
   //    -- ellipsoid point with uncertainty circle
   //    -- ellipsoid point with uncertainty ellipse
   //    -- ellipsoid point with altitude and uncertainty ellipsoid
   //    posEstimate Ext-GeographicalInformation
  // p_posEstimate = &p_CastPDU->component.U.msrPositionRsp.locationInfo.posEstimate;

   p_LocationInfo->posEstimate.buf = (uint8_t*)GN_Calloc( 1, sizeof(uint8_t) );//GN_Calloc( 1, GAD_Message.buflen );
   p_LocationInfo->posEstimate.size = GAD_Message.buflen;
   //p_posEstimate->Bits = GN_Calloc( 1, GAD_Message.buflen );
  // p_posEstimate->Length = GAD_Message.buflen;
   memcpy ( p_LocationInfo->posEstimate.buf, &GAD_Message.message, GAD_Message.buflen );

   //    gpsTOW      INTEGER (0..14399999)   OPTIONAL,   -- GPS TOW
   //p_CastPDU->component.U.msrPositionRsp.locationInfo.gpsTOWPresent = SDL_True;
   p_LocationInfo->gpsTOW = GN_Calloc( 1, sizeof(long) );

   // Convert from (R8) seconds to (U4) milliseconds
  ASN1_SET(p_LocationInfo->gpsTOW) = (U4) ( GN_GPS_SUPL_Nav_Data.Gps_TOW * 1000 ) % 14400000;

   if ( GAD_Ref_TOW_Subms != -1 )
   {
      p_CastPDU->component.choice.msrPositionRsp.rel_98_MsrPosition_Rsp_Extension =(RRLP_Rel_98_MsrPosition_Rsp_Extension_t*)GN_Calloc(1, sizeof(RRLP_Rel_98_MsrPosition_Rsp_Extension_t));
      p_CastPDU->component.choice.msrPositionRsp.rel_98_MsrPosition_Rsp_Extension->timeAssistanceMeasurements = (RRLP_GPSTimeAssistanceMeasurements_t*)GN_Calloc(1, sizeof(RRLP_GPSTimeAssistanceMeasurements_t));
      p_CastPDU->component.choice.msrPositionRsp.rel_98_MsrPosition_Rsp_Extension->timeAssistanceMeasurements->gpsTowSubms = (long*)GN_Calloc(1, sizeof(long));
      *p_CastPDU->component.choice.msrPositionRsp.rel_98_MsrPosition_Rsp_Extension->timeAssistanceMeasurements->gpsTowSubms = GAD_Ref_TOW_Subms ;
   }
}


//*****************************************************************************
/// \brief
///      Sets the Location Error Reason into an unencoded Measure Position
///      Response PDU.
///
/// \returns
///      Nothing.
//*****************************************************************************
void rrlp_PDU_Add_msrPositionRsp_locErrorReason
(
   void *p_PDU_Src,              ///< [out] Pointer to generated RRLP structure
                                 ///   PDU_TTCN_KEYWORD. Passed in as (void *) to hide
                                 ///   implementation.
   e_GN_RRLP_LocErrorReason GN_RRLP_LocErrorReason ///< [in] Location Error Reason.
)
{
   RRLP_PDU_t *p_CastPDU = p_PDU_Src;

   // LocErrorReason ::= ENUMERATED {
   //    unDefined (0),
   //    notEnoughBTSs (1),
   //    notEnoughSats (2),
   //    eotdLocCalAssDataMissing (3),
   //    eotdAssDataMissing (4),
   //    gpsLocCalAssDataMissing (5),
   //    gpsAssDataMissing (6),
   //    methodNotSupported (7),
   //    notProcessed (8),
   //    refBTSForGPSNotServingBTS (9),
   //    refBTSForEOTDNotServingBTS (10),
   //    ...
   // }

     BL Valid_LocErrorReason = TRUE;
     RRLP_LocErrorReason_t *p_locErrorReason;
     p_CastPDU->component.choice.msrPositionRsp.locationError = GN_Calloc(1,sizeof(RRLP_LocationError_RRLP_t));
     p_locErrorReason    = &p_CastPDU->component.choice.msrPositionRsp.locationError->locErrorReason;

   //p_CastPDU->component.U.msrPositionRsp.locationErrorPresent = SDL_True;

   switch ( GN_RRLP_LocErrorReason )
   {
   case GN_RRLP_LocErrorReason_unDefined:
       RRLP_ASN1_ENUM_SET_CHK(p_locErrorReason, RRLP_LocErrorReason_unDefined);
      //*p_locErrorReason = unDefined;
      break;
   case GN_RRLP_LocErrorReason_notEnoughBTSs:
       RRLP_ASN1_ENUM_SET_CHK(p_locErrorReason, RRLP_LocErrorReason_notEnoughBTSs);
      //*p_locErrorReason = notEnoughBTSs;
      break;
   case GN_RRLP_LocErrorReason_notEnoughSats:
       RRLP_ASN1_ENUM_SET_CHK(p_locErrorReason, RRLP_LocErrorReason_notEnoughSats);
     // *p_locErrorReason = notEnoughSats;
      break;
   case GN_RRLP_LocErrorReason_eotdLocCalAssDataMissing:
       RRLP_ASN1_ENUM_SET_CHK(p_locErrorReason, RRLP_LocErrorReason_eotdLocCalAssDataMissing);
    //  *p_locErrorReason = eotdLocCalAssDataMissing;
      break;
   case GN_RRLP_LocErrorReason_eotdAssDataMissing:
       RRLP_ASN1_ENUM_SET_CHK(p_locErrorReason, RRLP_LocErrorReason_eotdAssDataMissing);
      //*p_locErrorReason = eotdAssDataMissing;
      break;
   case GN_RRLP_LocErrorReason_gpsLocCalAssDataMissing:
       RRLP_ASN1_ENUM_SET_CHK(p_locErrorReason, RRLP_LocErrorReason_gpsLocCalAssDataMissing);
      //*p_locErrorReason = gpsLocCalAssDataMissing;
      break;
   case GN_RRLP_LocErrorReason_gpsAssDataMissing:
       RRLP_ASN1_ENUM_SET_CHK(p_locErrorReason, RRLP_LocErrorReason_gpsAssDataMissing);
     // *p_locErrorReason = gpsAssDataMissing;
      break;
   case GN_RRLP_LocErrorReason_methodNotSupported:
       RRLP_ASN1_ENUM_SET_CHK(p_locErrorReason, RRLP_LocErrorReason_methodNotSupported);
      //*p_locErrorReason = methodNotSupported;
      break;
   case GN_RRLP_LocErrorReason_notProcessed:
       RRLP_ASN1_ENUM_SET_CHK(p_locErrorReason, RRLP_LocErrorReason_notProcessed);
      //*p_locErrorReason = notProcessed;
      break;
   case GN_RRLP_LocErrorReason_refBTSForGPSNotServingBTS:
       RRLP_ASN1_ENUM_SET_CHK(p_locErrorReason, RRLP_LocErrorReason_refBTSForGPSNotServingBTS);
    //  *p_locErrorReason = refBTSForGPSNotServingBTS;
      break;
   case GN_RRLP_LocErrorReason_refBTSForEOTDNotServingBTS:
       RRLP_ASN1_ENUM_SET_CHK(p_locErrorReason, RRLP_LocErrorReason_refBTSForEOTDNotServingBTS);
      //*p_locErrorReason = refBTSForEOTDNotServingBTS;
      break;
   default:
      Valid_LocErrorReason = FALSE;
      break;
   }
   if ( ! Valid_LocErrorReason )
   {
      //p_CastPDU->component.U.msrPositionRsp.locationErrorPresent = SDL_False;
   }
}


//*****************************************************************************
/// \brief
///      Extracts method type from RRLP pdu and converts it to the #s_GN_RRLP_MethodType
///      format.
///
/// \returns
///      Flag to indicate whether method type was available.
/// \retval #TRUE if available.
/// \retval #FALSE if unavailable.
//*****************************************************************************
BL rrlp_PDU_Get_MethodType
(
   RRLP_MethodType_t           *p_MethodType,          ///< [in] Method Type within delivered PDU.
   s_GN_RRLP_MethodType *p_GN_RRLP_MethodType   ///< [out] Extracted Method Type
)
{
   if ( p_MethodType != NULL )
   {
      switch( p_MethodType->present )
      {
      case RRLP_MethodType_PR_msAssisted:     // msAssisted     AccuracyOpt,   -- accuracy is optional
         p_GN_RRLP_MethodType->methodType = GN_msAssisted;
         if ( p_MethodType->choice.msAssisted.accuracy != NULL )
         {
            p_GN_RRLP_MethodType->accuracy = (I1)*p_MethodType->choice.msAssisted.accuracy ;
            GN_RRLP_Log( "rrlp_PDU_Get_MethodType: msAssisted Accuracy %d.",
               p_GN_RRLP_MethodType->accuracy );
         }
         else
         {
            p_GN_RRLP_MethodType->accuracy = -1;
            GN_RRLP_Log( "rrlp_PDU_Get_MethodType: msAssisted Accuracy not present." );
         }
         break;
      case RRLP_MethodType_PR_msBased:        // msBased        Accuracy,      -- accuracy is mandatory
         p_GN_RRLP_MethodType->methodType = GN_msBased;
         p_GN_RRLP_MethodType->accuracy = (I1)p_MethodType->choice.msBased;
         GN_RRLP_Log( "rrlp_PDU_Get_MethodType: msBased Accuracy %d.",
            p_GN_RRLP_MethodType->accuracy );
         break;
      case RRLP_MethodType_PR_msBasedPref:    // msBasedPref    Accuracy,      -- accuracy is mandatory
         p_GN_RRLP_MethodType->methodType = GN_msBasedPref;
         p_GN_RRLP_MethodType->accuracy = (I1)p_MethodType->choice.msBasedPref;
         GN_RRLP_Log( "rrlp_PDU_Get_MethodType: msBasedPref Accuracy %d.",
            p_GN_RRLP_MethodType->accuracy );
         break;
      case RRLP_MethodType_PR_msAssistedPref: // msAssistedPref Accuracy       -- accuracy is mandatory
         p_GN_RRLP_MethodType->methodType = GN_msAssistedPref;
         p_GN_RRLP_MethodType->accuracy = (I1)p_MethodType->choice.msAssistedPref;
         GN_RRLP_Log( "rrlp_PDU_Get_MethodType: msAssistedPref Accuracy %d.",
            p_GN_RRLP_MethodType->accuracy );
         break;
      default:
         return FALSE;
         break;
      }
      return TRUE;
   }
   return FALSE;
}



//*****************************************************************************
/// \brief
///      Gets the method type from a decoded Measure Position Request PDU.
///
/// \returns
///      Flag to indicate whether method type was available.
/// \retval #TRUE if available.
/// \retval #FALSE if unavailable.
//*****************************************************************************
BL rrlp_PDU_Get_PositionInstruct_methodType
(
   void                 *p_PDU_Src,             ///< [in] Pointer to generated RRLP structure
                                                ///   PDU_TTCN_KEYWORD. Passed in as (void *) to hide
                                                ///   implementation.
   s_GN_RRLP_MethodType *p_GN_RRLP_MethodType   ///< [out] Positioning Method type
)
{
   RRLP_PDU_t *p_PDU_Cast = p_PDU_Src;

   // RRLP-Component ::= CHOICE {
   //    msrPositionReq       MsrPosition-Req,
   //    msrPositionRsp       MsrPosition-Rsp,
   //    assistanceData       AssistanceData,
   //    assistanceDataAck    NULL,
   //    protocolError        ProtocolError,
   //    ...
   // }

   switch( p_PDU_Cast->component.present )
   {
      // Only a msrPositionReq has a position instruct type.
   case RRLP_RRLP_Component_PR_msrPositionReq:
       return rrlp_PDU_Get_MethodType( &p_PDU_Cast->component.choice.msrPositionReq.positionInstruct.methodType, p_GN_RRLP_MethodType );
      break;
   case RRLP_RRLP_Component_PR_msrPositionRsp:    return FALSE;  break;
   case RRLP_RRLP_Component_PR_assistanceData:    return FALSE;  break;
   case RRLP_RRLP_Component_PR_assistanceDataAck: return FALSE;  break;
   case RRLP_RRLP_Component_PR_protocolError:     return FALSE;  break;
   default:
      return FALSE;
      break;
   }
}


