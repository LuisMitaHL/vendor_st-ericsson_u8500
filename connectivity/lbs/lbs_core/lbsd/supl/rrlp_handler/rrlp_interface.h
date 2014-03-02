//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename rrlp_interface.h
//
// $Header: X:/MKS Projects/prototype/prototype/rrlp_handler/rcs/rrlp_interface.h 1.23 2009/01/13 14:09:43Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_handler
///
/// \file
/// \brief
///      RRLP Handler interface declarations.
///
///      This header file contains declarations required to interface with the
///      RRLP Handler.
///
//*************************************************************************

#ifndef RRLP_INTERFACE_H
#define RRLP_INTERFACE_H

#include "rrlp_instance.h"

void RRLP_Handler_Init(    void );
void RRLP_Handler_DeInit(  void );
void RRLP_Message_Handler( void );

void RRLP_Send_Initiate_POS_Sequence(           s_RRLP_Instance *p_DestInstance, s_RRLP_Instance *p_SrcInstance, s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data );
void RRLP_Send_GPS_Position_Solution_Available( s_RRLP_Instance *p_DestInstance, s_RRLP_Instance *p_SrcInstance, s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data, U4 GAD_Ref_TOW, I2 GAD_Ref_TOW_Subms, s_GN_GPS_Nav_Data *p_GN_GPS_Nav_Data );
void RRLP_Send_GPS_Measurements_Available(      s_RRLP_Instance *p_DestInstance, s_RRLP_Instance *p_SrcInstance, s_GN_AGPS_Meas     *p_GN_AGPS_Meas );
void RRLP_Send_PDU_Delivery(                    s_RRLP_Instance *p_DestInstance, s_RRLP_Instance *p_SrcInstance, void               *Handle,             s_PDU_Encoded *p_PDU_Encoded, U4 PDU_TimeStamp, s_GN_RRLP_QoP *p_GN_RRLP_QoP );
void RRLP_Send_Terminate_POS_Sequence(      s_RRLP_Instance *p_SrcInstance, void               *Handle );

void RRLP_Process_protocolError(                void            *p_PDU_Decoded,  s_RRLP_Instance *p_RRLP_Instance );

#endif // RRLP_INTERFACE_H
