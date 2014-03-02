/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __CGPSMAIN_HIC__
#define __CGPSMAIN_HIC__
/**
* \file cgpsmain.hic
* \date 02/04/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all defines used by cgps0Main.c.\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 02.04.08</TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
*     <TR>
*             <TD> 07.05.08</TD><TD> Y.DESAPHI </TD><TD> SUPL implementation </TD>
*     </TR>
* </TABLE>
*/


/* FIXME used Share1 for the DEBUG/DEMO SESSION */
/* FIXME The official delivery must used the APP volume */


/**
* \enum e_gps_state
*
* define the current GPS State.
*
*/
typedef enum
{
    K_CGPS_GPS_OFF,
    K_CGPS_GPS_RUNNING,
    K_CGPS_GPS_COMA,   /*used only for TAT*/
    K_CGPS_GPS_SLEEP,
    K_CGPS_MAX_GPS_STATE = 0x7FFFFFFF
} e_gps_state;


/**
* \enum e_cgps_state
*
* define the current CGPS State
*
*/
typedef enum
{
    K_CGPS_INACTIVE,
    K_CGPS_ACTIVE_GPS_ON,
    K_CGPS_ACTIVE_GPS_OFF,
    K_CGPS_MAX_STATE = 0x7FFFFFFF
} e_cgps_state;


/**
* \enum e_naf_context_state
*
* define the GPS context state of an application
*
*/
typedef enum
{
    K_CGPS_NAF_UNUSED,         /**< Indicates NAF session is unregistered and session context is free.*/
    K_CGPS_NAF_REGISTERED,     /**< Indicates NAF session is registered with application but no positioning request has been placed */
    K_CGPS_NAF_READY,          /**< Indicates NAF session is registered with application & positioning fix has been requested */
    K_CGPS_NAF_SUSPENDED,      /**< Indicates NAF session is registered with application & positioning service has been suspended */
    K_CGPS_NAF_AWAIT_READY,    /**< Indicates NAF session is registered with application & positioning service is required in future */
    K_CGPS_MAX_CONTEXT_STATE = 0x7FFFFFFF
} e_naf_context_state;

/**
* \enum e_cgps_RelativePosition
* this enum is used to define if position is inside or outside to defined area.
*/
/* Any change in the enums have to be added in a_CGPS_RelativePositionasEnum */
typedef enum
{
    K_CGPS_POSITION_INSIDE,
    K_CGPS_POSITION_OUTSIDE,
    K_CGPS_POSITION_UNKNOWN
}e_cgps_RelativePosition;


/**
* \enum e_cgps_Bearer_State
*
* define the bearer state
*
*/
typedef enum
{
    K_CGPS_BEARER_NOT_INITIALISED,
    K_CGPS_BEARER_INITIALISED,
    K_CGPS_BEARER_ESTABLISHING,
    K_CGPS_BEARER_ESTABLISHED,
    K_CGPS_MAX_BEARER_STATE = 0x7FFFFFFF
} e_cgps_Bearer_State;


#ifdef AGPS_FTR
/**
* \enum  e_cgps_CpSessionType
* List of constants used to indicate the CP session type.
*/
typedef enum
{
   K_CGPS_CP_SESSION_TYPE_MS_BASED,
   K_CGPS_CP_SESSION_TYPE_MS_ASSISTED,
   K_CGPS_CP_SESSION_TYPE_NOT_SET = 0x7F
} e_cgps_CpSessionType;

/**
* \enum  e_cgps_CpMoSessionType
* List of constants used to indicate the Mobile Originated CP session type.
*/
typedef enum
{
   K_CGPS_CPMO_SESSION_TYPE_MOAR,                          /*<! Assitance Request, mainly used in case of MS Based Session*/
   K_CGPS_CPMO_SESSION_TYPE_MOLR,                          /*<! Location Request, mainly used in case of MS Assisted Session*/
   K_CGPS_CPMO_SESSION_TYPE_MOLF,                          /*<! Location Forward Request, mainly used when position to be sent to 3rd party*/
   K_CGPS_CPMO_SESSION_TYPE_NOT_SET = 0x7F   
} e_cgps_CpMoSessionType;



/**
* \enum e_cgps_cp_data_state
*
* define the CGPS CP data State
*
*/
typedef enum
{
    CGPS_CP_NO_DATA = 0x00,
    CGPS_CP_POS = 0x01,
    CGPS_CP_MEAS = 0x02,
    CGPS_CP_MAX = 0x7FFFFFFF
} e_cgps_cp_data_state;




#endif /* AGPS_FTR */

#if defined  AGPS_FTR || defined AGPS_UP_FTR

/**
* \enum  e_cgps_HandleType
* List of constants used to specify type of Assisted session - CP / SUPL.
*/
typedef enum
{
  K_CGPS_SUPL_HANDLE,          /**< Handle from SUPL session */
  K_CGPS_CP_HANDLE,            /**< Handle from CP session */
  K_CGPS_MAX_HANDLETYPE = 0x7FFFFFFF
} e_cgps_HandleType;

#endif /* AGPS_FTR */

/**
* \enum e_cgps_DelayType
* this enum is used to set the Delay TypeNAF to either of the two types.
*/
typedef enum
{
    K_CGPS_LOW_DELAY ,    /*<!When set to this it implies that fulfillment of the response time requirement has precedence over fulfillment of the accuracy requirement*/
    K_CGPS_DELAY_TOLERANT /*<!When set to this it implies that fulfillment of the accuracy requirement takes precedence over fulfillment of the response time requirement*/
} e_cgps_DelayType;     /*<!The delay field has 2 options according to 3GPP TS 29.002 version 10.5.0 Release 10 */

#ifdef AGPS_TEST_MODE_FTR
/* + LMSqc32518 */
#ifdef ATT_LOGGING_ENABLE
/* - LMSqc32518 */

typedef enum
{
    K_CGPS_POSITIONING_SESSION_START,
    K_CGPS_POSITIONING_SESSION_END,
    K_CGPS_RESPONSE_RETURNED, 
/* + LMSQC19754 */
    K_CGPS_ABORT_SESSION,
/* - LMSQC19754 */
    K_CGPS_GPS_STARTED,
    K_CGPS_GPS_STOPPED,
    K_CGPS_MAX_EVENT_TYPE = 0xFF
} e_cgps_EventType;
#endif /*ATT_LOGGING_ENABLE*/

/* + LMSqc32518 */

#endif /*AGPS_TEST_MODE_FTR*/
/* - LMSqc32518 */
/**
* \enum e_cgps_ConfigBits
* this enum is used to set the NAF session type based on user configuration
*/
typedef enum
{
   K_CGPS_NAF_SESSION_CONFIG_AUTONOMOUS    = (1<<0),
   K_CGPS_NAF_SESSION_CONFIG_ASSISTED_GPS  = (1<<1),   
   K_CGPS_NAF_SESSION_CONFIG_NETWORK_BASED = (1<<2),   
} e_cgps_ConfigBits;


/**
* \def K_CGPS_NMEA_BUFFER_SIZE
*
* Size of the published NMEA buffer [bytes]
*/
#define K_CGPS_NMEA_BUFFER_SIZE  1024

/**
* \def K_CGPS_MAX_NUMBER_OF_NAF
*
* Maximum number of native applications handled by CGPS
*/
#define K_CGPS_MAX_NUMBER_OF_NAF  5


/**
* \def K_CGPS_LBS_HOST_SW_VER
*
* Maximum characters of host software version field
*/
#define K_CGPS_LBS_HOST_SW_VER   80


#ifdef AGPS_UP_FTR
/**
* \def K_CGPS_MAX_NUMBER_OF_SUPL_APPLICATIONS
*
* Maximum number of SUPL applications handled by CGPS
*/
#define K_CGPS_MAX_NUMBER_OF_SUPL  5

/**
* \def K_CGPS_SUPL_NOTIFICATION_TIMEOUT
*
* Maximum duration of a SUPL notification, if the user hasn't answered
*/
#define K_CGPS_SUPL_NOTIFICATION_TIMEOUT    10000
/* SUPLv2.0 Modifications */

/**
* \def K_CGPS_MAX_NUM_GEO_AREA
*
* Maximum number of Geographic Area
*/
#define K_CGPS_MAX_NUM_GEO_AREA   1

/**
* \def K_CGPS_MAX_AREA_ID
*
* Maximum number of Area ID(According to SUPL interface)
*/
#define K_CGPS_MAX_AREA_ID       10

/**
* \def K_CGPS_MAX_AREA_ID_LIST
*
* Maximum number of Area ID list(According to SUPL interface)
*/
#define K_CGPS_MAX_AREA_ID_LIST   10


/**
* \enum e_cgps_AreaEventType
* this enum is used to define the type of area event
*/
typedef enum
{
   K_CGPS_TRIGGER_NONE,
   K_CGPS_TRIGGER_EVENTTYPE_PERIODIC,
   K_CGPS_TRIGGER_EVENTTYPE_AREA
}e_cgps_TriggerType;


/**
* \enum e_cgps_AreaIdSet
* this enum is used to define the type of details area Id set contains
*/
typedef enum
{
   K_CGPS_AREA_IDSET_GSMAREAID,
   K_CGPS_AREA_IDSET_WCDMAAREAID,
   K_CGPS_AREA_IDSET_CDMAAREAID,
   K_CGPS_AREA_IDSET_HRPDAREAID,
   K_CGPS_AREA_IDSET_UMBAREAID,
   K_CGPS_AREA_IDSET_LTEAREAID,
   K_CGPS_AREA_IDSET_WLANAREAID,
   K_CGPS_AREA_IDSET_WIMAXAREAID
}e_cgps_AreaIdSet;


/**
* \enum e_cgps_AreaIdSetType
* this enum is used to define the type of area covered by area Id 
*/
typedef enum
{
    K_CGPS_AREA_ID_WITHIN,
    K_CGPS_AREA_ID_BORDER
}e_cgps_AreaIdSetType;




/* SUPLv2.0 Modifications */

#endif /* AGPS_UP_FTR */


#ifdef AGPS_FTR
/**
* \def K_CGPS_MAX_NUMBER_OF_CP_APPLICATIONS
*
* Maximum number of CP applications handled by CGPS
*/
#define K_CGPS_MAX_NUMBER_OF_CP  5

/**
* \def K_CGPS_EMERGENCY_SESSION_INDEX
*
* Emergency session index
*/
#define K_CGPS_EMERGENCY_SESSION_INDEX  K_CGPS_MAX_NUMBER_OF_CP

/**
* \def K_CGPS_CP_NOTIFICATION_TIMEOUT
*
* Maximum duration of a CP notification, if the user hasn't answered
*/
#define K_CGPS_CP_NOTIFICATION_TIMEOUT    20000

/**
* \def CGPS_CP_DEFAULT_MEAS_QUALITY
*
* Default quality for a measurement
*/
#define CGPS_CP_DEFAULT_MEAS_QUALITY 200

#endif /* AGPS_FTR */

#if defined AGPS_FTR || defined AGPS_UP_FTR
/**
* \def K_CGPS_MAX_NUMBER_OF_NOTIFICATIONS
*
* Maximum number of notifications handled by CGPS
*/
#define K_CGPS_MAX_NUMBER_OF_NOTIFICATIONS  5
#endif /*  AGPS_FTR || AGPS_UP_FTR */


/**
* \def CGPS_PGNV_COLD_RESET
*
* PGNV command for cold reset
*
*/
#define CGPS_PGNV_COLD_RESET "$PGNVS,RSET,COLD\r\n"


#define K_CGPS_GAD_LATITUDE_SIGN_MASK          0x800000    

#define K_CGPS_GAD_LONGITUDE_SIGN_MASK         0x800000

#define K_CGPS_GAD_LONGITUDE_SIGN_PADDING      0xff000000
/**
* \def K_CGPS_PATCH_LENGTH
*
* PGNV command for cold reset
*
*/

#if defined(GPS_OVER_SPI) || defined(GPS_OVER_XOSGPS)
/* With SPI at 400kb/s: 40000*0.05 = 2000 bytes =  74*27 bytes targeted to 65 messages */
#define K_CGPS_PATCH_SEGMENT_LENGTH   65
#else

#ifdef GPS_OVER_HCI
#define K_CGPS_PATCH_SEGMENT_LENGTH   1020
#else
#ifdef X86_64_UBUNTU_FTR
#define K_CGPS_PATCH_SEGMENT_LENGTH   500
#else
#define K_CGPS_PATCH_SEGMENT_LENGTH   20
#endif
#endif

#endif



/**************************************************************************************************************************/
/*                                                   Timers declaration                                                   */
/**************************************************************************************************************************/

/**
* \def K_CGPS_FIX_LOOP_TIMER_DURATION
* Duration of the main loop [ms].
*/
#ifndef DUAL_OS_GEN_FTR
#define K_CGPS_FIX_LOOP_TIMER_DURATION          50
#else
#define K_CGPS_FIX_LOOP_TIMER_DURATION          100
#endif /*DUAL_OS_GEN_FTR*/ 

/**
* \def K_CGPS_FIX_LOOP_TIMER_DURATION
* Duration of the main loop [ms].
*/
#define K_CGPS_FIX_LOOP_LOW_FREQ_TIMER_DURATION 500


/**
* \def K_CGPS_SLEEP_TIMER_DURATION
* Sleep Mode-Timeout used for Sleep mode to be defined by the customer. e.g. 60 means max of 60 sec
*/
#ifdef GPS_SLEEP_TIMEOUT_FTR
#define K_CGPS_SLEEP_TIMER_DURATION     GPS_SLEEP_TIMEOUT_FTR
#else
#define K_CGPS_SLEEP_TIMER_DURATION     60
#endif  /* GPS_SLEEP_TIMEOUT_FTR */

#if(K_CGPS_SLEEP_TIMER_DURATION > 60)
#error SLEEP_TIMEOUT_INCORRECT_VALUE
#endif


/**
* \def K_CGPS_GPS_COMA_TIMER_DURATION
* Timeout used for coma mode to be defined by the customer. e.g. (4*24*60*60) means 4 days (eg long weekend)
*/
#define K_CGPS_COMA_TIMER_DURATION              0


/**
* \def K_CGPS_DEEPCOMA_TIMER_DURATION
* Timeout used for Deepcoma mode to be defined by the customer. e.g. (4*24*60*60) means 4 days (eg long weekend)
*/
#define K_CGPS_DEEPCOMA_TIMER_DURATION              2147483647


/**
* \def K_CGPS_TIMER_SAFETY_MARGIN
* Uncertainty period CGPS considers about GPS chip automatically goint to Deap Coma
*/
#define K_CGPS_TIMER_SAFETY_MARGIN              30


/* \def F_CGPS_FIX_LOOP_TIMER_EXPIRY
* Fix loop timer expiry message definition
*/
#define F_CGPS_FIX_LOOP_TIMER_EXPIRY            MC_PCC_FUNCTION_TYPE(CGPS_F_FIX_LOOP_TIMER, PCC_T_TIM)

#define CGPS_FIX_LOOP_TIMER_EXPIRY              MC_RTK_PROCESS_OPERATION(PROCESS_CGPS, F_CGPS_FIX_LOOP_TIMER_EXPIRY)
#define CGPS_FIX_LOOP_TIMEOUT                   MC_RTK_PROCESS_TIMER(PROCESS_CGPS, 0, CGPS_FIX_LOOP_TIMER_EXPIRY)


/* \def F_CGPS_SLEEP_TIMER_EXPIRY
* Sleep timer expiry message definition
*/
#define F_CGPS_SLEEP_TIMER_EXPIRY               MC_PCC_FUNCTION_TYPE(CGPS_F_SLEEP_TIMER, PCC_T_TIM)

#define CGPS_SLEEP_TIMER_EXPIRY                 MC_RTK_PROCESS_OPERATION(PROCESS_CGPS, F_CGPS_SLEEP_TIMER_EXPIRY)
#define CGPS_SLEEP_TIMEOUT                      MC_RTK_PROCESS_TIMER(PROCESS_CGPS, 0, CGPS_SLEEP_TIMER_EXPIRY)
/* +SUPLv2.0 Modifications */

/* \def F_CGPS_PERIODIC_TRIGGER_TIMER_EXPIRY
* Fix loop timer expiry message definition
*/
#define F_CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY0            MC_PCC_FUNCTION_TYPE(CGPS_F_SUPLEVENT_TRIGGER_TIMER0, PCC_T_TIM)

#define CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY0              MC_RTK_PROCESS_OPERATION(PROCESS_CGPS, F_CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY0)
#define CGPS_SUPLEVENT_TRIGGER_TIMEOUT0                   MC_RTK_PROCESS_TIMER(PROCESS_CGPS, 0, CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY0)


/* \def F_CGPS_PERIODIC_TRIGGER_TIMER_EXPIRY
* Fix loop timer expiry message definition
*/
#define F_CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY1            MC_PCC_FUNCTION_TYPE(CGPS_F_SUPLEVENT_TRIGGER_TIMER1, PCC_T_TIM)
    
#define CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY1              MC_RTK_PROCESS_OPERATION(PROCESS_CGPS, F_CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY1)
#define CGPS_SUPLEVENT_TRIGGER_TIMEOUT1                   MC_RTK_PROCESS_TIMER(PROCESS_CGPS, 0, CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY1)



/* \def F_CGPS_PERIODIC_TRIGGER_TIMER_EXPIRY
* Fix loop timer expiry message definition
*/
#define F_CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY2            MC_PCC_FUNCTION_TYPE(CGPS_F_SUPLEVENT_TRIGGER_TIMER2, PCC_T_TIM)
    
#define CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY2              MC_RTK_PROCESS_OPERATION(PROCESS_CGPS, F_CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY2)
#define CGPS_SUPLEVENT_TRIGGER_TIMEOUT2                   MC_RTK_PROCESS_TIMER(PROCESS_CGPS, 0, CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY2)


/* \def F_CGPS_PERIODIC_TRIGGER_TIMER_EXPIRY
* Fix loop timer expiry message definition
*/
#define F_CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY3            MC_PCC_FUNCTION_TYPE(CGPS_F_SUPLEVENT_TRIGGER_TIMER3, PCC_T_TIM)
    
#define CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY3              MC_RTK_PROCESS_OPERATION(PROCESS_CGPS, F_CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY3)
#define CGPS_SUPLEVENT_TRIGGER_TIMEOUT3                   MC_RTK_PROCESS_TIMER(PROCESS_CGPS, 0, CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY3)


/* \def F_CGPS_PERIODIC_TRIGGER_TIMER_EXPIRY
* Fix loop timer expiry message definition
*/
#define F_CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY4            MC_PCC_FUNCTION_TYPE(CGPS_F_SUPLEVENT_TRIGGER_TIMER4, PCC_T_TIM)
    
#define CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY4              MC_RTK_PROCESS_OPERATION(PROCESS_CGPS, F_CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY4)
#define CGPS_SUPLEVENT_TRIGGER_TIMEOUT4                   MC_RTK_PROCESS_TIMER(PROCESS_CGPS, 0, CGPS_SUPLEVENT_TRIGGER_TIMER_EXPIRY4)


/* -SUPLv2.0 modifications */


#endif /* __CGPSMAIN_HIC__ */
