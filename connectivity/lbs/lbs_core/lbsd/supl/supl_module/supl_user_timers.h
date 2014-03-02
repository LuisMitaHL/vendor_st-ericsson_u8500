
//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_user_timers.h
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/supl_user_timers.h 1.10 2009/01/13 14:10:56Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_module
///
/// \file
/// \brief
///      SUPL User Timers declarations.
///
///      The SUPL standards define 4 user timers UT1, UT2, UT3 and UT4
///      the support for starting and stopping timers on a per instance
///      basis is included within this section.
///
//*************************************************************************

#ifndef SUPL_TIMERS_H
#define SUPL_TIMERS_H

#include "gps_ptypes.h"

/// Timer data for handling SUPL timeouts.
typedef struct GN_GPS_SUPL_Timer
{
   BL Timer_Active;        ///< Flag to indicate if the Timer is active.
   U4 Timer_Duration_ms;   ///< Duration of timer.
   U4 Time_Activated_ms;   ///< Base marker to indicate when the timer was activated.
} s_GN_GPS_SUPL_Timer;


void GN_SUPL_Timer_Set(     s_GN_GPS_SUPL_Timer *p_Timer, U4 Duration_ms );
void GN_SUPL_Timer_Clear(   s_GN_GPS_SUPL_Timer *p_Timer );
BL   GN_SUPL_Timer_Expired( s_GN_GPS_SUPL_Timer *p_Timer );
BL   GN_SUPL_Get_Current_Timer( s_GN_GPS_SUPL_Timer *p_Timer , U4 *count_ms );
#endif   // SUPL_TIMERS_H
