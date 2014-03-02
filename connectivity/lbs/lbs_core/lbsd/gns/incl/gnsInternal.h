/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/
#ifndef GNS_INTERNAL_H
#define GNS_INTERNAL_H

#ifndef __RTK_E__
#include "agpsfsm.h"
#else
#include "master.h"
#endif

#include "gns.h"
#ifdef AGPS_FTR
#include "gnsCP_Api.h"
#endif /* AGPS_FTR */
#ifdef AGPS_UP_FTR
#include "gnsSUPL_Api.h"
#include "gnsWireless_Api.h"
#endif /* AGPS_UP_FTR */
#ifdef GNS_CELL_INFO_FTR
#include "gnsCellInfo_Api.h"
#endif

#ifdef GPS_FREQ_AID_FTR
#include "gnsFA.h"
#endif
#ifdef AGPS_TIME_SYNCH_FTR
#include "gnsFTA_Api.h"

#endif

#include "gnsEE.h"
#ifndef __RTK_E__
#define PROCESS_CGPS    AGPS_FIRST_VALID_PROCESS_ID
#define PROCESS_LSIM    (AGPS_FIRST_VALID_PROCESS_ID+1)
#define PROCESS_SBEE    (AGPS_FIRST_VALID_PROCESS_ID+2)
#define PROCESS_LSIMUP  PROCESS_LSIM
#define PROCESS_LSIMCP  PROCESS_LSIM
#define PROCESS_LSIMCCM   PROCESS_LSIM
#define PROCESS_LSIMEE   PROCESS_LSIM
#endif

#ifdef AGPS_FTR
void    GNS_ExecuteCpCallback( e_gnsCP_MsgType v_MsgType , uint32_t v_MsgLen, u_gnsCP_MsgData *pp_MsgData );
#endif

#ifdef AGPS_UP_FTR
void    GNS_ExecuteSuplCallback( e_gnsSUPL_MsgType v_MsgType , uint32_t v_MsgLen, u_gnsSUPL_MsgData *pp_MsgData );
void    GNS_ExecuteWirelessCallback(e_gnsWIRELESS_MsgType v_MsgType , uint32_t v_MsgLen, u_gnsWIRELESS_MsgData *pp_MsgData );

#endif

#ifdef GNS_CELL_INFO_FTR
void    GNS_ExecuteCellInfoCallback( e_gnsCellInfo_MsgType v_MsgType , uint32_t v_MsgLen, u_gnsCellInfo_MsgData *pp_MsgData );
#endif

#ifdef GPS_FREQ_AID_FTR
void GNS_ExecuteFaCallback(e_gnsFA_MsgType v_MsgType, uint32_t v_MsgLen, u_gnsFA_MsgDataOut *pp_MsgData);
#endif

#ifdef AGPS_TIME_SYNCH_FTR
void GNS_ExecuteFtaCallback(e_gnsFTA_MsgType v_MsgType, uint32_t v_MsgLen, u_gnsFTA_MsgDataOut *pp_MsgData);
#endif

#endif
