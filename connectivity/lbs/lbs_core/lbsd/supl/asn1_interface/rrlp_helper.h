//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename rrlp_helper.h
//
// $Header: X:/MKS Projects/prototype/prototype/asn1_interface/rcs/rrlp_helper.h 1.28 2009/01/13 14:09:33Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup rrlp_helper
///
/// \file
/// \brief
///      ASN.1 helper routines declarations.
///
///      Access routines specifically for using the RRLP encode/decode routines
///      in a way useful for the rrlp_agent.
///
//*************************************************************************

#ifndef RRLP_HELPER_H
#define RRLP_HELPER_H

#include <stdio.h>

#include "gps_ptypes.h"
#include "supl_ptypes.h"
#include "rrlp_instance.h"

#include "GN_AGPS_api.h"
#include "pdu_defs.h"

///////////////////////////////////////////////////////////////////////////////
// definitions for rrlp_asn1_abstraction

/// Identifier for the RRLP message component.
/// <pre>
///   RRLP-Component ::= CHOICE {
///      msrPositionReq       MsrPosition-Req,
///      msrPositionRsp       MsrPosition-Rsp,
///      assistanceData       AssistanceData,
///      assistanceDataAck    NULL,
///      protocolError        ProtocolError,
///      ...
///      }
/// </pre>
typedef enum GN_RRLP_Component_PR{
   GN_RRLP_Component_PR_NOTHING, /* No components present */
   GN_RRLP_Component_PR_msrPositionReq,
   GN_RRLP_Component_PR_msrPositionRsp,
   GN_RRLP_Component_PR_assistanceData,
   GN_RRLP_Component_PR_assistanceDataAck,
   GN_RRLP_Component_PR_protocolError
} e_GN_RRLP_Component_PR;

/// Reason for location attempt failure.
/// <pre>
///   LocErrorReason ::= ENUMERATED {
///      unDefined (0),    
///      notEnoughBTSs (1),
///      notEnoughSats (2),
///      eotdLocCalAssDataMissing (3),
///      eotdAssDataMissing (4),
///      gpsLocCalAssDataMissing (5),
///      gpsAssDataMissing (6),
///      methodNotSupported (7),
///      notProcessed (8),
///      refBTSForGPSNotServingBTS (9),
///      refBTSForEOTDNotServingBTS (10),
///      ...
///      }
/// </pre>
typedef enum GN_RRLP_LocErrorReason{
   GN_RRLP_LocErrorReason_unDefined                   = 0,
   GN_RRLP_LocErrorReason_notEnoughBTSs               = 1,
   GN_RRLP_LocErrorReason_notEnoughSats               = 2,
   GN_RRLP_LocErrorReason_eotdLocCalAssDataMissing    = 3,
   GN_RRLP_LocErrorReason_eotdAssDataMissing          = 4,
   GN_RRLP_LocErrorReason_gpsLocCalAssDataMissing     = 5,
   GN_RRLP_LocErrorReason_gpsAssDataMissing           = 6,
   GN_RRLP_LocErrorReason_methodNotSupported          = 7,
   GN_RRLP_LocErrorReason_notProcessed                = 8,
   GN_RRLP_LocErrorReason_refBTSForGPSNotServingBTS   = 9,
   GN_RRLP_LocErrorReason_refBTSForEOTDNotServingBTS  = 10
} e_GN_RRLP_LocErrorReason;

/// Error codes for RRLP protocol failure.
/// <pre>
///   -- Protocol Error Causes
///   ErrorCodes ::= ENUMERATED {
///      unDefined (0),
///      missingComponet (1),            
///      incorrectData (2),            
///      missingIEorComponentElement (3),        
///      messageTooShort (4),            
///      unknowReferenceNumber (5),        
///      ...
///      }
/// </pre>
typedef enum GN_RRLP_ErrorCodes{
   GN_RRLP_ErrorCodes_NONE,
   GN_RRLP_ErrorCodes_unDefined,
   GN_RRLP_ErrorCodes_missingComponet,
   GN_RRLP_ErrorCodes_incorrectData,
   GN_RRLP_ErrorCodes_missingIEorComponentElement,
   GN_RRLP_ErrorCodes_messageTooShort,
   GN_RRLP_ErrorCodes_unknowReferenceNumber
} GN_RRLP_ErrorCodes;

///////////////////////////////////////////////////////////////////////////////
void *rrlp_PDU_Alloc( void);
void rrlp_PDU_Free( void *p_PDU );

BL rrlp_PDU_Encode( void *p_PDU_Src, s_PDU_Buffer_Store *p_PDU_Buf );
BL rrlp_PDU_Decode( s_PDU_Buffer_Store *p_PDU_Buf, void **p_p_PDU_Dest, GN_RRLP_ErrorCodes *p_GN_RRLP_ErrorCodes );

e_GN_RRLP_Component_PR rrlp_PDU_Get_MessageType(      void *p_ThisPDU );
void *rrlp_PDU_Make_assistanceData(    U2 ReferenceNumber, s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data, s_GN_RequestedAssistData *p_GN_RequestedAssistData );
void *rrlp_PDU_Make_assistanceDataAck( U2 ReferenceNumber );
void *rrlp_PDU_Make_msrPositionReq(    U2 ReferenceNumber, s_GN_RRLP_MethodType *p_GN_RRLP_MethodType );
void *rrlp_PDU_Make_msrPositionRsp(    U2 ReferenceNumber );
void *rrlp_PDU_Make_protocolError(     U2 ReferenceNumber, GN_RRLP_ErrorCodes ErrorCode );

void rrlp_PDU_ProcessAssistanceData(                  void* handle , void *p_ThisPDU, U4 PDU_TimeStamp );

U1   rrlp_PDU_Get_Reference_Number(                   void *p_PDU_Src );
void rrlp_PDU_Set_Reference_Number(                   void *p_PDU_Src, U2 ReferenceNumber );
BL rrlp_PDU_Get_PositionInstruct_measureResponseTime( void *p_ThisPDU, U1 *p_ResponseTime , I1 *p_GADResponseTime );
void rrlp_PDU_Add_msrPositionRsp_locationInfo(        void *p_PDU_Src, s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data, I2 GAD_Ref_TOW_Subms );
void rrlp_PDU_Add_msrPositionRsp_gps_MeasureInfo(     void *p_PDU_Src, U1 NumMsrResults, s_GN_AGPS_Meas GN_AGPS_Meas[3] );
void rrlp_PDU_Add_msrPositionRsp_locErrorReason(      void *p_PDU_Src, e_GN_RRLP_LocErrorReason GN_RRLP_LocErrorReason );
BL   rrlp_PDU_Get_PositionInstruct_methodType(        void *p_PDU_Src, s_GN_RRLP_MethodType *p_GN_RRLP_MethodType );

#endif // RRLP_HELPER_H
