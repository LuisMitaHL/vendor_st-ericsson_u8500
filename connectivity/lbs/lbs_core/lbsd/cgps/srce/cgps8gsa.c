/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
* \file cgps8gsafreqaiding.c
* \date 23/07/2009
* \version 1.0
*
* <B>Compiler:</B> ARM ADS/RVDS\n
*
* <B>Description:</B>  This file contains all functions relative to frequency aiding\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 21.07.08 </TD><TD> ANSHUMAN PANDEY </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#define __CGPS8GSA_C__
#ifdef GPS_FREQ_AID_FTR

#include "cgpsgsa.h"
#include "GN_AGPS_api.h"


#undef MODULE_NUMBER
#define MODULE_NUMBER MODULE_CGPS

#undef PROCESS_NUMBER
#define PROCESS_NUMBER PROCESS_CGPS

#undef FILE_NUMBER
#define FILE_NUMBER 8

#ifdef __RTK_E__
/* ++ LMSQC06781*/
/*
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
*/
/*****************************************************************************************************/
/* CGPS8_00GsaInit :         */
/*****************************************************************************************************/
/*
void  CGPS8_00GsaInit()
{

    e_halgps_gsastatus     vl_RetVal;

    MC_CGPS_TRACE(("CGPS8_00GsaInit : Start"));

    vl_RetVal = MC_HALGPS_GSAINIT();

    if (( HALGPS_GSA_OK != vl_RetVal))
    {
        MC_CGPS_TRACE(("CGPS8_00GsaInit : HALGPS GSAINIT ERROR"));
    }

    MC_CGPS_TRACE(("CGPS8_00GsaInit : Exit"));
}
*/
/* +- LMSQC06781 */

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/*****************************************************************************************************/
/* CGPS8_01ActivateGsa :         */
/*****************************************************************************************************/
void CGPS8_01ActivateGsa()
{

    e_halgps_gsastatus     vl_RetVal;
    uint32_t vl_CurrentTime = GN_GPS_Get_OS_Time_ms();

    MC_CGPS_TRACE(("CGPS8_01ActivateGsa : Start"));

    vl_RetVal = MC_HALGPS_GSA_ACTIVATE_CLOCK_CAL();

   switch (vl_RetVal)
       {
        case HALGPS_GSA_CLK_ALREADY_ACTIVE:       /*!< GSA Clock calibration is already ON   */
           //reset timers
        case HALGPS_GSA_CLK_ACTIVATED:            /*!< GSA Clock calibration is activated   */
            /*start the timers 200ms and 3sec */
            vg_ClockCalibTimeout = vl_CurrentTime + K_CGPS_CALIB_CLK_TIMEOUT;
            vg_PeridicCheckTimeout = vl_CurrentTime + K_CGPS_CALIB_CLK_PERIOD_CHECK_TIMEOUT;
            MC_CGPS_TRACE(("CGPS8_01ActivateGsa : HALGPS_GSA_ACTIVATE_CLOCK_CAL OK"));
           break;
        case  HALGPS_GSA_CLK_ALREADY_INACTIVE:     /*!< GSA Clock calibration is already OFF   */
        case HALGPS_GSA_CLK_DEACTIVATED:          /*!< GSA Clock calibration is deactivated   */
        case HALGPS_GSA_AFC_STATUS_NOT_OK:
        case HALGPS_GSA_UNKNOWN:
            MC_CGPS_TRACE(("CGPS8_01ActivateGsa : HALGPS_GSA_ACTIVATE_CLOCK_CAL ERROR"));
            default:
        break;
     }

  MC_CGPS_TRACE(("CGPS8_01ActivateGsa : Exit"));

}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/*****************************************************************************************************/
/* CGPS8_02DeActivateGsa :         */
/*****************************************************************************************************/
void CGPS8_02DeActivateGsa()
{

    e_halgps_gsastatus     vl_RetVal;

    MC_CGPS_TRACE(("CGPS8_02DeActivateGsa : Start"));

    vl_RetVal = MC_HALGPS_GSA_DEACTIVATE_CLOCK_CAL();

   switch (vl_RetVal)
       {
        case  HALGPS_GSA_CLK_ALREADY_INACTIVE:     /*!< GSA Clock calibration is already OFF   */
        case HALGPS_GSA_CLK_DEACTIVATED:          /*!< GSA Clock calibration is deactivated   */
            vg_ClockCalibTimeout = 0;
            vg_PeridicCheckTimeout = 0;

             MC_CGPS_TRACE(("CGPS8_02DeActivateGsa : HALGPS_GSA_DEACTIVATE_CLOCK_CAL OK"));

            break;
        case HALGPS_GSA_AFC_STATUS_NOT_OK:
        case HALGPS_GSA_CLK_ALREADY_ACTIVE:       /*!< GSA Clock calibration is already ON     */
        case HALGPS_GSA_CLK_ACTIVATED:              /*!< GSA Clock calibration is activated    */
        case HALGPS_GSA_UNKNOWN:
            MC_CGPS_TRACE(("CGPS8_02DeActivateGsa : HALGPS_GSA_DEACTIVATE_CLOCK_CAL ERROR"));
            default:
        break;
     }

  MC_CGPS_TRACE(("CGPS8_02DeActivateGsa : Exit"));

}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/*****************************************************************************************************/
/* CGPS8_03CheckGsa :         */
/*****************************************************************************************************/
void CGPS8_03CheckGsa()
{

    e_halgps_gsastatus     vl_RetVal;

    MC_CGPS_TRACE(("CGPS8_03CheckGsa : Start"));

    vl_RetVal = MC_HALGPS_GSA_CHECK_CLOCK_CAL();

   switch (vl_RetVal)
       {

        case HALGPS_GSA_CLK_DEACTIVATED:          /*!< GSA Clock calibration is deactivated   */
            vg_ClockCalibTimeout = 0;
            vg_PeridicCheckTimeout = 0;
             MC_CGPS_TRACE(("CGPS8_03CheckGsa : Freq error is high, stopping clk calib"));
            break;
        case HALGPS_GSA_AFC_STATUS_NOT_OK:
        case HALGPS_GSA_CLK_ALREADY_INACTIVE:     /*!< GSA Clock calibration is already OFF   */
        case HALGPS_GSA_UNKNOWN:
            vg_ClockCalibTimeout = 0;
            vg_PeridicCheckTimeout = 0;
            MC_CGPS_TRACE(("CGPS8_03CheckGsa :  ERROR"));
            break;
        case HALGPS_GSA_CLK_ALREADY_ACTIVE:       /*!< GSA Clock calibration is already ON     */
            break;
        case HALGPS_GSA_CLK_ACTIVATED:              /*!< GSA Clock calibration is activated    */
            MC_CGPS_TRACE(("CGPS8_03CheckGsa : warn - activated now"));

            default:
        break;
     }

  MC_CGPS_TRACE(("CGPS8_03CheckGsa : Exit"));

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/*****************************************************************************************************/
/* CGPS8_04CheckGsaTimerExp :         */
/*****************************************************************************************************/
void CGPS8_04CheckGsaTimerExp(uint32_t vl_CurrentTime)
{

    e_halgps_gsastatus     vl_RetVal;

    if( vg_PeridicCheckTimeout == 0 && vg_ClockCalibTimeout == 0 )
    {
        return;
    }

    if(vl_CurrentTime >= vg_PeridicCheckTimeout )
    {
        vg_PeridicCheckTimeout = vl_CurrentTime + K_CGPS_CALIB_CLK_PERIOD_CHECK_TIMEOUT;
        CGPS8_03CheckGsa();
    }

    else if(vl_CurrentTime >= vg_ClockCalibTimeout )
    {
        CGPS8_02DeActivateGsa();
    }
}

#else /* __RTK_E__ */

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/*****************************************************************************************************/
/* CGPS8_01ActivateGsa :         */
/*****************************************************************************************************/
void CGPS8_01ActivateGsa()
{
    MC_CGPS_TRACE(("CGPS8_01ActivateGsa : Start"));

    switch( vg_CGPS_CCState )
    {
        case K_CGPS_CC_DEACTIVATED :
                {
            t_lsimccm_ClockCalReqM* pl_ClockCalReq=NULL ;
            pl_ClockCalReq = (t_lsimccm_ClockCalReqM*)MC_RTK_GET_MEMORY(sizeof(*pl_ClockCalReq));

            pl_ClockCalReq->v_Command   = LSIMCCM_CLOCK_CAL_START;
            pl_ClockCalReq->v_Frequency = vg_CGPS_CCFreq;
            pl_ClockCalReq->v_Threshold = 0;
            usleep(50000);

            MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                       PROCESS_LSIMCCM, 0,
                                       LSIMCCM_CLOCK_CAL_REQ,
                                       (t_MsgHeader *)pl_ClockCalReq);

            vg_CGPS_CCState        = K_CGPS_CC_ACTIVATE_PENDING;
                }
        break;

        case K_CGPS_CC_ACTIVATE_PENDING :
        case K_CGPS_CC_ACTIVATED :
            MC_CGPS_TRACE(("CGPS8_01ActivateGsa : Already requested to start"));
            /* Nothing to Do */
        break;

        default :
        break;
    }

    MC_CGPS_TRACE(("CGPS8_01ActivateGsa : Exit"));
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/*****************************************************************************************************/
/* CGPS8_02DeActivateGsa :         */
/*****************************************************************************************************/
void CGPS8_02DeActivateGsa()
{
    MC_CGPS_TRACE(("CGPS8_02DeActivateGsa : Start"));

    switch ( vg_CGPS_CCState )
    {
        case K_CGPS_CC_DEACTIVATED :
            /* Nothing to Do */
        break;

        case K_CGPS_CC_ACTIVATE_PENDING :
        case K_CGPS_CC_ACTIVATED :
            CGPS8_06StopFreqAidingComputation();
        break;

        default:
        break;
    }

    MC_CGPS_TRACE(("CGPS8_02DeActivateGsa : Exit"));

}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/*****************************************************************************************************/
/* CGPS8_04CheckGsaTimerExp :         */
/*****************************************************************************************************/
void CGPS8_04CheckGsaTimerExp(uint32_t vl_CurrentTime)
{
    if( vg_ClockCalibTimeout == 0 )
    {
        return;
    }

    if(vl_CurrentTime >= vg_ClockCalibTimeout )
    {
        CGPS8_02DeActivateGsa();
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/*****************************************************************************************************/
/* CGPS8_05CheckGsaTimerExp :         */
/*****************************************************************************************************/
const t_OperationDescriptor* CGPS8_05HandleClockCalibInd(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor     *pl_NextState   = SAME;
    t_lsimccm_ClockCalIndM          *pl_ClockCalInd = (t_lsimccm_ClockCalIndM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    MC_CGPS_TRACE(("CGPS8_05HandleClockCalibInd : Entry "));

    MC_CGPS_TRACE(("CGPS8_05HandleClockCalibInd : vg_CGPS_CCState <%d>" , vg_CGPS_CCState ));
    switch ( vg_CGPS_CCState )
    {
        case K_CGPS_CC_DEACTIVATED :
            /* Do Nothing */
        break;

        case K_CGPS_CC_ACTIVATE_PENDING :
            switch ( pl_ClockCalInd->v_Status )
            {
                case LSIMCCM_CLOCK_STAT_ACTIVE :
                if (   (LSIMCCM_FREQ_LOCKED == pl_ClockCalInd->v_FreqLockStatus )
                    && (K_CGPS_ACTIVE_GPS_ON == vg_CGPS_State)
                   )
                {
                        uint32_t vl_CurrentTime = GN_GPS_Get_OS_Time_ms();
                                        MC_CGPS_TRACE(("CGPS8_05HandleClockCalibInd : Frequency <%luHz>, RMS Error <%luppb>",
                                                                    pl_ClockCalInd->v_Frequency, pl_ClockCalInd->v_ErrorEstimateRMS));
                        vg_CGPS_FreqErrorAtStart = pl_ClockCalInd->v_ErrorEstimateRMS;

                        GN_AGPS_Start_Freq_Aiding(MC_CGPS_HZ_TO_MHZ(pl_ClockCalInd->v_Frequency), vg_CGPS_FreqErrorAtStart);
                        /*start the timer - 3sec */
                            vg_ClockCalibTimeout   = vl_CurrentTime + K_CGPS_CALIB_CLK_TIMEOUT;
                                        vg_CGPS_CCState = K_CGPS_CC_ACTIVATED;
                }
                else
                {
                        MC_CGPS_TRACE(("ERROR CGPS8_05HandleClockCalibInd : LockStatus <%d>, CGPS_State <%s>",
                                                    pl_ClockCalInd->v_FreqLockStatus, a_CGPS_StateAsEnum[vg_CGPS_State]));

                }
                break;

                case LSIMCCM_CLOCK_STAT_NOT_ACTIVE :
                case LSIMCCM_CLOCK_STAT_NOT_SUPPORTED :
                    vg_CGPS_CCState = K_CGPS_CC_DEACTIVATED;
                break;

                case LSIMCCM_CLOCK_STAT_ALTERNATE_FREQ :
                    // TODO
                break;

                default:
                break;
            }
        break;

        case K_CGPS_CC_ACTIVATED :
            switch ( pl_ClockCalInd->v_Status )
            {
                case LSIMCCM_CLOCK_STAT_ACTIVE :
                    if (   FALSE == pl_ClockCalInd->v_FreqLockStatus
                        && abs(vg_CGPS_FreqErrorAtStart - pl_ClockCalInd->v_ErrorEstimateRMS) > K_CGPS_MAX_FREQ_ERROR )
                    {
                        CGPS8_06StopFreqAidingComputation();
                    }
                break;

                case LSIMCCM_CLOCK_STAT_NOT_ACTIVE :
                case LSIMCCM_CLOCK_STAT_ALTERNATE_FREQ :
                case LSIMCCM_CLOCK_STAT_NOT_SUPPORTED :
                    MC_CGPS_TRACE(("ERROR CGPS8_05HandleClockCalibInd : Unexpected Ind message after Clock calibration started "));
                    CGPS8_06StopFreqAidingComputation();
                break;

                default:
                break;
            }
            break;
        break;

        default:
        break;
    }

    MC_CGPS_TRACE(("CGPS8_05HandleClockCalibInd : Exit "));

    return pl_NextState;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6
/*****************************************************************************************************/
/* CGPS8_06StopClockCalibration :         */
/*****************************************************************************************************/
void CGPS8_06StopFreqAidingComputation(void)
{

        switch( vg_CGPS_CCState )
    {
        case K_CGPS_CC_ACTIVATED :
                GN_AGPS_Stop_Freq_Aiding();
        case K_CGPS_CC_ACTIVATE_PENDING :
        {
                    t_lsimccm_ClockCalReqM* pl_ClockCalReq=NULL;
                    pl_ClockCalReq = (t_lsimccm_ClockCalReqM*)MC_RTK_GET_MEMORY(sizeof(*pl_ClockCalReq));

                    pl_ClockCalReq->v_Command = LSIMCCM_CLOCK_CAL_STOP;

                    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                                               PROCESS_LSIMCCM, 0,
                                               LSIMCCM_CLOCK_CAL_REQ,
                                               (t_MsgHeader *)pl_ClockCalReq);

                    vg_ClockCalibTimeout = 0;
                    vg_CGPS_CCState      = K_CGPS_CC_DEACTIVATED;

                    MC_CGPS_TRACE(("CGPS8_06StopFreqAidingComputation : Stopped Freq Aiding computation"));
                }
        break;

                case K_CGPS_CC_DEACTIVATED :
        default :
                MC_CGPS_TRACE(("CGPS8_06StopFreqAidingComputation : Cannot stop as in state <%d>" , vg_CGPS_CCState));
        break;
    }
}

#endif /* __RTK_E__ */

#endif /*GPS_FREQ_AID_FTR*/

#undef __CGPS8GSA_C__
