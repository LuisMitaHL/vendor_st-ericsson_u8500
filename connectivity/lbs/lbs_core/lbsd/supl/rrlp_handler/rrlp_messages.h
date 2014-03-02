//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename rrlp_messages.h
//
// $Header: X:/MKS Projects/prototype/prototype/rrlp_handler/rcs/rrlp_messages.h 1.29 2009/01/13 14:09:43Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_handler
///
/// \file
/// \brief
///      RRLP Handler message declarations.
///
///      This header file contains declarations required to interface with the
///      RRLP Handler. e.g. Message formats, instance ids etc.
///
//*************************************************************************

#ifndef RRLP_MESSAGES_H
#define RRLP_MESSAGES_H

#include "pdu_defs.h"
#include "rrlp_instance.h"
#include "GN_RRLP_api.h"
#include "GN_GPS_api.h"
#include "GN_AGPS_api.h"

/// Event identifier used to distinguish between the message types for #s_RRLP_Message.
typedef enum {
   event_RRLP_Initiate_POS_Sequence,            ///< Event identifier set in #RRLP_Send_Initiate_POS_Sequence and handled by #RRLP_Handle_Initiate_POS_Sequence.
   event_RRLP_POS_Payload,                      ///< Event identifier set in #RRLP_Send_PDU_Delivery and handled by #RRLP_Handle_POS_Payload.
   event_RRLP_GPS_Position_Solution_Available,  ///< Event identifier set in #RRLP_Send_GPS_Position_Solution_Available and handled by #RRLP_Handle_GPS_Position_Solution_Available.
   event_RRLP_GPS_Measurements_Available,       ///< Event identifier set in #RRLP_Send_GPS_Measurements_Available and handled by #RRLP_Handle_GPS_Measurements_Available.
   event_RRLP_Terminate_POS_Sequence,           ///< Event identifier set in #RRLP_Send_Terminate_POS_Sequence and handled by #RRLP_Handle_Terminate_POS_Sequence.
   event_RRLP_Expiry_MeasureResponseTime        ///< Event identifier set in #RRLP_Send_Expiry_MeasureResponseTime and handled by #RRLP_Handle_Expiry_MeasureResponseTime.
} e_RRLP_event;

/// Contains routing information of message
typedef struct RRLP_MsgHdr
{
   void*                Handle;           ///< Abstract Identifier.
   s_RRLP_Instance      *p_InstanceOfDest;///< Identfies the instance of the destination.
   s_RRLP_Instance      *p_InstanceOfSrc; ///< Identfies the instance of the source.
} s_RRLP_MsgHdr;

/// The RRLP payload contains the data specific to the RRLP Handler in messages.
typedef struct RRLP_Payload
{
   e_RRLP_event         MsgEvent;            ///< Entity which defines the type of the message.
   U4                   PDU_TimeStamp;       ///< Time at which the PDU was received by SUPL.
   s_PDU_Encoded        *p_RRLP_PDU;         ///< pointer to RRLP PDU
   s_GN_AGPS_GAD_Data   *p_GN_AGPS_GAD_Data; ///< pointer to Reference Position in GAD format.
   U4                   GAD_Ref_TOW;         ///< GAD Data Reference GPS Time of Week [range 0..604799999 ms]
   I2                   GAD_Ref_TOW_Subms;   ///< GAD Data Reference GPS Time of Week Sub-millisecond part [range 0..9999 x 0.0001 ms,  -1 = Unknown]
   s_GN_GPS_Nav_Data    *p_GN_GPS_Nav_Data;  ///< pointer to Reference Position in Nav format.
   s_GN_AGPS_Meas       *p_GN_AGPS_Meas;     ///< pointer to set of latched measurements.
   s_GN_RRLP_QoP        *p_GN_RRLP_QoP;      ///< QoP criteria. NULL pointer if no QoP criteria set.
}s_RRLP_Payload;

/// The message contains a header, pointers to precedent and subsequent messages and a payload.
typedef struct RRLP_Message
{
   s_RRLP_MsgHdr        MsgHdr;        ///< Message header instructions
   struct RRLP_Message  *p_PrevMsg;    ///< Pointer to previous message in queue (NULL indicates at head).
   struct RRLP_Message  *p_NextMsg;    ///< Pointer to next message in queue (NULL indicates at tail).
   s_RRLP_Payload       Payload;       ///< Payload of the Message.
} s_RRLP_Message;

/// The implementation of a queue for the RRLP subsystem.
typedef struct RRLP_queue
{
   s_RRLP_Message       *p_Head;       ///< Pointer to the head of the queue.
   s_RRLP_Message       *p_Tail;       ///< Pointer to the tail of the queue.
} s_RRLP_Queue;

extern s_RRLP_Queue     RRLP_Queue;    ///< Queue for RRLP Subsystem messages.

s_RRLP_Message* RRLP_Message_Init( s_RRLP_Instance *p_DestInstance, s_RRLP_Instance *p_SrcInstance );
void RRLP_Queue_Add_Message( s_RRLP_Message *p_Message );
s_RRLP_Message* RRLP_Queue_Retrieve_Message( void );
void RRLP_Message_Free( s_RRLP_Message *p_Message );

#endif // RRLP_MESSAGES_H
