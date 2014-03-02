//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename rrlp_client.c
//
// $Header: X:/MKS Projects/prototype/prototype/rrlp_handler/rcs/rrlp_client.c 1.32 2009/01/13 14:09:42Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_handler
///
/// \file
/// \brief
///      RRLP client implementation.
///
//*************************************************************************

#include <string.h>

#include "os_assert.h"

#include "rrlp_messages.h"
#include "rrlp_interface.h"
#include "rrlp_helper.h"
#include "rrlp_asn1_macros.h"

#include "GN_GPS_api.h"
#include "GN_AGPS_api.h"
#include "GN_RRLP_server_api.h"
#include "supl_log.h"

/// Global storage for the RRLP instance store.
extern s_RRLP_Instances    RRLP_Instances;
/// Global storage for the RRLP message queue.
extern s_RRLP_Queue        RRLP_Queue;

//*****************************************************************************
/// \brief
///      GN GPS API Function to request the current GPS Navigation solution data.
/// \details
///      This function is equivalent to #GN_GPS_Get_Nav_Data but will always
///      return the current GPS Navigation solution data if valid.
/// \returns
///      Flag to indicate if there is valid GPS Navigation solution data
///      available.
/// \retval #TRUE if GPS Navigation solution data is available.
/// \retval #FALSE if GPS Navigation solution data is not available.
//*****************************************************************************
BL GN_GPS_Get_Nav_Data_Copy(
   s_GN_GPS_Nav_Data *p_Nav_Data    ///< [in] Pointer to where the GPS Library should write the Nav Data to.
);

void GN_AGPS_Log_Meas(
   s_GN_AGPS_Meas *p_GN_AGPS_Meas   ///< [in] AGPS Measurements in GloNAV format.
);

void RRLP_Process_protocolError( void *p_PDU_Decoded, s_RRLP_Instance *p_RRLP_Instance );

void RRLP_Handle_POS_Payload(                      s_RRLP_Message* p_ThisMessage );
void RRLP_Handle_GPS_Position_Solution_Available(  s_RRLP_Message* p_ThisMessage );
void RRLP_Handle_GPS_Measurements_Available(       s_RRLP_Message* p_ThisMessage );
void RRLP_Handle_Terminate_POS_Sequence(           s_RRLP_Message* p_ThisMessage );
void RRLP_Handle_Expiry_MeasureResponseTime(       s_RRLP_Message* p_ThisMessage );

void RRLP_Send_Expiry_MeasureResponseTime( s_RRLP_Instance *p_RRLP_Instance, s_RRLP_Instance *p_SrcInstance );


//*****************************************************************************
/// \brief
///      GN GPS RRLP API Function to process RRLP messages.
/// \returns
///      Nothing.
//*****************************************************************************
void RRLP_Message_Handler( void )
{
   s_RRLP_Message* p_ThisMessage;

   // Do any processing first as this may initiate a message.
   RRLP_Instance_Handler();

   while( ( p_ThisMessage = RRLP_Queue_Retrieve_Message() ) != NULL )
   {
      switch ( p_ThisMessage->Payload.MsgEvent )
      {
      case event_RRLP_POS_Payload:                       RRLP_Handle_POS_Payload(                     p_ThisMessage );  break;
      case event_RRLP_GPS_Position_Solution_Available:   RRLP_Handle_GPS_Position_Solution_Available( p_ThisMessage );  break;
      case event_RRLP_GPS_Measurements_Available:        RRLP_Handle_GPS_Measurements_Available(      p_ThisMessage );  break;
      case event_RRLP_Terminate_POS_Sequence:            RRLP_Handle_Terminate_POS_Sequence(          p_ThisMessage );  break;
      case event_RRLP_Expiry_MeasureResponseTime:        RRLP_Handle_Expiry_MeasureResponseTime(      p_ThisMessage );  break;
      default:
         // log error
         OS_ASSERT( 0 );
         break;
      }
      // free up the message
      RRLP_Message_Free( p_ThisMessage );
      // Make sure we do any processing after each message is dealt with.
      RRLP_Instance_Handler();
   }
}

//*****************************************************************************
/// \brief
///      Check each instance of the RRLP Handler for any required processing.
//*****************************************************************************
void RRLP_Instance_Process(
   s_RRLP_Instance *p_RRLP_Instance    ///< pointer to an active instance.
)
{
   s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data;
   U4 GAD_Ref_TOW;
   I2 GAD_Ref_TOW_Subms;     // GAD Data Reference GPS Time of Week Sub-millisecond part [range 0..9999 x 0.0001 ms,  -1 = Unknown]
   BL Status;

   if ( p_RRLP_Instance == NULL )
   {
      // Special case: Check for assistance data request from GPS_Core
      /// \todo Query GPS_core for assistance data required. In the meantime always say yes.
      // RRLP_Assistance_Request();
   }
   else
   {
      s_RRLP_InstanceData  *p_IData = p_RRLP_Instance->p_RRLP;

      switch ( p_IData->State )
      {
      case state_RRLP_Idle:
         // In this state we can delete the instance.
         RRLP_Instance_Delete( p_RRLP_Instance );
         break;
      case state_RRLP_Position_Requested_From_GPS:
         switch ( p_IData->GN_RRLP_MethodType.methodType )
         {
         case GN_msAssisted:    // msAssisted     AccuracyOpt,   -- accuracy is optional
         case GN_msAssistedPref: // msAssistedPref Accuracy       -- accuracy is mandatory
            {
               s_GN_AGPS_Meas GN_AGPS_Meas;
               BL Status;

               // Retrieve a set of measurements if there is a new set since the last Prev_OS_Time_ms.
               // First time through Prev_OS_Time_ms == 0 so it always gets a set.
               Status = GN_AGPS_Get_Meas( &p_RRLP_Instance->p_RRLP->Prev_OS_Time_ms, &GN_AGPS_Meas );
               // Have we got a decent set of measurements?
               if ( Status && GN_AGPS_Meas.Quality >= 200 && GN_AGPS_Meas.Meas_GPS_TOW_Unc < 100 )
               {
                  // We have a set of measurements which are good enough.
                  // It must be the first time they are good enough, so send immediately.
                  s_GN_AGPS_Meas *p_GN_AGPS_Meas;

                  GN_RRLP_Log( "RRLP_Instance_Process:  Required measurement quality is achieved");

                  GN_AGPS_Log_Meas( &GN_AGPS_Meas );

                  p_GN_AGPS_Meas = GN_Calloc( 1, sizeof( s_GN_AGPS_Meas ) );
                  memcpy( p_GN_AGPS_Meas, &GN_AGPS_Meas, sizeof( s_GN_AGPS_Meas ) );
                  GN_AGPS_Log_Meas( p_GN_AGPS_Meas );
                  RRLP_Send_GPS_Measurements_Available(  p_RRLP_Instance,
                                                         p_RRLP_Instance,
                                                         p_GN_AGPS_Meas);
                  GN_RRLP_Timer_Clear( &p_IData->MsrResp_Timer );
               }
               else if ( GN_RRLP_Timer_Expired( &p_IData->MsrResp_Timer ) )
               {
                  // Ran out of time, get the last set of measurements available.
                  Status = GN_AGPS_Get_Meas( NULL, &GN_AGPS_Meas );

                  // If we have valid measurements either just retrieved or stored.
                  if ( Status || p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas != NULL )
                  {
                     s_GN_AGPS_Meas *p_GN_AGPS_Meas;

                     GN_RRLP_Log( "RRLP_Instance_Process:  Timeout reached");

                     // Check to see if we have some stored measurements better than the latest ones.
                     if ( Status && p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas != NULL )
                     {
                        // We have good current and stored measurements. Decide which to use.
                        if ( p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas->Quality > GN_AGPS_Meas.Quality )
                        {
                           // The best set of measurements stored is better than the latest.
                           p_GN_AGPS_Meas = p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas;
                           // Make sure the RRLP instance handler does not clean up the measurements before they're used.
                           p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas = NULL;
                        }
                        else
                        {
                           // Current measurements are better than the stored measurements.
                           p_GN_AGPS_Meas = GN_Calloc( 1, sizeof( s_GN_AGPS_Meas ) );
                           memcpy( p_GN_AGPS_Meas, &GN_AGPS_Meas, sizeof( s_GN_AGPS_Meas ) );
                        }
                     }
                     else if ( p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas != NULL )
                     {
                        // If we have a valid set of stored measurements we'll have to use them.
                        // We will only be here if we we don't have a valid current set of measurements.
                        p_GN_AGPS_Meas = p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas;
                        // Make sure the RRLP instance handler does not clean up the measurements before they're used.
                        p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas = NULL;
                     }
                     else
                     {
                        // We will only get here if there is a valid set of current measurements and no stored measurements.
                        p_GN_AGPS_Meas = GN_Calloc( 1, sizeof( s_GN_AGPS_Meas ) );
                        memcpy( p_GN_AGPS_Meas, &GN_AGPS_Meas, sizeof( s_GN_AGPS_Meas ) );
                     }
                     GN_AGPS_Log_Meas( p_GN_AGPS_Meas );

                     RRLP_Send_GPS_Measurements_Available(  p_RRLP_Instance,
                                                            p_RRLP_Instance,
                                                            p_GN_AGPS_Meas);
                     GN_RRLP_Timer_Clear( &p_IData->MsrResp_Timer );
                  }
                  else
                  {
                     GN_RRLP_Log( "RRLP_Instance_Process: Timeout reached with no valid GPS measurements");
                     
                     RRLP_Send_Expiry_MeasureResponseTime(  p_RRLP_Instance,
                                                            p_RRLP_Instance);
                  }
               }
               else if ( Status )
               {
                  // We have a valid set of measurements but not good enough to send immediately so...
                  // Save the set of measurements if it's better than what we already have stored.
                  if ( p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas == NULL )
                  {
                     p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas = GN_Calloc( 1, sizeof( s_GN_AGPS_Meas ) );
                  }
                  if ( GN_AGPS_Meas.Quality >= p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas->Quality )
                  {
                     memcpy( p_RRLP_Instance->p_RRLP->p_GN_AGPS_Meas, &GN_AGPS_Meas, sizeof( s_GN_AGPS_Meas ) );
                  }
                  GN_AGPS_Log_Meas( &GN_AGPS_Meas );
               }
            }
            break;
         case GN_methodType_NONE:
         case GN_msBased:        // msBased        Accuracy,      -- accuracy is mandatory
         case GN_msBasedPref:    // msBasedPref    Accuracy,      -- accuracy is mandatory
         default:
            {
               if ( p_IData->p_GN_GPS_Nav_Data == NULL )
               {
                  p_IData->p_GN_GPS_Nav_Data = GN_Calloc( 1, sizeof( s_GN_GPS_Nav_Data ) );
               }
               if ( GN_GPS_Get_Nav_Data_Copy( p_IData->p_GN_GPS_Nav_Data ) == TRUE )
               {
                  CH TempBuf[110];
                  CH *p_NextPos = TempBuf;
                  s_GN_AGPS_QoP QoP;

                  // A new fix has just been generated
                  //             Nav_Update_Counter++;
                  //             printf( "%4d ", Nav_Update_Counter );
                  p_NextPos += sprintf( p_NextPos, "RRLP_Instance_Process: Fix " );
                  p_NextPos += sprintf( p_NextPos, "%10d ", GN_GPS_Get_OS_Time_ms() );
                  p_NextPos += sprintf( p_NextPos, "%10d ", p_IData->p_GN_GPS_Nav_Data->Local_TTag );
                  p_NextPos += sprintf( p_NextPos, "%02d%02d%02d.%03d ",
                           p_IData->p_GN_GPS_Nav_Data->Hours,
                           p_IData->p_GN_GPS_Nav_Data->Minutes,
                           p_IData->p_GN_GPS_Nav_Data->Seconds,
                           p_IData->p_GN_GPS_Nav_Data->Milliseconds );
                  p_NextPos += sprintf( p_NextPos, "%10.6f %11.6f ",
                           p_IData->p_GN_GPS_Nav_Data->Latitude,
                           p_IData->p_GN_GPS_Nav_Data->Longitude );
                  p_NextPos += sprintf( p_NextPos, "%7.2f ",
                           p_IData->p_GN_GPS_Nav_Data->Altitude_MSL );
                  p_NextPos += sprintf( p_NextPos, "%2d/%2d",
                           p_IData->p_GN_GPS_Nav_Data->SatsUsed,
                           p_IData->p_GN_GPS_Nav_Data->SatsInView );
                  if      ( p_IData->p_GN_GPS_Nav_Data->Valid_SingleFix == TRUE ) p_NextPos += sprintf( p_NextPos, "  3D fix" );    //Required for CG2900 header files
                  else if ( p_IData->p_GN_GPS_Nav_Data->Valid_SingleFix == TRUE ) p_NextPos += sprintf( p_NextPos, "  2D fix" );    //Required for CG2900 header files
                  else                                                         p_NextPos += sprintf( p_NextPos, "  Invalid fix");
                  //GN_RRLP_Log(TempBuf);

                  if ( p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP != NULL )
                  {
                     if( (p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP->veracc == 0 ) || (p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP->veracc > 127) )
                         QoP.Vert_Accuracy = 255;
                     else
                         QoP.Vert_Accuracy  = p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP->veracc;
                     if ( p_RRLP_Instance->p_RRLP->GN_RRLP_MethodType.accuracy == -1 )
                     {
                        // No accuracy from RRLP fix, just use the QoP from SUPL exchange.
                        QoP.Horiz_Accuracy = p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP->horacc ;
                     }
                     else
                     {
                        // If we have both RRLP accuracy and SUPL Qop horacc, use the smaller of the two.
                        if ( p_RRLP_Instance->p_RRLP->GN_RRLP_MethodType.accuracy < p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP->horacc )
                        {
                           QoP.Horiz_Accuracy = p_RRLP_Instance->p_RRLP->GN_RRLP_MethodType.accuracy ;
                        }
                        else
                        {
                           QoP.Horiz_Accuracy = p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP->horacc ;
                        }
                     }
                  }
                  else
                  {
                     QoP.Vert_Accuracy  = 255;
                     if ( p_RRLP_Instance->p_RRLP->GN_RRLP_MethodType.accuracy == -1 )
                     {
                        // No accuracy from RRLP fix, just use the QoP from SUPL exchange.
                        QoP.Horiz_Accuracy = 255 ;
                     }
                     else
                     {
                        QoP.Horiz_Accuracy = p_RRLP_Instance->p_RRLP->GN_RRLP_MethodType.accuracy ;
                     }
                  }

                  QoP.Deadline_OS_Time_ms = p_RRLP_Instance->p_RRLP->Solution_Deadline ;

                  // Check qualified position.
                  Status = GN_AGPS_Qual_Pos( &p_RRLP_Instance->p_RRLP->Prev_OS_Time_ms, &QoP ) ;

                  if ( Status )
                  {
                     U4 NULL_OS_Time_ms = 0;
                     I4 temp;

                     p_GN_AGPS_GAD_Data = GN_Calloc( 1, sizeof( *p_GN_AGPS_GAD_Data ) );

                     Status = GN_AGPS_Get_GAD_Data( &NULL_OS_Time_ms, &GAD_Ref_TOW, &GAD_Ref_TOW_Subms, &temp, p_GN_AGPS_GAD_Data );

                     if ( Status )
                     {
                        U4 msTTFF = GN_GPS_Get_OS_Time_ms() - p_RRLP_Instance->p_RRLP->Solution_ReqTimeStamp;
#ifdef SUPL_DEBUG_EVENTS_ENABLE
                        {
                            u_GN_SUPL_DebugEventPayload v_Payload;

                            v_Payload.posReport.Latitude = p_IData->p_GN_GPS_Nav_Data->Latitude;
                            v_Payload.posReport.Longitude = p_IData->p_GN_GPS_Nav_Data->Longitude;
                            v_Payload.posReport.Altitude = p_IData->p_GN_GPS_Nav_Data->Altitude_Ell;

                            GN_SUPL_Debug_Event( SUPL_PosReport , &v_Payload );
                        }
#endif /* SUPL_DEBUG_EVENTS_ENABLE */
                        GN_RRLP_Log_PosReport( p_RRLP_Instance->Handle , p_GN_AGPS_GAD_Data , msTTFF );
                        RRLP_Send_GPS_Position_Solution_Available(  p_RRLP_Instance,
                                                                    p_RRLP_Instance,
                                                                    p_GN_AGPS_GAD_Data,
                                                                    GAD_Ref_TOW,
                                                                    GAD_Ref_TOW_Subms,
                                                                    p_IData->p_GN_GPS_Nav_Data );
                        // Make sure we don't delete free the data before it's used.
                        p_IData->p_GN_GPS_Nav_Data = NULL ;
                     }
                     else
                     {
                        // GN_Qual_Pos hit the timeout.
                        GN_RRLP_Log_PosRspTimeout( p_RRLP_Instance->Handle );
                        RRLP_Send_Expiry_MeasureResponseTime(  p_RRLP_Instance,
                                                               p_RRLP_Instance );
                        GN_Free( p_GN_AGPS_GAD_Data );

                     }
#ifdef SUPL_DEBUG_EVENTS_ENABLE
                     GN_SUPL_Debug_Event( SUPL_PosResponse , NULL );
#endif /* SUPL_DEBUG_EVENTS_ENABLE */
                  }
               }
            }
            break;
         }
         break;
      case state_RRLP_Transaction_In_Progress:
      case state_RRLP_Assistance_Data_Delivery:
      case state_RRLP_Measure_Position_Request:
      default:
         break;
      }
   }
}

//*****************************************************************************
/// \brief
///      GN GPS RRLP API Function to process an RRLP PDU containing a measurement
///      position request.
/// \returns
///      Nothing.
//*****************************************************************************
void RRLP_Process_msrPositionReq(
   void              *p_PDU_Decoded,   ///< Transparent handle to a decoded RRLP PDU.
   s_RRLP_Instance   *p_RRLP_Instance, ///< RRLP_Instance handling this PDU.
   U4                PDU_TimeStamp     ///< Time Stamp of receipt of PDU by the client.
)
{
   U1 MeasureResponseTime;
   BL status;
   I1 Delay;

   // Extract the RRLP reference number from the PDU
   p_RRLP_Instance->p_RRLP->ReferenceNumber = rrlp_PDU_Get_Reference_Number( p_PDU_Decoded );

   status = rrlp_PDU_Get_PositionInstruct_methodType( p_PDU_Decoded, &p_RRLP_Instance->p_RRLP->GN_RRLP_MethodType);

   status = rrlp_PDU_Get_PositionInstruct_measureResponseTime( p_PDU_Decoded,
                                                               &MeasureResponseTime,
                                                               &Delay );
   /* Update the QoP with latest response time */
   /* p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP->delay = MeasureResponseTime; */
   if( NULL != p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP )
   {
      p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP->delay = Delay;
   }
   GN_RRLP_Log( "RRLP_Process_msrPositionReq: measureResponseTime %d (s).",
                MeasureResponseTime);

   rrlp_PDU_ProcessAssistanceData( p_RRLP_Instance->Handle , p_PDU_Decoded, PDU_TimeStamp );

   p_RRLP_Instance->p_RRLP->Solution_Deadline = PDU_TimeStamp + ( MeasureResponseTime * 1000 );
   p_RRLP_Instance->p_RRLP->Solution_ReqTimeStamp = PDU_TimeStamp;

   switch ( p_RRLP_Instance->p_RRLP->GN_RRLP_MethodType.methodType )
   {
   case GN_msAssisted:     // msAssisted     AccuracyOpt,   -- accuracy is optional
   case GN_msAssistedPref: // msAssistedPref Accuracy       -- accuracy is mandatory
      {
         GN_RRLP_Timer_Set( &p_RRLP_Instance->p_RRLP->MsrResp_Timer, ( MeasureResponseTime * 1000 ) );
      }
      break;
   case GN_methodType_NONE:
   case GN_msBased:        // msBased        Accuracy,      -- accuracy is mandatory
   case GN_msBasedPref:    // msBasedPref    Accuracy,      -- accuracy is mandatory
      /* In these three cases , log the QoP as there is an update due to new ResponseTime*/
      GN_RRLP_Log_QoP( p_RRLP_Instance->Handle , p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP );
   default:
      break;
   }

#ifdef SUPL_DEBUG_EVENTS_ENABLE
   GN_SUPL_Debug_Event( SUPL_PosRequest , NULL );
#endif /* SUPL_DEBUG_EVENTS_ENABLE */

   RRLP_Instance_State_Transition( p_RRLP_Instance,
                                   state_RRLP_Position_Requested_From_GPS,
                                   "event_RRLP_POS_Payload(msrPositionReq)" );
}


//*****************************************************************************
/// \brief
///      GN GPS RRLP API Function to process an RRLP PDU containing assistance data.
/// \returns
///      Nothing.
//*****************************************************************************
void RRLP_Process_assistanceData(
   void              *p_PDU_Decoded,   ///< Transparent handle to a decoded RRLP PDU.
   s_RRLP_Instance   *p_RRLP_Instance, ///< RRLP_Instance handling this PDU.
   U4                PDU_TimeStamp     ///< Time Stamp of receipt of PDU by the client.
)
{
   s_PDU_Buffer_Store *p_PDU_Buf = asn1_PDU_Buffer_Store_Alloc( 0 );

   void *p_PDU_Src;

   // Extract the RRLP reference number from the PDU
   p_RRLP_Instance->p_RRLP->ReferenceNumber = rrlp_PDU_Get_Reference_Number( p_PDU_Decoded );

   rrlp_PDU_ProcessAssistanceData( p_RRLP_Instance->Handle , p_PDU_Decoded, PDU_TimeStamp );

   p_PDU_Src = rrlp_PDU_Make_assistanceDataAck( p_RRLP_Instance->p_RRLP->ReferenceNumber );
   if ( rrlp_PDU_Encode( p_PDU_Src, p_PDU_Buf ) )
   {
      BL          Status;

      Status = GN_RRLP_PDU_Delivery_Out( p_RRLP_Instance->Handle,
         p_PDU_Buf->PDU_Encoded.Length,
         p_PDU_Buf->PDU_Encoded.p_PDU_Data,
         NULL,
         NULL );

      RRLP_Instance_State_Transition( p_RRLP_Instance,
                                      state_RRLP_Transaction_In_Progress,
                                      "event_RRLP_POS_Payload(assistanceData)" );
   }
   else
   {
      BL          Status;

      Status = GN_RRLP_Sequence_Error_End_Out( p_RRLP_Instance->Handle );
      RRLP_Instance_State_Transition( p_RRLP_Instance,
                                      state_RRLP_Idle,
                                      "event_RRLP_POS_Payload(assistanceData) - Encode Error" );
   }
   rrlp_PDU_Free( p_PDU_Src );
   asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
}


//*****************************************************************************
/// \brief
///      Handles the #event_RRLP_POS_Payload.
//*****************************************************************************
void RRLP_Handle_POS_Payload(s_RRLP_Message* p_ThisMessage)
{
   s_PDU_Buffer_Store      *p_PDU_Buf = GN_Calloc( 1, sizeof( s_PDU_Buffer_Store ) );
   void                    *p_PDU_Decoded = NULL;
   e_GN_RRLP_Component_PR  This_Message_Type;
   GN_RRLP_ErrorCodes      GN_RRLP_ErrorCodes;

   s_RRLP_Instance *p_RRLP_Instance = p_ThisMessage->MsgHdr.p_InstanceOfDest;

   if ( p_RRLP_Instance == NULL )
   {
      p_RRLP_Instance = RRLP_Instance_From_Handle( p_ThisMessage->MsgHdr.Handle );
   }

   if ( p_RRLP_Instance == NULL )
   {
      p_RRLP_Instance = RRLP_Instance_Request_New();
   }

   if ( p_ThisMessage->Payload.p_GN_RRLP_QoP != NULL )
   {
      p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP = GN_Calloc( 1, sizeof( s_GN_RRLP_QoP ) );
      memcpy ( p_RRLP_Instance->p_RRLP->p_GN_RRLP_QoP,
               p_ThisMessage->Payload.p_GN_RRLP_QoP,
               sizeof( s_GN_RRLP_QoP ) );
   }

   switch ( p_RRLP_Instance->p_RRLP->State )
   {
   // Valid states.
   case state_RRLP_Idle:
   case state_RRLP_Transaction_In_Progress:
   case state_RRLP_Assistance_Data_Delivery:
   case state_RRLP_Measure_Position_Request:
      p_PDU_Buf->PDU_Encoded.p_PDU_Data = p_ThisMessage->Payload.p_RRLP_PDU->p_PDU_Data;
      p_PDU_Buf->PDU_Encoded.Length     = p_ThisMessage->Payload.p_RRLP_PDU->Length;

      p_ThisMessage->Payload.p_RRLP_PDU->p_PDU_Data = NULL;

      if ( rrlp_PDU_Decode( p_PDU_Buf, &p_PDU_Decoded, &GN_RRLP_ErrorCodes ) )
      {
         asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
         This_Message_Type = rrlp_PDU_Get_MessageType( p_PDU_Decoded );
         switch ( This_Message_Type )
         {
         case GN_RRLP_Component_PR_msrPositionReq:
            RRLP_Process_msrPositionReq( p_PDU_Decoded, p_RRLP_Instance, p_ThisMessage->Payload.PDU_TimeStamp );
            break;
         case GN_RRLP_Component_PR_msrPositionRsp:
            OS_ASSERT( 0 ); // \todo Return Protocol error as client should not receive this.
            // RRLP_Process_msrPositionRsp(p_PDU_Decoded, p_RRLP_Instance);
            break;
         case GN_RRLP_Component_PR_assistanceData:
            RRLP_Process_assistanceData( p_PDU_Decoded, p_RRLP_Instance, p_ThisMessage->Payload.PDU_TimeStamp );
            break;
         case GN_RRLP_Component_PR_assistanceDataAck:
            OS_ASSERT( 0 ); // \todo Return Protocol error as client should not receive this.
            // RRLP_Process_assistanceDataAck(p_PDU_Decoded, p_RRLP_Instance);
            break;
         case GN_RRLP_Component_PR_protocolError:
            RRLP_Process_protocolError( p_PDU_Decoded, p_RRLP_Instance );
            break;
         case GN_RRLP_Component_PR_NOTHING:  /* No components present */
         default:
            GN_RRLP_Log_MsgDecodeFailed( p_RRLP_Instance->Handle );
            OS_ASSERT( 0 );
            break;
         }
      }
      else
      {
         /// \todo Handle decode errors from RRLP decoder.
         // Log RRLP decode failure
          GN_RRLP_Log_MsgDecodeFailed( p_RRLP_Instance->Handle );
         OS_ASSERT( 0 );
      }
      break;
      // Invalid states
   case state_RRLP_Position_Requested_From_GPS:
      OS_ASSERT( 0 );
      break;
   default:
      // Coding error.
      OS_ASSERT( 0 );
      break;
   }
   rrlp_PDU_Free( p_PDU_Decoded );
   asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
}


//*****************************************************************************
/// \brief
///      Handles the #event_RRLP_GPS_Position_Solution_Available.
//*****************************************************************************
void RRLP_Handle_GPS_Position_Solution_Available(s_RRLP_Message* p_ThisMessage)
{
   s_RRLP_Instance *p_RRLP_Instance = RRLP_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );
   s_PDU_Buffer_Store *p_PDU_Buf;
   void *p_PDU_Src;

   // If the instance is still valid we indicate timeout.
   // If the instance is not valid this is due to a race condition with a result being returned. Do nothing.
   if ( p_RRLP_Instance != NULL )
   {
      switch ( p_RRLP_Instance->p_RRLP->State )
      {
      case state_RRLP_Position_Requested_From_GPS:

         p_PDU_Buf = asn1_PDU_Buffer_Store_Alloc( 0 );

         p_PDU_Src = rrlp_PDU_Make_msrPositionRsp( p_RRLP_Instance->p_RRLP->ReferenceNumber );

         rrlp_PDU_Add_msrPositionRsp_locationInfo( p_PDU_Src,
                                                   p_ThisMessage->Payload.p_GN_AGPS_GAD_Data,
                                                   p_ThisMessage->Payload.GAD_Ref_TOW_Subms);

         if ( rrlp_PDU_Encode( p_PDU_Src, p_PDU_Buf ) )
         {
            BL Status;

            Status = GN_RRLP_PDU_Delivery_Out( p_RRLP_Instance->Handle,
                                               p_PDU_Buf->PDU_Encoded.Length,
                                               p_PDU_Buf->PDU_Encoded.p_PDU_Data,
                                               p_ThisMessage->Payload.p_GN_AGPS_GAD_Data,
                                               p_ThisMessage->Payload.p_GN_GPS_Nav_Data );

            GN_RRLP_Sequence_End_Out( p_RRLP_Instance->Handle );

            RRLP_Instance_State_Transition( p_RRLP_Instance,
                                            state_RRLP_Idle,
                                            "event_RRLP_GPS_Position_Solution_Available" );
         }
         else
         {
            BL          Status;

            Status = GN_RRLP_Sequence_Error_End_Out( p_RRLP_Instance->Handle );
            RRLP_Instance_State_Transition( p_RRLP_Instance,
                                            state_RRLP_Idle,
                                            "event_RRLP_GPS_Position_Solution_Available - Encode Error" );
         }
         asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
         rrlp_PDU_Free( p_PDU_Src );

         // Valid states.
         break;
         // Error handling.
         break;
         // Invalid states
      case state_RRLP_Idle:
      case state_RRLP_Transaction_In_Progress:
      case state_RRLP_Assistance_Data_Delivery:
      case state_RRLP_Measure_Position_Request:
         OS_ASSERT( 0 );
         break;
      default:
         // Coding error.
         OS_ASSERT( 0 );
         break;
      }
   }
}


//*****************************************************************************
/// \brief
///      Handles the #event_RRLP_GPS_Measurements_Available.
//*****************************************************************************
void RRLP_Handle_GPS_Measurements_Available(s_RRLP_Message* p_ThisMessage)
{
   s_RRLP_Instance *p_RRLP_Instance = RRLP_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );
   s_PDU_Buffer_Store *p_PDU_Buf;
   void *p_PDU_Src;

   // If the instance is still valid we indicate timeout.
   // If the instance is not valid this is due to a race condition with a result being returned. Do nothing.
   if ( p_RRLP_Instance != NULL )
   {
      switch ( p_RRLP_Instance->p_RRLP->State )
      {
      case state_RRLP_Position_Requested_From_GPS:
         p_PDU_Buf = asn1_PDU_Buffer_Store_Alloc( 0 );

         p_PDU_Src = rrlp_PDU_Make_msrPositionRsp( p_RRLP_Instance->p_RRLP->ReferenceNumber );

         rrlp_PDU_Add_msrPositionRsp_gps_MeasureInfo(
            p_PDU_Src,
            1, // Only deliver one set for the moment, therefore we can get away
               // without making #s_RRLP_Payload:p_GN_AGPS_Meas an array.
            p_ThisMessage->Payload.p_GN_AGPS_Meas );

         if ( rrlp_PDU_Encode( p_PDU_Src, p_PDU_Buf ) )
         {
            BL          Status;

            Status = GN_RRLP_PDU_Delivery_Out( p_RRLP_Instance->Handle,
               p_PDU_Buf->PDU_Encoded.Length,
               p_PDU_Buf->PDU_Encoded.p_PDU_Data,
               NULL,
               NULL );

            GN_RRLP_Sequence_End_Out( p_RRLP_Instance->Handle );

            RRLP_Instance_State_Transition( p_RRLP_Instance,
                                            state_RRLP_Idle,
                                            "event_RRLP_GPS_Measurements_Available" );
         }
         else
         {
            BL          Status;

            Status = GN_RRLP_Sequence_Error_End_Out( p_RRLP_Instance->Handle );
            RRLP_Instance_State_Transition( p_RRLP_Instance,
                                            state_RRLP_Idle,
                                            "event_RRLP_GPS_Measurements_Available - Encode Error" );
         }
         asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
         rrlp_PDU_Free( p_PDU_Src );

         // Valid states.
         break;
         // Error handling.
         break;
         // Invalid states
      case state_RRLP_Idle:
      case state_RRLP_Transaction_In_Progress:
      case state_RRLP_Assistance_Data_Delivery:
      case state_RRLP_Measure_Position_Request:
         OS_ASSERT( 0 );
         break;
      default:
         // Coding error.
         OS_ASSERT( 0 );
         break;
      }
   }
}


//*****************************************************************************
/// \brief
///      Handles the #event_RRLP_Expiry_MeasureResponseTime.
//*****************************************************************************
void RRLP_Handle_Expiry_MeasureResponseTime(s_RRLP_Message* p_ThisMessage)
{
   s_RRLP_Instance *p_RRLP_Instance = RRLP_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );
   s_PDU_Buffer_Store *p_PDU_Buf = NULL;
   void *p_PDU_Src;
   BL Status;

   // If the instance is still valid we indicate timeout.
   // If the instance is not valid this is due to a race condition with a result being returned. Do nothing.
   if ( p_RRLP_Instance != NULL )
   {
      switch ( p_RRLP_Instance->p_RRLP->State )
      {
      // Valid states.
      case state_RRLP_Position_Requested_From_GPS:
         // Need to send a timeout to the server.
         p_PDU_Src = rrlp_PDU_Make_msrPositionRsp( p_RRLP_Instance->p_RRLP->ReferenceNumber );

         rrlp_PDU_Add_msrPositionRsp_locErrorReason(  p_PDU_Src,
                                                      GN_RRLP_LocErrorReason_notEnoughSats );
         p_PDU_Buf = asn1_PDU_Buffer_Store_Alloc( 0 );
         rrlp_PDU_Encode( p_PDU_Src, p_PDU_Buf );
         rrlp_PDU_Free( p_PDU_Src );

         Status = GN_RRLP_PDU_Delivery_Out( p_RRLP_Instance->Handle,
            p_PDU_Buf->PDU_Encoded.Length,
            p_PDU_Buf->PDU_Encoded.p_PDU_Data,
            NULL,
            NULL );

         GN_RRLP_Sequence_End_Out( p_RRLP_Instance->Handle );

         RRLP_Instance_State_Transition(  p_RRLP_Instance,
                                          state_RRLP_Idle,
                                          "event_RRLP_Expiry_MeasureResponseTime" );
         break;
      case state_RRLP_Idle:
      case state_RRLP_Transaction_In_Progress:
      case state_RRLP_Assistance_Data_Delivery:
      case state_RRLP_Measure_Position_Request:
         // Will only arrive here due to a benign race condition.
         // Do nothing.
         break;
      default:
         // Coding error.
         OS_ASSERT( 0 );
         break;
      }
   }
   asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_RRLP_GPS_Position_Solution_Available
///      message.
//*****************************************************************************
void RRLP_Send_GPS_Position_Solution_Available(
   s_RRLP_Instance      *p_RRLP_Instance,    ///< [in] RRLP Agent instance to process the message.
   s_RRLP_Instance      *p_SrcInstance,      ///< [in] Entity instance where the message originated.
   s_GN_AGPS_GAD_Data   *p_GN_AGPS_GAD_Data, ///< [in] Pointer to a valid position solution.
   U4                   GAD_Ref_TOW,         ///< [in] GAD Data Reference GPS Time of Week [range 0..604799999 ms]
   I2                   GAD_Ref_TOW_Subms,   ///< [in] GAD Data Reference GPS Time of Week Sub-millisecond part [range 0..9999 x 0.0001 ms,  -1 = Unknown]
   s_GN_GPS_Nav_Data    *p_GN_GPS_Nav_Data   ///< [in] Pointer to a valid position solution.
)
{
   s_RRLP_Message *p_MessageToSend;

   p_MessageToSend = RRLP_Message_Init( p_RRLP_Instance, p_SrcInstance );
   p_MessageToSend->Payload.p_GN_AGPS_GAD_Data  = p_GN_AGPS_GAD_Data ;
   p_MessageToSend->Payload.GAD_Ref_TOW         = GAD_Ref_TOW ;
   p_MessageToSend->Payload.GAD_Ref_TOW_Subms   = GAD_Ref_TOW_Subms ;
   p_MessageToSend->Payload.p_GN_GPS_Nav_Data   = p_GN_GPS_Nav_Data ;
   p_MessageToSend->Payload.MsgEvent            = event_RRLP_GPS_Position_Solution_Available ;
   RRLP_Queue_Add_Message( p_MessageToSend );
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_RRLP_GPS_Measurements_Available
///      message.
//*****************************************************************************
void RRLP_Send_GPS_Measurements_Available(
   s_RRLP_Instance *p_RRLP_Instance,///< RRLP Agent instance to process the message.
   s_RRLP_Instance *p_SrcInstance,  ///< Entity instance where the message originated.
   s_GN_AGPS_Meas  *p_GN_AGPS_Meas  ///< Pointer to GPS measurements.
)
{
   s_RRLP_Message *p_MessageToSend;

   p_MessageToSend = RRLP_Message_Init( p_RRLP_Instance, p_SrcInstance );
   p_MessageToSend->Payload.p_GN_AGPS_Meas   = p_GN_AGPS_Meas ;
   p_MessageToSend->Payload.MsgEvent         = event_RRLP_GPS_Measurements_Available ;
   RRLP_Queue_Add_Message( p_MessageToSend );
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_RRLP_Expiry_MeasureResponseTime
///      message.
//*****************************************************************************
void RRLP_Send_Expiry_MeasureResponseTime(
   s_RRLP_Instance *p_RRLP_Instance,///< RRLP Agent instance to process the message.
   s_RRLP_Instance *p_SrcInstance   ///< Entity instance where the message originated.
)
{
   s_RRLP_Message *p_MessageToSend;

   p_MessageToSend = RRLP_Message_Init( p_RRLP_Instance, p_SrcInstance );
   p_MessageToSend->Payload.MsgEvent = event_RRLP_Expiry_MeasureResponseTime;
   RRLP_Queue_Add_Message( p_MessageToSend );
}
