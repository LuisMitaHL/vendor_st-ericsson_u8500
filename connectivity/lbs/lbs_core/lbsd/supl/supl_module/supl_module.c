
//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
//
// Filename supl_module.c
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/supl_module.c 1.133 2009/02/20 16:30:20Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_module
///
/// \file
/// \brief
///      SUPL Interface Implementation.
///
//*************************************************************************

#include <stdio.h>
#include <string.h>

#include "os_assert.h"

#include "supl_config.h"
#include "supl_module.h"
#include "supl_helper.h"
#include "supl_user_timers.h"
#include "supl_interface.h"
#include "GAD_Conversions.h"
#include "supl_log.h"
#include "GN_RRLP_api.h"

#include "SUPL_ULP_ULP-PDU.h"
#include "time.h"


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
   s_GN_GPS_Nav_Data* p_Nav_Data    ///< [in] Pointer to where the GPS Library should write the Nav Data to.
);

// global data
s_SUPL_Instances     SUPL_Instances;      // ???
s_SUPL_Queue         SUPL_Queue;          // ???

/// Cached position solution.
s_GN_Position        GN_Position_Cache;   // ???

//*****************************************************************************
/// \brief
///      Type of Multiple_Loc_ID.
/// \details
///         Head for the Multiple_Loc_ID list (linked list for storing the information of each Loc ID )
s_GN_SUPL_V2_Multiple_Loc_ID *SUPL_Mlid_FirstElem ;

// Internal function declarations.
static void GN_SUPL_Cache_Store_Position(    s_GN_Position *p_GN_Position );
static BL   GN_SUPL_Cache_Retrieve_Position( s_GN_Position *p_GN_Position );
static void GN_SUPL_Cache_Initialise(        void );
static void GN_SUPL_Multiple_LocationID_Initialise( void );
static void GN_SUPL_Cache_DeInitialise(      void );


//*****************************************************************************
/// \brief
///      Handles the SUPL_POS PDU in a #event_SUPL_PDU_Received message.
///
/// \details
///      PDU specific processing invoked by #SUPL_Handle_PDU_Received() is
///      performed in this function specifically for the SUPL-POS PDU.
//*****************************************************************************
void SUPL_Handle_PDU_SUPL_POS
(
   void            *p_PDU_Decoded,     ///< [in] Pointer to a decoded pdu.
   s_SUPL_Instance *p_SUPL_Instance,   ///< [in/out] The SUPL instance dealing with this PDU.
   U4               PDU_TimeStamp      ///< [in] Time at which the PDU was received by SUPL.
)
{
   void                *p_PDU_Src            = NULL;
   BL                   PDU_Ready_To_Send    = FALSE;
   e_SUPL_States        Next_State           = p_SUPL_Instance->p_SUPL->State;
   s_PDU_Encoded       *p_POS_Payload        = GN_Calloc( 1, sizeof( s_PDU_Encoded ) );
   BL                   Status;
   e_GN_Status          GN_Status;
   e_GN_StatusCode      GN_StatusCode;

   /// Perform initial validation checks and return a SUPL-END with an error code if they fail.
   if ( ! supl_PDU_Validate_setSessionID(  p_PDU_Decoded,
                                           p_SUPL_Instance->p_SUPL->SetSessionId,
                                          &p_SUPL_Instance->p_SUPL->SetId ) )
   {
      // If the Set Session ID is invalid, send a SUPL-END with the incorrect ID.
      U2          SessionID;
      s_SetId     SetId;

      p_PDU_Src          =  supl_PDU_Make_SUPLEND( p_SUPL_Instance );
      supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,      GN_StatusCode_invalidSessionId );
      supl_PDU_Get_setSessionID(       p_PDU_Decoded, &SessionID, &SetId );
      supl_PDU_Add_setSessionID(       p_PDU_Src,      SessionID, &SetId );
      PDU_Ready_To_Send  =  TRUE;
      Next_State         =  state_SUPL_Comms_Close_Sent;
   }

   else if ( ! supl_PDU_Validate_slpSessionID(  p_PDU_Decoded,
                                                p_SUPL_Instance->p_SUPL->p_SlpSessionId,
                                               &p_SUPL_Instance->p_SUPL->SlpId ) )
   {
      // If the Set Session ID is invalid, send a SUPL-END with the incorrect ID.
      U1               SessionID[4];
      U1               *p_SessionID = SessionID;
      s_GN_SLPAddress  SlpId;

      p_PDU_Src          =  supl_PDU_Make_SUPLEND( p_SUPL_Instance );
      supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,      GN_StatusCode_invalidSessionId );
      supl_PDU_Get_slpSessionID(       p_PDU_Decoded, &p_SessionID, &SlpId );
      supl_PDU_Add_slpSessionID(       p_PDU_Src,      p_SessionID, &SlpId );

      PDU_Ready_To_Send  =  TRUE;
      Next_State         =  state_SUPL_Comms_Close_Sent;
   }
   else
   {
      s_GN_RRLP_QoP   GN_RRLP_QoP   = { 0, 0, 0, 0 };
      s_GN_RRLP_QoP  *p_GN_RRLP_QoP = NULL;

      switch ( p_SUPL_Instance->p_SUPL->State )
      {
      //-----------------------------------------------------------------------
      // Valid states.
      case state_SUPL_SUPL_POS_INIT_Sent:
      case state_SUPL_SUPL_POS_Sent:
         if ( p_SUPL_Instance->p_SUPL->p_GN_QoP )
         {
            s_GN_QoP *p_GN_QoP = p_SUPL_Instance->p_SUPL->p_GN_QoP;

            p_GN_RRLP_QoP = &GN_RRLP_QoP;

            if ( p_GN_QoP->p_horacc != NULL )
            {
               GN_RRLP_QoP.horacc    = *p_GN_QoP->p_horacc;
            }
            if ( p_GN_QoP->p_veracc != NULL )
            {
               GN_RRLP_QoP.veracc    = *p_GN_QoP->p_veracc;
            }
            if ( p_GN_QoP->p_maxLocAge != NULL )
            {
               GN_RRLP_QoP.maxLocAge = *p_GN_QoP->p_maxLocAge;
            }
            if ( p_GN_QoP->p_delay != NULL )
            {
               GN_RRLP_QoP.delay     = *p_GN_QoP->p_delay;
            }
         }

         if ( supl_Process_SUPL_POS( p_PDU_Decoded, p_POS_Payload, &GN_StatusCode ) )
         {
            if ( p_SUPL_Instance->POS_Handle == NULL )
            {
               /// If POS_Handle is not yet set, this is the first SUPL-POS
               /// message so start a new sequence.
               GN_SUPL_Log("New RRLP Session Started for handle %p", p_SUPL_Instance->Handle );
               Status = GN_RRLP_Sequence_Start_In(
                           &p_SUPL_Instance->POS_Handle, // [out] Opaque Handle used to coordinate requests.
                           p_POS_Payload->Length,        // [in] Size of data at p_PDU in bytes.
                           p_POS_Payload->p_PDU_Data,    // [in] Pointer to data.
                           PDU_TimeStamp,                // [in] Timestamp of reception of PDU.
                           p_GN_RRLP_QoP,                 // [in] QoP if available.
                           p_SUPL_Instance->p_SUPL->PrivacyMode ); ///< [in] This is used to enable/disable logging from the RRLP module. If privacy Mode. No Logging
            }
            else
            {
               /// If POS Handle is set use that to indicate to the RRLP handler
               /// which RRLP Instance to process this with.
               Status = GN_RRLP_PDU_Delivery_In(
                           p_SUPL_Instance->POS_Handle,  // [out] Opaque Handle used to coordinate requests.
                           p_POS_Payload->Length,        // [in] Size of data at p_PDU in bytes.
                           p_POS_Payload->p_PDU_Data,    // [in] Pointer to data.
                           PDU_TimeStamp );              // [in] Timestamp of reception of PDU.
            }
         }
         else
         {
            // Error in processing the SUPL_POS,  send SUPLEND
            p_PDU_Src          =  supl_PDU_Make_SUPLEND( p_SUPL_Instance );
            supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, GN_StatusCode );
            PDU_Ready_To_Send  =  TRUE;
            Next_State         =  state_SUPL_Comms_Close_Sent;
         }

         // free SUPL_Pos_Payload
         GN_Free( p_POS_Payload->p_PDU_Data );
         p_POS_Payload->p_PDU_Data = NULL;
         Next_State     =  state_SUPL_POS_Payload_Delivered;
         break;
      //-----------------------------------------------------------------------
      // Invalid states,  send SUPLEND.
      case state_SUPL_Idle:
      case state_SUPL_Comms_Open_Sent:
      case state_SUPL_SUPL_START_Sent:
      case state_SUPL_SUPL_RESPONSE_Sent:
      case state_SUPL_Waiting_For_RRLP:
      case state_SUPL_POS_Payload_Delivered:
      case state_SUPL_SUPL_END_Sent:
      case state_SUPL_Server_Open_Requested:
      case state_SUPL_Server_Active:
      case state_SUPL_Comms_Close_Sent:
         p_PDU_Src         = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
         supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, GN_StatusCode_protocolError );
         PDU_Ready_To_Send = TRUE;
         Next_State        = state_SUPL_Comms_Close_Sent;
         break;
      //-----------------------------------------------------------------------
      // Coding error.
      default:
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
                                   "event_SUPL_PDU_Received(SUPL-POS)" );

   GN_Free( p_POS_Payload );
   p_POS_Payload= NULL;

   return;
}


//*****************************************************************************
/// \brief
///      Handles the SUPL_END PDU in a #event_SUPL_PDU_Received message.
///
/// \details
///      PDU specific processing invoked by #SUPL_Handle_PDU_Received() is
///      performed in this function specifically for the SUPL-END PDU.
//*****************************************************************************
void SUPL_Handle_PDU_SUPL_END
(
   void              *p_PDU_Decoded,   ///< [in] Pointer to a decoded pdu.
   s_SUPL_Instance   *p_SUPL_Instance  ///< [in/out] The SUPL instance dealing with this PDU.
)
{
   e_SUPL_States        Next_State        = p_SUPL_Instance->p_SUPL->State;
   e_GN_Status          GN_Status         = GN_SUCCESS;
   s_GN_AGPS_GAD_Data   GN_AGPS_GAD_Data;
   s_GN_SUPL_Pos_Data   GN_SUPL_Pos_Data;
   s_GN_GPS_Nav_Data    GN_GPS_Nav_Data;

   switch ( p_SUPL_Instance->p_SUPL->State )
   {
   //-----------------------------------------------------------------------
   // Valid states.
   case state_SUPL_SUPL_POS_Sent:
   case state_SUPL_Waiting_For_RRLP:
   case state_SUPL_POS_Payload_Delivered:
   case state_SUPL_SUPL_START_Sent:
   case state_SUPL_SUPL_POS_INIT_Sent:
   case state_SUPLv2_SUPL_TRIGGER_Active:
      if ( p_SUPL_Instance->POS_Handle != NULL )
      {
         // There has been an exchange with the Supl Positioning subsystem.
         BL Status;

         Status = GN_RRLP_Sequence_End_In( p_SUPL_Instance->POS_Handle,
                                           &GN_Status,
                                           &GN_AGPS_GAD_Data );
         if ( Status )
         {
            BL             Status, Successful;
            s_GN_Position  GN_Position_Store;

            Status = GN_GPS_Get_Nav_Data_Copy( &GN_GPS_Nav_Data );

            GN_SUPL_Pos_Data_From_Nav_Data( &GN_SUPL_Pos_Data, &GN_GPS_Nav_Data );

            // The exchange with the Supl Positioning subsystem resulted in a fix.
            GN_SUPL_Log( "Ending SUPL sequence with GPS position:" );


            GN_Status = GN_SUCCESS;
            Status = GN_SUPL_Position_Resp_Out_Wrapper( p_SUPL_Instance->Handle,
                                                        &GN_Status,
                                                        Status ? &GN_SUPL_Pos_Data : NULL );


            // Even though we had a gps fix, lets check in case of MSA session has a valid position.
            Successful = supl_PDU_Get_SUPLEND_GAD_Position( p_PDU_Decoded, &GN_Position_Store, &GN_AGPS_GAD_Data );
            GN_SUPL_Log( "supl_PDU_Get_SUPLEND_GAD_Position: %s", Successful ? "Success" : "Failure" );

            /***  TODO:  Decide if the following is a good idea
            {
               s_GN_Position  GN_Position_Store;

               // Even though we had a gps fix, let's use any position in SUPL-END as a ref pos.
               Status = supl_PDU_Get_SUPLEND_GAD_Position( p_PDU_Decoded, &GN_Position_Store, &GN_AGPS_GAD_Data );
               GN_SUPL_Log( "supl_PDU_Get_SUPLEND_GAD_Position: %s", Status ? "Success" : "Failure" );
               if ( Status )
               {
                  // SUPL-END had a position, use this in the position response.
                  // Input the position to the GN_GPS_Lib in case this is good enough for a
                  // user only asking for a low QoP result from via GN_GPS_Get_Nav_Data() etc.
                  GN_AGPS_Set_GAD_Ref_Pos( &GN_AGPS_GAD_Data );
               }
            }
            ***/
         }
         else
         {
            BL             Status;
            s_GN_Position  GN_Position_Store;

            // The exchange with the Supl Positioning subsystem did not result in a fix.
            // Check the SUPL-END for a position.
            Status = supl_PDU_Get_SUPLEND_GAD_Position( p_PDU_Decoded, &GN_Position_Store, &GN_AGPS_GAD_Data );
            GN_SUPL_Log( "supl_PDU_Get_SUPLEND_GAD_Position: %s", Status ? "Success" : "Failure" );
            if ( Status )
            {
               // SUPL-END had a position, use this in the position response.
               // Input the position to the GN_GPS_Lib in case this is good enough for a
               // user only asking for a low QoP result from via GN_GPS_Get_Nav_Data() etc.
               GN_AGPS_Set_GAD_Ref_Pos( &GN_AGPS_GAD_Data );
               GN_SUPL_Pos_Data_From_Position( &GN_SUPL_Pos_Data, &GN_Position_Store );

               GN_SUPL_Cache_Store_Position( &GN_Position_Store );
               GN_SUPL_Log( "Ending SUPL sequence with position from SUPL-END:" );
               GN_Status = GN_SUCCESS;
               Status = GN_SUPL_Position_Resp_Out_Wrapper( p_SUPL_Instance->Handle,
                                                           &GN_Status,
                                                           &GN_SUPL_Pos_Data );
            }
            else
            {
               GN_SUPL_Log( "Ending SUPL sequence with no position:" );
               GN_Status = GN_WRN_NO_POSITION;
               Status = GN_SUPL_Position_Resp_Out_Wrapper( p_SUPL_Instance->Handle,
                                                           &GN_Status,
                                                           NULL );
            }
         }
      }
      else
      {
         BL             Status;
         s_GN_Position  GN_Position_Store;

         // There has been NO exchange with the Supl Positioning subsystem.
         Status = supl_PDU_Get_SUPLEND_GAD_Position( p_PDU_Decoded, &GN_Position_Store, &GN_AGPS_GAD_Data );
         GN_SUPL_Log( "supl_PDU_Get_SUPLEND_GAD_Position: %s", Status ? "Success" : "Failure" );
         if ( Status )
         {
            // Input the position to the GN_GPS_Lib in case this is good enough for a
            // user only asking for a low QoP result from via GN_GPS_Get_Nav_Data() etc.
            GN_AGPS_Set_GAD_Ref_Pos( &GN_AGPS_GAD_Data );
            GN_SUPL_Pos_Data_From_Position( &GN_SUPL_Pos_Data, &GN_Position_Store );

            GN_SUPL_Cache_Store_Position( &GN_Position_Store );
            GN_SUPL_Log( "Ending SUPL sequence with position from SUPL-END:" );
            GN_Status = GN_SUCCESS;
            Status = GN_SUPL_Position_Resp_Out_Wrapper( p_SUPL_Instance->Handle,
                                                        &GN_Status,
                                                        &GN_SUPL_Pos_Data );
         }
         else
         {
            GN_SUPL_Log( "Ending SUPL sequence with no position:" );
            GN_Status = GN_WRN_NO_POSITION;
            Status = GN_SUPL_Position_Resp_Out_Wrapper( p_SUPL_Instance->Handle,
                                                        &GN_Status,
                                                        NULL );
         }
      }
      Next_State = state_SUPL_Idle;
      break;
   case state_SUPLv2_SUPL_SET_INIT_Sent:
   {

    /*No position exchange with SUPL positioning subsytem*/
        BL             Status;
        s_GN_Position  GN_Position_Store;

        // There has been NO exchange with the Supl Positioning subsystem.
        Status = supl_PDU_Get_SUPLEND_GAD_Position( p_PDU_Decoded, &GN_Position_Store, &GN_AGPS_GAD_Data );
        p_SUPL_Instance->p_SUPL->V2_Data.ThirdParty_Retrieve = FALSE;
        GN_SUPL_Log( "supl_PDU_Get_SUPLEND_GAD_Position: %s", Status ? "Success" : "Failure" );
        if ( Status )
        {
           // Input the position to the GN_GPS_Lib in case this is good enough for a
           // user only asking for a low QoP result from via GN_GPS_Get_Nav_Data() etc.
           GN_SUPL_Pos_Data_From_Position( &GN_SUPL_Pos_Data, &GN_Position_Store );

           GN_SUPL_Log( "Ending SUPL sequence with position from SUPL-END:" );
           GN_Status = GN_SUCCESS;
           p_SUPL_Instance->p_SUPL->V2_Data.ThirdParty_Retrieve = FALSE;
               /*need to confirm what exactly is the data that is to be sent to the application*/
           Status = GN_SUPL_ThirdPartyPosition_Resp_Out_Wrapper( p_SUPL_Instance->Handle,
                                                       &GN_Status,
                                                       &GN_SUPL_Pos_Data );
        }
        else
        {
           GN_SUPL_Log( "Ending SUPL sequence with no position:" );
           GN_Status = GN_WRN_NO_POSITION;
              /*need to confirm what exactly is the data that is to be sent to the application*/
            p_SUPL_Instance->p_SUPL->V2_Data.ThirdParty_Retrieve = FALSE;
            Status = GN_SUPL_ThirdPartyPosition_Resp_Out_Wrapper( p_SUPL_Instance->Handle,
                                                       &GN_Status,
                                                       NULL);
        }
        Next_State = state_SUPL_Idle;
    }
      break;
   //-----------------------------------------------------------------------
   // Invalid states,
   case state_SUPL_Idle:
   case state_SUPL_Comms_Open_Sent:
   case state_SUPL_SUPL_RESPONSE_Sent:
   case state_SUPL_SUPL_END_Sent:
   case state_SUPL_Server_Open_Requested:
   case state_SUPL_Server_Active:
   case state_SUPL_Comms_Close_Sent:
   case state_SUPLv2_SUPL_TRIGGER_START_Sent:
   case state_SUPLv2_Location_Notification:
      //p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
      //supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,
      //                                 GN_StatusCode_protocolError );
      //PDU_Ready_To_Send  =  TRUE;
      //Next_State         =  state_SUPL_SUPL_END_Sent;
      Next_State = state_SUPL_Idle;
      break;
   //-----------------------------------------------------------------------
   // Coding error
   default:
      OS_ASSERT( 0 );
      break;
   //-----------------------------------------------------------------------
   }

   if ( Next_State == state_SUPL_Idle )
   {
      e_GN_Status    GN_Status = GN_SUCCESS;
      BL             Successful;

      Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                       &GN_Status );
   }

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_PDU_Received(SUPL-END)" );
   return;
}


//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Comms_Open_Failure message.
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_Comms_Open_Failure this function simply changes the current
///      state to state_SUPL_Idle which is then dealt with in
///      #SUPL_Instance_Handler().
//*****************************************************************************
void SUPL_Handle_Comms_Open_Failure( s_SUPL_Message* p_ThisMessage )
{
   void              *Handle           = p_ThisMessage->MsgHdr.Handle;
   s_SUPL_Instance   *p_SUPL_Instance;

   // If the message was directed at a specific instance, get the Instance.
   p_SUPL_Instance = SUPL_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );

   if ( p_SUPL_Instance == NULL )
   {
      // Try looking up the Instance via the Handle instead.
      p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
   }

   GN_SUPL_Log_ServerDisconnected( Handle );

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   state_SUPL_Idle,
                                   "event_SUPL_Comms_Open_Failure" );
   return;
}


//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Comms_Close message.
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_Comms_Close this function simply changes the current
///      state to state_SUPL_Idle which is then dealt with in
///      #SUPL_Instance_Handler().
//*****************************************************************************
void SUPL_Handle_Comms_Close( s_SUPL_Message* p_ThisMessage )
{
   void              *Handle           = p_ThisMessage->MsgHdr.Handle;
   s_SUPL_Instance   *p_SUPL_Instance;

   // If the message was directed at a specific instance, get the Instance.
   p_SUPL_Instance = SUPL_Instance_Get_Data( p_ThisMessage->MsgHdr.p_InstanceOfDest );

   if ( p_SUPL_Instance == NULL )
   {
      // Try looking up the Instance via the Handle instead.
      p_SUPL_Instance = SUPL_Instance_From_Handle( Handle );
   }

   if ( p_SUPL_Instance != NULL  &&  p_SUPL_Instance->p_SUPL != NULL )
   {
      switch ( p_SUPL_Instance->p_SUPL->State )
      {
      //-----------------------------------------------------------------------
      // Valid states.
      case state_SUPL_SUPL_START_Sent:
      case state_SUPL_SUPL_RESPONSE_Sent:
      case state_SUPL_SUPL_POS_INIT_Sent:
      case state_SUPL_Waiting_For_RRLP:
      case state_SUPL_POS_Payload_Delivered:
      case state_SUPL_SUPL_POS_Sent:
          /* In all these cases, a disconnect has happened */
          GN_SUPL_Log_ServerDisconnected( p_SUPL_Instance->Handle );
          break;
      case state_SUPL_Comms_Open_Sent:
          GN_SUPL_Log_ServerNotFound( p_SUPL_Instance->Handle );
          break;
      case state_SUPL_Comms_Close_Sent:
      case state_SUPL_SUPL_END_Sent:
         break;
      //-----------------------------------------------------------------------
      // Invalid states
      case state_SUPL_Idle:
      case state_SUPL_Server_Open_Requested:
         // Error handling.
         OS_ASSERT( 0 );
         break;
      //-----------------------------------------------------------------------
      default:
      // Coding error.
         OS_ASSERT( 0 );
         break;
      //-----------------------------------------------------------------------
      }

      SUPL_Instance_State_Transition( p_SUPL_Instance,
                                         state_SUPL_Idle,
                                         "event_SUPL_Comms_Close" );
   }

   return;
}


//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_POS_Payload message.
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_POS_Payload this function packages up the POS payload
///      delivered from the positioning subsystem in a SUPL-POS ready to be sent
///      to the SLP.
//*****************************************************************************
void SUPL_Handle_POS_Payload( s_SUPL_Message* p_ThisMessage )
{
   s_SUPL_Instance      *p_SUPL_Instance     = p_ThisMessage->MsgHdr.p_InstanceOfDest ;
   BL                   PDU_Ready_To_Send    = FALSE ;
   e_SUPL_States        Next_State ;
   void                 *p_PDU_Src           = NULL ;

   if ( p_SUPL_Instance == NULL )
   {
      // Try looking up the Instance via the Handle instead.
      p_SUPL_Instance = SUPL_Instance_From_POS_Handle( p_ThisMessage->MsgHdr.Handle );
   }

   // Start by assuming there is no state change.
   Next_State = p_SUPL_Instance->p_SUPL->State;

   switch ( p_SUPL_Instance->p_SUPL->State )
   {
   //-----------------------------------------------------------------------
   // Valid states.
   case state_SUPL_Waiting_For_RRLP:
   case state_SUPL_POS_Payload_Delivered:
      {
         s_GN_Position  GN_Position_Store;

         if ( p_ThisMessage->Payload.p_GN_AGPS_GAD_Data != NULL  &&
              p_ThisMessage->Payload.p_GN_GPS_Nav_Data  != NULL     )
         {
            memset( &GN_Position_Store, 0, sizeof(s_GN_Position) );
            GN_SUPL_Position_From_Solution( &GN_Position_Store,
                                            p_ThisMessage->Payload.p_GN_GPS_Nav_Data,
                                            p_ThisMessage->Payload.p_GN_AGPS_GAD_Data );
            GN_SUPL_Cache_Store_Position( &GN_Position_Store );
         }
      }

      p_PDU_Src = supl_PDU_Make_SUPLPOS( p_SUPL_Instance,
                                         p_ThisMessage->Payload.p_PDU,
                                         p_ThisMessage->Payload.p_GN_AGPS_GAD_Data );

      Next_State = state_SUPL_SUPL_POS_Sent ;

      PDU_Ready_To_Send = TRUE;

      GN_SUPL_Timer_Set_UT3( p_SUPL_Instance );
      break;
   //-----------------------------------------------------------------------
   // Invalid states.
   case state_SUPL_Idle:
   case state_SUPL_Comms_Open_Sent:
   case state_SUPL_SUPL_START_Sent:
   case state_SUPL_SUPL_RESPONSE_Sent:
   case state_SUPL_SUPL_POS_INIT_Sent:
   case state_SUPL_SUPL_POS_Sent:
   case state_SUPL_SUPL_END_Sent:
   case state_SUPL_Server_Open_Requested:
   case state_SUPL_Comms_Close_Sent:
      OS_ASSERT( 0 );
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
                                   "event_SUPL_POS_Payload" );
   return;
}


//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_POS_Sequence_Error message.
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_POS_Sequence_Error this function identifies the end of
///      the positioning sequence indication to the positioning subsystem the
///      sequence has ended and subsequently sending a SUPL-END.
///
///      For the client this will only happend in the event of an internal error
///      occuring.
//*****************************************************************************
void SUPL_Handle_POS_Sequence_Error( s_SUPL_Message* p_ThisMessage )
{
   s_SUPL_Instance      *p_SUPL_Instance     = p_ThisMessage->MsgHdr.p_InstanceOfDest ;
   void                 *p_PDU_Src           = NULL ;
   s_GN_AGPS_GAD_Data   *p_GN_AGPS_GAD_Data ;
   e_GN_Status          GN_Status            = GN_SUCCESS ;
   BL                   Successful ;
   BL                   PDU_Ready_To_Send    = FALSE ;
   e_SUPL_States        Next_State ;

   if ( p_SUPL_Instance == NULL )
   {
      // Try looking up the Instance via the Handle instead.
      p_SUPL_Instance = SUPL_Instance_From_Handle( p_ThisMessage->MsgHdr.Handle );
   }
   if ( p_SUPL_Instance == NULL )
       {
           return;
       }

   // Start by assuming there is no state change.
   Next_State = p_SUPL_Instance->p_SUPL->State;

   switch ( p_SUPL_Instance->p_SUPL->State )
   {
   //-----------------------------------------------------------------------
   // Valid States
   case state_SUPL_Waiting_For_RRLP:
   case state_SUPL_POS_Payload_Delivered:
      p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
      // Add optional fields to SUPL-END.
      p_GN_AGPS_GAD_Data = GN_Calloc( 1, sizeof(s_GN_AGPS_GAD_Data) );

      Successful = GN_RRLP_Sequence_End_In( p_SUPL_Instance->Handle,
                                            &GN_Status,
                                            p_GN_AGPS_GAD_Data );
      GN_Free( p_GN_AGPS_GAD_Data );

      Next_State =  state_SUPL_Comms_Close_Sent;

      PDU_Ready_To_Send = TRUE;
      break;

      // Valid states.
      break;
      // Error handling.
      break;                     // ??? Can't get here ???
      // Invalid states
   //-----------------------------------------------------------------------
   // Invalid States
   case state_SUPL_Idle:
   case state_SUPL_Comms_Open_Sent:
   case state_SUPL_SUPL_START_Sent:
   case state_SUPL_SUPL_RESPONSE_Sent:
   case state_SUPL_SUPL_POS_INIT_Sent:
   case state_SUPL_SUPL_POS_Sent:
   case state_SUPL_SUPL_END_Sent:
   case state_SUPL_Server_Open_Requested:
   case state_SUPL_Comms_Close_Sent:
      OS_ASSERT( 0 );
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
      Status = GN_SUPL_PDU_Send( p_SUPL_Instance, p_PDU_Src );
      supl_PDU_Free( p_PDU_Src );
      if ( ! Status )
      {
         // Transitions straight to Idle to save waiting for a close confirmation.
         Next_State = state_SUPL_Idle ;
      }
   }

   Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                    &GN_Status );

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_POS_Sequence_Err" );
   return;
}

//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_POS_Sequence_Complete message.
/// \details
///      Called in response to #SUPL_Message_Handler() receiving an
///      #event_SUPL_POS_Sequence_Complete this function identifies the end of
///      the positioning sequence indication to the positioning subsystem the
///      sequence has ended.
///
//*****************************************************************************
void SUPL_Handle_POS_Sequence_Complete( s_SUPL_Message* p_ThisMessage )
{
   s_SUPL_Instance      *p_SUPL_Instance     = p_ThisMessage->MsgHdr.p_InstanceOfDest ;
   e_SUPL_States        Next_State ;

   if ( p_SUPL_Instance == NULL )
   {
      // Try looking up the Instance via the Handle instead.
      p_SUPL_Instance = SUPL_Instance_From_Handle( p_ThisMessage->MsgHdr.Handle );
   }
   if ( p_SUPL_Instance == NULL )
   {
       return;
   }

   // Start by assuming there is no state change.
   Next_State = p_SUPL_Instance->p_SUPL->State;

   switch ( p_SUPL_Instance->p_SUPL->State )
   {
   //-----------------------------------------------------------------------
   // Valid States
   case state_SUPL_POS_Payload_Delivered:
     /* Dont change state here. Why is change of state required here?
         SUPL POS session needs to end with SUPL Report being sent from network which marks end of one iteration.
         Dont decrement the count as well here.
         Decrementing count in SUPL Report Handler */
      break;

      // Valid states.
      break;
      // Error handling.
      break;                     // ??? Can't get here ???
      // Invalid states
   //-----------------------------------------------------------------------
   // Invalid States
   case state_SUPL_Waiting_For_RRLP:
   case state_SUPL_Idle:
   case state_SUPL_Comms_Open_Sent:
   case state_SUPL_SUPL_START_Sent:
   case state_SUPL_SUPL_RESPONSE_Sent:
   case state_SUPL_SUPL_POS_INIT_Sent:
   case state_SUPL_SUPL_POS_Sent:
   case state_SUPL_SUPL_END_Sent:
   case state_SUPL_Server_Open_Requested:
   case state_SUPL_Comms_Close_Sent:
      OS_ASSERT( 0 );
   break;

   //-----------------------------------------------------------------------
   default:
      // Coding error.
      OS_ASSERT( 0 );
      break;
   //-----------------------------------------------------------------------
   }

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_POS_Sequence_Complete" );
   return;
}
//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Expiry_UT1 message.
//*****************************************************************************
void SUPL_Handle_Expiry_UT1( s_SUPL_Message* p_ThisMessage )
{
   s_SUPL_Instance      *p_SUPL_Instance  = p_ThisMessage->MsgHdr.p_InstanceOfDest;
   void                 *p_PDU_Src        = NULL;
   BL                   PDU_Ready_To_Send = FALSE;
   e_SUPL_States        Next_State        = p_SUPL_Instance->p_SUPL->State;
   e_GN_Status          GN_Status         = GN_SUCCESS;
   BL                   Successful;

   GN_SUPL_Log( "Inside SUPL_Handle_Expiry_UT1" );

   switch ( p_SUPL_Instance->p_SUPL->State )
   {
   //-----------------------------------------------------------------------
      // Valid states.
      break;                           // ??? Can we ever get here ???
      // Error handling.
      break;                           // ??? Can we ever get here ???
   //-----------------------------------------------------------------------
      // Invalid states
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
   case state_SUPL_Comms_Close_Sent:
   case state_SUPLv2_SUPL_TRIGGER_Active:
   case state_SUPLv2_SUPL_TRIGGER_START_Sent:
      GN_SUPL_Log_MsgTimerExpiry( p_SUPL_Instance->Handle , "UT1");
      p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
      // Allowed stati for timeouts are:
      //    GN_StatusCode_unspecified
      //    GN_StatusCode_systemFailure
      //    GN_StatusCode_posMethodFailure
      supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,
                                       GN_StatusCode_posMethodFailure );
      PDU_Ready_To_Send  =  TRUE;
      Next_State         =  state_SUPL_Comms_Close_Sent;
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
      Status = GN_SUPL_PDU_Send( p_SUPL_Instance, p_PDU_Src );
      supl_PDU_Free( p_PDU_Src );
      if ( ! Status )
      {
         // Transitions straight to Idle to save waiting for a close confirmation.
         Next_State = state_SUPL_Idle ;
      }
   }

   Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                    &GN_Status );

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_Expiry_UT1" );
   return;
}

//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Expiry_UT2 message.
//*****************************************************************************
void SUPL_Handle_Expiry_UT2(s_SUPL_Message* p_ThisMessage)
{
   s_SUPL_Instance      *p_SUPL_Instance  = p_ThisMessage->MsgHdr.p_InstanceOfDest;
   void                 *p_PDU_Src        = NULL;
   BL                   PDU_Ready_To_Send = FALSE;
   e_SUPL_States        Next_State        = p_SUPL_Instance->p_SUPL->State;
   e_GN_Status          GN_Status         = GN_SUCCESS;
   BL                   Successful;

   GN_SUPL_Log( "Inside SUPL_Handle_Expiry_UT2" );

   switch ( p_SUPL_Instance->p_SUPL->State )
   {
   //-----------------------------------------------------------------------
   // Valid states
   case state_SUPL_SUPL_POS_INIT_Sent:
   {
       if((supl_Is_Version2( p_SUPL_Instance ))&&(p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModePeriodic))
       {
            /*If UT2 timer expire abandon the POS session and wait for next timer to try again*/
            GN_SUPL_Log( "UT2 expiry: SUPLV2, Periodic Session" );
            Next_State         =  state_SUPLv2_SUPL_TRIGGER_Active;
       }
       else
       {
          p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
          // Allowed stati for timeouts are:
          // GN_StatusCode_unspecified
          // GN_StatusCode_systemFailure
          // GN_StatusCode_posMethodFailure
          GN_SUPL_Log_MsgTimerExpiry( p_SUPL_Instance->Handle , "UT2");
          supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,
                                           GN_StatusCode_posMethodFailure );
          PDU_Ready_To_Send  =  TRUE;
          Next_State         =  state_SUPL_Comms_Close_Sent;
       }
   }
   break;

   case state_SUPL_Idle:
   case state_SUPL_Comms_Open_Sent:
   case state_SUPL_SUPL_START_Sent:
   case state_SUPL_SUPL_RESPONSE_Sent:
   case state_SUPL_Waiting_For_RRLP:
   case state_SUPL_POS_Payload_Delivered:
   case state_SUPL_SUPL_POS_Sent:
   case state_SUPL_SUPL_END_Sent:
   case state_SUPL_Server_Open_Requested:
   case state_SUPL_Comms_Close_Sent:
   {
      p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
      // Allowed stati for timeouts are:
      // GN_StatusCode_unspecified
      // GN_StatusCode_systemFailure
      // GN_StatusCode_posMethodFailure
      GN_SUPL_Log_MsgTimerExpiry( p_SUPL_Instance->Handle , "UT2");
      supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,
                                       GN_StatusCode_posMethodFailure );
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
      Status = GN_SUPL_PDU_Send( p_SUPL_Instance, p_PDU_Src );
      supl_PDU_Free( p_PDU_Src );
      if ( ! Status )
      {
         // Transitions straight to Idle to save waiting for a close confirmation.
         Next_State = state_SUPL_Idle ;
      }

   Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                    &GN_Status );

   }


   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_Expiry_UT2" );

   return;
}

//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Expiry_UT3 message.
//*****************************************************************************
void SUPL_Handle_Expiry_UT3( s_SUPL_Message* p_ThisMessage )
{
   s_SUPL_Instance      *p_SUPL_Instance  = p_ThisMessage->MsgHdr.p_InstanceOfDest;
   void                 *p_PDU_Src        = NULL;
   BL                   PDU_Ready_To_Send = FALSE;
   e_GN_Status          GN_Status         = GN_SUCCESS;
   BL                   Successful;

   GN_SUPL_Log( "Inside SUPL_Handle_Expiry_UT3" );

   if((p_SUPL_Instance == NULL) || (p_SUPL_Instance->p_SUPL == NULL)) return;

   e_SUPL_States        Next_State        = p_SUPL_Instance->p_SUPL->State;

   switch( p_SUPL_Instance->p_SUPL->State )
   {
   //-----------------------------------------------------------------------
   // Valid states.
   case state_SUPL_POS_Payload_Delivered:
   case state_SUPL_SUPL_POS_Sent:
   case state_SUPLv2_SUPL_TRIGGER_Active:
   case state_SUPLv2_SUPL_TRIGGER_Suspended:


      if( supl_Is_Version2( p_SUPL_Instance ) && ( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode != GN_TriggeredModeNone ) )
      {
          GN_RRLP_Sequence_End_In( p_SUPL_Instance->POS_Handle,
                                            &GN_Status,
                                            NULL);

          p_SUPL_Instance->POS_Handle = NULL;

         /* In this case, we need to check for triggering */
            Next_State = state_SUPLv2_SUPL_TRIGGER_Active;
      }
      else
      {
          /*Immediate Modes- Session must be terminated*/
          p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
          // Allowed stati for timeouts are:
          // GN_StatusCode_unspecified
          // GN_StatusCode_systemFailure
          // GN_StatusCode_posMethodFailure
          GN_SUPL_Log_MsgTimerExpiry( p_SUPL_Instance->Handle , "UT3");
          supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,
                                           GN_StatusCode_posMethodFailure );
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
      Status = GN_SUPL_PDU_Send( p_SUPL_Instance, p_PDU_Src );
      supl_PDU_Free( p_PDU_Src );
      if ( ! Status )
      {
         // Transitions straight to Idle to save waiting for a close confirmation.
         Next_State = state_SUPL_Idle ;
      }

   Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                    &GN_Status );

   }

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_Expiry_UT3" );
   return;
}

//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Expiry_UT5 message.
//*****************************************************************************
void SUPL_V2_Handle_Expiry_UT5( s_SUPL_Message* p_ThisMessage )
{
   s_SUPL_Instance      *p_SUPL_Instance  = p_ThisMessage->MsgHdr.p_InstanceOfDest;
   void                 *p_PDU_Src        = NULL;
   BL                   PDU_Ready_To_Send = FALSE;
   e_SUPL_States        Next_State        = p_SUPL_Instance->p_SUPL->State;
   e_GN_Status          GN_Status         = GN_SUCCESS;
   BL                   Successful;

   GN_SUPL_Log( "Inside SUPL_Handle_Expiry_UT5" );

    p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
    // Allowed stati for timeouts are:
    // GN_StatusCode_unspecified
    // GN_StatusCode_systemFailure
    // GN_StatusCode_posMethodFailure
    GN_SUPL_Log_MsgTimerExpiry( p_SUPL_Instance->Handle , "UT5");
    supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,
                                   GN_StatusCode_posMethodFailure );
    PDU_Ready_To_Send  =    TRUE;
    Next_State         =    state_SUPL_Comms_Close_Sent;

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

   Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                    &GN_Status );

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_Expiry_UT5" );
   return;
}



//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Expiry_UT7 message.
//*****************************************************************************
void SUPL_V2_Handle_Expiry_UT7( s_SUPL_Message* p_ThisMessage )
{
   s_SUPL_Instance      *p_SUPL_Instance  = p_ThisMessage->MsgHdr.p_InstanceOfDest;
   void                 *p_PDU_Src        = NULL;
   BL                   PDU_Ready_To_Send = FALSE;
   e_SUPL_States        Next_State        = p_SUPL_Instance->p_SUPL->State;
   e_GN_Status          GN_Status         = GN_SUCCESS;
   BL                   Successful;

   GN_SUPL_Log( "Inside SUPL_Handle_Expiry_UT7" );

   p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
      // Allowed stati for timeouts are:
      //    GN_StatusCode_unspecified
      //    GN_StatusCode_systemFailure
      //    GN_StatusCode_posMethodFailure
   GN_SUPL_Log_MsgTimerExpiry( p_SUPL_Instance->Handle , "UT8");
   supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,
                                       GN_StatusCode_posMethodFailure );
   PDU_Ready_To_Send  =  TRUE;
   Next_State         =  state_SUPL_Comms_Close_Sent;

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

   Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                    &GN_Status );

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_Expiry_UT7" );
   return;
}


//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Expiry_UT8 message.
//*****************************************************************************
void SUPL_V2_Handle_Expiry_UT8( s_SUPL_Message* p_ThisMessage )
{
   s_SUPL_Instance      *p_SUPL_Instance  = p_ThisMessage->MsgHdr.p_InstanceOfDest;
   void                 *p_PDU_Src        = NULL;
   BL                   PDU_Ready_To_Send = FALSE;
   e_SUPL_States        Next_State        = p_SUPL_Instance->p_SUPL->State;
   e_GN_Status          GN_Status         = GN_SUCCESS;
   BL                   Successful;

   GN_SUPL_Log( "Inside SUPL_Handle_Expiry_UT8" );

    p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
    // Allowed stati for timeouts are:
    // GN_StatusCode_unspecified
    // GN_StatusCode_systemFailure
    // GN_StatusCode_posMethodFailure
    GN_SUPL_Log_MsgTimerExpiry( p_SUPL_Instance->Handle , "UT8");
    supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,
                                   GN_StatusCode_posMethodFailure );
    PDU_Ready_To_Send  =    TRUE;
    Next_State         =    state_SUPL_Comms_Close_Sent;

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

   Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                    &GN_Status );

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                               "event_SUPL_Expiry_UT8" );
   return;
}

//*****************************************************************************
/// \brief
///      Handles the #event_SUPL_Expiry_UT9 message.
//*****************************************************************************
void SUPL_V2_Handle_Expiry_UT9( s_SUPL_Message* p_ThisMessage )
{
   s_SUPL_Instance      *p_SUPL_Instance  = p_ThisMessage->MsgHdr.p_InstanceOfDest;
   void                 *p_PDU_Src        = NULL;
   BL                   PDU_Ready_To_Send = FALSE;
   e_SUPL_States        Next_State        = p_SUPL_Instance->p_SUPL->State;
   e_GN_Status          GN_Status         = GN_WRN_NO_POSITION;
   BL                   Successful;
   BL Status;

   GN_SUPL_Log( "Inside SUPL_V2_Handle_Expiry_UT9" );


   Status = GN_SUPL_ThirdPartyPosition_Resp_Out_Wrapper( p_SUPL_Instance->Handle,
                                              &GN_Status,
                                              NULL);

   p_SUPL_Instance->p_SUPL->V2_Data.ThirdParty_Retrieve = FALSE;

    p_PDU_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
    // Allowed stati for timeouts are:
    // GN_StatusCode_unspecified
    // GN_StatusCode_systemFailure
    // GN_StatusCode_posMethodFailure
    GN_SUPL_Log_MsgTimerExpiry( p_SUPL_Instance->Handle , "UT9");
    /*what status code should be sent in the SUPL_END needs to be discussed*/
    supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,
                                   GN_StatusCode_posMethodFailure );
    PDU_Ready_To_Send  =    TRUE;
    Next_State         =    state_SUPL_Comms_Close_Sent;

   if( PDU_Ready_To_Send )
   {

      Status = GN_SUPL_PDU_Send( p_SUPL_Instance, p_PDU_Src );
      supl_PDU_Free( p_PDU_Src );
      if ( ! Status )
      {
         // Transitions straight to Idle to save waiting for a close confirmation.
         Next_State = state_SUPL_Idle ;
      }
   }

   Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                    &GN_Status );

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                               "event_SUPL_V2_Expiry_UT9" );
   return;
}

//*****************************************************************************
/// \brief
///      Performs Initialisation of the SUPL Module.
//*****************************************************************************
void SUPL_Module_Init( void )
{
   SUPL_Instances_Initialise( INSTANCE_SUPL_MAX );

   memset( &SUPL_Queue, 0, sizeof(s_SUPL_Queue) );

   GN_SUPL_Cache_Initialise();

   GN_SUPL_Multiple_LocationID_Initialise();

   return;
}


//*****************************************************************************
/// \brief
///      Deinitialises the SUPL Subsystem.
//*****************************************************************************
void SUPL_SubSystem_DeInit( void )
{
   SUPL_Module_DeInit();

   return;
}


//*****************************************************************************
/// \brief
///      Deinitialises the SUPL Module.
//*****************************************************************************
void SUPL_Module_DeInit( void )
{
   SUPL_Instances_DeInitialise();

   GN_SUPL_Cache_DeInitialise();

   return;
}


//*****************************************************************************
/// \brief
///      Check each instance of the SUPL Agent for any required processing.
//*****************************************************************************
void SUPL_Instance_Handler( void )
{
   // Each existing instance is checked for actions to be taken such as polling for external events.
   U1                   i;
   BL                   ValidInstanceFound   = FALSE;
   s_GN_GPS_Nav_Data    Nav_Data;
   s_GN_AGPS_GAD_Data   GN_AGPS_GAD_Data;
   U4                   NULL_OS_Time_ms      = 0;
   U4                   GAD_Ref_TOW          = 0;
   I2                   GAD_Ref_TOW_Subms;     // GAD Data Reference GPS Time of Week Sub-millisecond part [range 0..9999 x 0.0001 ms,  -1 = Unknown]
   I4                   EFSP_dT_us;

   for ( i = 0 ; i < SUPL_Instances.MaxInstanceCount ; i++ )
   {
      if ( SUPL_Instances.InstanceList[i] != NULL )
      {
         // valid instance found in list
         SUPL_Instance_Process( SUPL_Instances.InstanceList[i] );
         ValidInstanceFound = TRUE;
      }
   }

   // Special case for SUPL client. Check for Assistance data request even when no instance.
   if ( !ValidInstanceFound ) SUPL_Instance_Process(NULL);

   // ???
   {
      s_GN_Position  GN_Position_Store;
      memset( &GN_Position_Store, 0, sizeof( s_GN_Position ) );
      GN_GPS_Get_Nav_Data_Copy(        &Nav_Data );
      GN_AGPS_Get_GAD_Data(            &NULL_OS_Time_ms,    &GAD_Ref_TOW,  &GAD_Ref_TOW_Subms, &EFSP_dT_us, &GN_AGPS_GAD_Data );
      GN_SUPL_Position_From_Solution(  &GN_Position_Store,  &Nav_Data,     &GN_AGPS_GAD_Data );
      GN_SUPL_Cache_Store_Position(    &GN_Position_Store );
   }

   return;
}

//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_SUPL_Notification_Request
///      message.
//*****************************************************************************
void SUPL_Send_Notification_Request(
   s_SUPL_Instance   *p_DestInstance,        ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance   *p_SrcInstance,         ///< [in] Entity instance where the message originated.
   void              *Handle                 ///< [in] Opaque Handle used to coordinate requests.
)
{
   s_SUPL_Message *p_MessageToSend;

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle                 = Handle;
   p_MessageToSend->Payload.MsgEvent              = event_SUPL_Notification_Request;

   SUPL_Queue_Add_Message( p_MessageToSend );

   return;
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_SUPL_Notification_Response
///      message.
//*****************************************************************************
void SUPL_Send_Notification_Response(
   s_SUPL_Instance   *p_DestInstance,        ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance   *p_SrcInstance,         ///< [in] Entity instance where the message originated.
   BL                Notification_Accepted,  ///< [in] Indicator as to whether the notification was accepted.
   void              *Handle                 ///< [in] Opaque Handle used to coordinate requests.
)
{
   s_SUPL_Message *p_MessageToSend;

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle                 = Handle;
   p_MessageToSend->Payload.MsgEvent              = event_SUPL_Notification_Response;
   p_MessageToSend->Payload.Notification_Accepted = Notification_Accepted;

   SUPL_Queue_Add_Message( p_MessageToSend );

   return;
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_SUPL_Initialisation
///      message.
//*****************************************************************************
void SUPL_Send_Initialisation(
    s_SUPL_Instance *p_DestInstance,   ///< [in] SUPL Agent instance to processthe message.
    s_SUPL_Instance *p_SrcInstance     ///< [in] Entity instance where the message originated.
)
{
   s_SUPL_Message *p_MessageToSend;

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->Payload.MsgEvent = event_SUPL_Initialisation;

   SUPL_Queue_Add_Message( p_MessageToSend );

   return;
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_SUPL_Comms_Open_Failure
///      message.
//*****************************************************************************
void SUPL_Send_Comms_Open_Failure(
   s_SUPL_Instance *p_DestInstance, ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,  ///< [in] Entity instance where the message originated.
   void            *Handle          ///< [in] Opaque Handle used to coordinate requests.
)
{
   s_SUPL_Message *p_MessageToSend;

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle    = Handle;
   p_MessageToSend->Payload.MsgEvent = event_SUPL_Comms_Open_Failure;

   SUPL_Queue_Add_Message( p_MessageToSend );

   return;
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_SUPL_Comms_Open_Success
///      message.
//*****************************************************************************
void SUPL_Send_Comms_Open_Success(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void            *Handle             ///< [in] Opaque Handle used to coordinate requests.
)
{
   s_SUPL_Message *p_MessageToSend;

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle    = Handle;
   p_MessageToSend->Payload.MsgEvent = event_SUPL_Comms_Open_Success;

   SUPL_Queue_Add_Message( p_MessageToSend );

   return;
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_SUPL_Comms_Close
///      message.
//*****************************************************************************
void SUPL_Send_Comms_Close(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void            *Handle             ///< [in] Opaque Handle used to coordinate requests.

)
{
   s_SUPL_Message *p_MessageToSend;

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle    = Handle;
   p_MessageToSend->Payload.MsgEvent = event_SUPL_Comms_Close;

   SUPL_Queue_Add_Message( p_MessageToSend );

   return;
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_SUPL_PDU_Received
///      message.
//*****************************************************************************
void SUPL_Send_PDU_Received(
   s_SUPL_Instance    *p_DestInstance, ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance    *p_SrcInstance,  ///< [in] Entity instance where the message originated.
   s_PDU_Buffer_Store *p_PDU,          ///< [in] Pointer to the PDU to send.
   U4                  PDU_TimeStamp,  ///< [in] OS Times stamp in milliseconds.
   void               *Handle          ///< [in] Opaque Handle used to coordinate requests.
)
{
   s_SUPL_Message *p_MessageToSend;

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle         = Handle;
   p_MessageToSend->Payload.MsgEvent      = event_SUPL_PDU_Received;
   p_MessageToSend->Payload.p_PDU         = p_PDU;
   p_MessageToSend->Payload.PDU_TimeStamp = PDU_TimeStamp;

   SUPL_Queue_Add_Message( p_MessageToSend );

   return;
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_SUPL_POS_Payload
///      message.
//*****************************************************************************
void SUPL_Send_POS_Payload(
   s_SUPL_Instance    *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance    *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void               *Handle,            ///< [in] Opaque Handle used to coordinate requests.
   s_PDU_Buffer_Store *p_PDU,             ///< [in] Pointer to the PDU to send.
   s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data,///< [in] Pointer to position solution (NULL is not present).
   s_GN_GPS_Nav_Data  *p_GN_GPS_Nav_Data  ///< [in] Pointer to navigation solution (NULL is not present).
)
{
   s_SUPL_Message *p_MessageToSend;

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle    = Handle;
   p_MessageToSend->Payload.MsgEvent = event_SUPL_POS_Payload;
   p_MessageToSend->Payload.p_PDU    = p_PDU;

   if ( p_GN_AGPS_GAD_Data != NULL )
   {
      p_MessageToSend->Payload.p_GN_AGPS_GAD_Data = GN_Calloc( 1, sizeof( s_GN_AGPS_GAD_Data ) );
      memcpy( p_MessageToSend->Payload.p_GN_AGPS_GAD_Data, p_GN_AGPS_GAD_Data, sizeof( s_GN_AGPS_GAD_Data ) );
   }

   if ( p_GN_GPS_Nav_Data != NULL )
   {
      p_MessageToSend->Payload.p_GN_GPS_Nav_Data = GN_Calloc( 1, sizeof( s_GN_GPS_Nav_Data ) );
      memcpy( p_MessageToSend->Payload.p_GN_GPS_Nav_Data, p_GN_GPS_Nav_Data, sizeof( s_GN_GPS_Nav_Data ) );
   }

   SUPL_Queue_Add_Message( p_MessageToSend );

   return;
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_SUPL_POS_Sequence_Complete
///      message.
//*****************************************************************************
void SUPL_Send_POS_Sequence_Complete(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void            *Handle             ///< [in] Opaque Handle used to coordinate requests.
)
{
   s_SUPL_Message *p_MessageToSend;

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle    = Handle;
   p_MessageToSend->Payload.MsgEvent = event_SUPL_POS_Sequence_Complete;

   SUPL_Queue_Add_Message( p_MessageToSend );

   return;
}

//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_SUPL_POS_Sequence_Error
///      message.
//*****************************************************************************
void SUPL_Send_POS_Sequence_Error(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void            *Handle             ///< [in] Opaque Handle used to coordinate requests.
)
{
   s_SUPL_Message *p_MessageToSend;

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle    = Handle;
   p_MessageToSend->Payload.MsgEvent = event_SUPL_POS_Sequence_Error;

   SUPL_Queue_Add_Message( p_MessageToSend );

   return;
}

//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_SUPL_MO_Position_Requested
///      message.
//*****************************************************************************
void SUPL_Send_MO_Position_Requested(
   s_SUPL_Instance               *p_DestInstance,              ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance               *p_SrcInstance,               ///< [in] Entity instance where the message originated.
   void                          *Handle,                      ///< [in] Opaque Handle used to coordinate requests.
   s_GN_SUPL_Connect_Req_Parms   *p_GN_SUPL_Connect_Req_Parms  ///< [in] Connection request parameters.
)
{
   s_SUPL_Message *p_MessageToSend;

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle     = Handle;
   p_MessageToSend->Payload.MsgEvent  = event_SUPL_MO_Position_Requested;
   p_MessageToSend->Payload.p_GN_SUPL_Connect_Req_Parms = GN_Calloc( 1, sizeof( s_GN_SUPL_Connect_Req_Parms ) );

   memcpy( p_MessageToSend->Payload.p_GN_SUPL_Connect_Req_Parms,
           p_GN_SUPL_Connect_Req_Parms,
           sizeof( s_GN_SUPL_Connect_Req_Parms ) );

   SUPL_Queue_Add_Message( p_MessageToSend );

   return;
}

/* +LMSqc38060 */
//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_SUPL_PDU_Received
///      message.
//*****************************************************************************
void SUPL_Send_Abort_Request(
   s_SUPL_Instance    *p_DestInstance, ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance    *p_SrcInstance,  ///< [in] Entity instance where the message originated.
   void               *Handle          ///< [in] Opaque Handle used to coordinate requests.
)
{
   s_SUPL_Message *p_MessageToSend;

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle         = Handle;
   p_MessageToSend->Payload.MsgEvent      = event_SUPL_Abort_Requested;

   SUPL_Queue_Add_Message( p_MessageToSend );

   return;
}
/* -LMSqc38060 */

//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_SUPL_PDU_Received
///      message.
//*****************************************************************************
void SUPL_Send_Trigger_End_Request(
   s_SUPL_Instance    *p_DestInstance, ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance    *p_SrcInstance,  ///< [in] Entity instance where the message originated.
   void               *Handle          ///< [in] Opaque Handle used to coordinate requests.
)
{
   s_SUPL_Message *p_MessageToSend;

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle         = Handle;
   p_MessageToSend->Payload.MsgEvent      = event_SUPL_Trigger_End_Requested;

   SUPL_Queue_Add_Message( p_MessageToSend );

   return;
}


//*****************************************************************************
/// \brief
///      Function to set (ie start/re-start count down) User Timer (UT1).
//*****************************************************************************
void GN_SUPL_Timer_Set_UT1
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be set.
)
{
   U4 Duration_ms = supl_config_get_SUPL_UT1();

   GN_SUPL_Log( "Inside GN_SUPL_Timer_Set_UT1 %d ms", Duration_ms );

   GN_SUPL_Timer_Set( &p_SUPL_Instance->p_SUPL->UserTimers.UT1, Duration_ms );

   return;
}

//*****************************************************************************
/// \brief
///      Function to clear (ie kill off) User Timer (UT1).
//*****************************************************************************
void GN_SUPL_Timer_Clear_UT1
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be cleared.
)
{
   GN_SUPL_Log( "Inside GN_SUPL_Timer_Clear_UT1" );

   GN_SUPL_Timer_Clear( &p_SUPL_Instance->p_SUPL->UserTimers.UT1 );
   return;
}


//*****************************************************************************
/// \brief
///      Function to check for the expiry of a User Timer (UT1) and send an
///      #event_SUPL_Expiry_UT1 message if it has expired.
///
/// \returns Flag to indicate if the timer had expired.
//*****************************************************************************
BL GN_SUPL_Timer_Expired_UT1
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance to check for timer expiry
)
{
   s_SUPL_Message *p_MessageToSend;
   BL             UT1_Expired;

   UT1_Expired = GN_SUPL_Timer_Expired( &p_SUPL_Instance->p_SUPL->UserTimers.UT1 );

   if ( UT1_Expired )
   {
      p_MessageToSend = SUPL_Message_Init( p_SUPL_Instance, p_SUPL_Instance );

      p_MessageToSend->Payload.MsgEvent = event_SUPL_Expiry_UT1;

      SUPL_Queue_Add_Message( p_MessageToSend );
   }

   return UT1_Expired;
}


//*****************************************************************************
/// \brief
///      Function to set (ie start/re-start count down) User Timer (UT2).
//*****************************************************************************
void GN_SUPL_Timer_Set_UT2
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be set.
)
{
   U4 Duration_ms = supl_config_get_SUPL_UT2();

   GN_SUPL_Log( "Inside GN_SUPL_Timer_Set_UT2 %d ms", Duration_ms );

   GN_SUPL_Timer_Set( &p_SUPL_Instance->p_SUPL->UserTimers.UT2, Duration_ms );

   return;
}


//*****************************************************************************
/// \brief
///      Function to clear (ie kill off) User Timer (UT2).
//*****************************************************************************
void GN_SUPL_Timer_Clear_UT2
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be cleared.
)
{
   GN_SUPL_Log( "Inside GN_SUPL_Timer_Clear_UT2" );

   GN_SUPL_Timer_Clear( &p_SUPL_Instance->p_SUPL->UserTimers.UT2 );

   return;
}


//*****************************************************************************
/// \brief
///      Function to check for the expiry of a User Timer (UT2) and send an
///      #event_SUPL_Expiry_UT2 message if it has expired.
///
/// \returns Flag to indicate if the timer had expired.
//*****************************************************************************
BL GN_SUPL_Timer_Expired_UT2
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance to check for timer expiry
)
{
   s_SUPL_Message *p_MessageToSend;
   BL             UT2_Expired;

   UT2_Expired = GN_SUPL_Timer_Expired( &p_SUPL_Instance->p_SUPL->UserTimers.UT2 );

   if ( UT2_Expired )
   {
      p_MessageToSend = SUPL_Message_Init( p_SUPL_Instance, p_SUPL_Instance );

      p_MessageToSend->Payload.MsgEvent = event_SUPL_Expiry_UT2;

      SUPL_Queue_Add_Message( p_MessageToSend );
   }

   return UT2_Expired;
}


//*****************************************************************************
/// \brief
///      Function to set (ie start/re-start count down) User Timer (UT3).
//*****************************************************************************
void GN_SUPL_Timer_Set_UT3
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be set.
)
{
   U4 Duration_ms = supl_config_get_SUPL_UT3();

   GN_SUPL_Log( "Inside GN_SUPL_Timer_Set_UT3 %d ms", Duration_ms );

   GN_SUPL_Timer_Set( &p_SUPL_Instance->p_SUPL->UserTimers.UT3, Duration_ms );

   return;
}


//*****************************************************************************
/// \brief
///      Function to clear (ie kill off) User Timer (UT3)
//*****************************************************************************
void GN_SUPL_Timer_Clear_UT3
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be cleared.
)
{
   GN_SUPL_Log( "Inside GN_SUPL_Timer_Clear_UT3" );

   GN_SUPL_Timer_Clear( &p_SUPL_Instance->p_SUPL->UserTimers.UT3 );

   return;
}


//*****************************************************************************
/// \brief
///      Function to check for the expiry of a User Timer (UT3) and send an
///      #event_SUPL_Expiry_UT3 message if it has expired.
///
/// \returns Flag to indicate if the timer had expired.
//*****************************************************************************
BL GN_SUPL_Timer_Expired_UT3
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance to check for timer expiry
)
{
   s_SUPL_Message *p_MessageToSend;
   BL             UT3_Expired;

   UT3_Expired = GN_SUPL_Timer_Expired( &p_SUPL_Instance->p_SUPL->UserTimers.UT3 );

   if ( UT3_Expired )
   {
      p_MessageToSend = SUPL_Message_Init( p_SUPL_Instance, p_SUPL_Instance );

      p_MessageToSend->Payload.MsgEvent = event_SUPL_Expiry_UT3;

      SUPL_Queue_Add_Message( p_MessageToSend );
   }

   return UT3_Expired;
}


//*****************************************************************************
/// \brief
///      Function to set (ie start/re-start count down) User Timer (UT5).
//*****************************************************************************
void GN_SUPL_V2_Timer_Set_UT5
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be set.
)
{
   U4 Duration_ms = supl_config_get_SUPL_UT5();

   GN_SUPL_Log( "Inside GN_SUPL_V2_Timer_Set_UT5 %d ms", Duration_ms );

   GN_SUPL_Timer_Set( &p_SUPL_Instance->p_SUPL->V2_Data.UserTimers.UT5, Duration_ms );

   return;
}


//*****************************************************************************
/// \brief
///      Function to clear (ie kill off) User Timer (UT5)
//*****************************************************************************
void GN_SUPL_V2_Timer_Clear_UT5
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be cleared.
)
{
   GN_SUPL_Log( "Inside GN_SUPL_V2_Timer_Clear_UT5" );

   GN_SUPL_Timer_Clear( &p_SUPL_Instance->p_SUPL->V2_Data.UserTimers.UT5 );

   return;
}



//*****************************************************************************
/// \brief
///      Function to check for the expiry of a User Timer (UT5) and send an
///      #event_SUPL_V2_Expiry_UT5 message if it has expired.
///
/// \returns Flag to indicate if the timer had expired.
//*****************************************************************************
BL GN_SUPL_V2_Timer_Expired_UT5
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance to check for timer expiry
)
{
   s_SUPL_Message *p_MessageToSend;
   BL             UT_Expired;

   UT_Expired = GN_SUPL_Timer_Expired( &p_SUPL_Instance->p_SUPL->V2_Data.UserTimers.UT5 );

   if ( UT_Expired )
   {
      p_MessageToSend = SUPL_Message_Init( p_SUPL_Instance, p_SUPL_Instance );

      p_MessageToSend->Payload.MsgEvent = event_SUPL_V2_Expiry_UT5;

      SUPL_Queue_Add_Message( p_MessageToSend );
   }

   return UT_Expired;
}




//*****************************************************************************
/// \brief
///      Function to set (ie start/re-start count down) User Timer (UT7).
//*****************************************************************************
void GN_SUPL_V2_Timer_Set_UT7
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be set.
)
{
   U4 Duration_ms = supl_config_get_SUPL_UT7();

   GN_SUPL_Log( "Inside GN_SUPL_V2_Timer_Set_UT7 %d ms", Duration_ms );

   GN_SUPL_Timer_Set( &p_SUPL_Instance->p_SUPL->V2_Data.UserTimers.UT7, Duration_ms );

   return;
}


//*****************************************************************************
/// \brief
///      Function to clear (ie kill off) User Timer (UT7)
//*****************************************************************************
void GN_SUPL_V2_Timer_Clear_UT7
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be cleared.
)
{
   GN_SUPL_Log( "Inside GN_SUPL_V2_Timer_Clear_UT7" );

   GN_SUPL_Timer_Clear( &p_SUPL_Instance->p_SUPL->V2_Data.UserTimers.UT7 );

   return;
}


//*****************************************************************************
/// \brief
///      Function to check for the expiry of a User Timer (UT7) and send an
///      #event_SUPL_V2_Expiry_UT7 message if it has expired.
///
/// \returns Flag to indicate if the timer had expired.
//*****************************************************************************
BL GN_SUPL_V2_Timer_Expired_UT7
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance to check for timer expiry
)
{
   s_SUPL_Message *p_MessageToSend;
   BL             UT_Expired;

   UT_Expired = GN_SUPL_Timer_Expired( &p_SUPL_Instance->p_SUPL->V2_Data.UserTimers.UT7 );

   if ( UT_Expired )
   {
      p_MessageToSend = SUPL_Message_Init( p_SUPL_Instance, p_SUPL_Instance );

      p_MessageToSend->Payload.MsgEvent = event_SUPL_V2_Expiry_UT7;

      SUPL_Queue_Add_Message( p_MessageToSend );
   }

   return UT_Expired;
}


//*****************************************************************************
/// \brief
///      Function to set (ie start/re-start count down) User Timer (UT8).
//*****************************************************************************
void GN_SUPL_V2_Timer_Set_UT8
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be set.
)
{
   U4 Duration_ms = supl_config_get_SUPL_UT8();

   GN_SUPL_Log( "Inside GN_SUPL_V2_Timer_Set_UT8 %d ms", Duration_ms );

   GN_SUPL_Timer_Set( &p_SUPL_Instance->p_SUPL->V2_Data.UserTimers.UT8, Duration_ms );

   return;
}


//*****************************************************************************
/// \brief
///      Function to clear (ie kill off) User Timer (UT8)
//*****************************************************************************
void GN_SUPL_V2_Timer_Clear_UT8
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be cleared.
)
{
   GN_SUPL_Log( "Inside GN_SUPL_V2_Timer_Clear_UT8" );

   GN_SUPL_Timer_Clear( &p_SUPL_Instance->p_SUPL->V2_Data.UserTimers.UT8 );

   return;
}


//*****************************************************************************
/// \brief
///      Function to check for the expiry of a User Timer (UT8) and send an
///      #event_SUPL_V2_Expiry_UT8 message if it has expired.
///
/// \returns Flag to indicate if the timer had expired.
//*****************************************************************************
BL GN_SUPL_V2_Timer_Expired_UT8
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance to check for timer expiry
)
{
   s_SUPL_Message *p_MessageToSend;
   BL             UT_Expired;

   UT_Expired = GN_SUPL_Timer_Expired( &p_SUPL_Instance->p_SUPL->V2_Data.UserTimers.UT8 );

   if ( UT_Expired )
   {
      p_MessageToSend = SUPL_Message_Init( p_SUPL_Instance, p_SUPL_Instance );

      p_MessageToSend->Payload.MsgEvent = event_SUPL_V2_Expiry_UT8;

      SUPL_Queue_Add_Message( p_MessageToSend );
   }

   return UT_Expired;
}


//*****************************************************************************
/// \brief
///      Function to set (ie start/re-start count down) User Timer (UT9).
//*****************************************************************************
void GN_SUPL_V2_Timer_Set_UT9
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be set.
)
{
   U4 Duration_ms = supl_config_get_SUPL_UT9();

   GN_SUPL_Log( "Inside GN_SUPL_V2_Timer_Set_UT9 %d ms", Duration_ms );

   GN_SUPL_Timer_Set( &p_SUPL_Instance->p_SUPL->V2_Data.UserTimers.UT9, Duration_ms );

   return;
}


//*****************************************************************************
/// \brief
///      Function to clear (ie kill off) User Timer (UT9).
//*****************************************************************************
void GN_SUPL_V2_Timer_Clear_UT9
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance for which the timer is to be cleared.
)
{
   GN_SUPL_Log( "Inside GN_SUPL_V2_Timer_Clear_UT9" );

   GN_SUPL_Timer_Clear(&p_SUPL_Instance->p_SUPL->V2_Data.UserTimers.UT9 );
   return;
}


//*****************************************************************************
/// \brief
///      Function to check for the expiry of a User Timer (UT9) and send an
///      #event_SUPL_V2_Expiry_UT9 message if it has expired.
///
/// \returns Flag to indicate if the timer had expired.
//*****************************************************************************
BL GN_SUPL_V2_Timer_Expired_UT9
(
   s_SUPL_Instance *p_SUPL_Instance    ///< [in] Instance to check for timer expiry
)
{
   s_SUPL_Message *p_MessageToSend;
   BL             UT9_Expired;

   UT9_Expired = GN_SUPL_Timer_Expired( &p_SUPL_Instance->p_SUPL->V2_Data.UserTimers.UT9 );

   if ( UT9_Expired )
   {
      p_MessageToSend = SUPL_Message_Init( p_SUPL_Instance, p_SUPL_Instance );

      GN_SUPL_Log( "Posted the event_SUPL_V2_Expiry_UT9 for expiry of the event " );

      p_MessageToSend->Payload.MsgEvent = event_SUPL_V2_Expiry_UT9;

      SUPL_Queue_Add_Message( p_MessageToSend );
   }

   return UT9_Expired;
}



//*****************************************************************************
/// \brief
///      Function to Initialise the SUPL position cache.
/// \details
///      This function clears the memory associated with the SUPL Position Cache.
/// \returns
///      Nothing.
//*****************************************************************************
static void GN_SUPL_Cache_Initialise( void )
{
   memset( &GN_Position_Cache, 0, sizeof( s_GN_Position ) );
   return;
}
//*****************************************************************************
/// \brief
///      Function to Initialise the SUPL multiple location id data store.
/// \details
///      This function clears the memory associated with the multiple location id data store.
/// \returns
///      Nothing.
//*****************************************************************************
static void GN_SUPL_Multiple_LocationID_Initialise( void )
{
   SUPL_Mlid_FirstElem = NULL;
   return;
}


//*****************************************************************************
/// \brief
///      Function to De-Initialise the SUPL position cache.
/// \details
///      This function does nothing as there is no memory to free up.
/// \returns
///      Nothing.
//*****************************************************************************
static void GN_SUPL_Cache_DeInitialise( void )
{
   // Nothing to do.
   return;
}

//*****************************************************************************
/// \brief
///      Function to log a SUPL position.
/// \returns
///      Nothing.
//*****************************************************************************
static void GN_SUPL_Position_Log
(
   CH             *LogPrefix
)
{
   {
      R4 TempLat, TempLong, TempAlt;
      CH LogString[255];

      // Calculate Latitude in degrees.
      TempLat = (R4) ( GN_Position_Cache.latitude & GAD_LATITUDE_RANGE_MASK ) /
                           GAD_LATITUDE_SCALING_FACTOR;
      TempLat *= (R4) GAD_LATITUDE_DEGREES_IN_RANGE *
                           ( GN_Position_Cache.latitudeSign == 0 ? 1 : -1 );

      // Calculate Longitude in degrees.
      TempLong = (R4) ( GN_Position_Cache.longitude / (R4) GAD_LONGITUDE_SCALING_FACTOR );
      TempLong *= (R4) GAD_LONGITUDE_DEGREES_IN_RANGE;

      // Calculate Altitude in signed metres.
      TempAlt = (R4) ( GN_Position_Cache.altitude & GAD_ALTITUDE_RANGE_MASK ) *
                              (GN_Position_Cache.altitudeDirection == 0 ? 1 : -1) ;

      sprintf(LogString, "%s: Timestamp: %s, Lat: %f [%d (%s)], Long: %f [%d], Alt: %.2f m [%d (%s)]",
         LogPrefix,
         GN_Position_Cache.timestamp,
         TempLat,  GN_Position_Cache.latitude, GN_Position_Cache.latitudeSign      == 0 ? "north" : "south",
         TempLong, GN_Position_Cache.longitude,
         TempAlt,  GN_Position_Cache.altitude, GN_Position_Cache.altitudeDirection == 0 ? "height" : "depth" );

      GN_SUPL_Log( LogString );

   }
}
//*****************************************************************************
/// \brief
///      Function to store a SUPL position in the position cache.
/// \returns
///      Nothing.
//*****************************************************************************
static void GN_SUPL_Cache_Store_Position
(
   s_GN_Position *p_GN_Position  /// [in] Position to cache.
)
{
   /// Check first for a position being passed in.
   if ( p_GN_Position != NULL )
   {
      /// Then check if the position is valid i.e. at least a valid 2D fix.
      if ( p_GN_Position->Valid_2D_Fix )
      {
         /// Then check that we haven't already stored this position.
         if ( p_GN_Position->OS_Time_ms != GN_Position_Cache.OS_Time_ms )
         {
            /// Then store the position in the cache.
            memcpy( &GN_Position_Cache, p_GN_Position, sizeof( s_GN_Position ) );
         }
      }
   }
   return;
}


//*****************************************************************************
/// \brief
///      Function to retrieve a SUPL position from the cache.
/// \returns
///      Flag to indicate whether a position was able to be retrieved.
/// \retval #TRUE Flag indicating success.
/// \retval #FALSE Flag indicating failure.
//*****************************************************************************
static BL GN_SUPL_Cache_Retrieve_Position
(
   s_GN_Position *p_GN_Position  /// [out] Cached Position.
)
{
   if ( p_GN_Position != NULL )
   {
      /// Check whether we have a valid position in the cache.
      if ( p_GN_Position->Valid_2D_Fix )
      {
         /// Then we copy the cached position to the output.
         memcpy( p_GN_Position, &GN_Position_Cache, sizeof( s_GN_Position ) );
         GN_SUPL_Position_Log( "GN_SUPL_Cache_Retrieve_Position" );
         return TRUE;
      }
   }

   return FALSE;
}


//*****************************************************************************
/// \brief
///      Function to retrieve a SUPL position from the cache if the position matches QoP.
/// \returns
///      Flag to indicate whether a position was able to be retrieved.
/// \retval #TRUE Flag indicating success.
/// \retval #FALSE Flag indicating failure.
//*****************************************************************************
BL GN_SUPL_Cache_Valid
(
   s_GN_Position  *p_GN_Position,   /// [out] Cached Position.
   s_GN_QoP       *p_GN_QoP         /// [in] Quality Criteria.
)
{
   /// First we check that Quality criteria are available.
   if ( p_GN_QoP == NULL )
   {
      return FALSE;
   }

   /// Then we check that we have a valid fix in the cache.
   if ( ! p_GN_Position->Valid_2D_Fix )
   {
      return FALSE;
   }

   /// Then we check the against horizontal accuracy if provided.
   if ( p_GN_QoP->p_horacc != NULL )
   {
      if ( GN_Position_Cache.uncertaintySemiMajor > p_GN_QoP->horacc )
      {
         return FALSE;
      }
   }

   /// Then we check the against vertical accuracy if provided.
   if ( p_GN_QoP->p_veracc != NULL )
   {
      // If we don't have a 3d fix, we can't meet a vertical accuracy.
      if ( !GN_Position_Cache.altitudeInfoPresent )
      {
         return FALSE;
      }
      else
      {
         if ( GN_Position_Cache.altUncertainty > p_GN_QoP->veracc )
         {
            return FALSE;
         }
      }
   }

   /// Then we check the against maximum location age if provided.
   if ( p_GN_QoP->p_maxLocAge != NULL )
   {
      U4 Current_OS_Time;
      U4 Time_Elapsed;

      Current_OS_Time = GN_GPS_Get_OS_Time_ms();

      if ( Current_OS_Time < GN_Position_Cache.OS_Time_ms )
      {
         Time_Elapsed = Current_OS_Time + ( 0xffffffff - GN_Position_Cache.OS_Time_ms );
      }
      else
      {
         Time_Elapsed = Current_OS_Time - GN_Position_Cache.OS_Time_ms;
      }
      if ( Time_Elapsed/1000 > p_GN_QoP->maxLocAge )
      {
         return FALSE;
      }
   }

   // If all the checks pass we retrieve the position.
   return GN_SUPL_Cache_Retrieve_Position( p_GN_Position );
}


//*****************************************************************************
/// \brief
///      Function to encode and send a SUPL PDU.
/// \details
///      This function encodes a PDU and sends it out, but if a failure occurs
///      a SUPL-END with error is generated and sent out instead. In the case
///      where the SUPL-END with error is not encoded correctly we just
///      disconnect.
/// \returns
///      Flag to indicate success or failure of the sending of the PDU.
/// \retval #TRUE Flag indicating success.
/// \retval #FALSE Flag indicating failure.
//*****************************************************************************
BL GN_SUPL_PDU_Send
(
   s_SUPL_Instance *p_SUPL_Instance,      ///< [in] The SUPL instance dealing with this PDU.
   void *p_PDU_Src                        ///< [in] Pointer to generated SUPL structure
)
{
   e_GN_Status    GN_Status;
   BL             Status = TRUE;

   s_PDU_Buffer_Store  *p_PDU_Buf  = asn1_PDU_Buffer_Store_Alloc( 0 );

   if ( supl_PDU_Encode( p_SUPL_Instance->Handle , p_PDU_Src, p_PDU_Buf ) )
   {
      // On successful encoding of the PDU send the encoded PDU.
      GN_SUPL_PDU_Delivery_Out_Wrapper( p_SUPL_Instance->Handle,
                                        &GN_Status,
                                        p_PDU_Buf->PDU_Encoded.Length,
                                        p_PDU_Buf->PDU_Encoded.p_PDU_Data );

      // Free up the buffer.
      asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
      GN_SUPL_Log( "Encoding Sucess" );   //RaghavTODO Temp
   }
   else
   {
      BL    Successful;
      void  *p_PDU_Error_Src;

      // Something went wrong with the encode. Indicate a system failure in a SUPL-END.
      GN_SUPL_Log( "Encoding Failed - Sending SUPL END" );

      asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
      p_PDU_Buf = asn1_PDU_Buffer_Store_Alloc( 0 );
      p_PDU_Error_Src = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
      supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Error_Src, GN_StatusCode_systemFailure );

      if ( supl_PDU_Encode( p_SUPL_Instance->Handle , p_PDU_Error_Src, p_PDU_Buf ) )
      {
         // On successful encoding of the PDU send the encoded PDU.
         GN_SUPL_PDU_Delivery_Out_Wrapper( p_SUPL_Instance->Handle,
                                           &GN_Status,
                                           p_PDU_Buf->PDU_Encoded.Length,
                                           p_PDU_Buf->PDU_Encoded.p_PDU_Data );

         asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
         supl_PDU_Free( p_PDU_Error_Src );
      }

      // Even if the encode of the error message worked, still indicate failure.
      GN_Status = GN_ERR_NO_RESOURCE;

      Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                       &GN_Status );

      Status = FALSE;
   }
   asn1_PDU_Buffer_Store_Free( &p_PDU_Buf );
   return Status;
}


//*****************************************************************************
/// \brief
///      Function to handle SUPL Notify PDU.
/// \details
///      This function handles and encodes the SUPL Notify PDU
/// \returns
///      None
//*****************************************************************************
void SUPL_Handle_PDU_SUPL_NOTIFY
(
   void            *p_PDU_Decoded,     ///< [in] Pointer to a decoded pdu.
   s_SUPL_Instance *p_SUPL_Instance    ///< [in/out] The SUPL instance dealing with this PDU.
)
{
    e_SUPL_States      Next_State                 = p_SUPL_Instance->p_SUPL->State;

    if ( supl_PDU_Get_SUPLNOTIFY_Notification( p_PDU_Decoded,
                                             &p_SUPL_Instance->p_SUPL->SUPL_Notification ) )
    {
       // if a notification request was present in the SUPL-INIT this must be checked with the user.
       SUPL_Send_Notification_Request( p_SUPL_Instance, p_SUPL_Instance, p_SUPL_Instance->Handle );

       Next_State = state_SUPLv2_Location_Notification;
    }
    else
    {
       p_SUPL_Instance->p_SUPL->Notification_Accepted = TRUE;
    }

    SUPL_Instance_State_Transition( p_SUPL_Instance,
                               Next_State,
                               "event_SUPL_PDU_Received(SUPL-NOTIFY)" );

}



void SUPL_Handle_PDU_SUPL_REPORT
(
   void            *p_PDU_Decoded,     ///< [in] Pointer to a decoded pdu.
   s_SUPL_Instance *p_SUPL_Instance    ///< [in/out] The SUPL instance dealing with this PDU.
)
{
    /* RaghavTODO: This is for SI MSA where the report need to be sent to application
        Other than that it will be used for :
        - Batch rerporting: Historical reporting or Enhanced Cell/Sector*/
       void                *p_PDU_Src             = NULL;
       BL                    PDU_Ready_To_Send     = FALSE;
       e_SUPL_States        Next_State             = p_SUPL_Instance->p_SUPL->State;
       static U1 Number_of_Reports_received = 0;

       GN_SUPL_Log( "Inside SUPL_Handle_PDU_SUPL_REPORT" );

       /// Perform initial validation checks and return a SUPL-END with an error code if they fail.
       if ( ! supl_PDU_Validate_setSessionID(  p_PDU_Decoded,
                                               p_SUPL_Instance->p_SUPL->SetSessionId,
                                              &p_SUPL_Instance->p_SUPL->SetId ) )
       {
          // If the Set Session ID is invalid, send a SUPL-END with the incorrect ID.
          U2          SessionID;
          s_SetId      SetId;

          p_PDU_Src          =    supl_PDU_Make_SUPLEND( p_SUPL_Instance );
          supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,       GN_StatusCode_invalidSessionId );
          supl_PDU_Get_setSessionID(       p_PDU_Decoded, &SessionID, &SetId );
          supl_PDU_Add_setSessionID(       p_PDU_Src,       SessionID, &SetId );
          PDU_Ready_To_Send  =    TRUE;
          Next_State         =    state_SUPL_Comms_Close_Sent;
       }

       else if ( ! supl_PDU_Validate_slpSessionID(    p_PDU_Decoded,
                                                    p_SUPL_Instance->p_SUPL->p_SlpSessionId,
                                                   &p_SUPL_Instance->p_SUPL->SlpId ) )
       {
          // If the Set Session ID is invalid, send a SUPL-END with the incorrect ID.
          U1               SessionID[4];
          U1               *p_SessionID = SessionID;
          s_GN_SLPAddress  SlpId;

          p_PDU_Src          =    supl_PDU_Make_SUPLEND( p_SUPL_Instance );
          supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,       GN_StatusCode_invalidSessionId );
          supl_PDU_Get_slpSessionID(       p_PDU_Decoded, &p_SessionID, &SlpId );
          supl_PDU_Add_slpSessionID(       p_PDU_Src,       p_SessionID, &SlpId );

          PDU_Ready_To_Send  =    TRUE;
          Next_State         =    state_SUPL_Comms_Close_Sent;
       }
       else
       {
          switch ( p_SUPL_Instance->p_SUPL->State )
          {
          //-----------------------------------------------------------------------
          // Valid states.
          case state_SUPL_SUPL_POS_INIT_Sent:
          case state_SUPL_SUPL_POS_Sent:
/* 6 cases
1) SUPL SI - MSB Periodic / MSB Area Triggered
        Position updated to application.
        If POS session started, no positon received in SUPL_REPORT.

2) SUPL SI - MSA Periodic / MSA Area Triggered
        POS session is always started, positon received in SUPL_REPORT.

3) SUPL NI - MSB Periodic
        Positition sent by SET in SUPL REPORT previously
        If POS session started, no positon received in SUPL_REPORT, only decrement number of fixes.

4) SUPL NI - MSA Periodic
        POS session is always started, positon received in SUPL_REPORT, only decrement number of fixes.

6) SUPL NI - MSB Area Triggered
    Positition sent in SUPL REPORT after check for area event.

    If POS session is started, no positon received in SUPL_REPORT.
            Check for event and sent SUPL_REPORT with position decrementing the number of fixes

7) SUPL NI - MSA Area Triggered
        POS session is always started, positon received in SUPL_REPORT.
            Check for event and sent SUPL_REPORT with position decrementing the number of fixes
*/

             {
                BL                  Status;
                BL                  SUPL_Report_To_Send  =  FALSE;
                s_GN_Position       GN_Position_Store;
                s_GN_AGPS_GAD_Data  GN_AGPS_GAD_Data;
                s_GN_SUPL_Pos_Data  GN_SUPL_Pos_Data;
                e_GN_Status         GN_Status;
                s_GN_Position       Supl_Report_Position_Store;


                // The exchange with the Supl Positioning subsystem did not result in a fix.
                // Check the SUPL-END for a position.
                Number_of_Reports_received++;//This count will say the number of reports the SLP had sent to SET

                Status = supl_PDU_Get_SUPLREPORT_GAD_Position( p_PDU_Decoded, &GN_Position_Store, &GN_AGPS_GAD_Data );
                GN_SUPL_Log( "supl_PDU_Get_SUPLREPORT_GAD_Position: %s", Status ? "Success" : "Failure" );

                if ( Status )
                {
                   // SUPL-REPORT had a position, use this in the position response.
                   // Input the position to the GN_GPS_Lib in case this is good enough for a
                   // user only asking for a low QoP result from via GN_GPS_Get_Nav_Data() etc.
                   GN_AGPS_Set_GAD_Ref_Pos( &GN_AGPS_GAD_Data );
                   GN_SUPL_Pos_Data_From_Position( &GN_SUPL_Pos_Data, &GN_Position_Store );

                   /* Not checking for MSA & MSB, as network may send position in SUPL REPORT as in case of SUPL END message */
                   if(    p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModeAreaEvent
                       && p_SUPL_Instance->p_SUPL->MO_Request == FALSE )
                   {
                       if( GN_SUPL_Check_AreaEvent_Wrapper( p_SUPL_Instance->Handle, &GN_AGPS_GAD_Data ) )
                       {
                          SUPL_Report_To_Send = TRUE;

                          memcpy( &Supl_Report_Position_Store, &GN_Position_Store, sizeof(s_GN_Position) );

                          p_SUPL_Instance->p_SUPL->V2_Data.Triggers.areaEventTrigger.repeatedReportingParams.maximumNumberOfReports--;
                       }
                   }

                   GN_SUPL_Cache_Store_Position( &GN_Position_Store );
                   GN_SUPL_Log( "SUPL_Handle_PDU_SUPL_REPORT : Ending SUPL POS sequence with position from SUPL-REPORT:" );
                   GN_Status = GN_SUCCESS;
                   Status = GN_SUPL_Position_Resp_Out_Wrapper( p_SUPL_Instance->Handle,
                                                               &GN_Status,
                                                               &GN_SUPL_Pos_Data );
                }
                else
                {
                   s_GN_GPS_Nav_Data    Nav_Data;
                   U4                   NULL_OS_Time_ms      = 0;
                   U4                   GAD_Ref_TOW          = 0;
                   I2                   GAD_Ref_TOW_Subms;     // GAD Data Reference GPS Time of Week Sub-millisecond part [range 0..9999 x 0.0001 ms,  -1 = Unknown]
                   I4                   EFSP_dT_us;
                   s_GN_Position        GN_Position;

                   GN_SUPL_Log( "SUPL_Handle_PDU_SUPL_REPORT : Ending SUPL POS sequence with no position:" );
                   GN_Status = GN_WRN_NO_POSITION;

                   memset( &GN_AGPS_GAD_Data, 0, sizeof( s_GN_AGPS_GAD_Data ) );
                   memset( &GN_Position, 0, sizeof( s_GN_Position ) );

                   if (    GN_GPS_Get_Nav_Data_Copy(        &Nav_Data )
                        && GN_AGPS_Get_GAD_Data(            &NULL_OS_Time_ms,    &GAD_Ref_TOW,  &GAD_Ref_TOW_Subms, &EFSP_dT_us, &GN_AGPS_GAD_Data ))
                   {

                       GN_SUPL_Position_From_Solution(  &GN_Position,  &Nav_Data,     &GN_AGPS_GAD_Data );

                       if(    p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModeAreaEvent
                           && p_SUPL_Instance->p_SUPL->MO_Request                     == FALSE
                           &&
                             (
                                  p_SUPL_Instance->p_SUPL->GN_PosMethod_Requested == GN_PosMethod_agpsSETbased
                               || p_SUPL_Instance->p_SUPL->GN_PosMethod_Requested == GN_PosMethod_agpsSETbasedpref
                              )
                         )
                      {
                         if( GN_SUPL_Check_AreaEvent_Wrapper( p_SUPL_Instance->Handle, &GN_AGPS_GAD_Data ) )
                         {
                            SUPL_Report_To_Send = TRUE;

                            memcpy( &Supl_Report_Position_Store, &GN_Position, sizeof(s_GN_Position) );

                            p_SUPL_Instance->p_SUPL->V2_Data.Triggers.areaEventTrigger.repeatedReportingParams.maximumNumberOfReports--;
                         }
                      }
                   }
                }

                Status = GN_RRLP_Sequence_End_In( p_SUPL_Instance->POS_Handle,
                                                  &GN_Status,
                                                  NULL);

                p_SUPL_Instance->POS_Handle = NULL;

                /*SUPL POS session needs to end with SUPL Report being sent from network which marks end of one iteration.
                                Hence decrement the count as well here. */

                if( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModePeriodic )
                {
                   p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger.NumberOfFixes--;

                   if( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger.NumberOfFixes == 0 )
                   {
                      GN_SUPL_Log("SUPL_Handle_PDU_SUPL_REPORT: Periodic Triggers Completed");

                      if( ! p_SUPL_Instance->p_SUPL->MO_Request )
                          GN_SUPL_V2_Timer_Set_UT8( p_SUPL_Instance );
                   }
                }

                if(    p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModeAreaEvent
                    && p_SUPL_Instance->p_SUPL->V2_Data.Triggers.areaEventTrigger.repeatedReportingParams.maximumNumberOfReports == 0 )
                {
                   GN_SUPL_Log("SUPL_Handle_PDU_SUPL_REPORT: Area Triggers Completed");
                }


                if( SUPL_Report_To_Send )
                {
                   BL LocationEstimateReq = FALSE;
                   p_PDU_Src    =  supl_PDU_Make_SUPLREPORT(p_SUPL_Instance);

                   if( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModeAreaEvent )
                      LocationEstimateReq = p_SUPL_Instance->p_SUPL->V2_Data.Triggers.areaEventTrigger.locationEstimateRequested;

                   if( LocationEstimateReq )
                      supl_PDU_Add_SUPLREPORT_Position(p_PDU_Src, &Supl_Report_Position_Store);

                   PDU_Ready_To_Send = TRUE;
                }

                /*Moving the state back to active*/
                Next_State = state_SUPLv2_SUPL_TRIGGER_Active;
              }
             break;
          //-----------------------------------------------------------------------
          // Invalid states,  send SUPLEND.
          case state_SUPL_Idle:
          case state_SUPL_Comms_Open_Sent:
          case state_SUPL_SUPL_START_Sent:
          case state_SUPL_SUPL_RESPONSE_Sent:
          case state_SUPL_Waiting_For_RRLP:
          case state_SUPL_POS_Payload_Delivered:
          case state_SUPL_SUPL_END_Sent:
          case state_SUPL_Server_Open_Requested:
          case state_SUPL_Server_Active:
          case state_SUPL_Comms_Close_Sent:
          case state_SUPL_Error:
          case state_SUPLv2_SUPL_TRIGGER_Active:
          case state_SUPLv2_SUPL_TRIGGER_Suspended:
          case state_SUPLv2_SUPL_TRIGGER_START_Sent:
             p_PDU_Src           = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
             supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, GN_StatusCode_protocolError );
             PDU_Ready_To_Send = TRUE;
             Next_State        = state_SUPL_Comms_Close_Sent;
             break;
          //-----------------------------------------------------------------------
          // Coding error.
          default:
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

       SUPL_Instance_State_Transition( p_SUPL_Instance,
                                       Next_State,
                                       "event_SUPL_PDU_Received(SUPL-REPORT)" );
}

void SUPL_Handle_PDU_SUPL_TRIGGERED_RESPONSE
(
   void            *p_PDU_Decoded,     ///< [in] Pointer to a decoded pdu.
   s_SUPL_Instance *p_SUPL_Instance    ///< [in/out] The SUPL instance dealing with this PDU.
)
{
   void                *p_PDU_Src            = NULL;
   BL                   PDU_Ready_To_Send    = FALSE;
   e_SUPL_States        Next_State           = p_SUPL_Instance->p_SUPL->State;

   GN_SUPL_Log( "Inside SUPL_Handle_PDU_SUPL_TRIGGERED_RESPONSE" );

   /// Perform initial validation checks and return a SUPL-END with an error code if they fail.

    /// Retrieve various items from the PDU for use in subsequent PDUs and to check
    /// for validity of the PDU.
    // Retrieve and store SLP session ID from PDU.
   if(p_SUPL_Instance->p_SUPL->NetworkInitiated == FALSE)
   {
       supl_PDU_Get_slpSessionID(  p_PDU_Decoded,
                                    &p_SUPL_Instance->p_SUPL->p_SlpSessionId,
                                    &p_SUPL_Instance->p_SUPL->SlpId );
   }

   //Retrieve Supported Network Information from SUPL TRIGGERED RESPONSE PDU.
   if( p_SUPL_Instance->p_SUPL->V2_Data.p_SupportedNetworkInfo == NULL )
   {
      supl_PDU_Get_SUPLTRIGGEREDRESPONSE_Supported_Network_Info( p_PDU_Decoded, &p_SUPL_Instance->p_SUPL->V2_Data.p_SupportedNetworkInfo);
   }


   if ( ! supl_PDU_Validate_setSessionID(  p_PDU_Decoded,
                                           p_SUPL_Instance->p_SUPL->SetSessionId,
                                          &p_SUPL_Instance->p_SUPL->SetId ) )
   {
      // If the Set Session ID is invalid, send a SUPL-END with the incorrect ID.
      U2          SessionID;
      s_SetId     SetId;

      p_PDU_Src          =  supl_PDU_Make_SUPLEND( p_SUPL_Instance );
      supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,      GN_StatusCode_invalidSessionId );
      supl_PDU_Get_setSessionID(       p_PDU_Decoded, &SessionID, &SetId );
      supl_PDU_Add_setSessionID(       p_PDU_Src,      SessionID, &SetId );
      PDU_Ready_To_Send  =  TRUE;
      Next_State         =  state_SUPL_Comms_Close_Sent;
   }
   else if ((p_SUPL_Instance->p_SUPL->NetworkInitiated) && (! supl_PDU_Validate_slpSessionID(  p_PDU_Decoded,
                                                p_SUPL_Instance->p_SUPL->p_SlpSessionId,
                                               &p_SUPL_Instance->p_SUPL->SlpId )) )
   {
      // If the Set Session ID is invalid, send a SUPL-END with the incorrect ID.
      U1               SessionID[4];
      U1               *p_SessionID = SessionID;
      s_GN_SLPAddress  SlpId;

      p_PDU_Src          =  supl_PDU_Make_SUPLEND( p_SUPL_Instance );
      supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,      GN_StatusCode_invalidSessionId );
      supl_PDU_Get_slpSessionID(       p_PDU_Decoded, &p_SessionID, &SlpId );
      supl_PDU_Add_slpSessionID(       p_PDU_Src,      p_SessionID, &SlpId );

      PDU_Ready_To_Send  =  TRUE;
      Next_State         =  state_SUPL_Comms_Close_Sent;
   }
   else
   {
      switch ( p_SUPL_Instance->p_SUPL->State )
      {
      //-----------------------------------------------------------------------
      // Valid states.
      case state_SUPLv2_SUPL_TRIGGER_START_Sent:
         /* In this case, retrieve the params from the SUPL TRIGGERED RESPONSE message
            and pass them on to the trigger function implemented in the host layer */

         /* Choose Periodic or Area Event */
         /*Trigger param:
         For SET initiated trigger service, this TriggerParam MAY be used to convey an Area Id List to the SET else this is optional
         IN case of NI this information is mandatory*/


         switch( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode )
         {
            case GN_TriggeredModePeriodic:
               if( supl_PDU_Get_SUPLTRIGGEREDRESPONSE_PeriodicParams( p_PDU_Decoded ,
                                          &p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger ) )
               {
                  /* @TODO : Use a wrapper */
                  GN_SUPL_TriggerPeriodic_Req_Out(p_SUPL_Instance->Handle,
                                          p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger.NumberOfFixes,
                                          p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger.IntervalBetweenFixes,
                                          p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger.StartTime);
                  Next_State         = state_SUPLv2_SUPL_TRIGGER_Active;

               }
               else if(p_SUPL_Instance->p_SUPL->NetworkInitiated == FALSE)
               {
                   /* This is because SI case already have periodic trigger set. Refer - "SUPL-2.0-con-141", where network doesnot provide */
                   GN_SUPL_TriggerPeriodic_Req_Out(p_SUPL_Instance->Handle,
                                           p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger.NumberOfFixes,
                                           p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger.IntervalBetweenFixes,
                                           p_SUPL_Instance->p_SUPL->V2_Data.Triggers.periodicTrigger.StartTime);
                   Next_State        = state_SUPLv2_SUPL_TRIGGER_Active;
               }
               else
               {
                  p_PDU_Src          =  supl_PDU_Make_SUPLEND( p_SUPL_Instance );
                  supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, GN_StatusCode_dataMissing );
                  PDU_Ready_To_Send  =  TRUE;
                  Next_State         =  state_SUPL_Comms_Close_Sent;
               }
               break;
            case GN_TriggeredModeAreaEvent:

               if( supl_PDU_Get_SUPLTRIGGEREDRESPONSE_AreaParams( p_PDU_Decoded ,
                                          &p_SUPL_Instance->p_SUPL->V2_Data.Triggers.areaEventTrigger , p_SUPL_Instance ) )
               {
                  /* @TODO : Use a wrapper */
                  GN_SUPL_TriggerAreaEvent_Req_Out(p_SUPL_Instance->Handle,
                                          &p_SUPL_Instance->p_SUPL->V2_Data.Triggers.areaEventTrigger);
                  Next_State         = state_SUPLv2_SUPL_TRIGGER_Active;

               }
               else if((p_SUPL_Instance->p_SUPL->NetworkInitiated == FALSE) && (supl_PDU_Check_SUPLTRIGGEREDRESPONSE_AreaParams( p_PDU_Decoded )))
               {
                   /* This is because SI case already have periodic trigger set. Refer - "SUPL-2.0-con-141", where network doesnot provide */
                   GN_SUPL_TriggerAreaEvent_Req_Out(p_SUPL_Instance->Handle,
                                           &p_SUPL_Instance->p_SUPL->V2_Data.Triggers.areaEventTrigger);
                   Next_State        = state_SUPLv2_SUPL_TRIGGER_Active;
               }
               else
               {
                  p_PDU_Src          =  supl_PDU_Make_SUPLEND( p_SUPL_Instance );
                  supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, GN_StatusCode_dataMissing );
                  PDU_Ready_To_Send  =  TRUE;
                  Next_State         =  state_SUPL_Comms_Close_Sent;
               }
               break;
            default:
                //RaghavTODO : Must send SUPL END here
               OS_ASSERT( 0 );
               break;
         }
//RaghavTODO - why is this trigger state done here. If SUPL end is being sent state is lost, hence commenting it.
//         Next_State        = state_SUPLv2_SUPL_TRIGGER_Active;
         break;
      //-----------------------------------------------------------------------
      // Invalid states,  send SUPLEND.
      case state_SUPL_Idle:
      case state_SUPL_Comms_Open_Sent:
      case state_SUPL_SUPL_START_Sent:
      case state_SUPL_SUPL_RESPONSE_Sent:
      case state_SUPL_Waiting_For_RRLP:
      case state_SUPL_POS_Payload_Delivered:
      case state_SUPL_SUPL_END_Sent:
      case state_SUPL_Server_Open_Requested:
      case state_SUPL_Server_Active:
      case state_SUPL_Comms_Close_Sent:
      case state_SUPL_SUPL_POS_INIT_Sent:
      case state_SUPL_SUPL_POS_Sent:
      case state_SUPL_Error:
      case state_SUPLv2_SUPL_TRIGGER_Active:
      case state_SUPLv2_SUPL_TRIGGER_Suspended:
         p_PDU_Src         = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
         supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, GN_StatusCode_protocolError );
         PDU_Ready_To_Send = TRUE;
         Next_State        = state_SUPL_Comms_Close_Sent;
         break;
      //-----------------------------------------------------------------------
      // Coding error.
      default:
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
      if(Next_State ==  state_SUPL_Comms_Close_Sent )
      {
      
         e_GN_Status     GN_Status = GN_SUCCESS;
         BL              Successful;
         Successful = GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                                               &GN_Status );
      }

   }

   SUPL_Instance_State_Transition( p_SUPL_Instance,
                                   Next_State,
                                   "event_SUPL_PDU_Received(SUPL-TRIGGERED-RESPONSE)" );
}


//*****************************************************************************
/// \brief
///      Function to handle SUPL TRIGGERED STOP PDU.
/// \details
///      This function handles the SUPL Triggered Stop
/// \returns
///      None
//*****************************************************************************
void SUPL_Handle_PDU_SUPL_TRIGGERED_STOP
(
   void            *p_PDU_Decoded,     ///< [in] Pointer to a decoded pdu.
   s_SUPL_Instance *p_SUPL_Instance    ///< [in/out] The SUPL instance dealing with this PDU.
)
{
    void                *p_PDU_Src             = NULL;
    BL                    PDU_Ready_To_Send     = FALSE;
    e_SUPL_States        Next_State             = p_SUPL_Instance->p_SUPL->State;

    GN_SUPL_Log( "Inside SUPL_Handle_PDU_SUPL_TRIGGERED_STOP" );

    /// Perform initial validation checks and return a SUPL-END with an error code if they fail.
    if ( ! supl_PDU_Validate_setSessionID(  p_PDU_Decoded,
                                            p_SUPL_Instance->p_SUPL->SetSessionId,
                                           &p_SUPL_Instance->p_SUPL->SetId ) )
    {
       // If the Set Session ID is invalid, send a SUPL-END with the incorrect ID.
       U2          SessionID;
       s_SetId     SetId;

       p_PDU_Src          =    supl_PDU_Make_SUPLEND( p_SUPL_Instance );
       supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,       GN_StatusCode_invalidSessionId );
       supl_PDU_Get_setSessionID(       p_PDU_Decoded,  &SessionID, &SetId );
       supl_PDU_Add_setSessionID(       p_PDU_Src,       SessionID, &SetId );
       PDU_Ready_To_Send  =    TRUE;
       Next_State         =    state_SUPL_Comms_Close_Sent;
    }

    else if ( ! supl_PDU_Validate_slpSessionID(  p_PDU_Decoded,
                                                 p_SUPL_Instance->p_SUPL->p_SlpSessionId,
                                                &p_SUPL_Instance->p_SUPL->SlpId ) )
    {
       // If the Set Session ID is invalid, send a SUPL-END with the incorrect ID.
       U1               SessionID[4];
       U1               *p_SessionID = SessionID;
       s_GN_SLPAddress  SlpId;

       p_PDU_Src          =    supl_PDU_Make_SUPLEND( p_SUPL_Instance );
       supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src,       GN_StatusCode_invalidSessionId );
       supl_PDU_Get_slpSessionID(       p_PDU_Decoded, &p_SessionID,  &SlpId );
       supl_PDU_Add_slpSessionID(       p_PDU_Src,       p_SessionID, &SlpId );

       PDU_Ready_To_Send  =    TRUE;
       Next_State         =    state_SUPL_Comms_Close_Sent;
    }
    else
    {
       BL Supl_End_To_Send = FALSE;
       BL Error_Cause_Sent = FALSE;

       // Status code is an optional field within SUPL TRIGGERED STOP. Irrespective of status,
       // for Triggered (periodic & event), session info query, SUPL END is sent.
       switch ( p_SUPL_Instance->p_SUPL->State )
       {
       case state_SUPLv2_SUPL_TRIGGER_Active:
       case state_SUPLv2_SUPL_TRIGGER_Suspended:
       case state_SUPLv2_SUPL_TRIGGER_START_Sent:
          Supl_End_To_Send = TRUE;
          break;

       case state_SUPL_SUPL_POS_INIT_Sent:
       case state_SUPL_SUPL_POS_Sent:
       case state_SUPL_Waiting_For_RRLP:
       case state_SUPL_POS_Payload_Delivered:
          Supl_End_To_Send = TRUE;

          if( p_SUPL_Instance->p_SUPL->V2_Data.Triggers.triggeredMode == GN_TriggeredModeNone )
          {
              Error_Cause_Sent = TRUE;
          }
          break;

       case state_SUPL_Error:
       case state_SUPL_Comms_Open_Sent:
        /* Nothing to do */
       break;

       case state_SUPL_Idle:
       case state_SUPL_SUPL_START_Sent:
       case state_SUPL_SUPL_RESPONSE_Sent:
       case state_SUPL_SUPL_END_Sent:
       case state_SUPL_Server_Open_Requested:
       case state_SUPL_Server_Active:
       case state_SUPL_Comms_Close_Sent: /* TODO - Need to handle this*/
           Supl_End_To_Send = TRUE;
           Error_Cause_Sent = TRUE;
           break;

       //-----------------------------------------------------------------------
       // Coding error.
       default:
          OS_ASSERT( 0 );
          break;
       //-----------------------------------------------------------------------
       }

       if( Supl_End_To_Send )
       p_PDU_Src         = supl_PDU_Make_SUPLEND( p_SUPL_Instance );
       if( Error_Cause_Sent )
       {
           supl_PDU_Add_SUPLEND_StatusCode( p_PDU_Src, GN_StatusCode_protocolError );
       }
       PDU_Ready_To_Send = TRUE;
       Next_State        = state_SUPL_Comms_Close_Sent;
    }

    if( PDU_Ready_To_Send )
    {
       BL Status;
       Status = GN_SUPL_PDU_Send( p_SUPL_Instance, p_PDU_Src );
       supl_PDU_Free( p_PDU_Src );
       if ( ! Status )
       {
           e_GN_Status    GN_Status = GN_SUCCESS;

           GN_SUPL_Disconnect_Req_Out_Wrapper( p_SUPL_Instance->Handle,
                                                            &GN_Status );
           // Transitions straight to Idle to save waiting for a close confirmation.

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
                                    "event_SUPL_PDU_Received(SUPL-TRIGGERED-STOP)" );
}


void SUPL_Send_Trigger(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void            *Handle             ///< [in] Opaque Handle used to coordinate requests.
)
{
   s_SUPL_Message *p_MessageToSend;

   GN_SUPL_Log( "Inside SUPL_Send_Trigger" );

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle         = Handle;
   p_MessageToSend->Payload.MsgEvent      = event_SUPL_V2_Trigger;

   SUPL_Queue_Add_Message( p_MessageToSend );
}



void SUPL_Send_TriggerPeriodic_Request(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void            *Handle,             ///< [in] Opaque Handle used to coordinate requests.
   s_GN_SUPL_V2_Triggers *p_Triggers     ///< [in] Type of Trigger event and reporting values.
)
{
   s_SUPL_Message *p_MessageToSend;


   GN_SUPL_Log( "Inside SUPL_Send_TriggerPeriodic_Request" );

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle         = Handle;
   p_MessageToSend->Payload.MsgEvent      = event_SUPL_V2_Trigger_Periodic_Requested;
   p_MessageToSend->Payload.p_Triggers    = GN_Calloc(1,sizeof(s_GN_SUPL_V2_Triggers));
   memcpy(p_MessageToSend->Payload.p_Triggers, p_Triggers,sizeof(s_GN_SUPL_V2_Triggers));
   GN_SUPL_Log( "SUPL_Send_TriggerPeriodic_Request Event Mode = %d",p_MessageToSend->Payload.p_Triggers->triggeredMode);
   SUPL_Queue_Add_Message( p_MessageToSend );
}

void SUPL_Send_ThirdParty_Info(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void            *Handle,             ///< [in] Opaque Handle used to coordinate requests.
   s_GN_SUPL_V2_ThirdPartyInfo *p_Third_Party_Info      ///< [in] ThirdParty Info
)
{
   s_SUPL_Message *p_MessageToSend;


   GN_SUPL_Log( "Inside SUPL_Send_ThirdParty_Info" );
   GN_SUPL_Log( "SUPL_Send_ThirdParty_Info Third Party Name = %s",p_Third_Party_Info->thirdPartyIdName);
   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle         = Handle;
   p_MessageToSend->Payload.MsgEvent      = event_SUPL_V2_Third_Party_info_Request;
   p_MessageToSend->Payload.p_Third_Party_Info    = GN_Calloc(1,sizeof(s_GN_SUPL_V2_ThirdPartyInfo));
   p_MessageToSend->Payload.p_Third_Party_Info->thirdPartyId = p_Third_Party_Info->thirdPartyId;
   p_MessageToSend->Payload.p_Third_Party_Info->thirdPartyIdName = GN_Calloc(1,(strlen(p_Third_Party_Info->thirdPartyIdName)+1));
   memcpy(p_MessageToSend->Payload.p_Third_Party_Info->thirdPartyIdName, p_Third_Party_Info->thirdPartyIdName,strlen(p_Third_Party_Info->thirdPartyIdName));
   p_MessageToSend->Payload.p_Third_Party_Info->thirdPartyIdName[strlen(p_Third_Party_Info->thirdPartyIdName) + 1] = '\0';
   SUPL_Queue_Add_Message( p_MessageToSend );
}

void SUPL_Send_TargetSet_Info(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void            *Handle,             ///< [in] Opaque Handle used to coordinate requests.
   s_GN_SUPL_V2_ThirdPartyInfo *p_Third_Party_Info      ///< [in] ThirdParty Info
)
{
   s_SUPL_Message *p_MessageToSend;


   GN_SUPL_Log( "Inside SUPL_Send_ThirdParty_Info" );
   GN_SUPL_Log( "SUPL_Send_ThirdParty_Info Third Party Name = %s",p_Third_Party_Info->thirdPartyIdName);
   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle         = Handle;
   p_MessageToSend->Payload.MsgEvent      = event_SUPL_V2_ThirdPartyPosReq_Received;
   p_MessageToSend->Payload.p_Third_Party_Info    = GN_Calloc(1,sizeof(s_GN_SUPL_V2_ThirdPartyInfo));
   p_MessageToSend->Payload.p_Third_Party_Info->thirdPartyId = p_Third_Party_Info->thirdPartyId;
   p_MessageToSend->Payload.p_Third_Party_Info->thirdPartyIdName = GN_Calloc(1,(strlen(p_Third_Party_Info->thirdPartyIdName)+1));
   memcpy(p_MessageToSend->Payload.p_Third_Party_Info->thirdPartyIdName, p_Third_Party_Info->thirdPartyIdName,strlen(p_Third_Party_Info->thirdPartyIdName));
   p_MessageToSend->Payload.p_Third_Party_Info->thirdPartyIdName[strlen(p_Third_Party_Info->thirdPartyIdName) + 1] = '\0';
   
   SUPL_Queue_Add_Message( p_MessageToSend );
}

void SUPL_Send_ApplicationId_Info(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void            *Handle,             ///< [in] Opaque Handle used to coordinate requests.
   s_GN_SUPL_V2_ApplicationInfo  *p_SUPL_ApplicationIDInfo      ///< [in] ThirdParty Info
)
{
   s_SUPL_Message *p_MessageToSend;


   GN_SUPL_Log( "Inside SUPL_Send_ApplicationId_Info" );

   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle         = Handle;
   p_MessageToSend->Payload.MsgEvent      = event_SUPL_V2_ApplicationId_info_Request;
   p_MessageToSend->Payload.p_SUPL_ApplicationIDInfo    = GN_Calloc(1,sizeof(s_GN_SUPL_V2_ApplicationInfo));
   p_MessageToSend->Payload.p_SUPL_ApplicationIDInfo->applicationIDInfoPresence = p_SUPL_ApplicationIDInfo->applicationIDInfoPresence;

   if(p_SUPL_ApplicationIDInfo->applicationProvider != NULL)
   {
        p_MessageToSend->Payload.p_SUPL_ApplicationIDInfo->applicationProvider =  GN_Calloc(1,(strlen(p_SUPL_ApplicationIDInfo->applicationProvider)+1));
        memcpy(p_MessageToSend->Payload.p_SUPL_ApplicationIDInfo->applicationProvider, p_SUPL_ApplicationIDInfo->applicationProvider,strlen(p_SUPL_ApplicationIDInfo->applicationProvider));
        p_MessageToSend->Payload.p_SUPL_ApplicationIDInfo->applicationProvider[strlen(p_SUPL_ApplicationIDInfo->applicationProvider) + 1] = '\0';
   }

   if(p_SUPL_ApplicationIDInfo->appName != NULL)
   {
        p_MessageToSend->Payload.p_SUPL_ApplicationIDInfo->appName =  GN_Calloc(1,(strlen(p_SUPL_ApplicationIDInfo->appName)+1));
        memcpy(p_MessageToSend->Payload.p_SUPL_ApplicationIDInfo->appName, p_SUPL_ApplicationIDInfo->appName,strlen(p_SUPL_ApplicationIDInfo->appName));
        p_MessageToSend->Payload.p_SUPL_ApplicationIDInfo->appName[strlen(p_SUPL_ApplicationIDInfo->appName) + 1] = '\0';
   }

   if(p_SUPL_ApplicationIDInfo->appVersion != NULL)
   {
        p_MessageToSend->Payload.p_SUPL_ApplicationIDInfo->appVersion =  GN_Calloc(1,(strlen(p_SUPL_ApplicationIDInfo->appVersion)+1));
        memcpy(p_MessageToSend->Payload.p_SUPL_ApplicationIDInfo->appVersion, p_SUPL_ApplicationIDInfo->appVersion,strlen(p_SUPL_ApplicationIDInfo->appVersion));
        p_MessageToSend->Payload.p_SUPL_ApplicationIDInfo->appVersion[strlen(p_SUPL_ApplicationIDInfo->appVersion) + 1] = '\0';
   }
   SUPL_Queue_Add_Message( p_MessageToSend );
}

void SUPL_Send_Mobile_Info_Received(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   s_LocationId*  p_SUPL_LocationId   ///< [in] Location ID required for the SUPL exchange.
)
{
   s_SUPL_Message *p_MessageToSend;
   
   GN_SUPL_Log( "Inside SUPL_Send_Mobile_Info_Received" );
   
   p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );
   
   p_MessageToSend->Payload.MsgEvent      = event_SUPL_V2_Mobile_Info_Received;
   p_MessageToSend->Payload.p_Location_Id = GN_Calloc(1,sizeof(s_LocationId));
   p_MessageToSend->Payload.p_Location_Id->Status =  p_SUPL_LocationId->Status;
   p_MessageToSend->Payload.p_Location_Id->Type   =  p_SUPL_LocationId->Type;
   
   switch(p_SUPL_LocationId->Type)
   {
       case CIT_AccessPoint_WLAN:
       {
          memcpy( &(p_MessageToSend->Payload.p_Location_Id->of_type.wirelessNetworkInfo), &(p_SUPL_LocationId->of_type.wirelessNetworkInfo),
                                   sizeof(s_GN_SUPL_V2_WirelessNetworkInfo));
          break;
       }
       case CIT_gsmCell:
       {
           p_MessageToSend->Payload.p_Location_Id->of_type.gsmCellInfo.refMCC   =  p_SUPL_LocationId->of_type.gsmCellInfo.refMCC;
           p_MessageToSend->Payload.p_Location_Id->of_type.gsmCellInfo.refMNC   =  p_SUPL_LocationId->of_type.gsmCellInfo.refMNC;
           p_MessageToSend->Payload.p_Location_Id->of_type.gsmCellInfo.refLAC   =  p_SUPL_LocationId->of_type.gsmCellInfo.refLAC;
           p_MessageToSend->Payload.p_Location_Id->of_type.gsmCellInfo.refCI    =  p_SUPL_LocationId->of_type.gsmCellInfo.refCI;
           p_MessageToSend->Payload.p_Location_Id->of_type.gsmCellInfo.tA       =  p_SUPL_LocationId->of_type.gsmCellInfo.tA;
           p_MessageToSend->Payload.p_Location_Id->of_type.gsmCellInfo.NMRCount =  p_SUPL_LocationId->of_type.gsmCellInfo.NMRCount;
   
           if( (p_SUPL_LocationId->of_type.gsmCellInfo.NMRCount > 0 ) && (p_SUPL_LocationId->of_type.gsmCellInfo.p_NMRList != NULL))
           {
               s_NMRElement *p_NMRDestination, *p_NMRSource;
               U1 NMRCount ;
               p_MessageToSend->Payload.p_Location_Id->of_type.gsmCellInfo.p_NMRList = GN_Calloc( p_MessageToSend->Payload.p_Location_Id->of_type.gsmCellInfo.NMRCount,
                                                                                                   sizeof( s_NMRElement ) );
               p_NMRDestination = p_MessageToSend->Payload.p_Location_Id->of_type.gsmCellInfo.p_NMRList;
               p_NMRSource      = p_SUPL_LocationId->of_type.gsmCellInfo.p_NMRList;
               NMRCount         = p_MessageToSend->Payload.p_Location_Id->of_type.gsmCellInfo.NMRCount ;
   
               memcpy( p_NMRDestination, p_NMRSource,NMRCount * sizeof( s_NMRElement ) );
           }
           break;
       }
       case CIT_cdmaCell:
       {
           memcpy( &(p_MessageToSend->Payload.p_Location_Id->of_type.cdmaCellInfo), &(p_SUPL_LocationId->of_type.cdmaCellInfo),
                              sizeof(s_cdmaCellInfo) );
           break;
       }
       case CIT_wcdmaCell:
       {
           supl_Copy_wcdmaCellInfo( &p_MessageToSend->Payload.p_Location_Id->of_type.wcdmaCellInfo, &p_SUPL_LocationId->of_type.wcdmaCellInfo );
           break;
       }
       default:
       break;
   }
   SUPL_Queue_Add_Message( p_MessageToSend );
}

//*****************************************************************************
/// \brief
///      Check Location Id expiry.
/// \details

void SUPL_Update_Loc_Id( s_GN_SUPL_V2_Multiple_Loc_ID *AddNode )
{   
   if( SUPL_Mlid_FirstElem == NULL )
   {
       SUPL_Mlid_FirstElem = AddNode;
       SUPL_Mlid_FirstElem->next=NULL;
       SUPL_Mlid_FirstElem->prev=NULL;
   
       GN_SUPL_Log( "SUPL_Update_Loc_Id :" );
   }
   else // last element is kept intact, as new element comes fist node points to it.
   {
   
       GN_SUPL_Log( "SUPL_Update_Loc_Id :" );
       SUPL_Mlid_FirstElem->next = AddNode;
   
       AddNode->prev = SUPL_Mlid_FirstElem ;
   
       AddNode->next = NULL;
   
       SUPL_Mlid_FirstElem = AddNode ;
   }
   return;
}

//*****************************************************************************
/// \brief
///      Check Location Id expiry.
/// \details

void SUPL_Check_Loc_Id_Expiry( void )
{
   s_GN_SUPL_V2_Multiple_Loc_ID* Temp  = NULL;
   s_GN_SUPL_V2_Multiple_Loc_ID* Temp1 = NULL;
   
   uint64_t   p_Ref_OSTimeMs; ///< Local Operating System Time [msec]
   
   struct timeval current = {0};
   (void) gettimeofday( &current , NULL );
   
   p_Ref_OSTimeMs = current.tv_sec*1000;
   
   GN_SUPL_Log( "SUPL_Check_Loc_Id_Expiry: Entry" );
   
   Temp  = SUPL_Mlid_FirstElem->prev ;// Check for expiry after Current location Id
   
   if(Temp != NULL)
   {
       while(Temp !=NULL)
       {
           if ( (p_Ref_OSTimeMs - Temp->v_OSTimeMs) > 655350 )
           {
               Temp1 = Temp->next ;
               Temp1->prev = NULL;
               while(Temp->prev != NULL)
               {
                   Temp1 = Temp->prev ;
                   GN_Free(Temp);
                   Temp  = Temp1 ;
               }
               GN_Free(Temp);
               Temp = NULL;
           }
           if(Temp != NULL )
           Temp = Temp->prev ;
       }
   }
}

void SUPL_Send_Delete_Location_id_Info( 
   s_SUPL_Instance *p_DestInstance,  ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance    ///< [in] Entity instance where the message originated.
)
{
    s_SUPL_Message *p_MessageToSend;
    
    GN_SUPL_Log( "Inside SUPL_Send_Delete_Location_id_Info" );
    
    p_MessageToSend = SUPL_Message_Init( p_DestInstance, p_SrcInstance );
    
    p_MessageToSend->Payload.MsgEvent      = event_SUPL_V2_Delete_Location_id_Requested ;
    
    SUPL_Queue_Add_Message( p_MessageToSend );
}




