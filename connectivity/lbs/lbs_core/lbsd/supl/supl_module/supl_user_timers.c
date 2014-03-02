//*************************************************************************
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 
//
// Filename supl_user_timers.c
//
// $Header: X:/MKS Projects/prototype/prototype/supl_module/rcs/supl_user_timers.c 1.8 2009/01/13 14:10:56Z grahama Rel $
// $Locker: $
//*************************************************************************
///
/// \ingroup supl_module
///
/// \file
/// \brief
///      SUPL User Timers implementation.
///
///      The SUPL standards define 4 user timers UT1, UT2, UT3 and UT4
///      the support for starting and stopping timers on a per instance
///      basis is included within this section.
//*************************************************************************

#include "supl_user_timers.h"
#include "GN_GPS_api.h"

//*****************************************************************************
/// \brief
///      Function to set a timer of a certain duration.
//*****************************************************************************
void GN_SUPL_Timer_Set(
   s_GN_GPS_SUPL_Timer  *p_Timer,   ///< [in] Timer to  set.
   U4                   Duration_ms ///< [in] Duration before timer will elapse.
)
{
   p_Timer->Timer_Active      = TRUE;
   p_Timer->Time_Activated_ms = GN_GPS_Get_OS_Time_ms();
   p_Timer->Timer_Duration_ms = Duration_ms;
}

//*****************************************************************************
/// \brief
///      Function to clear a timer.
//*****************************************************************************
void GN_SUPL_Timer_Clear(
   s_GN_GPS_SUPL_Timer *p_Timer  ///< [in] Timer to clear.
)
{
   p_Timer->Timer_Active      = FALSE;
   p_Timer->Time_Activated_ms = 0;
   p_Timer->Timer_Duration_ms = 0;
}

//*****************************************************************************
/// \brief
///      Function to check whether a timer has expired.
/// \returns
///      Flag to indicate whether the timer has expired:
/// \retval #TRUE if the Timer_Duration has been exceeded since the timer was last set.
/// \retval #FALSE if the Timer_Duration has not been exceeded since the timer was last set.
//*****************************************************************************
BL GN_SUPL_Timer_Expired(
   s_GN_GPS_SUPL_Timer *p_Timer  ///< [in] Timer to check.
)
{
   U4 Current_OS_Time;
   U4 Time_Elapsed;

   if ( p_Timer->Timer_Active == FALSE ) return FALSE;
   if ( p_Timer->Timer_Duration_ms == 0 ) return FALSE;
   
   Current_OS_Time = GN_GPS_Get_OS_Time_ms();
   
   if ( Current_OS_Time < p_Timer->Time_Activated_ms )
   {
      Time_Elapsed = Current_OS_Time + ( 0xffffffff - p_Timer->Time_Activated_ms );
   }
   else
   {
      Time_Elapsed = Current_OS_Time - p_Timer->Time_Activated_ms;
   }
   if ( Time_Elapsed > p_Timer->Timer_Duration_ms )
   {
      GN_SUPL_Timer_Clear( p_Timer );
      return TRUE;
   }
   return FALSE;
}

//*****************************************************************************
/// \brief
///      Function to get the current timer value.
/// \returns
///      Flag to indicate whether the call is successful:
/// \retval #TRUE if the Timer value is calculated.
/// \retval #FALSE if the Timer value has not been calculated.
//*****************************************************************************
BL GN_SUPL_Get_Current_Timer( s_GN_GPS_SUPL_Timer *p_Timer , U4 *count_ms )
{
   U4 Time_Elapsed;
   U4 Current_OS_Time;

   if ( p_Timer->Timer_Active == FALSE ) return FALSE;
   if ( p_Timer->Timer_Duration_ms == 0 ) return FALSE;

   Current_OS_Time = GN_GPS_Get_OS_Time_ms();
   
   if ( Current_OS_Time < p_Timer->Time_Activated_ms )
   {
      Time_Elapsed = Current_OS_Time + ( 0xffffffff - p_Timer->Time_Activated_ms );
   }
   else
   {
      Time_Elapsed = Current_OS_Time - p_Timer->Time_Activated_ms;
   }

   *count_ms = Time_Elapsed;

   return TRUE;
}
