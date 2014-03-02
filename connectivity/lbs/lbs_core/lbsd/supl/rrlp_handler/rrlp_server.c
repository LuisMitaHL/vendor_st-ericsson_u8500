//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename rrlp_server.c
//
// $Header: X:/MKS Projects/prototype/prototype/rrlp_handler/rcs/rrlp_server.c 1.11 2009/01/19 12:32:28Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_handler
///
/// \file
/// \brief
///      RRLP server implementation.
///
//*************************************************************************

//#include <stdio.h>
#include <string.h>

#include "os_assert.h"

#include "rrlp_messages.h"
#include "rrlp_interface.h"
#include "rrlp_helper.h"

#include "GN_GPS_api.h"
#include "GN_AGPS_api.h"
#include "GN_RRLP_server_api.h"

// global data
extern s_RRLP_Instances    RRLP_Instances;
extern s_RRLP_Queue        RRLP_Queue;

BL GN_GPS_Get_Nav_Data_Copy(
   s_GN_GPS_Nav_Data* p_Nav_Data    ///< [in] Pointer to where the GPS Library should write the Nav Data to.
);

void RRLP_Handle_Initiate_POS_Sequence(            s_RRLP_Message* p_ThisMessage);
void RRLP_Handle_POS_Payload(                      s_RRLP_Message* p_ThisMessage);
void RRLP_Handle_Terminate_POS_Sequence(           s_RRLP_Message* p_ThisMessage);
void RRLP_Handle_Expiry_MeasureResponseTime(       s_RRLP_Message* p_ThisMessage);

void RRLP_Send_Expiry_MeasureResponseTime(         s_RRLP_Instance *p_RRLP_Instance, s_RRLP_Instance *p_SrcInstance);

//*****************************************************************************
/// \brief
///      Retrieve messages and process them.
//*****************************************************************************
void RRLP_Message_Handler(void)
{
   s_RRLP_Message* p_ThisMessage;

   /// Do any processing first as this may initiate a message.
   RRLP_Instance_Handler();

   /// Retrieve each message on the queue.
   while( ( p_ThisMessage = RRLP_Queue_Retrieve_Message() ) != NULL )
   {
      /// If a message is available call it's handler.
      switch ( p_ThisMessage->Payload.MsgEvent )
      {
      case event_RRLP_Initiate_POS_Sequence:             RRLP_Handle_Initiate_POS_Sequence(  p_ThisMessage ); break;
      case event_RRLP_POS_Payload:                       RRLP_Handle_POS_Payload(            p_ThisMessage ); break;
      case event_RRLP_Terminate_POS_Sequence:            RRLP_Handle_Terminate_POS_Sequence( p_ThisMessage ); break;
      default:
         // log error
         OS_ASSERT( 0 );
         break;
     }
     // free up the message
     RRLP_Message_Free( p_ThisMessage );
     /// Make sure we do any processing after each message is dealt with.
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
   if ( p_RRLP_Instance == NULL )
   {
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
///      position response.
/// \returns
///      Nothing.
//*****************************************************************************
void RRLP_Process_msrPositionRsp(
   void              *p_PDU_Decoded,   ///< Transparent handle to a decoded RRLP PDU.
   s_RRLP_Instance   *p_RRLP_Instance  ///< RRLP_Instance handling this PDU.
)
{
   BL          Status;

   Status = GN_RRLP_Sequence_End_Out( p_RRLP_Instance->Handle );

   RRLP_Instance_State_Transition(  p_RRLP_Instance,
                                    state_RRLP_Transaction_In_Progress,
                                    "event_RRLP_POS_Payload(msrPositionReq)" );
}


//*****************************************************************************
/// \brief
///      GN GPS RRLP API Function to process an RRLP PDU containing an assistance
///      data acknowledgement.
/// \returns
///      Nothing.
//*****************************************************************************
void RRLP_Process_assistanceDataAck(
   void              *p_PDU_Decoded,   ///< Transparent handle to a decoded RRLP PDU.
   s_RRLP_Instance   *p_RRLP_Instance  ///< RRLP_Instance handling this PDU.
)
{
   s_PDU_Buffer_Store *p_PDU_Buf = asn1_PDU_Buffer_Store_Alloc( 0 );
   s_GN_RRLP_MethodType GN_RRLP_MethodType;
   void *p_PDU_Src;

   switch ( p_RRLP_Instance->p_RRLP->GN_PrefMethod )
   {
   case GN_noPreference:
   case GN_agpsSETBasedPreferred:
      /// \todo Provide OS lookup for #s_GN_RRLP_MethodType:methodType.
      GN_RRLP_MethodType.methodType = GN_msBased;
      /// \todo Calculate value for required accuracy.
      GN_RRLP_MethodType.accuracy = 5;
      break;

   case GN_agpsSETassistedPreferred:
      /// \todo Provide OS lookup for #s_GN_RRLP_MethodType:methodType.
      GN_RRLP_MethodType.methodType = GN_msAssisted;
      /// \todo Calculate value for required accuracy.
      GN_RRLP_MethodType.accuracy = -1;
      break;
   }
   p_PDU_Src = rrlp_PDU_Make_msrPositionReq( p_RRLP_Instance->p_RRLP->ReferenceNumber, &GN_RRLP_MethodType );
   if ( rrlp_PDU_Encode( p_PDU_Src, p_PDU_Buf ) )
   {
      BL          Status;

      Status = GN_RRLP_PDU_Delivery_Out( p_RRLP_Instance->Handle,
         p_PDU_Buf->PDU_Encoded.Length,
         p_PDU_Buf->PDU_Encoded.p_PDU_Data,
         NULL,
         NULL );
      RRLP_Instance_State_Transition( p_RRLP_Instance,
                                      state_RRLP_Measure_Position_Request,
                                      "event_RRLP_POS_Payload(assistanceDataAck)" );
   }
   else
   {
      BL          Status;

      asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
      Status = GN_RRLP_Sequence_End_Out( p_RRLP_Instance->Handle );
      RRLP_Instance_State_Transition( p_RRLP_Instance,
                                      state_RRLP_Idle,
                                      "event_RRLP_POS_Payload(assistanceDataAck) - Encode Error");
   }
   rrlp_PDU_Free( p_PDU_Src );
}
//
//*****************************************************************************
/// \brief
///      Handles the #event_RRLP_Initiate_POS_Sequence.
/// \details
///      Handles the initialisation of the positioning subsystem.
//*****************************************************************************
void RRLP_Handle_Initiate_POS_Sequence( s_RRLP_Message *p_ThisMessage )
{
   s_RRLP_Instance *p_RRLP_Instance = p_ThisMessage->MsgHdr.p_InstanceOfDest;
   s_PDU_Buffer_Store *p_PDU_Buf;
   void *p_PDU_Src;
   BL PDU_Ready_To_Send = FALSE;
   e_RRLP_States   Next_State;

   if ( p_RRLP_Instance == NULL )
   {
      p_RRLP_Instance = RRLP_Instance_Request_New();
      p_RRLP_Instance->p_RRLP->ReferenceNumber = ( p_RRLP_Instance->ThisInstanceId ) % 8;
   }

   if ( p_ThisMessage->Payload.p_GN_AGPS_GAD_Data != NULL )
   {
      // We have a reference position.
      if ( p_RRLP_Instance->p_RRLP->p_GN_AGPS_GAD_Data == NULL )
      { // Make sure we have storage in this instance.
         p_RRLP_Instance->p_RRLP->p_GN_AGPS_GAD_Data = GN_Calloc( 1, sizeof( s_GN_AGPS_GAD_Data ) );
      }
      memcpy (p_RRLP_Instance->p_RRLP->p_GN_AGPS_GAD_Data,
              p_ThisMessage->Payload.p_GN_AGPS_GAD_Data,
              sizeof( s_GN_AGPS_GAD_Data ) );
   }

   Next_State = p_RRLP_Instance->p_RRLP->State;
   switch ( p_RRLP_Instance->p_RRLP->State )
   {
   case state_RRLP_Idle:
   case state_RRLP_Transaction_In_Progress:
   case state_RRLP_Measure_Position_Request:
      // Valid states.
      p_PDU_Src = rrlp_PDU_Make_assistanceData( p_RRLP_Instance->p_RRLP->ReferenceNumber, p_ThisMessage->Payload.p_GN_AGPS_GAD_Data, &p_RRLP_Instance->p_RRLP->GN_RequestedAssistData );
      PDU_Ready_To_Send = TRUE;
      Next_State = state_RRLP_Assistance_Data_Delivery;
      break;
   // Error handling.
   case state_RRLP_Position_Requested_From_GPS:
      // Invalid states
      OS_ASSERT( 0 );
      break;
   default:
      // Coding error.
      OS_ASSERT( 0 );
      break;
   }
   if( PDU_Ready_To_Send )
   {
      p_PDU_Buf = asn1_PDU_Buffer_Store_Alloc( 0 );
      if ( rrlp_PDU_Encode( p_PDU_Src, p_PDU_Buf ) )
      {
         BL          Status;

         Status = GN_RRLP_PDU_Delivery_Out( p_RRLP_Instance->Handle,
            p_PDU_Buf->PDU_Encoded.Length,
            p_PDU_Buf->PDU_Encoded.p_PDU_Data,
            NULL,
            NULL );

         asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
         RRLP_Instance_State_Transition( p_RRLP_Instance,
                                         Next_State,
                                         "event_RRLP_Initiate_POS_Sequence");
      }
      else
      {
         BL          Status;

         asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
         Status = GN_RRLP_Sequence_End_Out( p_RRLP_Instance->Handle );
         RRLP_Instance_State_Transition( p_RRLP_Instance,
                                         state_RRLP_Idle,
                                         "event_RRLP_POS_Payload(assistanceDataAck) - Encode Error");
      }
      rrlp_PDU_Free( p_PDU_Src );
   }
}
//
//*****************************************************************************
/// \brief
///      Handles the #event_RRLP_POS_Payload.
/// \details
///      Sends a message to the supl server state machine to containing a POS
///      payload e.g. from a SUPL-POS pdu.
//*****************************************************************************
void RRLP_Handle_POS_Payload( s_RRLP_Message *p_ThisMessage )
{
   s_PDU_Buffer_Store  *p_PDU_Buf = GN_Calloc( 1, sizeof( s_PDU_Buffer_Store ) );
   void                *p_PDU_Decoded;
   e_GN_RRLP_Component_PR   This_Message_Type;
   GN_RRLP_ErrorCodes   GN_RRLP_ErrorCodes;

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
      // p_PDU_Decoded = rrlp_PDU_Alloc();
      // decode PDU.
      p_PDU_Buf->PDU_Encoded.p_PDU_Data = p_ThisMessage->Payload.p_RRLP_PDU->p_PDU_Data;
      p_PDU_Buf->PDU_Encoded.Length     = p_ThisMessage->Payload.p_RRLP_PDU->Length;

      p_ThisMessage->Payload.p_RRLP_PDU->p_PDU_Data = NULL;

      rrlp_PDU_Decode( p_PDU_Buf, &p_PDU_Decoded, &GN_RRLP_ErrorCodes );
      asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
      This_Message_Type = rrlp_PDU_Get_MessageType( p_PDU_Decoded );
      switch ( This_Message_Type )
      {
      case GN_RRLP_Component_PR_msrPositionReq:
         OS_ASSERT( 0 ); // \todo Return Protocol error as server should not receive this.
         //RRLP_Process_msrPositionReq(p_PDU_Decoded, p_RRLP_Instance, p_ThisMessage->Payload.PDU_TimeStamp);
         break;
      case GN_RRLP_Component_PR_msrPositionRsp:
         RRLP_Process_msrPositionRsp( p_PDU_Decoded, p_RRLP_Instance );
         break;
      case GN_RRLP_Component_PR_assistanceData:
         OS_ASSERT( 0 ); // \todo Return Protocol error as server should not receive this.
         //RRLP_Process_assistanceData(p_PDU_Decoded, p_RRLP_Instance);
         break;
      case GN_RRLP_Component_PR_assistanceDataAck:
         RRLP_Process_assistanceDataAck( p_PDU_Decoded, p_RRLP_Instance );
         break;
      case GN_RRLP_Component_PR_protocolError:
         RRLP_Process_protocolError( p_PDU_Decoded, p_RRLP_Instance );
         break;
      case GN_RRLP_Component_PR_NOTHING:  /* No components present */
      default:
         OS_ASSERT( 0 );
         break;
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
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_RRLP_Initiate_POS_Sequence
///      message.
/// \details
///      Sends a message to the supl server state machine to start a
///      positioning subsystem sequence.
//*****************************************************************************
void RRLP_Send_Initiate_POS_Sequence(
   s_RRLP_Instance *p_RRLP_Instance,///< RRLP Agent instance to process
                                    ///  the message.
   s_RRLP_Instance *p_SrcInstance,  ///< Entity instance where the message
                                    ///  originated.
   s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data      ///< [in] Reference Location in GAD format.
)
{
   s_RRLP_Message *p_MessageToSend;

   p_MessageToSend = RRLP_Message_Init( p_RRLP_Instance, p_SrcInstance );

   p_MessageToSend->Payload.p_GN_AGPS_GAD_Data = GN_Calloc( 1, sizeof( s_GN_AGPS_GAD_Data ) );

   memcpy ( p_MessageToSend->Payload.p_GN_AGPS_GAD_Data,
            p_GN_AGPS_GAD_Data,
            sizeof( s_GN_AGPS_GAD_Data ) );

   p_MessageToSend->Payload.MsgEvent = event_RRLP_Initiate_POS_Sequence;

   RRLP_Queue_Add_Message( p_MessageToSend );
}

//*****************************************************************************
/// \brief
///      RRLP Server Start in.
/// \details
///      Starts the RRLP sequence from the server side.
///      <p> For the RRLP Server Start the RRLP subsystem provides a
///      Handle which is used in subsequent exchanges to tie up the RRLP
///      sequence.
///      <ul>
///         <li><var>#GN_SUCCESS \copydoc GN_SUCCESS</var>\n
///            p_Status should be set to this value when the return value
///            indicated success.</li>
///         <li><var>#GN_ERR_NO_RESOURCE \copydoc GN_ERR_NO_RESOURCE</var>\n
///            p_Status should be set to this value when there are not
///            enough resources available to the library to perform the
///            requested function.</li>
///      </ul>
/// \attention
///      The calling party is responsible for managing the memory referenced by
///      p_Status, p_GN_AGPS_GAD_Data and p_RequestedAssistData.
/// \returns
///      Flag to indicate success or failure of the posting of the request.
/// \retval #TRUE Flag indicating success.
/// \retval #FALSE Flag indicating failure and that the status should be checked.
BL GN_RRLP_Server_Start_In(
   void*                *p_Handle,           ///< [out] Opaque Handle used to coordinate requests.
   e_GN_Status          *p_Status,           ///< [in/out] Status of disconnect and to be checked when return flag indicates failure.
   s_GN_AGPS_GAD_Data   *p_GN_AGPS_GAD_Data, ///< [in] Reference Location in GAD format.
   e_GN_PrefMethod      GN_PrefMethod,       ///< [in] Positioning Method the Positioning subsystem should use.
   s_GN_RequestedAssistData
                        *p_RequestedAssistData /// [in] Assistance data required.
)
{
   s_GN_AGPS_GAD_Data *p_GAD_Temp = GN_Calloc( 1, sizeof( s_GN_AGPS_GAD_Data ) );
   s_RRLP_Instance *p_RRLP_Instance = RRLP_Instance_Request_New();

   if ( p_RRLP_Instance != NULL )
   {
      // Create an RRLP reference number between 0 and 7.
      p_RRLP_Instance->p_RRLP->ReferenceNumber = p_RRLP_Instance->ThisInstanceId % 8;

      p_RRLP_Instance->p_RRLP->State = state_RRLP_Transaction_In_Progress;
      p_RRLP_Instance->p_RRLP->GN_PrefMethod = GN_PrefMethod;

      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.almanacRequested    = p_RequestedAssistData->almanacRequested;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.utcModelRequested   = p_RequestedAssistData->utcModelRequested;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.ionosphericModelRequested       = p_RequestedAssistData->ionosphericModelRequested;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.dgpsCorrectionsRequested        = p_RequestedAssistData->dgpsCorrectionsRequested;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.referenceLocationRequested      = p_RequestedAssistData->referenceLocationRequested;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.referenceTimeRequested          = p_RequestedAssistData->referenceTimeRequested;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.acquisitionAssistanceRequested  = p_RequestedAssistData->acquisitionAssistanceRequested;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.realTimeIntegrityRequested      = p_RequestedAssistData->realTimeIntegrityRequested;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.navigationModelRequested        = p_RequestedAssistData->navigationModelRequested;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.navigationModelDataPresent      = p_RequestedAssistData->navigationModelDataPresent;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.gpsWeek          = p_RequestedAssistData->gpsWeek;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.gpsToe           = p_RequestedAssistData->gpsToe;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.nSAT             = p_RequestedAssistData->nSAT;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.toeLimit         = p_RequestedAssistData->toeLimit;
      p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.satInfoPresent   = p_RequestedAssistData->satInfoPresent;
      memcpy ( p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.SatID, p_RequestedAssistData->SatID, sizeof( p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.SatID ) );
      memcpy ( p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.IODE,  p_RequestedAssistData->IODE,  sizeof( p_RRLP_Instance->p_RRLP->GN_RequestedAssistData.IODE ) );

      *p_Handle = p_RRLP_Instance->Handle;
      memcpy( p_GAD_Temp, p_GN_AGPS_GAD_Data, sizeof( s_GN_AGPS_GAD_Data ) );
      RRLP_Send_Initiate_POS_Sequence( p_RRLP_Instance, NULL, p_GAD_Temp );
      GN_Free( p_GAD_Temp );
      *p_Status = GN_SUCCESS;

      return TRUE;
   }
   else
   {
      *p_Status = GN_ERR_NO_RESOURCE;
      return FALSE;
   }
}
