/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSTAT_HIC__
#define __CGPSTAT_HIC__
/**
* \file cgpstat.hic
* \date 23/06/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all internal defines used by cgps for tat management.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 02.04.08</TD><TD> M.BELOU </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/


/* \def F_CGPS_FIX_LOOP_TIMER_EXPIRY
* Fix loop timer expiry message definition
*/
#define F_CGPS_TAT_LOOP_TIMER_EXPIRY               MC_PCC_FUNCTION_TYPE(CGPS_F_TAT_LOOP_TIMER, PCC_T_TIM)

#define CGPS_TAT_LOOP_TIMER_EXPIRY                  MC_RTK_PROCESS_OPERATION(PROCESS_CGPS, F_CGPS_TAT_LOOP_TIMER_EXPIRY)
#define CGPS_TAT_LOOP_TIMER                        MC_RTK_PROCESS_TIMER(PROCESS_CGPS, 0, CGPS_TAT_LOOP_TIMER_EXPIRY)



#define CGPS_TAT_SLEEP_TIMEOUT  60

#define CGPS_TAT_COMA_TIMEOUT  0x7FFF       /*FIXME put the max, i don't known if 0xFFFFFFF is a reserved value */


/* \def F_CGPS_FIX_LOOP_TIMER_EXPIRY
* Fix loop timer expiry message definition
*/
#define F_CGPS_TAT_SLEEP_TIMER_EXPIRY               MC_PCC_FUNCTION_TYPE(CGPS_F_TAT_SLEEP_TIMER, PCC_T_TIM)

#define CGPS_TAT_SLEEP_TIMER_EXPIRY                  MC_RTK_PROCESS_OPERATION(PROCESS_CGPS, F_CGPS_TAT_SLEEP_TIMER_EXPIRY)
#define CGPS_TAT_SLEEP_TIMER                        MC_RTK_PROCESS_TIMER(PROCESS_CGPS, 0, CGPS_TAT_SLEEP_TIMER_EXPIRY)

#define K_CGPS_TAT_LOOP_TIMER_DURATION          100

#define CGPS_TAT_TRANSPARENT_READ_BUFFER_SIZE   16384 

#endif /*__CGPSTAT_HIC__ */
