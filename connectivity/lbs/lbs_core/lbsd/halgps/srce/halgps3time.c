/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

/**
* \file halgps3time.c
* \date 25/01/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B>  This file contains all the callback functions needed 
* to run the GPS library\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> ?????? </TD><TD> ???EX-GLONAV GUY?? </TD><TD> Creation </TD>
*     </TR>
*     <TR>
*             <TD> 25.01.08 </TD><TD> M.BELOU </TD><TD> Sysol adaptation </TD>
*     </TR>
* </TABLE>
*/

#define __HALGPS3TIME_C__

#undef  MODULE_NUMBER
#define MODULE_NUMBER   MODULE_HALGPS

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER  PROCESS_HALGPS

#undef  FILE_NUMBER
#define FILE_NUMBER     3

#include "halgpstime.h"

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
/**
* \fn U4 GN_YMDHMS_To_CTime(U2 Year,U2 Month,U2 Day,U2 Hours,U2 Minutes,U2 Seconds )
*/

U4 GN_YMDHMS_To_CTime(U2 Year,U2 Month,U2 Day,U2 Hours,U2 Minutes,U2 Seconds )
{
    /* Return Value Definition*/
    U4 CTime;                  /* Time in 'C' Time units [seconds]*/
    /* Local Definitions*/
    I4 GPS_secs;               /* GPS Seconds since 5/6 Jan 1980.*/
    I4 dayOfYear;              /* Completed days into current year*/
    I4 noYears;                /* Completed years since 1900*/
    I4 noLeapYears;            /* Number of leap years between 1900 and present*/
    I4 noDays;                 /* Number of days since midnight, Dec 31/Jan 1, 1900.*/
    I4 noGPSdays;              /* Number of days since start of GPS time*/

    /* Compute the day number into the year*/
    dayOfYear = (I4)( Days_to_Month[Month-1] + Day );

    /* Leap year check.  (Note that this algorithm fails at 2100!)*/
    if ( Month > 2  &&  (Year%4) == 0 )  dayOfYear++;

    /* The number of days between midnight, Dec31/Jan 1, 1900 and
    *  midnight, Jan5/6, 1980 (ie start of GPS) is 28860.
    */
    noYears     = (I4)Year - 1901;
    noLeapYears = noYears / 4;
    noDays      = (noYears*365) + noLeapYears + dayOfYear;
    noGPSdays   = noDays - 28860;                    /* Number of GPS days*/
    GPS_secs    = noGPSdays*86400 + Hours*3600 + Minutes*60  + Seconds;

    /* Convert from GPS Time to C Time*/
    CTime = GPS_secs + Diff_GPS_C_Time;

    return( CTime );
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/**
* \fn void GN_CTime_To_YMDHMS(U4 C_Time,U2 *Year,U2 *Month,U2 *Day,U2 *Hours,U2 *Minutes, U2 *Seconds )
* \brief Time conversion 
*/
void GN_CTime_To_YMDHMS(U4 C_Time,U2 *Year,U2 *Month,U2 *Day,U2 *Hours,U2 *Minutes, U2 *Seconds )
{
    I4 GPS_secs;               /* GPS Seconds since 5/6 Jan 1980.*/
    I2 gpsWeekNo;              /* GPS Week Number*/
    I4 gpsTOW;                 /* GPS Time of Week [seconds]*/
    I4 loSecOfD;               /* Local Second of Day    (range 0-86399)*/
    I4 loYear;                 /* Local Year             (range 1980-2100)*/
    I4 loDayOfW;               /* Local Day of Week      (range 1-7)*/
    I4 loDayOfY;               /* Local Day of Year      (range 1-366)*/
    I4 loSecOfH;               /* Local Second of Hour   (range 0-3599)*/
    I4 i;                      /* Loop index*/
    I4 tempI4;                 /* Temporary I4 value*/

    /* Convert from 'C' Time to GPS Time.*/
    GPS_secs = C_Time - Diff_GPS_C_Time;

    /* Convert UTC Time of Week to Day of Week, Hours, Minutes and Seconds.*/
    gpsWeekNo = (I2)( GPS_secs /604800 );
    gpsTOW    = GPS_secs - 604800*gpsWeekNo;

    loDayOfW  = gpsTOW / 86400;            /* Calculate completed Days into Week*/
    loSecOfD  = gpsTOW - 86400*loDayOfW;   /* Calculate current Second of Day*/
    tempI4    = loSecOfD / 3600;           /* Calculate current Hour of Day*/
    *Hours    = (U2)tempI4;                /* Store current Hour of Day*/
    loSecOfH  = loSecOfD - 3600*tempI4;    /* Calculate current Second of Hour*/
    tempI4    = loSecOfH / 60;             /* Calculate current Minute of Hour*/
    *Minutes  = (U2)tempI4;                /* Store current Minute of Hours*/
    *Seconds  = (U2)(loSecOfH - 60*tempI4);/* Calc & Store current Minute of Second*/

    /* Convert day of week and week number to day of year (tempI4) and year*/
    tempI4  = loDayOfW + (I4)gpsWeekNo*7;  /* Calculate GPS day number*/
    tempI4  = tempI4 + 6;                  /* Offset for start of GPS time 6/1/1980*/
    loYear  = 1980;

    /* Advance completed 4 years periods,  which includes one leap year.
    * (Note that this algorithm fails at 2100, which is not a leap year.)
    */
    while ( tempI4 > ((365*4) + 1) )
    {
        tempI4 = tempI4 - ((365*4) + 1);
        loYear = loYear + 4;
    };
    /* Advance remaining completed years, which don't include any leap years.
    * (Note that this algorithm fails at 2100, which is not a leap year.)
    */
    while ( tempI4 > 366 )
    {
        tempI4 = tempI4 - 365;
        if ( (loYear & 0x3) == 0 ) tempI4--;   /* TRUE for leap years (fails at 2100)*/
        loYear++;
    };
    /* Check for one too many days in a non-leap year.
    * (Note that this algorithm fails at 2100, which is not a leap year.)
    */
    if ( tempI4 == 366  &&  (loYear & 0x3) != 0 )
    {
        loYear++;
        tempI4 = 1;
    }

    loDayOfY = tempI4;
    *Year    = (U2)loYear;

    /* Convert Day of Year to Day of Month and Month of Year*/
    for ( i=0 ; i<12 ; i++ )
    {
        if ( loDayOfY <= Days_in_Month[i] )
        {
            *Day   = (U2)loDayOfY;
            *Month = (U2)i+1;
            break;
        }
        else
        {
            loDayOfY = loDayOfY - Days_in_Month[i];

            /* Check for the extra day in February in Leap years*/
            if ( i == 1  &&  (loYear & 0x3) == 0 )   /* Only Works up to 2100*/
            {
                if ( loDayOfY > (29-28) )  // After Feb 29th in a Leap year
                {
                    loDayOfY--;                // Take off the 29th Feb
                }
                else                       // Must be the 29th of Feb on a Leap year
                {
                    *Day     = (U2)29;
                    *Month   = (U2)2;
                    break;
                }
            }
        }
    }

    return;
}
#undef __HALGPS3TIME_C__
