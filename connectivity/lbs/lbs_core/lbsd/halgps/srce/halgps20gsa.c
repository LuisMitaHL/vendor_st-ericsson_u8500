/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#define __HALGPS20GSA_C__

/**
* \file halgps20freqaiding.c
* \date 17/07/2009
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contains functions that manage frequency aiding.\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 17.07.09</TD><TD> Archana.B </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#if defined( GPS_FREQ_AID_FTR ) && defined(__RTK_E__)

#include "halgpsgsa.h"
#include "halgpsgsainc.h"

#undef  MODULE_NUMBER
#define MODULE_NUMBER   MODULE_HALGPS

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER  PROCESS_HALGPS

#undef  FILE_NUMBER
#define FILE_NUMBER     20

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0

/******************************************************************************/
/* HALGPS20_05GsaCheckClockCalibration                                        */
/******************************************************************************/
uint8_t HALGPS20_00GsaCheckClockCalibration()
{
    uint8_t vl_RetVal=0;

    MC_HALGPS_TRACE_INF("BEGIN : HALGPS20_00GsaCheckClockCalibration");

    if( vg_HALGPS_AFCStatus.v_Status        == K_GSA_AFC_STATUS_CLOSED   &&
        vg_HALGPS_AFCStatus.v_FreqError_ppb <  HALGPS_GSA_MAX_FREQ_ERROR  )
    {
        /* Frequency aiding can take place */
        vl_RetVal = 1;
    }
    MC_HALGPS_TRACE_INF("END : HALGPS20_00GsaCheckClockCalibration");
    return vl_RetVal;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/*******************************************************************************************/
/* HALGPS20_01GsaInit : This function is call by CGPS module when it is being initialized. */
/*******************************************************************************************/
e_halgps_gsastatus HALGPS20_01GsaInit(void)
{
    t_PWMInitData *  ptPWMInitData=NULL;
        t_PWMInitData    tPWMInitData;

    t_SCONInitData *  ptSCONInitData=NULL;
    t_SCONInitData    tSCONInitData;

    MC_HALGPS_TRACE_INF("BEGIN : HALGPS20_01GsaInit");

        ptPWMInitData = &tPWMInitData;
    memset ( ptPWMInitData, 0, sizeof ( tPWMInitData ) );

    if ( kid3_00Init ( PWM, HALGPS_PWM_DEV_INST, (void *) ptPWMInitData ) == KIDERR_OK )
    {
        kid3_04Ioctl ( SCON, 0, SCON_IOCTL_GET_CONFIGURATION, & tSCONInitData );

        ptSCONInitData = & tSCONInitData;

#if defined HALGPS_PWM1_GPIO_06c
        ptSCONInitData -> atSCONMUXInitData[0].sconMux  |= KID0_SetValue(SCON,sysMux0,gpio0_06c,SCON_MUX_GPIO0_PWM0);
        
        ptSCONInitData -> sysCon0                       |= KID0_SetValue(SCON,sysCon0,pwmOr0,SCON_PWM_OR_0_ONLY);
#elif defined HALGPS_PWM2_GPIO_03c
                ptSCONInitData -> atSCONMUXInitData[0].sconMux  |= KID0_SetValue(SCON,sysMux0,gpio0_03c,SCON_MUX_GPIO0_PWM1);
        
        ptSCONInitData -> sysCon0                       |= KID0_SetValue(SCON,sysCon0,pwmOr1,SCON_PWM_OR_1_ONLY);
#endif         

        kid3_04Ioctl ( SCON, 0, SCON_IOCTL_SET_CONFIGURATION, & tSCONInitData );
    }

    vg_HALGPS_AFCStatus.v_PrevFreqError_ppb = (uint32_t)sqrt(( pow(HALGPS_GSA_BTS_FREQ_TOLERANCE, 2) + pow(HALGPS_GSA_DOPPLER_EFFECT, 2))/2);

    MC_HALGPS_TRACE_INF("END : HALGPS20_01GsaInit");
    return HALGPS_GSA_OK;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/**********************************************************************************************/
/* HALGPS20_02GsaActivateClockCalibration : Function call when Clock calibration is required. */
/**********************************************************************************************/
e_halgps_gsastatus HALGPS20_02GsaActivateClockCalibration()
{
    t_kidSetFrequencyData  tPWMFrequencyData;
    t_kidSetModulationData tPWMModulationData;
        e_halgps_gsastatus     vl_RetVal;

    MC_HALGPS_TRACE_INF("BEGIN : HALGPS20_02GsaActivateClockCalibration");

    if( 0 == HALGPS20_00GsaCheckClockCalibration() )
    {
        MC_HALGPS_TRACE_INF("HALGPS20_02GsaActivateClockCalibration: clock calibration check failed");
        MC_HALGPS_TRACE_INF("END 1: HALGPS20_02GsaActivateClockCalibration");
        return HALGPS_GSA_AFC_STATUS_NOT_OK;
    }

    switch( vg_HALGPS_FreqAidingStatus )
    {
        case HALGPS_GSA_CLK_DEACTIVATED:
        case HALGPS_GSA_UNKNOWN:
        {

            /*------------------------------------------------------------------------*/
            /* prepare the data                                                       */
            /*------------------------------------------------------------------------*/

            tPWMFrequencyData.ulFrequency = HALGPS_GSA_FREQ;
            tPWMFrequencyData.ucDutyCycle = HALGPS_GSA_DUTY_CYCLE;
            tPWMFrequencyData.ucMode      = HALGPS_GSA_DUTY_MODE;
            /*------------------------------------------------------------------------*/
            /* Set Frequency and duty cycle to the modulator                          */
            /*------------------------------------------------------------------------*/

            kid3_04Ioctl( PWM, HALGPS_PWM_DEV_INST, PWM_IOCTL_SET_FREQUENCY, &tPWMFrequencyData);

            /*------------------------------------------------------------------------*/
            /* Check period and time to the modulator                                  */
            /*------------------------------------------------------------------------*/
            kid3_04Ioctl ( PWM, HALGPS_PWM_DEV_INST, PWM_IOCTL_GET_PERIOD_AND_TIMER,
                                           &tPWMModulationData );

            if( tPWMModulationData.usPeriodValue == HALGPS_GSA_PERIOD_VALUE &&
                tPWMModulationData.usTimerValue  == HALGPS_GSA_TIMER_VALUE  &&
                tPWMModulationData.ucMode        == HALGPS_GSA_DUTY_MODE        )
            {
                GN_AGPS_Start_Freq_Aiding(0.0002, vg_HALGPS_AFCStatus.v_FreqError_ppb);

                vg_HALGPS_FreqAidingStatus = HALGPS_GSA_CLK_ACTIVATED;

                MC_HALGPS_TRACE_INF("HALGPS20_02GsaActivateClockCalibration : Started freq aiding");
                MC_HALGPS_TRACE_INF("END 2: HALGPS20_02GsaActivateClockCalibration");
                return HALGPS_GSA_CLK_ACTIVATED;
            }
            else
            {
                MC_HALGPS_TRACE_INF("HALGPS20_02GsaActivateClockCalibration Period = %d, Timer = %d, Mode = %d",
                            tPWMModulationData.usPeriodValue,
                            tPWMModulationData.usTimerValue,
                            tPWMModulationData.ucMode );
                tPWMModulationData.usPeriodValue = 0;
                tPWMModulationData.usTimerValue  = 0;
                tPWMModulationData.ucMode        = 0;

                kid3_04Ioctl ( PWM, HALGPS_PWM_DEV_INST, PWM_IOCTL_SET_PERIOD_AND_TIMER,
                                           &tPWMModulationData );

                MC_HALGPS_TRACE_INF("END 3 : HALGPS20_02GsaActivateClockCalibration");
                vl_RetVal = HALGPS_GSA_UNKNOWN;
            }

        }
        break;
        case HALGPS_GSA_CLK_ACTIVATED:
            MC_HALGPS_TRACE_INF("HALGPS20_02GsaActivateClockCalibration : Freq aiding already started");
            MC_HALGPS_TRACE_INF("END 4: HALGPS20_02GsaActivateClockCalibration");
            vl_RetVal = HALGPS_GSA_CLK_ALREADY_ACTIVE;
        break;
        default:
            MC_HALGPS_TRACE_INF("END 5: HALGPS20_02GsaActivateClockCalibration");
            vl_RetVal = HALGPS_GSA_UNKNOWN;
        break;
    }
    return vl_RetVal;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/********************************************************************************************************/
/* HALGPS20_03GsaDeActivateClockCalibration : Function call when Clock calibration needs to be stopped. */
/********************************************************************************************************/
e_halgps_gsastatus HALGPS20_03GsaDeActivateClockCalibration()
{
    t_kidSetModulationData tPWMModulationData;

    MC_HALGPS_TRACE_INF("BEGIN : HALGPS20_03GsaDeActivateClockCalibration");

    if( vg_HALGPS_FreqAidingStatus == HALGPS_GSA_CLK_ACTIVATED )
    {
        GN_AGPS_Stop_Freq_Aiding();

        tPWMModulationData.usPeriodValue = 0;
        tPWMModulationData.usTimerValue  = 0;
        tPWMModulationData.ucMode        = 0;

        /*------------------------------------------------------------------------*/
        /* Set period and time to the modulator                                   */
        /*------------------------------------------------------------------------*/
        kid3_04Ioctl ( PWM, HALGPS_PWM_DEV_INST, PWM_IOCTL_SET_PERIOD_AND_TIMER,
                                   &tPWMModulationData );

        vg_HALGPS_FreqAidingStatus = HALGPS_GSA_CLK_DEACTIVATED;

        MC_HALGPS_TRACE_INF("HALGPS20_02GsaActivateClockCalibration : Freq aiding stopped");
        MC_HALGPS_TRACE_INF("END 1: HALGPS20_03GsaDeActivateClockCalibration");

        return HALGPS_GSA_CLK_DEACTIVATED;
    }
    else
    {
        MC_HALGPS_TRACE_INF("HALGPS20_02GsaActivateClockCalibration : Freq aiding already stopped");
        MC_HALGPS_TRACE_INF("END 2: HALGPS20_03GsaDeActivateClockCalibration");
        return HALGPS_GSA_CLK_ALREADY_INACTIVE;
    }
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/******************************************************************************/
/* HALGPS20_04GsaCheckClockCalibration                                        */
/******************************************************************************/
e_halgps_gsastatus HALGPS20_04GsaCheckClockCalibration()
{
    e_halgps_gsastatus vl_RetVal = HALGPS_GSA_AFC_STATUS_NOT_OK ;

    MC_HALGPS_TRACE_INF("BEGIN : HALGPS20_04GsaCheckClockCalibration");

    if(1 == HALGPS20_00GsaCheckClockCalibration())
    {
        /* Frequency aiding can take place */
        switch( vg_HALGPS_FreqAidingStatus )
        {
            case HALGPS_GSA_CLK_ACTIVATED:
            {
                int16_t vl_FreqErrDiff = vg_HALGPS_AFCStatus.v_PrevFreqError_ppb - vg_HALGPS_AFCStatus.v_FreqError_ppb;
                if( vl_FreqErrDiff < 0 )
                {    
                    vl_FreqErrDiff = vl_FreqErrDiff * -1;
                } 

                if( vl_FreqErrDiff > 40 )
                {
/*BL GN_AGPS_Start_Freq_Aiding(
   R8 Freq,                    // i - Known frequency of the input pulses [MHz]
   U4 RMS_ppb );               // i - Uncertainty (rms value) of the known frequency [ppb] */
                    GN_AGPS_Start_Freq_Aiding(0.0002, vg_HALGPS_AFCStatus.v_FreqError_ppb);
                    MC_HALGPS_TRACE_INF("HALGPS20_04GsaCheckClockCalibration: GN_AGPS_Start_Freq_Aiding called Prev : %d, Current : %d", vg_HALGPS_AFCStatus.v_PrevFreqError_ppb, vg_HALGPS_AFCStatus.v_FreqError_ppb);

                    vg_HALGPS_AFCStatus.v_PrevFreqError_ppb = vg_HALGPS_AFCStatus.v_FreqError_ppb;
                }
                vl_RetVal = HALGPS_GSA_CLK_ALREADY_ACTIVE;
            }
                break;
            case HALGPS_GSA_CLK_DEACTIVATED:
            case HALGPS_GSA_UNKNOWN:
            default:
                vl_RetVal = HALGPS_GSA_CLK_ALREADY_INACTIVE;
                break;
        }
    }
    else /*HALGPS20_05GsaCheckClockCalibration == FALSE*/
    {
        /* Frequency aiding should not take place */
        switch(vg_HALGPS_FreqAidingStatus)
        {
            case HALGPS_GSA_CLK_ACTIVATED:
            {
                HALGPS20_03GsaDeActivateClockCalibration();
                vg_HALGPS_FreqAidingStatus = HALGPS_GSA_CLK_DEACTIVATED;
                vl_RetVal = HALGPS_GSA_CLK_DEACTIVATED;
            }
               break;
            case HALGPS_GSA_UNKNOWN:
            case HALGPS_GSA_CLK_DEACTIVATED:
              vl_RetVal = HALGPS_GSA_CLK_ALREADY_INACTIVE;
             default:
                break;
        }
    }

    MC_HALGPS_TRACE_INF("END : HALGPS20_04GsaCheckClockCalibration");

    return vl_RetVal;
}

#endif

#undef __HALGPS20GSA_C__
