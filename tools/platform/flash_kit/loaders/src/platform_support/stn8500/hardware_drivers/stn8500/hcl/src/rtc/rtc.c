/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides some support routines for the RTC
* \author  ST-Ericsson
*/
/*****************************************************************************/
/*
 * RTC low level driver provides various methods for setting and getting the 
 * date, and a certain number of alarms.
 * Some assumptions are made:
 *
 * 
 * - WARNING: no public function must be called before the RTC_Init(),
 *   obviously, but this is not handled at this level.
 */
/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "debug.h"
#include "rtc.h"
#include "rtc_p.h"
/* #include "rtc_irqp.h"
#include "rtc_irq.h" */

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
/*For debug HCL */
#ifdef __DEBUG
#define MY_DEBUG_LEVEL_VAR_NAME myDebugLevel_RTC
#define MY_DEBUG_ID             myDebugID_RTC

PRIVATE t_dbg_level             MY_DEBUG_LEVEL_VAR_NAME = (t_dbg_level) 0xFFFFFFFF;
PRIVATE t_dbg_id                MY_DEBUG_ID = RTC_HCL_DBG_ID;
#endif
PRIVATE t_rtc_system_context    g_rtc_system_context;
t_rtc_date                      zero_date = { 0, 0, 0, 0, 0, 0, 0 };

/* Inline functions */
PRIVATE t_rtc_counter rtc_TestNonRecAlarms(volatile t_bool *non_recusrsive_alarm, volatile t_uint8 *return_id)
{
    t_rtc_counter   current = 0;
    t_rtc_counter   ret_exp_time = 0, tmp_time = 0, match_value = 0;
    t_uint32        i;
    t_rtc_date      new_date_value;

    for (i = 0; i < RTC_MAX_NON_REC_ALARMS; i++)
    {
        if (g_rtc_system_context.rtc_non_rec_alarm_list[i].inUse == TRUE)
        {
            tmp_time = g_rtc_system_context.rtc_non_rec_alarm_list[i].expTime;
            if (tmp_time <= current)
            {
                /*
                 * Id of alarms in the past are
                 * candidate for being returned.
                 * They are, however, always canceled.*/
                g_rtc_system_context.rtc_non_rec_alarm_list[i].inUse = FALSE;
                g_rtc_system_context.rtc_non_rec_alarm_list[i].expTime = 0x0;
                g_rtc_system_context.rtc_non_rec_alarm_list[i].expDate = zero_date;
                g_rtc_system_context.rtc_nnon_rec_alarms_inserted--;
                *non_recusrsive_alarm = TRUE;
                if (tmp_time >= ret_exp_time)
                {
                    *return_id = (t_uint8) i;
                    ret_exp_time = tmp_time;
                }
            }

            /*
             * expTime of alarms in the future
             * are candidate for RTC MATCH reprogramming*/
            else if (tmp_time <= match_value)
            {
                match_value = tmp_time;
                new_date_value = g_rtc_system_context.rtc_non_rec_alarm_list[i].expDate;
				/*coverity[self_assign]*/
                new_date_value = new_date_value;    /*to remove pc lint warning*/
            }
        }
    }

    return(match_value);
}

PRIVATE t_rtc_counter rtc_TestRecAlarms(volatile t_bool *rec_alarm_present, volatile t_rtc_alarm_id *p_rec_alarm)
{
    t_rtc_counter   now = 0;
    t_rtc_counter   tmp_time = 0, match_value = 0;
    t_uint32        i;
    t_uint32        temp_match;
    t_rtc_date		date;
    t_rtc_error     status = RTC_OK;

    for (i = 0; i < RTC_MAX_REC_ALARMS; i++)
    {
        if (g_rtc_system_context.rtc_rec_alarm_list[i].inUse == TRUE)
        {
            tmp_time = g_rtc_system_context.rtc_rec_alarm_list[i].expTime;
            status = RTC_GetCountDateAndTime(&date);
            status = date_to_sec(date, &now);
            
            if (tmp_time <= now)
            {
                /*
                 * Id of alarms in the past are
                 * candidate for being returned.
                 * They are, however, always canceled.
                 */
                temp_match = (now - g_rtc_system_context.rtc_rec_alarm_list[i].expTime) / g_rtc_system_context.rtc_rec_alarm_list[i].recTime;
                g_rtc_system_context.rtc_rec_alarm_list[i].expTime += (temp_match + 1) * g_rtc_system_context.rtc_rec_alarm_list[i].recTime;
                tmp_time = g_rtc_system_context.rtc_rec_alarm_list[i].expTime;

                *rec_alarm_present = TRUE;

                p_rec_alarm->id |= (1UL << i);
            }

            /*
             * expTime of alarms in the future
             * are candidate for RTC MATCH reprogramming
             */
             
           	status = RTC_GetMatchDateAndTime(&date);
        	status = date_to_sec(date, &match_value);
        
            if (tmp_time <= match_value)
            {
                match_value = tmp_time;

                /*  sec_to_date(&new_date_value,new_match_value); temporary */
            }
        }
    }
	/*coverity[self_assign]*/
	status = status;
    return(match_value);
}

/*****************************************************************************/
/* NAME:	t_rtc_error RTC_SetDbgLevel(IN t_dbg_level debug_level)        	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables to choose between different debug       */
/*              comments levels                                              */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :  t_dbg_level DebugLevel:          identify RTC debug level           */
/*                                                                           */
/*                                                                           */
/* RETURN:         t_rtc_error :          RTC error code                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC t_rtc_error RTC_SetDbgLevel(IN t_dbg_level debug_level)
{
   
#ifdef __DEBUG
    myDebugLevel_RTC = debug_level;
#endif
    return(RTC_OK);
    
}

/*****************************************************************************/
/* NAME:	t_rtc_error RTC_GetDbgLevel(OUT t_dbg_level *debug_level)      	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables to choose between different debug       */
/*              comments levels                                              */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :                                                                      */
/*                                                                           */
/* OUT: t_dbg_level DebugLevel:           identify RTC debug level           */
/*                                                                           */
/* RETURN:        t_rtc_error :           RTC error code                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC t_rtc_error RTC_GetDbgLevel(OUT t_dbg_level *debug_level)
{
  
#ifdef __DEBUG
    if (NULL == debug_level)
    {
        return(RTC_INVALID_PARAMETER);
    }

    *debug_level = myDebugLevel_RTC;
#endif
    return(RTC_OK);
}

/*****************************************************************************/
/* NAME:  t_rtc_error RTC_GetVersion(OUT t_version *p_version)     		  	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the RTC HCL version		     		 */
/*               		                                             		 */
/* PARAMETERS:                                                               */
/* IN :      	                  	                  	             		 */
/*                                                                           */
/* OUT :    t_version *p_version   The RTC HCL version                       */
/*                                                                           */
/* RETURN: t_rtc_error                  : RTC error code                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC t_rtc_error RTC_GetVersion(OUT t_version *p_version)
{
    
    DBGENTER1("p_version @=%p\n", (void *) p_version);

    if (NULL == p_version)
    {
        DBGEXIT(RTC_INVALID_PARAMETER);
        return(RTC_INVALID_PARAMETER);
    }

    p_version->major = RTC_HCL_MAJOR_ID;
    p_version->minor = RTC_HCL_MINOR_ID;
    p_version->version = RTC_HCL_VERSION_ID;

    DBGEXIT3(status, "major=%08d, minor=%08d, version=%08d\n", p_version->major, p_version->minor, p_version->version);
    DBGEXIT0(RTC_OK);

    return(RTC_OK);
}

/*****************************************************************************/
/* NAME:  t_rtc_error RTC_Init(IN t_logical_address rtc_base_address)        */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initializes the RTC registers and checks        */
/*              PCell Id. It also initializes internal data structures       */
/*              of the driver.                                               */
/* PARAMETERS:                                                               */
/* IN : t_logical_address rtc_base_address: RTC registers base address       */
/*                                                                           */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: t_rtc_error                  : RTC error code                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */
/*****************************************************************************/

/* WARNING: modify the ID mechanism, basing it only on the part number
 * and in case of mismatch, do not return an error
 */
PUBLIC t_rtc_error RTC_Init(IN t_logical_address rtc_base_address)
{
    t_rtc_error status = RTC_OK;

    DBGENTER0();

    if (rtc_base_address == 0x0)
    {
        status = RTC_UNSUPPORTED_HW;
        DBGEXIT0(status);
        return(status);
    }

    g_rtc_system_context.p_rtc_reg = (t_rtc_register *) rtc_base_address;

    /* Check Peripheral and Pcell Id Register for RTC */
    if
    (
        (g_rtc_system_context.p_rtc_reg->RTCPeriphID0 != RTCPERIPHID0)
    ||  (g_rtc_system_context.p_rtc_reg->RTCPeriphID1 != RTCPERIPHID1)
    ||  (g_rtc_system_context.p_rtc_reg->RTCPeriphID2 != RTCPERIPHID2)
    ||  (g_rtc_system_context.p_rtc_reg->RTCPeriphID3 != RTCPERIPHID3)
    )
    {
        g_rtc_system_context.p_rtc_reg = NULL;
        status = RTC_ERROR_PERIPHERAL_IDENTIFICATION_FAILED;

        /*DBGPRINT(DBGL_ERROR, " RTC ID Error");*/
        return(status);
    }

    rtcAlarm_Init();

    /* Reset the Load Register */
    /*    HCL_WRITE_REG(g_rtc_system_context.p_rtc_reg->RTTLR, 0x0);*/
    /*Commented for PEPS*/
    /* Clear Interrupts */
    /*    CLEAR_RTT_INTS;*/
    /*Commented for PEPS*/
    /* Mask Interrupts */
    /*   MASK_RTT_INTS;*/
    /*Commented for PEPS*/
    /* Disable RTT */
    /*   DISABLE_RTT;*/
    /*Commented for PEPS*/
    DBGEXIT0(status);
    return(status);
}

/*****************************************************************************/
/* NAME:  t_rtc_error RTC_SetTrim(In t_uint32 rtc_frequency_integer			 */
/*							      IN t_uint32 rtc_frequency_fraction		 */
/*							      IN t_uint16 rtc_decimal_point);            */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine through the RTC_TCR register, lets you          */
/*              adjust (or trim) the CLK1HZ time base to an error of less 	 */
/*				than 1ppm													 */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN : t_uint32  rtc_frequency_integer  : the integer part of output		 */
/*                    					  frequency at the CLK32 pin         */
/*      t_uint32  rtc_frequency_fraction : the fraction part of output	     */
/*                    					  frequency at the CLK32 pin         */
/*                                   					                     */
/*      t_uint16 rtc_decimal_point : the decimal point position in the       */
/*                                    above argument from the LSB            */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: t_rtc_error       : RTC error code                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant	                                             */

/*****************************************************************************/
PUBLIC t_rtc_error RTC_SetTrim
(
    IN t_uint32 rtc_frequency_integer,
    IN t_uint32 rtc_frequency_fraction,
    IN t_uint8  rtc_decimal_point
)
{
    t_uint32    factor_decimal, rtc_trim;
    t_uint8     index;

    DBGENTER3
    (
        "rtc_frequency_integer = %lu, rtc_frequency_fraction = %lu, rtc_decimal_point = %u",
        rtc_frequency_integer,
        rtc_frequency_fraction,
        rtc_decimal_point
    );

    /* Check for null frequency passed */
    if (0 == rtc_frequency_integer)
    {
        DBGEXIT0(RTC_INVALID_PARAMETER);
        return(RTC_INVALID_PARAMETER);
    }

    /* Calculate the decimal factor */
    index = rtc_decimal_point;
    factor_decimal = 1;

    while (index > 0)
    {
        factor_decimal *= 10;
        index--;
    }

    /* Multiply fractional part by trimming interval */
    rtc_frequency_fraction = (rtc_frequency_fraction * 1023) / factor_decimal;

    rtc_frequency_integer--;

    WRITE_TCR_TRIM(rtc_frequency_integer, rtc_frequency_fraction, rtc_trim);

    DBGEXIT0(RTC_OK);
    return(RTC_OK);
}

/*****************************************************************************/
/* NAME:  t_rtc_error RTC_GetTrim(OUT t_uint32 *p_rtc_frequency_integer,	 */
/*							      OUT t_uint32 *p_rtc_frequency_fraction);	 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine gets the RTC_TCR register trim fields			 */
/*                                                                           */
/* PARAMETERS:                                                               */
/* OUT : t_uint32  p_rtc_frequency_integer : CLK_DIV	field of			 */
/*											 TCR register                    */
/*       t_uint32  p_rtc_frequency_fraction : CLK_DEL	field of			 */
/*											 TCR register                    */
/*                                   					                     */
/* IN :                                                                      */
/*                                                                           */
/* RETURN: t_rtc_error       : RTC error code                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant	                                             */

/*****************************************************************************/
PUBLIC t_rtc_error RTC_GetTrim(OUT t_uint32 *p_rtc_frequency_integer, OUT t_uint32 *p_rtc_frequency_fraction)
{
    DBGENTER2
    (
        "p_rtc_frequency_integer @=%p, p_rtc_frequency_fraction @=%p\n",
        (void *) p_rtc_frequency_integer,
        (void *) p_rtc_frequency_fraction
    );

    /* Check for null frequency passed */
    if (NULL == p_rtc_frequency_integer || NULL == p_rtc_frequency_fraction)
    {
        DBGEXIT0(RTC_INVALID_PARAMETER);
        return(RTC_INVALID_PARAMETER);
    }

    /* Set RTC_TCR CLK_DIV */
    READ_TCR_CLKDIV(*p_rtc_frequency_integer);

    /* Set RTC_TCR CLK_DEL */
    READ_TCR_CLKDEL(*p_rtc_frequency_fraction);

    DBGEXIT0(RTC_OK);
    return(RTC_OK);
}

/*****************************************************************************/
/* NAME:  t_rtc_error RTC_SetSecureMode(IN t_bool rtc_secure)                */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This Routine sets(resets) secure mode which enables			 */
/*              (disables) write protection for selected registers			 */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_bool rtc_secure :Enable(Disable)  the secure mode,              */
/*  						  write protection if its True(False)            */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: t_rtc_error       : RTC error code                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant	                                             */

/*****************************************************************************/
PUBLIC t_rtc_error RTC_SetSecureMode(IN t_bool rtc_secure)
{
    DBGENTER1("%u", rtc_secure);

    /* Enable/Disable write secure mode */
    if (TRUE == rtc_secure)
    {
        CLEAR_TCR_SEC;
    }
    else
    {
        SET_TCR_SEC;
    }

    DBGEXIT0(RTC_OK);
    return(RTC_OK);
}

/*****************************************************************************/
/* NAME:      t_rtc_error RTC_SetPermanentDateAndTime(In t_rtc_date  rtc_date)*/
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to set the permanent clock               */
/*              to the given date                                            */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN : t_rtc_date   rtc_date : the date and time to be set in the permanent */
/*                             clock                                         */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: t_rtc_error       : RTC error code                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */

/*****************************************************************************/
PUBLIC t_rtc_error RTC_SetPermanentDateAndTime(IN t_rtc_date rtc_date)
{
    t_rtc_error         status = RTC_OK;
    t_rtc_counter       seconds;
    volatile t_uint32   dummy = 0;

    DBGENTER6
    (
        " %03d-%02d-%02d %02d:%02d:%02d\n",
        rtc_date.year,
        rtc_date.month,
        rtc_date.day,
        rtc_date.hour,
        rtc_date.minute,
        rtc_date.second
    );

    /* Transform rtc_date in number of seconds form */
    status = date_to_sec(rtc_date, &seconds);

    if (status != RTC_OK)
    {
        DBGEXIT0(status);
        return(status);
    }

    /* HCL_WRITE_BITS((g_rtc_system_context.p_rtc_reg)->RTCDR,0x0,0xFFFFFFFF); */
    /* HCL_WRITE_BITS((g_rtc_system_context.p_rtc_reg)->RTCYR,0x0,0xFFFFFFFF); */
    HCL_WRITE_BITS(dummy, (rtc_date.month) << 25, MASK_CWMONTH);
    HCL_WRITE_BITS(dummy, (rtc_date.day) << 20, MASK_CWDAYM);
    HCL_WRITE_BITS(dummy, (rtc_date.day_of_week) << 17, MASK_CWDAYW);
    HCL_WRITE_BITS(dummy, (rtc_date.hour) << 12, MASK_CWHOUR);
    HCL_WRITE_BITS(dummy, (rtc_date.minute) << 6, MASK_WMIN);
    HCL_WRITE_BITS(dummy, (rtc_date.second), MASK_CWSEC);

    /* HCL_WRITE_BITS((g_rtc_system_context.p_rtc_reg)->RTCYLR, (rtc_date.year+BASE_YEAR) , MASK_CWYEAR); */
    (g_rtc_system_context.p_rtc_reg)->RTCLR = dummy;
    HCL_WRITE_BITS
    (
        (g_rtc_system_context.p_rtc_reg)->RTCYLR,
        itobcd(rtc_date.year) + itobcd(BASE_YEAR) /*itobcd(rtc_date.year+BASE_YEAR)*/,
        MASK_CWYEAR
    );

    /*DBGPRINTDEC(DBGL_INTERNAL, "Number of seconds =", seconds);*/
    /* Call change date function */
    status = rtcAlarm_ChangeDate(seconds, rtc_date);

    DBGEXIT0(status);
    return(status);
}

/*****************************************************************************/
/* NAME:      t_rtc_error RTC_GetPermanentDateAndTime(OUT t_rtc_date *rtc_date)*/
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the current date                        */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :                                                                      */
/*                                                                           */
/* OUT :   t_rtc_date    *rtc_date      : the current date and time          */
/*                                                                           */
/* RETURN: t_rtc_error                 : RTC error code                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Re-Entrant                                                    */

/*****************************************************************************/
PUBLIC t_rtc_error RTC_GetPermanentDateAndTime(OUT t_rtc_date *rtc_date)
{
    t_uint32    cal = 0;
    DBGENTER1("rtc_date @=%p", (void *) rtc_date);

    /* Check for null pointer passed */
    if (NULL == rtc_date)
    {
        DBGEXIT0(RTC_INVALID_PARAMETER);
        return(RTC_INVALID_PARAMETER);
    }

    rtc_date->year = (t_uint16)bcdtoi(((g_rtc_system_context.p_rtc_reg)->RTCYR)) - BASE_YEAR;

    cal = (g_rtc_system_context.p_rtc_reg)->RTCDR;

    rtc_date->month = (t_uint8) ((cal & MASK_CWMONTH) >> 25);
    rtc_date->day = (t_uint16) ((cal & MASK_CWDAYM) >> 20);
    rtc_date->day_of_week = (t_uint8) ((cal & MASK_CWDAYW) >> 17);
    rtc_date->hour = (t_uint8) ((cal & MASK_CWHOUR) >> 12);
    rtc_date->minute = (t_uint8) ((cal & MASK_WMIN) >> 6);
    rtc_date->second = (t_uint8) (cal & MASK_CWSEC);

    return(RTC_OK);
}

/*****************************************************************************/
/* NAME:      t_rtc_error RTC_SetAlarm(										 */
/*									   IN t_rtc_date rtc_date, 			 	 */
/*									   IN t_rtc_date Rtc_recurse_period,	 */
/*									   OUT t_rtc_alarm_id *rtc_alarm_Id )    */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to set an alarm                          */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtc_date      rtc_date      : the date of the alarm to set      */
/*         t_rtc_date Rtc_recurse_period	: the recurse period of an alarm */
/*                                                                  		 */
/* OUT :   t_rtc_alarm_id* rtc_alarm_Id   : the ID for the alarm set         */
/*                                                                           */
/* RETURN: t_rtc_error                  : RTC error code                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */

/*****************************************************************************/
PUBLIC t_rtc_error RTC_SetAlarm
(
    IN t_rtc_date       rtc_date,
    IN t_rtc_date       Rtc_recurse_period,
    OUT t_rtc_alarm_id  *rtc_alarm_Id
)
{
    t_rtc_error     status = RTC_OK;
    t_rtc_counter   seconds = 0;
    t_rtc_counter   rec_seconds = 0;

    DBGENTER6
    (
        " %03d-%02d-%02d %02d:%02d:%02d\n",
        rtc_date.year,
        rtc_date.month,
        rtc_date.day,
        rtc_date.hour,
        rtc_date.minute,
        rtc_date.second
    );
    DBGENTER6
    (
        "Recurse Period is %03d-%02d-%02d %02d:%02d:%02d\n",
        Rtc_recurse_period.year,
        Rtc_recurse_period.month,
        Rtc_recurse_period.day,
        Rtc_recurse_period.hour,
        Rtc_recurse_period.minute,
        Rtc_recurse_period.second
    );

    DBGENTER1("rtc_alarm_Id @=%p", (void *) rtc_alarm_Id);

    /* Check for null id passed */
    if (NULL == rtc_alarm_Id)
    {
        DBGEXIT0(RTC_INVALID_PARAMETER);
        return(RTC_INVALID_PARAMETER);
    }

    /* Transform rtc_date in number of seconds form */
    status = date_to_sec(rtc_date, &seconds);
    if (status != RTC_OK)
    {
        rtc_alarm_Id->id = 0xffffffff;
        DBGEXIT0(status);
        return(status);
    }

    if ((0 == Rtc_recurse_period.year) && (0 == Rtc_recurse_period.month))
    {
        status = day_to_sec(Rtc_recurse_period, &rec_seconds);
        if (status != RTC_OK)
        {
            rtc_alarm_Id->id = 0xffffffff;
            DBGEXIT0(status);
            return(status);
        }
    }
    else
    {
        /*DBGPRINT(DBGL_ERROR, "Invalid recursion Period");*/
        status = RTC_ERROR_INVALID_ALARM;
        return(status);
    }

    /* Call low level set alarm function */
    status = rtcAlarm_Insert(seconds, rec_seconds, rtc_date, rtc_alarm_Id);

    /*DBGPRINTDEC(DBGL_INTERNAL, "Number of seconds for alarm= %d", seconds);
    DBGPRINTDEC(DBGL_INTERNAL, "Number of seconds for alarm= %d", seconds); */
    DBGEXIT2
    (
        status,
        "RECURSIVE -> %s, Alarm Id = %ld",
        ((rtc_alarm_Id->type == RTC_RECURSIVE_ALARM) ? "YES" : "NO"),
        rtc_alarm_Id->id
    );
    return(status);
}

/*****************************************************************************/
/* NAME:      t_rtc_error RTC_ClearAlarm(IN t_rtc_alarm_id  rtc_alarm_Id)    */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to clear an alarm, before it occurs.     */
/*              The corresponding ID is released                             */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtc_alarm_id rtc_alarm_Id   : the ID of the alarm to clear      */
/*                                                                           */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: t_rtc_error                 : RTC error code                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */

/*****************************************************************************/
PUBLIC t_rtc_error RTC_ClearAlarm(IN t_rtc_alarm_id rtc_alarm_Id)
{
    t_rtc_error status = RTC_OK;

    DBGENTER1("%ld", rtc_alarm_Id.id);

    /* Call low level clear alarm function */
    status = rtcAlarm_Delete(rtc_alarm_Id);

    DBGEXIT(status);
    return(status);
}

/*****************************************************************************/
/* NAME:      t_bool RTC_IsEventActive(IN t_rtc_event *p_event) 	   	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine checks the status of an event			         */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtc_event *p_event: Pointer to the event for checking the 		 */
/*								 status								         */
/*                                                                           */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: t_bool: Status of the interrupt			                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC t_bool RTC_IsEventActive(IN t_rtc_event *p_event)
{
    if (*p_event == g_rtc_system_context.rtc_active_events)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

/*****************************************************************************/
/* NAME:      void RTC_AcknowledgeEvent(IN t_rtc_event *p_event)	   	     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to acknowledge a particular event       */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    t_rtc_event *p_event: Pointer to the event to be acknowledged     */
/*                                                                           */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: None										                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC void RTC_AcknowledgeEvent(IN t_rtc_event *p_event)
{
    if (*p_event == g_rtc_system_context.rtc_active_events)
    {
        g_rtc_system_context.rtc_active_events = RTC_NO_EVENT;
    }
}

/*****************************************************************************/
/* NAME:  t_rtc_error RTC_Reset(void)									     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine resets the RTC registers to its power-on-reset  */
/*              value.			        									 */
/*										                                     */
/* PARAMETERS:                                                               */
/* IN : None															     */
/*                                                                           */
/* OUT : None                                                                */
/*                                                                           */
/* RETURN: t_rtc_error                  : RTC error code                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */

/*****************************************************************************/
PUBLIC t_rtc_error RTC_Reset(void)
{
    volatile t_uint32   time_out;

    DBGENTER0();

    DISABLE_RTT;

    g_rtc_system_context.p_rtc_reg->RTCMR = 0x0UL;
    g_rtc_system_context.p_rtc_reg->RTCYMR = 0x2000;
    g_rtc_system_context.p_rtc_reg->RTCLR = 0x0UL;
    g_rtc_system_context.p_rtc_reg->RTCYLR = 0x0UL;
    g_rtc_system_context.p_rtc_reg->RTCCR = 0x7FFFUL;

    /* Clear the RTC interrupt source */
    CLEAR_RTC_INTS;

    /* Mask the RTC interrupt */
    MASK_RTC_INTS;

    time_out = RTT_CLK32;
    while (time_out--)
        ;

    g_rtc_system_context.p_rtc_reg->RTTLR1 = 0x0UL;
    g_rtc_system_context.p_rtc_reg->RTTLR2 = 0x0UL;

    /* Delay added because 1 RTC clock cycle needed between two consecutive 
	 * access to control register assuming maximum clock frequency 264 MHz of
	 * the core and RTC clock at 32.768 KHz 
	 */
    time_out = RTT_CLK32;
    while (time_out--)
        ;

    g_rtc_system_context.p_rtc_reg->RTTCR = 0x0UL;

    /* Clear the timer interrupt source */
    CLEAR_RTT_INTS;

    /* Mask the timer interrupt */
    MASK_RTT_INTS;

    g_rtc_system_context.p_rtc_reg = NULL;

    DBGEXIT0(RTC_OK);

    return(RTC_OK);
}

/*------------------------------------------------------------------------
 * Alarm management functions
 *----------------------------------------------------------------------*/
/*
 * The functions below deal with alarm management in memory and with
 * HW programming.
 * The alarms are stored in an array.
 * NOTE: all these functions must be executed atomically. 
 */
/*****************************************************************************/
/* NAME:        void rtcAlarm_Init(void)                                     */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine enables RTC HW component and related            */
/*              data structures. Called by RTC_Init()                        */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :                                                                      */
/*                                                                           */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN:                                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PRIVATE void rtcAlarm_Init(void)
{
    t_uint8 i;

    /*
     * We bring RTC in a well known state, putting 137 years between us and
     * the next interrupt, because we don't want interrupts pending when we
     * will unmask them.
     */
    MASK_RTC_INTS;

    CLEAR_RTC_INTS;

	RTC_ENABLE;
    /* Mark all alarms as not used */
    for (i = 0; i < RTC_MAX_REC_ALARMS; i++)
    {
        g_rtc_system_context.rtc_rec_alarm_list[i].inUse = FALSE;
    }

    for (i = 0; i < RTC_MAX_NON_REC_ALARMS; i++)
    {
        g_rtc_system_context.rtc_non_rec_alarm_list[i].inUse = FALSE;
    }

    g_rtc_system_context.rtc_nrec_alarms_inserted = 0;
    g_rtc_system_context.rtc_nnon_rec_alarms_inserted = 0;

    return;
}

/*****************************************************************************/
/* NAME:       t_rtc_error rtcAlarm_Insert(									 */
/*										   In t_rtc_counter   alarm_time,    */
/*                                         In t_rtc_counter   recTime,       */
/*                                         OUT t_rtc_alarm_id *rtc_alarm_Id  */
/*                                        )                                  */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine inserts an alarm on the list and reprograms     */
/*              RTC match register if necessary. Called by RTC_SetAlarm()    */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :   t_rtc_counter  alarm_time:  expire time (in seconds) of the alarm  */
/*                                    to insert.                             */
/*        t_rtc_counter   recTime  :  recursive time (in seconds) of the 	 */
/* 									  alarm to insert.                       */
/* OUT:   t_rtc_alarm_id *rtc_alarm_Id: pointer to the t_rtc_alarm_id        */
/*                                    structure with the Id of the alarm set */
/*                                                                           */
/* RETURN: t_rtc_error  : RTC error code                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PRIVATE t_rtc_error rtcAlarm_Insert
(
    IN t_rtc_counter    alarm_time,
    IN t_rtc_counter    recTime,
    IN t_rtc_date       alm_date,
    OUT t_rtc_alarm_id  *rtc_alarm_Id
)
{
	/*coverity[var_decl]*/
    t_rtc_counter   now, oldMatchValue; /* new_match_value; */
    t_uint32        i, flags;
    t_uint32        temp_id = RTC_MAX_NON_REC_ALARMS + 1;
    t_rtc_id        retId;
    t_uint8         alarmsBefore = 0;
    t_rtc_date      date;
    t_rtc_error     status;

    /* Check for null alarm id passed */
    if (NULL == rtc_alarm_Id)
    {
        DBGEXIT0(RTC_INVALID_PARAMETER);
        return(RTC_INVALID_PARAMETER);
    }

    /* Is alarm in the future? */
    status = RTC_GetCountDateAndTime(&date);
    status = date_to_sec(date, &now);

	/*coverity[uninit_use]*/
    if (alarm_time <= now + 1)
    {
        rtc_alarm_Id->id = 0xffffffff;
        return(RTC_ERROR_INVALID_ALARM);
    }

    if (recTime != 0)
    {
        /* Find a free element of the array (that will be our Id) */
        for (i = 0; i < RTC_MAX_REC_ALARMS; i++)
        {
            if (g_rtc_system_context.rtc_rec_alarm_list[i].inUse == FALSE)
            {
                break;
            }
        }

        if (i == RTC_MAX_REC_ALARMS)
        {
            return(RTC_ERROR_NO_MORE_ALARM);
        }

        /* Now we can insert the alarm into the array */
        retId = (t_rtc_id) i;
        g_rtc_system_context.rtc_rec_alarm_list[retId].expTime = alarm_time;
        g_rtc_system_context.rtc_rec_alarm_list[retId].recTime = recTime;

        /* g_rtc_system_context.rtc_rec_alarm_list[retId].expDate = alm_date; */
        SAVE_AND_DISABLE_INTERRUPTS(flags);

        g_rtc_system_context.rtc_rec_alarm_list[retId].inUse = TRUE;
        g_rtc_system_context.rtc_nrec_alarms_inserted++;

        /* Save the number of alarms for later */
        alarmsBefore = (t_uint8) (g_rtc_system_context.rtc_nrec_alarms_inserted + g_rtc_system_context.rtc_nnon_rec_alarms_inserted);

        status = RTC_GetMatchDateAndTime(&date);
        status = date_to_sec(date, &oldMatchValue);

        RESTORE_INTERRUPTS(flags);
        rtc_alarm_Id->type = RTC_RECURSIVE_ALARM;
        rtc_alarm_Id->id = 1UL << retId;

        /**/
        /**/
    }
    else
    {
        /* Find a free element of the array (that will be our Id) */
        for (i = 0; i < RTC_MAX_NON_REC_ALARMS; i++)
        {
            if
            (
                (g_rtc_system_context.rtc_non_rec_alarm_list[i].expTime == alarm_time)
            &&  (g_rtc_system_context.rtc_non_rec_alarm_list[i].inUse == TRUE)
            )
            {
                rtc_alarm_Id->type = RTC_NON_RECURSIVE_ALARM;
                rtc_alarm_Id->id = (t_rtc_id) i;
                return(RTC_ALARM_IN_USE);
            }

            if
            (
                (g_rtc_system_context.rtc_non_rec_alarm_list[i].inUse == FALSE)
            &&  ((RTC_MAX_NON_REC_ALARMS + 1) == temp_id)
            )
            {
                temp_id = i;
            }
        }

        if (temp_id >= RTC_MAX_NON_REC_ALARMS)
        {
            return(RTC_ERROR_NO_MORE_ALARM);
        }

        /* Now we can insert the alarm into the array */
        retId = (t_rtc_id) temp_id;
        g_rtc_system_context.rtc_non_rec_alarm_list[retId].expTime = alarm_time;
        g_rtc_system_context.rtc_non_rec_alarm_list[retId].expDate = alm_date;

        /* SAVE_AND_DISABLE_INTERRUPTS(flags); */
        g_rtc_system_context.rtc_non_rec_alarm_list[retId].inUse = TRUE;
        g_rtc_system_context.rtc_nnon_rec_alarms_inserted++;

        /* Save the number of alarms for later */
        alarmsBefore = (t_uint8) (g_rtc_system_context.rtc_nrec_alarms_inserted + g_rtc_system_context.rtc_nnon_rec_alarms_inserted);

        status = RTC_GetMatchDateAndTime(&date);
        status = date_to_sec(date, &oldMatchValue);

        /* RESTORE_INTERRUPTS(flags); */
        rtc_alarm_Id->type = RTC_NON_RECURSIVE_ALARM;
        rtc_alarm_Id->id = retId;
    }

    /* Early exit conditions */
    if (alarmsBefore == 1)              /* This is the only alarm present */
    {
        /* safeMatchUpdate(alarm_time); */
        RTC_LoadMatchDateAndTime(alm_date);
        UNMASK_RTC_INTS;
        return(RTC_OK);
    }

    if (alarm_time >= oldMatchValue)
    {
        return(RTC_OK);
    }

    /*   new_match_value = alarm_time; */
    /*
     * Some alarms already considered by us could have been deleted
     * in the meanwhile by the ISR. This is detected in the function,
     * where we check if alarmsBefore == rtc_nAlarmsInserted.
     * In this case, the RTC MATCH register already has the correct
     * value (set by the ISR), so we must not reprogram it.
     */
    /* safeMatchUpdateCond(new_match_value, alarmsBefore); */
    RTC_LoadMatchDateAndTime(alm_date);
	/*coverity[self_assign]*/
    status = status;                    /* to remove PC lint warning*/

    /* Everything OK, return */
    return(RTC_OK);
}

/*****************************************************************************/
/* NAME:       t_rtc_error rtcAlarm_Delete(IN t_rtc_alarm_id  rtc_alarm_Id)  */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine deletes an alarm from the list and reprograms   */
/*              RTC match register if necessary. Called by RTC_ClearAlarm()  */
/*              					                                         */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN:   t_rtc_alarm_id rtc_alarm_Id: Id of the alarm to clear               */
/*                                                                           */
/* OUT:                                                                      */
/*                                                                           */
/* RETURN: t_rtc_error  : RTC error code                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */
/*****************************************************************************/

/*
 * NOTE: if an interrupt occurs before the lines:
 *
 * rtc_alarmList[delId].inUse = FALSE;
 * rtc_nAlarmsInserted--;
 *
 * RTC MATCH register is programmed in the right way by this function.
 * If it occurs after, the right programming will be done by the ISR,
 * and this situation will be detected in the critical section, avoiding
 * a write in the register which would be wrong, reflecting the
 * state of things preceding the ISR.
 */
PRIVATE t_rtc_error rtcAlarm_Delete(IN t_rtc_alarm_id rtc_alarm_Id)
{
    t_rtc_id        delId = 0;
    t_uint8         temp_id = 0;
	/*coverity[var_decl]*/
    t_rtc_counter   new_match_value, oldMatchValue;
    t_uint32        alarmsBefore = 0;
    t_bool          reprogram = FALSE;
    t_uint32        flags;
    t_rtc_date      date;
    t_rtc_error     status;

    switch (rtc_alarm_Id.type)
    {
        case RTC_NON_RECURSIVE_ALARM:
            {
                delId = rtc_alarm_Id.id;

                /* Check Id validity */
                if ((delId >= RTC_MAX_NON_REC_ALARMS) || (0 == g_rtc_system_context.rtc_nnon_rec_alarms_inserted))
                {
                    return(RTC_ERROR_INVALID_ALARM);
                }

                /* Physically delete the alarm in the array */
                SAVE_AND_DISABLE_INTERRUPTS(flags);

                /* Check if the alarm was already deleted */
                if (g_rtc_system_context.rtc_non_rec_alarm_list[delId].inUse == FALSE)
                {
                    RESTORE_INTERRUPTS(flags);
                    return(RTC_ERROR_INVALID_ALARM);
                }

                g_rtc_system_context.rtc_non_rec_alarm_list[delId].inUse = FALSE;
                g_rtc_system_context.rtc_nnon_rec_alarms_inserted--;
                alarmsBefore = (t_uint8) (g_rtc_system_context.rtc_nnon_rec_alarms_inserted + g_rtc_system_context.rtc_nrec_alarms_inserted);
                status = RTC_GetMatchDateAndTime(&date);
                status = date_to_sec(date, &oldMatchValue);
				/*coverity[self_assign]*/
                status = status;    /* to remove PC lint warning*/
				/*coverity[uninit_use]*/
                if (g_rtc_system_context.rtc_non_rec_alarm_list[delId].expTime == oldMatchValue)
                {
                    reprogram = TRUE;
                }

                RESTORE_INTERRUPTS(flags);

                break;
            }

        case RTC_RECURSIVE_ALARM:
            {
                temp_id = 0;
                if (0 == g_rtc_system_context.rtc_nrec_alarms_inserted)
                {
                    return(RTC_ERROR_INVALID_ALARM);
                }

                while (rtc_alarm_Id.id != MASK_BIT0)
                {
                    if (rtc_alarm_Id.id & MASK_BIT0)
                    {
                        return(RTC_ERROR_INVALID_ALARM);
                    }

                    rtc_alarm_Id.id = (rtc_alarm_Id.id >> 1);
                    temp_id++;
                }

                delId = (t_rtc_id) temp_id;

                /* Physically delete the alarm in the array */
                SAVE_AND_DISABLE_INTERRUPTS(flags);

                /* Check if the alarm was already deleted */
                if (g_rtc_system_context.rtc_rec_alarm_list[delId].inUse == FALSE)
                {
                    RESTORE_INTERRUPTS(flags);
                    return(RTC_ERROR_INVALID_ALARM);
                }

                g_rtc_system_context.rtc_rec_alarm_list[delId].inUse = FALSE;
                g_rtc_system_context.rtc_nrec_alarms_inserted--;
                alarmsBefore = (t_uint8) (g_rtc_system_context.rtc_nnon_rec_alarms_inserted + g_rtc_system_context.rtc_nrec_alarms_inserted);

                status = RTC_GetMatchDateAndTime(&date);
                status = date_to_sec(date, &oldMatchValue);
                status = status;    /* to remove PC lint warning*/
                if (g_rtc_system_context.rtc_rec_alarm_list[delId].expTime == oldMatchValue)
                {
                    reprogram = TRUE;
                }

                RESTORE_INTERRUPTS(flags);

                break;
            }
    }

    /* Early exit conditions */
    if (alarmsBefore == 0)
    {
        disableRtcInts();
        return(RTC_OK);
    }

    if (reprogram == FALSE)
    {
        return(RTC_OK);
    }

    /*
     * Now scan the array to decide if there is
     * the need to reprogram RTC MATCH register.
     */
    new_match_value = getNewMatch();

    /*
     * Some alarms already considered by us could have been deleted
     * in the meanwhile by the ISR. This is detected in the function,
     * where we check if alarmsBefore == rtc_nAlarmsInserted.
     * In this case, the RTC MATCH register already has the correct
     * value (set by the ISR), so we must not reprogram it.
     */
    safeMatchUpdateCond(new_match_value, alarmsBefore);

    return(RTC_OK);
}

/*****************************************************************************/
/* NAME:       t_rtc_error rtcAlarm_ChangeDate(IN t_rtc_counter new_time)    */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine deals with date changes, programming the RTC    */
/*              and possibly deleting alarms already set but now in the past */
/*              w.r.t. the new date. Called by the RTC_SetPermanentDate()    */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN:     t_rtc_counter new_time: the new date to set in the format number  */
/*                                of seconds since 00:00:00 - 01/01/2001     */
/*                                                                           */
/* OUT:                                                                      */
/*                                                                           */
/* RETURN: t_rtc_error  : RTC error code                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PRIVATE t_rtc_error rtcAlarm_ChangeDate(IN t_rtc_counter new_time, IN t_rtc_date date)
{
	/*coverity[var_decl]*/
    t_rtc_counter   now, oldMatchValue;
    t_uint32        flags;
    t_rtc_counter   new_match_value, tmpTime;
    t_uint32        temp_match;
    t_rtc_date      alarm_date, new_date_value;
    t_uint8         i;
    t_rtc_error     status;

    new_date_value.second = 0;
    new_date_value.minute = 0;
    new_date_value.hour = 0;
    new_date_value.day_of_week = 0;
    new_date_value.day = 0;
    new_date_value.month = 0;
    new_date_value.year = 0;    /* to remove pc lint warning*/

    SAVE_AND_DISABLE_INTERRUPTS(flags);

    status = RTC_GetMatchDateAndTime(&alarm_date);
    status = date_to_sec(alarm_date, &oldMatchValue);

    /* If date is before RTC MATCH value, exit */
    /*coverity[uninit_use]*/
    if (new_time < oldMatchValue)
    {
        /*RTC_LoadDateAndTime(date);*/
        RESTORE_INTERRUPTS(flags);
        return(RTC_OK);
    }
    else
    {
        /*
         * We set RTC MATCH <--- RTC COUNT - 1, so we avoid interrupts
         * for the rest of the function.
         */
        date.second--;
        RTC_LoadMatchDateAndTime(date);
        date.second++;

        /*RTC_LoadDateAndTime(date);*/
        CLEAR_RTC_INTS;
    }

    RESTORE_INTERRUPTS(flags);
    new_match_value = 0xFFFFFFFFU;

    /* Checking the non Recursive alarms for invalidated ones and finding the next 
	    match for the closest alarm	*/
    for (i = 0; i < RTC_MAX_NON_REC_ALARMS; i++)
    {
        if (g_rtc_system_context.rtc_non_rec_alarm_list[i].inUse == TRUE)
        {
            tmpTime = g_rtc_system_context.rtc_non_rec_alarm_list[i].expTime;
            status = RTC_GetCountDateAndTime(&alarm_date);
            status = date_to_sec(alarm_date, &now);

            /*
             * Alarms in the past are canceled.
             */
			/*coverity[uninit_use]*/
            if (tmpTime <= now)
            {
                g_rtc_system_context.rtc_non_rec_alarm_list[i].expTime = 0x0;
                g_rtc_system_context.rtc_non_rec_alarm_list[i].expDate = zero_date;
                g_rtc_system_context.rtc_non_rec_alarm_list[i].inUse = FALSE;
                g_rtc_system_context.rtc_nnon_rec_alarms_inserted--;
            }

            /*
             * expTime of alarms in the future
             * are candidate for RTC MATCH reprogramming
             */
            else if (tmpTime <= new_match_value)
            {
                new_match_value = tmpTime;
                new_date_value = g_rtc_system_context.rtc_non_rec_alarm_list[i].expDate;
            }
        }
    }

    for (i = 0; i < RTC_MAX_REC_ALARMS; i++)
    {
        if (g_rtc_system_context.rtc_rec_alarm_list[i].inUse == TRUE)
        {
            tmpTime = g_rtc_system_context.rtc_rec_alarm_list[i].expTime;
            status = RTC_GetCountDateAndTime(&alarm_date);
            status = date_to_sec(alarm_date, &now);

            /*
             * Alarms in the past are brought ahead to give alarms after the current time
             */
            if (tmpTime <= now)
            {
                temp_match = (now - g_rtc_system_context.rtc_rec_alarm_list[i].expTime) / g_rtc_system_context.rtc_rec_alarm_list[i].recTime;
                g_rtc_system_context.rtc_rec_alarm_list[i].expTime += (temp_match + 1) * g_rtc_system_context.rtc_rec_alarm_list[i].recTime;
                tmpTime = g_rtc_system_context.rtc_rec_alarm_list[i].expTime;
            }

            /*
             * expTime of alarms in the future
             * are candidate for RTC MATCH reprogramming
             */
            if (tmpTime <= new_match_value)
            {
                new_match_value = tmpTime;
                status = sec_to_date(&new_date_value, tmpTime);
            }
        }
    }

    /*
     * If there are no more alarms, disable interrupts.
     * Otherwise reprogram RTC with new_match_value.
     */
    if ((g_rtc_system_context.rtc_nnon_rec_alarms_inserted + g_rtc_system_context.rtc_nrec_alarms_inserted) == 0)
    {
        disableRtcInts();
    }
    else
    {
        /* safeMatchUpdate(new_match_value); */
        RTC_LoadMatchDateAndTime(new_date_value);
    }
	/*coverity[self_assign]*/
    status = status;            /*to remove PC lint warning*/
    return(RTC_OK);
}

/*
 * Scan the array to decide the new value for RTC MATCH register.
 */

/*
* It is assumed that there always exists a new match value and that it would be used
* (none of the alarms are invalid for the current value of the data register)
*/
PRIVATE t_rtc_counter getNewMatch(void)
{
    t_rtc_counter   new_match_value = 0xFFFFFFFFU, tmpTime;
    t_uint32        i;
    t_bool          flag = FALSE;
    t_uint32        temp_id = 1;

    for (i = 0; i < RTC_MAX_NON_REC_ALARMS; i++)
    {
        if (g_rtc_system_context.rtc_non_rec_alarm_list[i].inUse == TRUE)
        {
            tmpTime = g_rtc_system_context.rtc_non_rec_alarm_list[i].expTime;

            if (tmpTime < new_match_value)
            {
                new_match_value = tmpTime;
            }
        }
    }

    for (i = 0; i < RTC_MAX_REC_ALARMS; i++)
    {
        if (g_rtc_system_context.rtc_rec_alarm_list[i].inUse == TRUE)
        {
            tmpTime = g_rtc_system_context.rtc_rec_alarm_list[i].expTime;

            if (tmpTime < new_match_value)
            {
                flag = TRUE;
                new_match_value = tmpTime;
                temp_id = i;
            }
        }
    }

    if (flag == TRUE)
    {
        g_rtc_system_context.rtc_rec_alarm_list[temp_id].expTime += g_rtc_system_context.rtc_rec_alarm_list[temp_id].recTime;
    }

    return(new_match_value);
}

/*
 * Safe RTC MATCH update.
 * Even if it's a very rare event, someone could have interrupted us
 * and more than 1s can pass between reading and using RTC COUNT register,
 * so we must check the counter value again.
 * If new_match_value is <= RTC COUNT, it means that an alarm shifted 
 * in the past, so we set an interrupt for the next second, hoping
 * that the ISR will arrange things in the right way.
 * Obviously, this operation is safe only if we disable interrupts,
 * otherwise someone else could interrupt us and the counter value
 * we read could be out of sync with the real one.
 */
PRIVATE void safeMatchUpdate(IN t_rtc_counter new_match_value)
{
    t_rtc_error     status;
    t_uint32        flags;
	/*coverity[var_decl]*/
    t_rtc_counter   now;
    t_rtc_date      date, alarm;

    SAVE_AND_DISABLE_INTERRUPTS(flags);

    status = RTC_GetCountDateAndTime(&date);
    status = date_to_sec(date, &now);

    /*coverity[uninit_use]*/
    if (new_match_value > now)
    {
        status = sec_to_date(&alarm, new_match_value);
        RTC_LoadMatchDateAndTime(alarm);
    }
    else
    {
        date.second++;
        RTC_LoadMatchDateAndTime(date);
    }

    RESTORE_INTERRUPTS(flags);
	/*coverity[self_assign]*/
    status = status;    /*to remove PC lint warning*/
    return;
}

/* 
 * Same as above, but the update is done only if alarms_before 
 * equals rtc_nAlarmsInserted. We don't call the above function
 * because we want to avoid the overhead in a critical section.
 */
PRIVATE void safeMatchUpdateCond(IN t_rtc_counter new_match_value, IN t_uint32 alarms_before)
{
    t_uint32        flags;
	/*coverity[var_decl]*/
    t_rtc_counter   now;
    t_rtc_date      date, anodate;
    t_rtc_error     status = RTC_OK;

    SAVE_AND_DISABLE_INTERRUPTS(flags);

    if
    (
        alarms_before == (t_uint8)
            (g_rtc_system_context.rtc_nnon_rec_alarms_inserted + g_rtc_system_context.rtc_nrec_alarms_inserted)
    )
    {
        status = RTC_GetCountDateAndTime(&date);
        status = date_to_sec(date, &now);
        
        /*coverity[uninit_use]*/ 
        if (new_match_value > now)
        {
            status = sec_to_date(&anodate, new_match_value);
            RTC_LoadMatchDateAndTime(anodate);
        }
        else
        {
            date.second++;
            RTC_LoadMatchDateAndTime(date);
        }
    }
   	/*coverity[self_assign]*/
	status = status;
    
    RESTORE_INTERRUPTS(flags);

    return;
}

/*
 * Used when the number of alarms drops to zero. 
 * We set match reg <-- count_reg - 1, so we are reasonably
 * sure not to have unwanted interrupts, even if masked.
 */
PRIVATE void disableRtcInts(void)
{
    t_rtc_error status = RTC_OK;

    /*  t_rtc_counter   tmpTime;*/
    t_rtc_date  date;

    MASK_RTC_INTS;
    status = RTC_GetCountDateAndTime(&date);
		/*coverity[self_assign]*/
    status = status;    /*to remove PC lint warning*/

    /*	status = date_to_sec(date, &tmpTime);*/
    date.second--;
    RTC_LoadMatchDateAndTime(date);
    CLEAR_RTC_INTS;
}

/*------------------------------------------------------------------------
 * Date management functions
 *----------------------------------------------------------------------*/
/*****************************************************************************/
/* NAME:       t_rtc_error date_to_sec(                                      */
/*                                      IN t_rtc_date     rtc_date,          */
/*                                      OUT t_rtc_counter *seconds           */
/*                                    )                                      */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine converts the date from t_rtc_date format to     */
/*              number of seconds since 00:00:00 - 01/01/1999 format         */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN:     t_rtc_date     rtc_date: date in hh:mm:ss - dd/mm/yyyy format     */
/*                                                                           */
/* OUT:    t_rtc_counter *seconds: date in number of seconds format          */
/*                                                                           */
/* RETURN: t_rtc_error  : RTC error code                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PRIVATE t_rtc_error date_to_sec(IN t_rtc_date rtc_date, OUT t_rtc_counter *seconds)
{
    t_uint32    year = BASE_YEAR;
    t_uint32    offset;

    t_uint8     days_in_month[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    t_uint16    cumulative_days_in_month[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
    t_uint8     i;

    /* Check for null pointer passed */
    if (NULL == seconds)
    {
        DBGEXIT0(RTC_INVALID_PARAMETER);
        return(RTC_INVALID_PARAMETER);
    }

    /* This is a offset from 1999-01-01 00:00:00 */
    /* t_uint32 offset_from_base = 730091; */
    if
    (
        ((rtc_date.year > MAX_YEAR) || (rtc_date.year < MIN_YEAR))
    ||  ((rtc_date.month > MAX_MONTH) || (rtc_date.month < MIN_MONTH))
    ||  ((rtc_date.day > MAX_DAY) || (rtc_date.day < MIN_DAY))
    ||  ((rtc_date.hour > MAX_HOUR) /*|| (rtc_date.hour < MIN_HOUR)*/ )
    ||  ((rtc_date.minute > MAX_MIN) /*|| (rtc_date.minute < MIN_MIN)*/ )
    ||  ((rtc_date.second > MAX_SEC) /*|| (rtc_date.second < MIN_SEC)*/ )
    )
    {
        return(RTC_ERROR_INVALID_DATE);
    }

    /* check that that the date is not greater than 1/2/2135 23.59.59 */
    if ((rtc_date.year == MAX_YEAR) && (rtc_date.month > 1))
    {
        return(RTC_ERROR_INVALID_DATE);
    }

    /*correction for leap year*/
    if (isleap(rtc_date.year + BASE_YEAR))
    {
        days_in_month[1]++;
        for (i = 2; i < 12; i++)
        {
            cumulative_days_in_month[i]++;
        }
    }

    /*Checking validity of date for number of days in that month*/
    if (days_in_month[rtc_date.month - 1] < rtc_date.day)
    {
        return(RTC_ERROR_INVALID_DATE);
    }

    year += rtc_date.year;
    offset = (t_uint32) ((BASE_YEAR + 1) / 4 - (BASE_YEAR + 1) / 100 + (BASE_YEAR + 1) / 400) +
        (BASE_YEAR + 1) *
        365 +
        1;

    /*Calculations for the number of seconds*/
    *seconds =
        (
            (
                (
                    (t_uint32) ((year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400) +
                    cumulative_days_in_month[rtc_date.month - 1] +
                    rtc_date.day +
                    year *
                    365 -
                    offset
                ) * 24 + rtc_date.hour  /* now have hours 	*/
            ) *
            60 +
            rtc_date.minute             /* now have minutes */
        ) *
        60 +
        rtc_date.second;                /* finally seconds 	*/

    return(RTC_OK);
}

/*****************************************************************************/
/* NAME:       t_rtc_error sec_to_date(                                      */
/*                                      OUT t_rtc_date    *rtc_date,          */
/*                                      IN t_rtc_counter  seconds           */
/*                                    )                                      */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine converts the date from number of seconds since  */
/*              00:00:00 - 01/01/2001 format to t_rtc_date format            */
/*                                                                           */
/* PARAMETERS:                                                               */
/*                                                                           */
/* IN:     t_rtc_date    *rtc_date: date in hh:mm:ss - dd/mm/yyyy format     */
/*                                                                           */
/* OUT:    t_rtc_counter  seconds: date in number of seconds format 	     */
/*                                                                           */
/* RETURN: t_rtc_error  : RTC error code                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PRIVATE t_rtc_error sec_to_date(OUT t_rtc_date *rtc_date, IN t_rtc_counter seconds)
{
    t_uint32    year = BASE_YEAR, month = 0, days_of_week = 1, days, hour, minutes, secs, temp_sec;
    t_uint32    temp_years, days_of_year, old_temp_years, days_in_year = 1;
    t_uint32    i;

    /* Check for null pointer passed */
    if (NULL == rtc_date)
    {
        DBGEXIT0(RTC_INVALID_PARAMETER);
        return(RTC_INVALID_PARAMETER);
    }

    /*if (seconds > MAX_VALID_SEC)     */       /* 1/feb/2135 23h 59min 59sec */
    /*{
        return(RTC_ERROR_INVALID_DATE);
    } */
    
    days = (seconds / SECS_PER_DAY) + 1;

    temp_sec = (seconds % SECS_PER_DAY);

    /* calculate hour */
    hour = temp_sec / SECS_PER_HOUR;

    /* calculate minutes */
    minutes = (temp_sec % SECS_PER_HOUR) / SECS_PER_MIN;

    /* calculate seconds */
    secs = (temp_sec % SECS_PER_HOUR) % SECS_PER_MIN;

    /* year calculation */
    for (i = 1, days_of_year = old_temp_years = temp_years = 365; i <= 137; i++)
    {
        if ((((i + BASE_YEAR) % 4) == 0) && ((((i + BASE_YEAR) % 100) != 0) || (((i + BASE_YEAR) % 400) == 0)))
        {
            temp_years += 1;
            days_of_year += 1;
        }

        if (days <= temp_years)
        {
            year = i;
            days_in_year = days % old_temp_years;

            if (days_in_year == 0)
            {
                days_in_year = days_of_year;
            }
            break;
        }

        old_temp_years = temp_years;
        temp_years = temp_years + 365;
        days_of_year = 365;
    }

    /* year adjusting according to leaps etc */
    /* month and day calculation */
    month_and_day(days_in_year, isleap(BASE_YEAR + year), &month, &days);

    rtc_date->second = (t_uint8) secs;      /* second (from 0 to 59) */
    rtc_date->minute = (t_uint8) minutes;   /* minute (from 0 to 59) */
    rtc_date->hour = (t_uint8) hour;        /* hour in day (from 0 to 23) */
    rtc_date->day = (t_uint8) days;         /* day in month (from 1 to 31) */
    rtc_date->day_of_week = (t_uint8) days_of_week;
    rtc_date->month = (t_uint8) month;      /* month (from 1 to 12) */
    rtc_date->year = (t_uint16) year;        /* year from 2000 (from 01 to 137) */

    return(RTC_OK);
}

PRIVATE IS_LEAP isleap(IN t_uint32 year)
{
    /* see if the year is leap or not */
    if (((year % 4) == 0) && (((year % 100) != 0) || ((year % 400) == 0)))
    {
        return(LEAP);
    }
    else
    {
        return(NO_LEAP);
    }
}

/*
 * This function computes day and month corresponding to days_in_year days
 * since 1st Jan, taking into account if the year is leap or not.
 */
PRIVATE void month_and_day(IN t_uint32 days_in_year, IN IS_LEAP leap, OUT t_uint32 *month, OUT t_uint32 *day)
{
    t_uint32    months[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    t_uint32    i, sum, oldsum;

    if (LEAP == leap)
    {
        months[1] = 29;
    }

    for (i = 0, sum = 0, oldsum = months[0]; i < 12; i++)
    {
        /* compare the days of the year with the sum of the months' days */
        sum = sum + months[i];

        if (days_in_year <= sum)
        {
            *month = i + 1;
            *day = (days_in_year % oldsum);

            if (*day == 0)
            {
                *day = months[i];
            }

            return;
        }

        oldsum = sum;
    }
}

/*****************************************************************************/
/* NAME:       t_rtc_error day_to_sec(                                       */
/*                                     IN t_rtc_date     rtc_date,           */
/*                                     OUT t_rtc_counter *seconds            */
/*                                    )                                      */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine converts the number of days from t_rtc_date     */
/*              format to number of seconds format					         */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN:     t_rtc_date     rtc_date: date in hh:mm:ss - dd/mm/yyyy format     */
/*                                                                           */
/* OUT:    t_rtc_counter *seconds: date in number of seconds format          */
/*                                                                           */
/* RETURN: t_rtc_error  : RTC error code                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PRIVATE t_rtc_error day_to_sec(IN t_rtc_date rtc_date, OUT t_rtc_counter *seconds)
{
    /* Check for null pointer passed */
    if (NULL == seconds)
    {
        DBGEXIT0(RTC_INVALID_PARAMETER);
        return(RTC_INVALID_PARAMETER);
    }

    if
    (
        ((rtc_date.hour > MAX_HOUR) /*|| (rtc_date.hour < MIN_HOUR)*/ )
    ||  ((rtc_date.minute > MAX_MIN) /*|| (rtc_date.minute < MIN_MIN)*/ )
    ||  ((rtc_date.second > MAX_SEC) /*|| (rtc_date.second < MIN_SEC)*/ )
    )
    {
        return(RTC_ERROR_INVALID_DATE);
    }

    *seconds = (t_uint32) ((((rtc_date.day) * 24 + rtc_date.hour) * 60 + rtc_date.minute) * 60 + rtc_date.second);  /* finally seconds */

    return(RTC_OK);
}

/*****************************************************************************/
/* NAME:      void RTC_FilterProcessIRQSrc(									 */
/*										 IN t_rtc_IRQStatus *p_irq_status,	 */
/*										 OUT t_rtc_event *p_event,			 */
/*										 OUT t_rtc_alarm_id *p_rec_alarm, 	 */
/*										 OUT t_rtc_alarm_id *p_non_rec_alarm) */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is used to process the interrupts	             */
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN : t_rtc_IRQStatus *p_irq_status: status of the interrupts				 */
/*		t_rtc_event *p_event		: the events that occurred during		 */
/*									  processing of this interrupt			 */
/*		t_rtc_alarm_id *p_rec_alarm 	: recursive alarms that were 		 */
/*										  triggered							 */
/*		t_rtc_alarm_id *p_non_rec_alarm : non-recursive alarm triggered		 */
/*                                                                           */
/* OUT :                                                                     */
/*                                                                           */
/* RETURN: None										                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC t_rtc_error RTC_FilterProcessIRQSrc
(
    IN t_rtc_IRQStatus  *p_irq_status,
    OUT t_rtc_event     *p_event,
    OUT t_rtc_alarm_id  *p_rec_alarm,
    OUT t_rtc_alarm_id  *p_non_rec_alarm
)
{
    t_rtc_error     status = RTC_OK;
    t_rtc_counter   now;
   	volatile t_bool non_rec_alarm_present = FALSE;
    volatile t_bool          rec_alarm_present = FALSE;
    volatile t_uint8         ret_id = 1;
    t_rtc_counter   new_match_value;
    t_rtc_date      date;

    /* Check for null pointer passed */
    if (NULL == p_irq_status || NULL == p_event || NULL == p_rec_alarm || NULL == p_non_rec_alarm)
    {
        DBGEXIT0(RTC_INVALID_PARAMETER);
        return(RTC_INVALID_PARAMETER);
    }

    if (0 == *p_irq_status)
    {
        return(RTC_NO_PENDING_EVENT_ERROR);
    }

    /*
     * Interrupts should be cleared as soon as possible,
     * in order not to lose other events (even if probably
     * it doesn't work because RTC is not acknowledged yet).
     */
    CLEAR_RTC_INTS;
    *p_irq_status = 0;

    /*
     * 1) Scan rtc_alarmList. The alarms in the past are marked as not in use,
     *    and the most recent among them is returned to the caller.
     *    The least among the expTimes of the alarms in the future is assigned
     *    to new_match_value, to reprogram RTC MATCH register.
     */
    new_match_value = 0xFFFFFFFFU;

    status = RTC_GetCountDateAndTime(&date);
    status = date_to_sec(date, &now);
	/*coverity[self_assign]*/
    status = status;    /*to remove PC lint warning*/

    p_rec_alarm->type = RTC_RECURSIVE_ALARM;
    p_rec_alarm->id = 0x0;
    p_non_rec_alarm->type = RTC_NON_RECURSIVE_ALARM;

    /* Test for the non-recursive alarm present */
    new_match_value = rtc_TestNonRecAlarms(&non_rec_alarm_present, &ret_id);

    /* Test for the recursive alarm present */
    new_match_value = rtc_TestRecAlarms(&rec_alarm_present, p_rec_alarm);

    /*
     * 3) In any case, it's necessary to reprogram RTC MATCH register.
     *    The actions are different depending on the presence of residue
     *    alarms.
     */
    if ((g_rtc_system_context.rtc_nrec_alarms_inserted + g_rtc_system_context.rtc_nnon_rec_alarms_inserted) == 0)
    {
        /*
         * All alarms expired, so it is necessary
         * to disable RTC interrupts.
         */
        disableRtcInts();
    }
    else
    {
        safeMatchUpdate(new_match_value);

        /* RTC_LoadMatchDateAndTime(new_date_value); */
    }

    /*
     * 2) If there were alarms in the past, we return the ID of the
     *    most recent one (the return value is still RTC_OK).
     *    Otherwise, the return value is RTC_ERROR_NO_MORE_ALARM.
     */
    if (non_rec_alarm_present == TRUE)
    {
        p_non_rec_alarm->id = ret_id;
        if (rec_alarm_present == TRUE)
        {
            *p_event = RTC_MULTIPLE_ALARM_EVENT;
        }
        else
        {
            *p_event = RTC_NON_RECURSIVE_ALARM_EVENT;
        }

        status = RTC_NO_MORE_PENDING_EVENT;
    }
    else
    {
        if (rec_alarm_present == TRUE)
        {
            *p_event = RTC_RECURSIVE_ALARM_EVENT;
            status = RTC_NO_MORE_PENDING_EVENT;
        }
        else
        {
            *p_event = RTC_NO_EVENT;
            status = RTC_ERROR_NO_MORE_ALARM;
        }
    }

    g_rtc_system_context.rtc_active_events = (*p_event);
    
    new_match_value = new_match_value;
    
    return(status);
}

/* RTC Timer APIs*/
/*****************************************************************************/
/* NAME:  t_rtt_error RTT_StartTimer										 */
/*					(IN t_rtt_mode rtt_mode, const IN t_rtt_counter rtt_counter,*/
/*					 const IN t_rtt_pattern rtt_pattern)          			 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to configure the timer. If the Timer is  */
/*              started this just enables the timer, does not enable		 */
/*				interrupts.													 */
/* PARAMETERS:                                                               */
/* IN :    t_rtt_mode rtt_mode 			: Sets the timer mode				 */
/*		   t_rtt_count rtt_counter_value: Initial counter value              */
/*                                                                           */
/* OUT :   None                                                              */
/*                                                                           */
/* RETURN: t_rtt_error                  : RTT_OK                             */
/*										  RTT_INVALID_PARAMETER if null 	 */
/*										  counter value is passed            */
/*										  RTT_ERROR_TIMER_ALREADY_STARTED if */
/*										  rtt is already started		     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */

/*****************************************************************************/
PUBLIC t_rtt_error RTT_StartTimer
(
    IN t_rtt_mode           rtt_mode,
    const IN t_rtt_counter  rtt_counter,
    const IN t_rtt_pattern  rtt_pattern
)
{
    t_rtt_error         status = RTT_OK;
    t_uint32            enable;
    volatile t_uint32   time_out;
    t_uint32            num_pattern_bits;

    DBGENTER2("rtt_mode = %u, rtt_counter_value = %lu", rtt_mode, rtt_counter.rtt_counter1);

    /* Check if the Load Value is zero  */
    if ((RESET == rtt_counter.rtt_counter1) || (RESET == rtt_counter.rtt_counter2))
    {
        status = RTT_INVALID_PARAMETER;
        DBGEXIT0(status);
        return(status);
    }

    /*     Check whether the timer is already running */
    READ_RTT_ENABLE_BIT(enable);
    if (TRUE == (t_bool) enable)
    {
        /* If the timer is running, return with an error */
        status = RTT_ERROR_TIMER_ALREADY_STARTED;
        DBGEXIT0(status);
        return(status);
    }

    /* Delay added because 1 RTC clock cycle needed between two consecutive 
	 * access to control register assuming maximum clock frequency 264 MHz of
	 * the core and RTC clock at 32.768 KHz 
	 */
    time_out = RTT_CLK32;
    while (time_out--)
        ;

    /* Set the Timer Mode */
    if (RTT_MODE_PERIODIC == rtt_mode)
    {
        SET_RTT_MODE_PERIODIC;
    }
    else if (RTT_MODE_ONE_SHOT == rtt_mode)
    {
        SET_RTT_MODE_ONESHOT;
    }
    else
    {
        status = RTT_INVALID_PARAMETER;
        DBGEXIT0(status);
        return(status);
    }

    /* Delay added because 1 RTC clock cycle needed between two consecutive 
	 * access to control register assuming maximum clock frequency 264 MHz of
	 * the core and RTC clock at 32.768 KHz 
	 */
    time_out = RTT_CLK32;
    while (time_out--)
        ;

    DISABLE_RTT_SELF_START;
    time_out = RTT_CLK32;
    while (time_out--)
        ;

    num_pattern_bits = ((rtt_pattern.num_useful_pattern) << 4);

    /* HCL_WRITE_REG(g_rtc_system_context.p_rtc_reg->RTTCR, num_pattern_bits); */
    
    HCL_SET_BITS(g_rtc_system_context.p_rtc_reg->RTTCR, num_pattern_bits);
  
      time_out = RTT_CLK32;
    while (time_out--)
        ;

    /* Set the Load Register and pattern register */
    HCL_WRITE_REG(g_rtc_system_context.p_rtc_reg->RTTPR1, rtt_pattern.pattern_val1);
    HCL_WRITE_REG(g_rtc_system_context.p_rtc_reg->RTTPR2, rtt_pattern.pattern_val2);
    HCL_WRITE_REG(g_rtc_system_context.p_rtc_reg->RTTPR3, rtt_pattern.pattern_val3);
    HCL_WRITE_REG(g_rtc_system_context.p_rtc_reg->RTTPR4, rtt_pattern.pattern_val4);

    HCL_WRITE_REG(g_rtc_system_context.p_rtc_reg->RTTLR1, rtt_counter.rtt_counter1);
    HCL_WRITE_REG(g_rtc_system_context.p_rtc_reg->RTTLR2, rtt_counter.rtt_counter2);

    /* WRITE_RTT_ENABLE_BIT; */
    /*  for Test */
    ENABLE_RTT;

    DBGEXIT0(status);

    return(status);
}

/*****************************************************************************/
/* NAME:  t_rtt_error RTT_RestartTimer();									 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine starts the RTT but does not enable	interrupts.	 */
/* PARAMETERS:                                                               */
/* IN :		None  															 */
/*																			 */
/* OUT :    None                                                             */
/*                                                                           */
/* RETURN: t_rtt_error                  : RTT_OK				             */
/*										  RTT_ERROR_TIMER_ALREADY_STARTED if */
/*										  rtt is already started		     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */

/*****************************************************************************/
PUBLIC t_rtt_error RTT_RestartTimer(void)
{
    t_rtt_error         status = RTT_OK;
    t_uint32            enable;
    volatile t_uint32   time_out;

    DBGENTER0();

    /* Check whether the timer is already running */
    READ_RTT_ENABLE_BIT(enable);
    if (TRUE == (t_bool) enable)
    {
        /* If the timer is running, return with an error */
        status = RTT_ERROR_TIMER_ALREADY_STARTED;
        DBGEXIT0(status);
        return(status);
    }

    /* Delay added because 1 RTC clock cycle needed between two consecutive 
	 * access to control register assuming maximum clock frequency 264 MHz of
	 * the core and RTC clock at 32.768 KHz 
	 */
    time_out = RTT_CLK32;
    while (time_out--)
        ;

    /* Enable the timer */
    ENABLE_RTT;

    DBGEXIT0(status);

    return(status);
}

/*****************************************************************************/
/* NAME:  t_rtt_error RTT_StopTimer();										 */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine stops the timer.								 */
/* PARAMETERS:                                                               */
/* IN :		None															 */
/*																			 */
/* OUT :    None                                                             */
/*                                                                           */
/* RETURN: t_rtt_error                  : RTT_OK				             */
/*										  RTT_ERROR_TIMER_ALREADY_STOPPED if */
/*										  rtt is already stopped		     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */

/*****************************************************************************/
PUBLIC t_rtt_error RTT_StopTimer(void)
{
    t_rtt_error         status = RTT_OK;
    t_uint32            enable;
    volatile t_uint32   time_out;

    DBGENTER0();

    /* Check whether the timer is already stopped */
    READ_RTT_ENABLE_BIT(enable);
    if (FALSE == (t_bool) enable)
    {
        /* If the timer is stopped, return with an error */
        status = RTT_ERROR_TIMER_ALREADY_STOPPED;
        DBGEXIT0(status);
        return(status);
    }

    /* Delay added because 1 RTC clock cycle needed between two consecutive 
	 * access to control register assuming maximum clock frequency 264 MHz of
	 * the core and RTC clock at 32.768 KHz 
	 */
    time_out = RTT_CLK32;
    while (time_out--)
        ;

    DISABLE_RTT;

    DBGEXIT0(status);

    return(status);
}

/*****************************************************************************/
/* NAME:  t_rtt_error RTT_GetCounterValue(t_rtt_count *p_rtt_counter_value); */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the counter value.						 */
/* PARAMETERS:                                                               */
/* IN :																		 */
/*																			 */
/* OUT :     t_rtt_count *pRttCounterValue: Returns the Counter Value        */
/*                                                                           */
/* RETURN: t_rtt_error                  : RTT_OK                             */
/*                                        RTT_INVALID_PARAMETER if null 	 */
/*										  pointer is passed		   		     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */

/*****************************************************************************/
PUBLIC t_rtt_error RTT_GetCounterValue(OUT t_rtt_count *p_rtt_counter_value)
{
    DBGENTER1("p_rtt_counter_value = @%p", (void *) p_rtt_counter_value);

    /* Check if null pointer is passed */
    if (NULL == p_rtt_counter_value)
    {
        DBGEXIT0(RTT_INVALID_PARAMETER);
        return(RTT_INVALID_PARAMETER);
    }

    /* Get the current Counter value */
    GET_RTT_COUNTER(*p_rtt_counter_value);

    DBGEXIT1(RTT_OK, "Counter Value :%lu", *p_rtt_counter_value);

    return(RTT_OK);
}

/*****************************************************************************/
/* NAME:  t_rtt_error RTT_CheckTimerState(t_rtt_timer_state *p_rtt_state)    */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine returns the timer state.						 */
/* PARAMETERS:                                                               */
/* IN :		None															 */
/*																			 */
/* OUT :    t_rtt_timer_state *p_rtt_state : Returns the Timer State         */
/*                                                                           */
/* RETURN: t_rtt_error                  : RTT_OK                             */
/*										  RTT_INVALID_PARAMETER if null 	 */
/*										  pointer is passed		   		 	 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: Non-Reentrant                                                 */

/*****************************************************************************/
PUBLIC t_rtt_error RTT_CheckTimerState(OUT t_rtt_timer_state *p_rtt_state)
{
    t_rtt_error status = RTT_OK;
    t_uint32    enable;

    DBGENTER1("p_rtt_state = @%p", (void *) p_rtt_state);

    /* Check if null pointer is passed */
    if (NULL == p_rtt_state)
    {
        DBGEXIT0(RTT_INVALID_PARAMETER);
        return(RTT_INVALID_PARAMETER);
    }

    /* Check if The Timer is Enabled/Disabled */
    READ_RTT_ENABLE_BIT(enable);

    if (TRUE == (t_bool) enable)
    {
        *p_rtt_state = RTT_STATE_RUNNING;
    }
    else
    {
        *p_rtt_state = RTT_STATE_STOPPED;
    }

    DBGEXIT1(status, "Timer State :%d", *p_rtt_state);

    return(status);
}

/*****************************************************************************/
/* NAME:      t_rtt_error RTT_Reset(void) 	    	                         */
/*---------------------------------------------------------------------------*/
/* DESCRIPTION: This routine resets all RTT registers to power on reset value*/
/*                                                                           */
/* PARAMETERS:                                                               */
/* IN :    None                                         					 */
/*                                                                           */
/* OUT :   None                                                              */
/*                                                                           */
/* RETURN: t_rtt_error     : RTT_OK                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                            */

/*****************************************************************************/
PUBLIC t_rtt_error RTT_Reset(void)
{
    volatile t_uint32   time_out;

    DBGENTER0();

    DISABLE_RTT;

    /* Delay added because 1 RTC clock cycle needed between two consecutive 
	 * access to control register assuming maximum clock frequency 264 MHz of
	 * the core and RTC clock at 32.768 KHz 
	 */
    time_out = RTT_CLK32;
    while (time_out--)
        ;

    g_rtc_system_context.p_rtc_reg->RTTLR1 = 0x0UL;
    g_rtc_system_context.p_rtc_reg->RTTLR2 = 0x0UL;

    /* Delay added because 1 RTC clock cycle needed between two consecutive 
	 * access to control register assuming maximum clock frequency 264 MHz of
	 * the core and RTC clock at 32.768 KHz 
	 */
    time_out = RTT_CLK32;
    while (time_out--)
        ;

    g_rtc_system_context.p_rtc_reg->RTTCR = 0x0UL;

    /* Clear the timer interrupt source */
    CLEAR_RTT_INTS;

    /* Mask the timer interrupt */
    MASK_RTT_INTS;

    g_rtc_system_context.p_rtc_reg = NULL;

    DBGEXIT0(RTT_OK);

    return(RTT_OK);
}

/* Private APIs*/
PRIVATE t_rtc_error RTC_GetMatchDateAndTime(OUT t_rtc_date *rtc_date)
{
    t_rtc_error status = RTC_OK;
    t_uint32    cal = 0;

    /* Check for null pointer passed */
    if (NULL == rtc_date)
    {
        DBGEXIT0(RTC_INVALID_PARAMETER);
        return(RTC_INVALID_PARAMETER);
    }

    rtc_date->year = (t_uint16)bcdtoi(((g_rtc_system_context.p_rtc_reg)->RTCYMR)) - BASE_YEAR;
    cal = (g_rtc_system_context.p_rtc_reg)->RTCMR;

    rtc_date->month = (t_uint8) ((cal & MASK_CWMONTH) >> 25);
    rtc_date->day = (t_uint16) ((cal & MASK_CWDAYM) >> 20);
    rtc_date->day_of_week = (t_uint8) ((cal & MASK_CWDAYW) >> 17);
    rtc_date->hour = (t_uint8) ((cal & MASK_CWHOUR) >> 12);
    rtc_date->minute = (t_uint8) ((cal & MASK_WMIN) >> 6);
    rtc_date->second = (t_uint8) (cal & MASK_CWSEC);

    return(status);
}

PRIVATE t_rtc_error RTC_GetCountDateAndTime(OUT t_rtc_date *rtc_date)
{
    t_rtc_error status = RTC_OK;
    t_uint32    cal = 0;

    /* Check for null pointer passed */
    if (NULL == rtc_date)
    {
        DBGEXIT0(RTC_INVALID_PARAMETER);
        return(RTC_INVALID_PARAMETER);
    }

    rtc_date->year = (t_uint16) bcdtoi(((g_rtc_system_context.p_rtc_reg)->RTCYR)) - BASE_YEAR;
    cal = (g_rtc_system_context.p_rtc_reg)->RTCDR;

    rtc_date->month = (t_uint8)((cal & MASK_CWMONTH) >> 25);
    rtc_date->day = (t_uint16)((cal & MASK_CWDAYM) >> 20);
    rtc_date->day_of_week = (t_uint8)((cal & MASK_CWDAYW) >> 17);
    rtc_date->hour = (t_uint8)((cal & MASK_CWHOUR) >> 12);
    rtc_date->minute = (t_uint8)((cal & MASK_WMIN) >> 6);
    rtc_date->second = (t_uint8)(cal & MASK_CWSEC);

    return(status);
}

/*
PRIVATE void RTC_LoadDateAndTime(IN t_rtc_date rtc_date)
{
    volatile t_uint32   dummy = 0;

    HCL_WRITE_BITS(dummy, (rtc_date.month) << 25, MASK_CWMONTH);
    HCL_WRITE_BITS(dummy, (rtc_date.day) << 20, MASK_CWDAYM);
    HCL_WRITE_BITS(dummy, (rtc_date.day_of_week) << 17, MASK_CWDAYW);
    HCL_WRITE_BITS(dummy, (rtc_date.hour) << 12, MASK_CWHOUR);
    HCL_WRITE_BITS(dummy, (rtc_date.minute) << 6, MASK_WMIN);
    HCL_WRITE_BITS(dummy, (rtc_date.second), MASK_CWSEC);
    (g_rtc_system_context.p_rtc_reg)->RTCLR = dummy;
    HCL_WRITE_BITS
    (
        (g_rtc_system_context.p_rtc_reg)->RTCYLR,
        itobcd(rtc_date.year) + itobcd(BASE_YEAR), 
        MASK_CWYEAR
    );
}
*/
PRIVATE void RTC_LoadMatchDateAndTime(IN t_rtc_date rtc_date)
{
    volatile t_uint32   dummy = 0;

    HCL_WRITE_BITS(dummy, (rtc_date.month) << 25, MASK_CWMONTH);
    HCL_WRITE_BITS(dummy, (rtc_date.day) << 20, MASK_CWDAYM);
    HCL_WRITE_BITS(dummy, (rtc_date.day_of_week) << 17, MASK_CWDAYW);
    HCL_WRITE_BITS(dummy, (rtc_date.hour) << 12, MASK_CWHOUR);
    HCL_WRITE_BITS(dummy, (rtc_date.minute) << 6, MASK_WMIN);
    HCL_WRITE_BITS(dummy, (rtc_date.second), MASK_CWSEC);

    HCL_WRITE_BITS
    (
        (g_rtc_system_context.p_rtc_reg)->RTCYMR,
        itobcd(rtc_date.year) + itobcd(BASE_YEAR) /*itobcd(rtc_date.year+BASE_YEAR)*/,
        MASK_CWYEAR
    );
    (g_rtc_system_context.p_rtc_reg)->RTCMR = dummy;
}

