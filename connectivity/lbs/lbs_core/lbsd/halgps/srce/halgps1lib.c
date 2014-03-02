/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
* \file halgps1lib.c
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
#define __HALGPS1LIB_C__

#undef  MODULE_NUMBER
#define MODULE_NUMBER   MODULE_HALGPS

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER  PROCESS_CGPS

#undef  FILE_NUMBER
#define FILE_NUMBER     1

#include "halgpslib.h"
#ifdef WIN32
#pragma warning(disable : 4267)
#endif

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/**
* \fn U2 GN_GPS_Read_NV_Store( U2 NV_size,U1 *p_NV_Store )
* \brief CPS library Callback to read file.
*
* GPS library Callback Function to Read back the GPS Non-Volatile Store Data from
* the Host's chosen Non-Volatile Store media.
*
* \param NV_size Size of the NV Store Data [bytes]
* \param p_NV_Store Pointer to the NV Store Data
*
* \return Returns the number of bytes actually read back.  If this is not equal to
* 'NV_size' then it is assumed that the Non-Volatile Store Data read back is
* invalid and Time-To_First_fix will be significantly degraded.
*/
U2 GN_GPS_Read_NV_Store(U2 NV_size, U1 *p_NV_Store )
{

    size_t num_read=0;                  /* Number of bytes read*/

    /* Firstly, this is a good time to recover the Real-Time Clock Calibration
    * Data from file to the global structure where it will be read from in
    * function GN_GPS_Read_UTC_Data().
    * Open the Real-Time Clock data file for reading, in binary mode.
    * Note that, if the file does not exist, it must not be created.
    */
    {
        t_gfl_FILE * fp_RTC_Calib=NULL;           /* RTC Calibration File Pointer*/
        s_RTC_Calib *pl_RTC_Calib=NULL;

        /* not needed to read calibration if the calibration is always given by library */
        if ((g_fisrtCalibration==TRUE) && (vg_HALGPS_CalibrationFullPathFileName != NULL))
        {
            fp_RTC_Calib = (t_gfl_FILE *)MC_GFL_FOPEN((char*)vg_HALGPS_CalibrationFullPathFileName,(const U1*)"rb" );

            if ( fp_RTC_Calib == NULL )
            {
                /* WARNING: Could not open file */
                memset( &gn_RTC_Calib, 0, sizeof(*pl_RTC_Calib) );   /* Clear the RTC Calib data*/
                /* generate an error */
                MC_HALGPS_TRACE_INF("GN_GPS_Read_NV_Store: ErrorUN opening %s", vg_HALGPS_CalibrationFullPathFileName);
                vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
                MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
            }
            else
            {
                /* Read the RTC Calibration data*/
                num_read = MC_GFL_FREAD(&gn_RTC_Calib,1, sizeof(*pl_RTC_Calib),fp_RTC_Calib);

                /* Close the RTC Calibration data file*/
                MC_GFL_FCLOSE(fp_RTC_Calib);

                /* Check that the correct number of bytes were read*/
                if ( num_read != sizeof(*pl_RTC_Calib) )
                {
                    memset( &gn_RTC_Calib, 0, sizeof(*pl_RTC_Calib) );   /* Clear the RTC Calib data*/

                    /* generate an error */
                    vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
                    MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
                }
                else
                {
                    /*RTC Calibration data read OK*/
                }
            }
        }
    }

    /* Open the Non-Volatile Store data file for reading, in binary mode.*/
    /* Note that, if the file does not exist, it must not be created.*/
    {
        t_gfl_FILE * fp_NVol; /* GPS Non-Volatile data File pointer*/

        if (vg_HALGPS_StoreFullPathFileName != NULL)
        {
            fp_NVol = (t_gfl_FILE *)MC_GFL_FOPEN( (char*)vg_HALGPS_StoreFullPathFileName,(const U1*) "rb" );

            if ( fp_NVol == NULL )
            {
                MC_HALGPS_TRACE_INF("GN_GPS_Read_NV_Store: ErrorTROIS opening %s", vg_HALGPS_StoreFullPathFileName);
                memset( p_NV_Store, 0, NV_size );            /* Clear NV Store data*/
                /* generate an error */
                vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_THIRD_ERROR);
                MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
                return( 0 );
            }

            /* Read the NV Store data*/
            num_read = MC_GFL_FREAD( p_NV_Store, 1, NV_size, fp_NVol );

            /* Close the NV Store file*/
            MC_GFL_FCLOSE( fp_NVol );

            /* Check that the correct number of bytes were read*/
            if ( num_read != NV_size )
            {
                memset( p_NV_Store, 0, NV_size );            /* Clear NV Store data*/
                num_read = 0;
                /* generate an error */
                vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FOURTH_ERROR);
                MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
            }
            else
            {
                /* NV Store data read OK*/
            }
        }
    }

    /* Return the size of NV Store data read back.*/
    return( (U2)num_read );
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/**
* \fn U2 GN_GPS_Write_NV_Store( U2 NV_size,U1 *p_NV_Store)
* \brief CPS library Callback to write file.
*
* GPS library Callback Function to Write the GPS Non-Volatile Store Data to the
* Host's chosen Non-Volatile Store media.
*
* \param NV_size Size of the NV Store Data [bytes]
* \param p_NV_Store Pointer to the NV Store Data
*
* \return Returns the number of bytes actually written.
*/
void GN_GPS_Write_NV_Store(U2 NV_size,U1 *p_NV_Store )
{
    size_t  vl_NumWrite;         /* Number of bytes written*/

    /* Open the non-volatile data file for writing, in binary mode.
    * If the file already exists it will be over-written, otherwise
    * it will be created.
    */
    {
        t_gfl_FILE * fp_NVol;          /* GPS Non-Volatile data File pointer*/

        if (vg_HALGPS_StoreFullPathFileName != NULL)
        {
            fp_NVol = (t_gfl_FILE *)MC_GFL_FOPEN( (char*)vg_HALGPS_StoreFullPathFileName,(const U1*) "wb" );
            if ( fp_NVol == NULL )
            {
                /* WARNING: Could not open file*/
                /* generate an error */
                vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
                MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
                return;
            }

            /* Write the NV Store data to the NV Store file*/
            vl_NumWrite = MC_GFL_FWRITE( p_NV_Store, 1, (size_t)NV_size, fp_NVol );

            if (vl_NumWrite != NV_size)
            {
                vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
                MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
            }
            /* Flush & Close the NV Store file*/
            MC_GFL_FFLUSH( fp_NVol );
            MC_GFL_FCLOSE( fp_NVol );
        }
    }

    /* Finally, save this is a good time to save the RTC Calibration Data from
    * the structure it was written to in GN_GPS_Write_UTC_Data() to file.
    * Open the non-volatile data file for writing, in binary mode.
    * If the file already exists it will be over-written, otherwise
    * it will be created.
    */
    {
        t_gfl_FILE * fp_RTC_Calib=NULL;           /* RTC Calibration File Pointer*/

        s_RTC_Calib *pl_RTC_Calib=NULL;

        if ((g_fisrtCalibration == FALSE) && (vg_HALGPS_CalibrationFullPathFileName!= NULL))
        {
            fp_RTC_Calib = (t_gfl_FILE *)MC_GFL_FOPEN( (char*)vg_HALGPS_CalibrationFullPathFileName,(const U1*) "wb" );
            if ( fp_RTC_Calib == NULL )
            {
                /* WARNING: Could not open file*/
                vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_THIRD_ERROR);
                MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
                return;
            }

            /* Write the RTC Calibration Data to the RTC Calibration file*/
            vl_NumWrite = MC_GFL_FWRITE( &gn_RTC_Calib, 1, sizeof(*pl_RTC_Calib), fp_RTC_Calib );

            if (vl_NumWrite != sizeof(*pl_RTC_Calib))
            {
                vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FOURTH_ERROR);
                MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
            }

            /* Flush & Close the RTC Calibration file*/
            MC_GFL_FFLUSH( fp_RTC_Calib );
            MC_GFL_FCLOSE( fp_RTC_Calib );
        }
    }


    return;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/**
* \fn U4 GN_GPS_Get_OS_Time_ms(void)
* \brief CPS library Callback to get current OS time
*
* GN GPS Callback Function to Get the current OS Time tick in integer
* millisecond units.  The returned 'OS_Time_ms' must be capable of going all
* the way up to and correctly wrapping a 32-bit unsigned integer boundary.
*
* \return Returns OS time in millisecond
*/

U4 GN_GPS_Get_OS_Time_ms( void )
{
#ifdef __RTK_E__
/********************** RTK_E ****************************/
    uint32_t ms = 0;
#ifdef UTS_CPR
    t_UTSER_CounterRead vl_counterRead;
#endif

#ifdef UTS_CPR
    if (MC_UTSER_READ_COUNTER(g_HALGPS_OSCounter,&vl_counterRead)==UTSER_OK)
    {
        ms = vl_counterRead.v_countValue;
        if (vl_counterRead.v_countValue==0)
        {
            /*possibility of error*/
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }
    }
    else
    {
        /*generate an error*/
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
    }
#else
    /* Return of actual time in ms (1 tick = 60/13ms = 4.615ms) */
    ms = MC_OST_GET_SYS_TIME;
#endif
    return ms;

#elif AGPS_LINUX_FTR
/********************** LINUX ****************************/
   struct tms  process_times;
   clock_t     pr_time;

   // Get the current process time.
   // Note that times() is used because clock() does not appear to work properly.
   pr_time = times( &process_times );

   // If the process time counts in 1ms units then return the value directly
   // as it correctly wraps every 2^32 milliseconds.
   if ( gn_CLK_TCK == 1000 )
   {
      return( (U4)pr_time +HALGPS_SYSTEM_TIME_OFFSET);
   }

   // Otherwise, if the process time count 100 times per second, ie 10ms for 1 tick.
   else if ( gn_CLK_TCK == 100 )
   {
      static BL tick_count_set = FALSE;
      static U4 last_clk       = 0;
      static U4 OS_Time_ms     = 0;

      if ( ! tick_count_set )
      {
         last_clk       = (U4)pr_time;
         OS_Time_ms     = (last_clk * 10)+HALGPS_SYSTEM_TIME_OFFSET;
         tick_count_set = TRUE;
      }
      else
      {
         U4 this_clk = (U4)pr_time;
         if ( this_clk >= last_clk )
         {
            OS_Time_ms = OS_Time_ms  +  ( this_clk - last_clk ) * 10;
         }
         else
         {
            OS_Time_ms = OS_Time_ms  + ( (0xFFFFFFFF - last_clk) + this_clk + 1 ) * 10;
         }
         last_clk = this_clk;
      }
      return( OS_Time_ms );
   }

   // Otherwise, if the process time count 10000 times per second, ie 10 in 1ms.
   else if ( gn_CLK_TCK == 10000 )
   {
      static BL tick_count_set = FALSE;
      static U4 last_clk       = 0;
      static U4 OS_Time_ms     = 0;

      if ( ! tick_count_set )
      {
         last_clk       = (U4)pr_time;
         OS_Time_ms     = (last_clk / 10) +HALGPS_SYSTEM_TIME_OFFSET;
         tick_count_set = TRUE;
      }
      else
      {
         U4 this_clk = (U4)pr_time;
         if ( this_clk >= last_clk )
         {
            OS_Time_ms = OS_Time_ms  +  ( this_clk - last_clk ) / 10;
         }
         else
         {
            OS_Time_ms = OS_Time_ms  + ( (0xFFFFFFFF - last_clk) + this_clk + 1 ) / 10;
         }
         last_clk = this_clk;
      }
      return( OS_Time_ms );
   }

   // Otherwise, the process time counts in as yet unsupported units, so return 0.
   else
   {
      return( 0 );
   }
#elif WIN32
/********************** WIN32 ****************************/
   return 0;
#endif
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/**
* \fn U4 GN_GPS_Get_Meas_OS_Time_ms( void )
* \brief CPS library Callback to get OS time since the GPS module started
*
* GN GPS Callback Function to Get the OS Time tick, in integer millisecond
* units, corresponding to when a burst of Measurement data received from
* GPS Baseband started.  This helps in determining the system latency.
* If it is not possible to determine this value then, return( 0 );
* The returned 'OS_Time_ms' must be capable of going all the way up to and
* correctly wrapping a 32-bit unsigned integer boundary.
*
* \return Returns OS time in millisecond since the GPS module started
*/

U4 GN_GPS_Get_Meas_OS_Time_ms( void )
{
    uint32_t vl_returnValue=0;
#if defined LBS_INTRPT_BB_READ
    uint32_t vl_ostime;

    if (vg_HALGPS_LantencyTimer != 0)
    {
        vl_ostime = GN_GPS_Get_OS_Time_ms();

        if (vg_HALGPS_LantencyTimer<vl_ostime)
        {
            vl_returnValue = vg_HALGPS_LantencyTimer;
        }
        else
        {
            /*the latency can't be negative*/
            /*generate an error*/
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }

    }
    else
    {
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
    }
#endif
    return(vl_returnValue);
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/**
* \fn BL GN_GPS_Read_UTC(s_GN_GPS_UTC_Data *p_UTC )
* \brief CPS library Callback to read UTC.
*
* GPS library Callback Function to Read back the current UTC Date & Time data
* (eg from the Host's Real-Time Clock).
*
* \param p_UTC Pointer to the UTC Date & Time
*
* \return Returns TRUE if successful.
*/
BL GN_GPS_Read_UTC( s_GN_GPS_UTC_Data *p_UTC )
{
    /* RTC is calibrated by the GPS. That is, the offset between the GPS time
    *  and the RTC time is computed and stored in a structure (checksumed) which
    *  is then in-turn stored in a file when we save the NV_Store data.
    *  When the GPS Library makes this function call (i.e. calls GN_GPS_Read_UTC),
    *  we check the content of the calibration data against its expected checksum
    *  and if it passes we apply the calibration.
    *  If we find anything wrong with the calibration data we assume that
    *  the RTC Time cannot be trusted and we return( FALSE ) to say that there
    *  is no starting UTC Time available.
    *
    *  Note that, if the error in the time returned by this function is
    *  significantly greater than is indicated by the associated
    *  accuracy estimate (i.e. 'Acc_Est' in the s_GN_GPS_UTC_Data structure)
    *  then the performance of the GPS might be degraded.
    */
#ifdef HALGPS_DONOT_USE_RTC
    p_UTC = p_UTC;
    return FALSE;
#else

    U4             Curr_SecureClock;            /* Secure clock counter*/
    U4             Curr_CTime;                  /* Current Time in 'C' Time units [seconds]*/
    I4             dT_s;                        /* A Time difference [seconds]*/
    I4             acc_est_deg;                 /* Time Accuracy Estimate degradation [ms]*/
    I4             acc_est_now;                 /* Time Accuracy Estimate now [ms]*/
    I4             age;                         /* Age since RTC Last Set/Calibrated*/
    U4             checksum;                    /* RTC Calibration File 32-bit checksum*/
    I4             vl_offsetMs=0;               /* Time difference [millisecond]*/
    long           vl_secureClockDelta =0;      /* Secure clock difference [second]*/
    long           vl_osClockDelta =0;          /* OS time difference [millisecond]*/
    s_GN_GPS_UTC_Data *pl_GN_GPS_UTC_Data=NULL;


    if (p_UTC == NULL)
    {
        return (FALSE);
    }

    /* Clear the return UTC time to a NULL state with the Estimated Accuracy
    *  set very high (ie a NOT SET state).
    */
    memset( p_UTC, 0, sizeof(*pl_GN_GPS_UTC_Data) );
    p_UTC->Acc_Est  =  0x7FFFFF00;

    /* Get the current OS time [ms]*/
    p_UTC->OS_Time_ms = GN_GPS_Get_OS_Time_ms();

    /* Get a RTC counter */
    /* this counter must be used with the calibration file to calculate the UTC*/
#ifdef __RTK_E__
    Curr_SecureClock = MC_GSEC_CLK_GET_SECURE_CLOCK();
#elif AGPS_LINUX_FTR
    Curr_SecureClock = MC_GSEC_CLK_GET_SECURE_CLOCK();
#endif  /* __RTK_E__ */

    /* Check the RTC Calibration Data checksum to see if it the data is OK.*/
    checksum = (U4)( (U4)0x55555555              +
                     (U4)gn_RTC_Calib.CTime_Set  +
                     (U4)gn_RTC_Calib.SecureClock   +
                     (U4)gn_RTC_Calib.OSTime  +
                     (U4)gn_RTC_Calib.Acc_Est_Set );

    /* If the checksum is NOT correct then Time cannot be trusted, so QUIT now
    *  without a valid starting UTC Time.
    */
    if ( checksum != gn_RTC_Calib.checksum  )
    {
        /* WARNING: RTC Calibration Checksum fail*/
        return( FALSE );
    }

    /* Compute the age since the RTC was last calibrated.*/
    age = (I4)( Curr_SecureClock - gn_RTC_Calib.SecureClock );             /* seconds*/


    /* Compute the expected degradation in the accuracy of this RTC time,
    *  allowing for degradations due to.
    *  a) 150ms RMS for the +/- 0.5 sec error in reading the RTC.
    *  b1) 50 ppm RMS for the UTS timer if it's not the fisrt calibration
    *  b2) 20 ppm RMS for the RTC timer if it's the first calibration
    *  Note that 'age' is in seconds, but RTC_acc and acc are ms.
    */
    if (g_fisrtCalibration)
    {
        acc_est_deg =  150  +  age / (1000000/(20*1000));
    }
    else
    {
        acc_est_deg =  150  +  age / (1000000/(50*1000));
    }

    /* Compute the Estimated time Accuracy now [RMS ms] , using the Gaussian
    *  Propagation of Error Law to combine the estimated accuracy when set
    *  and the estimated degradation since then.
    */
    {
        R4 tempR4 =  (R4)gn_RTC_Calib.Acc_Est_Set * (R4)gn_RTC_Calib.Acc_Est_Set
                     +  (R4)acc_est_deg * (R4)acc_est_deg;
        tempR4 = tempR4/2;
        acc_est_now  =  (U4)sqrt( (R8)tempR4 );
    }
    p_UTC->Acc_Est  =  acc_est_now;

    /* Limit the accuracy to 300 ms RMS (i.e. a worst-case error of about 1 sec).
    *  We do this because the resolution of a Typical OS appears to be 1 seconds.
    */
    if ( p_UTC->Acc_Est < 300 )  p_UTC->Acc_Est = 300;                /* [ms]*/

    /* delta time in second*/
    vl_secureClockDelta= Curr_SecureClock-gn_RTC_Calib.SecureClock;

    /*set the current time in second*/
    Curr_CTime = gn_RTC_Calib.CTime_Set + vl_secureClockDelta;

    if (!g_fisrtCalibration)
    {
        /* delta time in ms*/
        vl_osClockDelta = p_UTC->OS_Time_ms - gn_RTC_Calib.OSTime;

        /* if the last second given by secure clock is not terminated perhaps 0.8 or other number upper than 0.5 */
        if ((vl_secureClockDelta * 1000)>vl_osClockDelta)
        {
            /* we must minus one second */
            Curr_CTime --;
            vl_secureClockDelta --;
        }

        /* Apply the millisecond part of the calibration offset
        */
        vl_offsetMs = vl_osClockDelta - (vl_secureClockDelta * 1000);

        /* If the Milliseconds have gone over a second boundary, then move the integer
        *   seconds part of it into the Current 'C' Time.
        */
      if (vl_offsetMs >0)
      {
        if ( vl_offsetMs >= 1000 )
        {
            dT_s                 = vl_offsetMs / 1000;             /* [sec]*/
            p_UTC->Milliseconds  = vl_offsetMs  - dT_s*1000;       /* [ms] */
            Curr_CTime           = Curr_CTime + dT_s;              /* {sec] */
        }
        else
        {
            p_UTC->Milliseconds  = vl_offsetMs;
        }
    }
    else
    {
         MC_HALGPS_TRACE_INF("Negative vl_offsetMs: %d \n",vl_offsetMs);
         p_UTC->Milliseconds =0;

        }

    }
    else
    {
        /* don't have a millisecond resolution, therefore,
         *  Set the Milliseconds to 500 so that the error is -499 to +499 ms.
         */
        p_UTC->Milliseconds = 500;
    }

    /* Convert the Current 'C' Time back to a YY-MM-DD hh:mm:ss date & time format.*/
    GN_CTime_To_YMDHMS( Curr_CTime,
                        &p_UTC->Year,  &p_UTC->Month,   &p_UTC->Day,
                        &p_UTC->Hours, &p_UTC->Minutes, &p_UTC->Seconds );
    MC_HALGPS_TRACE_INF("UTC Year: %d  UTC Month : %d  UTC Day: %d  UTC Hours: %d\n",p_UTC->Year,p_UTC->Month,p_UTC->Day,p_UTC->Hours);

    return( TRUE );
#endif /* HALGPS_DONOT_USE_RTC */
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6
/**
* \fn void GN_GPS_Write_UTC(s_GN_GPS_UTC_Data *p_UTC )
* \brief CPS library Callback to write UTC.
*
* GPS library Callback Function to Write UTC Date & Time data , which can be used
* by the Host to update its Real-Time Clock.
*
* \param p_UTC Pointer to the UTC Date & Time
*/
void GN_GPS_Write_UTC(s_GN_GPS_UTC_Data *p_UTC )
{
#ifdef HALGPS_DONOT_USE_RTC
    p_UTC = p_UTC;
    return;
#else
    static U4 counter = 0;

    /* Compute the difference between the time from the RTC and that
    *  provided by the GPS.  This difference will be saved as a Calibration
    *  Offset to a structure (checksumed), which is then written to a data file
    *  on the local disk when we next save the NV Store data.
    *  On the next start-up this calibration data will be read back and applied
    *  to the Time read back from the RTC.
    *
    *  When the GPS is generating position fixes, this function will be called
    *  once per update.
    *
    *  It may also be necessary to actually set the RTC if the Calibration Offset
    *  is too big in order to prevent other applications on the Host system
    *  from doing so and hence causing our calibration offset to be wrong.
    */

    U4             RTC_CTime;           /* Real-Time Clock Time, in 'C' Time units [seconds]*/
    /* +LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
    /* I4             dT_s;                 A Time difference [seconds]*/
    /* -LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
    U4             New_CTime;           /* New UTC Time, in 'C' Time units [seconds]*/


    /* If the new time is worse than the currently saved then consider ignoring it.
    *  The worse it is the longer it is ignored in the hope something better comes along.
    */

    /*if(gn_RTC_Calib.CTime_Set != 0)
    {
        if ( p_UTC->Acc_Est >= gn_RTC_Calib.Acc_Est_Set )
        {
       counter++;
       if ( p_UTC->Acc_Est > 1000  &&  counter < ( 20 * 60 ) )  return;
       if ( p_UTC->Acc_Est > 300   &&  counter < ( 10 * 60 ) )  return;
       if ( p_UTC->Acc_Est > 100   &&  counter < (  5 * 60 ) )  return;
       if ( p_UTC->Acc_Est > 30    &&  counter < (  3 * 60 ) )  return;
       if ( p_UTC->Acc_Est > 10    &&  counter < (  2 * 60 ) )  return;
       if (                            counter < (  1 * 60 ) )  return;
        }
    }*/
    counter++;

    /* Check for the case of being given time of all zero's with a very large */
    /* Accuracy Estimate and treat this as a request to Clear Time,  eg if */
    /* a Cold Start Re-Set was detected by GN_GPS_Parse_PGNV(). */
    /* Here this is achieved simply by deleting the UTC Calibration file. */
    if ( p_UTC->Acc_Est > 0x7FFF0000  &&
            p_UTC->Year  == 0  &&  p_UTC->Month   == 0  &&  p_UTC->Day     == 0  &&
            p_UTC->Hours == 0  &&  p_UTC->Minutes == 0  &&  p_UTC->Seconds == 0  && (vg_HALGPS_CalibrationFullPathFileName != NULL)  )
    {
        t_gfl_FILE * pl_File=NULL;

        s_RTC_Calib *pl_RTC_Calib=NULL;

        /*Erase the calibration file*/
        pl_File=(t_gfl_FILE *)MC_GFL_FOPEN((char*)vg_HALGPS_CalibrationFullPathFileName,(const uint8_t*)"wb+");
        if (!pl_File)
        {
            /*generate an error*/
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }
        else
        {
            MC_GFL_FCLOSE(pl_File);
        }
        memset( &gn_RTC_Calib, 0, sizeof(*pl_RTC_Calib) );   /* Clear the RTC Calib data*/
        return;
    }

    /* Only Calibrate the RTC if we have been given time to better than
    *  300ms RMS, ie < 1 sec MAX,  ie it is good enough to be reliable.
    */
    if ( p_UTC->Acc_Est < 300 )
    {
        /* Convert the New UTC Time to 'C' Time units [seconds]*/
        New_CTime = GN_YMDHMS_To_CTime( p_UTC->Year,
                                        p_UTC->Month,
                                        p_UTC->Day,
                                        p_UTC->Hours,
                                        p_UTC->Minutes,
                                        p_UTC->Seconds );


        /* Get the current RTC Time
        *  (This RTC time must always be UTC (ie no time-zone adjustments etc).
        *  Also get the equivalent OS Time.
        */
#ifdef __RTK_E__
        RTC_CTime = MC_GSEC_CLK_GET_SECURE_CLOCK();
#elif AGPS_LINUX_FTR
        RTC_CTime = MC_GSEC_CLK_GET_SECURE_CLOCK();
#endif

        /* +LMSqc05668 : LMSqc05894 Remove HALGPS warnings */
        /* Compute the difference between the Current RTC Time the New UTC Time*/
        /* dT_s = (I4)( RTC_CTime - New_CTime );         [seconds]*/
        /* -LMSqc05668 : LMSqc05894 Remove HALGPS warnings */

        /* Save the RTC Calibration data to the global structure*/

        /* a) When this Calibration was done*/
        gn_RTC_Calib.CTime_Set = New_CTime;

        /* b) The bigger integer second part of the Calibration Offset*/
        gn_RTC_Calib.SecureClock  = RTC_CTime;

        /* c) The smaller millisecond part of the Calibration Offset based on:
        *    1) The +/- 0.5sec rounding in reading the RTC
        *    2) Milliseconds bit of the input UTC Time which was not used
        *    3) The latency between when the UTC Time was computed for and when
        *       we read-back the RTC to compute the calibration.
        */
        gn_RTC_Calib.OSTime = GN_GPS_Get_OS_Time_ms();

        /* d) The Estimated accuracy of the RTC Calibration.  Assume that we can:
        *    1) Keep the GN_GPS_Update loop latency to within 100ms, say 30ms RMS,
        *    2) Calibration is good to +/- 0.5 second,  ie 170ms RMS
        */
        if (g_fisrtCalibration)
        {
            gn_RTC_Calib.Acc_Est_Set =  p_UTC->Acc_Est + 30 + 150;
            g_fisrtCalibration = FALSE;
        }
        else
        {
#ifdef UTS_CPR
            /* the OS time can given a precision of 1 ms */
            gn_RTC_Calib.Acc_Est_Set =  p_UTC->Acc_Est + 30;
#else
            /* the OS time can given a precision of 5 ms (tick precision) */
            gn_RTC_Calib.Acc_Est_Set =  p_UTC->Acc_Est + 30 + 2;
#endif
        }

        /* e) Compute the new checksum for the Calibration data structure*/
        gn_RTC_Calib.checksum =  (U4)( (U4)0x55555555               +
                                       (U4)gn_RTC_Calib.CTime_Set   +
                                       (U4)gn_RTC_Calib.SecureClock +
                                       (U4)gn_RTC_Calib.OSTime      +
                                       (U4)gn_RTC_Calib.Acc_Est_Set  );
    }

    return;
#endif /*HALGPS_DONOT_USE_RTC*/

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7
/**
* \fn U2 GN_GPS_Read_GNB_Meas(U2  max_bytes,CH *p_GNB_Meas )
* \brief CPS library Callback to read UART.
*
* GPS library Callback Function to Read GPS Measurement Data from the Host's
* chosen GPS Baseband communications interface.
* Internally the GPS Core library uses a circular buffer to store this
* data.  Therefore, 'max_bytes' is dynamically set to prevent a single Read
* operation from straddling the internal circular buffer's end wrap point, or
* from over writing data that has not been processed yet.
*
* \param max_bytes Maximum number of bytes to read
* \param p_GNB_Meas Pointer to the Measurement data.
*
*\return Returns the number of bytes actually read.  If this is equal to 'max_bytes'
* then this callback function may be called again if 'max_bytes' was limited
* due to the circular buffer's end wrap point.
*/
U2 GN_GPS_Read_GNB_Meas(U2 max_bytes,CH *p_GNB_Meas )
{
    U2 bytes_read = 0;                /* Number of bytes actually read*/

#ifdef GPS_OVER_SPI
    bytes_read=HALGPS6_01SpiRcv((uint8_t*)p_GNB_Meas,max_bytes);
#elif GPS_OVER_UART
    bytes_read=HALGPS2_01UartRcv((uint8_t*)p_GNB_Meas,max_bytes);
#elif GPS_OVER_XOSGPS
    bytes_read=HALGPS8_02XosgpsRcv((uint8_t*)p_GNB_Meas,max_bytes);
#elif GPS_OVER_HCI
    bytes_read=HALGPS9_07HciRcv((uint8_t*)p_GNB_Meas,max_bytes);

#endif

/*    MC_HALGPS_TRACE_INF("GN_GPS_Read_GNB_Meas: Read %d of %d bytes", bytes_read, max_bytes);*/
    return( (U2)bytes_read );
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8
/**
* \fn U2 GN_GPS_Write_GNB_Ctrl(U2  num_bytes,CH *p_GNB_Ctrl )
* \brief CPS library Callback to write data on UART.
*
* GPS library Callback Function to Write GPS Control Data to the Host's chosen
* GPS Baseband communications interface.
* Internally the GPS Core library uses a circular buffer to store this
* data.  Therefore, this callback function may be called twice if the data to
* be written straddles the internal circular buffer's end wrap point.
*
* \param num_bytes Available number of bytes to Write
* \param p_GNB_Ctrl Pointer to the Ctrl data..
*
* \return Returns the number of bytes actually written.  If this is less than the
* number of bytes requested to be written, then it is assumed that this is
* because the host side cannot physically handle any more data at this time.
*/
U2 GN_GPS_Write_GNB_Ctrl(U2  num_bytes,CH *p_GNB_Ctrl )
{
    U2 bytes_written = 0;             /* Number of bytes written*/

#ifdef GPS_OVER_SPI
    bytes_written=HALGPS6_02SpiSend((uint8_t*)p_GNB_Ctrl,num_bytes);
#elif GPS_OVER_UART
    bytes_written=HALGPS2_02UartSend((uint8_t*)p_GNB_Ctrl,num_bytes);
#elif GPS_OVER_XOSGPS
    bytes_written=HALGPS8_02XosgpsSend((uint8_t*)p_GNB_Ctrl,num_bytes);
#elif GPS_OVER_HCI
    bytes_written=HALGPS9_06HciSend((uint8_t*)p_GNB_Ctrl,num_bytes);

#endif
    MC_HALGPS_TRACE_INF("GN_GPS_Write_GNB_Ctrl: written %d of %d bytes", bytes_written, num_bytes);

    return( (U2)bytes_written );
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9
uint16_t halgps1_09TraceLibLogs(uint16_t  vp_NumBytes, int8_t *pp_TraceBuff, e_lbsosatrace_ModuleType vp_TraceType  )
{
    uint16_t  vl_NumWrite;
    uint16_t  vl_Index = 0;
    uint16_t  vl_PreviousIndex = 0;

    /*
     * Call underneath tracing utility line by line and remove the new line charecters as it is handled in LBS OSA TRACE
     * Scenarios are handled in here :
     * data1\r\ndata2    =>    data1
     * data1\ndata2       =>   data1
     * data1\rdata2       =>    data1
     */
    for( vl_Index = 0; vl_Index < vp_NumBytes ; vl_Index++ )
    {
        if( *(pp_TraceBuff + vl_Index) == '\r' || *(pp_TraceBuff + vl_Index) == '\n')
        {
            int8_t vl_TempChar = *(pp_TraceBuff + vl_Index);
            *(pp_TraceBuff + vl_Index) = '\0';

            /* Check if it is first element in new line. If it is a new line send a space instead of a null string to trace */
            if( vl_Index != vl_PreviousIndex )
            {
                LBSOSATRACE_PRINT(vp_TraceType, K_LBSOSATRACE_LOG_LEV_INF, (char*)(pp_TraceBuff + vl_PreviousIndex));
            }
            else
            {
                LBSOSATRACE_PRINT(vp_TraceType, K_LBSOSATRACE_LOG_LEV_INF, " ");
            }

            *(pp_TraceBuff + vl_Index) = vl_TempChar;

            if(  vl_TempChar == '\r' && *(pp_TraceBuff + vl_Index + 1) == '\n' )
            {
                vl_Index++;
            }

            vl_PreviousIndex    = vl_Index + 1;
        }
    }

    vl_NumWrite = vp_NumBytes;
    return( vl_NumWrite );
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10
/**
* \fn U2 GN_GPS_Write_NMEA(U2  num_bytes,CH *p_NMEA )
* \brief CPS library Callback to sent NMEA
*
* GPS library Callback Function to sent GPS NMEA 183 Output Sentences.
* Internally the GN GPS Core library uses a circular buffer to store this
* data.  Therefore, this callback function may be called twice if the data to
* be written straddles the internal circular buffer's end wrap point.
*
* \param num_bytes Available number of bytes to Write
* \param p_NMEA Pointer to the NMEA data.
*
* \return Returns the number of bytes actually written.If this is less than the
* number of bytes requested to be written, then it is assumed that this is
* because the host side cannot physically handle any more data at this time.
*/

U2 GN_GPS_Write_NMEA(U2  num_bytes,CH *p_NMEA )
{
    return(halgps1_09TraceLibLogs( num_bytes,  (int8_t*)p_NMEA, K_LBSOSATRACE_CHIP_LOG2));
}




#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11
/**
* \fn U2 GN_GPS_Write_GNB_Debug(U2  num_bytes,CH *p_GNB_Debug )
* \brief CPS library Callback to write debug log file
*
* Debug Callback Functions called by the GN GPS High Level Software library
* that need to be implemented by the Host platform software to capture debug
* data to an appropriate interface (eg UART, File, both etc).
* GN GPS Callback Function to Write GPS Baseband I/O communications Debug data
* to the the Host's chosen debug interface.
* Internally the GPS Core library uses a circular buffer to store this
* data.  Therefore, this callback function may be called twice if the data to
* be written straddles the internal circular buffer's end wrap point.
*
* \param num_bytes Available number of bytes to Write
* \param p_GNB_Debug  Pointer to the GNB Debug data..
*
* \return Returns the number of bytes actually written.  If this is less than the
* number of bytes requested to be written, then it is assumed that this is
* because the host side cannot physically handle any more data at this time.
*/
U2 GN_GPS_Write_GNB_Debug(U2  num_bytes,CH *p_GNB_Debug )
{
    return(halgps1_09TraceLibLogs( num_bytes,  (int8_t*)p_GNB_Debug, K_LBSOSATRACE_CHIP_LOG1));
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 12
/**
* \fn U2 GN_GPS_Write_Nav_Debug(U2  num_bytes, CH *p_Nav_Debug )
* \brief CPS library Callback to write navigation debug informations
*
* GPS library Callback Function to Write GPS Navigation Solution Debug data
* Internally the GPS Core library uses a circular buffer to store this
* data.  Therefore, this callback function may be called twice if the data to
* be written straddles the internal circular buffer's end wrap point.
*
* \param num_bytes Available number of bytes to Write
* \param p_Nav_Debug  Pointer to the Nav Debug data.
*
* \return Returns the number of bytes actually written.  If this is less than the
* number of bytes requested to be written, then it is assumed that this is
* because the host side cannot physically handle any more data at this time.
*/
U2 GN_GPS_Write_Nav_Debug(U2  num_bytes, CH *p_Nav_Debug )
{
    return(halgps1_09TraceLibLogs( num_bytes,  (int8_t*)p_Nav_Debug, K_LBSOSATRACE_CHIP_LOG3));
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13
/**
* \fn U2 GN_GPS_Write_Event_Log(U2  num_bytes,CH *p_Event_Log )
* \brief CPS library Callback to write navigation event informations
*
* GPS library Callback Function to Write GPS Navigation Library Event Log data
* Internally the GN GPS Core library uses a circular buffer to store this
* data.  Therefore, this callback function may be called twice if the data to
* be written straddles the internal circular buffer's end wrap point.
*
* \param num_bytes Available number of bytes to Write
* \param p_Event_Log  Pointer to the Event Log data.
*
* \return Returns the number of bytes actually written.  If this is less than the
* number of bytes requested to be written, then it is assumed that this is
* because the host side cannot physically handle any more data at this time.
*/
U2 GN_GPS_Write_Event_Log(U2  num_bytes,CH *p_Event_Log )
{
    return(halgps1_09TraceLibLogs( num_bytes,  (int8_t*)p_Event_Log, K_LBSOSATRACE_CHIP_LOG4));
}




#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 14
/**
* \fn U2 GN_SUPL_Write_Event_Log(U2  num_bytes,CH *p_Event_Log )
* \return Returns the number of bytes actually written.  If this is less than the
* number of bytes requested to be written, then it is assumed that this is
* because the host side cannot physically handle any more data at this time.
*/
U2 GN_SUPL_Write_Event_Log(U2  num_bytes,CH *p_Event_Log )
{
    return(halgps1_09TraceLibLogs( num_bytes,  (int8_t*)p_Event_Log, K_LBSOSATRACE_SUPL));
}

#ifdef CMCC_LOGGING_ENABLE

//*****************************************************************************
/// \brief
///      GN SUPL API Function to request the Host software to Write the SUPL Library's
///      Event Log data to the CMCC's chosen debug interface.
/// \details
///      GN SUPL API Function to request the Host software to Write the SUPL Library's
///      Event Log data to the CMCC's chosen debug interface. The current implementation
///      is ad-hoc. In future, a more generic implementation should be done and the host
///      software should manage any CMCC specific logging
/// \note
///      This function must be implemented by the Host software and will be
///      called by functions from within the SUPL Library.
/// \returns
///      void
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 15

void GN_SUPL_Write_Event_Log_CMCC(
   U4    code,        ///< [in] CMCC defined code to be output
   char* textLog,    ///< [in] Pointer to where the Host software can get the Log data from.
   char* notes        ///< [in] Pointer to where the CMCC specific Host software can get the notes from.
)
{
    char strCmccLog[K_HALGPS_LOG_MAX_CMCC_LOG_LEN];
    s_GN_GPS_UTC_Data p_UTC = {0};
    bool bUtcReady = 1;
    uint32_t  length;

#if AGPS_LINUX_FTR
    struct timeval current = {0};
    U32 seconds = 0;

    if( 0 == gettimeofday( &current , NULL ) )
    {
       seconds = current.tv_sec;

       GN_CTime_To_YMDHMS((U4)seconds,&p_UTC.Year,&p_UTC.Month,&p_UTC.Day,&p_UTC.Hours,&p_UTC.Minutes,
                             &p_UTC.Seconds);
    }
    else
    {
     bUtcReady = 0;
    }
#else
    bUtcReady = GN_GPS_Read_UTC(&p_UTC);
#endif

    if(bUtcReady)
    {
    if(notes)
    {
       length = _snprintf(strCmccLog, K_HALGPS_LOG_MAX_CMCC_LOG_LEN, "[%04d%02d%02d%02d%02d%02d.%02d] 0x%08X: %s # %s\n",
                       p_UTC.Year, p_UTC.Month, p_UTC.Day, p_UTC.Hours, p_UTC.Minutes,
                       p_UTC.Seconds, p_UTC.Milliseconds, (int)code, textLog, notes
                       );
    }
    else
    {
       length = _snprintf(strCmccLog,K_HALGPS_LOG_MAX_CMCC_LOG_LEN, "[%04d%02d%02d%02d%02d%02d.%02d] 0x%08X: %s\n",
                       p_UTC.Year, p_UTC.Month, p_UTC.Day, p_UTC.Hours, p_UTC.Minutes,
                       p_UTC.Seconds, p_UTC.Milliseconds, (int)code, textLog
                       );
    }
    }
    else
    {
        if(notes)
        {
           length = _snprintf(strCmccLog,K_HALGPS_LOG_MAX_CMCC_LOG_LEN, "[UTC not yet ready] 0x%08X: %s # %s\n", (int)code, textLog, notes);
        }
        else
        {
           length = _snprintf(strCmccLog,K_HALGPS_LOG_MAX_CMCC_LOG_LEN, "[UTC not yet ready] 0x%08X: %s\n", (int)code, textLog);
        }
    }

    LBSOSATRACE_PRINT(K_LBSOSATRACE_CMCC, K_LBSOSATRACE_LOG_LEV_INF, strCmccLog);
}

#endif

#undef HALGPS1LIB_C
