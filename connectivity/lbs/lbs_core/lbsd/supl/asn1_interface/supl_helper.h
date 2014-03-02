//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_helper.h
//
// $Header: X:/MKS Projects/prototype/prototype/asn1_interface/rcs/supl_helper.h 1.57 2009/01/15 12:18:09Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_helper
///
/// \file
/// \brief
///      ASN.1 helper routines declarations.
///
///      Access routines specifically for using the SUPL encode/decode routines
///      in a way useful for the supl_subsystem.
///
//*************************************************************************

#ifndef SUPL_HELPER_H
#define SUPL_HELPER_H

#include <stdio.h>

#include "gps_ptypes.h"
#include "supl_ptypes.h"

#include "pdu_defs.h"
#include "GN_GPS_api.h"
#include "supl_instance.h"

///////////////////////////////////////////////////////////////////////////////
// Definitions for supl_asn1_abstraction
// The variable names used match those in the SUPL ASN.1 definitions.
// See the SUPL specification for a full description.

///////////////////////////////////////////////////////////////////////////////
void *supl_PDU_Make_SUPLPOSINIT(    s_SUPL_Instance *p_SUPL_Instance );
void *supl_PDU_Make_SUPLSTART(      s_SUPL_Instance *p_SUPL_Instance );
void *supl_PDU_Make_SUPLSETINIT(      s_SUPL_Instance *p_SUPL_Instance );
void *supl_PDU_Make_SUPLRESPONSE(   s_SUPL_Instance *p_SUPL_Instance );
void *supl_PDU_Make_SUPLEND(        s_SUPL_Instance *p_SUPL_Instance );
void *supl_PDU_Make_SUPLNOTIFYRESPONSE( s_SUPL_Instance *p_SUPL_Instance,   e_GN_StatusCode GN_StatusCode );
void *supl_PDU_Make_SUPLPOS(        s_SUPL_Instance *p_SUPL_Instance, s_PDU_Buffer_Store *p_PDU_RRLP, s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data );
void *supl_PDU_Make_SUPLTRIGGEREDSTART(s_SUPL_Instance *p_SUPL_Instance);
void *supl_PDU_Make_SUPLTRIGGEREDSTOP(s_SUPL_Instance *p_SUPL_Instance,e_GN_StatusCode GN_StatusCode );

void *supl_PDU_Make_SUPLREPORT(     s_SUPL_Instance *p_SUPL_Instance );
void supl_PDU_Get_SUPLINIT_EmergencyRequest (void *p_ThisPDU, BL *p_EmeregencyReq );
BL supl_PDU_Get_SUPLINIT_ESlp_Address ( void *p_ThisPDU, s_GN_SUPL_V2_InstanceData *p_SUPL_V2_InstanceData );

void *supl_PDU_Alloc(                        void );
void supl_PDU_Free(                          void *p_PDU );

void supl_PDU_Add_Version(                   void *p_ThisPDU,s_SUPL_Instance *p_SUPL_Instance );
void supl_PDU_Get_Version(                   void *p_ThisPDU, U1 *p_Major, U1 *p_Minor, U1 *p_Service_Indicator);

void supl_PDU_Add_Length(                    void *p_ThisPDU, U2 Length );

void supl_PDU_Add_setSessionID(              void *p_ThisPDU, U2 sessionID,       s_SetId *p_SetId );
BL supl_PDU_Get_setSessionID(                void *p_ThisPDU, U2 *p_sessionID,    s_SetId *p_SetId );
BL supl_PDU_Validate_setSessionID(           void *p_ThisPDU, U2 sessionID,       s_SetId *p_SetId );

void supl_PDU_Add_slpSessionID(              void *p_ThisPDU, U1 *p_sessionID,    s_GN_SLPAddress *p_SlpId );
BL supl_PDU_Get_slpSessionID(                void *p_ThisPDU, U1 **p_p_sessionID, s_GN_SLPAddress *p_SlpId );
BL supl_PDU_Validate_slpSessionID(           void *p_ThisPDU, U1 *p_sessionID,    s_GN_SLPAddress *p_SlpId );

void supl_Make_slpSessionID( U4 SessionId, U1 **p_p_sessionID, s_GN_SLPAddress *p_SlpId );

e_GN_UlpMessageType supl_PDU_Get_MessageType( void *p_ThisPDU );

void supl_PDU_Add_SUPLSTART(                 void *p_ThisPDU );
void supl_PDU_Add_SUPLSETINIT(                 void *p_ThisPDU );
void supl_PDU_Add_SUPLSTART_SetCapabilities( void *p_ThisPDU, s_GN_SetCapabilities *p_SetCapabilities,s_SUPL_Instance *p_SUPL_Instance);
void supl_PDU_Get_SUPLSTART_SetLocationID(   void *p_ThisPDU, s_LocationId *p_SetLocationID);
void supl_PDU_Add_SUPLSTART_SetLocationID(   void *p_ThisPDU, s_LocationId *p_SetLocationID);
void supl_PDU_Add_SUPLSTART_ThirdPartyInfo(   void *p_ThisPDU, s_GN_SUPL_V2_ThirdPartyInfo   *p_third_party_info);
void supl_PDU_Add_SUPLSTART_ApplicationID(   void *p_ThisPDU, s_GN_SUPL_V2_ApplicationInfo   *p_application_id);
void supl_PDU_Add_SUPLSETINIT_ThirdPartyInfo(   void *p_ThisPDU, s_GN_SUPL_V2_ThirdPartyInfo   *p_third_party_info);
void supl_PDU_Add_SUPLSETINIT_AppIdInfo(   void *p_ThisPDU, s_GN_SUPL_V2_ApplicationInfo   *p_AppId_Info);
void supl_PDU_Add_SUPLSETINIT_QoP( void *p_ThisPDU, U1 *p_horacc, U1 *p_veracc, U2 *p_maxLocAge, U1 *p_delay);

void supl_PDU_Add_SUPLTRIGGEREDSTART_ApplicationID(   void *p_ThisPDU, s_GN_SUPL_V2_ApplicationInfo   *p_application_id);
void supl_PDU_Add_SUPLTRIGGEREDSTART_ThirdPartyInfo(   void *p_ThisPDU, s_GN_SUPL_V2_ThirdPartyInfo   *p_third_party_info);

void supl_PDU_Add_SUPLRESPONSE(              void *p_ThisPDU );
void supl_PDU_Add_SUPLRESPONSE_PosMethod(    void *p_ThisPDU, e_GN_PosMethod GN_PosMethod );

void supl_PDU_Get_SUPLINIT_PosMethod(        void *p_ThisPDU, e_GN_PosMethod *p_GN_PosMethod );
BL supl_PDU_Get_SUPLINIT_Notification(       void *p_ThisPDU, s_GN_SUPL_Notification_Parms *p_SUPL_Notification );
BL supl_PDU_Get_SUPLINIT_MinMajorVer(   void *p_ThisPDU,  U1 *p_MinMajVer );
void supl_PDU_Get_SUPLINIT_Supported_Network_Info( void *p_ThisPDU, s_GN_SUPL_V2_SupportedNetworkInfo **p_p_SupportedNetworkInfo);
void supl_PDU_Get_SUPLRESPONSE_Supported_Network_Info( void *p_ThisPDU, s_GN_SUPL_V2_SupportedNetworkInfo **p_p_SupportedNetworkInfo);
void supl_PDU_Get_SUPLTRIGGEREDRESPONSE_Supported_Network_Info( void *p_ThisPDU, s_GN_SUPL_V2_SupportedNetworkInfo **p_p_SupportedNetworkInfo);

BL supl_PDU_Get_SUPLNOTIFY_Notification( void *p_ThisPDU, s_GN_SUPL_Notification_Parms *p_SUPL_Notification );
BL supl_PDU_Get_SUPLINIT_QoP(                void *p_ThisPDU, s_GN_QoP **p_p_GN_QoP );
void supl_PDU_Get_SUPLINIT_SLPMode(          void *p_ThisPDU, e_GN_SLPMode *p_GN_SLPMode );
BL supl_PDU_Get_SUPLINIT_TriggeredMode(      void *p_ThisPDU, e_GN_SUPL_V2_TriggeredMode *p_TriggeredMode );

void supl_PDU_Get_SUPLPOSINIT_RequestedAssistData( void *p_ThisPDU, s_GN_RequestedAssistData *p_GN_RequestedAssistData );
void supl_PDU_Add_SUPLPOSINIT_PosMethod(     void *p_ThisPDU, e_GN_PosMethod GN_PosMethod );
void supl_PDU_Get_SUPLPOSINIT_SetLocationID( void *p_ThisPDU, s_LocationId *p_SetLocationID );
void supl_PDU_Add_SUPLPOSINIT_Position(      void *p_ThisPDU, s_GN_Position *p_GN_Position );
void supl_PDU_Add_SUPLPOSINIT(               void *p_ThisPDU );

void supl_PDU_Add_SUPLEND_VER(               void *p_ThisPDU, s_SUPL_Instance *p_SUPL_Instance );
//void supl_PDU_Add_SUPLEND_Position(          void *p_ThisPDU, s_GN_GPS_Nav_Data *p_GN_GPS_Nav_Data );
BL supl_PDU_Get_SUPLEND_GAD_Position(        void *p_ThisPDU, s_GN_Position *p_GN_Position, s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data );
void supl_PDU_Add_SUPLEND_Position(          void *p_ThisPDU, s_GN_Position *p_GN_Position );
void supl_PDU_Add_SUPLEND_StatusCode(        void *p_ThisPDU, e_GN_StatusCode GN_StatusCode );
void supl_PDU_Add_SUPLEND(                   void *p_ThisPDU );
void supl_PDU_Add_SUPLNOTIFYRESPONSE(        void *p_ThisPDU );

void supl_PDU_Add_SUPLPOS_RRLP_Payload(      void *p_ThisPDU, s_PDU_Encoded *p_POS_Payload );
void supl_PDU_Add_SUPLPOS(                   void *p_ThisPDU );

void supl_PDU_Add_SUPLREPORT_SessionList(    void *p_ThisPDU,s_SUPL_Instance *p_SUPL_Instance );
void supl_PDU_Add_SUPLREPORT_Position(       void *p_ThisPDU, s_GN_Position  *p_GN_Position );

void supl_PDU_Add_SUPLTRIGGEREDSTART_Position(void *p_ThisPDU,s_GN_Position  *p_GN_Position);

BL supl_PDU_Get_SUPLTRIGGEREDRESPONSE_PeriodicParams(void *p_ThisPDU,s_GN_SUPL_V2_PeriodicTrigger  *p_TriggerParams);

BL supl_PDU_Get_SUPLTRIGGEREDRESPONSE_AreaParams( void *p_ThisPDU,  s_GN_SUPL_V2_AreaEventTrigger  *p_AreaEventParams,s_SUPL_Instance *p_SUPL_Instance);

BL supl_PDU_Check_SUPLTRIGGEREDRESPONSE_AreaParams( void *p_ThisPDU );

BL supl_PDU_Encode(                          void *handle , void *p_PDU_Src, s_PDU_Buffer_Store *p_PDU_Buf );

BL supl_PDU_Decode(                          void *handle , s_PDU_Buffer_Store *p_PDU_Buf, void **p_p_PDU_Dest, e_GN_StatusCode *p_GN_StatusCode);

BL supl_Process_SUPL_POS(                    void *p_ThisPDU, s_PDU_Encoded *p_PDU_Encoded, e_GN_StatusCode *p_GN_StatusCode );
void GN_SUPL_Position_From_Solution(         s_GN_Position      *p_GN_Position,      s_GN_GPS_Nav_Data  *p_GN_GPS_Nav_Data, s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data );
void GN_SUPL_Pos_Data_From_Nav_Data(         s_GN_SUPL_Pos_Data *p_GN_SUPL_Pos_Data, s_GN_GPS_Nav_Data  *p_GN_GPS_Nav_Data );
void GN_SUPL_Pos_Data_From_GAD_Data(         s_GN_SUPL_Pos_Data *p_GN_SUPL_Pos_Data, s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data );
void GN_SUPL_Pos_Data_From_Position(         s_GN_SUPL_Pos_Data *p_GN_SUPL_Pos_Data, s_GN_Position      *p_GN_Position );

BL supl_Copy_wcdmaCellInfo(                  s_wcdmaCellInfo *p_dst, s_wcdmaCellInfo *p_src );
void supl_Free_LocationId_Data(              s_LocationId *p_LocationId );

void supl_log_LocationId(                    CH *Prefix, s_LocationId *p_LocationId );

e_GN_PrefMethod supl_PDU_Get_SUPLSTART_PreferredMethod( void *p_ThisPDU );
e_GN_StatusCode supl_PDU_Get_SUPLEND_StatusCode(void *p_ThisPDU );
BL supl_PDU_Get_SUPLREPORT_GAD_Position(void *p_ThisPDU, s_GN_Position      *p_GN_Position, s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data );

BL supl_Is_Version2( s_SUPL_Instance  *p_SUPL_Instance );
#endif // SUPL_HELPER_H
