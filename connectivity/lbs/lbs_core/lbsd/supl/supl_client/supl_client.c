
//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
//
// Filename supl_client.c
//
// $Header: X:/MKS Projects/prototype/prototype/supl_client/rcs/supl_client.c 1.97 2009/02/20 16:30:19Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_client
///
/// \file
/// \brief
///      SUPL Client Implementation.
///
//*************************************************************************

#include <stdio.h>
#include <string.h>

#include "os_assert.h"

#include "supl_config.h"
#include "supl_helper.h"
#include "supl_interface.h"
#include "supl_module.h"
#include "supl_log.h"
#include "supl_hmac.h"
#include "time.h"

extern s_GN_SUPL_V2_Multiple_Loc_ID *SUPL_Mlid_FirstElem;

#define SUPL_MODULE_LOGGING

// Prototypes for static functions local to this module.

static void SUPL_Handle_PDU_SUPL_INIT(      void *p_PDU_Decoded, s_SUPL_Instance *p_SUPL_Instance, s_PDU_Buffer_Store *p_PDU_Buf_Incoming );
static void SUPL_Handle_PDU_SUPL_RESPONSE(  void *p_PDU_Decoded, s_SUPL_Instance *p_SUPL_Instance );
static void SUPL_Handle_PDU_SUPL_AUTH_RESP( s_SUPL_Instance *p_SUPL_Instance );
static void SUPL_Handle_Comms_Open_Success( s_SUPL_Message* p_ThisMessage );
static void SUPL_V2_Handle_Trigger( s_SUPL_Message* p_ThisMessage );
static BL supl_Verify_Ecall_WhiteList(s_GN_SUPL_V2_InstanceData *p_SUPL_V2_InstanceData);
void SUPL_V2_Handle_Trigger_Periodic_Requested( s_SUPL_Message* p_ThisMessage );

void SUPL_V2_Handle_ThirdParty_Location_Request( s_SUPL_Message* p_ThisMessage );
void  SUPL_V2_Handle_ThirdPartyPosReq( s_SUPL_Message* p_ThisMessage );
void SUPL_V2_Handle_ApplicationID_Info_Request(s_SUPL_Message* p_ThisMessage);
void SUPL_V2_Handle_Mobile_Info_Received( s_SUPL_Message* p_ThisMessage );
void SUPL_V2_Handle_Delete_Location_id_Requested( s_SUPL_Message* p_ThisMessage );

// @TODO : Remove dependency on this function!
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

/* +LMSqb89433 */
static e_SUPL_States SUPL_Handle_SUPL_INIT_Comms( s_SUPL_Instance   *p_SUPL_Instance , void **pp_PDU_Src )
{
    e_SUPL_States       Next_State;
    void*               p_PDU_Src;

         if ( p_SUPL_Instance->p_SUPL->Notification_Accepted )
         {
            if ( p_SUPL_Instance->p_SUPL->GN_PosMethod_Requested == GN_PosMethod_noPosition )
            {
               p_PDU_Src   =  supl_PDU_Make_SUPLEND( p_SUPL_Instance );
               if ( p_SUPL_Instance->p_SUPL->SUPL_Notification.GN_SUPL_Notify_Type != NOTIFY_ONLY )
               {
                  // If we have a no position notification with notification type of NOTIFY_ONLY we don't need to grant consent.
                  // For all other cases we do.
                  supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, GN_StatusCode_consentGrantedByUser );
               }
               supl_PDU_Add_SUPLEND_VER(        p_PDU_Src, p_SUPL_Instance );

               Next_State  =  state_SUPL_Comms_Close_Sent;
            }
            else
            {

               s_GN_Position     GN_Position_Store_Local;
               memset (&GN_Position_Store_Local, 0, sizeof (GN_Position_Store_Local));

               /* In SUPL v2.0 we check if Triggered Mode is requested */
               if( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode != GN_TriggeredModeNone )
               {
                  p_PDU_Src = supl_PDU_Make_SUPLTRIGGEREDSTART( p_SUPL_Instance);

                  if ( GN_SUPL_Cache_Valid( &GN_Position_Store_Local, p_SUPL_Instance->p_SUPL->p_GN_QoP ) )
                  {
                      supl_PDU_Add_SUPLTRIGGEREDSTART_Position( p_PDU_Src, &GN_Position_Store_Local );
                      GN_SUPL_Log_CachePosReport( (void*) p_SUPL_Instance , &GN_Position_Store_Local , 0 );
                  }

                  Next_State  = state_SUPLv2_SUPL_TRIGGER_START_Sent;
                  // Start timer.
                  GN_SUPL_Timer_Set_UT1( p_SUPL_Instance );
               }
               else
               {
                   p_PDU_Src   =  supl_PDU_Make_SUPLPOSINIT( p_SUPL_Instance );
                   if ( GN_SUPL_Cache_Valid( &GN_Position_Store_Local, p_SUPL_Instance->p_SUPL->p_GN_QoP ) )
                   {
                      supl_PDU_Add_SUPLPOSINIT_Position( p_PDU_Src, &GN_Position_Store_Local );
                      GN_SUPL_Log_CachePosReport( (void*) p_SUPL_Instance , &GN_Position_Store_Local , 0 );
                   }
                   Next_State  =  state_SUPL_SUPL_POS_INIT_Sent;
                  // Start timer.
                  GN_SUPL_Timer_Set_UT2( p_SUPL_Instance );
               }
            }
         }
         else
         {
            p_PDU_Src   =  supl_PDU_Make_SUPLEND( p_SUPL_Instance );
            supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, GN_StatusCode_consentDeniedByUser );
            supl_PDU_Add_SUPLEND_VER(        p_PDU_Src, p_SUPL_Instance );
            Next_State  =  state_SUPL_Comms_Close_Sent;
         }

    *pp_PDU_Src = p_PDU_Src;

    return Next_State;
}
/* -LMSqb89433 */

//*****************************************************************************
/// \brief
///      Handles the SUPL Notify Response and creates the PDU for Notify Response
//*****************************************************************************
static e_SUPL_States SUPL_Handle_SUPL_NOTIFY_RESPONSE( s_SUPL_Instance   *p_SUPL_Instance , void **pp_PDU_Src )
{
    e_SUPL_States       Next_State;
    void*               p_PDU_Src = NULL;

    if ( p_SUPL_Instance->p_SUPL->Notification_Accepted )
    {
          if ( p_SUPL_Instance->p_SUPL->SUPL_Notification.GN_SUPL_Notify_Type != NOTIFY_ONLY )
          {

              p_PDU_Src   =  supl_PDU_Make_SUPLNOTIFYRESPONSE( p_SUPL_Instance, GN_StatusCode_consentGrantedByUser  );

          }
          else
          {
              /*NOTIFY RESPONSE need to be sent to also incase of Notify Only*/
              p_PDU_Src   =  supl_PDU_Make_SUPLNOTIFYRESPONSE( p_SUPL_Instance, GN_StatusCode_NONE);
          }
    }
    else
    {

        p_PDU_Src   =  supl_PDU_Make_SUPLNOTIFYRESPONSE( p_SUPL_Instance, GN_StatusCode_consentDeniedByUser  );
    }
    Next_State  =  state_SUPLv2_Location_Notification;

    /*UT5 timer is started and shall expire if SUPL END is not received.*/
    GN_SUPL_V2_Timer_Set_UT5(p_SUPL_Instance);

    *pp_PDU_Src = p_PDU_Src;

    return Next_State;


}


//*****************************************************************************
/// \brief
///      Initialises the SUPL SubSystem (i.e. SUPL_Client + SUPL_Module)
//*****************************************************************************
void SUPL_SubSystem_Init( void )
{
   SUPL_Module_Init();
   SUPL_Send_Initialisation( NULL, NULL );
//   SUPL_Assistance_Request();
   return;
}


//*****************************************************************************
/// \brief
///      Handles all new SUPL Messages until there are none left in the queue.
//*****************************************************************************
void SUPL_Message_Handler( void )
{
   s_SUPL_Message* p_ThisMessage;

   /// Do any processing of existing instances first as this may initiate a message.
   SUPL_Instance_Handler();

   /// Retrieve a message from the SUPL Queue and call a function to process
   /// it based on the message type.
   while( ( p_ThisMessage = SUPL_Queue_Retrieve_Message() ) != NULL )
   {
      switch( p_ThisMessage->Payload.MsgEvent )
      {
      case event_SUPL_Initialisation:                 SUPL_Handle_Initialisation       ( p_ThisMessage );           break;
      case event_SUPL_PDU_Received:                   SUPL_Handle_PDU_Received         ( p_ThisMessage );           break;
      case event_SUPL_Comms_Open_Success:             SUPL_Handle_Comms_Open_Success   ( p_ThisMessage );           break;
      case event_SUPL_Comms_Open_Failure:             SUPL_Handle_Comms_Open_Failure   ( p_ThisMessage );           break;
      case event_SUPL_Comms_Close:                    SUPL_Handle_Comms_Close          ( p_ThisMessage );           break;
      case event_SUPL_POS_Payload:                    SUPL_Handle_POS_Payload          ( p_ThisMessage );           break;
      case event_SUPL_POS_Sequence_Error:             SUPL_Handle_POS_Sequence_Error   ( p_ThisMessage );           break;
      case event_SUPL_POS_Sequence_Complete:          SUPL_Handle_POS_Sequence_Complete( p_ThisMessage );           break;
      case event_SUPL_Expiry_UT1:                     SUPL_Handle_Expiry_UT1           ( p_ThisMessage );           break;
      case event_SUPL_Expiry_UT2:                     SUPL_Handle_Expiry_UT2           ( p_ThisMessage );           break;
      case event_SUPL_Expiry_UT3:                     SUPL_Handle_Expiry_UT3           ( p_ThisMessage );           break;
      case event_SUPL_Notification_Request:           SUPL_Handle_Notification_Request ( p_ThisMessage );           break;
      case event_SUPL_Notification_Response:          SUPL_Handle_Notification_Response( p_ThisMessage );           break;
      case event_SUPL_MO_Position_Requested:          SUPL_Handle_MO_Position_Requested( p_ThisMessage );           break;
/* +LMSqc38060 */
      case event_SUPL_Abort_Requested:                SUPL_Handle_Abort_Requested      ( p_ThisMessage );           break;
/* -LMSqc38060 */
      case event_SUPL_Trigger_End_Requested:          SUPL_Handle_Trigger_End_Requested( p_ThisMessage );           break;
      case event_SUPL_V2_Trigger:                     SUPL_V2_Handle_Trigger           ( p_ThisMessage );           break;
      case event_SUPL_V2_Trigger_Periodic_Requested:  SUPL_V2_Handle_Trigger_Periodic_Requested ( p_ThisMessage );  break;
      case event_SUPL_V2_Third_Party_info_Request:    SUPL_V2_Handle_ThirdParty_Location_Request( p_ThisMessage );  break;
      case event_SUPL_V2_ThirdPartyPosReq_Received:   SUPL_V2_Handle_ThirdPartyPosReq           ( p_ThisMessage );  break;
      case event_SUPL_V2_ApplicationId_info_Request:  SUPL_V2_Handle_ApplicationID_Info_Request ( p_ThisMessage );  break;
      case event_SUPL_V2_Mobile_Info_Received:        SUPL_V2_Handle_Mobile_Info_Received( p_ThisMessage ); break;
      case event_SUPL_V2_Delete_Location_id_Requested:SUPL_V2_Handle_Delete_Location_id_Requested( p_ThisMessage ); break;
      case event_SUPL_V2_Expiry_UT5:                  SUPL_V2_Handle_Expiry_UT5        ( p_ThisMessage );           break;
      case event_SUPL_V2_Expiry_UT7:                  SUPL_V2_Handle_Expiry_UT7        ( p_ThisMessage );           break;
      case event_SUPL_V2_Expiry_UT8:                  SUPL_V2_Handle_Expiry_UT8        ( p_ThisMessage );           break;
      case event_SUPL_V2_Expiry_UT9:                  SUPL_V2_Handle_Expiry_UT9        ( p_ThisMessage );           break;
         break;

      default:
         // log error
         GN_SUPL_Log( "SUPL_Message_Handler: Unhandled message id: %u", p_ThisMessage->Payload.MsgEvent );
         OS_ASSERT( 0 );
         break;
      }
      /// Once the message has been processed free it up.
      SUPL_Message_Free( p_ThisMessage );
      /// Run through processing the messages again in case a message has
      /// kicked off some event.
      SUPL_Instance_Handler();
   }

   return;
}

s_PDU_Buffer_Store g_Segmented_PDU_Buffer = {
     . PDU_Encoded = {
           .p_PDU_Data = NULL,
     },
};


void SUPL_Segmented_PDU_Buffer_Init(void)
{
   if( g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data != NULL )
       GN_Free( g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data );

   g_Segmented_PDU_Buffer.PDU_Encoded.Length = 0;
   g_Segmented_PDU_Buffer.AvailSize = 0;
   g_Segmented_PDU_Buffer.PDU_Complete = 0;
   g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data = NULL;
}


//*****************************************************************************
/// \brief
///      Concatenates the incomming pdu data with the g_Segmented_PDU_Buffer(which stores
///      the previously received partial pdu).
//*****************************************************************************
void SUPL_Concat_Segmented_PDU(s_PDU_Buffer_Store *p_PDU)
{
    if( g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data != NULL && p_PDU->PDU_Encoded.p_PDU_Data != NULL )
    {
        U2 totalLength = 0;
        U1 *data;
        GN_SUPL_Log("SUPL_Concat_Segmented_PDU: Previous Segment available");

        totalLength = g_Segmented_PDU_Buffer.PDU_Encoded.Length + p_PDU->PDU_Encoded.Length;
        //Allocate memory for the concatenate data
        data = (U1 *)GN_Calloc( totalLength, (U2)sizeof(U1) );
        //copy data from both g_Segmented_PDU_Buffer and parameter p_PDU
        memcpy(data, g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data, g_Segmented_PDU_Buffer.PDU_Encoded.Length );
        memcpy(data + g_Segmented_PDU_Buffer.PDU_Encoded.Length, p_PDU->PDU_Encoded.p_PDU_Data, p_PDU->PDU_Encoded.Length );

        //Free allocated memory of  g_Segmented_PDU_Buffer
        GN_Free( g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data );


        //Copy the concatenated data to g_Segmented_PDU_Buffer
        g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data = data;
        g_Segmented_PDU_Buffer.PDU_Encoded.Length = totalLength;
        g_Segmented_PDU_Buffer.AvailSize = g_Segmented_PDU_Buffer.PDU_Encoded.Length;
    }
    else if( p_PDU->PDU_Encoded.p_PDU_Data != NULL )
    {
        GN_SUPL_Log("SUPL_Concat_Segmented_PDU: New segment; adding to the buffer");
        //Allocate memory for the concatenate data
        g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data = (U1 *)GN_Calloc( p_PDU->PDU_Encoded.Length, (U2)sizeof(U1) );

        //Copy the concatenated data to g_Segmented_PDU_Buffer
        memcpy(g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data, p_PDU->PDU_Encoded.p_PDU_Data, p_PDU->PDU_Encoded.Length );
        g_Segmented_PDU_Buffer.PDU_Encoded.Length = p_PDU->PDU_Encoded.Length;
        g_Segmented_PDU_Buffer.AvailSize = g_Segmented_PDU_Buffer.PDU_Encoded.Length;
    }
}


//*****************************************************************************
/// \brief
///      Check if the g_Segmented_PDU_Buffer has full PDU.
//*****************************************************************************
BL SUPL_Is_Full_Pdu_Available()
{
    U2 pduSize = 0;
    BL retVal = FALSE;

    if( g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data != NULL )
    {
        //Calculate the size of the pdu from the data
        pduSize = g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data[0];
        pduSize = (pduSize << 8) +  g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data[1];

        //Check if sufficient data is available in the g_Segmented_PDU_Buffer
        if(pduSize <= g_Segmented_PDU_Buffer.PDU_Encoded.Length )
            retVal = TRUE;

    }

    return retVal;
}


//*****************************************************************************
/// \brief
///      buffers the incoming pdu if it is not a complete pdu.
//*****************************************************************************
s_PDU_Buffer_Store * SUPL_Buffer_Segmented_PDU(s_PDU_Buffer_Store *p_PDU)
{
   SUPL_Concat_Segmented_PDU( p_PDU );
   GN_SUPL_Log("SUPL_Buffer_Segmented_PDU: PDU concatenated");
   if( SUPL_Is_Full_Pdu_Available() )
   {
       GN_SUPL_Log("SUPL_Buffer_Segmented_PDU: PDU is complete");
       //free the existing buffer in parameter p_PDU and copy the new data into it after allocating the memory
       GN_Free( p_PDU->PDU_Encoded.p_PDU_Data );
       p_PDU->PDU_Encoded.p_PDU_Data = (U1 *)GN_Calloc( (U2)g_Segmented_PDU_Buffer.PDU_Encoded.Length, (U2)sizeof(U1) );
       memcpy(p_PDU->PDU_Encoded.p_PDU_Data, g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data, g_Segmented_PDU_Buffer.PDU_Encoded.Length );

       //copy the remaining data also to the parameter p_PDU from g_Segmented_PDU_Buffer
       p_PDU->AvailSize = g_Segmented_PDU_Buffer.AvailSize;
       p_PDU->PDU_Complete = g_Segmented_PDU_Buffer.PDU_Complete;
       p_PDU->PDU_Encoded.Length = g_Segmented_PDU_Buffer.PDU_Encoded.Length;

       //free the g_Segmented_PDU_Buffer
       g_Segmented_PDU_Buffer.AvailSize = 0;
       g_Segmented_PDU_Buffer.PDU_Complete = 0;
       g_Segmented_PDU_Buffer.PDU_Encoded.Length = 0;
       GN_Free( (void *)g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data );
       g_Segmented_PDU_Buffer.PDU_Encoded.p_PDU_Data = NULL;

       return p_PDU;
   }
   else
   {
       return NULL;
   }
}



//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_PDU_Received message.
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_PDU_Received this function decodes the PDU and calls a
///      function to process it depending on the encapsulated PDU.
//*****************************************************************************
void SUPL_Handle_PDU_Received( s_SUPL_Message* p_ThisMessage )
{
   s_SUPL_Instance      *p_SUPL_Instance;
   s_PDU_Buffer_Store   *p_PDU_Buf_Incoming;
   e_GN_UlpMessageType  This_Message_Type;
   e_SUPL_States        Next_State;
   void                 *p_PDU_Decoded       = 0; /* Type to decode        */
   void                 *p_PDU_Src           = 0;
   e_GN_StatusCode      GN_StatusCode        = GN_StatusCode_NONE;

   /// If the message was directed at a specific instance, get the Instance.
   p_SUPL_Instance = SUPL_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );

   /// Otherwise use the handle to get the Instance.
   if ( p_SUPL_Instance == NULL )
   {
      p_SUPL_Instance = SUPL_Instance_From_Handle( p_ThisMessage->MsgHdr.Handle );
   }

   if( NULL == p_SUPL_Instance )
   {
       GN_SUPL_Log("Invalid Handle value :  %p" , p_ThisMessage->MsgHdr.Handle );
       GN_SUPL_Log_PduIgnored( p_ThisMessage->MsgHdr.Handle );
       asn1_PDU_Buffer_Store_Free( &p_ThisMessage->Payload.p_PDU );
       return;
   }

   Next_State = p_SUPL_Instance->p_SUPL->State;

   /// Clear all user timers as we are handling a PDU so none of the User Timers are valid.
   GN_SUPL_Timer_Clear_UT1( p_SUPL_Instance );
   GN_SUPL_Timer_Clear_UT2( p_SUPL_Instance );
   //GN_SUPL_Timer_Clear_UT4( p_SUPL_Instance );

#if 1
    GN_SUPL_Log("SUPL_Handle_PDU_Received: Calling SUPL_Buffer_Segmented_PDU()");
    p_PDU_Buf_Incoming = SUPL_Buffer_Segmented_PDU(p_ThisMessage->Payload.p_PDU);
    if(p_PDU_Buf_Incoming == NULL)
    {
        GN_SUPL_Log("SUPL_Handle_PDU_Received: PDU is not complete, buffered the data");
        /*No need to free the data. Check supl_Buffer_Segmented_PDU*/
        return;
    }
    GN_SUPL_Log("SUPL_Handle_PDU_Received: PDU is Complete, sending it for processing");

#else

   p_PDU_Buf_Incoming = p_ThisMessage->Payload.p_PDU;
#endif

   p_ThisMessage->Payload.p_PDU = NULL;

   /// Decode the pdu.
   if ( supl_PDU_Decode( p_SUPL_Instance->Handle , p_PDU_Buf_Incoming, &p_PDU_Decoded, &GN_StatusCode ) )
   {
      /// Get the message type from the decoded PDU and call individual handlers based on the message type.
      /// If the message is not valid for the client save the status as GN_StatusCode_unexpectedMessage ready to send back a SUPL-END.
      This_Message_Type = supl_PDU_Get_MessageType( p_PDU_Decoded );
      switch ( This_Message_Type )
      {
      case GN_UlpMessage_PR_msSUPLINIT:
         SUPL_Handle_PDU_SUPL_INIT( p_PDU_Decoded, p_SUPL_Instance, p_PDU_Buf_Incoming );
         break;
      case GN_UlpMessage_PR_msSUPLSTART:
         GN_StatusCode = GN_StatusCode_unexpectedMessage;
         GN_SUPL_Log_MsgOutOfOrder( p_SUPL_Instance->Handle , p_PDU_Decoded);
         break;
      case GN_UlpMessage_PR_msSUPLRESPONSE:
         SUPL_Handle_PDU_SUPL_RESPONSE( p_PDU_Decoded, p_SUPL_Instance );
         break;
      case GN_UlpMessage_PR_msSUPLPOSINIT:
         GN_StatusCode = GN_StatusCode_unexpectedMessage;
         GN_SUPL_Log_MsgOutOfOrder( p_SUPL_Instance->Handle , p_PDU_Decoded );
         break;
      case GN_UlpMessage_PR_msSUPLPOS:
         GN_SUPL_Timer_Clear_UT3( p_SUPL_Instance );
         SUPL_Handle_PDU_SUPL_POS( p_PDU_Decoded, p_SUPL_Instance, p_ThisMessage->Payload.PDU_TimeStamp );
         break;
      case GN_UlpMessage_PR_msSUPLEND:
         GN_SUPL_Timer_Clear_UT3( p_SUPL_Instance );
         GN_SUPL_V2_Timer_Clear_UT5(p_SUPL_Instance);
         GN_SUPL_V2_Timer_Clear_UT7(p_SUPL_Instance);
         GN_SUPL_V2_Timer_Clear_UT8(p_SUPL_Instance);
         GN_SUPL_V2_Timer_Clear_UT9(p_SUPL_Instance);
         SUPL_Handle_PDU_SUPL_END( p_PDU_Decoded, p_SUPL_Instance );
         break;
      case GN_UlpMessage_PR_msSUPLAUTHREQ:
         GN_StatusCode = GN_StatusCode_unexpectedMessage;
         GN_SUPL_Log_MsgOutOfOrder( p_SUPL_Instance->Handle , p_PDU_Decoded );
         break;
      case GN_UlpMessage_PR_msSUPLAUTHRESP:
         SUPL_Handle_PDU_SUPL_AUTH_RESP( p_SUPL_Instance );
         break;

/* +RRR : 26-08-2010 : SUPLv2 */

      case GN_UlpMessage_PR_msSUPLNOTIFY:
        GN_SUPL_Timer_Clear_UT3( p_SUPL_Instance );
        SUPL_Handle_PDU_SUPL_NOTIFY(p_PDU_Decoded,p_SUPL_Instance );
        break;
      case GN_UlpMessage_PR_msSUPLNOTIFYRESPONSE:
        GN_StatusCode = GN_StatusCode_unexpectedMessage;
        GN_SUPL_Log_MsgOutOfOrder( p_SUPL_Instance->Handle , p_PDU_Decoded );
        break;
      case GN_UlpMessage_PR_msSUPLREPORT:
        GN_SUPL_Timer_Clear_UT3( p_SUPL_Instance );
        SUPL_Handle_PDU_SUPL_REPORT(p_PDU_Decoded,p_SUPL_Instance);
        break;
      case GN_UlpMessage_PR_msSUPLTRIGGEREDSTART:
        GN_StatusCode = GN_StatusCode_unexpectedMessage;
        GN_SUPL_Log_MsgOutOfOrder( p_SUPL_Instance->Handle , p_PDU_Decoded );
        break;
      case GN_UlpMessage_PR_msSUPLTRIGGEREDRESPONSE:
        SUPL_Handle_PDU_SUPL_TRIGGERED_RESPONSE(p_PDU_Decoded,p_SUPL_Instance );
        break;
      case GN_UlpMessage_PR_msSUPLTRIGGEREDSTOP:
        GN_SUPL_Timer_Clear_UT3( p_SUPL_Instance );
        GN_SUPL_V2_Timer_Clear_UT7(p_SUPL_Instance);
        GN_SUPL_V2_Timer_Clear_UT8(p_SUPL_Instance);
        SUPL_Handle_PDU_SUPL_TRIGGERED_STOP(p_PDU_Decoded,p_SUPL_Instance );
        break;
      case GN_UlpMessage_PR_msSUPLSETINIT:
        GN_StatusCode = GN_StatusCode_unexpectedMessage;
        GN_SUPL_Log_MsgOutOfOrder( p_SUPL_Instance->Handle , p_PDU_Decoded );
        break;

/* -RRR : 26-08-2010 : SUPLv2 */


      // System error scenarios.
      case GN_UlpMessage_PR_NOTHING:
      default:
         GN_StatusCode = GN_StatusCode_protocolError;
         GN_SUPL_Log_MsgDecodeFailed( p_SUPL_Instance->Handle );
         break;
      }
   }
   else
   {
      /* GN_StatusCode = GN_StatusCode_unexpectedDataValue; */
      GN_StatusCode = GN_StatusCode_protocolError;
      GN_SUPL_Log_MsgDecodeFailed( p_SUPL_Instance->Handle );
   }

   /// If there was an error we send a SUPL-END to the server.
   if ( GN_StatusCode != GN_StatusCode_NONE )
   {
      /// If there is not server connection one needs to be established
      /// before we can send a SUPL-END (this is if the error occured
      /// when processing a SUPL-INIT i.e. a Push).
      if ( ! p_SUPL_Instance->p_SUPL->ServerConnectionAvailable )
      {
         e_GN_Status       GN_Status                  = GN_SUCCESS;
         BL                Status;
         s_SUPL_IpAddress  *p_CFG_IpAddress;
         BL                PDU_SlpSessionID_Present;

         PDU_SlpSessionID_Present = supl_PDU_Get_slpSessionID( p_PDU_Decoded,
                                                               &p_SUPL_Instance->p_SUPL->p_SlpSessionId,
                                                               &p_SUPL_Instance->p_SUPL->SlpId );

         p_SUPL_Instance->p_SUPL->ErrorStatus = GN_StatusCode;
         p_CFG_IpAddress = supl_config_get_SUPL_SLP_address();

         p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.TcpIp_AddressType = p_CFG_IpAddress->TcpIp_AddressType;
         p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.p_TcpIp_Address   = p_CFG_IpAddress->Address;
         p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.Port              = p_CFG_IpAddress->Port;

         p_SUPL_Instance->p_SUPL->Notification_Accepted = TRUE;

         Status = GN_SUPL_Connect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                   &GN_Status,
                                                   &p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms );

         Next_State = state_SUPL_Error;
         SUPL_Instance_State_Transition( p_SUPL_Instance,
                                         Next_State,
                                         "event_SUPL_PDU_Received(Decode Error)" );
      }
      else
      {
         e_GN_Status          GN_Status;
         s_PDU_Buffer_Store   *p_PDU_Buf;

         asn1_PDU_Buffer_Store_Free( &p_PDU_Buf_Incoming );
         p_PDU_Buf = asn1_PDU_Buffer_Store_Alloc( 0 );
         p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
         supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, GN_StatusCode );

         if ( supl_PDU_Encode( p_SUPL_Instance->Handle , p_PDU_Src, p_PDU_Buf ) )
         {
            BL Status;
            GN_Status = GN_SUCCESS;

            GN_SUPL_PDU_Delivery_Out_Wrapper( p_SUPL_Instance->Handle,
                                              &GN_Status,
                                              p_PDU_Buf->PDU_Encoded.Length,
                                              p_PDU_Buf->PDU_Encoded.p_PDU_Data );

            GN_Status = GN_SUCCESS;
            Status = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                         &GN_Status);

            // Transitions straight to Idle to save waiting for a close confirmation.
            supl_PDU_Free( p_PDU_Src );
            Next_State = state_SUPL_Idle;
            SUPL_Instance_State_Transition( p_SUPL_Instance,
                                            Next_State,
                                            "event_SUPL_PDU_Received" );
         }

         asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
      }
   }

   asn1_PDU_Buffer_Store_Free( &p_PDU_Buf_Incoming );
   supl_PDU_Free( p_PDU_Decoded );

   return;
}


//*****************************************************************************
/// \brief
///      Check each instance of the SUPL Client for any required
///      processing.
//*****************************************************************************
void SUPL_Instance_Process
(
   s_SUPL_Instance *p_SUPL_Instance    ///< pointer to an active instance.
)
{
   s_SUPL_Instance *p_SUPL_TempInstance = SUPL_Instance_Get_Data( p_SUPL_Instance );

   if ( p_SUPL_TempInstance == NULL )
   {
      // Special case: Check for assistance data request from GPS_Core
      /// \todo Query GPS_core for assistance data required. In the meantime always say yes.
   }
   else
   {
      s_SUPL_InstanceData  *p_IData = p_SUPL_TempInstance->p_SUPL;

      switch ( p_IData->State )
      {
      case state_SUPL_Idle:
         /// An instance is put in the Idle state after all processing is
         /// complete. It can therefore be deleted.
         SUPL_Instance_Delete( p_SUPL_TempInstance );
         break;
      case state_SUPLv2_SUPL_SET_INIT_Sent:
         GN_SUPL_V2_Timer_Expired_UT9( p_SUPL_TempInstance );
         break;
      case state_SUPLv2_SUPL_TRIGGER_Suspended:
         GN_SUPL_V2_Timer_Expired_UT7( p_SUPL_TempInstance );
         break;
      case state_SUPLv2_SUPL_TRIGGER_Active:
         GN_SUPL_V2_Timer_Expired_UT8( p_SUPL_TempInstance );
         break;
      case state_SUPLv2_Location_Notification:
         GN_SUPL_V2_Timer_Expired_UT5( p_SUPL_TempInstance );
         break;      
      case state_SUPL_Comms_Open_Sent:
      case state_SUPL_SUPL_START_Sent:
      case state_SUPL_SUPL_RESPONSE_Sent:
      case state_SUPL_SUPL_POS_INIT_Sent:
      case state_SUPL_Waiting_For_RRLP:
      case state_SUPL_POS_Payload_Delivered:
      case state_SUPL_SUPL_POS_Sent:
      case state_SUPL_SUPL_END_Sent:
      case state_SUPL_Server_Open_Requested:
      case state_SUPL_Comms_Close_Sent:
      case state_SUPL_Awaiting_Confirmation:
/* +LMSqb89433 */
      case state_SUPL_Awaiting_Confirmation_Or_Comms_Open_Sent:
/* -LMSqb89433 */
      default:
         // Check for timer expiry at this point.
         GN_SUPL_Timer_Expired_UT1( p_SUPL_TempInstance );
         GN_SUPL_Timer_Expired_UT2( p_SUPL_TempInstance );
         GN_SUPL_Timer_Expired_UT3( p_SUPL_TempInstance );

         break;
      }
   }
}


//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Initialisation message.
/// \details
///      This function is intended to kick off any activity required when the
///      SUPL subsystem is initialised, such as setting up an instance for
///      polling the GPS subsystem for assistance data queries.
///
///      This is a placeholder and currently performs no function.
//*****************************************************************************
void SUPL_Handle_Initialisation( s_SUPL_Message* p_ThisMessage )
{
   // Currently nothing needs to be done.
   p_ThisMessage = p_ThisMessage;
   return;
}


//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Notification_Request message.
///
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_Notification_Request this function requests confirmation
///      from the user based on the notification parameters.
//*****************************************************************************
void SUPL_Handle_Notification_Request( s_SUPL_Message* p_ThisMessage )
{
   e_GN_Status       GN_Status         = GN_SUCCESS;
   BL                Status;
   void              *Handle           = p_ThisMessage->MsgHdr.Handle;
   s_SUPL_Instance   *p_SUPL_Instance;

   // If the message was directed at a specific instance, get the Instance.
   p_SUPL_Instance = SUPL_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );

   if ( p_SUPL_Instance == NULL )
   {
      // Look up the Handle.
      p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
   }

   if ( p_SUPL_Instance != NULL  &&  p_SUPL_Instance->p_SUPL != NULL )
   {
/* +LMSqb89433 */
/*      SUPL_Instance_State_Transition( p_SUPL_Instance,
                                      state_SUPL_Awaiting_Confirmation,
                                      "event_SUPL_Notification_Request" );
*/
/* -LMSqb89433 */

      Status = GN_SUPL_Notification_Req_Out_Wrapper( Handle,
                                                     &GN_Status,
                                                     &p_SUPL_Instance->p_SUPL->SUPL_Notification );
   }

   return;
}


//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Notification_Response message.
///
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_Notification_Response this function requests a connection to
///      the SUPL server.
//*****************************************************************************
void SUPL_Handle_Notification_Response( s_SUPL_Message* p_ThisMessage )
{
   void              *Handle           = p_ThisMessage->MsgHdr.Handle;
   s_SUPL_Instance   *p_SUPL_Instance;
   e_SUPL_States     Next_State;
   void             *p_PDU_Src;
   BL                PDU_Ready_To_Send    = FALSE;

   // If the message was directed at a specific instance, get the Instance.
   p_SUPL_Instance = SUPL_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );

   if ( p_SUPL_Instance == NULL )
   {
      // Look up the Handle.
      p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
   }

   if ( p_SUPL_Instance != NULL  &&  p_SUPL_Instance->p_SUPL != NULL )
   {
      p_SUPL_Instance->p_SUPL->Notification_Accepted = p_ThisMessage->Payload.Notification_Accepted;

      Next_State = p_SUPL_Instance->p_SUPL->State;

/* +LMSqb89433 */
      switch ( p_SUPL_Instance->p_SUPL->State )
      {
        case state_SUPL_Awaiting_Confirmation_Or_Comms_Open_Sent:
            /* Now that we have received confirmation, we will wait for connection to be setup */
            Next_State = state_SUPL_Comms_Open_Sent;
            break;
        case state_SUPL_Awaiting_Confirmation:
            /* This state means that we already have a connection and can immediately send the PDU */
            Next_State = SUPL_Handle_SUPL_INIT_Comms( p_SUPL_Instance , &p_PDU_Src );
            PDU_Ready_To_Send = TRUE;
            break;
        case state_SUPLv2_Location_Notification:
            Next_State = SUPL_Handle_SUPL_NOTIFY_RESPONSE( p_SUPL_Instance , &p_PDU_Src );
            PDU_Ready_To_Send = (p_PDU_Src != NULL);

            break;
        default:
            OS_ASSERT( 0 );
      }
/* -LMSqb89433 */
      /// \todo Add check for proxy/non-proxy.
/* +LMSqb89433 */
      if( PDU_Ready_To_Send )
      {
          e_GN_Status    GN_Status = GN_SUCCESS;
          BL             Successful;
          BL             Status;

          Status = GN_SUPL_PDU_Send( p_SUPL_Instance, p_PDU_Src );
          supl_PDU_Free( p_PDU_Src );
          if ( ! Status )
          {
             Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                              &GN_Status );
             // Transitions straight to Idle to save waiting for a close confirmation.
             Next_State = state_SUPL_Idle ;
          }
      }
/* -LMSqb89433 */
       if ( Next_State == state_SUPL_Comms_Close_Sent )
       {
          e_GN_Status    GN_Status = GN_SUCCESS;
          BL             Successful;

          Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                           &GN_Status );
          // Transitions straight to Idle to save waiting for a close confirmation.
          Next_State = state_SUPL_Idle ;
       }
      SUPL_Instance_State_Transition( p_SUPL_Instance,
                                      Next_State,
                                      "event_SUPL_Notification_Response" );

/* +LMSqb89433 */
/*      Status = GN_SUPL_Connect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                &GN_Status,
                                                &p_SUPL_Instance->p_SUPL->SUPL_Connect_Req_Parms ); */
/* -LMSqb89433 */
   }

   return;
}


//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_MO_Position_Requested message.
///
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_MO_Position_Requested this function requests a connection to
///      the SUPL server in order to start a SET Initiated SUPL exchange.
//*****************************************************************************
void SUPL_Handle_MO_Position_Requested( s_SUPL_Message* p_ThisMessage )
{
   e_GN_Status       GN_Status         = GN_SUCCESS;
   BL                Status;
   void              *Handle           = p_ThisMessage->MsgHdr.Handle;
   s_SUPL_Instance   *p_SUPL_Instance;
   e_SUPL_States     Next_State        = state_SUPL_Idle;

   // If the message was directed at a specific instance, get the Instance.
   p_SUPL_Instance = SUPL_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );

   if ( p_SUPL_Instance == NULL )
   {
      // Look up the Handle.
      p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
   }


   if ( p_SUPL_Instance != NULL  &&  p_SUPL_Instance->p_SUPL != NULL )
   {
      BL Supl_Mo_Allowed = TRUE;

      // Check if there is any ongoing Emergency SUPL session in case of which SUPL MO request is ignored.
      if(supl_Is_Version2( p_SUPL_Instance ) ) 
      {
          U1 Index = 0;

          for ( Index = 0 ; Index < SUPL_Instances.MaxInstanceCount ; Index++ )
          {
              // Check if there is any other ongoing SUPL session. 
              // If the ongoing session is not a Emergency request & it is not in idle state
              if (    SUPL_Instances.InstanceList[Index] != NULL 
                   && SUPL_Instances.InstanceList[Index]->p_SUPL->V2_Data.Emergency_CallReq == TRUE )
              {
                  Supl_Mo_Allowed = FALSE;
                  break;
              }
          }
      }

      if( Supl_Mo_Allowed )
      {
         #ifdef SUPL_MODULE_LOGGING
         GN_SUPL_Log( "SUPL_Handle_MO_Position_Requested: Handle = %p, instance = %p, data container = %p", Handle, p_SUPL_Instance, p_SUPL_Instance->p_SUPL );

         if ( p_ThisMessage->Payload.p_GN_SUPL_Connect_Req_Parms->TcpIp_AddressType != IP_None )
         {
            GN_SUPL_Log( "SUPL_Handle_MO_Position_Requested: Rcvd SLP IPAddress == %s, Port == %u",
                          p_ThisMessage->Payload.p_GN_SUPL_Connect_Req_Parms->p_TcpIp_Address,
                          p_ThisMessage->Payload.p_GN_SUPL_Connect_Req_Parms->Port );
         }
         #endif

         memcpy( &p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms,
                 p_ThisMessage->Payload.p_GN_SUPL_Connect_Req_Parms,
                 sizeof( s_GN_SUPL_Connect_Req_Parms ) );

         #ifdef SUPL_MODULE_LOGGING
         if ( p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.TcpIp_AddressType != IP_None )
         {
            GN_SUPL_Log( "SUPL_Handle_MO_Position_Requested: Used SLP IPAddress == %s, Port == %u",
                           p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.p_TcpIp_Address,
                           p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.Port );
         }
         #endif

         /// \todo Add check for proxy/non-proxy.
         Status = GN_SUPL_Connect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                   &GN_Status,
                                                   &p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms );

         Next_State = state_SUPL_Comms_Open_Sent;
      }
      else
      {
         Next_State = state_SUPL_Idle;
      }
   }

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_MO_Position_Requested" );

   return;
}

/* +LMSqc38060 */
//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Abort_Requested message.
///
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_Abort_Requested this function requests close of SUPL session.
//*****************************************************************************
void SUPL_Handle_Abort_Requested( s_SUPL_Message* p_ThisMessage )
{
   e_GN_Status       GN_Status         = GN_SUCCESS;
   void              *Handle           = p_ThisMessage->MsgHdr.Handle;
   s_SUPL_Instance   *p_SUPL_Instance;

   // If the message was directed at a specific instance, get the Instance.
   p_SUPL_Instance = SUPL_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );

   if ( p_SUPL_Instance == NULL )
   {
      GN_SUPL_Log( "SUPL_Handle_Abort_Requested: Instance is NULL ");
      // Look up the Handle.
      p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
   }

   if ( p_SUPL_Instance != NULL  &&  p_SUPL_Instance->p_SUPL != NULL )
   {

       void                 *p_PDU_Src        = NULL;
       BL                   PDU_Ready_To_Send = FALSE;
       e_SUPL_States        Next_State        = p_SUPL_Instance->p_SUPL->State;
       BL                   Successful;

#ifdef SUPL_MODULE_LOGGING
       GN_SUPL_Log( "SUPL_Handle_Abort_Requested: Handle = %p, instance = %p, data container = %p", Handle, p_SUPL_Instance, p_SUPL_Instance->p_SUPL );
#endif

       switch( p_SUPL_Instance->p_SUPL->State )
       {
           //-----------------------------------------------------------------------
           // Valid states.
           case state_SUPL_Idle:
           case state_SUPL_Comms_Close_Sent:
             GN_SUPL_Log( "SUPL_Handle_Abort_Requested: END ");
             return;

           case state_SUPL_Comms_Open_Sent:
           case state_SUPL_SUPL_END_Sent:  //in what case SUPL END is sent without being sending disconnectind
             Next_State = state_SUPL_Idle ;
             break;

           case state_SUPL_SUPL_START_Sent:
           case state_SUPL_SUPL_RESPONSE_Sent:
           case state_SUPL_SUPL_POS_INIT_Sent:
           case state_SUPL_Waiting_For_RRLP:
           case state_SUPL_POS_Payload_Delivered:
           case state_SUPL_SUPL_POS_Sent:
           case state_SUPLv2_SUPL_TRIGGER_START_Sent:
           case state_SUPLv2_SUPL_TRIGGER_Active:
           case state_SUPLv2_SUPL_TRIGGER_Suspended:
              if( supl_Is_Version2( p_SUPL_Instance ) && ( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode != GN_TriggeredModeNone ) )
              {
                  /*Send Triggered Stop and start timer UT7*/
                  GN_SUPL_V2_Timer_Set_UT7(p_SUPL_Instance);
                  p_PDU_Src = supl_PDU_Make_SUPLTRIGGEREDSTOP(p_SUPL_Instance, GN_StatusCode_unspecified);
                  PDU_Ready_To_Send  =  TRUE;
                  Next_State         =  state_SUPLv2_SUPL_TRIGGER_Suspended;
              }
              else
              {
                 p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
                 // Allowed stati for timeouts are:
                 // GN_StatusCode_unspecified
                 // GN_StatusCode_systemFailure
                 // GN_StatusCode_posMethodFailure
                 supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,
                                               GN_StatusCode_unspecified );
                 PDU_Ready_To_Send  =  TRUE;
                 Next_State         =  state_SUPL_Comms_Close_Sent;
              }
              break;
           //-----------------------------------------------------------------------
           default:
              // Coding error.
              OS_ASSERT( 0 );
              break;
           //-----------------------------------------------------------------------
       }

       if( PDU_Ready_To_Send )
       {
          BL Status;
          GN_SUPL_Log( "SUPL_Handle_Abort_Requested: Send SUPL END ");
          Status = GN_SUPL_PDU_Send( p_SUPL_Instance, p_PDU_Src );
          supl_PDU_Free( p_PDU_Src );
          if ( ! Status )
          {
             // Transitions straight to Idle to save waiting for a close confirmation.
             Next_State = state_SUPL_Idle ;
          }

       }

       if(Next_State !=  state_SUPLv2_SUPL_TRIGGER_Suspended )
       {
           GN_SUPL_Log( "SUPL_Handle_Abort_Requested: TCPIP Disconnect requested ");
           Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,&GN_Status );
       }

       SUPL_Instance_State_Transition( p_SUPL_Instance,
                                       Next_State,
                                       "event_SUPL_Abort_Requested" );


   }

   return;
}
/* -LMSqc38060 */


//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Trigger_End_Requested message.
///
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_Trigger_End_Requested this function requests close of SUPL session.
//*****************************************************************************
void SUPL_Handle_Trigger_End_Requested( s_SUPL_Message* p_ThisMessage )
{
   e_GN_Status       GN_Status         = GN_SUCCESS;
   void              *Handle           = p_ThisMessage->MsgHdr.Handle;
   s_SUPL_Instance   *p_SUPL_Instance;

   // If the message was directed at a specific instance, get the Instance.
   p_SUPL_Instance = SUPL_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );

   if ( p_SUPL_Instance == NULL )
   {
      GN_SUPL_Log( "SUPL_Handle_Trigger_End_Requested: Instance is NULL ");
      // Look up the Handle.
      p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
   }

   if ( p_SUPL_Instance != NULL  &&  p_SUPL_Instance->p_SUPL != NULL )
   {

       void                 *p_PDU_Src        = NULL;
       BL                   PDU_Ready_To_Send = FALSE;
       e_SUPL_States        Next_State        = p_SUPL_Instance->p_SUPL->State;
       BL                   Successful;

#ifdef SUPL_MODULE_LOGGING
       GN_SUPL_Log( "SUPL_Handle_Trigger_End_Requested: Handle = %p, instance = %p, data container = %p", Handle, p_SUPL_Instance, p_SUPL_Instance->p_SUPL );
#endif

       switch( p_SUPL_Instance->p_SUPL->State )
       {
           //-----------------------------------------------------------------------
           // Valid states.
           case state_SUPL_Idle:
           case state_SUPL_Comms_Close_Sent:
             GN_SUPL_Log( "SUPL_Handle_Trigger_End_Requested: END ");
             return;

           case state_SUPL_Comms_Open_Sent:
           case state_SUPL_SUPL_END_Sent:  //in what case SUPL END is sent without being sending disconnectind
             Next_State = state_SUPL_Idle ;
             break;

           case state_SUPL_SUPL_START_Sent:
           case state_SUPL_SUPL_RESPONSE_Sent:
           case state_SUPL_SUPL_POS_INIT_Sent:
           case state_SUPL_Waiting_For_RRLP:
           case state_SUPL_POS_Payload_Delivered:
           case state_SUPL_SUPL_POS_Sent:
           case state_SUPLv2_SUPL_TRIGGER_START_Sent:
           case state_SUPLv2_SUPL_TRIGGER_Active:
           case state_SUPLv2_SUPL_TRIGGER_Suspended:
              if( supl_Is_Version2( p_SUPL_Instance ) && ( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode != GN_TriggeredModeNone ) ) 
              {
                  if(  p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModeAreaEvent )
                  {
                     p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
                     supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, GN_StatusCode_ver2_sessionStopped );
                     PDU_Ready_To_Send  =  TRUE;
                     Next_State         =  state_SUPL_Comms_Close_Sent;
                  }
              }
              break;
           //-----------------------------------------------------------------------
           default:
              // Coding error.
              OS_ASSERT( 0 );
              break;
           //-----------------------------------------------------------------------
       }

       if( PDU_Ready_To_Send )
       {
          BL Status;
          GN_SUPL_Log( "SUPL_Handle_Trigger_End_Requested: Send SUPL END ");
          Status = GN_SUPL_PDU_Send( p_SUPL_Instance, p_PDU_Src );
          supl_PDU_Free( p_PDU_Src );
          if ( ! Status )
          {
             // Transitions straight to Idle to save waiting for a close confirmation.
             Next_State = state_SUPL_Idle ;
          }

          if(Next_State ==  state_SUPL_Comms_Close_Sent )
          {
             GN_SUPL_Log( "SUPL_Handle_Trigger_End_Requested: TCPIP Disconnect requested ");
             Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                                               &GN_Status );
          }

       }

       SUPL_Instance_State_Transition( p_SUPL_Instance,
                                       Next_State,
                                       "event_SUPL_Trigger_End_Requested" );


   }

   return;
}


//*****************************************************************************
/// \brief
///      Handles the SUPL_INIT PDU in a #event_SUPL_PDU_Received message.
///
/// \details
///      PDU specific processing invoked by #SUPL_Handle_PDU_Received() is
///      performed in this function specifically for the SUPL-INIT PDU.
//*****************************************************************************
static void SUPL_Handle_PDU_SUPL_INIT
(
   void                 *p_PDU_Decoded,         ///< Pointer to a decoded pdu.
   s_SUPL_Instance      *p_SUPL_Instance,       ///< The SUPL instance dealing with this PDU.
   s_PDU_Buffer_Store   *p_PDU_Buf_Incoming     ///< The RAW SUPL-INIT PDU.
)
{
   e_GN_Status       GN_Status                  = GN_SUCCESS;
   BL                Status;
   e_SUPL_States     Next_State                 = p_SUPL_Instance->p_SUPL->State;
   BL                PDU_SlpSessionID_Present;
   BL                PDU_SetSessionID_Present;
   e_GN_SLPMode      GN_SLPMode;

   s_SUPL_IpAddress *p_CFG_IpAddress;

   U1                Major,   Minor,   Service_Indicator, MinMajVer;
   U1                Config_Major, Config_Minor, Config_Service_Indicator;

   switch ( p_SUPL_Instance->p_SUPL->State )
   {
   //--------------------------------------------------------------------------
   // Valid states.
   case state_SUPL_Push_Received:
      /// \todo Add check for proxy/non-proxy.
      p_SUPL_Instance->p_SUPL->NetworkInitiated = TRUE; // Set as NI session
      p_SUPL_Instance->p_SUPL->SUPL_INIT.p_PDU_Data = GN_Calloc( 1, p_PDU_Buf_Incoming->PDU_Encoded.Length );
      p_SUPL_Instance->p_SUPL->SUPL_INIT.Length     = p_PDU_Buf_Incoming->PDU_Encoded.Length;
      memcpy( p_SUPL_Instance->p_SUPL->SUPL_INIT.p_PDU_Data,
              p_PDU_Buf_Incoming->PDU_Encoded.p_PDU_Data,
              p_SUPL_Instance->p_SUPL->SUPL_INIT.Length );

      /// Retrieve various items from the PDU for use in subsequent PDUs and to check
      /// for validity of the PDU.
      // Retrieve and store SLP session ID from PDU.
      PDU_SlpSessionID_Present = supl_PDU_Get_slpSessionID( p_PDU_Decoded,
                                                            &p_SUPL_Instance->p_SUPL->p_SlpSessionId,
                                                            &p_SUPL_Instance->p_SUPL->SlpId );

      p_CFG_IpAddress = supl_config_get_SUPL_SLP_address();

      p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.TcpIp_AddressType = p_CFG_IpAddress->TcpIp_AddressType;
      p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.p_TcpIp_Address   = p_CFG_IpAddress->Address;
      p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.Port              = p_CFG_IpAddress->Port;

      // Retrieve SUPL version from PDU.
      supl_PDU_Get_Version( p_PDU_Decoded, &Major, &Minor, &Service_Indicator );

      supl_config_get_SUPL_version( &Config_Major, &Config_Minor, &Config_Service_Indicator );

      if( Major >= 2 ) // Ex SLP version 
      {
         // Ex (SLP version = 3, SET version = 2 or 1)/( Ex SLP version = 2, SET version =  1)
         if ( Config_Major < Major )
         {
            // Retrieve SUPL Minimum Major version from SUPL INIT PDU.
           if(   supl_PDU_Get_SUPLINIT_MinMajorVer( p_PDU_Decoded, &MinMajVer )  )
           {
              // Ex SLP version = 3, SLP Min Maj version =2, SET version = 1
              if( Config_Major < MinMajVer )
                 p_SUPL_Instance->p_SUPL->ErrorStatus = GN_StatusCode_versionNotSupported;
              // Ex SLP version = 3, (SLP Min Maj version =2, SET version = 2)/(SLP Min Maj version =1, SET version = 1)
              else
                 p_SUPL_Instance->p_SUPL->TransactionSuplVersion = MinMajVer ;
           }
           else
           {
              p_SUPL_Instance->p_SUPL->ErrorStatus = GN_StatusCode_versionNotSupported;
           }
         }
         else
         {
            // Ex SET version = 2, SLP version = 1
            p_SUPL_Instance->p_SUPL->TransactionSuplVersion = Major ;
         }
      }
      // SLP version = 1
      else
      {
         p_SUPL_Instance->p_SUPL->TransactionSuplVersion = Major ;
      }

      // Retrieve and store SET session ID from PDU.
      PDU_SetSessionID_Present = supl_PDU_Get_setSessionID( p_PDU_Decoded,
                                                            &p_SUPL_Instance->p_SUPL->SetSessionId,
                                                            &p_SUPL_Instance->p_SUPL->SetId );

      if ( PDU_SetSessionID_Present || ! PDU_SlpSessionID_Present )
      {
         p_SUPL_Instance->p_SUPL->ErrorStatus = GN_StatusCode_invalidSessionId;
      }

      // Retrieve and store SLP mode (proxy/non-proxy from PDU.
      supl_PDU_Get_SUPLINIT_SLPMode( p_PDU_Decoded, &GN_SLPMode );

      /// Check that we are requested to work in Proxy mode.
      if ( GN_SLPMode == GN_nonProxy )
      {
         p_SUPL_Instance->p_SUPL->ErrorStatus = GN_StatusCode_nonProxyModeNotSupported;
      }

      if(supl_Is_Version2( p_SUPL_Instance ))
      { 
         //Retrieve Supported Network Information from SUPL INIT PDU.
         supl_PDU_Get_SUPLINIT_Supported_Network_Info( p_PDU_Decoded, &p_SUPL_Instance->p_SUPL->V2_Data.p_SupportedNetworkInfo);

         // Check for Emergency location request
         supl_PDU_Get_SUPLINIT_EmergencyRequest(p_PDU_Decoded,&p_SUPL_Instance->p_SUPL->V2_Data.Emergency_CallReq);

         if( p_SUPL_Instance->p_SUPL->V2_Data.Emergency_CallReq == TRUE )
         {
            p_CFG_IpAddress = supl_config_get_SUPL_ESLP_address();

            p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.TcpIp_AddressType = p_CFG_IpAddress->TcpIp_AddressType;
            p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.p_TcpIp_Address   = p_CFG_IpAddress->Address;
            p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.Port              = p_CFG_IpAddress->Port;
         }

         //Check for E-SLP Address and store the address
         if(supl_PDU_Get_SUPLINIT_ESlp_Address(p_PDU_Decoded,&p_SUPL_Instance->p_SUPL->V2_Data))
         {
            if(supl_Verify_Ecall_WhiteList(&p_SUPL_Instance->p_SUPL->V2_Data))
            {
               if(!p_SUPL_Instance->p_SUPL->V2_Data.Emergency_CallReq)
                  GN_SUPL_Log( "E-SLP Address present but no Emergency indication ");
 
               /* Modify the supl server address with E-slp*/
               p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.TcpIp_AddressType = FQDN_Address;
               p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms.p_TcpIp_Address   = p_SUPL_Instance->p_SUPL->V2_Data.Eslp_FQDN;
            }
            else
            {
               p_SUPL_Instance->p_SUPL->V2_Data.Emergency_CallReq    = FALSE;
               p_SUPL_Instance->p_SUPL->V2_Data.Eslp_Address_Present = FALSE;
                  /* TODO How to ignore this supl init instance*/
            }
         }
         /* Based on SUPL version,if its SUPL 2.0, populate the VER container field to hash generated by SHA256 (SUPL 2.0) */
         {
            int error_Sha  ;
            char server[256] ;
            U2 PDU_Size = p_PDU_Buf_Incoming->PDU_Encoded.Length ;
            U1* p_PDU = p_PDU_Buf_Incoming->PDU_Encoded.p_PDU_Data ;

            strcpy(server,p_CFG_IpAddress->Address) ;

            if ( p_SUPL_Instance->p_SUPL->p_VER != NULL )
            GN_Free( p_SUPL_Instance->p_SUPL->p_VER );

            p_SUPL_Instance->p_SUPL->p_VER = GN_Calloc( 1, SIZE_SHA256_HASH_IN_BYTES );

            error_Sha = GN_SUPL_GetHmac_sha256( (U1 *)server,
                                                strlen ( server),
                                                p_PDU ,
                                                PDU_Size ,
                                                pHashedKeysha256
                                              );

            GN_SUPL_Log ( "Hashing with SHA256 returned server address %s", server );

            memcpy (p_SUPL_Instance->p_SUPL->p_VER, pHashedKeysha256, SIZE_SHA256_HASH_IN_BYTES );

 #ifdef SUPL_MODULE_LOGGING
            {
               CH VER_String_sha[17];
               U1 i;

               for ( i = 0 ; i < 8 && i < SIZE_SHA256_HASH_IN_BYTES ; i++ ) // Ver ::= BIT STRING(SIZE (64)) i.e. 8 octets
               {
                  sprintf( &VER_String_sha[i*2], "%02x", pHashedKeysha256[i] );
               }

               GN_SUPL_Log( "SUPL_Handle_PDU_SUPL_INIT: VER_Size == %d, VER_SHA256 = 0x\"%s\"", SIZE_SHA256_HASH_IN_BYTES, VER_String_sha);
            }
 #endif
            GN_SUPL_Log( "SUPL_Handle_PDU_SUPL_INIT:SUPL2.0 Hash Ver generated internally");
         }
      }

      if ( p_SUPL_Instance->p_SUPL->ErrorStatus == GN_StatusCode_NONE )
      {
         BL SUPL_Init_Process_Reqd = TRUE;

         // Retrieve and store posMethod from PDU.
         supl_PDU_Get_SUPLINIT_PosMethod( p_PDU_Decoded,
                                          &p_SUPL_Instance->p_SUPL->GN_PosMethod_Requested );

         // Retrieve and store QoP from PDU.
         if( supl_PDU_Get_SUPLINIT_QoP( p_PDU_Decoded,
                                       &p_SUPL_Instance->p_SUPL->p_GN_QoP ) )
         {
             GN_SUPL_Log_QoP( p_SUPL_Instance->Handle , p_SUPL_Instance->p_SUPL->p_GN_QoP );
         }

         if(supl_Is_Version2( p_SUPL_Instance ))
         {

            // Check for triggered mode
            supl_PDU_Get_SUPLINIT_TriggeredMode( p_PDU_Decoded , &p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode);

            // Check if the request is an emergency call
            if( p_SUPL_Instance->p_SUPL->V2_Data.Emergency_CallReq == TRUE )
            {
                U1 Index = 0;

                //Check if there is any other ongoing SUPL transaction.
                for ( Index = 0 ; Index < SUPL_Instances.MaxInstanceCount ; Index++ )
                {
                    // Check if there is any other ongoing SUPL session.
                    // If the ongoing session is not a Emergency request & it is not in idle state
                    if (    SUPL_Instances.InstanceList[Index] != NULL
                         && SUPL_Instances.InstanceList[Index]->p_SUPL->V2_Data.Emergency_CallReq == FALSE
                         && SUPL_Instances.InstanceList[Index]->p_SUPL->State != state_SUPL_Idle )
                    {
                        SUPL_Send_Abort_Request(
                                                NULL,
                                                NULL,
                                                SUPL_Instances.InstanceList[Index]->Handle
                                                );
                    }
                }
            }
            else
            {
                //Check if there is an ongoing Emergency SUPL session and ignore current SUPL INIT if it is the case.
                U1 Index = 0;

                for ( Index = 0 ; Index < SUPL_Instances.MaxInstanceCount ; Index++ )
                {
                    // Check if there is any other ongoing SUPL session.
                    // If the ongoing session is not a Emergency request & it is not in idle state
                    if (    SUPL_Instances.InstanceList[Index] != NULL
                         && SUPL_Instances.InstanceList[Index]->p_SUPL->V2_Data.Emergency_CallReq == TRUE )
                    {
                        SUPL_Init_Process_Reqd = FALSE;
                        break;
                    }
                }

            }

         }

         if( SUPL_Init_Process_Reqd )
         {
            /* +LMSqb89433 */
            Status = GN_SUPL_Connect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                      &GN_Status,
                                                      &p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms );

            Next_State = state_SUPL_Comms_Open_Sent;
            if ( supl_PDU_Get_SUPLINIT_Notification( p_PDU_Decoded,
                                                    &p_SUPL_Instance->p_SUPL->SUPL_Notification ) )
            {
               // if a notification request was present in the SUPL-INIT this must be checked with the user.
               SUPL_Send_Notification_Request( p_SUPL_Instance, p_SUPL_Instance, p_SUPL_Instance->Handle );

               Next_State = state_SUPL_Awaiting_Confirmation_Or_Comms_Open_Sent;
            }
            else
            {
               p_SUPL_Instance->p_SUPL->Notification_Accepted = TRUE;
            }
            /* -LMSqb89433 */
         }
         else
         {
            Next_State = state_SUPL_Idle;
         }

      }
      else
      {
         /// If at some point an error was identified note it and request a connection
         /// to the server to send a SUPL-END with error code.
         p_SUPL_Instance->p_SUPL->Notification_Accepted = TRUE;
         Status = GN_SUPL_Connect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                   &GN_Status,
                                                   &p_SUPL_Instance->p_SUPL->SessionCfg.SUPL_Connect_Req_Parms );

         Next_State = state_SUPL_Error;
      }
      break;
   //--------------------------------------------------------------------------
   case state_SUPL_Idle:
   case state_SUPL_Comms_Open_Sent:
   case state_SUPL_SUPL_START_Sent:
   case state_SUPL_SUPL_RESPONSE_Sent:
   case state_SUPL_SUPL_POS_INIT_Sent:
   case state_SUPL_Waiting_For_RRLP:
   case state_SUPL_POS_Payload_Delivered:
   case state_SUPL_SUPL_POS_Sent:
   case state_SUPL_SUPL_END_Sent:
   case state_SUPL_Server_Open_Requested:
   case state_SUPL_Server_Active:
   case state_SUPL_Comms_Close_Sent:
   default:
     GN_SUPL_Log_MsgOutOfOrder( p_SUPL_Instance->Handle , p_PDU_Decoded );
     OS_ASSERT( 0 );
     break;
   //--------------------------------------------------------------------------
   }

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_PDU_Received(SUPL-INIT)" );
   return;
}


//*****************************************************************************
/// \brief
///      Handles the SUPL_RESPONSE PDU in a #event_SUPL_PDU_Received message.
///
/// \details
///      PDU specific processing invoked by #SUPL_Handle_PDU_Received() is
///      performed in this function specifically for the SUPL-RESPONSE PDU.
//*****************************************************************************
static void SUPL_Handle_PDU_SUPL_RESPONSE
(
   void              *p_PDU_Decoded,   ///< Pointer to a decoded pdu.
   s_SUPL_Instance   *p_SUPL_Instance  ///< The SUPL instance dealing with this PDU.
)
{
   void                 *p_PDU_Src              = 0;
   BL                   PDU_Ready_To_Send       = FALSE;
   e_SUPL_States        Next_State              = p_SUPL_Instance->p_SUPL->State;
   BL                   PDU_SlpSessionID_Present;
   BL                   Status;
  e_GN_Status          GN_Status;

   /// Retrieve various items from the PDU for use in subsequent PDUs and to check
   /// for validity of the PDU.
   // Retrieve and store SLP session ID from PDU.
   PDU_SlpSessionID_Present = supl_PDU_Get_slpSessionID(  p_PDU_Decoded,
                                                         &p_SUPL_Instance->p_SUPL->p_SlpSessionId,
                                                         &p_SUPL_Instance->p_SUPL->SlpId );

   //Retrieve Supported Network Information from SUPL RESPONSE PDU.
   if(supl_Is_Version2( p_SUPL_Instance ))
   {
      supl_PDU_Get_SUPLRESPONSE_Supported_Network_Info( p_PDU_Decoded, &p_SUPL_Instance->p_SUPL->V2_Data.p_SupportedNetworkInfo);
   }
   // Check the SET Session ID against the stored value.
   Status = supl_PDU_Validate_setSessionID(  p_PDU_Decoded,
                                             p_SUPL_Instance->p_SUPL->SetSessionId,
                                            &p_SUPL_Instance->p_SUPL->SetId );

   if ( ! Status )
   {
      // If the Set Session ID is invalid, i.e. different to the saved one, send
      // a SUPL-END with the incorrect ID.
      U2       SessionID;
      s_SetId  SetId;

      p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
      supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,      GN_StatusCode_invalidSessionId );
      supl_PDU_Get_setSessionID(       p_PDU_Decoded, &SessionID, &SetId );
      supl_PDU_Add_setSessionID(       p_PDU_Src,      SessionID, &SetId );
      PDU_Ready_To_Send  =  TRUE;
      Next_State         =  state_SUPL_Comms_Close_Sent;
   }
   else
   {
      switch ( p_SUPL_Instance->p_SUPL->State )
      {
      //-----------------------------------------------------------------------
      // Valid states.
      case state_SUPL_SUPL_START_Sent:
         // If we received the SUPL-RESPONSE after sending a SUPL-START
         // continue on in the sequence by making and sending a SUPL-POS-INIT.
         p_PDU_Src = supl_PDU_Make_SUPLPOSINIT( p_SUPL_Instance );
         PDU_Ready_To_Send  =  TRUE;
         Next_State         =  state_SUPL_SUPL_POS_INIT_Sent;
         // Start timer.
         GN_SUPL_Timer_Set_UT2( p_SUPL_Instance );
         break;
      //-----------------------------------------------------------------------
      case state_SUPL_Idle:
      case state_SUPL_Comms_Open_Sent:
      case state_SUPL_SUPL_RESPONSE_Sent:
      case state_SUPL_SUPL_POS_INIT_Sent:
      case state_SUPL_Waiting_For_RRLP:
      case state_SUPL_POS_Payload_Delivered:
      case state_SUPL_SUPL_POS_Sent:
      case state_SUPL_SUPL_END_Sent:
      case state_SUPL_Server_Open_Requested:
      case state_SUPL_Server_Active:
      case state_SUPL_Comms_Close_Sent:
         p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
         supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, GN_StatusCode_unexpectedMessage );
         PDU_Ready_To_Send  =  TRUE;
         Next_State         =  state_SUPL_Comms_Close_Sent;
         break;
      //-----------------------------------------------------------------------
      default:
         GN_SUPL_Log_MsgOutOfOrder( p_SUPL_Instance->Handle , p_PDU_Decoded );
         OS_ASSERT( 0 );
         break;
      //-----------------------------------------------------------------------
      }
   }

   if( PDU_Ready_To_Send )
   {
      BL Status;
      Status = GN_SUPL_PDU_Send( p_SUPL_Instance, p_PDU_Src );
      supl_PDU_Free( p_PDU_Src );
      if ( ! Status )
      {
         // Transitions straight to Idle to save waiting for a close confirmation.
         Next_State = state_SUPL_Idle ;
      }
   }
   if( Next_State == state_SUPL_Comms_Close_Sent )
   {
      GN_Status = GN_SUCCESS;
      GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                        &GN_Status );
   }

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_PDU_Received(SUPL-RESPONSE)" );

   return;
}


//*****************************************************************************
/// \brief
///      Handles the SUPL_AUTH_RESP PDU in a #event_SUPL_PDU_Received message.
///
/// \details
///      PDU specific processing invoked by #SUPL_Handle_PDU_Received() is
///      performed in this function specifically for the SUPL-AUTH-RESP PDU.
///
///      As we never send a SUPL-AUTH-REQ it is a protocol error if we ever
///      receive a SUPL-AUTH-RESP.
//*****************************************************************************
static void SUPL_Handle_PDU_SUPL_AUTH_RESP
(
   s_SUPL_Instance *p_SUPL_Instance    ///< The SUPL instance dealing with this PDU.
)
{
   void                 *p_PDU_Src           = 0;
   BL                   PDU_Ready_To_Send    = FALSE;
   e_SUPL_States        Next_State           = p_SUPL_Instance->p_SUPL->State;

   switch ( p_SUPL_Instance->p_SUPL->State )
   {
   //--------------------------------------------------------------------------
   // Valid states.
   case state_SUPL_Idle:
   case state_SUPL_Comms_Open_Sent:
   case state_SUPL_SUPL_START_Sent:
   case state_SUPL_SUPL_RESPONSE_Sent:
   case state_SUPL_SUPL_POS_INIT_Sent:
   case state_SUPL_Waiting_For_RRLP:
   case state_SUPL_POS_Payload_Delivered:
   case state_SUPL_SUPL_POS_Sent:
   case state_SUPL_SUPL_END_Sent:
   case state_SUPL_Server_Open_Requested:
   case state_SUPL_Server_Active:
   case state_SUPL_Comms_Close_Sent:
      p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
      supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, GN_StatusCode_protocolError );
      PDU_Ready_To_Send  =  TRUE;
      Next_State         =  state_SUPL_Comms_Close_Sent;
      break;
   //--------------------------------------------------------------------------
   default:
      OS_ASSERT( 0 );
      break;
   //--------------------------------------------------------------------------
   }

   if( PDU_Ready_To_Send )
   {
      BL Status;
      Status = GN_SUPL_PDU_Send( p_SUPL_Instance, p_PDU_Src );
      supl_PDU_Free( p_PDU_Src );
      if ( ! Status )
      {
         // Transitions straight to Idle to save waiting for a close confirmation.
         Next_State = state_SUPL_Idle ;
      }
   }

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_PDU_Received(SUPL-AUTH-RESP)" );

   return;
}


//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Comms_Open_Success message.
///
/// \details
///      When comms is open it is possible that we already have a SUPL sequence
///      in place (via SUPL-INIT) or we need to start one because we are
///      performing a Mobile Originated request.
///
///      Added complications occur when the SUPL-INIT may or may not have had
///      a notification associated with it.
//*****************************************************************************
static void SUPL_Handle_Comms_Open_Success( s_SUPL_Message* p_ThisMessage )
{
   s_SUPL_Instance      *p_SUPL_Instance     = p_ThisMessage->MsgHdr.p_InstanceOfDest;
   void                 *Handle              = p_ThisMessage->MsgHdr.Handle;
   BL                   PDU_Ready_To_Send    = FALSE;
   e_SUPL_States        Next_State;
   void                 *p_PDU_Src           = 0;     /* Type to decode        */

   // If the destination Instance in the message was not configured use the handle.
   if ( p_SUPL_Instance == NULL )
   {
      // Look up the Handle.
      p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
   }

   // If there was no destination Instance in the message and no Handle we need a
   // new instance to cope with this.
   if ( p_SUPL_Instance == NULL )
   {
      p_SUPL_Instance = SUPL_Instance_Request_New();
   }

   // Start by assuming there is no state change.
   Next_State = p_SUPL_Instance->p_SUPL->State;

   // We now have a server connection available.
   p_SUPL_Instance->p_SUPL->ServerConnectionAvailable = TRUE;

   switch ( p_SUPL_Instance->p_SUPL->State )
   {
   //--------------------------------------------------------------------------
   // Valid states.
   case state_SUPL_Idle:
   case state_SUPL_Comms_Open_Sent:
   {
      if(p_SUPL_Instance->p_SUPL->V2_Data.ThirdParty_Retrieve == TRUE)
      {
          /*This is reuqest for retrieving third party location so prepare fpr SUPL_SET_INIT message and start the UT9 timer*/
          p_PDU_Src   =  supl_PDU_Make_SUPLSETINIT( p_SUPL_Instance );
          /*Also set the next state to state_SUPL_SET_INIT_Sent*/
          Next_State  =state_SUPLv2_SUPL_SET_INIT_Sent;
          /*start the 60 sec UT9 timer and wait for SUPL_END from SLP*/
          GN_SUPL_V2_Timer_Set_UT9( p_SUPL_Instance );

      }
      else
      {
              /*This is normal reuqest so chech if it is SUPL_INIT or SUPL_START or SUPL_TRIGGERED_START message*/
          GN_SUPL_Log( "TriggerMode: p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode = %d",
               p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode );
          if ( p_SUPL_Instance->p_SUPL->SlpId.type != GN_SLPAddress_PR_NOTHING )
          {
             // If we already have an slp id it must have come from a SUPL-INIT.
             // Therefore start from the next stage in the NI sequence.
             /* +LMSqb89433 */
             Next_State = SUPL_Handle_SUPL_INIT_Comms( p_SUPL_Instance , &p_PDU_Src );
             /* -LMSqb89433 */
          }
          else if((supl_Is_Version2(  p_SUPL_Instance ))&&((p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode != GN_TriggeredModeNone)))
          {
             /// In the case where there was no SUPL-INIT we simply start the SUPL SI sequence. Since periodic sesssion is initiated, SUPL Triggered Start
             /// will be sent instead of SUPL-START.
             GN_SUPL_Log( "TriggerStart: Before fromating");
             p_PDU_Src   =  supl_PDU_Make_SUPLTRIGGEREDSTART( p_SUPL_Instance );
             Next_State  =  state_SUPLv2_SUPL_TRIGGER_START_Sent;
             GN_SUPL_Log( "TriggerStart: After fromating");
             // Start timer.
             GN_SUPL_Timer_Set_UT1( p_SUPL_Instance );
          }
          else
          {
             /// In the case where there was no SUPL-INIT we simply start the SUPL SI sequence
             /// by creating a SUPL-START.
             p_PDU_Src   =  supl_PDU_Make_SUPLSTART( p_SUPL_Instance );
             Next_State  =  state_SUPL_SUPL_START_Sent;
              // Start timer.
              GN_SUPL_Timer_Set_UT1( p_SUPL_Instance );
          }
      }


      PDU_Ready_To_Send = TRUE;


      break;
   }
   /* +LMSqb89433 */
   //--------------------------------------------------------------------------
   case state_SUPL_Awaiting_Confirmation_Or_Comms_Open_Sent:
      /* We have succesfully connected. Now await confirmation */
      Next_State = state_SUPL_Awaiting_Confirmation;
      break;
   /* -LMSqb89433 */
   //-------------------------------------------------------------------------
   case state_SUPL_Push_Received:
      // We now wait for the incoming Push to be read.
      break;

   //--------------------------------------------------------------------------
   case state_SUPL_Error:
      /// If while processing the SUPL-INIT we had an error we had to wait for a
      /// connection to the server before we can send a SUPL-END with the appropriate
      /// error code.
      p_PDU_Src          =  supl_PDU_Make_SUPLEND( p_SUPL_Instance );
      supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, p_SUPL_Instance->p_SUPL->ErrorStatus );
      supl_PDU_Add_SUPLEND_VER(        p_PDU_Src, p_SUPL_Instance );
      PDU_Ready_To_Send  =  TRUE;
      Next_State         =  state_SUPL_Comms_Close_Sent;
      break;

   //--------------------------------------------------------------------------
   // Invalid states
   case state_SUPL_SUPL_START_Sent:
   case state_SUPL_SUPL_RESPONSE_Sent:
   case state_SUPL_SUPL_POS_INIT_Sent:
   case state_SUPL_Waiting_For_RRLP:
   case state_SUPL_POS_Payload_Delivered:
   case state_SUPL_SUPL_POS_Sent:
   case state_SUPL_SUPL_END_Sent:
   case state_SUPL_Server_Open_Requested:
   case state_SUPL_Comms_Close_Sent:
   default:
      OS_ASSERT( 0 );
      break;
   //--------------------------------------------------------------------------
   }

   if( PDU_Ready_To_Send )
   {
      e_GN_Status    GN_Status = GN_SUCCESS;
      BL             Successful;
      BL             Status;

      Status = GN_SUPL_PDU_Send( p_SUPL_Instance, p_PDU_Src );
      supl_PDU_Free( p_PDU_Src );
      if ( ! Status )
      {
         Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                          &GN_Status );
         // Transitions straight to Idle to save waiting for a close confirmation.
         Next_State = state_SUPL_Idle ;
      }
   }

   if ( Next_State == state_SUPL_Comms_Close_Sent )
   {
      e_GN_Status    GN_Status = GN_SUCCESS;
      BL             Successful;

      Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                       &GN_Status );
      // Transitions straight to Idle to save waiting for a close confirmation.
      Next_State = state_SUPL_Idle ;
   }

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_Comms_Open_Success" );
   return;
}


//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_V2_Trigger message.
///
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_V2_Trigger this function starts a positioning session with the network
//*****************************************************************************
void SUPL_V2_Handle_Trigger( s_SUPL_Message* p_ThisMessage )
{
   void              *Handle           = p_ThisMessage->MsgHdr.Handle;
   s_SUPL_Instance   *p_SUPL_Instance;

   GN_SUPL_Log( "Inside SUPL_V2_Handle_Trigger");
   // If the message was directed at a specific instance, get the Instance.
   p_SUPL_Instance = SUPL_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );

   if ( p_SUPL_Instance == NULL )
   {
      GN_SUPL_Log( "SUPL_V2_Handle_Trigger: Instance is NULL ");
      // Look up the Handle.
      p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
   }

   if ( p_SUPL_Instance != NULL  &&  p_SUPL_Instance->p_SUPL != NULL )
   {
#ifdef SUPL_MODULE_LOGGING
       GN_SUPL_Log( "SUPL_V2_Handle_Trigger: Handle = %p, instance = %p, data container = %p", Handle, p_SUPL_Instance, p_SUPL_Instance->p_SUPL );
#endif
      void                 *p_PDU_Src        = NULL;
      BL                   PDU_Ready_To_Send = FALSE;
      e_SUPL_States        Next_State        = p_SUPL_Instance->p_SUPL->State;

      switch( p_SUPL_Instance->p_SUPL->State )
      {
           //-----------------------------------------------------------------------
           // Valid states.
         case state_SUPLv2_SUPL_TRIGGER_Active:

            if( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModeAreaEvent ||
                ( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModePeriodic
                  && p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger.NumberOfFixes > 0 )
              )
            {
               /* The behavior differs for MS-Based and MS-Assisted cases */

               /* MS-Based case, NI, if no assistance is required just send a SUPL-REPORT with position */
               /* MS-Based case, NI or SI, If Assistance is required, start off a positioning session */

               /* In MS-Assisted case, start off a positioning session */
               BL Start_Pos_Session = FALSE;

               if( (p_SUPL_Instance->p_SUPL->GN_PosMethod_Requested == GN_PosMethod_agpsSETassisted ) ||
                    (p_SUPL_Instance->p_SUPL->GN_PosMethod_Requested == GN_PosMethod_agpsSETassistedpref )
                    )
               {
                  Start_Pos_Session = TRUE;
               }
               else
               /* UE-Based case */
               {
                  s_GN_AGPS_Assist_Req vl_GN_AGPS_Assist;
#if 0
                  s_GN_AGPS_GAD_Data   GN_AGPS_GAD_Data;
                  U4                   NULL_OS_Time_ms      = 0;
                  U4                   GAD_Ref_TOW          = 0;
                  I2                   GAD_Ref_TOW_Subms;     // GAD Data Reference GPS Time of Week Sub-millisecond part [range 0..9999 x 0.0001 ms,  -1 = Unknown]
                  I4                   EFSP_dT_us;

                  /* Only if position is not available & assistance is required, start a POS session */
                  if(     !GN_AGPS_Get_GAD_Data( &NULL_OS_Time_ms, &GAD_Ref_TOW,  &GAD_Ref_TOW_Subms, &EFSP_dT_us, &GN_AGPS_GAD_Data )
                       && GN_AGPS_Get_Assist_Req( &vl_GN_AGPS_Assist ) )
#endif
                  if( GN_AGPS_Get_Assist_Req( &vl_GN_AGPS_Assist ) )
                  {
                     /* Library indicates assistance is required . Check for the fields */
#ifndef DO_NOT_USE_ALMANAC
                     if(( vl_GN_AGPS_Assist.Alm_Req ) && (vl_GN_AGPS_Assist.Num_Sat_Alm < 32))
                     {
                        GN_SUPL_Log( "SUPL_V2_Handle_Trigger : Alm needed");
                        Start_Pos_Session = TRUE;
                     }
#endif
                     if(( vl_GN_AGPS_Assist.Eph_Req ) && (vl_GN_AGPS_Assist.Num_Sat_Eph < 8))
                     {
                        GN_SUPL_Log( "SUPL_V2_Handle_Trigger : Eph needed");
                        Start_Pos_Session = TRUE;
                     }

                     if(( vl_GN_AGPS_Assist.Ref_Time_Req ) && (!vl_GN_AGPS_Assist.Approx_Time_Known ))
                     {
                        GN_SUPL_Log( "SUPL_V2_Handle_Trigger : Ref Time needed");
                        Start_Pos_Session = TRUE;
                     }

                     if(( vl_GN_AGPS_Assist.Ref_Pos_Req ) && ( !vl_GN_AGPS_Assist.Approx_Pos_Known ))
                     {
                        GN_SUPL_Log( "SUPL_V2_Handle_Trigger : Ref Pos needed");
                        Start_Pos_Session = TRUE;
                     }

                     if(( vl_GN_AGPS_Assist.Ion_Req ) && ( !vl_GN_AGPS_Assist.Ion_Known ))
                     {
                        GN_SUPL_Log( "SUPL_V2_Handle_Trigger : Iono needed");
                        Start_Pos_Session = TRUE;
                     }

                     if(( vl_GN_AGPS_Assist.UTC_Req ) && ( !vl_GN_AGPS_Assist.UTC_Known ))
                     {
                        GN_SUPL_Log( "SUPL_V2_Handle_Trigger : UTC needed");
                        Start_Pos_Session = TRUE;
                     }
                  }
               }

               /* If a POS Session is setup, then the trigger count is updated only at the end of the POS Sequence.
                  In case of sending a SUPL REPORT, we should manage the trigger count in here */
               if( Start_Pos_Session )
               {
                   s_GN_Position     GN_Position_Store_Local;
                   memset (&GN_Position_Store_Local, 0, sizeof (GN_Position_Store_Local));

                  p_PDU_Src   =  supl_PDU_Make_SUPLPOSINIT( p_SUPL_Instance );
                  if ( GN_SUPL_Cache_Valid( &GN_Position_Store_Local, p_SUPL_Instance->p_SUPL->p_GN_QoP ) )
                  {
                      supl_PDU_Add_SUPLPOSINIT_Position( p_PDU_Src, &GN_Position_Store_Local );
                      GN_SUPL_Log_CachePosReport( (void*) p_SUPL_Instance , &GN_Position_Store_Local , 0 );
                  }
                  Next_State = state_SUPL_SUPL_POS_INIT_Sent;
                  PDU_Ready_To_Send = TRUE;
                  // Start timer.
                  GN_SUPL_Timer_Set_UT2( p_SUPL_Instance );
               }

               /* Need to check if SUPL Report needs to be sent.*/
               else
               {
                  /* SUPL Report needs to be sent only in SUPL NI MSB cases */
                  if( !p_SUPL_Instance->p_SUPL->MO_Request )
                  {
                     BL                   SUPL_Report_To_Send  = FALSE;
                     s_GN_GPS_Nav_Data    Nav_Data;
                     s_GN_AGPS_GAD_Data   GN_AGPS_GAD_Data;
                     U4                   NULL_OS_Time_ms      = 0;
                     U4                   GAD_Ref_TOW          = 0;
                     I2                   GAD_Ref_TOW_Subms;     // GAD Data Reference GPS Time of Week Sub-millisecond part [range 0..9999 x 0.0001 ms,  -1 = Unknown]
                     I4                   EFSP_dT_us;
                     s_GN_Position        GN_Position;
                     memset( &GN_Position, 0, sizeof( s_GN_Position ) );

                     if(    GN_GPS_Get_Nav_Data_Copy(        &Nav_Data )
                         && GN_AGPS_Get_GAD_Data(            &NULL_OS_Time_ms,    &GAD_Ref_TOW,  &GAD_Ref_TOW_Subms, &EFSP_dT_us, &GN_AGPS_GAD_Data ) )
                     {
                        GN_SUPL_Position_From_Solution(  &GN_Position,  &Nav_Data,     &GN_AGPS_GAD_Data );

                        if( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModeAreaEvent )
                        {
                           if( GN_SUPL_Check_AreaEvent_Wrapper( Handle, &GN_AGPS_GAD_Data ) )
                               SUPL_Report_To_Send = TRUE;
                        }
                        else if( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModePeriodic )
                        {
                           SUPL_Report_To_Send = TRUE;
                        }

                        if( SUPL_Report_To_Send )
                        {
                           p_PDU_Src   =  supl_PDU_Make_SUPLREPORT(p_SUPL_Instance);

                           supl_PDU_Add_SUPLREPORT_Position(p_PDU_Src,&GN_Position);

                           PDU_Ready_To_Send = TRUE;
                           
                           p_SUPL_Instance->p_SUPL->V2_Data.Triggers.areaEventTrigger.repeatedReportingParams.maximumNumberOfReports--; // Report needs to be decreamented when ever sent
                        }
                     }
                  }

                  Next_State = state_SUPLv2_SUPL_TRIGGER_Active; // Next_State should be same for both SI and NI

                  /* In periodic mode, count down the number of fixes */
                  if( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModePeriodic )
                  {
                     p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger.NumberOfFixes--;
                  }

                  if( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger.NumberOfFixes == 0 )
                  {
                     GN_SUPL_V2_Timer_Set_UT8(p_SUPL_Instance);
                     GN_SUPL_Log("SUPL_V2_Handle_Trigger: Periodic Triggers Completed");
                  }
               }
            }
            else if(   p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModePeriodic
                    && p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger.NumberOfFixes == 0)
            {
               GN_SUPL_Log("SUPL_V2_Handle_Trigger: Handle = %p , Ignored as all triggers are sent!" ,
                  p_SUPL_Instance->Handle );
            }
            else
            {
               GN_SUPL_Log("SUPL_V2_Handle_Trigger: Handle = %p , Ignored as triggered mode is not set! Unexpected" ,
                  p_SUPL_Instance->Handle );
            }
            break;
         case state_SUPLv2_SUPL_TRIGGER_Suspended: /* @TODO */
           //-----------------------------------------------------------------------
         case state_SUPLv2_SUPL_TRIGGER_START_Sent:
         case state_SUPL_Idle:
         case state_SUPL_Comms_Close_Sent:
         case state_SUPL_Comms_Open_Sent:
         case state_SUPL_SUPL_END_Sent:
         case state_SUPL_SUPL_START_Sent:
         case state_SUPL_SUPL_RESPONSE_Sent:
         case state_SUPL_SUPL_POS_INIT_Sent:
         case state_SUPL_Waiting_For_RRLP:
         case state_SUPL_POS_Payload_Delivered:
         case state_SUPL_SUPL_POS_Sent:
            GN_SUPL_Log( "SUPL_V2_Handle_Trigger: Ignored Trigger in state %s" ,
            SUPL_StateDesc_From_State(p_SUPL_Instance->p_SUPL->State));  //Roy
            return;
         default:
            // Coding error.
            OS_ASSERT( 0 );
            break;
           //-----------------------------------------------------------------------
      }

      if( PDU_Ready_To_Send )
      {
         e_GN_Status    GN_Status = GN_SUCCESS;
         BL             Successful;
         BL             Status;

         Status = GN_SUPL_PDU_Send( p_SUPL_Instance, p_PDU_Src );
         supl_PDU_Free( p_PDU_Src );
         if ( ! Status )
         {
            Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                             &GN_Status );
            // Transitions straight to Idle to save waiting for a close confirmation.
            Next_State = state_SUPL_Idle ;
         }
      }

      SUPL_Instance_State_Transition( p_SUPL_Instance,
                                      Next_State,
                                      "event_SUPL_V2_Trigger" );
   }

   return;
}
//*****************************************************************************
/// \brief
///      Verify the e-slp address from the network with the whitelist
///
/// \details
///
//*****************************************************************************

BL supl_Verify_Ecall_WhiteList(s_GN_SUPL_V2_InstanceData *p_SUPL_V2_InstanceData)
{
    U1 index;
    CH *p_Eslp_address;

    for(index=0;index<10;index++)
    {
      p_Eslp_address = supl_config_get_SUPL_eslp_whitelist_entry(index);

      if( p_Eslp_address != NULL )
      {
         if(strcmp(p_Eslp_address, p_SUPL_V2_InstanceData->Eslp_FQDN) ==0)
         return TRUE;
      }
    }
    return FALSE;
}




//*****************************************************************************



//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_V2_Trigger_Periodic_Requested.
///
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_V2_Trigger_Periodic_Requested this function set the session as Periodic
//*****************************************************************************

void SUPL_V2_Handle_Trigger_Periodic_Requested( s_SUPL_Message* p_ThisMessage )
{
    void              *Handle            = p_ThisMessage->MsgHdr.Handle;
    s_SUPL_Instance   *p_SUPL_Instance;
    s_GN_SUPL_V2_Triggers *p_Triggers = p_ThisMessage->Payload.p_Triggers;
    GN_SUPL_Log( "Inside SUPL_V2_Handle_Trigger_Periodic_Requested");

    if(p_Triggers == NULL) return;

    // If the message was directed at a specific instance, get the Instance.
    p_SUPL_Instance = SUPL_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );

    if ( p_SUPL_Instance == NULL )
    {
       GN_SUPL_Log( "SUPL_V2_Handle_Trigger_Periodic_Requested: Instance is NULL ");
       // Look up the Handle.
       p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
    }


    if ( p_SUPL_Instance != NULL  &&  p_SUPL_Instance->p_SUPL != NULL )
    {
#ifdef SUPL_MODULE_LOGGING
        GN_SUPL_Log( "SUPL_V2_Handle_Trigger_Periodic_Requested: Handle = %p, instance = %p, data container = %p", Handle, p_SUPL_Instance, p_SUPL_Instance->p_SUPL );
#endif

        switch( p_SUPL_Instance->p_SUPL->State )
        {
            case state_SUPL_Comms_Open_Sent:
                /*Triger Start must be send before SUPL Start/Trigger Start request is made to network*/
                GN_SUPL_Log( "SUPL_V2_Handle_Trigger_Periodic_Requested: Handle = %p, instance = %p, Mode=%d", Handle, p_SUPL_Instance, p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode);
                p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode    = p_Triggers->triggeredMode;
                p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger  = p_Triggers->periodicTrigger;
                p_SUPL_Instance->p_SUPL->V2_Data.Triggers.areaEventTrigger = p_Triggers->areaEventTrigger;
                GN_SUPL_Log( "SUPL_V2_Handle_Trigger_Periodic_Requested After: Handle = %p, instance = %p, Mode=%d", Handle, p_SUPL_Instance, p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode);
            break;
            case state_SUPLv2_SUPL_TRIGGER_Suspended: /* @TODO */
            case state_SUPLv2_SUPL_TRIGGER_START_Sent:
            case state_SUPL_Idle:
            case state_SUPL_Comms_Close_Sent:
            case state_SUPL_SUPL_END_Sent:
            case state_SUPL_SUPL_START_Sent:
            case state_SUPL_SUPL_RESPONSE_Sent:
            case state_SUPL_SUPL_POS_INIT_Sent:
            case state_SUPL_Waiting_For_RRLP:
            case state_SUPL_POS_Payload_Delivered:
            case state_SUPL_SUPL_POS_Sent:
                GN_SUPL_Log( "SUPL_V2_Handle_Trigger_Periodic_Requested: Ignored Trigger Peridic Request in state %s" ,
                GN_SUPL_StatusDesc_From_Status(p_SUPL_Instance->p_SUPL->State));
                return;
            default:
               // Coding error.
               OS_ASSERT( 0 );
               break;
            //-----------------------------------------------------------------------
        }

    }
}


//*****************************************************************************
/// \brief
///      Handles the #SUPL_V2_Handle_ThirdParty_Location_Request.
///
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_V2_Third_Party_info_Request  this function sets the third party data on to the supl instance
//*****************************************************************************

void SUPL_V2_Handle_ThirdParty_Location_Request( s_SUPL_Message* p_ThisMessage )
{
    void              *Handle            = p_ThisMessage->MsgHdr.Handle;
    s_SUPL_Instance   *p_SUPL_Instance;
    s_GN_SUPL_V2_ThirdPartyInfo *p_Third_Party_Info = p_ThisMessage->Payload.p_Third_Party_Info;
    GN_SUPL_Log( "Inside SUPL_V2_Handle_ThirdParty_Location_Request");

    if(p_Third_Party_Info == NULL) return;

    // If the message was directed at a specific instance, get the Instance.
    p_SUPL_Instance = SUPL_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );

    if ( p_SUPL_Instance == NULL )
    {
       GN_SUPL_Log( "SUPL_V2_Handle_ThirdParty_Location_Request: Instance is NULL ");
       // Look up the Handle.
       p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
    }


    if ( p_SUPL_Instance != NULL  &&  p_SUPL_Instance->p_SUPL != NULL )
    {
#ifdef SUPL_MODULE_LOGGING
        GN_SUPL_Log( "SUPL_V2_Handle_ThirdParty_Location_Request: Handle = %p, instance = %p, data container = %p", Handle, p_SUPL_Instance, p_SUPL_Instance->p_SUPL );
#endif
        GN_SUPL_Log( "SUPL_V2_Handle_ThirdParty_Location_Request: SRC Third Pary Name ==    %s",
                      p_Third_Party_Info->thirdPartyIdName );

        GN_SUPL_Log( "SUPL_V2_Handle_ThirdParty_Location_Request: SUPL State == %d",
                      p_SUPL_Instance->p_SUPL->State );


        switch( p_SUPL_Instance->p_SUPL->State )
        {
             case state_SUPL_Comms_Open_Sent:

                 p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData = GN_Calloc(1,(sizeof(s_GN_SUPL_V2_ThirdPartyInfo)));
                 p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData->thirdPartyId = p_Third_Party_Info->thirdPartyId;

#ifdef SUPL_API_LOGGING
                 GN_SUPL_Log( "SUPL_V2_Handle_ThirdParty_Location_Request: Third Pary Id ==  %d",
                                p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData->thirdPartyId );
#endif
                 p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData->thirdPartyIdName = GN_Calloc(1,(strlen( p_Third_Party_Info->thirdPartyIdName)+1));
                 memcpy(p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData->thirdPartyIdName,  p_Third_Party_Info->thirdPartyIdName, strlen( p_Third_Party_Info->thirdPartyIdName));
                 p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData->thirdPartyIdName[(strlen( p_Third_Party_Info->thirdPartyIdName))+1] = '\0';

#ifdef SUPL_API_LOGGING
                 GN_SUPL_Log( "SUPL_V2_Handle_ThirdParty_Location_Request: Third Pary Id ==  %s",
                                p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData->thirdPartyIdName );
#endif

            break;
            case state_SUPLv2_SUPL_TRIGGER_Suspended: /* @TODO */
            case state_SUPLv2_SUPL_TRIGGER_START_Sent:
            case state_SUPL_Idle:
            case state_SUPL_Comms_Close_Sent:
            case state_SUPL_SUPL_END_Sent:
            case state_SUPL_SUPL_START_Sent:
            case state_SUPL_SUPL_RESPONSE_Sent:
            case state_SUPL_SUPL_POS_INIT_Sent:
            case state_SUPL_Waiting_For_RRLP:
            case state_SUPL_POS_Payload_Delivered:
            case state_SUPL_SUPL_POS_Sent:
                GN_SUPL_Log( "SUPL_V2_Handle_ThirdParty_Location_Request: Ignored Third Party Request in state %s" ,
                GN_SUPL_StatusDesc_From_Status(p_SUPL_Instance->p_SUPL->State));
                return;
            default:
               // Coding error.
               GN_SUPL_Log( "SUPL_V2_Handle_ThirdParty_Location_Request:No Valid state");
               OS_ASSERT( 0 );
               break;
            //-----------------------------------------------------------------------
        }
    }
}
//*****************************************************************************
/// \brief
///      Handles the #SUPL_V2_Handle_ThirdPartyPosReq.
///
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_V2_ThirdPartyPosReq_Received  this function sets the third party data on to the supl instance
//*****************************************************************************

void SUPL_V2_Handle_ThirdPartyPosReq( s_SUPL_Message* p_ThisMessage )
{
    void              *Handle            = p_ThisMessage->MsgHdr.Handle;
    s_SUPL_Instance   *p_SUPL_Instance;
    s_GN_SUPL_V2_ThirdPartyInfo *p_Third_Party_Info = p_ThisMessage->Payload.p_Third_Party_Info;
    GN_SUPL_Log( "Inside SUPL_V2_Handle_ThirdPartyPosReq");

    if(p_Third_Party_Info == NULL) return;

    // If the message was directed at a specific instance, get the Instance.
    p_SUPL_Instance = SUPL_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );

    if ( p_SUPL_Instance == NULL )
    {
       GN_SUPL_Log( "SUPL_V2_Handle_ThirdPartyPosReq: Instance is NULL ");
       // Look up the Handle.
       p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
    }


    if ( p_SUPL_Instance != NULL  &&  p_SUPL_Instance->p_SUPL != NULL )
    {
#ifdef SUPL_MODULE_LOGGING
        GN_SUPL_Log( "SUPL_V2_Handle_ThirdPartyPosReq: Handle = %p, instance = %p, data container = %p", Handle, p_SUPL_Instance, p_SUPL_Instance->p_SUPL );
#endif
        GN_SUPL_Log( "SUPL_V2_Handle_ThirdPartyPosReq: SRC Third Pary Name ==   %s",
                       p_Third_Party_Info->thirdPartyIdName );

        GN_SUPL_Log( "SUPL_V2_Handle_ThirdPartyPosReq: SUPL State ==    %d",
                       p_SUPL_Instance->p_SUPL->State );


        switch( p_SUPL_Instance->p_SUPL->State )
        {
             case state_SUPL_Comms_Open_Sent:

                 if(p_Third_Party_Info != NULL)
                 {
                 p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData = GN_Calloc(1,(sizeof(s_GN_SUPL_V2_ThirdPartyInfo)));
                 p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData->thirdPartyId = p_Third_Party_Info->thirdPartyId;

#ifdef SUPL_API_LOGGING
                 GN_SUPL_Log( "SUPL_V2_Handle_ThirdPartyPosReq: Target ID ==  %d",
                                p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData->thirdPartyId );
#endif
                 p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData->thirdPartyIdName = GN_Calloc(1,(strlen( p_Third_Party_Info->thirdPartyIdName)+1));
                 memcpy(p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData->thirdPartyIdName,  p_Third_Party_Info->thirdPartyIdName, strlen( p_Third_Party_Info->thirdPartyIdName));
                 p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData->thirdPartyIdName[(strlen( p_Third_Party_Info->thirdPartyIdName))+1] = '\0';

#ifdef SUPL_API_LOGGING
                 GN_SUPL_Log( "SUPL_V2_Handle_ThirdPartyPosReq: Target ID Name ==  %s",
                                p_SUPL_Instance->p_SUPL->V2_Data.p_ThirdPartyData->thirdPartyIdName );
#endif
                 }
          
            break;
            case state_SUPLv2_SUPL_TRIGGER_Suspended: /* @TODO */
            case state_SUPLv2_SUPL_TRIGGER_START_Sent:
            case state_SUPL_Idle:
            case state_SUPL_Comms_Close_Sent:
            case state_SUPL_SUPL_END_Sent:
            case state_SUPL_SUPL_START_Sent:
            case state_SUPL_SUPL_RESPONSE_Sent:
            case state_SUPL_SUPL_POS_INIT_Sent:
            case state_SUPL_Waiting_For_RRLP:
            case state_SUPL_POS_Payload_Delivered:
            case state_SUPL_SUPL_POS_Sent:
                GN_SUPL_Log( "SUPL_V2_Handle_ThirdPartyPosReq: Ignored Third Party Request in state %s" ,
                GN_SUPL_StatusDesc_From_Status(p_SUPL_Instance->p_SUPL->State));
                return;
            default:
               // Coding error.
               GN_SUPL_Log( "SUPL_V2_Handle_ThirdPartyPosReq:No Valid state");
               OS_ASSERT( 0 );
               break;
            //-----------------------------------------------------------------------
        }
    }
}
//*****************************************************************************
/// \brief
///      Handles the #SUPL_V2_Handle_ApplicationID_Info_Request.
///
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_V2_ApplicationId_info_Request this function sets the application id info on to the supl instance
//*****************************************************************************

void SUPL_V2_Handle_ApplicationID_Info_Request( s_SUPL_Message* p_ThisMessage )
{
    void              *Handle            = p_ThisMessage->MsgHdr.Handle;
    s_SUPL_Instance   *p_SUPL_Instance;
    s_GN_SUPL_V2_ApplicationInfo *p_ApplicationID_info = p_ThisMessage->Payload.p_SUPL_ApplicationIDInfo;
    GN_SUPL_Log( "Inside SUPL_V2_Handle_ApplicationID_Info_Request");

    if(p_ApplicationID_info == NULL) return;

    // If the message was directed at a specific instance, get the Instance.
    p_SUPL_Instance = SUPL_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );

    if ( p_SUPL_Instance == NULL )
    {
       GN_SUPL_Log( "SUPL_V2_Handle_ApplicationID_Info_Request: Instance is NULL ");
       // Look up the Handle.
       p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
    }


    if ( p_SUPL_Instance != NULL  &&  p_SUPL_Instance->p_SUPL != NULL )
    {
#ifdef SUPL_MODULE_LOGGING
        GN_SUPL_Log( "SUPL_V2_Handle_ApplicationID_Info_Request: Handle = %p, instance = %p, data container = %p", Handle, p_SUPL_Instance, p_SUPL_Instance->p_SUPL );
#endif

        switch( p_SUPL_Instance->p_SUPL->State )
        {
             case state_SUPL_Comms_Open_Sent:

                 p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID =  GN_Calloc(1,(sizeof(s_GN_SUPL_V2_ApplicationInfo)));
                 p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->applicationIDInfoPresence = p_ApplicationID_info->applicationIDInfoPresence;
                 GN_SUPL_Log( "SUPL_V2_Handle_ApplicationID_Info_Request: Application Id Info Present ==  %d",
                              p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->applicationIDInfoPresence );
                 if(p_ApplicationID_info->applicationProvider != NULL)
                 {

                     p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->applicationProvider = GN_Calloc(1,(strlen(p_ApplicationID_info->applicationProvider)+1));
                     memcpy(p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->applicationProvider, p_ApplicationID_info->applicationProvider, strlen(p_ApplicationID_info->applicationProvider));
                     p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->applicationProvider[strlen(p_ApplicationID_info->applicationProvider)+1] = '\0';

                     GN_SUPL_Log( "SUPL_V2_Handle_ApplicationID_Info_Request: Application Id  Application Provider ==  %s",
                                 p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->applicationProvider );
                 }

                 if(p_ApplicationID_info->appName != NULL)
                 {

                     p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->appName = GN_Calloc(1,(strlen(p_ApplicationID_info->appName)+1));
                     memcpy( p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->appName, p_ApplicationID_info->appName, strlen(p_ApplicationID_info->appName));
                     p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->appName[strlen(p_ApplicationID_info->appName)+1] = '\0';

                     GN_SUPL_Log( "SUPL_V2_Handle_ApplicationID_Info_Request: Application Id  Application Name ==  %s",
                                  p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->appName );

                 }

                 if(p_ApplicationID_info->appVersion != NULL)
                 {

                     p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->appVersion = GN_Calloc(1,(strlen(p_ApplicationID_info->appVersion)+1));
                     memcpy( p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->appVersion, p_ApplicationID_info->appVersion, strlen(p_ApplicationID_info->appVersion));
                     p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->appVersion[strlen(p_ApplicationID_info->appVersion)+1] = '\0';

                     GN_SUPL_Log( "SUPL_V2_Handle_ApplicationID_Info_Request: Application Id  Application Version ==    %s",
                                   p_SUPL_Instance->p_SUPL->V2_Data.p_ApplicationID->appVersion );
                 }

            break;
            case state_SUPLv2_SUPL_TRIGGER_Suspended: /* @TODO */
            case state_SUPLv2_SUPL_TRIGGER_START_Sent:
            case state_SUPL_Idle:
            case state_SUPL_Comms_Close_Sent:
            case state_SUPL_SUPL_END_Sent:
            case state_SUPL_SUPL_START_Sent:
            case state_SUPL_SUPL_RESPONSE_Sent:
            case state_SUPL_SUPL_POS_INIT_Sent:
            case state_SUPL_Waiting_For_RRLP:
            case state_SUPL_POS_Payload_Delivered:
            case state_SUPL_SUPL_POS_Sent:
                GN_SUPL_Log( "SUPL_V2_Handle_ApplicationID_Info_Request: Ignored Application Id Request in state %s" ,
                GN_SUPL_StatusDesc_From_Status(p_SUPL_Instance->p_SUPL->State));
                return;
            default:
               // Coding error.
               OS_ASSERT( 0 );
               break;
            //-----------------------------------------------------------------------
        }



    }
}


//*****************************************************************************
/// \brief
///      Handles the #SUPL_V2_Handle_Mobile_Info_Received.
///
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_V2_Handle_Mobile_Info_Received this function Stores the Location Id data.
//*****************************************************************************
void SUPL_V2_Handle_Mobile_Info_Received( s_SUPL_Message* p_ThisMessage )
{
    U1 i;

    GN_SUPL_Log( "Inside SUPL_V2_Handle_Mobile_Info_Received");

    struct timeval current = {0};
    (void) gettimeofday( &current , NULL );

    s_GN_SUPL_V2_Multiple_Loc_ID* NewNode = (s_GN_SUPL_V2_Multiple_Loc_ID*)GN_Calloc( 1 , sizeof(s_GN_SUPL_V2_Multiple_Loc_ID) );

    NewNode->next = NULL ;
    NewNode->prev = NULL ;
    NewNode->v_OSTimeMs = current.tv_sec*1000;

    supl_log_LocationId( "SUPL_V2_Handle_Mobile_Info_Received:", p_ThisMessage->Payload.p_Location_Id);

    if(p_ThisMessage->Payload.p_Location_Id->Type == CIT_AccessPoint_WLAN)
    {
       s_GN_SUPL_V2_WLANAPInfo *p_WLANAPInfo;
       /*WLAN is the Wireless Network and Access point Information present is that of WLAN's*/
       /*so copy the WLAN Access Point Info on to the SUPL Instance*/
       NewNode->v_LocationId.Type   = p_ThisMessage->Payload.p_Location_Id->Type;
       NewNode->v_LocationId.Status = p_ThisMessage->Payload.p_Location_Id->Status;
       p_WLANAPInfo = &(NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo);

       if(((sizeof(p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr))+
            (sizeof(p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr)))<= 6)
       {
           p_WLANAPInfo->v_MS_Addr = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr;
           p_WLANAPInfo->v_LS_Addr = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr;

           if((p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_TransmitPower >= -127) &&
              (p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_TransmitPower <= 128))
           {
              p_WLANAPInfo->v_TransmitPower = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_TransmitPower;
           }
           else
           {
              /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
              p_WLANAPInfo->v_TransmitPower = 0;
           }
           if((p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_AntennaGain >= -127) &&
              (p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_AntennaGain <= 128))
           {
              p_WLANAPInfo->v_AntennaGain = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_AntennaGain;
           }
           else
           {
              /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
              p_WLANAPInfo->v_AntennaGain = 0;
           }

           if((p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SNR >= -127) &&
              (p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SNR <= 128))
           {
              p_WLANAPInfo->v_SNR = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SNR;
           }
           else
           {
              /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
              p_WLANAPInfo->v_SNR = 0;
           }

           if((p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SignalStrength >= -127) &&
              (p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SignalStrength <= 128))
           {
              p_WLANAPInfo->v_SignalStrength = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SignalStrength;
           }
           else
           {
              /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
              p_WLANAPInfo->v_SignalStrength = 0;
           }

           if((p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETTransmitPower >= -127) &&
              (p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETTransmitPower <= 128))
           {
              p_WLANAPInfo->v_SETTransmitPower = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETTransmitPower;
           }
           else
           {
              /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
              p_WLANAPInfo->v_SETTransmitPower = 0;
           }

           if((p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETAntennaGain >= -127) &&
              (p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETAntennaGain <= 128))
           {
              p_WLANAPInfo->v_SETAntennaGain = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETAntennaGain;
           }
           else
           {
              /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
              p_WLANAPInfo->v_SETAntennaGain = 0;
           }
           if((p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSNR >= -127) &&
              (p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSNR <= 128))
           {
              p_WLANAPInfo->v_SETSNR = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSNR;
           }
           else
           {
              /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
              p_WLANAPInfo->v_SETSNR = 0;
           }

           if((p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSignalStrength >= -127) &&
              (p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSignalStrength <= 128))
           {
              p_WLANAPInfo->v_SETSignalStrength = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSignalStrength;
           }
           else
           {
              /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
              p_WLANAPInfo->v_SETSignalStrength = 0;
           }

           if((p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Channel >= 0) &&
              (p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Channel <= 256))
           {
              p_WLANAPInfo->v_Channel = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Channel;
           }
           else
           {
              /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
              p_WLANAPInfo->v_Channel = 0;
           }

           if((p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Accuracy >= 0) &&
              (p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Accuracy <= 255))
           {
              p_WLANAPInfo->v_Accuracy = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Accuracy;
           }
           else
           {
              /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
              p_WLANAPInfo->v_Accuracy = 0;
           }

           if((p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDValue >= 0) &&
              (p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDValue <= 1677216))
           {
              p_WLANAPInfo->v_RTDValue = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDValue;
           }
           else
           {
              /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
              p_WLANAPInfo->v_RTDValue = 0;
           }
           if((p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_DeviceType >= 0) &&
              (p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_DeviceType <= 2))
           {
              p_WLANAPInfo->v_DeviceType = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_DeviceType;
           }
           else
           {
           /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
              p_WLANAPInfo->v_DeviceType = 0;
           }
           if((p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDUnits >= 0) &&
              (p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDUnits <= 4))
           {
               p_WLANAPInfo->v_RTDUnits = p_ThisMessage->Payload.p_Location_Id->of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDUnits;
           }
           else
           {
              /*Since apart from AP MAC address all other parameters are optional need to know what can they be filled with if they are found to be out of range*/
               p_WLANAPInfo->v_RTDUnits = 0;
           }
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_MS_Addr %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_MS_Addr);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_LS_Addr %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_LS_Addr);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_Accuracy %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Accuracy);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_AntennaGain %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_AntennaGain);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_Channel %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_Channel);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_DeviceType %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_DeviceType);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_RTDUnits %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDUnits);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_RTDValue %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_RTDValue);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_SETAntennaGain %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETAntennaGain);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_SETSignalStrength %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSignalStrength);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_SETSNR %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETSNR);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_SETTransmitPower %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SETTransmitPower);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_SignalStrength %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SignalStrength);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_SNR %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_SNR);
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WlanAccessPointInfo.v_TransmitPower %d",
                         NewNode->v_LocationId.of_type.wirelessNetworkInfo.AccessPointInfo.WlanAccessPointInfo.v_TransmitPower);
       }
       else
       {
           GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: WLAN AP MAC Address Invalid" );
       }
    }
    else if(p_ThisMessage->Payload.p_Location_Id->Type == CIT_AccessPoint_WIMAX)
    {
       /*This Wimax Acces Point Info is present so copy on to the SUPL_Instance*/
    }
    else
    {
       NewNode->v_LocationId.Status = p_ThisMessage->Payload.p_Location_Id->Status;
       if( SUPL_Mlid_FirstElem != NULL )
       {
           SUPL_Mlid_FirstElem->v_LocationId.Status = CIS_stale;
       }
       switch ( p_ThisMessage->Payload.p_Location_Id->Type )
       {
       case CIT_gsmCell:       ///< Cell information is from a GSM network.
          NewNode->v_LocationId.Type = p_ThisMessage->Payload.p_Location_Id->Type;
          {
             s_gsmCellInfo  *p_gsmCellInfo = &NewNode->v_LocationId.of_type.gsmCellInfo;
             // INTEGER(0..999), -- Mobile Country Code
             if ( /* p_LocationId->of_type.gsmCellInfo.refMCC >= 0 && */ p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.refMCC <= 999 )
             {
                p_gsmCellInfo->refMCC  = p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.refMCC;
             }
             else
             {
                GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: Cell Information == gsmCell: MCC %d out of range 0..999",
                             p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.refMCC);
                ///\todo Log at the moment and decide if we need to handle differently later.
                //Status = FALSE;
                //*p_Status = GN_ERR_PARAMETER_INVALID;
             }
             // INTEGER(0..999), -- Mobile Network Code
             if ( /* p_LocationId->of_type.gsmCellInfo.refMNC >= 0 && */ p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.refMNC <= 999 )
             {
                p_gsmCellInfo->refMNC  = p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.refMNC;
             }
             else
             {
                GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: Cell Information == gsmCell: MNC %d out of range 0..999",
                             p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.refMNC );
                ///\todo Log at the moment and decide if we need to handle differently later.
                //Status = FALSE;
                //*p_Status = GN_ERR_PARAMETER_INVALID;
             }
             // INTEGER(0..65535), -- Location area code
             //if (p_LocationId->of_type.gsmCellInfo.refLAC >= 0 && p_LocationId->of_type.gsmCellInfo.refLAC <= 65535)
             //{
                p_gsmCellInfo->refLAC  = p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.refLAC;
             //}
             //else
             //{
             //   GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == gsmCell: LAC %d out of range 0..65535",
             //                p_LocationId->of_type.gsmCellInfo.refLAC);
             //   ///\todo Log at the moment and decide if we need to handle differently later.
             //   //Status = FALSE;
             //   //*p_Status = GN_ERR_PARAMETER_INVALID;
             //}
             // INTEGER(0..65535), -- Cell identity
             //if (p_LocationId->of_type.gsmCellInfo.refLAC >= 0 && p_LocationId->of_type.gsmCellInfo.refLAC <= 65535)
             //{
                p_gsmCellInfo->refCI   = p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.refCI;
             //}
             //else
             //{
             //   GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == gsmCell: CI %d out of range 0..65535",
             //                p_LocationId->of_type.gsmCellInfo.refCI );
             //   ///\todo Log at the moment and decide if we need to handle differently later.
             //   //Status = FALSE;
             //   //*p_Status = GN_ERR_PARAMETER_INVALID;
             //}
             // INTEGER(0..255) OPTIONAL, --Timing Advance. (-1 = Not present).
             if ( p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.tA >= -1 &&
                  p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.tA <= 250 )
             {
                p_gsmCellInfo->tA      = p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.tA;
             }
             else
             {
                GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: Cell Information == gsmCell: tA %d out of range 0..250",
                             p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.tA );
                ///\todo Log at the moment and decide if we need to handle differently later.
                //Status = FALSE;
                //*p_Status = GN_ERR_PARAMETER_INVALID;
             }
             p_gsmCellInfo->NMRCount = p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.NMRCount;
             if ( p_gsmCellInfo->NMRCount != 0 )
             {
                s_NMRElement *p_NMRDestination, *p_NMRSource;

                p_gsmCellInfo->p_NMRList = GN_Calloc( p_gsmCellInfo->NMRCount, sizeof( s_NMRElement ) );
                p_NMRDestination = p_gsmCellInfo->p_NMRList;
                p_NMRSource = p_ThisMessage->Payload.p_Location_Id->of_type.gsmCellInfo.p_NMRList;
                for ( i = 0 ; i < p_gsmCellInfo->NMRCount ; i++ )
                {
                   // INTEGER(0..1023),
                   if ( /* p_NMRSource->aRFCN >= 0 && */ p_NMRSource->aRFCN <= 1023 )
                   {
                      p_NMRDestination->aRFCN = p_NMRSource->aRFCN;
                   }
                   else
                   {
                      GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: Cell Information == gsmCell: NMR %u of %u : aRFCN %d out of range 0..1023",
                                   i + 1,
                                   p_gsmCellInfo->NMRCount,
                                   p_NMRSource->aRFCN );
                      ///\todo Log at the moment and decide if we need to handle differently later.
                      //Status = FALSE;
                      //*p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                   // INTEGER(0..63),
                   if ( /* p_NMRSource->bSIC >= 0 && */ p_NMRSource->bSIC <= 63 )
                   {
                      p_NMRDestination->bSIC = p_NMRSource->bSIC;
                   }
                   else
                   {
                      GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: Cell Information == gsmCell: NMR %u of %u : bSIC %d out of range 0..63",
                                   i + 1,
                                   p_gsmCellInfo->NMRCount,
                                   p_NMRSource->bSIC );
                      ///\todo Log at the moment and decide if we need to handle differently later.
                      //Status = FALSE;
                      //*p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                   // INTEGER(0..63),
                   if ( /* p_NMRSource->rxLev >= 0 && */ p_NMRSource->rxLev <= 63 )
                   {
                      p_NMRDestination->rxLev = p_NMRSource->rxLev;
                   }
                   else
                   {
                      GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: Cell Information == gsmCell: NMR %u of %u : rxLev %d out of range 0..63",
                                   i + 1,
                                   p_gsmCellInfo->NMRCount,
                                   p_NMRSource->rxLev );
                      ///\todo Log at the moment and decide if we need to handle differently later.
                      //Status = FALSE;
                      //*p_Status = GN_ERR_PARAMETER_INVALID;
                   }
                   p_NMRDestination++;
                   p_NMRSource++;
                }
             }
          }
          break;
       case CIT_cdmaCell:         ///< Cell information is from a CDMA network.
          NewNode->v_LocationId.Type = p_ThisMessage->Payload.p_Location_Id->Type;
          {
             s_cdmaCellInfo  *p_cdmaCellInfo = &NewNode->v_LocationId.of_type.cdmaCellInfo;

             // INTEGER(0..65535), -- Network Id
             //if ( p_LocationId->of_type.cdmaCellInfo.refNID >= 0 && p_LocationId->of_type.cdmaCellInfo.refNID <= 65535 )
             //{
                p_cdmaCellInfo->refNID        = p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refNID;          // INTEGER(0..65535), -- Network Id
             //}
             //else
             //{
             //   GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == cdmaCell: refNID %u out of range 0..65535",
             //                p_LocationId->of_type.cdmaCellInfo.refNID );
             //   ///\todo Log at the moment and decide if we need to handle differently later.
             //   //Status = FALSE;
             //   //*p_Status = GN_ERR_PARAMETER_INVALID;
             //}
             // INTEGER(0..32767), -- System Id
             if ( /* p_LocationId->of_type.cdmaCellInfo.refSID >= 0 && */ p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refSID <= 32767 )
             {
                p_cdmaCellInfo->refSID        = p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refSID;          // INTEGER(0..32767), -- System Id
             }
             else
             {
                GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: Cell Information == cdmaCell: refSID %u out of range 0..32767",
                             p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refSID );
                ///\todo Log at the moment and decide if we need to handle differently later.
                //Status = FALSE;
                //*p_Status = GN_ERR_PARAMETER_INVALID;
             }
             // INTEGER(0..65535), -- Base Station Id
             //if ( p_LocationId->of_type.cdmaCellInfo.refBASEID >= 0 && p_LocationId->of_type.cdmaCellInfo.refBASEID <= 65535 )
             //{
                p_cdmaCellInfo->refBASEID     = p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refBASEID;       // INTEGER(0..65535), -- Base Station Id
             //}
             //else
             //{
             //   GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == cdmaCell: refSID %u out of range 0..65535",
             //                p_LocationId->of_type.cdmaCellInfo.refBASEID );
             //   ///\todo Log at the moment and decide if we need to handle differently later.
             //   //Status = FALSE;
             //   //*p_Status = GN_ERR_PARAMETER_INVALID;
             //}
             // INTEGER(0..4194303), -- Base Station Latitude
             if ( /* p_LocationId->of_type.cdmaCellInfo.refBASELAT >= 0 && */ p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refBASELAT <= 4194303 )
             {
                p_cdmaCellInfo->refBASELAT    = p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refBASELAT;      // INTEGER(0..4194303), -- Base Station Latitude
             }
             else
             {
                GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: Cell Information == cdmaCell: refBASELAT %u out of range 0..4194303",
                             p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refBASELAT );
                ///\todo Log at the moment and decide if we need to handle differently later.
                //Status = FALSE;
                //*p_Status = GN_ERR_PARAMETER_INVALID;
             }
             // INTEGER(0..8388607), -- Base Station Longitude
             if ( /* p_LocationId->of_type.cdmaCellInfo.reBASELONG >= 0 && */ p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.reBASELONG <= 8388607 )
             {
                p_cdmaCellInfo->reBASELONG    = p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.reBASELONG;      // INTEGER(0..8388607), -- Base Station Longitude
             }
             else
             {
                GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: Cell Information == cdmaCell: reBASELONG %u out of range 0..8388607",
                             p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.reBASELONG );
                ///\todo Log at the moment and decide if we need to handle differently later.
                //Status = FALSE;
                //*p_Status = GN_ERR_PARAMETER_INVALID;
             }
             // INTEGER(0..511), -- Base Station PN Code
             if ( /* p_LocationId->of_type.cdmaCellInfo.refREFPN >= 0 && */ p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refREFPN <= 511 )
             {
                p_cdmaCellInfo->refREFPN      = p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refREFPN;        // INTEGER(0..511), -- Base Station PN Code
             }
             else
             {
                GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: Cell Information == cdmaCell: refREFPN %u out of range 0..511",
                             p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refREFPN );
                ///\todo Log at the moment and decide if we need to handle differently later.
                //Status = FALSE;
                //*p_Status = GN_ERR_PARAMETER_INVALID;
             }
             // INTEGER(0..65535), -- GPS Week Number
             //if ( p_LocationId->of_type.cdmaCellInfo.refWeekNumber >= 0 && p_LocationId->of_type.cdmaCellInfo.refWeekNumber <= 65535 )
             //{
                p_cdmaCellInfo->refWeekNumber = p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refWeekNumber;   // INTEGER(0..65535), -- GPS Week Number
             //}
             //else
             //{
             //   GN_SUPL_Log( "GN_SUPL_Connect_Ind_In: Cell Information == cdmaCell: refWeekNumber %u out of range 0..65535",
             //                p_LocationId->of_type.cdmaCellInfo.refWeekNumber );
             //   ///\todo Log at the moment and decide if we need to handle differently later.
             //   //Status = FALSE;
             //   //*p_Status = GN_ERR_PARAMETER_INVALID;
             //}
             // INTEGER(0..4194303),-- GPS Seconds
             if ( /* p_LocationId->of_type.cdmaCellInfo.refSeconds >= 0 && */ p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refSeconds <= 4194303 )
             {
                p_cdmaCellInfo->refSeconds = p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refSeconds;   // INTEGER(0..65535), -- GPS Week Number
             }
             else
             {
                GN_SUPL_Log( "SUPL_V2_Handle_Mobile_Info_Received: Cell Information == cdmaCell: refSeconds %u out of range 0..4194303",
                             p_ThisMessage->Payload.p_Location_Id->of_type.cdmaCellInfo.refSeconds );
                ///\todo Log at the moment and decide if we need to handle differently later.
                //Status = FALSE;
                //*p_Status = GN_ERR_PARAMETER_INVALID;
             }
          }
          break;
       case CIT_wcdmaCell:        // Cell information is from a WCDMA network.
          NewNode->v_LocationId.Type = p_ThisMessage->Payload.p_Location_Id->Type;
          supl_Copy_wcdmaCellInfo( &NewNode->v_LocationId.of_type.wcdmaCellInfo, &p_ThisMessage->Payload.p_Location_Id->of_type.wcdmaCellInfo );
          break;
       }
    }

    SUPL_Update_Loc_Id( NewNode );

    SUPL_Check_Loc_Id_Expiry();

    GN_SUPL_Log( "-SUPL_V2_Handle_Mobile_Info_Received:" );

}

//*****************************************************************************
/// \brief
///      Handles the #SUPL_V2_Handle_Delete_Location_id_Requested.
///
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #SUPL_V2_Handle_Delete_Location_id_Requested this function clears the location Id data Stored.
//*****************************************************************************

void SUPL_V2_Handle_Delete_Location_id_Requested( s_SUPL_Message* p_ThisMessage )
{
   s_GN_SUPL_V2_Multiple_Loc_ID* Temp  = NULL;
   s_GN_SUPL_V2_Multiple_Loc_ID* Temp1 = NULL;
   p_ThisMessage = p_ThisMessage;

   GN_SUPL_Log( "SUPL_V2_Handle_Delete_Location_id_Requested: Entry" );
   
   Temp  = SUPL_Mlid_FirstElem ;
   
   if(Temp != NULL)
   {
       while(Temp->prev != NULL)
       {
           Temp1 = Temp->prev ;
           GN_Free(Temp);
           Temp  = Temp1 ;
       }
       GN_Free(Temp);
       Temp = NULL;
   }
   SUPL_Mlid_FirstElem = NULL;
}


