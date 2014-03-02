/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) 2009 ST-Ericsson.
 *  All rights reserved
 *  Date: 03-12-2009
 *  Author: Rahul Ranade
 *  Email : rahul.ranade@stericsson.com
 *****************************************************************************/
#ifndef GNS_FSM_H
#define GNS_FSM_H

/* This file is only included in gns.c. Including in other locations will result in link errors */



#ifdef AGPS_LINUX_FTR 

#include "cgps.h"
#if defined( AGPS_FTR ) || defined ( AGPS_UP_FTR )
#include "lsim.h"
#endif /* #if defined( AGPS_FTR ) || defined ( AGPS_UP_FTR ) */

extern t_OperationDescriptor a_CGPSInitState[];
void CGPS0_00Init(void);

#if defined( AGPS_FTR ) || defined ( AGPS_UP_FTR )
extern t_OperationDescriptor a_LSIMInitState[];
void LSIM0_00Init(void);
#endif /* #if defined( AGPS_FTR ) || defined ( AGPS_UP_FTR ) */
#if defined( AGPS_SBEE_FTR )
extern t_OperationDescriptor a_SbeeNotInitialized[];
void sbee00_00Init();
#endif /* #if defined( AGPS_SBEE_FTR ) */




AGPS_START_ALL_FSM( g_AgpsFsmTable )
/*LSIM fsm initialization has to be done first as CGPS0_00Init sends messages to LSIM which require the LSIM FSM to be initialized & ready*/
#if defined( AGPS_FTR ) || defined ( AGPS_UP_FTR )
AGPS_NEW_FSM( PROCESS_LSIM, a_LSIMInitState, LSIM0_00Init, OSA_THREAD_PRIORITY_NORMAL ) 
#endif /* #if defined( AGPS_FTR ) || defined ( AGPS_UP_FTR ) */
AGPS_NEW_FSM( PROCESS_CGPS , a_CGPSInitState , CGPS0_00Init, OSA_THREAD_PRIORITY_NORMAL ) 
#if defined( AGPS_SBEE_FTR )
AGPS_NEW_FSM( PROCESS_SBEE, a_SbeeNotInitialized, sbee00_00Init, OSA_THREAD_PRIORITY_NORMAL ) 
#endif
AGPS_FINISH_ALL_FSM( g_AgpsFsmTable )

#endif

#endif /* GNS_FSM_H */
