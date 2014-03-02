//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_log.h
//
//*************************************************************************
///
/// \ingroup supl_module
///
/// \file
/// \brief
///      Handles logging of supl modules to the host system
///
///      
///
//*************************************************************************

#ifndef SUPL_LOG_H
#define SUPL_LOG_H

#include "Codec_Name_Changes.h"
#include "supl_instance.h"

void GN_SUPL_Log( const char *format, ... );
void GN_SUPL_Log_SuplMessageEncoded( void* handle , void  *p_PDU  );
void GN_SUPL_Log_SuplMessageDecoded( void* handle , void  *p_PDU  );
void GN_SUPL_Log_SuplEndStatus( void* handle , e_GN_StatusCode statusCode );
void GN_SUPL_Log_SessionStarted( void* handle );
void GN_SUPL_Log_SessionEnded( void* handle  );
void GN_SUPL_Log_QoP(void* handle , s_GN_QoP* p_GN_QoP );
void GN_SUPL_Log_PduIgnored( void* handle );
void GN_SUPL_Log_NoNetworkResource( void* handle );
void GN_SUPL_Log_ServerConnected( void *handle );
void GN_SUPL_Log_ServerDisconnected( void *handle );
void GN_SUPL_Log_MsgTimerExpiry( void *handle , char* timerName );
void GN_SUPL_Log_ServerNotFound( void *handle );
void GN_SUPL_Log_MsgDecodeFailed( void* handle );
void GN_SUPL_Log_MsgOutOfOrder( void* handle , void *p_PDU );
void* GN_SUPL_GetHandleFromRRLPHandle( void* handle ); 
void GN_SUPL_Log_CMCC_PosReport( void* handle ,s_GN_AGPS_GAD_Data *p_GN_AGPS_GAD_Data , U4 msTTFF );
void GN_SUPL_Log_CMCC_CachePosReport( void* handle ,s_GN_Position *p_GN_Position , U4 msTTFF );
void GN_SUPL_Log_CachePosReport( void* handle , s_GN_Position *p_GN_Position , U4 msTTFF );

#endif /* #ifndef SUPL_LOG_H */