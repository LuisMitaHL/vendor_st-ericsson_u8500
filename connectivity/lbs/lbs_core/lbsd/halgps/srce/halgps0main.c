/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
* \file halgps0main.c
* \date 25/01/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B>  This file contains system entry function for RTK process and FSM table
* All message receive by the other module are receive in this file\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 25.01.08 </TD><TD> M.BELOU </TD><TD> Creation </TD>
*     </TR>
*     <TR>
*             <TD> 31.03.08 </TD><TD> M.BELOU </TD><TD> add TAT API </TD>
*     </TR>
* </TABLE>
*/

#define __HALGPS0MAIN_C__

#include "halgpsmain.h"
#ifdef WIN32
#pragma warning(disable : 4267)
#endif

#undef MODULE_NUMBER
#define MODULE_NUMBER MODULE_HALGPS

#undef PROCESS_NUMBER
#define PROCESS_NUMBER PROCESS_CGPS

#undef FILE_NUMBER
#define FILE_NUMBER 0


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
#ifdef AGPS_ANDR_WAKEUP_FTR
#include <hardware_legacy/power.h>
#define STE_GPS_WAKE_LOCK_NAME      "ste-gps-interface" //"efsd-interface" //EFSD_WAKE_LOCK_NAME
#endif
/*****************************************************************************/
/* HALGPS_timer_Expired : Timer function                                     */
/*****************************************************************************/
void HALGPS0_01Init(void)
{
    e_halgps_internal_state vl_state= HALGPS_NOT_INITIALIZED_INTERNAL_STATE;

#ifdef UTS_CPR
    t_UTSER_CounterConfig vl_CounterConfig;
#endif

    MC_HALGPS_TRACE_INF("BEGIN : HALGPS0_01Init");

    vg_HALGPS_ErrorNumber = 0;
#ifdef AGPS_LINUX_FTR
   gn_CLK_TCK = (U4)sysconf( _SC_CLK_TCK );
   MC_HALGPS_TRACE_INF( " gn_CLK_TCK: %u   (CLOCKS_PER_SEC: %u)", gn_CLK_TCK, (U4)CLOCKS_PER_SEC );

#endif

    /*create empty log file */
#ifdef __RTK_E__
    /*Register callback on gsecurity module for UTC corruption*/
    if (MC_GSEC_CLK_REG_CB_FOR_CORRUPTION(&HALGPS0_04UTCCorruptedCallback)!=GSEC_OK)
    {
        /*generate an error*/
        vg_HALGPS_ErrorNumber = MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_THIRD_ERROR);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
    }
#endif /*__RTK_E__*/

    /*Init Communication port : UART or SPI*/
#if defined GPS_OVER_SPI
    if (HALGPS6_00SpiInit()!=TRUE)
#elif GPS_OVER_UART
    if (HALGPS2_00UartInit()!=TRUE)
#elif GPS_OVER_XOSGPS
    if (HALGPS8_00XosgpsInit()!=TRUE)
#elif GPS_OVER_UART_LINUX
    if( 0 )
#elif GPS_OVER_HCI
    if( 0 )
#endif
    {
        vl_state = HALGPS_INITIALISATION_FAILED_INTERNAL_STATE;
    }

#ifdef UTS_CPR
    if (MC_UTSER_GET_COUNTER(&g_HALGPS_OSCounter,UTSER_1MSEC_PRECISION)!=UTSER_OK)
    {
        /* generate an error */
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FOURTH_ERROR);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        /* if the counter don't start not block the initialisation on this error*/
        /* the gps module can start but the fisrt fix wil  be more longer to get*/
    }
    vl_CounterConfig.v_initialCount = 0;
    if (MC_UTSER_SET_COUNTER(g_HALGPS_OSCounter,&vl_CounterConfig,UTSER_COUNTER_START)!=UTSER_OK)
    {
        /* generate an error */
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIFTH_ERROR);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        /* if the counter don't start not block the initialisation on this error*/
        /* the gps module can start but the fisrt fix wil  be more longer to get*/
    }
#endif

    g_fisrtCalibration = TRUE;

#if defined LBS_INTRPT_BB_READ
    vg_HALGPS_LantencyTimer = 0;
    vg_HALGPS_ResetLantencyTimer = 0;
#endif

    if (vl_state == HALGPS_NOT_INITIALIZED_INTERNAL_STATE)
    {
        vg_HALGPS_State = HALGPS_INITIALIZED_INTERNAL_STATE;
        MC_HALGPS_TRACE_INF("HALGPS Init OK");
    }
    else
    {
        vg_HALGPS_State = vl_state;
    }

    MC_HALGPS_TRACE_INF("END : HALGPS0_01Init vg_HALGPS_State=%d", vg_HALGPS_State);

}


#ifdef __RTK_E__


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/******************************************************************************/
/* HALGPS_0DefaultMessage : Call back used by gsecurity module if the UTC     */
/* is corrupted by another application                                        */
/******************************************************************************/
void HALGPS0_04UTCCorruptedCallback(void)
{
    MC_HALGPS_TRACE_INF("BEGIN : HALGPS0_04UTCCorruptedCallback");

    MC_CGPS_UTC_CORRUPTED();

    MC_HALGPS_TRACE_INF("END : HALGPS0_04UTCCorruptedCallback");
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/*****************************************************************************/
/* HALGPS0_05UTCCorrupted : message receive to inform that the UTC was corrupted */
/* This message is sent on callback. The same process that execute the calibration */
/* file update, must execute this function to delete the calibration file */
/*****************************************************************************/
void HALGPS0_05UTCCorrupted()
{
    t_gfl_FILE * pl_File;

    MC_HALGPS_TRACE_INF("BEGIN : HALGPS0_05UTCCorrupted");

    /*Erase the calibration file*/
    pl_File=(t_gfl_FILE *)MC_GFL_FOPEN(HALGPS_UTC_CALIBRATION_FILE_NAME,(const uint8_t*)"wb+");
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

    MC_HALGPS_TRACE_INF("END : HALGPS0_05UTCCorrupted");
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6
/*****************************************************************************/
/* HALGPS0_06Start : start GPS module                                        */
/*****************************************************************************/
e_halgps_state HALGPS0_06Start(void)
{
    e_halgps_state vl_Return;
    /*s_GN_GPS_Config vl_GPS_Config;*/

/* +LMSqb88934 */
#ifdef GPS_OVER_UART
    if (HALGPS2_00UartInit()!=TRUE)
    {
        vl_Return = HALGPS_INITIALIZED_FAILED;
        return vl_Return;
    }
#endif
/* -LMSqb88934 */

    MC_HALGPS_TRACE_INF("BEGIN : HALGPS0_06Start vg_HALGPS_State=%d", vg_HALGPS_State);

    switch (vg_HALGPS_State)
    {
    case HALGPS_INITIALIZED_INTERNAL_STATE:
    {
        if ((vg_HALGPS_CalibrationFullPathFileName != NULL) && (vg_HALGPS_StoreFullPathFileName != NULL))
        {
            /* Deactivate sleep mode */
            MC_HALGPS_SLEEP(FALSE);
#if defined GPS_OVER_SPI || defined GPS_OVER_UART
            /* turn on the GPS module */
            /*toggle the power GPIO*/
            if (kid3_04Ioctl( GPIO, MC_GPIO_BANK_NUMBER(HALGPS_GPIO_POWER), GPIO_IOCTL_SET_OUTPUT_ONE, (uint32_t *)GPIO_MAP_PIN_TO_MASK(MC_GPIO_PIN_NUMBER(HALGPS_GPIO_POWER)) ) != KIDERR_OK)
            {
                /*generate an error*/
                vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
                MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
                vl_Return = HALGPS_GPIO_FAILED;
                MC_HALGPS_TRACE_INF("HALGPS_GPIO_POWER failed");
                /* Re-activate sleep mode */
                MC_HALGPS_SLEEP(TRUE);
            }
            else
            {
#ifdef UTS_CPR
                MC_UTSER_SLEEP(UTSER_1MSEC_PRECISION,50);
#else
                MC_OST_START_TASK_TIMER(50);
#endif
                /* The GPIO control the reset pin of GPS module */
                if (kid3_04Ioctl( GPIO, MC_GPIO_BANK_NUMBER(HALGPS_GPIO_RESET), GPIO_IOCTL_SET_OUTPUT_ONE, (uint32_t *)GPIO_MAP_PIN_TO_MASK(MC_GPIO_PIN_NUMBER(HALGPS_GPIO_RESET)) )!= KIDERR_OK)
                {
                    /*generate an error*/
                    vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
                    MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
                    vl_Return = HALGPS_GPIO_FAILED;
                    MC_HALGPS_TRACE_INF("HALGPS_GPIO_RESET failed");
                    /* Re-activate sleep mode */
                    MC_HALGPS_SLEEP(TRUE);
                }
                else
                {
                    vg_HALGPS_State = HALGPS_RUNNING_INTERNAL_STATE;

                    vl_Return = HALGPS_OK;
                    MC_HALGPS_TRACE_INF("HALGPS started");
                }
            }
#elif GPS_OVER_XOSGPS
            if ( HALGPS8_03XosgpsCtrl(HALGPS_XOSGPS_POWER_ON) == FALSE )
            {
                /*generate an error*/
                vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
                MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
                vl_Return = HALGPS_GPIO_FAILED;
                MC_HALGPS_TRACE_INF("HALGPS_XOSGPS_POWER failed");
                /* Re-activate sleep mode */
                MC_HALGPS_SLEEP(TRUE);
            }
            else
            {
#ifdef UTS_CPR
                MC_UTSER_SLEEP(UTSER_1MSEC_PRECISION,50);
#else
                MC_OST_START_TASK_TIMER(50);
#endif
                if ( HALGPS8_03XosgpsCtrl(HALGPS_XOSGPS_RESET_ON) == FALSE )
                {
                    /*generate an error*/
                    vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
                    MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
                    vl_Return = HALGPS_GPIO_FAILED;
                    MC_HALGPS_TRACE_INF("HALGPS_XOSGPS_RESET failed");
                    /* Re-activate sleep mode */
                    MC_HALGPS_SLEEP(TRUE);
                }
                else
                {
                    vg_HALGPS_State = HALGPS_RUNNING_INTERNAL_STATE;

                    vl_Return = HALGPS_OK;
                    MC_HALGPS_TRACE_INF("HALGPS started");
                }
            }
#endif
        }
        else
        {
            vl_Return = HALGPS_INITIALIZED_FAILED;

            MC_HALGPS_TRACE_INF("HALGPS calibration file not ready");
        }
        break;
    }
    case HALGPS_RUNNING_INTERNAL_STATE:
    {
        vl_Return = HALGPS_ALREADY_RUN;

        MC_HALGPS_TRACE_INF("HALGPS already started");
        break;
    }
    case HALGPS_INITIALISATION_FAILED_INTERNAL_STATE:
    {
        vl_Return = HALGPS_INITIALIZED_FAILED;

        MC_HALGPS_TRACE_INF("HALGPS initialised failed");
        break;
    }
    case HALGPS_NOT_INITIALIZED_INTERNAL_STATE:
    default:
    {
        vl_Return = HALGPS_NOT_INITIALIZED;

        MC_HALGPS_TRACE_INF("HALGPS not initialised");
        break;
    }
    }

    MC_HALGPS_TRACE_INF("END : HALGPS0_06Start");
    return vl_Return;

}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7
/*****************************************************************************/
/* HALGPS0_07Stop : stop GPS module                                          */
/*****************************************************************************/
e_halgps_state HALGPS0_07Stop(void)
{
    e_halgps_state vl_Return;

   MC_HALGPS_TRACE_INF("BEGIN : HALGPS0_07Stop vg_HALGPS_State=%d", vg_HALGPS_State);

    switch (vg_HALGPS_State)
    {
    case HALGPS_RUNNING_INTERNAL_STATE:
    {

        MC_HALGPS_SLEEP(TRUE);

#if defined( GPS_OVER_SPI) || defined(GPS_OVER_UART)
        /* turn off the GPS module */
        /* The GPIO control the reset pin of GPS module */
        if (kid3_04Ioctl( GPIO, MC_GPIO_BANK_NUMBER(HALGPS_GPIO_RESET), GPIO_IOCTL_SET_OUTPUT_ZERO, (uint32_t *)GPIO_MAP_PIN_TO_MASK(MC_GPIO_PIN_NUMBER(HALGPS_GPIO_RESET)) ) != KIDERR_OK)
        {
            /*generate an error*/
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
            vl_Return = HALGPS_GPIO_FAILED;
        }
        else
        {
            /*set the power GPIO to zero*/
            if (kid3_04Ioctl( GPIO, MC_GPIO_BANK_NUMBER(HALGPS_GPIO_POWER), GPIO_IOCTL_SET_OUTPUT_ZERO, (uint32_t *)GPIO_MAP_PIN_TO_MASK(MC_GPIO_PIN_NUMBER(HALGPS_GPIO_POWER)) ) != KIDERR_OK)
            {
                /*generate an error*/
                vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
                MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
                vl_Return = HALGPS_GPIO_FAILED;
            }
            else
            {
                vg_HALGPS_State = HALGPS_INITIALIZED_INTERNAL_STATE;

/* +LMSqb88934 */                                //need to reset the uart
#ifdef GPS_OVER_UART
                    if (HALGPS2_14ResetUartDriver()!=TRUE)
                    {
                         vl_Return = HALGPS_INITIALIZED_FAILED;
                         return vl_Return;
                    }
#endif
/* -LMSqb88934 */

                vl_Return = HALGPS_OK;
            }
        }
#elif GPS_OVER_XOSGPS
        /* turn off the GPS module */
        /* The GPIO control the reset pin of GPS module */
        if ( HALGPS8_03XosgpsCtrl(HALGPS_XOSGPS_RESET_OFF) == FALSE )
        {
            /*generate an error*/
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
            vl_Return = HALGPS_GPIO_FAILED;
        }
        else
        {
            /*set the power GPIO to zero*/
            if ( HALGPS8_03XosgpsCtrl(HALGPS_XOSGPS_POWER_OFF) == FALSE )
            {
                /*generate an error*/
                vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
                MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
                vl_Return = HALGPS_GPIO_FAILED;
            }
            else
            {
                vg_HALGPS_State = HALGPS_INITIALIZED_INTERNAL_STATE;

                vl_Return = HALGPS_OK;
            }
        }
#endif
        MC_HALGPS_TRACE_INF("HALGPS stopped");

        break;
    }
    case HALGPS_INITIALIZED_INTERNAL_STATE:
    {
        vl_Return = HALGPS_ALREADY_STOP;

        MC_HALGPS_TRACE_INF("HALGPS already stopped");
        break;
    }
    case HALGPS_INITIALISATION_FAILED_INTERNAL_STATE:
    {
        vl_Return = HALGPS_INITIALIZED_FAILED;

        MC_HALGPS_TRACE_INF("HALGPS initialised failed");
        break;
    }
    case HALGPS_NOT_INITIALIZED_INTERNAL_STATE:
    default:
    {
        vl_Return = HALGPS_NOT_INITIALIZED;

        MC_HALGPS_TRACE_INF("HALGPS not initialised");
        break;
    }
    }

    MC_HALGPS_TRACE_INF("END : HALGPS0_07Stop");

    return vl_Return;
}
#endif   /* __RTK_E__ */


#ifdef AGPS_LINUX_FTR


#define GPS_GPIO_DRIVER "/dev/gps_hw"

#define GPS_POWER_ON      111
#define GPS_POWER_OFF     222
#define GPS_RESET_ON      333
#define GPS_RESET_OFF     444


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6
/*****************************************************************************/
/* HALGPS0_06Start : start GPS module                                        */
/*****************************************************************************/
e_halgps_state HALGPS0_06Start(void)
{
/* +LMSqc16476 */
#ifndef AGPS_PID0001_FTR
#ifdef GPS_OVER_HCI
        MC_HALGPS_TRACE_INF("HALGPS0_06 Start Entry inside GPS_OVER_HCI" );
    if ( FALSE == HALGPS9_00HciStart() )
    {
        MC_HALGPS_TRACE_INF( "HALGPS0_06: Unable to start GNSS over HCI" );
         return( HALGPS_NOT_INITIALIZED );
    }
#else /* #ifdef GPS_OVER_HCI */

/* -LMSqc16476 */
        /* TBD share this with gps_hw module */
     int gps_hw;

    MC_HALGPS_TRACE_INF("HALGPS0_06 Start : %s" , GPS_GPIO_DRIVER );

     gps_hw = open(GPS_GPIO_DRIVER , O_RDONLY /* O_WRONLY */ );
     if ( gps_hw == -1 )
     {
            MC_HALGPS_TRACE_INF( "HALGPS0_06: Unable to open gps_hw" );
             return( FALSE );
     }

     MC_HALGPS_TRACE_INF( "HALGPS0_06: gps_hw opened" );

     // Hold the RST_N Line Low for at least 0.5 sec.
     ioctl( gps_hw, GPS_RESET_OFF );
     MC_HALGPS_TRACE_INF( "HALGPS0_06: GPS_RESET_OFF Done" );
     usleep( (100*1000) );
     MC_HALGPS_TRACE_INF( "HALGPS0_06: usleep #1 Done" );
     ioctl( gps_hw, GPS_POWER_ON );
     MC_HALGPS_TRACE_INF( "HALGPS0_06: GPS_POWER_ON Done" );
     usleep( (500*1000) );
     MC_HALGPS_TRACE_INF( "HALGPS0_06: usleep #2 Done" );

     ioctl( gps_hw, GPS_RESET_ON );
     MC_HALGPS_TRACE_INF( "HALGPS0_06: GPS_RESET_ON Done" );
     usleep( (500*1000) );

     close( gps_hw );

    MC_HALGPS_SLEEP(FALSE);

/* +LMSqc16476 */
#endif /* #ifndef AGPS_PID0001_FTR */
/* -LMSqc16476 */

#ifdef GPS_OVER_UART
/* + LMSqc32518 */
    if( FALSE == HALGPS2_00UartInit() )
    {
         return( FALSE );
    }
/* - LMSqc32518 */
#endif /* defined( GPS_OVER_UART ) && defined( AGPS_LINUX_FTR ) */
#endif /* #ifdef GPS_OVER_HCI */
    MC_HALGPS_TRACE_INF("HALGPS0_06 Start Exit " );
    return HALGPS_OK;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7
/*****************************************************************************/
/* HALGPS0_07Stop : stop GPS module                                          */
/*****************************************************************************/
e_halgps_state HALGPS0_07Stop(void)
{
    MC_HALGPS_TRACE_INF("HALGPS0_07Stop Start");

#ifdef GPS_OVER_HCI
    {
        uint8_t vl_RetVal;

        vl_RetVal = HALGPS9_01HciStop();

        if (vl_RetVal == FALSE)
            MC_HALGPS_TRACE_INF ("ERROR - HALGPS0_07Stop Write unsucessfull: %d", vl_RetVal);
    }
#else
    {
        int gps_hw;

#ifdef GPS_OVER_UART
        HALGPS2_13ResetUart();
#endif

    /* +LMSqc16476 */
#ifndef AGPS_PID0001_FTR
    /* -LMSqc16476 */
        /// \todo Replace hard coded path to gps_hw driver.
        gps_hw = open( GPS_GPIO_DRIVER, O_RDONLY /* O_WRONLY */ );
        if ( gps_hw == -1 )
        {
           MC_HALGPS_TRACE_INF( "HALGPS0_07Stop: Unable to open gps_hw" );
           return( FALSE );
        }

        MC_HALGPS_TRACE_INF( "HALGPS0_07Stop: gps_hw opened" );


        // Hold the RST_N Line Low for at least 0.5 sec.
        ioctl( gps_hw, GPS_RESET_OFF );
        MC_HALGPS_TRACE_INF( "HALGPS0_07Stop: GPS_RESET_OFF Done" );
        usleep( (100*1000) );
        MC_HALGPS_TRACE_INF( "HALGPS0_07Stop: usleep #1 Done" );
        ioctl( gps_hw, GPS_POWER_OFF );
        MC_HALGPS_TRACE_INF( "HALGPS0_07Stop: GPS_POWER_OFF Done");
        usleep( (500*1000) );
        MC_HALGPS_TRACE_INF( "HALGPS0_07Stop: usleep #2 Done");
        ioctl( gps_hw, GPS_RESET_ON );
        MC_HALGPS_TRACE_INF( "HALGPS0_07Stop: GPS_RESET_ON Done");

        close( gps_hw );
    /* +LMSqc16476 */
#endif /* #ifndef AGPS_PID0001_FTR */
    /* -LMSqc16476 */

    }
#endif /* #ifdef GPS_OVER_HCI */

    LbsOsaTrace_CloseLogFiles();

    return HALGPS_OK;
}
#endif /* AGPS_LINUX_FTR */

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8
/*****************************************************************************/
/* HALGPS0_08Reset : Reset GPS module                                        */
/*****************************************************************************/
void HALGPS0_08Reset(void)
{
    MC_HALGPS_TRACE_INF("BEGIN : HALGPS0_08Reset");

    HALGPS0_07Stop();
#ifdef __RTK_E__

#ifdef UTS_CPR
    MC_UTSER_SLEEP(UTSER_1MSEC_PRECISION,500);
#else
    MC_OST_START_TASK_TIMER(500);
#endif

#endif   /* __RTK_E__ */

#if defined GPS_OVER_SPI
    HALGPS6_04ResetSpi();
#elif GPS_OVER_UART
#ifdef __RTK_E__
    HALGPS2_13ResetUartBuffer();
#elif AGPS_LINUX_FTR
    HALGPS2_13ResetUart();
#else
    #error Unknown Platform
#endif /* __RTK_E */
#endif /* GPS_OVER_SPI */


    HALGPS0_06Start();

    MC_HALGPS_TRACE_INF("END : HALGPS0_08Reset");
}


#ifdef __RTK_E__

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13
/*****************************************************************************/
/* HALGPS0_13TATGetInfo : Get information about GPS (hardware version....)   */
/*****************************************************************************/
bool HALGPS0_13TATGetInfo(t_HALGPSTatInfo * pp_Info)
{
    s_GN_GPS_Version vl_version;
    bool vl_Return = FALSE;

    MC_HALGPS_TRACE_INF("BEGIN : HALGPS0_13TATGetInfo");
    if (pp_Info != NULL)
    {
        /*get information from library */
        if (GN_GPS_Get_Version(&vl_version)==TRUE)
        {
            pp_Info->v_BBVersion = vl_version.BB_SW_Ver_ROM;
            pp_Info->v_PatchVersion = vl_version.BB_SW_Patch_Cs; /*vg_PatchVersion;*/
            pp_Info->v_EmbeddedMajorVersion = vl_version.MajorVersNo;
            pp_Info->v_EmbeddedMinorVersion = vl_version.MinorVersNo;
            vl_Return = TRUE;
        }
        else
        {
            /* generate an error */
            vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_FIRST_ERROR);
            MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
        }
    }
    else
    {
        /* generate an error */
        vg_HALGPS_ErrorNumber=MC_HALGPS_GENERATE_ERROR_NUMBER(HALGPS_SECOND_ERROR);
        MC_HALGPS_TRACE_ERROR(vg_HALGPS_ErrorNumber);
    }

    MC_HALGPS_TRACE_INF("END : HALGPS0_13TATGetInfo");
    return vl_Return;
}
#endif /* __RTK_E__ */

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 14
/*****************************************************************************/
/* HALGPS0_14Sleep : Sleep mode                                              */
/*****************************************************************************/
void HALGPS0_14Sleep(bool vp_SleepOnOff)
{
#if defined (GPS_OVER_SPI ) || defined (GPS_OVER_XOSGPS)
    uint8_t dummyBuffer[10];
#endif

    MC_HALGPS_TRACE_INF("BEGIN : HALGPS0_14Sleep %d", vp_SleepOnOff);

    if (vp_SleepOnOff == TRUE)
    {
/* +LMSqc13768 */
#if defined GPS_OVER_SPI
/* -LMSqc13768 */
        /* Do a dummy read to flush last data to send over. This is since SPI driver impl sends data over ONLY during a call to RECEIVE from GNS */
        HALGPS6_01SpiRcv(dummyBuffer, 10);
        HALGPS6_04ResetSpi();
/* +LMSqc13768 */
#elif GPS_OVER_UART
/* -LMSqc13768 */
#ifdef __RTK_E__
        HALGPS2_13ResetUartBuffer();
#endif /* __RTK_E__ */
#elif GPS_OVER_XOSGPS
        /* Do a dummy read to flush last data to send over */
        HALGPS8_02XosgpsRcv(dummyBuffer, 10);
#endif

#if defined( __RTK_E__ ) && defined( SLEEP_MODE )
        /*turn on the SLEEP mode*/
        /* Sleep mode is allowed */
#ifdef GPS_OVER_SPI
        MC_DPW_SLEEP_MODE_ALLOWED(HALGPS_DPW_SPI_SELECTOR);
#elif GPS_OVER_UART
        MC_DPW_SLEEP_MODE_ALLOWED(DPW_UART);
#endif /* GPS_OVER_SPI */
#endif /* defined( __RTK_E__ ) && defined( SLEEP_MODE ) */

#ifdef AGPS_ANDR_WAKEUP_FTR
        MC_HALGPS_TRACE_INF("HALGPS0_14Sleep : release_wake_lock");
    release_wake_lock(STE_GPS_WAKE_LOCK_NAME);
#endif
#ifdef GPS_OVER_HCI
        HALGPS9_12EnterDSM();
        HALGPS9_14HciRxBuffDeInit();
#endif
        LbsOsaTrace_CloseLogFiles();
    }
    else
    {
#if defined( __RTK_E__ ) && defined( SLEEP_MODE )
        /* Sleep mode is forbiden */
#ifdef GPS_OVER_SPI
        MC_DPW_SLEEP_MODE_FORBIDEN(HALGPS_DPW_SPI_SELECTOR);
#elif GPS_OVER_UART
        MC_DPW_SLEEP_MODE_FORBIDEN(DPW_UART);
#endif
#endif /* defined( __RTK_E__ ) && defined( SLEEP_MODE ) */
#ifdef AGPS_ANDR_WAKEUP_FTR
        MC_HALGPS_TRACE_INF("HALGPS0_14Sleep : acquire_wake_lock");
    acquire_wake_lock(PARTIAL_WAKE_LOCK, STE_GPS_WAKE_LOCK_NAME);
#endif
#ifdef GPS_OVER_HCI
        HALGPS9_03HciRxBuffInit();
        HALGPS9_13ExitDSM();
#endif
    }
    MC_HALGPS_TRACE_INF("END : HALGPS0_14Sleep");
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 15
/*****************************************************************************/
/* HALGPS0_15SetSystemPath : Get path from MEA                               */
/*****************************************************************************/
void HALGPS0_15SetSystemPath(uint8_t * pp_DirPath)
{
    uint8_t * pl_tmp= NULL;

    if (vg_HALGPS_CalibrationFullPathFileName == NULL)
    {
        pl_tmp = (uint8_t*)MC_RTK_GET_MEMORY( strlen((char*)pp_DirPath) + strlen((char*)HALGPS_UTC_CALIBRATION_FILE_NAME) + 1);
        strncpy((char*)pl_tmp,(char*)pp_DirPath,strlen((char*)HALGPS_UTC_CALIBRATION_FILE_NAME) + 1);
        strncat((char*)pl_tmp,(char*)HALGPS_UTC_CALIBRATION_FILE_NAME,strlen((char*)HALGPS_UTC_CALIBRATION_FILE_NAME) + 1 );
        vg_HALGPS_CalibrationFullPathFileName = pl_tmp;
    }

    if (vg_HALGPS_StoreFullPathFileName == NULL)
    {
        pl_tmp = (uint8_t*)MC_RTK_GET_MEMORY( strlen((char*)pp_DirPath) + strlen((char*)HALGPS_STORE_FILE_NAME) + 1);
        strncpy((char*)pl_tmp,(char*)pp_DirPath,strlen((char*)HALGPS_STORE_FILE_NAME) + 1);
        strncat((char*)pl_tmp,(char*)HALGPS_STORE_FILE_NAME,strlen((char*)HALGPS_STORE_FILE_NAME) + 1);
        vg_HALGPS_StoreFullPathFileName = pl_tmp;
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 16
/***********************************************************************************************/
/* HALGPS0_16FlushRxBuffer : Ignore data read from GPS baseband when GPS is in sleep/coma      */
/***********************************************************************************************/
void HALGPS0_16FlushRxBuffer()
{
#if defined (GPS_OVER_SPI)
    uint8_t dummyBuffer[256];

    HALGPS6_01SpiRcv(dummyBuffer, 256);
/* + LMSqc32518 */
#elif defined (GPS_OVER_UART)
    uint8_t vl_ReadBuffer[4096];

    HALGPS2_01UartRcv(&vl_ReadBuffer, sizeof(vl_ReadBuffer));
#elif defined (GPS_OVER_HCI)
    uint8_t *vl_ReadBuffer = MC_RTK_GET_MEMORY(HALGPS_READ_BUF_LEN);
    if(vl_ReadBuffer == NULL)
    {
        MC_HALGPS_TRACE_ERROR("Unable to allocate Memory to Read Buffer");
        return;
    }
    HALGPS9_07HciRcv(vl_ReadBuffer, HALGPS_READ_BUF_LEN);
    MC_RTK_FREE_MEMORY(vl_ReadBuffer);
#endif
/* - LMSqc32518 */
}


/* +LMSqc06482 : Production test */
#ifdef CGPS_CNO_VALUE_FTR
    uint8_t a_readBuffer[4096];
    #define PATCH_LINE_NB   46

    char patch[PATCH_LINE_NB][140] = {
        "#COMD 24 04201 65535 &08\x0D\x0A#COMD 23 33437 06276 &11\x0D\x0A#COMD 23 33438 59260 &13\x0D\x0A#COMD 23 33437 06276 &11\x0D\x0A",
        "#COMD 23 33438 59260 &13\x0D\x0A#COMD 25 31128 &E9\x0D\x0A#COMD 25 31128 &E9\x0D\x0A#COMD 23 33442 38768 &18\x0D\x0A",
        "#COMD 23 33443 38768 &19\x0D\x0A#COMD 23 33444 38768 &1A\x0D\x0A#COMD 23 33445 54456 &13\x0D\x0A#COMD 23 33446 05873 &13\x0D\x0A",
        "#COMD 23 33447 20593 &10\x0D\x0A#COMD 23 33448 54456 &16\x0D\x0A#COMD 23 33449 05858 &19\x0D\x0A#COMD 23 33450 54460 &0A\x0D\x0A",
        "#COMD 23 33451 05857 &11\x0D\x0A#COMD 23 33452 26464 &0F\x0D\x0A#COMD 23 33453 54460 &0D\x0D\x0A#COMD 23 33454 05855 &12\x0D\x0A",
        "#COMD 23 33455 17792 &16\x0D\x0A#COMD 23 33456 08448 &15\x0D\x0A#COMD 23 33457 54460 &11\x0D\x0A#COMD 23 33458 08025 &0E\x0D\x0A",
        "#COMD 23 33459 54456 &18\x0D\x0A#COMD 23 33460 08025 &07\x0D\x0A#COMD 23 33461 26576 &13\x0D\x0A#COMD 23 33462 54460 &0D\x0D\x0A",
        "#COMD 23 33463 08025 &0A\x0D\x0A#COMD 23 33464 52234 &0C\x0D\x0A#COMD 23 33465 20979 &18\x0D\x0A#COMD 23 33466 16832 &12\x0D\x0A",
        "#COMD 23 33467 10632 &0B\x0D\x0A#COMD 23 33468 20802 &0C\x0D\x0A#COMD 23 33469 24088 &17\x0D\x0A#COMD 23 33470 29952 &14\x0D\x0A",
        "#COMD 23 33471 16832 &0E\x0D\x0A#COMD 23 33472 22610 &06\x0D\x0A#COMD 23 33473 24154 &0C\x0D\x0A#COMD 23 33474 24088 &13\x0D\x0A",
        "#COMD 23 33475 29440 &11\x0D\x0A#COMD 23 33476 16832 &13\x0D\x0A#COMD 23 33477 22610 &0B\x0D\x0A#COMD 23 33478 54523 &14\x0D\x0A",
        "#COMD 23 33479 04213 &0C\x0D\x0A#COMD 23 33480 24185 &0E\x0D\x0A#COMD 23 33481 22619 &0F\x0D\x0A#COMD 23 33482 36792 &17\x0D\x0A",
        "#COMD 23 33483 52488 &18\x0D\x0A#COMD 23 33484 20547 &10\x0D\x0A#COMD 23 33485 36288 &1A\x0D\x0A#COMD 23 33486 65528 &1A\x0D\x0A",
        "#COMD 23 33487 20501 &09\x0D\x0A#COMD 23 33488 20544 &11\x0D\x0A#COMD 23 33489 26464 &19\x0D\x0A#COMD 23 33490 54460 &0E\x0D\x0A",
        "#COMD 23 33491 07987 &1B\x0D\x0A#COMD 23 33492 21984 &15\x0D\x0A#COMD 23 33493 23298 &16\x0D\x0A#COMD 23 33494 35322 &0E\x0D\x0A",
        "#COMD 23 33495 00192 &0C\x0D\x0A#COMD 23 33496 26465 &18\x0D\x0A#COMD 23 33497 17792 &1C\x0D\x0A#COMD 23 33498 08449 &1C\x0D\x0A",
        "#COMD 23 33499 16832 &18\x0D\x0A#COMD 23 33500 00634 &00\x0D\x0A#COMD 23 33501 16832 &08\x0D\x0A#COMD 23 33502 09967 &14\x0D\x0A",
        "#COMD 23 33503 04128 &05\x0D\x0A#COMD 23 33504 16768 &13\x0D\x0A#COMD 23 33505 10137 &04\x0D\x0A#COMD 23 33506 24067 &0C\x0D\x0A",
        "#COMD 23 33507 00011 &FC\x0D\x0A#COMD 23 33508 24068 &0F\x0D\x0A#COMD 23 33509 24618 &11\x0D\x0A#COMD 23 33510 24064 &04\x0D\x0A",
        "#COMD 23 33511 00064 &FF\x0D\x0A#COMD 23 33512 24071 &04\x0D\x0A#COMD 23 33513 32768 &11\x0D\x0A#COMD 23 33514 35044 &08\x0D\x0A",
        "#COMD 23 33515 04095 &0B\x0D\x0A#COMD 23 33516 22482 &0C\x0D\x0A#COMD 23 33517 23554 &0E\x0D\x0A#COMD 23 33518 33531 &0B\x0D\x0A",
        "#COMD 23 33519 26464 &13\x0D\x0A#COMD 23 33520 22567 &0B\x0D\x0A#COMD 23 33521 23582 &0A\x0D\x0A#COMD 23 33522 33526 &0A\x0D\x0A",
        "#COMD 23 33523 38025 &0A\x0D\x0A#COMD 23 33524 38025 &0B\x0D\x0A#COMD 23 33525 34785 &15\x0D\x0A#COMD 23 33526 00064 &05\x0D\x0A",
        "#COMD 23 33527 39804 &14\x0D\x0A#COMD 23 33528 07048 &10\x0D\x0A#COMD 23 33529 06984 &19\x0D\x0A#COMD 23 33530 34791 &0E\x0D\x0A",
        "#COMD 23 33531 00002 &F9\x0D\x0A#COMD 23 33532 34788 &16\x0D\x0A#COMD 23 33533 00032 &FE\x0D\x0A#COMD 23 33534 00147 &06\x0D\x0A",
        "#COMD 23 33535 22188 &10\x0D\x0A#COMD 23 33536 16768 &18\x0D\x0A#COMD 23 33537 10044 &06\x0D\x0A#COMD 23 33538 54456 &16\x0D\x0A",
        "#COMD 23 33539 05875 &18\x0D\x0A#COMD 23 33540 54520 &07\x0D\x0A#COMD 23 33541 05873 &0F\x0D\x0A#COMD 23 33542 20625 &08\x0D\x0A",
        "#COMD 23 33543 54456 &12\x0D\x0A#COMD 23 33544 05858 &15\x0D\x0A#COMD 23 33545 54460 &0F\x0D\x0A#COMD 23 33546 05861 &11\x0D\x0A",
        "#COMD 23 33547 54712 &11\x0D\x0A#COMD 23 33548 05860 &12\x0D\x0A#COMD 23 33549 36280 &13\x0D\x0A#COMD 23 33550 22466 &0C\x0D\x0A",
        "#COMD 23 33551 20544 &08\x0D\x0A#COMD 23 33552 24092 &0B\x0D\x0A#COMD 23 33553 00002 &FD\x0D\x0A#COMD 23 33554 26592 &14\x0D\x0A",
        "#COMD 23 33555 22498 &16\x0D\x0A#COMD 23 33556 16832 &12\x0D\x0A#COMD 23 33557 23773 &15\x0D\x0A#COMD 23 33558 00000 &00\x0D\x0A",
        "#COMD 23 33434 58121 &0A\x0D\x0A#COMD 23 32769 31552 &13\x0D\x0A#COMD 23 32768 16768 &1E\x0D\x0A#COMD 23 33439 33558 &16\x0D\x0A",
        "#COMD 23 33439 33558 &16\x0D\x0A#COMD 23 38772 33440 &11\x0D\x0A#COMD 23 38773 33558 &1C\x0D\x0A#COMD 23 38774 33441 &14\x0D\x0A",
        "#COMD 23 38775 33558 &1E\x0D\x0A#COMD 23 38776 33329 &1B\x0D\x0A#COMD 23 38777 33445 &1B\x0D\x0A#COMD 23 38778 33328 &1C\x0D\x0A",
        "#COMD 23 38779 16768 &26\x0D\x0A#COMD 23 38780 33204 &0E\x0D\x0A#COMD 23 38781 33456 &18\x0D\x0A#COMD 23 38782 32770 &17\x0D\x0A",
        "#COMD 23 38783 33538 &1B\x0D\x0A#COMD 23 38784 32771 &1A\x0D\x0A#COMD 23 38785 33498 &22\x0D\x0A#COMD 23 38770 38785 &20\x0D\x0A",
        "#COMD 23 38786 19200 &14\x0D\x0A#COMD 23 38787 10190 &14\x0D\x0A#COMD 23 38788 19201 &17\x0D\x0A#COMD 23 38789 10133 &13\x0D\x0A",
        "#COMD 23 38790 19216 &16\x0D\x0A#COMD 23 38791 00003 &07\x0D\x0A#COMD 23 38792 08072 &16\x0D\x0A#COMD 23 38793 00003 &09\x0D\x0A",
        "#COMD 23 38771 38793 &20\x0D\x0A#COMD 23 38769 38793 &27\x0D\x0A#COMD 23 38768 62669 &25\x0D\x0A#COMD 25 31300 &E1\x0D\x0A",
        "#COMD 23 38772 33440 &11\x0D\x0A#COMD 23 38773 33558 &1C\x0D\x0A#COMD 23 38774 33441 &14\x0D\x0A#COMD 23 38775 33558 &1E\x0D\x0A",
        "#COMD 23 38776 33329 &1B\x0D\x0A#COMD 23 38777 33445 &1B\x0D\x0A#COMD 23 38778 33328 &1C\x0D\x0A#COMD 23 38779 16768 &26\x0D\x0A",
        "#COMD 23 38780 33204 &0E\x0D\x0A#COMD 23 38781 33456 &18\x0D\x0A#COMD 23 38782 32770 &17\x0D\x0A#COMD 23 38783 33538 &1B\x0D\x0A",
        "#COMD 23 38784 32771 &1A\x0D\x0A#COMD 23 38785 33498 &22\x0D\x0A#COMD 23 38770 38785 &20\x0D\x0A#COMD 23 38786 19200 &14\x0D\x0A",
        "#COMD 23 38787 10190 &14\x0D\x0A#COMD 23 38788 19201 &17\x0D\x0A#COMD 23 38789 10133 &13\x0D\x0A#COMD 23 38790 19216 &16\x0D\x0A",
        "#COMD 23 38791 00003 &07\x0D\x0A#COMD 23 38792 08072 &16\x0D\x0A#COMD 23 38793 00003 &09\x0D\x0A#COMD 23 38771 38793 &20\x0D\x0A",
        "#COMD 23 38769 38793 &27\x0D\x0A#COMD 23 38768 62669 &25\x0D\x0A#COMD 25 31300 &E1\x0D\x0A"
    };



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 17
/*********************************************************************************************************************/
/* HALGPS0_17QuickStart : Start the chip for test mode, it must be used only to get the CNO value in production test */
/*********************************************************************************************************************/
void HALGPS0_17QuickStart(void)
{

    uint32_t  vl_readLen;
    uint32_t  vl_i;
    uint8_t   vl_attempt = 10;
    bool bPatchLoaded = FALSE;


    HALGPS0_06Start();

/*Patch Upload Section */

/* UART only */
    while(!bPatchLoaded && vl_attempt > 0)
    {
        /* patch loading */
        for(vl_i=0; vl_i < PATCH_LINE_NB; vl_i++)
        {
            /* HALGPS2_02UartSend((uint8_t*)patch[vl_i],strlen(patch[vl_i])); */
            GN_GPS_Write_GNB_Ctrl((U2) strlen(patch[vl_i]),(CH *)patch[vl_i]);
            /* GN_GPS_Read_GNB_Meas(0,NULL); */
            if( vl_i%3 == 0 )
            {
            /* + RRR */
            /* wait 50ms */
#ifdef __RTK_E__
#ifdef UTS_CPR
                    MC_UTSER_SLEEP(UTSER_10MSEC_PRECISION,50);
#else
                    MC_OST_START_TASK_TIMER(50);
#endif
#else
                    OSA_Sleep( 50 );
#endif /* __RTK_E__ */
            /* - RRR */
            }
        }

        /* + RRR */
        /* wait 50ms */
#ifdef __RTK_E__
#ifdef UTS_CPR
        MC_UTSER_SLEEP(UTSER_10MSEC_PRECISION,50);
#else
        MC_OST_START_TASK_TIMER(50);
#endif
#else
        OSA_Sleep( 50 );
#endif /* __RTK_E__ */
        /* - RRR */

        //do a reading and check if the chip stops sending #STAT, if yes it means the patch has been received
        vl_readLen = GN_GPS_Read_GNB_Meas( 4096 , a_readBuffer );

        if(vl_readLen < 4096)
            a_readBuffer[vl_readLen] = 0;
        else
            a_readBuffer[4095] = 0;

        if(strncmp((char*)a_readBuffer, "#STAT", 5) != 0)
            bPatchLoaded = TRUE;

        vl_attempt--;
    }
/* GPS_OVER_UART */
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 18
/*********************************************************************************************************************/
/* HALGPS0_18QuickStop : Stop the chip for test mode, it must be used only to get the CNO value in production test */
/*********************************************************************************************************************/
void HALGPS0_18QuickStop(void)
{
    HALGPS0_07Stop();
}
#endif /* CGPS_CNO_VALUE_FTR */
/* -LMSqc06482 : Production test */


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 19
/*****************************************************************************/
/* HALGPS0_19DeleteUTCCalibStore : message receive to Delete GPS_UTC_Calib.bin file which has RTC calibration */
/*****************************************************************************/
void HALGPS0_19DeleteUTCCalibStore()
{
    int32_t vl_status = remove(HALGPS_ROOT_PATH HALGPS_UTC_CALIBRATION_FILE_NAME);
    MC_HALGPS_TRACE_INF("BEGIN : HALGPS0_19DeleteUTCCalibStore");
    if( vl_status == 0 )
    {
       MC_HALGPS_TRACE_INF("HALGPS0_19DeleteUTCCalibStore file deleted successfully\n");
    }
    else
    {
       MC_HALGPS_TRACE_INF("HALGPS0_19DeleteUTCCalibStore: Unable to delete the file\n");
       perror("HALGPS0_19DeleteUTCCalibStore :Error ");
    }

    MC_HALGPS_TRACE_INF("END : HALGPS0_19DeleteUTCCalibStore");
}

#undef __HALGPS0MAIN_C__
