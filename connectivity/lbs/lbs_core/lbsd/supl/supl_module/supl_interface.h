
//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_interface.h
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/supl_interface.h 1.43 2009/01/13 14:10:54Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_module
///
/// \file
/// \brief
///      SUPL subsystem interface declarations.
///
///      This header file contains declarations required to interface with the
///      SUPL Subsystem.
///
//*************************************************************************

#ifndef SUPL_INTERFACE_H
#define SUPL_INTERFACE_H

#include "supl_messages.h"
#include "GN_GPS_api.h"
#include "GN_AGPS_api.h"

void SUPL_Module_Init(        void );
void SUPL_Module_DeInit(      void );
void SUPL_SubSystem_Init(     void );
void SUPL_SubSystem_DeInit(   void );
void SUPL_Message_Handler(    void );


void SUPL_Send_Initialisation(
   s_SUPL_Instance      *p_DestInstance,
   s_SUPL_Instance      *p_SrcInstance );

void SUPL_Send_Comms_Open_Success(
   s_SUPL_Instance      *p_DestInstance,
   s_SUPL_Instance      *p_SrcInstance,
   void                 *Handle );

void SUPL_Send_Comms_Open_Failure(
   s_SUPL_Instance      *p_DestInstance,
   s_SUPL_Instance      *p_SrcInstance,
   void                 *Handle );

void SUPL_Send_Comms_Close(
   s_SUPL_Instance      *p_DestInstance,
   s_SUPL_Instance      *p_SrcInstance,
   void                 *Handle );

void SUPL_Send_PDU_Received(
   s_SUPL_Instance      *p_DestInstance,
   s_SUPL_Instance      *p_SrcInstance,
   s_PDU_Buffer_Store   *p_PDU,
   U4                   PDU_TimeStamp,
   void                 *Handle );

void SUPL_Send_POS_Payload(
   s_SUPL_Instance      *p_DestInstance,
   s_SUPL_Instance      *p_SrcInstance,
   void                 *POS_Handle,
   s_PDU_Buffer_Store   *p_PDU,
   s_GN_AGPS_GAD_Data   *p_GN_AGPS_GAD_Data,
   s_GN_GPS_Nav_Data    *p_GN_GPS_Nav_Data );

void SUPL_Send_POS_Sequence_Complete(
   s_SUPL_Instance      *p_DestInstance,
   s_SUPL_Instance      *p_SrcInstance,
   void                 *POS_Handle );

void SUPL_Send_POS_Sequence_Error(
   s_SUPL_Instance      *p_DestInstance,
   s_SUPL_Instance      *p_SrcInstance,
   void                 *POS_Handle );

void SUPL_Send_Notification_Request(
   s_SUPL_Instance   *p_DestInstance,
   s_SUPL_Instance   *p_SrcInstance,
   void              *Handle
);
/* +LMSqc38060 */
void SUPL_Send_Abort_Requested(
   s_SUPL_Instance      *p_DestInstance,
   s_SUPL_Instance      *p_SrcInstance,
   void                 *Handle );
/* -LMSqc38060 */

void SUPL_Send_Notification_Response(
   s_SUPL_Instance      *p_DestInstance,
   s_SUPL_Instance      *p_SrcInstance,
   BL                   Notification_Accepted,
   void                 *POS_Handle );

void SUPL_Send_MO_Position_Requested(
   s_SUPL_Instance      *p_DestInstance,
   s_SUPL_Instance      *p_SrcInstance,
   void                 *POS_Handle,
   s_GN_SUPL_Connect_Req_Parms   *p_GN_SUPL_Connect_Req_Parms );

BL GN_SUPL_Connect_Req_Out_Wrapper(
   void* Handle,
   e_GN_Status* p_Status,
   s_GN_SUPL_Connect_Req_Parms* p_SUPL_Connect_Req_Parms );

BL GN_SUPL_Disconnect_Req_Out_Wrapper(
   void* Handle,
   e_GN_Status* p_Status );

BL GN_SUPL_PDU_Delivery_Out_Wrapper(
   void* Handle,
   e_GN_Status* p_Status,
   U2 PDU_Size,
   U1* p_PDU );

BL GN_SUPL_Notification_Req_Out_Wrapper(
   void* Handle,
   e_GN_Status* p_Status,
   s_GN_SUPL_Notification_Parms* p_SUPL_Notification_Parms );

BL GN_SUPL_Position_Resp_Out_Wrapper(
   void* Handle,
   e_GN_Status* p_Status,
   s_GN_SUPL_Pos_Data* p_GN_SUPL_Pos_Data );

BL GN_SUPL_ThirdPartyPosition_Resp_Out_Wrapper(
   void* Handle,
   e_GN_Status* p_Status,
   s_GN_SUPL_Pos_Data* p_GN_SUPL_Pos_Data );
CH* GN_SUPL_StatusDesc_From_Status( e_GN_Status GN_Status );

void SUPL_Send_Abort_Request(
   s_SUPL_Instance    *p_DestInstance,
   s_SUPL_Instance    *p_SrcInstance,
   void               *Handle
);

void SUPL_Send_Trigger_End_Request(
   s_SUPL_Instance    *p_DestInstance,
   s_SUPL_Instance    *p_SrcInstance,
   void               *Handle
);

void SUPL_Send_Trigger(
   s_SUPL_Instance    *p_DestInstance,
   s_SUPL_Instance    *p_SrcInstance,
   void               *Handle
);

void SUPL_Send_TriggerPeriodic_Request(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void            *Handle,             ///< [in] Opaque Handle used to coordinate requests.
   s_GN_SUPL_V2_Triggers *p_Triggers     ///< [in] Type of Trigger event and reporting values.
);


BL GN_SUPL_Check_AreaEvent
( 
    void*                     Handle,                   ///< [in] Opaque Handle used to coordinate requests.
    s_GN_SUPL_V2_Coordinate*  pl_CurrentPosition        /// <[in] Position for which event occurence needs to be checked against initial boundary conditions.
);

void SUPL_Send_ThirdParty_Info(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void            *Handle,             ///< [in] Opaque Handle used to coordinate requests.
   s_GN_SUPL_V2_ThirdPartyInfo *p_Third_Party_Info     ///< [in] ThirdParty Info
);

void SUPL_Send_TargetSet_Info(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void            *Handle,             ///< [in] Opaque Handle used to coordinate requests.
   s_GN_SUPL_V2_ThirdPartyInfo *p_Third_Party_Info  ///< [in] Target SET Info
);
void SUPL_Send_ApplicationId_Info(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   void            *Handle,             ///< [in] Opaque Handle used to coordinate requests.
   s_GN_SUPL_V2_ApplicationInfo  *p_SUPL_ApplicationIDInfo     ///< [in] Application Id Info
);

void SUPL_Send_Mobile_Info_Received(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance,     ///< [in] Entity instance where the message originated.
   s_LocationId*  p_SUPL_LocationId   ///< [in] Location ID required for the SUPL exchange.
);

void SUPL_Send_Delete_Location_id_Info(
   s_SUPL_Instance *p_DestInstance,    ///< [in] SUPL Agent instance to process the message.
   s_SUPL_Instance *p_SrcInstance     ///< [in] Entity instance where the message originated.
);

void SUPL_Update_Loc_Id( s_GN_SUPL_V2_Multiple_Loc_ID *AddNode );
void SUPL_Check_Loc_Id_Expiry( void );
void SUPL_Delete_LocationID_Data( void );

BL GN_SUPL_Check_AreaEvent_Wrapper
(
    void*          Handle,           ///< [in] Opaque Handle used to coordinate requests.
    s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data    ///< [in] GN A-GPS GAD (Geographical Area Description)
);



#endif // SUPL_INTERFACE_H
