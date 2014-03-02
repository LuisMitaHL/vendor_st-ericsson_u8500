//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename abst_messages.c
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/supl_messages.c 1.36 2009/01/13 14:10:54Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup os_abstraction
///
/// \file
/// \brief
///      Internal message function implementation.
///
///      The SUPL implementation requires messages to allow asynchronous 
///      handling of data between the various modules.
///
//*************************************************************************

#include "supl_messages.h"
#include "supl_log.h"

/// \brief List of SUPL Agent events for logging purposes.
char * SUPL_Event_Text[]={
    "event_SUPL_Initialisation",

    "event_SUPL_Server_Open_Success",
    "event_SUPL_Comms_Open_Request",
    "event_SUPL_Comms_Open_Success",
    "event_SUPL_Comms_Open_Failure",
    "event_SUPL_Comms_Close",

    "event_SUPL_PDU_Received",
    "event_SUPL_POS_Payload",
    "event_SUPL_POS_Sequence_Complete",
    "event_SUPL_POS_Sequence_Error",

    "event_SUPL_Notification_Request",
    "event_SUPL_Notification_Response",
    "event_SUPL_MO_Position_Requested",
/* +LMSqc38060 */
    "event_SUPL_Abort_Requested",
/* -LMSqc38060 */
    "event_SUPL_Trigger_End_Requested",
    "event_SUPL_Expiry_UT1",
    "event_SUPL_Expiry_UT2",
    "event_SUPL_Expiry_UT3",

   "event_SUPL_V2_Trigger",     
   "event_SUPL_V2_Trigger_Periodic_Requested",
   "event_SUPL_V2_Third_Party_info_Request",
   "event_SUPL_V2_ApplicationId_info_Request",
   "event_SUPL_V2_ThirdPartyPosReq_Received",
   "event_SUPL_V2_Mobile_Info_Received",
   "event_SUPL_V2_Delete_Location_id_Requested",
   "event_SUPL_V2_Expiry_UT5",  
   "event_SUPL_V2_Expiry_UT6",  
   "event_SUPL_V2_Expiry_UT7",  
   "event_SUPL_V2_Expiry_UT8",  
   "event_SUPL_V2_Expiry_UT9",  
   "event_SUPL_V2_Expiry_UT10", 

};

//*****************************************************************************
// Function prototypes local to this file.
void SUPL_MSC_Log_Message(s_SUPL_Message  *p_Message);

//*****************************************************************************
/// \brief
///      Requests a new message structure and initialises it as a SUPL Agent
///      message.
/// \returns
///      Pointer to partially completed message structure.
//*****************************************************************************
s_SUPL_Message* SUPL_Message_Init(
   s_SUPL_Instance *p_DestInstance, ///< SUPL Agent instance to process
                                    ///  the message.
   s_SUPL_Instance *p_SrcInstance   ///< Entity instance where the message 
                                    ///  originated.
)
{
   s_SUPL_Message *p_Message;

   p_Message = GN_Calloc( 1, sizeof( s_SUPL_Message ) );

   if ( p_Message == NULL )
   {
      return NULL;
   }
   else
   {
      p_Message->MsgHdr.p_InstanceOfDest = p_DestInstance;

      p_Message->MsgHdr.p_InstanceOfSrc = p_SrcInstance;

      return p_Message;
   }
}

//*****************************************************************************
/// \brief
///      Function to add a message to a queue.
//*****************************************************************************
void SUPL_Queue_Add_Message
(
    s_SUPL_Message  *p_Message  ///< [in] Message to place at the end of 
                                ///  the Queue.
)
{
    s_SUPL_Queue    *p_Queue = &SUPL_Queue;   ///< [in/out] Queue on which to place the 
                                ///  message
   /// The SUPL implementation uses messages to allow asynchronous handling of 
   /// data between the various modules yet retain integrity.
   if( p_Queue->p_Tail == NULL )
    {
        // queue is empty
        p_Queue->p_Head = p_Message;
        p_Queue->p_Tail = p_Message;
        p_Message->p_PrevMsg = NULL;
        p_Message->p_NextMsg = NULL;
    }
    else
    {
        // add message to tail end.
        p_Queue->p_Tail->p_NextMsg = p_Message;         // point p_NextMsg of last message in queue to new message
        p_Message->p_PrevMsg       = p_Queue->p_Tail;   // point p_PrevMsg of new message to last message in queue
        p_Queue->p_Tail            = p_Message;         // point tail to new message
        p_Message->p_NextMsg       = NULL;              // keep p_NextMsg of new message NULL
    }
    SUPL_MSC_Log_Message( p_Message );
}

//*****************************************************************************
/// \brief
///      Function to remove a message from a queue.
/// \returns
///      Pointer to message removed from queue (NULL if queue empty)
//*****************************************************************************
s_SUPL_Message*    SUPL_Queue_Retrieve_Message( void )
{
   s_SUPL_Queue   *p_Queue    = &SUPL_Queue; // Queue on which to place the message
   s_SUPL_Message *p_Message;                // temporary message pointer

   if( p_Queue->p_Head == NULL )
   {
      // queue is empty
      return NULL;
   }
   else
   {
      // remove message from head end.
      p_Message = p_Queue->p_Head;
      // is this the only remaingin message in the queue
      if ( p_Queue->p_Head->p_NextMsg == NULL )
      {
         // reset the head and tail
         p_Queue->p_Tail = NULL;
         p_Queue->p_Head = NULL;
      }
      else
      {
         // join up the p_Head of the queue to the following message
         p_Queue->p_Head            = p_Message->p_NextMsg; // point head of queue to next message in queue
         p_Queue->p_Head->p_PrevMsg = NULL;                 // there are no previous messages for now head of queue
         // tidy up the message before passing it back
         p_Message->p_NextMsg       = NULL;                 // p_PrevMsg will be NULL already for head message
      }
      return p_Message;
   }
}

//*****************************************************************************
/// \brief
///      Frees up a SUPL Agent message.
//*****************************************************************************
void SUPL_Message_Free( s_SUPL_Message *p_Message )
{
   // free up any NET specific payloads
   if ( p_Message->Payload.p_PDU != NULL )
   {
      asn1_PDU_Buffer_Store_Free( &p_Message->Payload.p_PDU );
   }
   if ( p_Message->Payload.p_GN_AGPS_GAD_Data != NULL )
   {
      GN_Free( p_Message->Payload.p_GN_AGPS_GAD_Data );
   }
   if ( p_Message->Payload.p_GN_GPS_Nav_Data != NULL )
   {
      GN_Free( p_Message->Payload.p_GN_GPS_Nav_Data );
   }
   if ( p_Message->Payload.p_GN_SUPL_Connect_Req_Parms != NULL )
   {
      GN_Free( p_Message->Payload.p_GN_SUPL_Connect_Req_Parms );
   }

    // free underlying message
    GN_Free( p_Message );
}


//*****************************************************************************
/// \brief
///      Function to log a message in a form for parsing by mscgen.
//*****************************************************************************
void SUPL_MSC_Log_Message
(
    s_SUPL_Message  *p_Message  ///< [in] Pointer to Message to be logged.
)
{
   char  *MsgSrcName = "",
         *MsgDestName = "",
         *MsgEventName = "";
   I4 MsgSrcId, MsgDestId;
   s_SUPL_Instance *p_srcInst  = (void *) -1;
   s_SUPL_Instance *p_destInst = (void *) -1;

   MsgDestName = "SUPL_AgentPeer";
   p_destInst = SUPL_Instance_Get_Data( p_Message->MsgHdr.p_InstanceOfDest );
   if ( p_destInst != NULL )
   {
       MsgDestId = p_destInst->ThisInstanceId;
   }
   else
   {
       // Destination Id no longer in use.
       MsgDestId = -1;
   }
   MsgSrcName = "SUPL_AgentPeer";
   if ( p_Message->MsgHdr.p_InstanceOfSrc == NULL )
   {
       MsgSrcId = 0;
   }
   else
   {
       p_srcInst = SUPL_Instance_Get_Data( p_Message->MsgHdr.p_InstanceOfSrc );
       if ( p_srcInst != NULL )
       {
           MsgSrcId = p_srcInst->ThisInstanceId;
       }
       else
       {
           // Destination Id no longer in use.
           MsgSrcId = -1;
       }
   }

   MsgEventName = SUPL_Event_Text[p_Message->Payload.MsgEvent];
   GN_SUPL_Log( "Event:    %s->%s [label=\"%s SrcId=%d, DestId=%d, Handle=%p\"];",
      MsgSrcName, MsgDestName, MsgEventName, MsgSrcId, MsgDestId, p_Message->MsgHdr.Handle );
}
