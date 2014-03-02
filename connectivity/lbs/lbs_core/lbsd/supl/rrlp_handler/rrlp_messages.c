//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename rrlp_messages.c
//
// $Header: X:/MKS Projects/prototype/prototype/rrlp_handler/rcs/rrlp_messages.c 1.13 2009/01/13 14:09:43Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_handler
///
/// \file
/// \brief
///      Internal message function implementation.
///
///      The SUPL implementation requires messages to allow asynchronous 
///      handling of data between the various modules.
///
//*************************************************************************

#include "rrlp_messages.h"

/// \brief List of SUPL Agent events for logging purposes.
char * RRLP_Event_Text[]={
   "event_RRLP_Initiate_POS_Sequence",
   "event_RRLP_POS_Payload",
   "event_RRLP_GPS_Position_Solution_Available",
   "event_RRLP_GPS_Measurements_Available",
   "event_RRLP_Terminate_POS_Sequence",
   "event_RRLP_Expiry_MeasureResponseTime"
};

//*****************************************************************************
// Function prototypes local to this file.
void RRLP_MSC_Log_Message(s_RRLP_Message  *p_Message);

//*****************************************************************************
/// \brief
///      Requests a new message structure and initialises it as a RRLP Agent
///      message.
/// \returns
///      Pointer to partially completed message structure.
//*****************************************************************************
s_RRLP_Message* RRLP_Message_Init(
   s_RRLP_Instance *p_RRLP_Instance,///< RRLP Agent instance to process
                                    ///  the message.
   s_RRLP_Instance *p_SrcInstance   ///< Entity instance where the message 
                                    ///  originated.
)
{
   s_RRLP_Message *p_Message;

   p_Message = GN_Calloc( 1, sizeof( s_RRLP_Message ) );

   if ( p_Message == NULL )
   {
      return NULL;
   }
   else
   {
      p_Message->MsgHdr.p_InstanceOfDest = p_RRLP_Instance;

      p_Message->MsgHdr.p_InstanceOfSrc = p_SrcInstance;

      return p_Message;
   }
}

//*****************************************************************************
/// \brief
///      Function to add a message to a queue.
//*****************************************************************************
void RRLP_Queue_Add_Message
(
    s_RRLP_Message  *p_Message  ///< [in] Message to place at the end of 
                                ///  the Queue.
)
{
    s_RRLP_Queue    *p_Queue = &RRLP_Queue;   ///< [in/out] Queue on which to place the 
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
        p_Queue->p_Tail->p_NextMsg = p_Message;      // point p_NextMsg of last message in queue to new message
        p_Message->p_PrevMsg       = p_Queue->p_Tail;// point p_PrevMsg of new message to last message in queue
        p_Queue->p_Tail            = p_Message;      // point tail to new message
        p_Message->p_NextMsg       = NULL;           // keep p_NextMsg of new message NULL
    }
    RRLP_MSC_Log_Message( p_Message );
}


//*****************************************************************************
/// \brief
///      Function to remove a message from a queue.
/// \returns
///      Pointer to message removed from queue (NULL if queue empty)
//*****************************************************************************
s_RRLP_Message*    RRLP_Queue_Retrieve_Message( void )
{
   s_RRLP_Queue *p_Queue  = &RRLP_Queue;       ///< [in] Queue on which to place the message
   s_RRLP_Message *p_Message;    // temporary message pointer

    if ( p_Queue->p_Head == NULL )
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
///      Frees up an RRLP Agent message.
//*****************************************************************************
void RRLP_Message_Free(s_RRLP_Message *p_Message)
{
   if ( p_Message->Payload.p_GN_AGPS_GAD_Data != NULL )
   {
      GN_Free(p_Message->Payload.p_GN_AGPS_GAD_Data);
   }
   if ( p_Message->Payload.p_GN_GPS_Nav_Data != NULL )
   {
      GN_Free( p_Message->Payload.p_GN_GPS_Nav_Data );
   }
   if ( p_Message->Payload.p_GN_RRLP_QoP != NULL )
   {
      GN_Free( p_Message->Payload.p_GN_RRLP_QoP );
   }
   if ( p_Message->Payload.p_GN_AGPS_Meas != NULL )
   {
      GN_Free( p_Message->Payload.p_GN_AGPS_Meas );
   }
   if ( p_Message->Payload.p_RRLP_PDU != NULL )
   {
      if ( p_Message->Payload.p_RRLP_PDU->p_PDU_Data != NULL )
      {
         GN_Free( p_Message->Payload.p_RRLP_PDU->p_PDU_Data );
      }
      GN_Free( p_Message->Payload.p_RRLP_PDU );
   }
   // free underlying message
   GN_Free( p_Message );
}


//*****************************************************************************
/// \brief
///      Function to log a message in a form for parsing by mscgen.
//*****************************************************************************
void RRLP_MSC_Log_Message
(
   s_RRLP_Message  *p_Message  ///< [in] Pointer to Message to be logged.
)
{
   char  *MsgSrcName = "",
         *MsgDestName = "",
         *MsgEventName = "";
   I4 MsgSrcId, MsgDestId;
   s_RRLP_Instance *p_srcInst  = (void *) -1;
   s_RRLP_Instance *p_destInst = (void *) -1;

   MsgDestName = "RRLP_HandlerPeer";
   p_destInst = RRLP_Instance_Get_Data( p_Message->MsgHdr.p_InstanceOfDest );
   if ( p_destInst != NULL )
   {
       MsgDestId=p_destInst->ThisInstanceId;
   }
   else
   {
       // Destination Id no longer in use.
       MsgDestId = -1;
   }

   MsgSrcName = "RRLP_HandlerPeer";
   if ( p_Message->MsgHdr.p_InstanceOfSrc == NULL )
   {
      MsgSrcId = 0;
   }
   else
   {
      p_srcInst = RRLP_Instance_Get_Data( p_Message->MsgHdr.p_InstanceOfSrc );
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

   MsgEventName = RRLP_Event_Text[p_Message->Payload.MsgEvent];
   GN_RRLP_Log( "Event:    %s->%s [label=\"%s SrcId=%d, DestId=%d, Handle=%p\"];",
      MsgSrcName, MsgDestName, MsgEventName, MsgSrcId, MsgDestId, p_Message->MsgHdr.Handle );
}
