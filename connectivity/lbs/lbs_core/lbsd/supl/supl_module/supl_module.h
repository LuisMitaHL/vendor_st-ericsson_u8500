
//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_module.h
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/supl_module.h 1.18 2009/01/13 14:10:56Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_module
///
/// \file
/// \brief
///      SUPL Module header declarations.
///
///      This header file contains declarations used by the SUPL Module
///
//*************************************************************************

#ifndef SUPL_MODULE_H
#define SUPL_MODULE_H

#include "supl_messages.h"

extern s_SUPL_Queue     SUPL_Queue;

void SUPL_Module_Init( void );

void SUPL_Instance_Handler( void );
void SUPL_Instance_Process( s_SUPL_Instance *p_SUPL_Instance );
void SUPL_Segmented_PDU_Buffer_Init(void);

// Local message handling functions.
void SUPL_Handle_Initialisation(         s_SUPL_Message* p_ThisMessage );
void SUPL_Handle_Server_Open_Success(    s_SUPL_Message* p_ThisMessage );
void SUPL_Handle_PDU_Received(           s_SUPL_Message* p_ThisMessage );
void SUPL_Handle_Comms_Open_Failure(     s_SUPL_Message* p_ThisMessage );
void SUPL_Handle_Comms_Close(            s_SUPL_Message* p_ThisMessage );
void SUPL_Handle_POS_Payload(            s_SUPL_Message* p_ThisMessage );
void SUPL_Handle_POS_Sequence_Error(  s_SUPL_Message* p_ThisMessage );
void SUPL_Handle_POS_Sequence_Complete(  s_SUPL_Message* p_ThisMessage );
void SUPL_Handle_Expiry_UT1(             s_SUPL_Message* p_ThisMessage );
void SUPL_Handle_Expiry_UT2(             s_SUPL_Message* p_ThisMessage );
void SUPL_Handle_Expiry_UT3(             s_SUPL_Message* p_ThisMessage );
void SUPL_V2_Handle_Expiry_UT5(             s_SUPL_Message* p_ThisMessage );
void SUPL_V2_Handle_Expiry_UT7(             s_SUPL_Message* p_ThisMessage );
void SUPL_V2_Handle_Expiry_UT8(             s_SUPL_Message* p_ThisMessage );
void SUPL_V2_Handle_Expiry_UT9(             s_SUPL_Message* p_ThisMessage );

void SUPL_Handle_Notification_Request(   s_SUPL_Message* p_ThisMessage );
void SUPL_Handle_Notification_Response(  s_SUPL_Message* p_ThisMessage );
void SUPL_Handle_MO_Position_Requested(  s_SUPL_Message* p_ThisMessage );
/* +LMSqc38060 */
void SUPL_Handle_Abort_Requested        (  s_SUPL_Message* p_ThisMessage );
/* -LMSqc38060 */
void SUPL_Handle_Trigger_End_Requested        (  s_SUPL_Message* p_ThisMessage );

void SUPL_Handle_PDU_SUPL_POS( void *p_PDU_Decoded, s_SUPL_Instance *p_SUPL_Instance, U4 PDU_TimeStamp );
void SUPL_Handle_PDU_SUPL_END( void *p_PDU_Decoded, s_SUPL_Instance *p_SUPL_Instance );

void GN_SUPL_Timer_Set_UT1(     s_SUPL_Instance *p_SUPL_Instance );
void GN_SUPL_Timer_Clear_UT1(   s_SUPL_Instance *p_SUPL_Instance );
BL   GN_SUPL_Timer_Expired_UT1( s_SUPL_Instance *p_SUPL_Instance );

void GN_SUPL_Timer_Set_UT2(     s_SUPL_Instance *p_SUPL_Instance );
void GN_SUPL_Timer_Clear_UT2(   s_SUPL_Instance *p_SUPL_Instance );
BL   GN_SUPL_Timer_Expired_UT2( s_SUPL_Instance *p_SUPL_Instance );

void GN_SUPL_Timer_Set_UT3(     s_SUPL_Instance *p_SUPL_Instance );
void GN_SUPL_Timer_Clear_UT3(   s_SUPL_Instance *p_SUPL_Instance );
BL   GN_SUPL_Timer_Expired_UT3( s_SUPL_Instance *p_SUPL_Instance );


void GN_SUPL_V2_Timer_Set_UT5(     s_SUPL_Instance *p_SUPL_Instance );
void GN_SUPL_V2_Timer_Clear_UT5 ( s_SUPL_Instance *p_SUPL_Instance );
BL GN_SUPL_V2_Timer_Expired_UT5( s_SUPL_Instance *p_SUPL_Instance );

void GN_SUPL_V2_Timer_Set_UT7(     s_SUPL_Instance *p_SUPL_Instance );
void GN_SUPL_V2_Timer_Clear_UT7( s_SUPL_Instance *p_SUPL_Instance );
BL   GN_SUPL_V2_Timer_Expired_UT7( s_SUPL_Instance *p_SUPL_Instance );

void GN_SUPL_V2_Timer_Set_UT8(     s_SUPL_Instance *p_SUPL_Instance );
void GN_SUPL_V2_Timer_Clear_UT8( s_SUPL_Instance *p_SUPL_Instance );
BL   GN_SUPL_V2_Timer_Expired_UT8( s_SUPL_Instance *p_SUPL_Instance );

void GN_SUPL_V2_Timer_Set_UT9(     s_SUPL_Instance *p_SUPL_Instance );
void GN_SUPL_V2_Timer_Clear_UT9(   s_SUPL_Instance *p_SUPL_Instance );
BL   GN_SUPL_V2_Timer_Expired_UT9( s_SUPL_Instance *p_SUPL_Instance );

BL GN_SUPL_Cache_Valid( s_GN_Position *p_GN_Position, s_GN_QoP *p_GN_QoP );

BL GN_SUPL_PDU_Send( s_SUPL_Instance *p_SUPL_Instance, void *p_PDU_Src );


void SUPL_Handle_PDU_SUPL_NOTIFY( void *p_PDU_Decoded, s_SUPL_Instance *p_SUPL_Instance );

void SUPL_Handle_PDU_SUPL_REPORT( void *p_PDU_Decoded, s_SUPL_Instance *p_SUPL_Instance );

void SUPL_Handle_PDU_SUPL_TRIGGERED_RESPONSE( void *p_PDU_Decoded, s_SUPL_Instance *p_SUPL_Instance );

void SUPL_Handle_PDU_SUPL_TRIGGERED_STOP( void *p_PDU_Decoded, s_SUPL_Instance *p_SUPL_Instance );





#endif   // SUPL_MODULE_H
