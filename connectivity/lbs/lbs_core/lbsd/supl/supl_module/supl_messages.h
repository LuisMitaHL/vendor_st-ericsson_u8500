
//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_messages.h
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/supl_messages.h 1.33 2009/01/13 14:10:55Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_module
///
/// \file
/// \brief
///      SUPL Subsystem message declarations.
///
///      This header file contains declarations required to interface with the
///      SUPL subsystem. e.g. Message formats, instance ids etc.
///
//*************************************************************************

#ifndef SUPL_MESSAGES_H
#define SUPL_MESSAGES_H

#include "pdu_defs.h"
#include "supl_instance.h"
#include "GN_AGPS_api.h"
#include "GN_GPS_api.h"

/// Event identifier used to distinguish between the message types for #s_SUPL_Message.
typedef enum {
   event_SUPL_Initialisation,          ///< Event identifier set in #SUPL_Send_Initialisation and handled by #SUPL_Handle_Initialisation.

   event_SUPL_Server_Open_Success,     ///< Event identifier set in #SUPL_Send_Server_Open_Success and handled by #SUPL_Handle_Server_Open_Success.
   event_SUPL_Comms_Open_Request,      ///< Event identifier set in #SUPL_Send_Comms_Open_Request and handled by #SUPL_Handle_Comms_Open_Request.
   event_SUPL_Comms_Open_Success,      ///< Event identifier set in #SUPL_Send_Comms_Open_Success and handled by #SUPL_Handle_Comms_Open_Success.
   event_SUPL_Comms_Open_Failure,      ///< Event identifier set in #SUPL_Send_Comms_Open_Failure and handled by #SUPL_Handle_Comms_Open_Failure.
   event_SUPL_Comms_Close,             ///< Event identifier set in #SUPL_Send_Comms_Close and handled by #SUPL_Handle_Comms_Close.

   event_SUPL_PDU_Received,            ///< Event identifier set in #SUPL_Send_PDU_Received and handled by #SUPL_Handle_PDU_Received.
   event_SUPL_POS_Payload,             ///< Event identifier set in #SUPL_Send_POS_Payload and handled by #SUPL_Handle_POS_Payload.
   event_SUPL_POS_Sequence_Complete,   ///< Event identifier set in #SUPL_Send_POS_Sequence_Complete and handled by #SUPL_Handle_POS_Sequence_Complete.
   event_SUPL_POS_Sequence_Error,       ///< Event identifier set in #SUPL_Send_POS_Sequence_Complete and handled by #SUPL_Handle_POS_Sequence_Error.

   event_SUPL_Notification_Request,    ///< Event identifier set in #SUPL_Send_Notification_Request and handled by #SUPL_Handle_Notification_Request.
   event_SUPL_Notification_Response,   ///< Event identifier set in #SUPL_Send_Notification_Response and handled by #SUPL_Handle_Notification_Response.
   event_SUPL_MO_Position_Requested,   ///< Event identifier set in #SUPL_Send_MO_Position_Requested and handled by #SUPL_Handle_MO_Position_Requested.
/* +LMSqc38060 */
   event_SUPL_Abort_Requested,         ///< Event identifier set in #SUPL_Send_Abort_Requested and handled by #SUPL_Handle_Abort_Requested.   
/* -LMSqc38060 */
   
   event_SUPL_Trigger_End_Requested,         ///< Event identifier set in #SUPL_Send_Trigger_End_Request and handled by #SUPL_Handle_Abort_Requested.   
   event_SUPL_Expiry_UT1,              ///< Event identifier set in #GN_SUPL_Timer_Expired_UT1 and handled by #SUPL_Handle_Expiry_UT1.
   event_SUPL_Expiry_UT2,              ///< Event identifier set in #GN_SUPL_Timer_Expired_UT2 and handled by #SUPL_Handle_Expiry_UT2.
   event_SUPL_Expiry_UT3,              ///< Event identifier set in #GN_SUPL_Timer_Expired_UT3 and handled by #SUPL_Handle_Expiry_UT3.

   event_SUPL_V2_Trigger,              ///< Event identifier set in #GN_SUPL_Trigger_Ind_In and handlef by #SUPL_V2_Handle_Trigger
   event_SUPL_V2_Trigger_Periodic_Requested,  ///< Event identifier set in #GN_SUPL_TriggerPeriodic_Req_In and handlef by #SUPL_V2_Handle_Trigger_Periodic_Requested  
   event_SUPL_V2_Third_Party_info_Request,   ///< Event identifier set in #SUPL_Send_ThirdParty_Info and handlef by #SUPL_V2_Handle_Trigger_Periodic_Requested    
   event_SUPL_V2_ApplicationId_info_Request, ///< Event identifier set in #SUPL_Send_ThirdParty_Info and handlef by #SUPL_V2_Handle_Trigger_Periodic_Requested    
   event_SUPL_V2_ThirdPartyPosReq_Received, ///<Event identifier set in #SUPL_Send_TargetSet_Info and handled by #SUPL_V2_Handle_ThirdPartyPosReq
   event_SUPL_V2_Mobile_Info_Received,      ///<Event identifier set in #GN_SUPL_Mobile_Info_Ind_In and handled by #SUPL_V2_Mobile_Info_Received
   event_SUPL_V2_Delete_Location_id_Requested, ///<Event identifier set in #GN_SUPL_Mobile_Info_del and handled by #SUPL_V2_Delete_Location_id_Requested
   event_SUPL_V2_Expiry_UT5,           ///< Event identifier set in #GN_SUPL_Timer_Expired_UT5 and handled by #SUPL_Handle_Expiry_UT5.
   event_SUPL_V2_Expiry_UT6,           ///< Event identifier set in #GN_SUPL_Timer_Expired_UT6 and handled by #SUPL_Handle_Expiry_UT6.
   event_SUPL_V2_Expiry_UT7,           ///< Event identifier set in #GN_SUPL_Timer_Expired_UT7 and handled by #SUPL_Handle_Expiry_UT7.
   event_SUPL_V2_Expiry_UT8,           ///< Event identifier set in #GN_SUPL_Timer_Expired_UT8 and handled by #SUPL_Handle_Expiry_UT8.
   event_SUPL_V2_Expiry_UT9,           ///< Event identifier set in #GN_SUPL_Timer_Expired_UT9 and handled by #SUPL_Handle_Expiry_UT9.
   event_SUPL_V2_Expiry_UT10,          ///< Event identifier set in #GN_SUPL_Timer_Expired_UT10 and handled by #SUPL_Handle_Expiry_UT10.

} e_SUPL_event;

/// Contains routing information of message
typedef struct SUPL_MsgHdr
{
   void*                Handle;                 ///< Abstract Identifier.
   s_SUPL_Instance      *p_InstanceOfDest;      ///< Identfies the instance of the destination.
   s_SUPL_Instance      *p_InstanceOfSrc;       ///< Identfies the instance of the source.
} s_SUPL_MsgHdr;

/// The SUPL payload contains the data specific to the SUPL subsystem in messages.
typedef struct SUPL_Payload
{
   e_SUPL_event         MsgEvent;               ///< Entity which defines the type of the message.
   U4                   PDU_TimeStamp;          ///< Time at which the PDU was received by SUPL.
   s_PDU_Buffer_Store   *p_PDU;                 ///< Data associated with #event_SUPL_PDU_Received
   s_GN_AGPS_GAD_Data   *p_GN_AGPS_GAD_Data;    ///< Used to populate velocity in SUPL_POS.
   s_GN_GPS_Nav_Data    *p_GN_GPS_Nav_Data;     ///< Used to populate Cache.
   s_LocationId         *p_Location_Id;         ///<Location ID Information
   BL                   Notification_Accepted;  ///< Identifies whether notification was accepted.
   s_GN_SUPL_Connect_Req_Parms     *p_GN_SUPL_Connect_Req_Parms;         ///< Parameters for connection reques message.
   s_GN_SUPL_V2_Triggers           *p_Triggers;                          ///< Periodic or Event Triggers     
   s_GN_SUPL_V2_ThirdPartyInfo     *p_Third_Party_Info;                  ///<  ThirdParty Info
   s_GN_SUPL_V2_ApplicationInfo    *p_SUPL_ApplicationIDInfo;            ///<  Application Id Info
} s_SUPL_Payload;

/// The message contains a header, pointers to precedent and subsequent messages and a payload.
typedef struct SUPL_Message
{
   s_SUPL_MsgHdr        MsgHdr;                 ///< Message header instructions
   struct SUPL_Message  *p_PrevMsg;             ///< Pointer to previous message in queue (NULL indicates at head).
   struct SUPL_Message  *p_NextMsg;             ///< Pointer to next message in queue (NULL indicates at tail).
   s_SUPL_Payload       Payload;                ///< Payload of the Message.
} s_SUPL_Message;

/// The implementation of a queue for the SUPL subsystem.
typedef struct SUPL_queue
{
   s_SUPL_Message       *p_Head;                ///< Pointer to the head of the queue.
   s_SUPL_Message       *p_Tail;                ///< Pointer to the tail of the queue.
} s_SUPL_Queue;

extern s_SUPL_Queue     SUPL_Queue;             ///< Queue for SUPL Subsystem messages.

s_SUPL_Message* SUPL_Message_Init(           s_SUPL_Instance *p_DestInstance, s_SUPL_Instance *p_SrcInstance );
void            SUPL_Queue_Add_Message(      s_SUPL_Message *p_Message ) ;
s_SUPL_Message* SUPL_Queue_Retrieve_Message( void );
void            SUPL_Message_Free(           s_SUPL_Message *p_Message );

#endif // SUPL_MESSAGES_H
