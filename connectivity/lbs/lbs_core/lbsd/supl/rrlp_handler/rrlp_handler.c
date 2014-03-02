
//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename rrlp_handler.c
//
// $Header: X:/MKS Projects/prototype/prototype/rrlp_handler/rcs/rrlp_handler.c 1.58 2009/01/13 14:09:42Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_handler
///
/// \file
/// \brief
///      RRLP handler implementation.
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
#include "GN_SUPL_api.h"
#include "GN_RRLP_server_api.h"

// global data
s_RRLP_Instances    RRLP_Instances;
s_RRLP_Queue        RRLP_Queue;

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

void RRLP_Handle_Terminate_POS_Sequence( s_RRLP_Message* p_ThisMessage );

//*****************************************************************************

//*****************************************************************************
/// \brief
///      Initialise the RRLP Handler.
/// \returns
///      Nothing.
//*****************************************************************************
void RRLP_Handler_Init( void )
{
   /// Initialises the RRLP Instance container.
   RRLP_Instances_Initialise( INSTANCE_RRLP_MAX );
   /// Then initialises the RRLP_Queue.
   memset( &RRLP_Queue, 0, sizeof( s_RRLP_Queue ) );
}

//*****************************************************************************
/// \brief
///      De-Initialise the RRLP Handler.
/// \returns
///      Nothing.
//*****************************************************************************
void RRLP_Handler_DeInit( void )
{
   /// De-Initialise the RRLP Instances.
   RRLP_Instances_DeInitialise();
}

//*****************************************************************************
/// \brief
///      GN GPS RRLP API Function to process an RRLP PDU containing a protocol
///      error.
/// \details
///      Handles situation where a protocol error pdu is received.
/// \returns
///      Nothing.
//*****************************************************************************
void RRLP_Process_protocolError(
   void              *p_PDU_Decoded,   ///< Transparent handle to a decoded RRLP PDU.
   s_RRLP_Instance   *p_RRLP_Instance  ///< RRLP_Instance handling this PDU.
)
{
   /* TODO */
   p_PDU_Decoded   = p_PDU_Decoded;
   p_RRLP_Instance = p_RRLP_Instance;
}


//*****************************************************************************
/// \brief
///      Handles the #event_RRLP_Terminate_POS_Sequence.
//*****************************************************************************
void RRLP_Handle_Terminate_POS_Sequence( s_RRLP_Message* p_ThisMessage )
{
   s_RRLP_Instance *p_RRLP_Instance = p_ThisMessage->MsgHdr.p_InstanceOfDest;

   e_RRLP_States   Next_State;

   if ( p_RRLP_Instance == NULL )
   {
      // Instance no longer valid. No need to terminate.
      // Probably caused by Instance deletion at SUPL side.
      return;
   }

   p_RRLP_Instance = RRLP_Instance_Get_Data( p_RRLP_Instance );
   if ( p_RRLP_Instance != NULL )
   {
      Next_State = p_RRLP_Instance->p_RRLP->State;

      switch ( p_RRLP_Instance->p_RRLP->State )
      {
      case state_RRLP_Position_Requested_From_GPS:
#ifdef SUPL_DEBUG_EVENTS_ENABLE
      GN_SUPL_Debug_Event( SUPL_PosAbort , NULL ); // Do not break but fall through
#endif
      case state_RRLP_Idle:
      case state_RRLP_Transaction_In_Progress:
      case state_RRLP_Assistance_Data_Delivery:
      case state_RRLP_Measure_Position_Request:
         // Valid states.
         Next_State = state_RRLP_Idle;
         break;
      // Error handling.
         // Invalid states
         OS_ASSERT( 0 );
         break;
      default:
         // Coding error.
         OS_ASSERT( 0 );
         break;
      }
      RRLP_Instance_State_Transition(  p_RRLP_Instance,
                                       Next_State,
                                       "event_RRLP_Terminate_POS_Sequence" );
   }
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_RRLP_POS_Payload
///      message.
//*****************************************************************************
void RRLP_Send_PDU_Delivery(
   s_RRLP_Instance   *p_RRLP_Instance, ///< [in] RRLP Agent instance to process
                                       ///  the message.
   s_RRLP_Instance   *p_SrcInstance,   ///< [in] Entity instance where the message
                                       ///  originated.
   void*             Handle,           ///< [in] Opaque Handle used to coordinate requests.
   s_PDU_Encoded     *p_PDU_Encoded,   ///< [in] Pointer to an ASN.1 Encoded RRLP PDU.
   U4                PDU_TimeStamp,    ///< [in] Time at which PDU was received by SUPL.
   s_GN_RRLP_QoP     *p_GN_RRLP_QoP    ///< [in] QoP criteria. NULL pointer if no QoP criteria set.
)
{
   s_RRLP_Message *p_MessageToSend;

   p_MessageToSend = RRLP_Message_Init( p_RRLP_Instance, p_SrcInstance );

   p_MessageToSend->MsgHdr.Handle         = Handle;
   p_MessageToSend->Payload.MsgEvent      = event_RRLP_POS_Payload;
   p_MessageToSend->Payload.p_RRLP_PDU    = p_PDU_Encoded;
   p_MessageToSend->Payload.PDU_TimeStamp = PDU_TimeStamp;

   if ( p_GN_RRLP_QoP != NULL )
   {
      p_MessageToSend->Payload.p_GN_RRLP_QoP = GN_Calloc( 1, sizeof( s_GN_RRLP_QoP ) );
      p_MessageToSend->Payload.p_GN_RRLP_QoP->horacc    = p_GN_RRLP_QoP->horacc;
      p_MessageToSend->Payload.p_GN_RRLP_QoP->veracc    = p_GN_RRLP_QoP->veracc;
      p_MessageToSend->Payload.p_GN_RRLP_QoP->maxLocAge = p_GN_RRLP_QoP->maxLocAge;
      p_MessageToSend->Payload.p_GN_RRLP_QoP->delay     = p_GN_RRLP_QoP->delay;
   }
   RRLP_Queue_Add_Message( p_MessageToSend );
}


//*****************************************************************************
/// \brief
///      Function to initialise and populate an #event_RRLP_Terminate_POS_Sequence
///      message.
//*****************************************************************************
void RRLP_Send_Terminate_POS_Sequence(
   s_RRLP_Instance *p_SrcInstance,  ///< [in] Entity instance where the message
                                    ///  originated.
   void*             Handle         ///< [in] Opaque Handle used to coordinate requests.
)
{
    s_RRLP_Message *p_MessageToSend;
    s_RRLP_Instance *p_Dest_Instance = RRLP_Instance_From_Handle( Handle );

    p_MessageToSend = RRLP_Message_Init( p_Dest_Instance, p_SrcInstance );

    p_MessageToSend->Payload.MsgEvent = event_RRLP_Terminate_POS_Sequence;
    RRLP_Queue_Add_Message( p_MessageToSend );
}

//*****************************************************************************
/// \brief
///      Function to set (ie start/re-start count down) a RRLP timer.
//*****************************************************************************
void GN_RRLP_Timer_Set(
   s_GN_GPS_RRLP_Timer *p_Timer,    ///< [in] Timer to set.
   U4 Duration_ms                   ///< [in] Duration before timer will elapse.
)
{
   p_Timer->Timer_Active      = TRUE;
   p_Timer->Time_Activated_ms = GN_GPS_Get_OS_Time_ms();
   p_Timer->Timer_Duration_ms = Duration_ms;
}

//*****************************************************************************
/// \brief
///      Function to clear (ie kill off) a RRLP timer.
//*****************************************************************************
void GN_RRLP_Timer_Clear(
   s_GN_GPS_RRLP_Timer *p_Timer    ///< [in] Timer to clear.
)
{
   p_Timer->Timer_Active      = FALSE;
   p_Timer->Time_Activated_ms = 0;
   p_Timer->Timer_Duration_ms = 0;
}

//*****************************************************************************
/// \brief
///      Function to check whether an RRLP timer has expired.
/// \returns
///      Flag to indicate whether the RRLP timer has expired:
/// \retval #TRUE if the Timer_Duration has been exceeded since the timer was last set.
/// \retval #FALSE if the Timer_Duration has not been exceeded since the timer was last set.
//*****************************************************************************
BL GN_RRLP_Timer_Expired(
   s_GN_GPS_RRLP_Timer *p_Timer    ///< [in] Timer to check.
)
{
   U4 Current_OS_Time;
   U4 Time_Elapsed;

   if ( p_Timer->Timer_Active      == FALSE ) return FALSE;
   if ( p_Timer->Timer_Duration_ms == 0     ) return FALSE;
   
   Current_OS_Time = GN_GPS_Get_OS_Time_ms();
   
   if ( Current_OS_Time < p_Timer->Time_Activated_ms )
   {
      Time_Elapsed = Current_OS_Time + ( 0xffffffff - p_Timer->Time_Activated_ms );
   }
   else
   {
      Time_Elapsed = Current_OS_Time - p_Timer->Time_Activated_ms;
   }
   if ( Time_Elapsed > p_Timer->Timer_Duration_ms )
   {
      GN_RRLP_Timer_Clear( p_Timer );
      return TRUE;
   }
   return FALSE;
}

//*****************************************************************************
/// \brief
///      Function to check whether an RRLP timer has expired.
/// \returns
///      Flag to indicate whether the RRLP timer has expired:
/// \retval #TRUE if the Timer_Duration has been exceeded since the timer was last set.
/// \retval #FALSE if the Timer_Duration has not been exceeded since the timer was last set.
//*****************************************************************************
BL GN_RRLP_GetTimerCount(
   s_GN_GPS_RRLP_Timer *p_Timer,    ///< [in] Timer to check.
   U4 *p_CountMs
)
{
   U4 Current_OS_Time;
   U4 Time_Elapsed;

   if ( p_Timer->Timer_Active      == FALSE ) return FALSE;
   if ( p_Timer->Timer_Duration_ms == 0     ) return FALSE;
   
   Current_OS_Time = GN_GPS_Get_OS_Time_ms();
   
   if ( Current_OS_Time < p_Timer->Time_Activated_ms )
   {
      Time_Elapsed = Current_OS_Time + ( 0xffffffff - p_Timer->Time_Activated_ms );
   }
   else
   {
      Time_Elapsed = Current_OS_Time - p_Timer->Time_Activated_ms;
   }
   
   *p_CountMs = Time_Elapsed;
   return TRUE;
}
