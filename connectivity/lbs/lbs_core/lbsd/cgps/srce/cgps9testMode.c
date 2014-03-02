/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
* \file cgps9testMode.c
* \date 21/07/2008
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B>  This file contains all functions relative to the control plane\n
*
* <TABLE>
*     <TR>
*             <TD> Date</TD><TD> Author</TD><TD> Description</TD>
*     </TR>
*     <TR>
*             <TD> 21.07.08 </TD><TD> BALAJI NATAKALA</TD><TD> Creation </TD>
*     </TR>
* </TABLE>
*/

#define __CGPS9TESTMODE_C__


#ifdef AGPS_TEST_MODE_FTR

#include "cgpsTestMode.h"
#include "Baseband_Patch_748_SAT.c"
#include "Baseband_Patch_747_SAT.c"
#include "Baseband_Patch_923_SAT.c"

#undef MODULE_NUMBER
#define MODULE_NUMBER MODULE_CGPS

#undef PROCESS_NUMBER
#define PROCESS_NUMBER PROCESS_CGPS

#undef FILE_NUMBER
#define FILE_NUMBER 9

/* + RRR */
#ifdef ATT_LOGGING_ENABLE

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
/*****************************************************************************************************/
/* CGPS9_00HandleEvents :         */
/*****************************************************************************************************/
void CGPS9_00HandleEvents(t_CgpsEventInformation* pp_EventInfo)
{
    uint8_t   vl_CharactersWritten = 0;
    uint8_t   vl_CheckSum          = 0;
    char vl_NmeaBuffer[K_CGPS_MAX_NMEA_SIZE];
    /*++  For adding newline at the end of PSTE messages */
    char vl_PsteBuffer[K_CGPS_MAX_NMEA_SIZE];
    /*--  For adding newline at the end of PSTE messages */

/* + LMSQC19754 */
    /*The transview tool expects the PSTE messages to have a timestamp that is in resolution of 4ms*/
    uint32_t  vl_timer             = (uint32_t)GN_GPS_Get_OS_Time_ms() / 4;
/*    uint32_t  vl_timer             = GN_GPS_Get_OS_Time_ms(); */
/* - LMSQC19754 */

    vl_CharactersWritten = snprintf(vl_NmeaBuffer, K_CGPS_MAX_NMEA_SIZE, "$PSTE,");

    switch( pp_EventInfo->v_EventType )
    {
        case K_CGPS_RESPONSE_RETURNED:
            vl_CharactersWritten += MC_CGPS_ADD_PSTE_TYPE(vl_NmeaBuffer+vl_CharactersWritten, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten, 4);  /* Pste type */
            vl_CharactersWritten += MC_CGPS_ADD_OS_TIME  (vl_NmeaBuffer+vl_CharactersWritten, vl_timer, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);                /*OS time*/
            vl_CharactersWritten += MC_CGPS_ADD_SEPERATOR(vl_NmeaBuffer+vl_CharactersWritten, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);                          /* , */
            vl_CharactersWritten += MC_CGPS_ADD_LATLON(vl_NmeaBuffer+vl_CharactersWritten, pp_EventInfo->v_Latitude, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);/*Latitude*/
            vl_CharactersWritten += MC_CGPS_ADD_SEPERATOR(vl_NmeaBuffer+vl_CharactersWritten, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);                          /* , */
            vl_CharactersWritten += MC_CGPS_ADD_LATLON  (vl_NmeaBuffer+vl_CharactersWritten, pp_EventInfo->v_Longitude, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);/*Longitude*/
            vl_CharactersWritten += MC_CGPS_ADD_SEPERATOR(vl_NmeaBuffer+vl_CharactersWritten, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);                          /* , */
            vl_CharactersWritten += MC_CGPS_ADD_ALT  (vl_NmeaBuffer+vl_CharactersWritten, pp_EventInfo->v_Altitude, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);/*Altitude*/
            vl_CharactersWritten += MC_CGPS_ADD_EOM      (vl_NmeaBuffer+vl_CharactersWritten, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);                          /* * */
        break;

        case K_CGPS_POSITIONING_SESSION_START:
            vl_CharactersWritten += MC_CGPS_ADD_PSTE_TYPE(vl_NmeaBuffer+vl_CharactersWritten, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten, 1);
            vl_CharactersWritten += MC_CGPS_ADD_OS_TIME  (vl_NmeaBuffer+vl_CharactersWritten, vl_timer, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);
            vl_CharactersWritten += MC_CGPS_ADD_EOM      (vl_NmeaBuffer+vl_CharactersWritten, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);
        break;

        case K_CGPS_POSITIONING_SESSION_END:
            vl_CharactersWritten += MC_CGPS_ADD_PSTE_TYPE(vl_NmeaBuffer+vl_CharactersWritten, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten, 2);
            vl_CharactersWritten += MC_CGPS_ADD_OS_TIME  (vl_NmeaBuffer+vl_CharactersWritten, vl_timer, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);
            vl_CharactersWritten += MC_CGPS_ADD_EOM      (vl_NmeaBuffer+vl_CharactersWritten, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);
        break;

/* + LMSQC19754 */
        case K_CGPS_ABORT_SESSION:
            vl_CharactersWritten += MC_CGPS_ADD_PSTE_TYPE(vl_NmeaBuffer+vl_CharactersWritten, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten, 3);
            vl_CharactersWritten += MC_CGPS_ADD_OS_TIME  (vl_NmeaBuffer+vl_CharactersWritten, vl_timer, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);
            vl_CharactersWritten += MC_CGPS_ADD_EOM      (vl_NmeaBuffer+vl_CharactersWritten, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);
        break;
/* - LMSQC19754 */

        default:
        break;
    }

    vl_CheckSum = CGPS9_01GenerateCheckSum(vl_NmeaBuffer, strlen((const char*)vl_NmeaBuffer));

    vl_CharactersWritten += MC_CGPS_ADD_CS (vl_NmeaBuffer+vl_CharactersWritten, vl_CheckSum, K_CGPS_MAX_NMEA_SIZE-vl_CharactersWritten);

    MC_CGPS_TRACE(("%s", vl_NmeaBuffer));

    sprintf(vl_PsteBuffer,"%s\r\n",vl_NmeaBuffer);
/*--  For adding newline at the end of PSTE messages */
    if( vg_CgpsTestall.p_Callback != NULL  )
    {
        t_cgps_NavData vl_NavDataToSend;

        vl_NavDataToSend.v_Type    = K_CGPS_NMEA;
        vl_NavDataToSend.v_Length  = vl_CharactersWritten;
        vl_NavDataToSend.p_NavData = vl_NmeaBuffer;
        /*++  For adding newline at the end of PSTE messages */
        sprintf(vl_PsteBuffer,"%s\r\n",vl_NavDataToSend.p_NavData);
        vl_NavDataToSend.v_Length  += 2;
        vl_NavDataToSend.p_NavData = vl_PsteBuffer;
        MC_CGPS_TRACE(("In CGPS9_00HandleEvents calling p_Callback"));

        (vg_CgpsTestall.p_Callback)( vl_NavDataToSend );
    /*--  For adding newline at the end of PSTE messages */
    }
    /* + DUR 19 Jan 2011 NMEA Logging */
       CGPS4_11SendAcknowledge(vl_PsteBuffer);
    /* - DUR 19 Jan 2011 NMEA Logging */
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/*****************************************************************************************************/
/* CGPS9_01GenerateCheckSum :         */
/*****************************************************************************************************/
uint8_t CGPS9_01GenerateCheckSum(char* vp_NmeaBuffer, uint16_t vp_BufferLen)
{
    uint8_t  vl_Index     = 0;
    uint8_t  vl_CheckSum  = 0;

/* + LMSQC19754 */
/*  for ( vl_Index = 0 ; vl_Index < vp_BufferLen && vp_NmeaBuffer[vl_Index] != '*'; vl_Index++) */
    for ( vl_Index = 1 ; vl_Index < vp_BufferLen && vp_NmeaBuffer[vl_Index] != '*'; vl_Index++)
/* - LMSQC19754 */
    {
        vl_CheckSum ^= (unsigned char)vp_NmeaBuffer[vl_Index];
    }

    return vl_CheckSum;
}

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 02
/*****************************************************************************************************/
/* CGPS9_02PsteCallbackRegistrationReq :         */
/*****************************************************************************************************/
void CGPS9_02PsteCallbackRegistrationReq(t_cgps_PsteNmeaCallback vp_Callback)
{
    if( vp_Callback != NULL  )
    {
        MC_CGPS_TRACE(("CGPS0_21NotificationRegisterReq"));
        vg_PsteNmeaCallback = vp_Callback;
    }
}
#endif /*ATT_LOGGING_ENABLE*/
/* - RRR */


/* + LMSQC06481 : Production test */
#ifdef CGPS_CNO_VALUE_FTR
#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 03
/**************************************************************************/
/* CGPS9_03GetCnoValue : put the chip^intest mode to retrieve a register value then shutdown   */
/**************************************************************************/
uint16_t CGPS9_03GetCnoValue(void)
{
    char comd_code14[] = "#COMD 14 &C8\x0D\x0A#COMD 14 &C8\x0D\x0A#COMD 14 &C8\x0D\x0A#COMD 14 &C8\x0D\x0A#COMD 14 &C8\x0D\x0A";
    uint32_t  vl_readLen;
    uint32_t  vl_index;
    int8_t    a_readBuffer[4096];
    uint8_t   vl_attempt;
    bool vl_obtained      = FALSE;
    uint8_t   vl_COMD14attempt = 2;
    uint16_t  vl_Cno           = 0;

    /* ++ jaekuk80.lee 20090726 add variable for AT command */
    if ( vg_CgpsATcommandActivated == FALSE )
        /* start the chip */
        MC_HALGPS_QUICK_START();
    /* -- jaekuk80.lee 20090726 add variable for AT command */

    while(vl_COMD14attempt > 0 && vl_Cno < 15)
    {
        /* send the Command to request the CNO */
        GN_GPS_Write_GNB_Ctrl (sizeof(comd_code14) -  1, comd_code14);

/* + RRR */
    /* wait 50ms */
#ifdef __RTK_E__
#ifdef UTS_CPR
        MC_UTSER_SLEEP(UTSER_1MSEC_PRECISION,50);
#else
        MC_OST_START_TASK_TIMER(50);
#endif /* UTS_CPR */
#else
        OSA_Sleep( 50 );
#endif /* __RTK_E__ */
/* - RRR */

        vl_attempt = 5;

        while(!vl_obtained && vl_attempt > 0)
        {
            vl_readLen = GN_GPS_Read_GNB_Meas(4096, (char *)a_readBuffer);

            if(vl_readLen < 4096)
                a_readBuffer[vl_readLen] = 0;
            else
                a_readBuffer[4095] = 0;

            if( vl_readLen != 0 )
            {
                MC_CGPS_TRACE(("CGPS9_03GetCnoValue Read Data - <%s>" , a_readBuffer));
                for(vl_index=0; vl_index<vl_readLen-2 && vl_readLen > 0; vl_index++)
                {
                    if(a_readBuffer[vl_index]=='E' && a_readBuffer[vl_index+1]=='9'    && a_readBuffer[vl_index+2]==' ')
                    {
                        vl_Cno = 16*CHARTOINT(a_readBuffer[vl_index+3]) + CHARTOINT(a_readBuffer[vl_index+4]);
                        MC_CGPS_TRACE(("CGPS9_03GetCnoValue : Obtained CNo <%u>" , vl_Cno ));
                        vl_obtained = TRUE;
                    }
                }
            }
            else
            {
                MC_CGPS_TRACE(("CGPS9_03GetCnoValue No Data Read!" , a_readBuffer));
            }

/* + RRR */
/* wait 50ms */
#ifdef __RTK_E__
#ifdef UTS_CPR
            MC_UTSER_SLEEP(UTSER_1MSEC_PRECISION,50);
#else
            MC_OST_START_TASK_TIMER(50);
#endif /* UTS_CPR */
#else
            OSA_Sleep( 50 );
#endif /* __RTK_E__ */
/* - RRR */

            vl_attempt--;
        }

        vl_COMD14attempt--;

        /* if the chip doesn't answer, we must reset it */
        if(vl_attempt == 0 && !vl_obtained && vl_COMD14attempt > 0)
        {
            MC_HALGPS_QUICK_STOP();

            MC_HALGPS_QUICK_START();
        }
    }

    /* ++ jaekuk80.lee 20090726 add variable for AT command */
    if ( vg_CgpsATcommandActivated == FALSE )
    /* the chip must be reset after this operation */
    MC_HALGPS_QUICK_STOP();
    /* -- jaekuk80.lee 20090726 add variable for AT command */

    return vl_Cno;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 04
/**************************************************************************/
/* CGPS9_04HandleGetCnoMessage :    */
/**************************************************************************/
const t_OperationDescriptor* CGPS9_04HandleGetCnoMessage(t_RtkObject* p_FsmObject)
{
    uint16_t  vl_Cno = 0;
    bool vl_CheckLimit = FALSE;

    t_CGPS_GetCnoValueReq* pl_GetCnoValueReq = (t_CGPS_GetCnoValueReq*) MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    MC_CGPS_TRACE(("CGPS9_04HandleGetCnoMessage"));

    vl_Cno = CGPS9_03GetCnoValue();

    if(vl_Cno > 0)
    {
        vl_CheckLimit = (vl_Cno >= (pl_GetCnoValueReq->v_Criteria.v_E9L))?TRUE:FALSE;

        if( vg_ProductionTestCallback != NULL )
        {
            (vg_ProductionTestCallback)(vl_Cno, vl_CheckLimit);
        }
    }

    return (t_OperationDescriptor*)SAME;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 05
/**************************************************************************/
/* CGPS9_05StartProductionTest :    */
/**************************************************************************/
void CGPS9_05StartProductionTest( t_CGPS_CnoCriteria vp_CnoCriteria )
{
    t_CGPS_CnoCriteria *pl_CGPS_CnoCriteria=NULL;
    t_CGPS_GetCnoValueReq* pl_GetCnoValueReq=NULL ;
    pl_GetCnoValueReq = (t_CGPS_GetCnoValueReq*) MC_RTK_GET_MEMORY(sizeof(*pl_GetCnoValueReq));

    memcpy(&(pl_GetCnoValueReq->v_Criteria), &vp_CnoCriteria, sizeof(*pl_CGPS_CnoCriteria));

    MC_CGPS_TRACE(("CGPS9_05StartProductionTest"));

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_CGPS,0,
                               PROCESS_CGPS,0,
                               CGPS_GET_CNO_VALUE_REQ,
                               (t_MsgHeader*)pl_GetCnoValueReq );
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 06
/**************************************************************************/
/* CGPS9_06RegisterProductionTestCB :    */
/**************************************************************************/
void CGPS9_06RegisterProductionTestCB(t_cgps_ProductionTestCallback vp_Callback)
{
    MC_CGPS_TRACE(("CGPS0_21NotificationRegisterReq"));
    vg_ProductionTestCallback = vp_Callback;
}

#endif /* CGPS_CNO_VALUE_FTR */
/* - LMSQC06481 : Production test */





void CGPS9_23LogPosition (uint8_t vl_index, uint32_t Status_Code, s_GN_GPS_Nav_Data *pl_nav_data_to_use)
{
#if defined( CMCC_LOGGING_ENABLE ) && defined( AGPS_UP_FTR )
    int8_t Session_Start[20], Session_End[20];

    switch( Status_Code )
    {
        case CGPS_SUPL_CMCC_AGPS_SESSION_STARTED:
           sprintf( (char*)Session_Start , "%d" , 0 );
           GN_SUPL_Write_Event_Log_CMCC(CGPS_SUPL_CMCC_AGPS_SESSION_STARTED , Session_Start , "session starts");
        break;

        case CGPS_SUPL_CMCC_REPONSE_TIMEOUT_CODE:
            if (pl_nav_data_to_use != NULL)
            {
                if(pl_nav_data_to_use->Valid_SingleFix || pl_nav_data_to_use->Valid_SingleFix)
                {
                    /*we have a fix but don't reach the quality*/
                    sprintf( s_CmccLog, "%i, %04d%02d%02d%02d%02d%02d.%03d, %f, %f, %f, %f, # Position(Timestamp, lat, lon, orientation, height)  time to fix : %ld ms",
                        s_CgpsSupl[vl_index].v_GPSHandle, pl_nav_data_to_use->Year, pl_nav_data_to_use->Month,
                        pl_nav_data_to_use->Day, pl_nav_data_to_use->Hours, pl_nav_data_to_use->Minutes,
                        pl_nav_data_to_use->Seconds, pl_nav_data_to_use->Milliseconds,
                        pl_nav_data_to_use->Latitude, pl_nav_data_to_use->Longitude,
                        pl_nav_data_to_use->CourseOverGround, pl_nav_data_to_use->Altitude_MSL,
                        GN_GPS_Get_OS_Time_ms()-s_CgpsNaf[vl_index].v_RegisterTime);

                    GN_SUPL_Write_Event_Log_CMCC(CGPS_SUPL_CMCC_REPONSE_TIMEOUT_CODE, (char*)s_CmccLog, CGPS_SUPL_CMCC_POSITION_QUALITY_NOT_REACHED_STRING);
                }
                else
                {
                    /*no fix*/
                    snprintf( s_CmccLog,CGPS_SUPL_CMCC_STRING_MAX_LENGHT-1, "%i",s_CgpsSupl[vl_index].v_GPSHandle);
                    GN_SUPL_Write_Event_Log_CMCC(CGPS_SUPL_CMCC_REPONSE_TIMEOUT_CODE, (char*)s_CmccLog, CGPS_SUPL_CMCC_CANNOT_PRODUCE_POSITION_WITHIN_RESP_TIME_STRING);
                }
            }
            else
            {
                sprintf( s_CmccLog, "%i, %04d%02d%02d%02d%02d%02d.%03d, %f, %f, %f, %f, # Position(Timestamp, lat, lon, orientation, height)  time to fix : %ld ms",
                    s_CgpsSupl[vl_index].v_GPSHandle, vg_Nav_Data.Year, vg_Nav_Data.Month,
                    vg_Nav_Data.Day, vg_Nav_Data.Hours, vg_Nav_Data.Minutes,
                    vg_Nav_Data.Seconds, vg_Nav_Data.Milliseconds,
                    vg_Nav_Data.Latitude, vg_Nav_Data.Longitude,
                    vg_Nav_Data.CourseOverGround, vg_Nav_Data.Altitude_MSL,
                    GN_GPS_Get_OS_Time_ms()-s_CgpsSupl[vl_index].v_RegisterTime);
                GN_SUPL_Write_Event_Log_CMCC(CGPS_SUPL_CMCC_REPONSE_TIMEOUT_CODE, (char*)s_CmccLog, NULL);
            }
        break;

        case CGPS_SUPL_CMCC_POSITION_RESULT_CODE:
            if (pl_nav_data_to_use != NULL)
            {
                sprintf( s_CmccLog, "%i, %04d%02d%02d%02d%02d%02d.%03d, %f, %f, %f, %f, # Position(Timestamp, lat, lon, orientation, height)  time to fix : %ld ms",
                    s_CgpsSupl[vl_index].v_GPSHandle, pl_nav_data_to_use->Year, pl_nav_data_to_use->Month,
                    pl_nav_data_to_use->Day, pl_nav_data_to_use->Hours, pl_nav_data_to_use->Minutes,
                    pl_nav_data_to_use->Seconds, pl_nav_data_to_use->Milliseconds,
                    pl_nav_data_to_use->Latitude, pl_nav_data_to_use->Longitude,
                    pl_nav_data_to_use->CourseOverGround, pl_nav_data_to_use->Altitude_MSL,
                    GN_GPS_Get_OS_Time_ms()-s_CgpsNaf[vl_index].v_RegisterTime);

                GN_SUPL_Write_Event_Log_CMCC(CGPS_SUPL_CMCC_POSITION_RESULT_CODE, (char*)s_CmccLog, NULL);
            }
        break;

        case CGPS_SUPL_CMCC_NETWORKCONNECTION_CODE:
            snprintf(s_CmccLog, CGPS_SUPL_CMCC_STRING_MAX_LENGHT-1, "%i",s_CgpsSupl[vl_index].v_GPSHandle);
            GN_SUPL_Write_Event_Log_CMCC(CGPS_SUPL_CMCC_NETWORKCONNECTION_CODE,(char*)s_CmccLog,CGPS_SUPL_CMCC_NETWORKCONNECTION_STRING);
        break;

        case CGPS_SUPL_CMCC_NO_NETWORKCONNECTION_CODE:
            if (vl_index == 0xFF)
            {
                snprintf(s_CmccLog, CGPS_SUPL_CMCC_STRING_MAX_LENGHT-1, "%x", 0xFFFFFFFF);
                GN_SUPL_Write_Event_Log_CMCC(CGPS_SUPL_CMCC_NO_NETWORKCONNECTION_CODE, (char*)s_CmccLog, CGPS_SUPL_CMCC_AUTONOMOUS_SET_STRING);
            }
            else
            {
                snprintf(s_CmccLog, CGPS_SUPL_CMCC_STRING_MAX_LENGHT-1,"%i",s_CgpsSupl[vl_index].v_GPSHandle);
                GN_SUPL_Write_Event_Log_CMCC(CGPS_SUPL_CMCC_NO_NETWORKCONNECTION_CODE, (char*)s_CmccLog, CGPS_SUPL_CMCC_NO_NETWORKCONNECTION_STRING);
            }
        break;

        case CGPS_SUPL_CMCC_NETWORKCONNECTION_FAILURE_CODE:
            snprintf(s_CmccLog, CGPS_SUPL_CMCC_STRING_MAX_LENGHT-1, "%i",s_CgpsSupl[vl_index].v_GPSHandle);
            GN_SUPL_Write_Event_Log_CMCC(CGPS_SUPL_CMCC_NETWORKCONNECTION_FAILURE_CODE, (char*)s_CmccLog, CGPS_SUPL_CMCC_NETWORKCONNECTION_FAILURE_STRING);
        break;

        case CGPS_SUPL_CMCC_SERVERCONNECTION_CODE:
            snprintf(s_CmccLog, CGPS_SUPL_CMCC_STRING_MAX_LENGHT-1, "%i",s_CgpsSupl[vl_index].v_GPSHandle);
            GN_SUPL_Write_Event_Log_CMCC(CGPS_SUPL_CMCC_SERVERCONNECTION_CODE,(char*)s_CmccLog,CGPS_SUPL_CMCC_SERVERCONNECTION_STRING);
        break;

        case CGPS_SUPL_CMCC_SERVERCONNECTION_FAILURE_CODE:
            snprintf(s_CmccLog, CGPS_SUPL_CMCC_STRING_MAX_LENGHT-1, "%i",s_CgpsSupl[vl_index].v_GPSHandle);
            GN_SUPL_Write_Event_Log_CMCC(CGPS_SUPL_CMCC_SERVERCONNECTION_FAILURE_CODE,(char*)s_CmccLog,CGPS_SUPL_CMCC_SERVERCONNECTION_FAILURE_STRING);
        break;

        case CGPS_SUPL_CMCC_AGPS_SESSION_ENDED:
           sprintf( (char*)Session_End , "%d" , 0 );
           GN_SUPL_Write_Event_Log_CMCC(CGPS_SUPL_CMCC_AGPS_SESSION_STARTED , Session_End , "session ends");
        break;
        default:

        break;
    };
#else
    vl_index           = vl_index;
    Status_Code        = Status_Code;
    pl_nav_data_to_use = pl_nav_data_to_use;
#endif
}



/**
* \fn U2 CGPS9_10SetupSatMEPatch( U2 ROM_version, U2 Patch_CkSum )
*/
U2 CGPS9_10SetupSatMEPatch( U2 ROM_version, U2 Patch_CkSum )
{

    /* Save the GPS baseband ROM verion for GN_Upload_GNB_Patch */
    gn_ROM_version = ROM_version;

    MC_CGPS_TRACE(("CGPS9_10SetupSatMEPatch: ROM=%d CkSum=0x%04X vg_PatchVersion=0x%04X, gn_Patch_Status=%d",
                                         ROM_version, Patch_CkSum, vg_PatchVersion, gn_Patch_Status));

    /* Save the GPS baseband ROM patch version (ie its checksum) for TAT */
    switch ( gn_ROM_version )
    {
    case 747:
    {
        vg_PatchVersion = PatchCheckSum_747_SAT;
        break;
    }
    case 748:
    {
        vg_PatchVersion = PatchCheckSum_748_SAT;
        break;
    }

    case 923:
    {
        vg_PatchVersion = PatchCheckSum_923_SAT;
        break;
    }

    case 713:
        /* Support for CG2900 PG1 chip is removed  */
    default :
        /* Check that we have patch code available for this ROM version*/
        MC_DIN_ERROR( BLOCKING, ERROR_1 );
    }



    /* Check that the patch available is not already uploaded correctly*/
    if (( vg_PatchVersion == Patch_CkSum ) && (gn_Patch_Force_Upload != 1) )
    {
        /*GN_Setup_GNB_Patch: Patch available and already in use*/
        return( vg_PatchVersion );
    }

    /* Set the patch status to indicate that the patch upload should start*/
    gn_Patch_Status   = 1;
    gn_Patch_Progress = 0;
    gn_Cur_Mess[0]    = 0;
    gn_Cur_Mess[1]    = 0;
    gn_Cur_Mess[2]    = 0;
    gn_Cur_Mess[3]    = 0;
    gn_Cur_Mess[4]    = 0;
    gn_Patch_Force_Upload = 0;

    MC_CGPS_TRACE(("2 CGPS9_10SetupSatMEPatch: ROM=%d CkSum=0x%04X vg_PatchVersion=0x%04X, gn_Patch_Status=%d",
                                         ROM_version, Patch_CkSum, vg_PatchVersion, gn_Patch_Status));

    return( vg_PatchVersion );
}


/**
* \fn void CGPS9_11UploadSatMEPatchCG29xx( U4 Max_Num_Patch_Mess )
*/
void CGPS9_11UploadSatMEPatchCG29xx( U4 Max_Num_Patch_Mess )
{

    MC_CGPS_TRACE(("CGPS9_11UploadSatMEPatchCG29xx: Max_Num_Patch_Mess=%d gn_ROM_version=%d, gn_Patch_Status=%d", Max_Num_Patch_Mess, gn_ROM_version, gn_Patch_Status));

    /* Select the appropriate Patch Upload function for the ROM_version
       identified and saved in GN_Setup_GNB_Patch() */

    switch ( gn_ROM_version )
    {
        case 747:
        {
            CGPS2_09UploadMEPatchCG29xx(Max_Num_Patch_Mess, vg_CG2900_Patch_747_SAT , sizeof(vg_CG2900_Patch_747_SAT));
            MC_CGPS_TRACE(("Patch download Production test Patch_Mess %d, size %d \n", Max_Num_Patch_Mess, sizeof(vg_CG2900_Patch_747_SAT)));
            break;
        }
        case 748:
        {
            CGPS2_09UploadMEPatchCG29xx(Max_Num_Patch_Mess, vg_CG2900_Patch_748_SAT, sizeof(vg_CG2900_Patch_748_SAT));
            MC_CGPS_TRACE(("Patch download Production test Patch_Mess %d, size %d \n", Max_Num_Patch_Mess, sizeof(vg_CG2900_Patch_748_SAT)));
            break;
        }
        case 923:
        {
            CGPS2_09UploadMEPatchCG29xx(Max_Num_Patch_Mess, vg_CG2900_Patch_923_SAT , sizeof(vg_CG2900_Patch_923_SAT));
            MC_CGPS_TRACE(("Patch download Production test Patch_Mess %d, size %d \n", Max_Num_Patch_Mess, sizeof(vg_CG2900_Patch_923_SAT)));
            break;
        }

        case 713:
            /* Support for CG2900 PG1 chip is removed  */
        default :
        {
            MC_CGPS_TRACE(("CGPS9_11UploadSatMEPatchCG29xx Not prepared ROM %d\n", gn_ROM_version));
            MC_DIN_ERROR( BLOCKING, ERROR_1 );
        }
    }

    return;
}

/*************************************************************************
*
* Name            CalcCheckSum
*
* Parameters:    pp_Message pointer to the message
*
* Returns:       None.
*
* Description:   Calculate a checksum to the string upto the '&' character.
*                The checksum does not include the first character (which is a '#').
*
*************************************************************************/
static uint8_t TREQ_CalcCheckSum(const int8_t* pp_Message)
{
    uint8_t CheckSum = 0;   // Checksum counter.
    int i = 1;  // Don't include the # in the checksum calculation

    do
    {
        CheckSum += (U1) pp_Message[i++];
    } while ( pp_Message[i] != '&' );

    return CheckSum;
}

/*************************************************************************
*
* Name           AddMessageTail
*
* Parameters:
*                Message_p       pointer to start odf message.
*                MessageInsert_p pointer to end of message, where the tail is to be added.
*
* Returns:       Pointer to terminating nul.
*
* Description:   Add the '&' character the check sum and the '\n'.
*                The checksum does not include the first character (which is a '#').
*
*************************************************************************/
char * TREQ_AddMessageTail (const int8_t* const Message_p, char *MessageInsert_p)
{
  *MessageInsert_p++ = '&';

  sprintf (MessageInsert_p, "%02X", TREQ_CalcCheckSum(Message_p));
  // Skip the two character checksum.
  MessageInsert_p++ ;
  MessageInsert_p++ ;

  // Teminating new line (it is required !)
  *MessageInsert_p++ = '\n';

  *MessageInsert_p++ = '\r';

  // null terminate the string for safety.
  *MessageInsert_p = 0;

  return MessageInsert_p;
}


/*****************************************************************************/
/* CGPS9_30ProductionTestStartReq : ProductionTest Start Request  function              */
/*****************************************************************************/
uint8_t vg_WaitTimer;
uint16_t vg_ReadCount;
uint8_t vg_ReTry;

const t_OperationDescriptor* CGPS9_30ProductionTestStartReq(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = a_CGPSTATState;

    MC_CGPS_TRACE(("START CGPS9_30ProductionTestStartReq"));

    uint32_t v_ROMVer;

    t_CGPS_ProdTestStartReq *pl_ProdStartReq = NULL;

    pl_ProdStartReq = (t_CGPS_ProdTestStartReq*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    MC_CGPS_TRACE(("state %d, vg_TREQ_Status : %d ",pl_ProdStartReq->v_State, vg_TREQ_Status));

    if ((e_cgps_TREQstate)pl_ProdStartReq->v_State == CGPS_TREQ_STATE_START)
    {
        pl_NextState = a_CGPSTATState;
        vg_WaitTimer = 0;
        vg_TREQTestMask = pl_ProdStartReq->v_TestMask;
        memcpy(&vg_TREQ_TestRequest, pl_ProdStartReq->p_ProdTestReq, sizeof(t_CGPS_TestRequest));
        MC_CGPS_TRACE(("state %d, vg_TREQ_Status : %d ",pl_ProdStartReq->v_State, vg_TREQ_Status));

        if ( vg_TREQ_Status <= CGPS_TREQ_STATE_PATCH_DONE )
        {
            MC_RTK_PROCESS_START_TIMER(CGPS_TREQ_LOOP_TIMER, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_TREQ_LOOP_TIMER_DURATION));
            if ((vg_TREQ_Status & CGPS_TREQ_STATE_PATCH_DONE) == 0)
            {

                v_ROMVer = CGPS9_32CheckROMVersion();

                if (v_ROMVer != 0)
                {
                    {
                        int8_t vl_TempData[CGPS_TREQ_READDATA_MAX_LENGTH];
                        uint32_t vl_ReadLen;
                        do
                        {
                            vl_ReadLen = GN_GPS_Read_GNB_Meas(CGPS_TREQ_READDATA_MAX_LENGTH, (char*)&vl_TempData);
                            if (vl_ReadLen > 0)
                            {
                                if (vl_ReadLen >= 1000)
                                {
                                    int8_t *newStr;
                                    newStr = (int8_t*)strtok((char*)vl_TempData, "\r\n");
                                    while (newStr != NULL)
                                    {
                                        vl_ReadLen = strlen((const char*)newStr);
                                        MC_CGPS_TRACE(("#2 Kenny Read_GNB_Meas [%d] Len<%d>\n< %s >\n",vl_ReadLen, newStr));
                                        newStr = (int8_t*)strtok(NULL, "\r\n");
                                    }
                                }
                                else
                                {
                                    MC_CGPS_TRACE(("2 Kenny Read_GNB_Meas [%d] %s", vl_ReadLen, vl_TempData));
                                }
                            }
                        } while( vl_ReadLen != 0 );
                    }

                    CGPS9_10SetupSatMEPatch(v_ROMVer, 0x0000);
                    /* launch main loop*/
                    vg_TREQ_Status |= CGPS_TREQ_STATE_DOWNLOAD;
                }
                else
                {
                    pl_NextState = SAME;
                }
            }
        }

        if (vg_TREQ_Status >= CGPS_TREQ_STATE_START)
        {
            MC_CGPS_TRACE(("Doing Before Test Waiting"));
        }
        vg_TREQ_Status |= CGPS_TREQ_STATE_START;
    }

    MC_CGPS_TRACE(("state %d, vg_TREQ_Status : %d ",pl_ProdStartReq->v_State, vg_TREQ_Status));

    MC_RTK_FREE_MEMORY(pl_ProdStartReq->p_ProdTestReq);

    MC_CGPS_TRACE(("END CGPS9_30ProductionTestStartReq"));

    return pl_NextState;
}


/*****************************************************************************/
/* CGPS9_31ProductionTestStopReq : ProductionTest Stop Request  function              */
/*****************************************************************************/

const t_OperationDescriptor* CGPS9_31ProductionTestStopReq(t_RtkObject* p_FsmObject)
{
    MC_CGPS_TRACE(("START CGPS9_31ProductionTestStopReq"));
    const t_OperationDescriptor* pl_NextState = SAME;

    t_CGPS_ProdTestStopReq *pl_ProdStopReq = NULL;

    pl_ProdStopReq = (t_CGPS_ProdTestStopReq*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    CGPS9_34StopProduction();

    pl_NextState = a_CGPSInitState;

    MC_CGPS_TRACE(("END CGPS9_31ProductionTestStopReq"));

    return pl_NextState;
}

uint32_t CGPS9_32CheckROMVersion(void)
{
    int8_t vl_TempData[CGPS_TREQ_READDATA_MAX_LENGTH];
    int8_t vl_Message[CGPS_TREQ_READDATA_MAX_LENGTH];
    uint32_t vl_MsgLen;
    uint32_t  v_OStime;
    uint32_t vl_ReadLen;
    bool vb_STAT = FALSE;
    uint32_t v_ROMversion = 0;
    bool vl_return;

    MC_CGPS_TRACE(("START CGPS9_32CheckROMVersion"));

    MC_HALGPS_SLEEP( FALSE );
    vl_return = GN_GPS_Hard_Reset_GNB();

    if (vl_return == TRUE)
    {
        v_OStime = GN_GPS_Get_OS_Time_ms();
        sprintf( (char*)vl_Message ,"#WAKE %d [WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP] ", v_OStime);
        TREQ_AddMessageTail(vl_Message, strchr((const char*)vl_Message, '\0'));

        vl_MsgLen = strlen((const char*)vl_Message);
        MC_CGPS_TRACE(("1 WAKEUP Message %s Len %d\n",vl_Message, vl_MsgLen));
        OSA_Sleep( 200 );
        GN_GPS_Write_GNB_Ctrl(vl_MsgLen, (char*)vl_Message);

        do
        {
            vl_ReadLen = GN_GPS_Read_GNB_Meas(CGPS_TREQ_READDATA_MAX_LENGTH, (char*)&vl_TempData);
            if (vl_ReadLen > 0)
            {
                MC_CGPS_TRACE(("Check to ROM version [%d] %s", vl_ReadLen, vl_TempData));
                if (strncmp( (const char*)vl_TempData ,"#STAT", 5) == 0)
                {
                    int8_t   *newStr;
                    uint16_t nLoopCount = 0;
                    newStr =  (int8_t*)strtok((char*)vl_TempData, " ");
                    while (newStr != NULL)
                    {
                        nLoopCount++;
                        if (nLoopCount == 3)
                        {
                            sprintf((char*)vl_Message, "%s", newStr);
                        }
                        newStr = (int8_t*)strtok(NULL, " ");
                    }

                    if (strlen((const char*)vl_Message) > 0)
                        v_ROMversion = (uint32_t)atoi((char*)vl_Message);
                    else
                        v_ROMversion = 0;

                    vb_STAT = TRUE;
                }
                else
                {
                    vb_STAT = FALSE;
                }
            }
            else
            {
                v_OStime = GN_GPS_Get_OS_Time_ms();
                sprintf( (char*)vl_Message ,"#WAKE %d [WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP][WAKE-UP] ", v_OStime);
                TREQ_AddMessageTail(vl_Message, strchr((const char*)vl_Message, '\0'));
                vl_MsgLen = strlen((const char*)vl_Message);
                MC_CGPS_TRACE(("2 WAKEUP Message %s Len %d\n",vl_Message, vl_MsgLen));
                OSA_Sleep( 200 );
                GN_GPS_Write_GNB_Ctrl(vl_MsgLen, (char*)vl_Message);
            }
        } while( vb_STAT != TRUE );
    }
    else
    {
        v_ROMversion = 0;
    }

    MC_CGPS_TRACE(("END CGPS9_32CheckROMVersion ROMVer %d", v_ROMversion));

    return v_ROMversion;
}



/*****************************************************************************/
/* CGPS9_33StartProduction : ProductionTest Start function                                          */
/*****************************************************************************/

void  CGPS9_33StartProduction(uint16_t v_TestMask, t_CGPS_TestRequest *p_cgpsTREQ_Req)
{
    MC_CGPS_TRACE(("START CGPS9_33StartProduction"));

    vg_ReadCount = 1;
    vg_TREQ_Status &= 0x0001;
    vg_TREQ_Status |= CGPS_TREQ_STATE_MEASURE;

    switch(v_TestMask)
    {
        case CGPS_TREQ_SNR:
        {
            MC_CGPS_TRACE(("CGPS_TREQ_SNR"));
            CGPS9_35ProductionTREQ_InputSignalSNR(p_cgpsTREQ_Req);
        }
        break;
        case CGPS_TREQ_ME:
        {
            MC_CGPS_TRACE(("CGPS_TREQ_ME"));
            CGPS9_36ProductionTREQ_ME(p_cgpsTREQ_Req);
        }
        break;
        case CGPS_TREQ_RTC:
        {
            MC_CGPS_TRACE(("CGPS_TREQ_RTC"));
            CGPS9_37ProductionTREQ_RTC(p_cgpsTREQ_Req);
        }
        break;
        case CGPS_TREQ_INPIN:
        {
            MC_CGPS_TRACE(("CGPS_TREQ_INPIN"));
            CGPS9_38ProductionTREQ_InputPin(p_cgpsTREQ_Req);
        }
        break;
        case CGPS_TREQ_OUTPIN:
        {
            MC_CGPS_TRACE(("CGPS_TREQ_OUTPIN"));
            CGPS9_39ProductionTREQ_Outputpin(p_cgpsTREQ_Req);
        }
        break;
        case CGPS_TREQ_ROMCHKSUM:
        {
            MC_CGPS_TRACE(("CGPS_TREQ_ROMCHKSUM"));
            CGPS9_40ProductionTREQ_ROMChecksum(p_cgpsTREQ_Req);
        }
        break;
        case CGPS_TREQ_PATCHCHKSUM:
        {
            MC_CGPS_TRACE(("CGPS_TREQ_PATCHCHKSUM"));
            CGPS9_41ProductionTREQ_PatchChecksum(p_cgpsTREQ_Req);
        }
        break;
        case CGPS_TREQ_BIST:
        {
            MC_CGPS_TRACE(("CGPS_TREQ_BIST"));
           CGPS9_42ProductionTREQ_BIST(p_cgpsTREQ_Req);
        }
        break;
        case CGPS_TREQ_SPECANALYZ:
        {
            MC_CGPS_TRACE(("CGPS_TREQ_SPECANALYZ Freq %s SATCmdType %d",p_cgpsTREQ_Req->v_InputSigFreq, p_cgpsTREQ_Req->v_SpectrumTestType));
            CGPS9_43ProductionTREQ_SpectrumAnalyzer(p_cgpsTREQ_Req);
        }
        break;
    }



    MC_CGPS_TRACE(("END CGPS9_33StartProduction"));
}

/*****************************************************************************/
/* CGPS9_34StopProduction : ProductionTest Stop function                                          */
/*****************************************************************************/
void CGPS9_34StopProduction( )
{
    MC_CGPS_TRACE(("START CGPS9_34StopProduction"));

    vg_TREQ_Status = CGPS_TREQ_STATE_NOT_INITIATE;

    MC_RTK_STOP_TIMER(CGPS_TREQ_LOOP_TIMER);

    MC_HALGPS_SLEEP( TRUE );
    gn_Patch_Status = 0;

    MC_CGPS_TRACE(("END CGPS9_34StopProduction"));
}


/*****************************************************************************/
/*TBD CGPS9_35ProductionTREQ_InputSignalSNR :Input signal SNR test function            */
/*****************************************************************************/

void CGPS9_35ProductionTREQ_InputSignalSNR(t_CGPS_TestRequest *p_cgpsTREQ_Req)
{
    MC_CGPS_TRACE(("CGPS9_35ProductionTREQ_InputSignalSNR"));
    p_cgpsTREQ_Req = p_cgpsTREQ_Req;
}


/*****************************************************************************/
/*TBD CGPS9_36ProductionTREQ_ME : ME oscillator offset test function                    */
/*****************************************************************************/
void CGPS9_36ProductionTREQ_ME(t_CGPS_TestRequest *p_cgpsTREQ_Req)
{
    MC_CGPS_TRACE(("CGPS9_36ProductionTREQ_ME"));
    p_cgpsTREQ_Req = p_cgpsTREQ_Req;
}


/*****************************************************************************/
/*TBD CGPS9_37ProductionTREQ_RTC : RTC oscillator test function                        */
/*****************************************************************************/

void CGPS9_37ProductionTREQ_RTC(t_CGPS_TestRequest *p_cgpsTREQ_Req)
{
    MC_CGPS_TRACE(("CGPS9_37ProductionTREQ_RTC"));
    p_cgpsTREQ_Req = p_cgpsTREQ_Req;
}


/*****************************************************************************/
/*TBD CGPS9_38ProductionTREQ_InputPin :Input pin test function                        */
/*****************************************************************************/

void CGPS9_38ProductionTREQ_InputPin(t_CGPS_TestRequest *p_cgpsTREQ_Req)
{
    MC_CGPS_TRACE(("CGPS9_38ProductionTREQ_InputPin"));
    p_cgpsTREQ_Req = p_cgpsTREQ_Req;
}


/*****************************************************************************/
/*TBD CGPS9_39ProductionTREQ_Outputpin : Output pin test function                    */
/*****************************************************************************/
void CGPS9_39ProductionTREQ_Outputpin(t_CGPS_TestRequest *p_cgpsTREQ_Req)
{
    MC_CGPS_TRACE(("CGPS9_39ProductionTREQ_Outputpin"));
    p_cgpsTREQ_Req = p_cgpsTREQ_Req;
}


/*****************************************************************************/
/*TBD CGPS9_40ProductionTREQ_ROMChecksum : ROM check sum test function            */
/*****************************************************************************/
void CGPS9_40ProductionTREQ_ROMChecksum(t_CGPS_TestRequest *p_cgpsTREQ_Req)
{
    MC_CGPS_TRACE(("CGPS9_40ProductionTREQ_ROMChecksum"));
    p_cgpsTREQ_Req = p_cgpsTREQ_Req;
}


/*****************************************************************************/
/*TBD CGPS9_41ProductionTREQ_PatchChecksum : Patch Check sum test function        */
/*****************************************************************************/
void CGPS9_41ProductionTREQ_PatchChecksum(t_CGPS_TestRequest *p_cgpsTREQ_Req)
{
    MC_CGPS_TRACE(("CGPS9_41ProductionTREQ_PatchChecksum"));
    p_cgpsTREQ_Req = p_cgpsTREQ_Req;
}


/*****************************************************************************/
/*TBD CGPS9_42ProductionTREQ_BIST : BIST test function                            */
/*****************************************************************************/
void CGPS9_42ProductionTREQ_BIST(t_CGPS_TestRequest *p_cgpsTREQ_Req)
{
    MC_CGPS_TRACE(("CGPS9_42ProductionTREQ_BIST"));
    p_cgpsTREQ_Req = p_cgpsTREQ_Req;
}


/*****************************************************************************/
/* CGPS9_43ProductionTREQ_SpectrumAnalyzer : Spectrum Analyzer function            */
/*****************************************************************************/

void CGPS9_43ProductionTREQ_SpectrumAnalyzer(t_CGPS_TestRequest *p_cgpsTREQ_Req)
{
    int8_t Message[CGPS_MAX_TREQ_MSG_LEN];
    uint16_t Size=0;

    MC_CGPS_TRACE(("START CGPS9_43ProductionTREQ_SpectrumAnalyzer"));

    if((p_cgpsTREQ_Req->v_SpectrumTestType == CGPS_READ_PGA_AGC_VALUE) || (p_cgpsTREQ_Req->v_SpectrumTestType == CGPS_RESET_RF))
        snprintf( (char*)Message ,CGPS_MAX_TREQ_MSG_LEN, "#TREQ 9 %1d ", p_cgpsTREQ_Req->v_SpectrumTestType);
    else if(p_cgpsTREQ_Req->v_SpectrumTestType == CGPS_SET_PGA_AGC_VALUE)
        snprintf( (char*)Message ,CGPS_MAX_TREQ_MSG_LEN, "#TREQ 9 %1d %s ", p_cgpsTREQ_Req->v_SpectrumTestType, p_cgpsTREQ_Req->v_PgaValues);
    else
        snprintf((char*)Message ,CGPS_MAX_TREQ_MSG_LEN, "#TREQ 9 %1d %s ", p_cgpsTREQ_Req->v_SpectrumTestType, p_cgpsTREQ_Req->v_InputSigFreq);

    (void)TREQ_AddMessageTail(Message, strchr((const char*)Message, '\0'));

    Size = strlen((const char*)Message);

    MC_CGPS_TRACE(("Message %s Len %d\n",Message, Size));

    (void)GN_GPS_Write_GNB_Ctrl(Size, (char*)Message);

    if((p_cgpsTREQ_Req->v_SpectrumTestType == CGPS_SET_PGA_AGC_VALUE) || (p_cgpsTREQ_Req->v_SpectrumTestType == CGPS_RESET_RF))
    {
        MC_CGPS_TRACE(("CGPS9_43 Not expecting a response. Set ReadCount = 2"));
        vg_ReadCount = CGPS_MIN_TRES_RESP_RECVD;
    }

    MC_CGPS_TRACE(("END CGPS9_43ProductionTREQ_SpectrumAnalyzer"));

}

/*****************************************************************************/
/* CGPS9_44TREQTimerExpired : ProductionTest Timer function                                      */
/*****************************************************************************/
const t_OperationDescriptor* CGPS9_44TREQTimerExpired(t_RtkObject* p_FsmObject)
{
    MC_CGPS_TRACE(("START CGPS9_44TREQTimerExpired TREQStatus[%d]", vg_TREQ_Status));
    p_FsmObject = p_FsmObject;

    const t_OperationDescriptor* pl_NextState = SAME;
    uint32_t v_ReadLen = 0;

    if ( vg_TREQ_Status > CGPS_TREQ_STATE_PATCH_DONE )
    {
        MC_RTK_PROCESS_START_TIMER(CGPS_TREQ_LOOP_TIMER, MC_DIN_MILLISECONDS_TO_TICK(K_CGPS_TREQ_LOOP_TIMER_DURATION));
    }

    if (((vg_TREQ_Status & CGPS_TREQ_STATE_START) > 0) && ((vg_TREQ_Status & CGPS_TREQ_STATE_MEASURE) == 0))
    {
        if ((vg_TREQ_Status & CGPS_TREQ_STATE_DOWNLOAD) > 0 &&
            (vg_TREQ_Status & CGPS_TREQ_STATE_PATCH_DONE) == 0 )
        {
            {
                int8_t vl_TempData[CGPS_TREQ_READDATA_MAX_LENGTH];
                uint32_t vl_ReadLen;

                vl_ReadLen = GN_GPS_Read_GNB_Meas(CGPS_TREQ_READDATA_MAX_LENGTH, (char*)&vl_TempData);
            }

            if( gn_Patch_Status > 0  &&  gn_Patch_Status < 7 )     /* Patching in progress*/
            {
                CGPS9_11UploadSatMEPatchCG29xx( K_CGPS_PATCH_SEGMENT_LENGTH );
            }

            MC_CGPS_TRACE(("TREQStatus[%d] gn_Patch_Status[%d]", vg_TREQ_Status, gn_Patch_Status));

            if (gn_Patch_Status == 7)
            {
                {
                    int8_t vl_TempData[CGPS_TREQ_READDATA_MAX_LENGTH];
                    uint32_t vl_ReadLen;
                    do
                    {
                        vl_ReadLen = GN_GPS_Read_GNB_Meas(CGPS_TREQ_READDATA_MAX_LENGTH, (char*)&vl_TempData);

                    } while( vl_ReadLen != 0 );
                }
                if ((vg_TREQ_Status & CGPS_TREQ_STATE_2ND_DOWNLOAD) > 0)
                {
                    int8_t   vl_Temp[1024];
                    uint32_t vl_Len;

                    vg_TREQ_Status |= CGPS_TREQ_STATE_PATCH_DONE;
                    vg_WaitTimer = 0;

                    sprintf( (char*)vl_Temp ,"#COMD 41 1 0 ");

                    TREQ_AddMessageTail(vl_Temp, strchr((const char*)vl_Temp, '\0'));
                    vl_Len = strlen((const char*)vl_Temp);
                    MC_CGPS_TRACE(("LNA Enable %s Len %d\n",vl_Temp, vl_Len));
                    OSA_Sleep( 200 );
                    GN_GPS_Write_GNB_Ctrl(vl_Len, (char*)vl_Temp);
                }
                else
                {
                    gn_Patch_Status = 1;
                    vg_TREQ_Status |= CGPS_TREQ_STATE_2ND_DOWNLOAD;
                }
            }


        }

        MC_CGPS_TRACE(("vg_WaitTimer[%d]", vg_WaitTimer));

        if ((vg_TREQ_Status & CGPS_TREQ_STATE_PATCH_DONE) > 0 )
        {
            if (vg_WaitTimer >= 3 && gn_Patch_Status == 7)
            {
                {
                    int8_t vl_TempData[CGPS_TREQ_READDATA_MAX_LENGTH];
                    uint32_t vl_ReadLen;
                    do
                    {
                        vl_ReadLen = GN_GPS_Read_GNB_Meas(CGPS_TREQ_READDATA_MAX_LENGTH, (char*)&vl_TempData);
                    } while( vl_ReadLen != 0 );
                }

                CGPS9_33StartProduction(vg_TREQTestMask, &vg_TREQ_TestRequest);
            }
            else
            {
                vg_WaitTimer++;
            }
        }
    }
    else if ((vg_TREQ_Status & CGPS_TREQ_STATE_MEASURE) > 0)
    {
        MC_CGPS_TRACE(("CGPS_TREQ_STATE_MEASURE vg_WaitTimer[%d]", vg_WaitTimer));

        v_ReadLen = CGPS9_45Get_ProductionTestResult();
    }

    MC_CGPS_TRACE(("END CGPS9_44TREQTimerExpired[%d]", vg_TREQ_Status));

    return pl_NextState;
}


/*****************************************************************************/
/* CGPS9_45Get_ProductionTestResult : ProductionTest Result read from patch function        */
/*****************************************************************************/
uint32_t CGPS9_45Get_ProductionTestResult()
{
    uint32_t  vl_readLen;
    int8_t    a_readBuffer[CGPS_TREQ_READDATA_MAX_LENGTH+1];
    bool      vl_EOF = FALSE;

    vl_readLen = GN_GPS_Read_GNB_Meas(CGPS_TREQ_READDATA_MAX_LENGTH, (char*)a_readBuffer);

    if(vl_readLen <= CGPS_TREQ_READDATA_MAX_LENGTH)
        a_readBuffer[vl_readLen] = 0;
    else
        a_readBuffer[CGPS_TREQ_READDATA_MAX_LENGTH] = 0;

    if( vl_readLen != 0 )
    {
        //MC_CGPS_TRACE(("CGPS9_45Get_ProductionTestResult Len<%d>\n< %s >\n", vl_readLen, a_readBuffer));
        if (strncmp((const char*)a_readBuffer ,"#TRES", 5) == 0)
        {
            vg_ReTry = 0;
            if (vl_readLen > 150)
            {
                int8_t *newStr;
                newStr = (int8_t*)strtok((char*)a_readBuffer, "\r\n");
                while (newStr != NULL)
                {
                    if (strncmp((const char*)newStr ,"#TRES", 5) == 0)
                    {
                        vl_readLen = strlen((const char*)newStr);
                        MC_CGPS_TRACE(("#LINE Count [%d] Len<%d>\n< %s\r\n >\n",vg_ReadCount ,vl_readLen, newStr));
                        newStr = (int8_t*)strtok(NULL, "\r\n");
                        vg_ReadCount++;
                    }
                    else
                    {
                        MC_CGPS_TRACE(("NOT #TRES Count [%d] Len<%d>\n< %s\r\n >\n",vg_ReadCount ,vl_readLen, newStr));
                        newStr = (int8_t*)strtok(NULL, "\r\n");
                    }
                }
            }
            else
            {
                MC_CGPS_TRACE(("LINE Count [%d] Len<%d>\n< %s >\n",vg_ReadCount ,vl_readLen, a_readBuffer));
                vg_ReadCount++;
            }
            vl_EOF = FALSE;
        }
        else
        {
            if (vg_ReadCount > 1)
            {
                MC_CGPS_TRACE(("EOF REMIND Data Len<%d>\n< %s >\n",vl_readLen, a_readBuffer));
                vl_EOF = TRUE;

                int8_t vl_TempData[CGPS_TREQ_READDATA_MAX_LENGTH];
                uint32_t vl_ReadLen;
                do
                {
                    vl_ReadLen = GN_GPS_Read_GNB_Meas(CGPS_TREQ_READDATA_MAX_LENGTH, (char*)&vl_TempData);
                    MC_CGPS_TRACE(("EOF Read_GNB_Meas [%d]", vl_ReadLen));
                } while( vl_ReadLen != 0 );
            }
        }
    }
    else
    {
        if (vg_ReadCount > 1)
        {
            vg_ReTry++;
        }
        MC_CGPS_TRACE(("CGPS9_45Get_ProductionTestResult No Data Read!vg_ReTry %d \n Len<%d>\n< %s >\n", vg_ReTry,vl_readLen, a_readBuffer));
    }


    vg_WaitTimer = 0;

    if (vl_EOF == TRUE)
    {
        vg_ReadCount = 1;
        vg_TREQ_Status &= 0x0001;
    }

    return vl_readLen;
}



#endif /* AGPS_TEST_MODE_FTR */
