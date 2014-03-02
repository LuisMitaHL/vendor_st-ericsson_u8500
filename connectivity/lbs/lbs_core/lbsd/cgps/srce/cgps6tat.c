/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

/**
* \file cgps6tat.c
* \date 24/06/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B>  This file contains TAT function\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 24.06.08 </TD><TD> M.BELOU </TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#define __CGPS6TAT_C__

#ifdef __RTK_E__

#include "cgpstat.h"

#undef MODULE_NUMBER
#define MODULE_NUMBER MODULE_CGPS

#undef PROCESS_NUMBER
#define PROCESS_NUMBER PROCESS_CGPS

#undef FILE_NUMBER
#define FILE_NUMBER 0


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
/*****************************************************************************/
/* CGPS0_00TATtimerExpired : TAT Timer function                              */
/*****************************************************************************/
const t_OperationDescriptor* CGPS6_00TATTimerExpired(t_RtkObject* p_FsmObject)
{
    /* check SUPL notification timeout */
    uint16_t vl_TransparentReadDataSize;
/* ++LMSqb95231  */
    uint8_t* pl_TransparentData = NULL;
/* --LMSqb95231  */
    t_CGPS_TatTransparentDataInd * pl_CnfMsg=NULL;


    /*first relaucnh timer*/
    MC_RTK_PROCESS_START_TIMER(CGPS_TAT_LOOP_TIMER, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_TAT_LOOP_TIMER_DURATION));

    if ((vg_TatTransparentMode==1) && (gn_Patch_Status == 7))
    {
/* ++LMSqb95231  */
        pl_TransparentData =  (uint8_t*) MC_RTK_GET_MEMORY( CGPS_TAT_TRANSPARENT_READ_BUFFER_SIZE );
/* --LMSqb95231  */
        memset((char*)&pl_TransparentData[0],0,CGPS_TAT_TRANSPARENT_READ_BUFFER_SIZE);
        vl_TransparentReadDataSize = GN_GPS_Read_GNB_Meas(CGPS_TAT_TRANSPARENT_READ_BUFFER_SIZE,(char*)&pl_TransparentData[0]);

        if (vl_TransparentReadDataSize!=0)
        {
            pl_CnfMsg = (t_CGPS_TatTransparentDataInd*)MC_RTK_GET_MEMORY(sizeof(*pl_CnfMsg));
            pl_CnfMsg->v_Lenght = vl_TransparentReadDataSize;
            pl_CnfMsg->v_Data = (uint8_t*)MC_RTK_GET_MEMORY(vl_TransparentReadDataSize*sizeof(uint8_t));
            memcpy(pl_CnfMsg->v_Data,(uint8_t*)&pl_TransparentData[0],vl_TransparentReadDataSize);
            MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_MIS,0,CGPS_TAT_TRANSPARENT_DATA_IND,(t_MsgHeader*)pl_CnfMsg );
        }
/* ++LMSqb95231  */
        MC_RTK_FREE_MEMORY(pl_TransparentData);
/* --LMSqb95231  */
    }
    else
    {
        /* If in here there is at least one user so the GPS is already, or will be */
        /* powered up now. */
        GN_GPS_Update();

        if (! GN_GPS_Get_Nav_Data( &vg_Nav_Data ) )
        {
            /* No New Nav data was received from the baseband so GN_GPS_Update()   */
            /* did not call GN_GPS_Write_GNB_Ctrl() to send data to the baseband.  */
            /* If there is still some patch code data that needs to be sent to the */
            /* baseband, call the patch uploader.                                  */
            /* This main loop repeats every 50ms which means at 115200 baud TX you */
            /* can send up to 11520*0.050 = 576 bytes, or 22 x 26 byte messages.   */
            /* So target 20 messages, but this number may need to be reduced based */
            /* on the efficiency of the UART TX Driver                             */
            if ( gn_Patch_Status > 0  &&  gn_Patch_Status < 7 )   /* Patching in progress*/
            {
#ifdef GPS_OVER_SPI
                CGPS2_00UploadMEPatch( 65 ); /* With SPI at 400kb/s: 40000*0.05 = 2000 bytes =  74*27 bytes targeted to 65 messages */
#else
                CGPS2_00UploadMEPatch( 20 );
#endif
            }    
        }       
    }
    return SAME;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/*****************************************************************************/
/* CGPS0_04TATActivate : activate or deactivate TAT                          */
/*****************************************************************************/
const t_OperationDescriptor* CGPS6_01TATActivate(t_RtkObject* p_FsmObject)
{
    e_cgps_Error vl_return = K_CGPS_NO_ERROR;
    e_cgps_tat_error vl_error = CGPS_TAT_ERROR_UNKNOWN;
    t_CGPS_TatActivationCnf * pl_CnfMsg=NULL;
    t_CGPS_TatActivationErr * pl_ErrMsg=NULL;
    t_CGPS_TatActivationReq *pl_ReqMsg=NULL;

    pl_ReqMsg =  (t_CGPS_TatActivationReq*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    switch (pl_ReqMsg->v_State)
    {
    case CGPS_TAT_STATE_ACTIVATED:
    {
        if (vg_Tat_Status != CGPS_TAT_STATE_ACTIVATED)
        {
#ifdef AGPS_UP_FTR
            /* SUPL isn't used with TAT */
            MC_CGPS_UPDATE_CONFIGURATION_ELEMENT(0, K_CGPS_CONFIG_AGPS_TRANSPORT_UPLANE);
#endif

            /* start CGPS : init lib and start HALGPS*/
            vl_return = CGPS0_30StartLowLayerGPS();

            if (vl_return == K_CGPS_NO_ERROR)
            {
                /* launch main loop*/
                MC_RTK_PROCESS_START_TIMER(CGPS_TAT_LOOP_TIMER, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_TAT_LOOP_TIMER_DURATION));

                vg_Tat_Status = CGPS_TAT_STATE_ACTIVATED;
                vg_Tat_StateMode = CGPS_TAT_NORMAL_MODE;
            }
            else
            {
                vl_error = CGPS_TAT_ERROR_UNKNOWN;
            }
        }
        break;
    }
    case CGPS_TAT_STATE_DEACTIVATED:
    {
        if (vg_Tat_Status == CGPS_TAT_STATE_ACTIVATED)
        {
            MC_HALGPS_POWERDOWN();
            vg_Tat_Status = CGPS_TAT_STATE_DEACTIVATED;
            vg_Tat_StateMode = CGPS_TAT_OFF_MODE;
        }
        break;
    }
    default:
    {
        vl_error = CGPS_TAT_ERROR_INVALID_PARAM;
        vl_return = K_CGPS_ERR_INVALID_PARAMETER;
        break;
    }
    }

    /*send message to MIS (TAT)*/
    if (vl_return ==K_CGPS_NO_ERROR)
    {
        pl_CnfMsg = (t_CGPS_TatActivationCnf*)MC_RTK_GET_MEMORY(sizeof(*pl_CnfMsg));

        pl_CnfMsg->v_State = vg_Tat_Status;

        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_MIS,0,CGPS_TAT_ACTIVATION_CNF,(t_MsgHeader*)pl_CnfMsg );
    }
    else
    {
        pl_ErrMsg = (t_CGPS_TatActivationErr*)MC_RTK_GET_MEMORY(sizeof(*pl_ErrMsg));
        pl_ErrMsg->v_CurrentState = vg_Tat_Status;
        pl_ErrMsg->v_ErrorNumber = vl_error;

        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_MIS,0,CGPS_TAT_ACTIVATION_ERR,(t_MsgHeader*)pl_ErrMsg );
    }
    
    return SAME;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/*****************************************************************************/
/* CGPS0_05TATGetInfo : ask info to halgps and return them to TAT            */
/*****************************************************************************/
const t_OperationDescriptor* CGPS6_02TATGetInfo(t_RtkObject* p_FsmObject)
{
    t_CGPS_TatGetInfoErr * pl_ErrMsg=NULL;
    t_CGPS_TatGetInfoCnf *pl_CnfMsg=NULL;
    t_HALGPSTatInfo     *vp_Info=NULL;
    vp_Info = (t_HALGPSTatInfo*)MC_RTK_GET_MEMORY(sizeof(*vp_Info));

    if (MC_HALGPS_GETINFO(vp_Info) == TRUE)
    {
        pl_CnfMsg = (t_CGPS_TatGetInfoCnf*)MC_RTK_GET_MEMORY(sizeof(*pl_CnfMsg));
        pl_CnfMsg->v_EmbeddedMajorVersion = vp_Info->v_EmbeddedMajorVersion;
        pl_CnfMsg->v_EmbeddedMinorVersion = vp_Info->v_EmbeddedMinorVersion;
        pl_CnfMsg->v_BBVersion = vp_Info->v_BBVersion;
        pl_CnfMsg->v_PatchVersion = vp_Info->v_PatchVersion;
        /*send message to MIS (TAT)*/
        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_MIS,0,CGPS_TAT_GET_INFO_CNF,(t_MsgHeader*)pl_CnfMsg);
        MC_RTK_FREE_MEMORY(vp_Info);
    }
    else
    {
        /*send message to MIS (TAT)*/
        pl_ErrMsg = (t_CGPS_TatGetInfoErr*)MC_RTK_GET_MEMORY(sizeof(*pl_ErrMsg));

        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_MIS,0,CGPS_TAT_GET_INFO_ERR,(t_MsgHeader*)pl_ErrMsg );
        MC_RTK_FREE_MEMORY(vp_Info);
    }

    return SAME;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/*****************************************************************************/
/* CGPS0_06TATGetLocation : request for navigation data                      */
/*****************************************************************************/
const t_OperationDescriptor* CGPS6_03TATGetLocation(t_RtkObject* p_FsmObject)
{
    t_CGPS_TatGetLocationCnf * pl_CnfMsg=NULL;
    t_CGPS_TatGetLocationErr * pl_ErrMsg=NULL;

    if (vg_Nav_Data.OS_Time_ms !=0)
    {
        pl_CnfMsg = (t_CGPS_TatGetLocationCnf*)MC_RTK_GET_MEMORY(sizeof(*pl_CnfMsg));

        //memcpy((uint8_t*)&(pl_CnfMsg->v_NavData),(uint8_t*)&vg_Nav_Data,sizeof(s_GN_GPS_Nav_Data));
        CGPS0_44CopyNavData( &(pl_CnfMsg->v_NavData) , &vg_Nav_Data );

        /*send confirmation message*/
        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_MIS,0,CGPS_TAT_GET_LOCATION_CNF,(t_MsgHeader*)pl_CnfMsg );
    }
    else
    {
        /*send error message*/
        pl_ErrMsg = (t_CGPS_TatGetLocationErr*)MC_RTK_GET_MEMORY(sizeof(*pl_ErrMsg));

        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_MIS,0,CGPS_TAT_GET_LOCATION_ERR,(t_MsgHeader*)pl_ErrMsg );
    }

    return SAME;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/*****************************************************************************/
/* CGPS0_07TATMode :                                                         */
/*****************************************************************************/
const t_OperationDescriptor* CGPS6_04TATMode(t_RtkObject* p_FsmObject)
{
    e_cgps_tat_mode vl_tat_mode;
    t_CGPS_TatGPSModeReq * pl_ReqMsg=NULL;
    t_CGPS_TatGPSModeCnf * pl_CnfMsg=NULL;

    pl_ReqMsg = (t_CGPS_TatGPSModeReq *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
    vl_tat_mode = pl_ReqMsg->v_ModeId;

    if ((vg_Tat_Status == CGPS_TAT_STATE_ACTIVATED) && (vg_TatTransparentMode == 0))
    {
        switch (pl_ReqMsg->v_ModeId)
        {
        case CGPS_TAT_NORMAL_MODE:
        {
            vl_tat_mode = CGPS6_06TATGoToNormalMode(vg_Tat_StateMode);
            break;
        }
        case CGPS_TAT_SLEEP_MODE:
        {
            vl_tat_mode = CGPS6_07TATGoToSleepMode(vg_Tat_StateMode);
            break;
        }
        case CGPS_TAT_COMA_MODE:
        {
            vl_tat_mode = CGPS6_08TATGoToComaMode(vg_Tat_StateMode);
            break;
        }
        case CGPS_TAT_DEEP_COMA_MODE:
        {
            /* FIXME :at this time deep coma is equal to OFF state*/
            vl_tat_mode = CGPS6_09TATGoToDeepComaMode(vg_Tat_StateMode);
            break;
        }
        }
    }

    pl_CnfMsg = (t_CGPS_TatGPSModeCnf*)MC_RTK_GET_MEMORY(sizeof(*pl_CnfMsg));
    pl_CnfMsg->v_ModeId = vl_tat_mode;
    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_MIS,0,CGPS_TAT_MODE_CNF,(t_MsgHeader*)pl_CnfMsg );

    return SAME;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/*****************************************************************************/
/* CGPS6_05TATTransparentMode :                                              */
/*****************************************************************************/
const t_OperationDescriptor* CGPS6_05TATTransparentMode(t_RtkObject* p_FsmObject)
{
    t_CGPS_TatTransparentModeCnf * pl_CnfMsg=NULL;
    t_CGPS_TatTransparentModeReq *pl_ReqMsg=NULL;
    
        pl_ReqMsg = (t_CGPS_TatTransparentModeReq *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
        vg_TatTransparentMode = pl_ReqMsg->v_EnableDisable;

    pl_CnfMsg = (t_CGPS_TatTransparentModeCnf*)MC_RTK_GET_MEMORY(sizeof(*pl_CnfMsg));
    pl_CnfMsg->v_EnableDisable = vg_TatTransparentMode;
    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_MIS,0,CGPS_TAT_TRANSPARENT_MODE_CNF ,(t_MsgHeader*)pl_CnfMsg );

    return SAME;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6
/***************************************************************************************/
/* CGPS0_31TATGoToNormalMode : If CGPS is in OFF state start CGPS (lib HALGPS and loop timer   */
/***************************************************************************************/
e_cgps_tat_mode CGPS6_06TATGoToNormalMode(e_cgps_tat_mode vp_state)
{
    e_cgps_tat_mode vl_retState= vp_state;
    e_cgps_Error vl_return;

    switch (vp_state)
    {
    case CGPS_TAT_SLEEP_MODE:
    {
        MC_RTK_STOP_TIMER(CGPS_TAT_SLEEP_TIMER);
        vl_retState = CGPS6_06TATGoToNormalMode(CGPS_TAT_COMA_MODE);
        break;
    }
    case CGPS_TAT_COMA_MODE:
    {
        GN_GPS_WakeUp();
        MC_HALGPS_SLEEP(FALSE);
        /* launch main loop*/
        MC_RTK_PROCESS_START_TIMER(CGPS_TAT_LOOP_TIMER, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_TAT_LOOP_TIMER_DURATION));

        vl_retState = CGPS_TAT_NORMAL_MODE;
        break;
    }
    case CGPS_TAT_DEEP_COMA_MODE:
    {
        /* start CGPS : init lib and start HALGPS*/
        vl_return = CGPS0_30StartLowLayerGPS();

        if (vl_return == K_CGPS_NO_ERROR)
        {
            /* launch main loop*/
            MC_RTK_PROCESS_START_TIMER(CGPS_TAT_LOOP_TIMER, MC_DIN_MILLISECONDS_TO_TICK(CGPS_TAT_SLEEP_TIMEOUT-1));
            vl_retState = CGPS_TAT_NORMAL_MODE;
        }
        break;
    }
    case CGPS_TAT_NORMAL_MODE:
    {
        vl_retState = CGPS_TAT_NORMAL_MODE;
        break;
    }
    default:
    {
        /*nothing to do*/
        break;
    }
    }
    return vl_retState;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7
/***************************************************************************************/
/* CGPS0_32TATGoToSleepMode : If CGPS is in OFF state start CGPS (lib HALGPS and loop timer   */
/***************************************************************************************/
e_cgps_tat_mode CGPS6_07TATGoToSleepMode(e_cgps_tat_mode vp_state)
{
    e_cgps_tat_mode vl_retState= vp_state;
    e_cgps_Error vl_return;

    switch (vp_state)
    {
    case CGPS_TAT_COMA_MODE:
    {
        /*go to off state*/
        MC_HALGPS_POWERDOWN();
        vl_retState = CGPS6_07TATGoToSleepMode(CGPS_TAT_DEEP_COMA_MODE);
        break;
    }
    case CGPS_TAT_DEEP_COMA_MODE:
    {
        /* go to running state*/
        vl_return = CGPS0_30StartLowLayerGPS();
        if (vl_return == K_CGPS_NO_ERROR)
        {
            /* launch main loop*/
            MC_RTK_PROCESS_START_TIMER(CGPS_TAT_LOOP_TIMER, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_TAT_LOOP_TIMER_DURATION));

            vl_retState = CGPS6_07TATGoToSleepMode(CGPS_TAT_NORMAL_MODE);
        }
        break;
    }
    case CGPS_TAT_NORMAL_MODE:
    {
        MC_RTK_STOP_TIMER(CGPS_TAT_LOOP_TIMER);
        /*first relaucnh timer*/
        MC_RTK_PROCESS_START_TIMER(CGPS_TAT_SLEEP_TIMER, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_TAT_LOOP_TIMER_DURATION));
        GN_GPS_Sleep(CGPS_TAT_SLEEP_TIMEOUT,0);
        MC_HALGPS_SLEEP(TRUE);
        /*FIXME launch timer to Go to sleep every CGPS_TAT_SLEEP_TIMEOUT second */
        vl_retState = CGPS_TAT_SLEEP_MODE;
        break;
    }
    case CGPS_TAT_SLEEP_MODE:
    {
        vl_retState = CGPS_TAT_SLEEP_MODE;
        break;
    }
    default:
    {
        /*generate an error*/
        break;
    }
    }
    return vl_retState;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8
/***************************************************************************************/
/* CGPS0_33TATGoToComaMode : If CGPS is in OFF state start CGPS (lib HALGPS and loop timer   */
/***************************************************************************************/
e_cgps_tat_mode CGPS6_08TATGoToComaMode(e_cgps_tat_mode vp_state)
{
    e_cgps_tat_mode vl_retState= vp_state;
    e_cgps_Error vl_return;

    switch (vp_state)
    {
    case CGPS_TAT_DEEP_COMA_MODE:
    {
        /* go to running state*/
        vl_return = CGPS0_30StartLowLayerGPS();
        if (vl_return == K_CGPS_NO_ERROR)
        {
            /* launch main loop*/
            MC_RTK_PROCESS_START_TIMER(CGPS_TAT_LOOP_TIMER, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_TAT_LOOP_TIMER_DURATION));

            vl_retState = CGPS6_08TATGoToComaMode(CGPS_TAT_NORMAL_MODE);
        }
        break;
    }
    case CGPS_TAT_NORMAL_MODE:
    {
        MC_RTK_STOP_TIMER(CGPS_TAT_LOOP_TIMER);
        GN_GPS_Sleep(0,CGPS_TAT_COMA_TIMEOUT);
        MC_HALGPS_SLEEP(TRUE);
        vl_retState = CGPS_TAT_COMA_MODE;
        break;
    }
    case CGPS_TAT_SLEEP_MODE:
    {
        MC_RTK_STOP_TIMER(CGPS_TAT_SLEEP_TIMER);
        GN_GPS_WakeUp();
        MC_HALGPS_SLEEP(FALSE);
        /* launch main loop*/
        MC_RTK_PROCESS_START_TIMER(CGPS_TAT_LOOP_TIMER, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_TAT_LOOP_TIMER_DURATION));
        vl_retState = CGPS6_08TATGoToComaMode(CGPS_TAT_NORMAL_MODE);
        break;
    }
    case CGPS_TAT_COMA_MODE:
    {
        vl_retState = CGPS_TAT_COMA_MODE;
        break;
    }
    default:
    {
        /*generate an error*/
        break;
    }
    }
    return vl_retState;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9
/***************************************************************************************/
/* CGPS0_34TATGoToDeepComaMode :  */
/***************************************************************************************/
e_cgps_tat_mode CGPS6_09TATGoToDeepComaMode(e_cgps_tat_mode vp_state)
{
    e_cgps_tat_mode vl_retState= vp_state;

    switch (vp_state)
    {
    case CGPS_TAT_NORMAL_MODE:
    {
        MC_RTK_STOP_TIMER(CGPS_TAT_LOOP_TIMER);
        MC_HALGPS_POWERDOWN();
        break;
    }
    case CGPS_TAT_SLEEP_MODE:
    {
        MC_RTK_STOP_TIMER(CGPS_TAT_SLEEP_TIMER);
        MC_HALGPS_POWERDOWN();
        break;
    }
    case CGPS_TAT_COMA_MODE:
    {
        MC_HALGPS_POWERDOWN();
        break;
    }
    case CGPS_TAT_DEEP_COMA_MODE:
    {
        vl_retState = CGPS_TAT_DEEP_COMA_MODE;
        break;
    }
    default:
    {
        /*generate an error*/
        break;
    }
    }
    return vl_retState;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10
/***************************************************************************************/
/* CGPS0_10SleepTimeExpired :  */
/***************************************************************************************/
const t_OperationDescriptor* CGPS6_10SleepTimeExpired(t_RtkObject* p_FsmObject)
{
    GN_GPS_WakeUp();
    MC_HALGPS_SLEEP(FALSE);
    MC_RTK_PROCESS_START_TIMER(CGPS_TAT_SLEEP_TIMER, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_TAT_LOOP_TIMER_DURATION));
    GN_GPS_Sleep(CGPS_TAT_SLEEP_TIMEOUT,0);
    MC_HALGPS_SLEEP(TRUE);
    return SAME;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11
/***************************************************************************************/
/* CGPS0_10SleepTimeExpired :  */
/***************************************************************************************/
const t_OperationDescriptor* CGPS6_11TATTransparentDataReq(t_RtkObject* p_FsmObject)
{
    t_CGPS_TatTransparentDataReq * pl_ReqMsg=NULL;
    t_CGPS_TatTransparentDataCnf * pl_CnfMsg=NULL;
    t_CGPS_TatTransparentDataErr * pl_ErrMsg=NULL;
    uint16_t vl_WriteLenght=0;

    pl_ReqMsg = (t_CGPS_TatTransparentDataReq *)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    vl_WriteLenght = GN_GPS_Write_GNB_Ctrl(pl_ReqMsg->v_Lenght,(char*)&pl_ReqMsg->v_Data);
    if (vl_WriteLenght == pl_ReqMsg->v_Lenght)
    {
        pl_CnfMsg = (t_CGPS_TatTransparentDataCnf*)MC_RTK_GET_MEMORY(sizeof(*pl_CnfMsg));
        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_MIS,0,CGPS_TAT_TRANSPARENT_DATA_CNF,(t_MsgHeader*)pl_CnfMsg );
    }
    else
    {
        pl_ErrMsg = (t_CGPS_TatTransparentDataErr*)MC_RTK_GET_MEMORY(sizeof(*pl_ErrMsg));
        pl_ErrMsg->v_Lenght= vl_WriteLenght;

        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_MIS,0,CGPS_TAT_TRANSPARENT_DATA_ERR,(t_MsgHeader*)pl_ErrMsg );
    }

    /* MC_RTK_FREE_MEMORY(pl_ReqMsg->v_Data); */
    return SAME;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 12
/***************************************************************************************/
/*  :  */
/***************************************************************************************/
const t_OperationDescriptor* CGPS6_12IniTat(void)
{
    t_CGPS_TatInitCnf  *pl_initInd;

    /* Allocate memory for response */
    pl_initInd = (t_CGPS_TatInitCnf *)MC_RTK_GET_MEMORY(sizeof(*pl_initInd));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS, 0,
                               PROCESS_CGPS, 0,
                               CGPS_TAT_INIT_CNF,
                               (t_MsgHeader *)pl_initInd);

    return SAME;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13
/***************************************************************************************/
/*  :  */
/***************************************************************************************/
const t_OperationDescriptor* CGPS6_13TATInitCnf(t_RtkObject* p_FsmObject)
{
    func_enter();

    func_exit();

    return SAME;
}


/* + LMSQC06481 : Production test */
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 13
/***************************************************************************************/
/*  :  */
/***************************************************************************************/
#ifdef CGPS_CNO_VALUE_FTR
const t_OperationDescriptor* CGPS6_14GetCnValueReq(t_RtkObject* p_FsmObject)
{
    uint16_t vl_Cno = 0;
    t_CGPS_TatGetCnValueReq *pl_TatGetCnValueReq;
    t_CGPS_TatGetCnValueCnf *pl_TatGetCnValueCnf = (t_CGPS_TatGetCnValueCnf*)MC_RTK_GET_MEMORY(sizeof(t_CGPS_TatGetCnValueCnf));

    vl_Cno = CGPS9_03GetCnoValue();

    if(vl_Cno > 0)
    {
        pl_TatGetCnValueReq = (t_CGPS_TatGetCnValueReq*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
        pl_TatGetCnValueCnf->v_CnValue = vl_Cno;
        pl_TatGetCnValueCnf->b_checkLimit = (vl_Cno >= (pl_TatGetCnValueReq->v_Limits.v_E9L))?TRUE:FALSE;
        MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,PROCESS_MIS,0,CGPS_TAT_GET_CN_VALUE_CNF,(t_MsgHeader*)pl_TatGetCnValueCnf );
    }

    return SAME;
}
#endif /* CGPS_CNO_VALUE_FTR */
/* - LMSQC06481 : Production test */

#endif /* #ifdef __RTK_E__ */

#undef __CGPS6TAT_C__
