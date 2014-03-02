/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
* \file cgps11ee.c
* \date 19/01/2012
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
*             <TD> 21.07.08 </TD><TD> Y.DESAPHI </TD><TD> Creation </TD>
*     </TR>
*     <TR>
*             <TD> 12.08.08 </TD><TD> M.BELOU </TD><TD> Correct errors and add operator rules </TD>
*     </TR>
* </TABLE>
*/

#define __CGPS11EE_C__

#include <stdlib.h>
#include <string.h>

#include "gps_ptypes.h"

#include "GN_EE_api.h"


#include "cgpsee.h"

#undef MODULE_NUMBER
#define MODULE_NUMBER MODULE_CGPS

#undef PROCESS_NUMBER
#define PROCESS_NUMBER PROCESS_CGPS

#undef FILE_NUMBER
#define FILE_NUMBER 11





//*****************************************************************************
//*****************************************************************************
// CGPS11_01EEClientGPSEphemerisCheck:  Checks for a valid RXN_ephem_t record.
// Only items which do not make use of the full bit range of their data type
// can be checked for being out of bounds, apart from sqrt_a which is nearly
// always 2702000000 +/- 100000.  In addition the reference time tags for the
// ephemeris and clock terms should be the same.

BOOL CGPS11_01EEClientGPSEphemerisCheck( // Check the RXN Ephemeris Record ?
   const t_cgps_GPSExtendedEphData *eph ,U1 SV)      // [in] RXN ephemeris record.
{


   // If the reported URA is 10 = 256m URE, then Rx-Networks recommend that
   // the ephemeris should not be used for Navigation.
   // Although it could be de-weighted and used for Pre-Positioning, the
   // almanac is probably a better bet so ignore this ephemeris.
   

   if ( eph->v_Prn      !=  SV         || 
        eph->v_Prn      ==  0          ||     // U08 6 bits [1..63]
        eph->v_Prn      >   32         ||     // U08 6 bits [1..63]
        eph->v_Toc      !=  eph->v_Toe ||     // toc and toe should be the same !
        eph->v_Ura      >=  10         ||     // U08 4 bits [0..15]
        eph->v_Health   >=  (1<<6)     ||     // U08 6 bits [0..63]
        eph->v_EphemFit <   0          ||     // S08 1 bit  [0=4hrs, 1=6hrs]
        eph->v_EphemFit >=  (1<<1)     ||     // S08 1 bit  [0=4hrs, 1=6hrs]
        eph->v_GpsWeek  <   (1*1024)   ||     // U16 10 bits [0..1023] + roll over
        eph->v_GpsWeek  >=  (4*1024)   ||     // U16 10 bits [0..1023] + roll over
        eph->v_Iode     >=  (1<<10)    ||     // 10 bits [0..1023]
        eph->v_IDot     >=  (1<<13)    ||     // 14 bits [x 2^-43 semi-circles/sec]
        eph->v_IDot     <  -(1<<13)    ||     // 14 bits [x 2^-43 semi-circles/sec]
        eph->v_OmegaDot >=  (1<<23)    ||     // 24 bits [x 2^-43 semi-circles/sec]
        eph->v_OmegaDot <  -(1<<23)    ||     // 24 bits [x 2^-43 semi-circles/sec]
        eph->v_SqrtA   <  2600000000U   ||     // 24 bits [x 2^-19 m^0.5]
        eph->v_SqrtA   >  2800000000U   ||
        eph->v_Af0 < -(1<<21)          ||
        eph->v_Af0  >= (1<<21)
        )   // 24 bits [x 2^-19 m^0.5]
   {
      MC_CGPS_TRACE(( "CGPS11_01EEClientGPSEphemerisCheck: INVALID prn:%d  toc:%d  toe:%d  ura:%d  health:%d  ephem_fit:%d  gps_week:%d  iode:%d  i_dot:%d  omega_dot:%d",
               (int32_t)eph->v_Prn,       (int32_t)eph->v_Toc,      (int32_t)eph->v_Toe,
               (int32_t)eph->v_Ura,       (int32_t)eph->v_Health,   (int32_t)eph->v_EphemFit,
               (int32_t)eph->v_GpsWeek,  (int32_t)eph->v_Iode,     (int32_t)eph->v_IDot,
               (int32_t)eph->v_OmegaDot  ));
      return( FALSE );
   }

   MC_CGPS_TRACE(( "CGPS11_01EEClientGPSEphemerisCheck: VALID prn:%d",(int32_t)eph->v_Prn));
   return( TRUE );
}



//*****************************************************************************
//*****************************************************************************
// CGPS11_02EEClientGlonEphemerisCheck:  Checks for a valid RXN_ephem_t record.

/*

Glonass Ephemeris validation criteria
************************************************************************************************
    MSL Naming     GNSS PE Naming   Range                                   Reference [Document- Page-Doc Verison]
    =========================================================================
    [gloN]              v_Slot                  [range 1..24]                       ICD-p21-V5.1
    [gloFT]             v_FT                    [range 0..15]                       ICD-p-32-V5.1
    [gloM]              v_M                     [range 0..3]                         RRLP - p37- V10
    [gloLn]             v_Bn                   [range 0..7]                         ICD-p-34-V5.1
    [gloFrqCh]        v_FreqChannel     [range -7..+13]                   RRLP - p44- V10
    [gloGamma]     v_Gamma            [range -1024..1023]            RRLP - p38- V10
    [gloXdotdot]     v_Lsx                  [range -16..15]                     RRLP - p32- V10
    [gloYdotdot]     v_Lsy                  [range -16..15]                     RRLP - p32- V10
    [gloZdotdot]     v_Lsz                  [range -16..15]                     RRLP - p32- V10
    [gloTau]           v_TauN               [range -2097152..2097151]    RRLP - p38- V10
    [gloX]              v_X                     [range -67108864..67108863] RRLP - p32- V10
    [gloY]              v_Y                     [range -67108864..67108863] RRLP - p32- V10
    [gloZ]              v_Z                     [range -67108864..67108863] RRLP - p32- V10
    [gloXdot]          v_Vx                   [range -8388608..8388607]   RRLP - p32- V10
    [gloYdot]          v_Vy                   [range -8388608..8388607]   RRLP - p32- V10
    [gloZdot]          v_Vz                   [range -8388608..8388607]   RRLP - p32- V10
************************************************************************************************

*/


BOOL CGPS11_02EEClientGlonEphemerisCheck(const t_cgps_GlonassExtendedEphData *eph,uint32_t  v_Requested_GLONASS_secs )
{
    uint32_t v_TimeDiff = 0;

    

    /*127 is unknown FreqChannel, This is a valid input, this check is updated in the below list */
    if (   eph->v_Slot         <   1 
        || eph->v_Slot         >   24
        || eph->v_FT           >   15
        || eph->v_M            >   3 
        || eph->v_Bn           >   7 
        || eph->v_FreqChannel  <   -7
        || (eph->v_FreqChannel >   13 && eph->v_FreqChannel  !=   127)
        || eph->v_Gamma        <   -1024
        || eph->v_Gamma        >   1023
        || eph->v_Lsx          <   -16
        || eph->v_Lsx          >   15
        || eph->v_Lsy          <   -16
        || eph->v_Lsy          >   15
        || eph->v_Lsz          <   -16
        || eph->v_Lsz          >   15
        || eph->v_TauN         <   -2097152
        || eph->v_TauN         >   2097151
        || eph->v_X            <   -67108864
        || eph->v_X            >   67108863
        || eph->v_Y            <   -67108864
        || eph->v_Y            >   67108863
        || eph->v_Z            <   -67108864
        || eph->v_Z            >   67108863
        || eph->v_Vx           <   -8388608
        || eph->v_Vx           >   8388607
        || eph->v_Vy           <   -8388608
        || eph->v_Vy           >   8388607
        || eph->v_Vz           <   -8388608
        || eph->v_Vz           >   8388607
        || eph->v_GloSec       <=   0
      )

   {
      MC_CGPS_TRACE(( "CGPS11_02EEClientGlonEphemerisCheck:INVALID  v_Slot:%d  v_FreqChannel:%d,v_GloSec=%u,Requested_GLONASS_secs=%u",
               (int32_t)eph->v_Slot,(int32_t)eph->v_FreqChannel,eph->v_GloSec,v_Requested_GLONASS_secs));
      
      return( FALSE );
   }

   v_TimeDiff =  v_Requested_GLONASS_secs - eph->v_GloSec;
   
   MC_CGPS_TRACE(( "CGPS11_02EEClientGlonEphemerisCheck v_Slot:%d v_TimeDiff = %u",(int32_t)eph->v_Slot,v_TimeDiff));
       
    
    if(v_TimeDiff > 900)
    {
        MC_CGPS_TRACE(( "CGPS11_02EEClientGlonEphemerisCheck: OUTDATED,v_TimeDiff = %u ",v_TimeDiff));
        return( FALSE );

    }
    
   MC_CGPS_TRACE(( "CGPS11_02EEClientGlonEphemerisCheck VALID:  v_Slot:%d  v_FreqChannel:%d,v_GloSec:%u,Requested_GLONASS_secs=%u",
               (int32_t)eph->v_Slot,(int32_t)eph->v_FreqChannel,eph->v_GloSec,v_Requested_GLONASS_secs));

   return( TRUE );
}



/*******************************************************************************************************/
/* CGPS11_03ExtendedEphRsp : Reception of Extended Ephemeris data from Client                             */
/*******************************************************************************************************/
const t_OperationDescriptor* CGPS11_03ExtendedEphRsp(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;
    uint8_t                            vl_Index = 0;

    t_lsimee_GetEphemerisRspM * pl_NavDataList = (t_lsimee_GetEphemerisRspM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);
   
    vg_CGPS_NavDataList.v_NumEntriesGps = pl_NavDataList->v_GetEphemeris.v_NumEntriesGps;
    vg_CGPS_NavDataList.v_NumEntriesGlonass = pl_NavDataList->v_GetEphemeris.v_NumEntriesGlonass;

    MC_CGPS_TRACE(("CGPS11_03ExtendedEphRsp NumEntriesGPS =%d NumEntriesGlonass=%d",
        vg_CGPS_NavDataList.v_NumEntriesGps,vg_CGPS_NavDataList.v_NumEntriesGlonass));


    for(vl_Index = 0 ; vl_Index < vg_CGPS_NavDataList.v_NumEntriesGps ; vl_Index++)
    {
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Prn       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Prn;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_CAOrPOnL2 = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_CAOrPOnL2;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Ura       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Ura;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Health    = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Health;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_L2PData   = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_L2PData;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Ure       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Ure;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_AODO      = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_AODO;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_TGD       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_TGD;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Af2       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Af2;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_EphemFit  = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_EphemFit;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_GpsWeek   = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_GpsWeek;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_IoDc      = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_IoDc;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Toc       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Toc;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Toe       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Toe;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Iode      = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Iode;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Af1       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Af1;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Crs       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Crs;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_DeltaN    = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_DeltaN;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Cuc       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Cuc;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Cus       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Cus;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Cic       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Cic;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Cis       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Cis;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Crc       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Crc;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_IDot      = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_IDot;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_E         = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_E;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_SqrtA     = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_SqrtA;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Af0       = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Af0;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_M0        = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_M0;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Omega0    = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_Omega0;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_I0        = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_I0;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_W         = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_W;
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_OmegaDot  = pl_NavDataList->v_GetEphemeris.a_GpsList[vl_Index].v_OmegaDot;
        
        MC_CGPS_TRACE(("CGPS11_03ExtendedEphRsp GPS PRN =%u",
        vg_CGPS_NavDataList.a_GpsList[vl_Index].v_Prn));
  
    }

    for(vl_Index = 0 ; vl_Index < vg_CGPS_NavDataList.v_NumEntriesGlonass ; vl_Index++)
    {

        vg_CGPS_NavDataList.a_GloList[vl_Index].v_Slot          = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_Slot;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_FT            = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_FT;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_M             = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_M;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_Bn            = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_Bn;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_UtcOffset     = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_UtcOffset;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_FreqChannel   = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_FreqChannel;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_Gamma         = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_Gamma;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_Lsx           = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_Lsx;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_Lsy           = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_Lsy;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_Lsz           = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_Lsz;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_GloSec        = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_GloSec;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_TauN          = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_TauN;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_X             = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_X;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_Y             = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_Y;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_Z             = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_Z;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_Vx            = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_Vx;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_Vy            = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_Vy;
        vg_CGPS_NavDataList.a_GloList[vl_Index].v_Vz            = pl_NavDataList->v_GetEphemeris.a_GloList[vl_Index].v_Vz;
        

    }


    return pl_NextState;
}

/*XYBRID Integration :194997*/



const t_OperationDescriptor* CGPS11_04GetRefLocationRsp(t_RtkObject* p_FsmObject)
{
    const t_OperationDescriptor* pl_NextState = SAME;
     s_GN_AGPS_Ref_Pos p_RPo;
    uint8_t vl_ret=0;
    
    memset( &p_RPo , 0 , sizeof( p_RPo ) );

    t_lsimee_GetRefLocationRspM * pl_RefLocation = (t_lsimee_GetRefLocationRspM*)MC_CGPS_GET_MSG_POINTER(p_FsmObject);

    p_RPo.Latitude = pl_RefLocation->v_GetrRefLocation.v_Latitude;
    p_RPo.Longitude = pl_RefLocation->v_GetrRefLocation.v_Longitude;

    p_RPo.RMS_SMaj = (pl_RefLocation->v_GetrRefLocation.v_UncertSemiMajor / 100); /*Centimeter to meter Conversion*/
    p_RPo.RMS_SMin = p_RPo.RMS_SMaj;
    p_RPo.RMS_SMajBrg = 0;
    
    p_RPo.Height_OK = TRUE;
    p_RPo.Height = pl_RefLocation->v_GetrRefLocation.v_Altitude;
    p_RPo.RMS_Height = 1000; /*Unknown Value*/

    MC_CGPSEE_TRACE_REF_POS_FROM_MSL(pl_RefLocation->v_GetrRefLocation);
    MC_CGPSEE_TRACE_REF_POS_DATA(p_RPo);

    
    vl_ret = GN_AGPS_Set_Ref_Pos(&p_RPo);

    
    
    if (vl_ret == FALSE)
    {
        MC_CGPS_TRACE(("CGPS11_04GetRefLocationRsp : library reject the reference position"));
    }
    else
    {
        MC_CGPS_TRACE(("CGPS11_04GetRefLocationRsp : reference position injected to library "));
    }


    return pl_NextState;
}

/*XYBRID Integration :194997*/



//*****************************************************************************
//*****************************************************************************
// CGPS11_05EEClientTimeDiffCheck:  Checks for validity of time for catering a second Extended Ephemeris request.

uint64_t CGPS11_05EEClientTimeDiffCheck(uint64_t EEReqTimestamp)
{
    uint64_t Current_TimeStamp = 0;

    uint64_t Timestamp_diff = 0;

    Current_TimeStamp = OSA_GetMsCount();

    Timestamp_diff = Current_TimeStamp - EEReqTimestamp;

    MC_CGPS_TRACE(( "CGPS11_05EEClientTimeDiffCheck: Time stamp ddifference when request made in msec = %ul",Timestamp_diff));

    return (Timestamp_diff);

}


//*****************************************************************************
//*****************************************************************************
// CGPS11_06EEClientDeleteSeedData:  Delete Extended Ephemeris Seed request.

void CGPS11_06EEClientDeleteSeedData()
{

    t_lsimee_DeleteSeedReq   *pl_lsimee_DeleteSeed;

    pl_lsimee_DeleteSeed = (t_lsimee_DeleteSeedReq*)MC_RTK_GET_MEMORY(sizeof(t_lsimee_DeleteSeedReq));

    MC_CGPS_TRACE(( "CGPS11_06EEClientDeleteSeedData: "));

    /*Clearing the Cache of Extended Ephemeris*/
    memset( &vg_CGPS_NavDataList , 0 , sizeof(vg_CGPS_NavDataList) );


    /*Request for Seed Deletion*/
    CGPS4_28SendMsgToProcess(PROCESS_CGPS ,PROCESS_LSIMEE,
    CGPS_LSIMEE_DELETE_SEED_REQ,(t_MsgHeader *)pl_lsimee_DeleteSeed);

}


 

#undef __CGPS11EE_C__

