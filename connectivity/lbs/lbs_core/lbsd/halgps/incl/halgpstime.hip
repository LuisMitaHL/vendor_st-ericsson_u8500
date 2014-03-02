/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#ifndef __HALGPSTIME_HIP__
#define __HALGPSTIME_HIP__
/**
* \file halgpstime.hip
* \date 25/01/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contain all function protoype declared in halgps4time.c.\n
* 
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 25.01.08</TD><TD> M.BELOU </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

/**
* \fn U4 GN_YMDHMS_To_CTime(U2 Year,U2 Month,U2 Day,U2 Hours,U2 Minutes,U2 Seconds )
* \brief Time conversion 
*
* Convert Time in YY-MM-DD & HH:MM:SS format to 'C' Time units [seconds].
* ('C' Time starts on the 1st Jan 1970.)
*
* \param Year    Year         [eg 2006]
* \param Month   Month        [range 1..12]
* \param Day     Days         [range 1..31]
* \param Hours   Hours        [range 0..23]
* \param Minutes Minutes      [range 0..59]
* \param Seconds Seconds      [range 0..59]
*
* \return time in 'C' Time units [seconds]
*/

U4 GN_YMDHMS_To_CTime(U2 Year,U2 Month,U2 Day,U2 Hours,U2 Minutes,U2 Seconds );

/**
* \fn void GN_CTime_To_YMDHMS(U4 C_Time,U2 *Year,U2 *Month,U2 *Day,U2 *Hours,U2 *Minutes, U2 *Seconds )
* \brief Time conversion 
*
* Convert Time in 'C' Time units [seconds] to a YY-MM-DD & HH:MM:SS format.
* ('C' Time starts on the 1st Jan 1970.)
*
* \param C_Time  Time in 'C' Time units [seconds]
* \param Year    Year         [eg 2006]
* \param Month   Month        [range 1..12]
* \param Day     Days         [range 1..31]
* \param Hours   Hours        [range 0..23]
* \param Minutes Minutes      [range 0..59]
* \param Seconds Seconds      [range 0..59]
*/
void GN_CTime_To_YMDHMS(U4 C_Time,U2 *Year,U2 *Month,U2 *Day,U2 *Hours,U2 *Minutes, U2 *Seconds );


#endif /* __HALGPSTIME_HIP__ */
