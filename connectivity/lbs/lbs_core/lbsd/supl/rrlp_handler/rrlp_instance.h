//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename rrlp_instance.h
//
// $Header: X:/MKS Projects/prototype/prototype/rrlp_handler/rcs/rrlp_instance.h 1.37 2009/01/14 14:27:48Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_handler
///
/// \file
/// \brief
///      RRLP Handler type declarations.
///
///      This header file contains declarations required to hold the instance
///      specific data for the RRLP Handler.
///
//*************************************************************************

#ifndef RRLP_INSTANCE_H
#define RRLP_INSTANCE_H
#include "Codec_Name_Changes.h"
#include "pdu_defs.h"
#include "GN_GPS_api.h"
#include "GN_RRLP_api.h"
#include "GN_AGPS_api.h"
#include "GN_RRLP_server_api.h"
#include "RRLP_GPS-AssistData.h"

/// Maximum number of RRLP instances valid at a time.
#define INSTANCE_RRLP_MAX   10
/// Maximum value of a RRLP instance.
#define INSTANCE_RRLP_LIMIT   0xffff

/// Enumeration identifying the permissable states within the RRLP subsystem
typedef enum {
    state_RRLP_Idle,
    state_RRLP_Transaction_In_Progress,
    state_RRLP_Assistance_Data_Delivery,
    state_RRLP_Measure_Position_Request,
    state_RRLP_Position_Requested_From_GPS
} e_RRLP_States;

/// Method type used in RRLP exchange.
typedef enum {
   GN_methodType_NONE,
   GN_msAssisted,    ///< msAssisted     AccuracyOpt,   -- accuracy is optional
   GN_msBased,       ///< msBased        Accuracy,      -- accuracy is mandatory
   GN_msBasedPref,   ///< msBasedPref    Accuracy,      -- accuracy is mandatory
   GN_msAssistedPref ///< msAssistedPref Accuracy       -- accuracy is mandatory
} e_GN_methodType;

/// \brief
///      Method type in an RRLP position instruct message.
/// \details
/// <pre>
///   MethodType ::= CHOICE {
///      msAssisted     AccuracyOpt,   -- accuracy is optional
///      msBased        Accuracy,      -- accuracy is mandatory
///      msBasedPref    Accuracy,      -- accuracy is mandatory
///      msAssistedPref Accuracy       -- accuracy is mandatory
///   }
/// </pre>
typedef struct GN_RRLP_MethodType {
   e_GN_methodType   methodType; ///< Actual method type.
   // -- The values of this field are defined in 3GPP TS 23.032 (Uncertainty code)
   I1                accuracy;   ///< Accuracy ::= INTEGER (0..127) (-1 = undefined (msAssisted optional))
} s_GN_RRLP_MethodType;

//typedef struct s_GN_RRLP_Meas_Set {
//   U1 MeasurementCount;
//   s_GN_AGPS_Meas GN_AGPS_Meas[3];
//} s_GN_RRLP_Meas_Set;

/// Timer data for handling RRLP timeouts.
typedef struct GN_GPS_RRLP_Timer
{
   BL Timer_Active;        ///< Flag to indicate if the Timer is active.
   U4 Timer_Duration_ms;   ///< Duration of timer.
   U4 Time_Activated_ms;   ///< Base marker to indicate when the timer was activated.
} s_GN_GPS_RRLP_Timer;


/// The RRLP instance data describes data needed for each instance of the RRLP Handler.
typedef struct RRLP_InstanceData
{
   e_RRLP_States     State;               ///< Current RRLP State for this instance.
   I1                ReferenceNumber;     ///< Reference Number provided by the server.
   U4                 Solution_ReqTimeStamp;
   U4                Solution_Deadline;   ///< Deadline in OS time for when the sequence should end.
   U4                Prev_OS_Time_ms;     ///< Store for the last time a measurement or position was retrieved.
   s_GN_RRLP_MethodType GN_RRLP_MethodType; ///< Method Type of client.
   e_GN_PrefMethod   GN_PrefMethod;       ///< Method Type requested by the client.
   s_PDU_Encoded     Payload;             ///< Contents of the positioning payload.
   s_GN_GPS_Nav_Data *p_GN_GPS_Nav_Data;  ///< Store for retrieved solution.
   s_GN_AGPS_GAD_Data
                     *p_GN_AGPS_GAD_Data;///< Store for retrieved solution.
   s_GN_AGPS_Meas    *p_GN_AGPS_Meas;     ///< Store for retrieved measurements.
//   s_GN_RRLP_Meas_Set *p_GN_RRLP_Meas_Set;
   s_GN_RRLP_QoP     *p_GN_RRLP_QoP;      ///< QoP criteria. NULL pointer if no QoP criteria set.
   s_GN_RequestedAssistData
                     GN_RequestedAssistData; ///< Structure for the assisted data requested.
   s_GN_GPS_RRLP_Timer
                     MsrResp_Timer;       ///< Timer to keep track of the measurement response time for an msAssisted fix.
   BL                LoggingDisabled;
} s_RRLP_InstanceData;

typedef U2 t_RRLP_InstanceId;             ///< Identifies the instance ID. Range [1..65535]

//*************************************************************************
/// \brief
///      Definition of the RRLP Subsystem instance type.
//*************************************************************************
typedef struct RRLP_Instance
{
   t_RRLP_InstanceId    ThisInstanceId;      ///< Identifies the instance ID. Range [1..65535]
   BL                   EventPending;        ///< Can be set during processing to indicate that more processing is necessary.
   void                 *Handle;             ///< Abstract Handle.
   s_RRLP_InstanceData  *p_RRLP;             ///< RRLP Handler specific data.
   struct RRLP_Instance *Peer_RRLP_Instance; ///< Indicates peer RRLP instance (NULL if invalid)
} s_RRLP_Instance;

//*************************************************************************
/// \brief
///      Definition of the current SUPL instances store.
//*************************************************************************
typedef struct RRLP_Instances
{
   U1                MaxInstanceCount;                   ///< Maximum number of instances supported.
   t_RRLP_InstanceId InstanceIdHighWaterMark;            ///< Identifies the highest assigned instance ID. Range [1..65535]
   s_RRLP_Instance   *InstanceList[INSTANCE_RRLP_MAX];   ///< Container for instance data.
} s_RRLP_Instances;

extern s_RRLP_Instances    RRLP_Instances;   ///< Container for all valid instances.

void              RRLP_Instance_Handler( void );

void              RRLP_Instances_Initialise(       U1 MaxInstanceCount );
void              RRLP_Instances_DeInitialise(     void );
s_RRLP_Instance   *RRLP_Instance_Get_Data(         s_RRLP_Instance *p_RRLP_Instance );

void              RRLP_Instance_Process(           s_RRLP_Instance *p_RRLP_Instance );

s_RRLP_Instance   *RRLP_Instance_Request_New(      void );
s_RRLP_Instance   *RRLP_Instance_Base_Request_New( void );

s_RRLP_Instance   *RRLP_Instance_From_Handle(      void *Handle );

void              RRLP_Instance_Delete(            s_RRLP_Instance *p_RRLP_Instance );
BL                RRLP_Instance_Base_Delete(       s_RRLP_Instance *p_RRLP_Instance );

void              RRLP_Instance_State_Transition(  s_RRLP_Instance *p_RRLP_Instance, e_RRLP_States New_State, char *Cause );

void              GN_RRLP_Log( const char *format, ... );
void              GN_RRLP_Log_AssistRecvd( void* handle , RRLP_GPS_AssistData_t *p_GPS_AssistData );
void              GN_RRLP_Log_PosRspTimeout( void* handle );
void              GN_RRLP_Log_MsgDecodeFailed( void* handle );
void              GN_RRLP_Log_PosReport( void* handle , s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data , U4 msTTFF );
void              GN_RRLP_Log_QoP(void* handle , s_GN_RRLP_QoP* p_GN_QoP );

void              GN_RRLP_Timer_Set(      s_GN_GPS_RRLP_Timer *p_Timer, U4 Duration_ms );
void              GN_RRLP_Timer_Clear(    s_GN_GPS_RRLP_Timer *p_Timer );
BL                GN_RRLP_Timer_Expired(  s_GN_GPS_RRLP_Timer *p_Timer );
BL                  GN_RRLP_GetTimerCount(  s_GN_GPS_RRLP_Timer *p_Timer, U4 *p_CountMs );
#endif // RRLP_INSTANCE_H
