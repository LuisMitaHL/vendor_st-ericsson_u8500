/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#define LSIM7FTA_C


#ifdef AGPS_TIME_SYNCH_FTR

#include "lsimfta.h"

#undef  FILE_NUMBER
#define FILE_NUMBER     7

void lsim7_02SendFtaPulseReq( void )
{
    u_gnsFTA_MsgDataOut v_MsgData;

    memset( &v_MsgData , 0 , sizeof(v_MsgData) );
    switch(vg_FtaInfo.v_CellType)
    {
       case LSIMCP_WCDMA_CELL_TIME:
        {
           v_MsgData.v_gnsFtaPulseReq.v_CellType = vg_FtaInfo.v_CellType;
           v_MsgData.v_gnsFtaPulseReq.v_CellIdentifier= vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_Psc;
           GNS_ExecuteFtaCallback( E_gnsFTA_PULSE_REQ, sizeof(s_gnsFTA_PulseReq) , &v_MsgData  );
        }
       break;
       case LSIMCP_GSM_CELL_TIME:
        {
           v_MsgData.v_gnsFtaPulseReq.v_CellType = vg_FtaInfo.v_CellType;
           v_MsgData.v_gnsFtaPulseReq.v_CellIdentifier = K_gnsUNKNOWN_U16; //Setting to invalid value

           GNS_ExecuteFtaCallback( E_gnsFTA_PULSE_REQ, sizeof(s_gnsFTA_PulseReq) , &v_MsgData  );
        }
       break;
       default:
        {
           DEBUG_LOG_PRINT_LEV1(("No Previous Cell time. FTA request not send"));
        }
       break;
    }

}

void lsim7_03SendFtaPulseCnf( s_gnsFTA_PulseCnf *pp_PulseTime )
{

     int32_t vl_CellDeltaTime;
     uint32_t vl_FineGpsTime;

     t_lsim_FtaPulseCnfM  *pl_FtaPulseCnfM = NULL;
     DEBUG_LOG_PRINT_LEV1(("+lsim7_03: send FTA pulse conf"));
     if(pp_PulseTime != NULL)
     {
         lsim7_14FtaStorePulsetime(pp_PulseTime->v_CellTimeType,&pp_PulseTime->v_CellTime);
         vl_CellDeltaTime = lsim7_05FtaCalculateCellDeltatime(pp_PulseTime);

         pl_FtaPulseCnfM = (t_lsim_FtaPulseCnfM*)MC_RTK_GET_MEMORY (sizeof(*pl_FtaPulseCnfM));
         if(pl_FtaPulseCnfM == NULL)
         {
             DEBUG_LOG_PRINT_LEV1(("lsim7_03SendFtaPulseCnf : Unable to allocate Memory FTA Confirmation will not be sent"));
             return;

         }

         pl_FtaPulseCnfM->v_IsRefTimeValid               =   TRUE;
         pl_FtaPulseCnfM->v_RefTime.v_GpsWeek            =   vg_FtaInfo.v_RefTime.v_GpsWeek; //To be Checked
         pl_FtaPulseCnfM->v_RefTime.v_GpsTow             =   vg_FtaInfo.v_RefTime.v_GpsTow;
         pl_FtaPulseCnfM->v_RefTime.v_Prec               =   vg_FtaInfo.v_RefTime.v_Prec;
         pl_FtaPulseCnfM->v_RefTime.v_RefDeltaCellTime   =   vl_CellDeltaTime;
         pl_FtaPulseCnfM->v_RefTime.v_SfnTowUnc          =   vg_FtaInfo.v_RefTime.v_SfnTowUnc ;
         pl_FtaPulseCnfM->v_RefTime.v_UtranGpsDriftRate  =   vg_FtaInfo.v_RefTime.v_UtranGpsDriftRate;
         pl_FtaPulseCnfM->v_RefTime.v_NumTowAssist       =   0;

     }
     else
     {
         pl_FtaPulseCnfM->v_IsRefTimeValid = FALSE;
     }

    MC_RTK_SEND_MSG_TO_PROCESS(PROCESS_LSIMCP, 0,
                              PROCESS_CGPS, 0,
                              CGPS_LSIMCP_FTA_PULSE_CNF,
                              (t_MsgHeader *) pl_FtaPulseCnfM);

}


void lsim7_04FtaStoreCelltime(e_gns_RATType v_CellTimeType,u_gnsCP_CellTime *pp_CellTime)
{
 DEBUG_LOG_PRINT_LEV1(("lsim7_04 : Storing Cell Time"));

 switch(v_CellTimeType)
 {
    case E_gns_RAT_TYPE_GSM:
    {
       vg_FtaInfo.v_CellType                                = LSIMCP_GSM_CELL_TIME;
       vg_FtaInfo.v_CellTime.v_GsmCellTime.v_QBitNumber      = pp_CellTime->v_GsmCellTime.v_QBitNumber;
       vg_FtaInfo.v_CellTime.v_GsmCellTime.v_BCCHCarrier    = pp_CellTime->v_GsmCellTime.v_BCCHCarrier;
       vg_FtaInfo.v_CellTime.v_GsmCellTime.v_BSIC           = pp_CellTime->v_GsmCellTime.v_BSIC;
       vg_FtaInfo.v_CellTime.v_GsmCellTime.v_TimeSlot       = pp_CellTime->v_GsmCellTime.v_TimeSlot;
       vg_FtaInfo.v_CellTime.v_GsmCellTime.v_FrameNumber    = pp_CellTime->v_GsmCellTime.v_FrameNumber;

       DEBUG_LOG_PRINT_LEV1(("lsim7_04 : v_CellTimeType %s ; v_QBitNumber %d ; v_BCCHCarrier %d v_BSIC %d v_TimeSlot %d v_FrameNumber %d", \
                                     "GSM Cell" ,\
                                     vg_FtaInfo.v_CellTime.v_GsmCellTime.v_QBitNumber,\
                                     vg_FtaInfo.v_CellTime.v_GsmCellTime.v_BCCHCarrier,\
                                     vg_FtaInfo.v_CellTime.v_GsmCellTime.v_BSIC, \
                                     vg_FtaInfo.v_CellTime.v_GsmCellTime.v_TimeSlot,\
                                     vg_FtaInfo.v_CellTime.v_GsmCellTime.v_FrameNumber));

    }
    break;
    case E_gns_RAT_TYPE_WCDMA_FDD:
    {
       vg_FtaInfo.v_CellType                                       = LSIMCP_WCDMA_CELL_TIME;
       vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_Sfn                 = pp_CellTime->v_WcdmaFddCellTime.v_Sfn;
       vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_Psc    = pp_CellTime->v_WcdmaFddCellTime.v_Psc;
       vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_CellFrameTimeLS     = pp_CellTime->v_WcdmaFddCellTime.v_CellFrameTimeLS;
       vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_CellFrameTimeMS     = pp_CellTime->v_WcdmaFddCellTime.v_CellFrameTimeMS;
       vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_RoundTripTime       = pp_CellTime->v_WcdmaFddCellTime.v_RoundTripTime;

       DEBUG_LOG_PRINT_LEV1(("lsim7_04 : v_CellTimeType %s ; v_Sfn %u ; v_Psc %u v_CellFrameTimeLS %u v_CellFrameTimeMS %u v_RoundTripTime %u", \
                                     "WCDMA Cell" ,\
                                     vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_Sfn,\
                                     vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_Psc,\
                                     vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_CellFrameTimeLS, \
                                     vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_CellFrameTimeMS,\
                                     vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_RoundTripTime));

    }
    break;
    case E_gns_RAT_TYPE_WCDMA_TDD: /*TDD is not supported*/
    default:
       vg_FtaInfo.v_CellType = LSIMCP_NO_CELL_TIME;
       DEBUG_LOG_PRINT_LEV1(("lsim7_04 : Invalid Cell Type "));
    break;
 }

 return;

}

static int32_t lsim7_05FtaCalculateCellDeltatime(s_gnsFTA_PulseCnf * pp_PulseTime)
{

    int32_t vl_DeltaTime = LSIMCP_U32_UNKNOWN;

    switch(pp_PulseTime->v_CellTimeType)
    {
        case E_gns_RAT_TYPE_GSM:
        {
          vl_DeltaTime= lsim7_06FtaCalculateCellGsmDeltatime(&pp_PulseTime->v_CellTime.v_GsmCellTime);
        }
        break;

        case E_gns_RAT_TYPE_WCDMA_FDD:
        {
          vl_DeltaTime = lsim7_07FtaCalculateCellWcdmaFddDeltatime(&pp_PulseTime->v_CellTime.v_WcdmaFddCellTime);
        }
        break;

        default:
        {
           DEBUG_LOG_PRINT_LEV1(("lsim7_05 : Invalid Cell Type "));
        }
    }
    return vl_DeltaTime;

}

static int32_t lsim7_06FtaCalculateCellGsmDeltatime(s_gnsCPGsmCellTime *p_GsmGpsTime )
{

    /*Time_qb = bit_no + ta * 4 + time_slot * 625 + frame_number * 5000
           So for 2 consecutive instances, you get Time1_qb and Time2_qb:
           vl_DeltaQuadrabit = Time_qb2 - Time_qb1
            Delta_Time_micros = Delta_Time_qb * 60/13/5.*/

    int32_t vl_CellDeltaTime = 0;
    uint64_t vl_PrevGsmQuadraBits = 0;
    uint64_t vl_CurrentGsmQuadraBits = 0;
    int64_t vl_DeltaQuadrabit = 0;

    DEBUG_LOG_PRINT_LEV1(("lsim7_06 :Calculating GSM cell delta time "));

    if((p_GsmGpsTime->v_FrameNumber > K_LSIMCP_GSM_REF_TIME_MAX_FN_VALUE) &&
       (vg_FtaInfo.v_CellTime.v_GsmCellTime.v_FrameNumber < K_LSIMCP_GSM_REF_TIME_FN_MAX_TRUNCATED_VALUE))
    {
        /*If current frame number is greater than 0x1FFFFF that means the frame number we got earlier in refrence Time is trunacted by 1 bit
                 so add 0x20000 to the frame number in Refrence time to convert that into same range as Frame number in GSM time Resp*/
        vg_FtaInfo.v_CellTime.v_GsmCellTime.v_FrameNumber |= K_LSIMCP_GSM_REF_TIME_FN_ROLLOVER_VALUE;
        DEBUG_LOG_PRINT_LEV1(("FTA: REF_TIME Framenumber after adding 2097152 = %u", \
                               vg_FtaInfo.v_CellTime.v_GsmCellTime.v_FrameNumber));
    }
    else if((p_GsmGpsTime->v_FrameNumber < vg_FtaInfo.v_CellTime.v_GsmCellTime.v_FrameNumber) &&
            ((vg_FtaInfo.v_CellTime.v_GsmCellTime.v_FrameNumber - p_GsmGpsTime->v_FrameNumber) > K_LSIMCP_GSM_REF_TIME_FN_MAX_UNC_VALUE))
    {
       /* If current frame number in GSM_TIME_STAMP_RESP is less than Frame number and also
                 ((frame number in Ref time - Frame number in GSM_TIME_STAMP_RESP) > 1000) we got in refrence time
                 then we have 2 possible cases
                  1) Frame number in that we Got in REF Time is Truncated
                  2) Frame Number in Ref Time is Not Truncated

                   Case 1 doesn't hold good as if frame number in Ref time is Not truncated then for frame number to rollover it will take about
                     40 minutes but we request and get a GSM_TIME_STAMP_RESP within few seconds.

                   case 2 is the possible case so we will increment the Frame number in Refrence time to convert into the correct range and
                   incrementing Frame number in  GSM_TIME_STAMP_RESP for handling rollover.
             */
        if(vg_FtaInfo.v_CellTime.v_GsmCellTime.v_FrameNumber < K_LSIMCP_GSM_REF_TIME_FN_MAX_TRUNCATED_VALUE)
        {
           DEBUG_LOG_PRINT_LEV1(("lsim7_06 :Ref time frameno. is truncated and GSM time stamp frameno. has rolledover "));
        vg_FtaInfo.v_CellTime.v_GsmCellTime.v_FrameNumber += K_LSIMCP_GSM_REF_TIME_FN_ROLLOVER_VALUE;
        p_GsmGpsTime->v_FrameNumber                       += K_LSIMCP_GSM_FN_ROLLOVER_VALUE;
    }
        else
        {
          DEBUG_LOG_PRINT_LEV1(("lsim7_06 :ERROR: Wrong Frame Numbers Returning with Zero delta "));
          return 0;
        }
       
    }
    else if((p_GsmGpsTime->v_FrameNumber > vg_FtaInfo.v_CellTime.v_GsmCellTime.v_FrameNumber) &&
            ((p_GsmGpsTime->v_FrameNumber - vg_FtaInfo.v_CellTime.v_GsmCellTime.v_FrameNumber) > K_LSIMCP_GSM_REF_TIME_FN_MAX_TRUNCATED_VALUE))
    {
       /*The reference Frame time is less than Pulse frame time:
                This is possible as referenec time need to be accurate and go up to max -2 to +2 sec in case of coarse time
                As Same GPS Tow is being used in both Coarse time and Fine time, delta calculated from Frame may go upto max of -2 sec.
                In this case if Frame Number if Ref time is just say 400 frames grearter than K_LSIMCP_GSM_REF_TIME_FN_ROLLOVER_VALUE then it will be 
                truncated and Due to -2 -+2 seconds uncertainity frame number in GSM_TIME_STAMP_RESP comes just below 
                K_LSIMCP_GSM_REF_TIME_FN_ROLLOVER_VALUE than only Frame Number in Ref time is truncated  and for exact delta we need to 
                add K_LSIMCP_GSM_REF_TIME_FN_ROLLOVER_VALUE */
        vg_FtaInfo.v_CellTime.v_GsmCellTime.v_FrameNumber += K_LSIMCP_GSM_REF_TIME_FN_ROLLOVER_VALUE;
    }

    vl_CurrentGsmQuadraBits = MC_LSIMCP_CALCULATE_GSM_CELL_TIME(p_GsmGpsTime->v_QBitNumber, \
                                 p_GsmGpsTime->v_TimeSlot, \
                                 p_GsmGpsTime->v_FrameNumber);


    vl_PrevGsmQuadraBits = MC_LSIMCP_CALCULATE_GSM_CELL_TIME(vg_FtaInfo.v_CellTime.v_GsmCellTime.v_QBitNumber, \
                                   vg_FtaInfo.v_CellTime.v_GsmCellTime.v_TimeSlot, \
                                   vg_FtaInfo.v_CellTime.v_GsmCellTime.v_FrameNumber);


       vl_DeltaQuadrabit = vl_CurrentGsmQuadraBits - vl_PrevGsmQuadraBits;

    vl_CellDeltaTime = MC_LSIMCP_GSM_QUADRA_BITS_TO_MICROSECONDS(vl_DeltaQuadrabit);


    DEBUG_LOG_PRINT_LEV1(("lsim7_06 : vl_PrevGsmQuadraBits = %llu vl_CurrentGsmQuadraBits = %llu vl_DeltaQuadrabit =%lld vl_CellDeltaTime = %d", \
                            vl_PrevGsmQuadraBits, \
                            vl_CurrentGsmQuadraBits, \
                            vl_DeltaQuadrabit, \
                            vl_CellDeltaTime));

     return vl_CellDeltaTime;

}

static int32_t lsim7_07FtaCalculateCellWcdmaFddDeltatime(s_gnsCPWcdmaFddCellTime *p_GsmWcdmaTime)
{

    int32_t vl_CellDeltaTime = 0;
    uint64_t vl_PrevWcdmaCellTime = 0;
    uint64_t vl_CurrentWcdmaCellTime = 0;
    uint64_t vl_PrevWcdmachips = 0;
    uint64_t vl_PrevWcdmachipsinmicrosec = 0;

    DEBUG_LOG_PRINT_LEV1(("lsim7_07 : Calculating Wcdma Delta Time"));

    vl_PrevWcdmaCellTime = MC_LSIMCP_CONVERT_TO_WCDMA_SFN_TO_MICROSECONDS(vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_Sfn);
    if(p_GsmWcdmaTime->v_Sfn < vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_Sfn)
    {
       DEBUG_LOG_PRINT_LEV1(("lsim7_07 : SFN Wrap around Happened"));
       p_GsmWcdmaTime->v_Sfn = K_LSIMCP_WCDMA_SFN_ROLLOVER_VALUE + p_GsmWcdmaTime->v_Sfn;
    }

    vl_CurrentWcdmaCellTime = MC_LSIMCP_CONVERT_TO_WCDMA_SFN_TO_MICROSECONDS(p_GsmWcdmaTime->v_Sfn);
    vl_PrevWcdmachips =
      MC_LSIMCP_CONVERT_WCDMA_CELLFRAME_MSLS_TO_NUMBER_OF_CHIP(vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_CellFrameTimeLS,\
                                                                vg_FtaInfo.v_CellTime.v_WcdmaFddCellTime.v_CellFrameTimeMS);

    vl_PrevWcdmachipsinmicrosec = MC_LSIMCP_CONVERT_WCDMA_CHIP_TO_MICROSECONDS(vl_PrevWcdmachips);

    /* Tow we receive in Refrence time is uncertain by -2 to +2 sec as per 3gpp spec 25.331
          In the same Ref Time Message we also get Cell Frame Time LS and Cell Frame Time MS part as Cell timing parameters
          which is the more accurate indication for  TOW than the TOW we get in Refrence time
          TOW (in Milliseconds) =  ((ms-part * 4294967296) + ls-part) / 3840
          as 1 millisecond = 3840 chips
          so the TOW we calculate from LS and MS part is the actuall TOW with less uncertainity that we have to use*/

    vg_FtaInfo.v_RefTime.v_GpsTow = vl_PrevWcdmachipsinmicrosec / 1000;
    DEBUG_LOG_PRINT_LEV1(("lsim7_07 : TOW from Chip %u",vg_FtaInfo.v_RefTime.v_GpsTow));

    vl_CellDeltaTime = (vl_CurrentWcdmaCellTime - vl_PrevWcdmaCellTime);

    DEBUG_LOG_PRINT_LEV1(("lsim7_07 : vl_PrevWcdmaCellTime = %llu vl_CurrentWcdmaCellTime = %llu vl_PrevWcdmachips =%llu vl_PrevWcdmachipsinmicrosec = %llu vl_CellDeltaTime = %d" , \
                            vl_PrevWcdmaCellTime, \
                            vl_CurrentWcdmaCellTime, \
                            vl_PrevWcdmachips, \
                            vl_PrevWcdmachipsinmicrosec, \
                            vl_CellDeltaTime));

    return vl_CellDeltaTime;
}

void lsim7_08FtaClearCellTimeInfo(void)
{
    t_lsim_FTAInfo *pl_FTAInfo;
    t_lsim_PulseInfo *pl_PulseInfo;
    DEBUG_LOG_PRINT_LEV1(("lsim7_08 :Clear cell time information "));
    memset(&vg_FtaInfo, 0, sizeof(*pl_FTAInfo));
    vg_FtaInfo.v_CellType  = LSIMCP_NO_CELL_TIME;
    memset(vg_PulseInfo, 0, (sizeof(*pl_PulseInfo) * K_LSIMCP_PULSE_TIME_DATABASE_LENGTH));
    vg_CurrentPulseInfoIndex = -1;

    return;
}


void lsim7_01HandleFtaPulseReq( t_RtkObject* p_FsmObject )
{
      lsim7_02SendFtaPulseReq();
}

void lsim7_09FtaUpdateRefTime(t_lsimcp_RefTime *pp_RefTime)
{

  vg_FtaInfo.v_RefTime.v_GpsTow                      = pp_RefTime->v_GpsTow;
  vg_FtaInfo.v_RefTime.v_GpsWeek                     = pp_RefTime->v_GpsWeek;
  vg_FtaInfo.v_RefTime.v_Prec                          = pp_RefTime->v_Prec ;
  vg_FtaInfo.v_RefTime.v_RefDeltaCellTime  = pp_RefTime->v_RefDeltaCellTime ;
  vg_FtaInfo.v_RefTime.v_SfnTowUnc                 = pp_RefTime->v_SfnTowUnc  ;
  vg_FtaInfo.v_RefTime.v_UtranGpsDriftRate = pp_RefTime->v_UtranGpsDriftRate;
}

void lsim7_10FtaFillCellTimeInMeasRes(s_gnsCP_GpsMeasResult *p_GpsMeasResult)
{
   u_gnsCP_CellTime *pl_CellTime = NULL;
   DEBUG_LOG_PRINT_LEV1(("lsim7_10 :Populating Cell time Info in Measurment report"));

   switch(vg_FtaInfo.v_CellType)
   {
     case  LSIMCP_GSM_CELL_TIME:
     {
         p_GpsMeasResult->v_CellTimeType = E_gns_RAT_TYPE_GSM;
         lsim7_12FtaConvertDeltaToGSMCellTime(p_GpsMeasResult->v_Measurements.v_RefDeltaCellTime,&p_GpsMeasResult->v_CellTime.v_GsmCellTime);
     }
     break;
     case  LSIMCP_WCDMA_CELL_TIME:
     {
         p_GpsMeasResult->v_CellTimeType = E_gns_RAT_TYPE_WCDMA_FDD;
         lsim7_13FtaConvertDeltaToWCDMACellTime(p_GpsMeasResult->v_Measurements.v_RefDeltaCellTime,p_GpsMeasResult->v_Measurements.v_GpsTow,&p_GpsMeasResult->v_CellTime.v_WcdmaFddCellTime);
     }
     break;
     case  LSIMCP_NO_CELL_TIME:
     {
         DEBUG_LOG_PRINT_LEV1(("lsim7_10 :No Cell Time Present"));
         p_GpsMeasResult->v_CellTimeType = E_gns_RAT_TYPE_NONE;
         memset(&p_GpsMeasResult->v_CellTime,0,sizeof(*pl_CellTime));
     }
     break;
   }
  
   return;
  
}

void lsim7_11FtaFillCellTimeInPosRes(s_gnsCP_GpsPosnResult *p_GpsPosnResult)
{

   u_gnsCP_CellTime *pl_CellTime = NULL;
   DEBUG_LOG_PRINT_LEV1(("lsim7_11 :Populating Cell time Info in Position Results"));

   switch(vg_FtaInfo.v_CellType)
   {
     case  LSIMCP_GSM_CELL_TIME:
     {
         p_GpsPosnResult->v_CellTimeType = E_gns_RAT_TYPE_GSM;
         lsim7_12FtaConvertDeltaToGSMCellTime(p_GpsPosnResult->v_Position.v_RefDeltaCellTime,&p_GpsPosnResult->v_CellTime.v_GsmCellTime);
     }
     break;
     case  LSIMCP_WCDMA_CELL_TIME:
     {
         p_GpsPosnResult->v_CellTimeType = E_gns_RAT_TYPE_WCDMA_FDD;
         lsim7_13FtaConvertDeltaToWCDMACellTime(p_GpsPosnResult->v_Position.v_RefDeltaCellTime,p_GpsPosnResult->v_Position.v_GpsTow,&p_GpsPosnResult->v_CellTime.v_WcdmaFddCellTime);
     }
     break;
     case  LSIMCP_NO_CELL_TIME:
     {
         DEBUG_LOG_PRINT_LEV1(("lsim7_11 :No Cell Time Present"));
         p_GpsPosnResult->v_CellTimeType = E_gns_RAT_TYPE_NONE;
         memset(&p_GpsPosnResult->v_CellTime,0,sizeof(*pl_CellTime));
     }
     break;
   }
     
   return;
  
}

static void lsim7_12FtaConvertDeltaToGSMCellTime(int32_t vl_RefDeltaCellTime,s_gnsCPGsmCellTime *p_GsmCellTime)
{
   
   uint16_t vl_DeltaFramenumber = vl_RefDeltaCellTime / K_LSIMCP_GSM_FRAMELENGTH_MICROSECONDS;
   int8_t vl_Index = vg_CurrentPulseInfoIndex;

   DEBUG_LOG_PRINT_LEV1(("lsim7_12 :Populating Cell time Info"));
   DEBUG_LOG_PRINT_LEV1(("lsim7_12 :vl_RefDeltaCellTime %d vl_DeltaFramenumber %d ",vl_RefDeltaCellTime,vl_DeltaFramenumber));
   if(vl_RefDeltaCellTime > K_LSIMCP_PULSE_REQUEST_INTERVAL_MICROSECONDS)
   {
      vl_Index = vg_CurrentPulseInfoIndex -1;
      if(vl_Index < 0)
      {
         vl_Index = K_LSIMCP_PULSE_TIME_DATABASE_LENGTH + vl_Index;
         if(vg_PulseInfo[vl_Index].v_PulseTimetimestamp == 0 || \
             vg_PulseInfo[vl_Index].v_PulseTimetimestamp > (2 * (K_LSIMCP_PULSE_REQUEST_INTERVAL_MICROSECONDS/1000)))
            vl_Index = vg_CurrentPulseInfoIndex;
      }
   }
   DEBUG_LOG_PRINT_LEV1(("lsim7_12 :Pulse information used is vg_PulseInfo[%d]",vg_CurrentPulseInfoIndex));
   
   p_GsmCellTime->v_BCCHCarrier     =  LSIMCP_U16_UNKNOWN;
   p_GsmCellTime->v_QBitNumber      =  LSIMCP_U16_UNKNOWN;
   p_GsmCellTime->v_BSIC            =  LSIMCP_U8_UNKNOWN;
   p_GsmCellTime->v_TimingAdvance   =  LSIMCP_U8_UNKNOWN;
   p_GsmCellTime->v_TimeSlot        =  LSIMCP_U8_UNKNOWN;
   p_GsmCellTime->v_FrameNumber     =  vg_PulseInfo[vl_Index].v_PulseTime.v_GsmCellTime.v_FrameNumber + vl_DeltaFramenumber;

   if(p_GsmCellTime->v_FrameNumber >= K_LSIMCP_GSM_FN_ROLLOVER_VALUE)
   {
      p_GsmCellTime->v_FrameNumber -= K_LSIMCP_GSM_FN_ROLLOVER_VALUE;
      DEBUG_LOG_PRINT_LEV1(("lsim7_13 :Frame Rollover happened "));
   }

   DEBUG_LOG_PRINT_LEV1(("lsim7_12 : v_CellTimeType %s ; v_QBitNumber %d ; v_BCCHCarrier %d v_BSIC %d v_TimeSlot %d v_FrameNumber %d", \
                                        "GSM Cell" ,\
                                        p_GsmCellTime->v_QBitNumber,\
                                        p_GsmCellTime->v_BCCHCarrier,\
                                        p_GsmCellTime->v_BSIC, \
                                        p_GsmCellTime->v_TimeSlot ,\
                                        p_GsmCellTime->v_FrameNumber));
   

   return;

}

static void lsim7_13FtaConvertDeltaToWCDMACellTime(int32_t vl_RefDeltaCellTime,uint32_t vl_GpsTow,s_gnsCPWcdmaFddCellTime *p_WcdmaFddCellTime)
{

   uint64_t vl_NumOfChipsInGpsTow = (uint64_t)vl_GpsTow * K_LSIMCP_WCDMA_NUMBER_OF_CHIPS_IN_ONEMS;
   int16_t vl_DeltaFramenumber = vl_RefDeltaCellTime / K_LSIMCP_WCDMA_SFNLENGTH_MICROSECONDS;
   int8_t vl_Index = vg_CurrentPulseInfoIndex;
   
   DEBUG_LOG_PRINT_LEV1(("lsim7_13 :Populating Cell time Info"));
   DEBUG_LOG_PRINT_LEV1(("lsim7_13 :vl_RefDeltaCellTime %d vl_DeltaFramenumber %d vl_NumOfChipsInGpsTow %llu",vl_RefDeltaCellTime,vl_DeltaFramenumber,vl_NumOfChipsInGpsTow));
   if(vl_RefDeltaCellTime > K_LSIMCP_PULSE_REQUEST_INTERVAL_MICROSECONDS)
   {
      vl_Index = vg_CurrentPulseInfoIndex -1;
      if(vl_Index < 0)
      {
         vl_Index = K_LSIMCP_PULSE_TIME_DATABASE_LENGTH + vl_Index;
         if(vg_PulseInfo[vl_Index].v_PulseTimetimestamp == 0 || \
             vg_PulseInfo[vl_Index].v_PulseTimetimestamp > (2 * (K_LSIMCP_PULSE_REQUEST_INTERVAL_MICROSECONDS/1000)))
            vl_Index = vg_CurrentPulseInfoIndex;
      }
      
   }

   DEBUG_LOG_PRINT_LEV1(("lsim7_13 :Pulse information used is vg_PulseInfo[%d]",vg_CurrentPulseInfoIndex));
   
   p_WcdmaFddCellTime->v_Psc                =  vg_PulseInfo[vl_Index].v_PulseTime.v_WcdmaFddCellTime.v_Psc; 
   p_WcdmaFddCellTime->v_Sfn                =  vg_PulseInfo[vl_Index].v_PulseTime.v_WcdmaFddCellTime.v_Sfn + vl_DeltaFramenumber;

   if(p_WcdmaFddCellTime->v_Sfn >= K_LSIMCP_WCDMA_SFN_ROLLOVER_VALUE)
   {
      p_WcdmaFddCellTime->v_Sfn  -= K_LSIMCP_WCDMA_SFN_ROLLOVER_VALUE;
      DEBUG_LOG_PRINT_LEV1(("lsim7_13 :SFN Rollover happened "));
   }
   
   p_WcdmaFddCellTime->v_RoundTripTime      =  LSIMCP_U16_UNKNOWN;
   p_WcdmaFddCellTime->v_CellFrameTimeMS    =  (uint16_t) (vl_NumOfChipsInGpsTow / K_LSIMCP_WCDMA_CELLTIME_LS_MAX_VALUE);
   p_WcdmaFddCellTime->v_CellFrameTimeLS    =  (uint32_t) (vl_NumOfChipsInGpsTow % K_LSIMCP_WCDMA_CELLTIME_LS_MAX_VALUE);
   
   DEBUG_LOG_PRINT_LEV1(("lsim7_13 : v_CellTimeType %s ; v_Sfn %u ; v_Psc %u v_RoundTripTime %u CellLS %u CellMS %u", \
                                        "WCDMA Cell" ,\
                                        p_WcdmaFddCellTime->v_Sfn,\
                                        p_WcdmaFddCellTime->v_Psc,\
                                        p_WcdmaFddCellTime->v_RoundTripTime,
                                        p_WcdmaFddCellTime->v_CellFrameTimeLS,
                                        p_WcdmaFddCellTime->v_CellFrameTimeMS));

   return;
}

static void lsim7_14FtaStorePulsetime(e_gns_RATType v_CellTimeType,u_gnsCP_CellTime *pp_CellTime)
{
    t_lsim_PulseInfo *pl_CurrentPulseInfo = NULL;
    DEBUG_LOG_PRINT_LEV1(("lsim7_14 : Storing Pulse Time"));

    vg_CurrentPulseInfoIndex++;
    
    if(vg_CurrentPulseInfoIndex == K_LSIMCP_PULSE_TIME_DATABASE_LENGTH)
    {
        vg_CurrentPulseInfoIndex -= K_LSIMCP_PULSE_TIME_DATABASE_LENGTH;
    }
    pl_CurrentPulseInfo = &vg_PulseInfo[vg_CurrentPulseInfoIndex];

     DEBUG_LOG_PRINT_LEV1(("lsim7_14: current Pulse information stored at vg_PulseInfo[%d]",vg_CurrentPulseInfoIndex));
    pl_CurrentPulseInfo->v_PulseTimetimestamp = OSA_GetMsCount();

    switch(v_CellTimeType)
    {
       case E_gns_RAT_TYPE_GSM:
       {
          pl_CurrentPulseInfo->v_CellType                                 = LSIMCP_GSM_CELL_TIME;
          pl_CurrentPulseInfo->v_PulseTime.v_GsmCellTime.v_QBitNumber     = pp_CellTime->v_GsmCellTime.v_QBitNumber;
          pl_CurrentPulseInfo->v_PulseTime.v_GsmCellTime.v_BCCHCarrier    = pp_CellTime->v_GsmCellTime.v_BCCHCarrier;
          pl_CurrentPulseInfo->v_PulseTime.v_GsmCellTime.v_BSIC           = pp_CellTime->v_GsmCellTime.v_BSIC;
          pl_CurrentPulseInfo->v_PulseTime.v_GsmCellTime.v_TimeSlot       = pp_CellTime->v_GsmCellTime.v_TimeSlot;
          pl_CurrentPulseInfo->v_PulseTime.v_GsmCellTime.v_FrameNumber    = pp_CellTime->v_GsmCellTime.v_FrameNumber;
          pl_CurrentPulseInfo->v_PulseTime.v_GsmCellTime.v_TimingAdvance  = pp_CellTime->v_GsmCellTime.v_TimingAdvance;

          DEBUG_LOG_PRINT_LEV1(("lsim7_14 : v_CellTimeType %s ; v_QBitNumber %d ; v_BCCHCarrier %d v_BSIC %d v_TimeSlot %d v_FrameNumber %d", \
                                        "GSM Cell" ,\
                                        pl_CurrentPulseInfo->v_PulseTime.v_GsmCellTime.v_QBitNumber,\
                                        pl_CurrentPulseInfo->v_PulseTime.v_GsmCellTime.v_BCCHCarrier,\
                                        pl_CurrentPulseInfo->v_PulseTime.v_GsmCellTime.v_BSIC, \
                                        pl_CurrentPulseInfo->v_PulseTime.v_GsmCellTime.v_TimeSlot,\
                                        pl_CurrentPulseInfo->v_PulseTime.v_GsmCellTime.v_FrameNumber));
   
       }
       break;
       case E_gns_RAT_TYPE_WCDMA_FDD:
       {
          pl_CurrentPulseInfo->v_CellType                                        = LSIMCP_WCDMA_CELL_TIME;
          pl_CurrentPulseInfo->v_PulseTime.v_WcdmaFddCellTime.v_Sfn              = pp_CellTime->v_WcdmaFddCellTime.v_Sfn;
          pl_CurrentPulseInfo->v_PulseTime.v_WcdmaFddCellTime.v_Psc              = pp_CellTime->v_WcdmaFddCellTime.v_Psc;
          pl_CurrentPulseInfo->v_PulseTime.v_WcdmaFddCellTime.v_RoundTripTime    = pp_CellTime->v_WcdmaFddCellTime.v_RoundTripTime;
   
          DEBUG_LOG_PRINT_LEV1(("lsim7_14 : v_CellTimeType %s ; v_Sfn %u ; v_Psc %u v_RoundTripTime %u", \
                                        "WCDMA Cell" ,\
                                        pl_CurrentPulseInfo->v_PulseTime.v_WcdmaFddCellTime.v_Sfn,\
                                        pl_CurrentPulseInfo->v_PulseTime.v_WcdmaFddCellTime.v_Psc,\
                                        pl_CurrentPulseInfo->v_PulseTime.v_WcdmaFddCellTime.v_RoundTripTime));
   
       }
       break;
       case E_gns_RAT_TYPE_WCDMA_TDD: /*TDD is not supported*/
       default:
          pl_CurrentPulseInfo->v_CellType = LSIMCP_NO_CELL_TIME;
          DEBUG_LOG_PRINT_LEV1(("lsim7_14 : Invalid Cell Type "));
       break;
    }

 return;
}


#endif /*AGPS_TIME_SYNCH_FTR*/

