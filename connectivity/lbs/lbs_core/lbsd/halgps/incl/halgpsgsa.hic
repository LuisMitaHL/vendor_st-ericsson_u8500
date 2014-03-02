/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSGSA_HIC__
#define __HALGPSGSA_HIC__

/**
* \file halgpsgsa.hic
* \date 17/07/2009
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain constants used by halgps.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 17.07.09 </TD><TD> R. GUPTA </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/


/*
VCTCXO case
2G worst case:
      0.05 ppm (BTS frequency tolerance)
    + 0.3  ppm (AFC algo tolerance in IDLE)
    + 0.12 ppm (doppler effect at 130km/h)
    -----------
      0.47 ppm (max error of RefClk)
3G worst case:
      0.05 ppm (BTS frequency tolerance)
    + 0.4  ppm (rake tolerance in IDLE)
    + 0.12 ppm (doppler effect at 130km/h)
    -----------
      0.57 ppm (max error of RefClk)
*/

/**
* \def HALGPS_GSA_BTS_FREQ_TOLERANCE
*
* Define the value for BTS frequency tolerance in parts per billion(ppb)\n
* The peak value is 50 and RMS is 100/3 = 16.67
*/
#define HALGPS_GSA_BTS_FREQ_TOLERANCE  33.33


/**
* \def HALGPS_GSA_DOPPLER_EFFECT
*
* Define the value for doppler effect at 130km/h in parts per billion(ppb)\n
* The peak value is 120 and RMS is 120/3 = 40
*/
#define HALGPS_GSA_DOPPLER_EFFECT     40


/**
* \def HALGPS_GSA_MAX_FREQ_ERROR
*
* Define the value for worst case frequency error in parts per billion(ppb)\n
* The value contained is RMS value with 800 assumed as peak value
*/
#define HALGPS_GSA_MAX_FREQ_ERROR     267

#endif /*__HALGPSGSA_HIC__*/

#ifdef HALGPS_PWM1_GPIO_06c
    /* Wave C */
    #define HALGPS_PWM_GPIO           gpio0_06c
    #define HALGPS_PWM_DEV_INST       0                      /* PWM 1 */
    #define HALGPS_SCON_MUX_GPIO      SCON_MUX_GPIO0_PWM0
    #define HALGPS_PWM_OR             pwmOr0 
    #define HALGPS_SCON_PWM_OR        SCON_PWM_OR_0_ONLY
#elif defined HALGPS_PWM2_GPIO_03c
    /* Wave B */
    #define HALGPS_PWM_GPIO           gpio0_03c
    #define HALGPS_PWM_DEV_INST       1                      /* PWM 2 */
    #define HALGPS_SCON_MUX_GPIO      SCON_MUX_GPIO0_PWM1
    #define HALGPS_PWM_OR             pwmOr1
    #define HALGPS_SCON_PWM_OR        SCON_PWM_OR_1_ONLY
#else
    #error "PWM configuration for Frequency aiding not defined correctly"
#endif


/* Following values are for PWM frequency = 200 Hz and duty cycle = 20%
*
*  HALGPS_GSA_PERIOD_VALUE(pf): 
*       PWM Frequency = 13MHz/(pf+1)
*
*  HALGPS_GSA_TIMER_VALUE(tmr):
*       Duty Cycle = tmr/(pf+1)
*/
#define HALGPS_GSA_FREQ           200
#define HALGPS_GSA_DUTY_CYCLE     20
#define HALGPS_GSA_DUTY_MODE      0
#define HALGPS_GSA_PERIOD_VALUE   64999
#define HALGPS_GSA_TIMER_VALUE    13000
