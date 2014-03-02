/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
* \file cgps2patch.c
* \date 02/01/2008
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
*     <TR>
*             <TD> 02.04.08 </TD><TD> Y.DESAPHI </TD><TD> CGPS adaptation </TD>
*     </TR>
*     <TR>
*             <TD> 02.09.08 </TD><TD> T.Vivet </TD><TD> Adaptation for dynamic multiple patchs management </TD>
*     </TR>
* </TABLE>
*/
#define __CGPS2PATCH_C__

#undef  MODULE_NUMBER
#define MODULE_NUMBER   MODULE_CGPS

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER  PROCESS_CGPS

#undef  FILE_NUMBER
#define FILE_NUMBER     2

#include "cgpspatch.h"

/* include patch file */
#include "Baseband_Patch_510.c"
#include "Baseband_Patch_730.c"
#include "Baseband_Patch_747.c"
#include "Baseband_Patch_748.c"
#include "Baseband_Patch_950.c"
#include "Baseband_Patch_923.c"
#include "Baseband_Patch_957.c"



/* Static data definitions local to this module */
static U4 Delay_ms_end_1_510;           // OS time when the delay has nominally ended
static U4 Delay_ms_end_2_510;           // OS time after Delay_ms_end_1
/*
CG2900 ROM versions :
PG1.05          - 713 Not supported anymore.
PG1.1 ( Milou ) - 730
PG2.0           - 747
PG2.1           - 748
*/


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
/**************************************************************************************************/
/* CGPS2_00UploadMEPatch :                                                              */
/**************************************************************************************************/
void CGPS2_00UploadMEPatch( U4 Max_Num_Patch_Mess )
{

    /* MC_CGPS_TRACE(("CGPS2_00UploadMEPatch: Max_Num_Patch_Mess=%d gn_ROM_version=%d, gn_Patch_Status=%d", Max_Num_Patch_Mess, gn_ROM_version, gn_Patch_Status)); */
    /* Select the appropriate Patch Upload function for the ROM_version
       identified and saved in CGPS2_01SetupMEPatch() */
    switch ( gn_ROM_version )
    {
    case 510:
    {
        CGPS2_08UploadMEPatch_510( Max_Num_Patch_Mess );
        break;
    }
    case 730:
    {
        if((vg_CGPS_PatchUploadState == K_CGPS_PATCHUPLOAD_FS))
            CGPS2_11UploadMEFSPatchCG29xx(vg_patchFileName,Max_Num_Patch_Mess,vg_PatchFileSize);
        else
            CGPS2_09UploadMEPatchCG29xx( Max_Num_Patch_Mess, vg_CG2900_Patch_730 , sizeof(vg_CG2900_Patch_730));
        break;
    }
    case 747:
    {
        if((vg_CGPS_PatchUploadState == K_CGPS_PATCHUPLOAD_FS))
            CGPS2_11UploadMEFSPatchCG29xx(vg_patchFileName,Max_Num_Patch_Mess,vg_PatchFileSize);
        else
            CGPS2_09UploadMEPatchCG29xx(Max_Num_Patch_Mess, vg_CG2900_Patch_747 , sizeof(vg_CG2900_Patch_747));
        break;
    }
    case 748:
    {
        if((vg_CGPS_PatchUploadState == K_CGPS_PATCHUPLOAD_FS))
            CGPS2_11UploadMEFSPatchCG29xx(vg_patchFileName,Max_Num_Patch_Mess,vg_PatchFileSize);
        else
            CGPS2_09UploadMEPatchCG29xx(Max_Num_Patch_Mess, vg_CG2900_Patch_748 , sizeof(vg_CG2900_Patch_748));
        break;
    }
    case 923:
    {
        if((vg_CGPS_PatchUploadState == K_CGPS_PATCHUPLOAD_FS))
            CGPS2_11UploadMEFSPatchCG29xx(vg_patchFileName,Max_Num_Patch_Mess,vg_PatchFileSize);
        else
            CGPS2_09UploadMEPatchCG29xx(Max_Num_Patch_Mess, vg_CG2900_Patch_923 , sizeof(vg_CG2900_Patch_923));
        break;
    }
    case 950:
    {
        if((vg_CGPS_PatchUploadState == K_CGPS_PATCHUPLOAD_FS))
            CGPS2_11UploadMEFSPatchCG29xx(vg_patchFileName,Max_Num_Patch_Mess,vg_PatchFileSize);
        else
            CGPS2_09UploadMEPatchCG29xx(Max_Num_Patch_Mess, vg_CG2900_Patch_950 , sizeof(vg_CG2900_Patch_950));
        break;
    }
    case 957:
    {
        if((vg_CGPS_PatchUploadState == K_CGPS_PATCHUPLOAD_FS))
            CGPS2_11UploadMEFSPatchCG29xx(vg_patchFileName,Max_Num_Patch_Mess,vg_PatchFileSize);
        else
            CGPS2_09UploadMEPatchCG29xx(Max_Num_Patch_Mess, vg_CG2900_Patch_957 , sizeof(vg_CG2900_Patch_957));
        break;
    }

    case 205:
    case 207:
    case 301:
    case 502:
    case 506:
    case 713:
        /* Support for CG2900 PG1 chip is removed  */
    default :
        MC_DIN_ERROR( BLOCKING, ERROR_1 );
    }

    return;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 1
/**************************************************************************************************/
/* CGPS2_01SetupMEPatch :                                                              */
/**************************************************************************************************/
U2 CGPS2_01SetupMEPatch( U2 ROM_version, U2 Patch_CkSum )
{

    /* Save the GPS baseband ROM verion for CGPS2_00UploadMEPatch */
    gn_ROM_version = ROM_version;

    MC_CGPS_TRACE(("CGPS2_01SetupMEPatch: File patch retries %d",vg_FsPatchUploadCount));
    if(vg_FsPatchUploadCount < K_CGPS_MAX_FS_PATCH_RETRY)
        CGPS2_10CheckMEPatchOnFS(ROM_version);
    else
    {
        MC_CGPS_TRACE(("CGPS2_01SetupMEPatch: ERROR Max file retry done. Fallback to static patch"));
        vg_CGPS_PatchUploadState = K_CGPS_PATCHUPLOAD_FS_ERR;
    }
    MC_CGPS_TRACE(("CGPS2_01SetupMEPatch: Entry. ROM PE=%d CkSum PE =0x%04X CkSum patch = 0x%04X, Patch status = %d", ROM_version, Patch_CkSum, vg_PatchVersion, gn_Patch_Status));

    /* Save the GPS baseband ROM patch version (ie its checksum) for TAT */
    if((vg_CGPS_PatchUploadState == K_CGPS_PATCHUPLOAD_FS))
        vg_PatchVersion = vg_FsPatchCheckSum;
    else
    {
        switch ( gn_ROM_version )
        {
            case 510:
            {
                Delay_ms_end_1_510   = 0;
                Delay_ms_end_2_510   = 0;
                vg_PatchVersion = PatchCheckSum_510;
                break;
            }
            case 730:
            {
                vg_PatchVersion = PatchCheckSum_730;
                break;
            }
            case 747:
            {
                vg_PatchVersion = PatchCheckSum_747;
                break;
            }
            case 748:
            {
                vg_PatchVersion = PatchCheckSum_748;
                break;
            }
            case 923:
            {
                vg_PatchVersion = PatchCheckSum_923;
                break;
            }
            case 950:
            {
                vg_PatchVersion = PatchCheckSum_950;
                break;
            }
            case 957:
            {
                vg_PatchVersion = PatchCheckSum_957;
                break;
            }

            case 205:
            case 207:
            case 301:
            case 502:
            case 506:

            case 713:
                /* Support for CG2900 PG1 chip is removed  */
            default :
            {
                /* Check that we have patch code available for this ROM version*/
                MC_CGPS_TRACE(("CGPS2_01SetupMEPatch : Provided ROM version %d Not supported.", ROM_version));
                MC_DIN_ERROR( BLOCKING, ERROR_1 );
            }
        }
    }



    /* Check that the patch available is not already uploaded correctly*/
    if (( vg_PatchVersion == Patch_CkSum ) && (gn_Patch_Force_Upload != 1) )
    {
        /*CGPS2_01SetupMEPatch: Patch available and already in use*/
        vg_FsPatchUploadCount = 0;
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
    MC_CGPS_TRACE(("CGPS2_01SetupMEPatch: Upload required. ROM PE=%d CkSum PE =0x%04X CkSum patch = 0x%04X, Patch status = %d", ROM_version, Patch_CkSum, vg_PatchVersion, gn_Patch_Status));
    return( vg_PatchVersion );
}




#ifdef GPS_OVER_XOSGPS
#define MAX_PATCH_BUF_LEN 2048
static U1 concatenated_patch_data[MAX_PATCH_BUF_LEN];
#endif

/* + LMSqc32518 */
#ifndef CGPS_USE_COMPRESSED_PATCH
/* - LMSqc32518 */

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8
/**************************************************************************************************/
/* CGPS2_08UploadMEPatch_510 :                                                              */
/**************************************************************************************************/
void CGPS2_08UploadMEPatch_510( U4 Max_Num_Patch_Mess )
{
   const U1*   p_Comm = NULL;  // Pointer to the array of sentence identifiers to send
   const U2*   p_Addr = NULL;  // Pointer to the array of addresses to send
   const U2*   p_Data;         // pointer to the array of data to send
   U2          StartAdr = 0;   // First address of type 23 consecutive addresses
   U1          index;          // Current 'index' of data to be sent
   U1          ckSum;          // Checksum
   U1          CS1;            // First character of checksum in Hex
   U1          CS2;            // Second character of checksum in Hex
   U2          Addr;           // Address to be sent
   U4          i;              // Index into array for checksum calculation
   U4          tot_num;        // Total number of sentences in current stage available
   U2          size;           // Size of patch data to send
   U4          num_sent;       // Number of sentences sent in the current session
   U1          patch_data[30]; // Single sentence to be sent
   U4          tot_mess;       // Total number of messages sent
#ifdef GPS_OVER_XOSGPS
   U4          concat_buf_pos = 0;
#endif

   // When a patch file is available for upload we must write it out here.
   // We must write full sentences only.

    MC_CGPS_TRACE(("CGPS2_08UploadMEPatch_510 ONGOING gn_ROM_version=%d gn_Patch_Status=%d gn_Patch_Progress=%d"
                   " gn_Cur_Mess={%d,%d,%d,%d,%d}", gn_ROM_version, gn_Patch_Status, gn_Patch_Progress,
           gn_Cur_Mess[0], gn_Cur_Mess[1],gn_Cur_Mess[2],gn_Cur_Mess[3],gn_Cur_Mess[4]));

   switch( gn_Patch_Status )
   {
         case 1:               // First stage
         {
            index    = 0;
            tot_num  = NUM_1_510;
            p_Comm   = &Comm1_510[gn_Cur_Mess[index]];
            p_Addr   = &Addr1_510[gn_Cur_Mess[index]];
            p_Data   = &Data1_510[gn_Cur_Mess[index]];
            tot_mess = gn_Cur_Mess[0];
            break;
         }
         case 2:               // Second stage
         {                     // All the second stage are COMD 23
            index    = 1;      // and are consecutive addresses
            tot_num  = NUM_2_510;
            StartAdr = START_ADDR_2_510;
            p_Data   = &Data2_510[gn_Cur_Mess[index]];
            tot_mess = NUM_1_510 + gn_Cur_Mess[1];
            break;
         }
         case 3:               // Third stage
         {
            index    = 2;
            tot_num  = NUM_3_510;
            p_Comm   = &Comm3_510[gn_Cur_Mess[index]];
            p_Addr   = &Addr3_510[gn_Cur_Mess[index]];
            p_Data   = &Data3_510[gn_Cur_Mess[index]];
            tot_mess = NUM_1_510 + NUM_2_510 + gn_Cur_Mess[2];
            break;
         }
         case 4:
         {                    // All the 4th stage are COMD 23
            index    = 3;     // and are consecutive addresses
            tot_num  = NUM_4_510;
            StartAdr = START_ADDR_4_510;
            p_Data   = &Data4_510[gn_Cur_Mess[index]];
            tot_mess = NUM_1_510 + NUM_2_510  + NUM_3_510 + gn_Cur_Mess[3];
            break;
         }
         case 5:             // Fifth stage
         {
            index    = 4;
            tot_num  = NUM_5_510;
            p_Comm   = &Comm5_510[gn_Cur_Mess[index]];
            p_Addr   = &Addr5_510[gn_Cur_Mess[index]];
            p_Data   = &Data5_510[gn_Cur_Mess[index]];
            tot_mess = NUM_1_510 + NUM_2_510  + NUM_3_510 + NUM_4_510 + gn_Cur_Mess[4];
            break;
         }
         default : return;          // Error
   }

   // Check if we are in a delay period. If we are, get the current OS ms time
   // to see if the delay has been completed.
   if ( tot_mess == DELAY_CMD_NUM )
   {
      // We need to add the delay here
      if ( Delay_ms_end_1_510 == 0 )
      {
         // This is the very beginning of the delay, so record the (nominal)
         // time at which the delay ends (i.e. the current ms time plus the delay period).
         // Also, send a blank line so that the location of the delay can be identified
         // for debug purposes, and return
         Delay_ms_end_1_510   = GN_GPS_Get_OS_Time_ms( ) + DELAY_MS;
         Delay_ms_end_2_510   = 0;
         GN_GPS_Write_GNB_Ctrl(1, (CH*)"\n");  // For Debug visibility
         {
            U2 num;
            CH  temp_buffer[128];
/* ++LMSqb95231  */
            num = snprintf( temp_buffer, sizeof(temp_buffer),"CGPS2_08UploadMEPatch_510: Starting delay period at       %8d %8d\n\r",
                           (Delay_ms_end_1_510-DELAY_MS), Delay_ms_end_1_510 );
/* --LMSqb95231  */
            GN_GPS_Write_Event_Log( num, (CH*)temp_buffer );
         }
         return;
      }
      else
      {
         // We've already started the delay period, so see if it's completed
         U4 currOStime;

         currOStime = GN_GPS_Get_OS_Time_ms( );
         if ( Delay_ms_end_2_510 == 0 )
         {
            // We're still waiting for the nominal delay period to be completed
            if ( currOStime >= Delay_ms_end_1_510 )
            {
               // We've reached the end of the (nomonal) delay period, but we don't
               // know what the resolution of the timer is, so we can't be completely
               // sure that the period has actually been exceeded. So, we wait
               // until the GN_GPS_Get_OS_Time_ms function returns a different
               // value - in effect, we add a period equal to the at least the
               // resolution of the timer to the delay period.
               Delay_ms_end_2_510 = currOStime;
               {
                  U2 num;
                  CH  temp_buffer[128];
/* ++LMSqb95231  */
                  num = snprintf( temp_buffer, sizeof(temp_buffer),
                                 "CGPS2_08UploadMEPatch_510: End of nominal delay period at %8d\n\r",
                                 Delay_ms_end_2_510 );
/* --LMSqb95231  */
                  GN_GPS_Write_Event_Log( num, (CH*)temp_buffer );
               }
            }
            return;  // Carry on waiting
         }
         else if ( Delay_ms_end_2_510 == currOStime )
         {
            return;  // Carry on waiting
         }
         else
         {
            U2 num;
            CH  temp_buffer[128];
/* ++LMSqb95231  */
            num = snprintf( temp_buffer, sizeof(temp_buffer),
                           "CGPS2_08UploadMEPatch_510: End of delay period at         %8d\n\r",
                           currOStime );
/* --LMSqb95231  */
            GN_GPS_Write_Event_Log( num, (CH*)temp_buffer );
         }
      }
   }

   // Re-constitute the next message to be sent
   num_sent = 0;
   while ( (num_sent < Max_Num_Patch_Mess) && (gn_Cur_Mess[index] < tot_num) )
   {
      if ( (index == 1) || (index == 3) )
      {
         // All the index 1 and 3 sentences are of type 23, with consecutive addresses
         size = 25;
         Addr = StartAdr + gn_Cur_Mess[index];
/* ++LMSqb95231  */
         snprintf( (char*)patch_data, sizeof(patch_data), "#COMD 23 %05d %05d &  \n\r",
                  Addr, p_Data[num_sent] );
/* --LMSqb95231  */
      }
      else
      {
         if ( p_Comm[num_sent] == 11 )
         {
            size = 15;
/* ++LMSqb95231  */
            snprintf( (char*)patch_data, sizeof(patch_data), "#COMD %2d %1d &  \n\r",
                     p_Comm[num_sent], p_Addr[num_sent]);
/* --LMSqb95231  */
         }
         else if ( p_Comm[num_sent] == 25 )
         {
            size = 19;
/* ++LMSqb95231  */
            snprintf( (char*)patch_data, sizeof(patch_data), "#COMD %2d %05d &  \n\r",
                     p_Comm[num_sent], p_Addr[num_sent]);
/* --LMSqb95231  */
         }
         else
         {
            size = 25;
/* ++LMSqb95231  */
            snprintf( (char*)patch_data, sizeof(patch_data), "#COMD %2d %05d %05d &  \n\r",
                     p_Comm[num_sent], p_Addr[num_sent], p_Data[num_sent]);
/* --LMSqb95231  */
         }
      }

      // Add the checksum
      ckSum = 0;   // Checksum counter.
      i     = 1;   // Don't include the '#' in the checksum validation.
      do
      {
         ckSum = (U1)(ckSum + patch_data[i++]);
      } while ( patch_data[i] != '&' );
      i++;
      CS1 = ckSum/16;
      if ( CS1 <= 9 )  patch_data[i] = (CH)(CS1+48);
      else             patch_data[i] = (CH)(CS1+55);
      i++;
      CS2 = ckSum % 16;
      if ( CS2 <= 9 )  patch_data[i] = (CH)(CS2+48);
      else             patch_data[i] = (CH)(CS2+55);

#ifdef GPS_OVER_XOSGPS
      if(concat_buf_pos+size > MAX_PATCH_BUF_LEN)
      {
         // remaining space too small. Purge
        GN_GPS_Write_GNB_Ctrl( concat_buf_pos, (CH*)concatenated_patch_data );
        concat_buf_pos = 0;
      }

      // store the data for later send
      memcpy(concatenated_patch_data+concat_buf_pos, patch_data, size);
      concat_buf_pos+=size;
#else
      // Send the data
      if ( GN_GPS_Write_GNB_Ctrl( size, (CH*)patch_data ) != size )
      {
            /*"CGPS2_00UploadMEPatch: ERROR:  Some Patch data not taken by UART TX driver*/
      }
#endif
      gn_Cur_Mess[index]++;
      num_sent++;
   }

#ifdef GPS_OVER_XOSGPS
      // Send the data remaining
      if(concat_buf_pos)
      {
        GN_GPS_Write_GNB_Ctrl( concat_buf_pos, (CH*)concatenated_patch_data );
      }
#endif

   // Check if all the data has been sent
   if ( gn_Cur_Mess[index] == tot_num )
   {
      gn_Patch_Status++;
   }

   if ( gn_Patch_Status == 6 )
   {
      MC_CGPS_TRACE(("CGPS2_08UploadMEPatch_510: COMPLETED. gn_ROM_version=%d, gn_Patch_Status=%d", gn_ROM_version, gn_Patch_Status));

      // Clear the counter of messages sent ready for a future time.
      gn_Patch_Progress = 100;

        gn_Cur_Mess[0]    = 0;
        gn_Cur_Mess[1]    = 0;
        gn_Cur_Mess[2]    = 0;
        gn_Cur_Mess[3]    = 0;
        gn_Cur_Mess[4]    = 0;

        gn_Patch_Status   = 7;   // Flag the end of the patch data
    }

    return;
}
/* + LMSqc32518 */


#else /* CGPS_USE_COMPRESSED_PATCH */



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 2
/**************************************************************************************************/
/* CGPS2_02Patch510ProcessDelay  : return TRUE if delay in progress. Return FALSE if delay done*/
/**************************************************************************************************/
uint8_t CGPS2_02Patch510ProcessDelay()
{
    // We need to add the delay here
    if ( Delay_ms_end_1_510 == 0 )
    {
        // This is the very beginning of the delay, so record the (nominal)
        // time at which the delay ends (i.e. the current ms time plus the delay period).
        // Also, send a blank line so that the location of the delay can be identified
        // for debug purposes, and return
        Delay_ms_end_1_510   = GN_GPS_Get_OS_Time_ms( ) + DELAY_MS;
        Delay_ms_end_2_510   = 0;
        GN_GPS_Write_GNB_Ctrl(1, (CH*)"\n");  // For Debug visibility
        {
            U2 num;
            CH  temp_buffer[128];
            /* ++LMSqb95231  */
            num = snprintf( temp_buffer, sizeof(temp_buffer),"CGPS2_02Patch510ProcessDelay: Starting delay period at       %8d %8d\n\r",
                            (Delay_ms_end_1_510-DELAY_MS), Delay_ms_end_1_510 );
            /* --LMSqb95231  */
            GN_GPS_Write_Event_Log( num, (CH*)temp_buffer );
            MC_CGPS_TRACE(((CH*)temp_buffer));
        }
        return TRUE;
    }
    else
    {
        // We've already started the delay period, so see if it's completed
        U4 currOStime;

        currOStime = GN_GPS_Get_OS_Time_ms( );
        if ( Delay_ms_end_2_510 == 0 )
        {
            // We're still waiting for the nominal delay period to be completed
            if ( currOStime >= Delay_ms_end_1_510 )
            {
                // We've reached the end of the (nomonal) delay period, but we don't
                // know what the resolution of the timer is, so we can't be completely
                // sure that the period has actually been exceeded. So, we wait
                // until the GN_GPS_Get_OS_Time_ms function returns a different
                // value - in effect, we add a period equal to the at least the
                // resolution of the timer to the delay period.
                Delay_ms_end_2_510 = currOStime;
                {
                    U2 num;
                    CH  temp_buffer[128];
                    /* ++LMSqb95231  */
                    num = snprintf( temp_buffer, sizeof(temp_buffer),
                                    "CGPS2_02Patch510ProcessDelay: End of nominal delay period at %8d\n\r",
                                    Delay_ms_end_2_510 );
                    /* --LMSqb95231  */
                    GN_GPS_Write_Event_Log( num, (CH*)temp_buffer );
                    MC_CGPS_TRACE(((CH*)temp_buffer));
                }
            }
            return TRUE;  // Carry on waiting
        }
        else if ( Delay_ms_end_2_510 == currOStime )
        {
            return TRUE;  // Carry on waiting
        }
        else
        {
            U2 num;
            CH  temp_buffer[128];
            /* ++LMSqb95231  */
            num = snprintf( temp_buffer, sizeof(temp_buffer),
                            "CGPS2_02Patch510ProcessDelay: End of delay period at         %8d\n\r",
                            currOStime );
            /* --LMSqb95231  */
            GN_GPS_Write_Event_Log( num, (CH*)temp_buffer );
            MC_CGPS_TRACE(((CH*)temp_buffer));
            return FALSE;
        }
    }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 3
/**************************************************************************************************/
/* CGPS2_03Patch510CalcCheckSum  : Calculate a checksum to the string upto the '&' character.
                                                                       The checksum does not include the first character (which is a '#'). */
/**************************************************************************************************/
static uint8_t CGPS2_03Patch510CalcCheckSum(const char *Message_p)
{
    uint8_t CheckSum = 0;   // Checksum counter.
    int i = 1;  // Don't include the # in the checksum calculation

    do
    {
        CheckSum += (U1) Message_p[i++];
    } while ( Message_p[i] != '&' );

    return CheckSum;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 4
/**************************************************************************************************/
/* CGPS2_04Patch510AddMessageTail  : Add the '&' character the check sum and the '\n'.
*              The checksum does not include the first character (which is a '#').
* Parameters:
*                Message_p       pointer to start odf message.
*                MessageInsert_p pointer to end of message, where the tail is to be added.
* Returns:       Pointer to terminating null.                                                                            */
/**************************************************************************************************/
char * CGPS2_04Patch510AddMessageTail (const char *const Message_p, char *MessageInsert_p)
{
    *MessageInsert_p++ = '&';

    sprintf (MessageInsert_p, "%02X", CGPS2_03Patch510CalcCheckSum(Message_p));
    // Skip the two character checksum.
    MessageInsert_p++ ;
    MessageInsert_p++ ;

    // Teminating new line (it is required !)
    *MessageInsert_p++ = '\n';

    // null terminate the string for safety.
    *MessageInsert_p = 0;

    return MessageInsert_p;
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 5
/**************************************************************************************************/
/* CGPS2_05Patch510SendGeneralCommands  :                                             */
/**************************************************************************************************/
static void CGPS2_05Patch510SendGeneralCommands (int NumCommands, const uint8_t *Command_p, const uint16_t *Address_p, const uint16_t *Data_p)
{
    char Message[26];
    uint8_t CommandsSent = 0;

    while (NumCommands)
    {
        const uint8_t Command = *Command_p;
        const uint16_t Address = *Address_p;
        const uint16_t Data    = *Data_p;
        uint16_t Size;

        if ( Command == 11 )
        {
            Size = 15;
            sprintf( Message ,"#COMD 11 %1d ", Address);
        }
        else if ( Command == 25 )
        {
            Size = 19;
            sprintf( Message, "#COMD 25 %05d ", Address);
        }
        else
        {
            Size = 25;
            sprintf( Message, "#COMD %2d %05d %05d ", Command, Address, Data);
        }

        (void)CGPS2_04Patch510AddMessageTail(Message, strchr(Message, '\0'));
        (void)GN_GPS_Write_GNB_Ctrl(Size, Message);
        CommandsSent++;

        NumCommands --;
        Address_p++;
        Command_p++;
        Data_p++;
    }
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 6
/**************************************************************************************************/
/* CGPS2_06Patch510InsertWordIntoMessage  : Stores the data as hex.  We don't use sprintf because we don't want this to be slow.
* Parameters:
*                MessageInsert_p    Place to insert word.
*                Data               The word to be inserted.
* Returns:       None.                                                                            */
/**************************************************************************************************/
static char *CGPS2_06Patch510InsertWordIntoMessage (char *MessageInsert_p, uint16_t Data)
{
    static const char ToHex[]="0123456789ABCDEF";

    if (Data > 0x0FFF)
    {
        MessageInsert_p[4] = ' ';
        MessageInsert_p[3] = ToHex[ (Data & 0x0f) ]; Data >>= 4;
        MessageInsert_p[2] = ToHex[ (Data & 0x0f) ]; Data >>= 4;
        MessageInsert_p[1] = ToHex[ (Data & 0x0f) ]; Data >>= 4;
        MessageInsert_p[0] = ToHex[ Data];
        MessageInsert_p += 5;
    }
    else if (Data > 0x0FF)
    {
        MessageInsert_p[3] = ' ';
        MessageInsert_p[2] = ToHex[ (Data & 0x0f) ]; Data >>= 4;
        MessageInsert_p[1] = ToHex[ (Data & 0x0f) ]; Data >>= 4;
        MessageInsert_p[0] = ToHex[ Data];
        MessageInsert_p += 4;
    }
    else if (Data > 0x000F)
    {
        MessageInsert_p[2] = ' ';
        MessageInsert_p[1] = ToHex[ (Data & 0x0f) ]; Data >>= 4;
        MessageInsert_p[0] = ToHex[ Data ];
        MessageInsert_p += 3;
    }
    else
    {
        MessageInsert_p[1] = ' ';
        MessageInsert_p[0] = ToHex[Data];
        MessageInsert_p += 2;
    }
    return MessageInsert_p;
}



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 7
/**************************************************************************************************/
/* CGPS2_07Patch510SendDataCommands  : Send a block of data using "COMD 6".  This command is foramtted as follows:
*                                                                 #COMD 6 Num StartAdress dddd dddd dddd dddd
*                                                                 where
*                                                                 Num                is number of words as a decimal number
*                                                                 StartAdress      the adress where the data is to go in hex
*                                                                 dddd                data items in hex, no padding required
* Parameters:
*              NumWords        NUmber of words.
*              StartAdress     The adress for where the data is to go in the device.
*              Data_p          Pointer to array of data items.
* Returns:     None.                                                                             */
/**************************************************************************************************/
static void CGPS2_07Patch510SendDataCommands(int NumWords, uint16_t StartAdress, const uint16_t *Data_p)
{
  const int MIN_MESSAGE_LENGTH = 12;
  char *Message_p = MC_RTK_GET_MEMORY(MIN_MESSAGE_LENGTH+K_CGPS_ITEMS_PER_MESSAGE*5+20);
  char *MessageInsert_p = Message_p;
  int MessageItems;

  if (Message_p)
  {
    while (NumWords >= K_CGPS_ITEMS_PER_MESSAGE)
    {
      MessageItems = K_CGPS_ITEMS_PER_MESSAGE;
      MessageInsert_p = Message_p;
#if K_CGPS_ITEMS_PER_MESSAGE == 16
      sprintf(MessageInsert_p, "#COMD 6 16 %X ",  StartAdress);
#else
      sprintf(MessageInsert_p, "#COMD 6 %d %X ",  MessageItems, StartAdress);
#endif

      // Find the end of the string.
      MessageInsert_p = strchr(MessageInsert_p+MIN_MESSAGE_LENGTH, 0);

      while (MessageItems>0)
      {
        MessageInsert_p = CGPS2_06Patch510InsertWordIntoMessage(MessageInsert_p, *Data_p++);
        MessageItems--;
      }
      MessageInsert_p = CGPS2_04Patch510AddMessageTail(Message_p, MessageInsert_p);
      (void)GN_GPS_Write_GNB_Ctrl( (uint16_t)(MessageInsert_p-Message_p), Message_p);

      NumWords -= K_CGPS_ITEMS_PER_MESSAGE;
      StartAdress += K_CGPS_ITEMS_PER_MESSAGE;
    }

    // Do the runt of the message
    if (NumWords > 0)
    {
      MessageItems = NumWords;
      MessageInsert_p = Message_p;
      sprintf(MessageInsert_p, "#COMD 6 %d %X ",  MessageItems, StartAdress);

      // Find the end of the string.
      MessageInsert_p = strchr(MessageInsert_p+MIN_MESSAGE_LENGTH, 0);

      while (MessageItems>0)
      {
        MessageInsert_p = CGPS2_06Patch510InsertWordIntoMessage(MessageInsert_p, *Data_p++);
        MessageItems--;
      }
      MessageInsert_p = CGPS2_04Patch510AddMessageTail(Message_p, MessageInsert_p);
      (void)GN_GPS_Write_GNB_Ctrl( (uint16_t)(MessageInsert_p-Message_p), Message_p);
    }
  }
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 8
/**************************************************************************************************/
/* CGPS2_08UploadMEPatch_510  :                                                            */
/**************************************************************************************************/
void CGPS2_08UploadMEPatch_510( U4 Max_Num_Patch_Mess )
{
   const U1*   p_Comm;         // Pointer to the array of sentence identifiers to send
   const U2*   p_Addr;         // Pointer to the array of addresses to send
   const U2*   p_Data;         // pointer to the array of data to send
   U2          StartAdr;       // First address of type 23 consecutive addresses
   U1          index;          // Current 'index' of data to be sent
   U1          ckSum;          // Checksum
   U1          CS1;            // First character of checksum in Hex
   U1          CS2;            // Second character of checksum in Hex
   U2          Addr;           // Address to be sent
   U4          i;              // Index into array for checksum calculation
   U4          tot_num;        // Total number of sentences in current stage available
   U2          size;           // Size of patch data to send
   U4          num_sent;       // Number of sentences sent in the current session
   U1          patch_data[30]; // Single sentence to be sent
   U4          tot_mess;       // Total number of messages sent
#ifdef GPS_OVER_XOSGPS
   U4          concat_buf_pos = 0;
#endif

   // When a patch file is available for upload we must write it out here.
   // We must write full sentences only.

    MC_CGPS_TRACE(("CGPS2_08UploadMEPatch_510 ONGOING gn_ROM_version=%d gn_Patch_Status=%d gn_Patch_Progress=%d"
                   " gn_Cur_Mess={%d,%d,%d,%d,%d}", gn_ROM_version, gn_Patch_Status, gn_Patch_Progress,
           gn_Cur_Mess[0], gn_Cur_Mess[1],gn_Cur_Mess[2],gn_Cur_Mess[3],gn_Cur_Mess[4]));

   /* Max_Num_Patch_Messages expands to 25*Max_Num_Patch_Messages bytes of data.
          For general patch commands, Number of commands to be sent is same as Max_Num_Patch_Messages
          For data commands, it is more complicated.

          General Commands are sent in patch status 1,3 & 5
          Data is sent in patch status 2 & 4
    */

   switch( gn_Patch_Status )
   {
         case 1:               // First stage
         {
            index    = 0;
            tot_num  = NUM_1_510;
            p_Comm   = &Comm1_510[gn_Cur_Mess[index]];
            p_Addr   = &Addr1_510[gn_Cur_Mess[index]];
            p_Data   = &Data1_510[gn_Cur_Mess[index]];
            tot_mess = gn_Cur_Mess[0];

            /* If the number of messages to be sent is more than existing messages in this state, change it */
            if( Max_Num_Patch_Mess + gn_Cur_Mess[index] > tot_num )
            {
                Max_Num_Patch_Mess = tot_num - gn_Cur_Mess[index];
            }

            CGPS2_05Patch510SendGeneralCommands(Max_Num_Patch_Mess,
                                     p_Comm,
                                     p_Addr,
                                     p_Data);

            gn_Cur_Mess[index] += Max_Num_Patch_Mess;

            break;
         }
         case 2:               // Second stage
         {                     // All the second stage are COMD 23
            index    = 1;      // and are consecutive addresses
            tot_num  = NUM_2_510;
            StartAdr = START_ADDR_2_510 + gn_Cur_Mess[index];
            p_Data   = &Data2_510[gn_Cur_Mess[index]];
            tot_mess = NUM_1_510 + gn_Cur_Mess[1];

            /* For data commands, we send 16 Data packets at one go */
            /* Each 16 data packet consists of 12+16*5+20 = 112 bytes*/
            /* If we are asked to send X message, we can send 25*X bytes. */
            /* To calculate number of data commands, 25*X / 112 * 116  gives us number of commands */
            /* Of the remaining, subtract 32 bytes as this is always same. Of the remainder, calculate number of
                        commands to send */
            {
                uint16_t numCommands = 0;
                uint16_t numBytesToSend = 25*Max_Num_Patch_Mess;
                uint16_t numLeftoverBytes;

                numCommands = ( numBytesToSend / 112 ) * 16;

                numLeftoverBytes = numBytesToSend % 112;

                if( numLeftoverBytes > 32 )
                {
                    numCommands += ( numLeftoverBytes - 32 ) / 5;
                }

                if( numCommands + gn_Cur_Mess[index] > tot_num )
                {
                    numCommands = tot_num - gn_Cur_Mess[index];
                }

                CGPS2_07Patch510SendDataCommands(numCommands,
                                      StartAdr,
                                      p_Data);
                gn_Cur_Mess[index] += numCommands;
            }

            break;
         }
         case 3:               // Third stage
         {
            index    = 2;
            tot_num  = NUM_3_510;
            p_Comm   = &Comm3_510[gn_Cur_Mess[index]];
            p_Addr   = &Addr3_510[gn_Cur_Mess[index]];
            p_Data   = &Data3_510[gn_Cur_Mess[index]];
            tot_mess = NUM_1_510 + NUM_2_510 + gn_Cur_Mess[2];

            /* If the number of messages to be sent is more than existing messages in this state, change it */
            if( Max_Num_Patch_Mess + gn_Cur_Mess[index] > tot_num )
            {
                Max_Num_Patch_Mess = tot_num - gn_Cur_Mess[index];
            }

            CGPS2_05Patch510SendGeneralCommands(Max_Num_Patch_Mess,
                                     p_Comm,
                                     p_Addr,
                                     p_Data);

            gn_Cur_Mess[index] += Max_Num_Patch_Mess;

            break;
         }

         case 4:
         {                    // All the 4th stage are COMD 23
            index    = 3;     // and are consecutive addresses
            tot_num  = NUM_4_510;
            StartAdr = START_ADDR_4_510;
            p_Data   = &Data4_510[gn_Cur_Mess[index]];
            tot_mess = NUM_1_510 + NUM_2_510  + NUM_3_510 + gn_Cur_Mess[3];


            // Check if we are in a delay period. If we are, get the current OS ms time
            // to see if the delay has been completed.
            if ( tot_mess == DELAY_CMD_NUM )
            {
               if( CGPS2_02Patch510ProcessDelay() )
               {
                 return;
               }
            }

            /* For data commands, we send 16 Data packets at one go */
            /* Each 16 data packet consists of 12+16*5+20 = 112 bytes*/
            /* If we are asked to send X message, we can send 25*X bytes. */
            /* To calculate number of data commands, 25*X / 112 * 116  gives us number of commands */
            /* Of the remaining, subtract 32 bytes as this is always same. Of the remainder, calculate number of
                        commands to send */
            {
                uint16_t numCommands = 0;
                uint16_t numBytesToSend = 25*Max_Num_Patch_Mess;
                uint16_t numLeftoverBytes;

                numCommands = ( numBytesToSend / 112 ) * 16;

                numLeftoverBytes = numBytesToSend % 112;

                if( numLeftoverBytes > 32 )
                {
                    numCommands += ( numLeftoverBytes - 32 ) / 5;
                }

                if( numCommands + gn_Cur_Mess[index] > tot_num )
                {
                    numCommands = tot_num - gn_Cur_Mess[index];
                }

                CGPS2_07Patch510SendDataCommands(numCommands,
                                      StartAdr + gn_Cur_Mess[index],
                                      p_Data);
                gn_Cur_Mess[index] += numCommands;
            }

            break;
         }
         case 5:             // Fifth stage
         {
            index    = 4;
            tot_num  = NUM_5_510;
            p_Comm   = &Comm5_510[gn_Cur_Mess[index]];
            p_Addr   = &Addr5_510[gn_Cur_Mess[index]];
            p_Data   = &Data5_510[gn_Cur_Mess[index]];
            tot_mess = NUM_1_510 + NUM_2_510  + NUM_3_510 + NUM_4_510 + gn_Cur_Mess[4];

            /* If the number of messages to be sent is more than existing messages in this state, change it */
            if( Max_Num_Patch_Mess + gn_Cur_Mess[index] > tot_num )
            {
                Max_Num_Patch_Mess = tot_num - gn_Cur_Mess[index];
            }

            CGPS2_05Patch510SendGeneralCommands(Max_Num_Patch_Mess,
                                     p_Comm,
                                     p_Addr,
                                     p_Data);

            gn_Cur_Mess[index] += Max_Num_Patch_Mess;

            break;
         }
         default : return;          // Error
   }

   // Check if all the data has been sent
   if ( gn_Cur_Mess[index] == tot_num )
   {
      gn_Patch_Status++;
   }

   if ( gn_Patch_Status == 6 )
   {
      MC_CGPS_TRACE(("CGPS2_08UploadMEPatch_510: COMPLETED. gn_ROM_version=%d, gn_Patch_Status=%d", gn_ROM_version, gn_Patch_Status));

      // Clear the counter of messages sent ready for a future time.
      gn_Patch_Progress = 100;

        gn_Cur_Mess[0]    = 0;
        gn_Cur_Mess[1]    = 0;
        gn_Cur_Mess[2]    = 0;
        gn_Cur_Mess[3]    = 0;
        gn_Cur_Mess[4]    = 0;

        gn_Patch_Status   = 7;   // Flag the end of the patch data
    }

    return;
}

#endif /* CGPS_USE_COMPRESSED_PATCH */



#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 9
/**************************************************************************************************/
/* CGPS2_09UploadMEPatchCG29xx  : Upload patch to a CG2900 chip, for different ROM version - 730, 747,748
*                                                       Upload the next set of patch code to the a CG2900 GPS baseband.
*                                                       This function sends up to a maximum of Max_Num_Patch_Mess sentences each time it is called.
*                                                        The complete set of patch data is divided into six stages.
* Parameters:
*              Max_Num_Patch_Mess  Maximum Number of Patch Messages to Upload
* Returns:     None.                                                                             */
/**************************************************************************************************/
void CGPS2_09UploadMEPatchCG29xx( U4 Max_Num_Patch_Mess , const U1 *pp_CG2900_Patch, U2 vp_PatchLength)
{
    if( gn_Patch_Status == 1 )
    {
        /* We always send Max_Num_Patch_Mess bytes of data at a time */
        U2 bytesWritten = 0;
        U2 patchLength = vp_PatchLength - 1;

        if( patchLength < Max_Num_Patch_Mess + gn_Cur_Mess[0] )
        {
            Max_Num_Patch_Mess= patchLength - gn_Cur_Mess[0];
            MC_CGPS_TRACE(("CGPS2_09UploadMEPatchCG29xx: Write leftover %u bytes" , Max_Num_Patch_Mess ));
        }

        /* Search for line ending with 0xA */
        while( pp_CG2900_Patch[gn_Cur_Mess[0]+Max_Num_Patch_Mess-1] != 0xA )
        {
            Max_Num_Patch_Mess--;
        }

        bytesWritten = GN_GPS_Write_GNB_Ctrl(Max_Num_Patch_Mess,(CH *)(pp_CG2900_Patch + gn_Cur_Mess[0]));

        gn_Cur_Mess[0] += bytesWritten;

        MC_CGPS_TRACE(("CGPS2_09UploadMEPatchCG29xx: Written %u bytes , total written %u bytes" , bytesWritten, gn_Cur_Mess[0] ));

        if( patchLength == gn_Cur_Mess[0] )
        {
            gn_Patch_Status = 7; /* End of patch */
            gn_Cur_Mess[0] = 0;

            MC_CGPS_TRACE(("CGPS2_09UploadMEPatchCG29xx: COMPLETED. gn_ROM_version=%d, gn_Patch_Status=%d", gn_ROM_version, gn_Patch_Status));
        }
    }
}
/**
 *  \fn     void CGPS2_10CheckMEPatchOnFS( U2 ROM_version )
 */

#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 10
/**************************************************************************************************/
/* CGPS2_10CheckMEPatchOnFS  :                                            */
/**************************************************************************************************/
void CGPS2_10CheckMEPatchOnFS( U2 ROM_version )
{
    const char *dir_path=(char *)K_CGPS_PATCHUPLOAD_FS_PATH;
    directory *DirPath;
    int8_t patchFsName[80]={0,};
    uint8_t NoOfPatchFs=0;
    DirectoryPath *PatchFsPath = NULL;
    int8_t*         TempCheckSum = NULL;
    uint8_t TempFsName[80]={0,};
    DirPath= MC_GFL_OPENDIR(dir_path);
    if(DirPath== NULL)
    {
        vg_CGPS_PatchUploadState = K_CGPS_PATCHUPLOAD_STATIC;
        return;
    }
    // print the file name for Eg. GNSS_E748....
    sprintf((char *)patchFsName, "GNSS_E%d",ROM_version);
    // check in the folder for files
    while((PatchFsPath = MC_GFL_READDIR(DirPath))!=NULL)   //there are some files/folders available
    {
        //check for the patch file presence in the folder
        if(strncmp(PatchFsPath->d_name,(const char *)patchFsName,strlen((char *)patchFsName)) == 0)
        {
            NoOfPatchFs++;
            MC_CGPS_TRACE(("CGPS2_10CheckMEPatchOnFS:%s, %d",PatchFsPath->d_name,NoOfPatchFs));
            //complete file path of the patch file
            sprintf((char *)vg_patchFileName,"%s%s",dir_path,PatchFsPath->d_name);
            strcpy((char *)TempFsName,(const char *)vg_patchFileName);
            MC_CGPS_TRACE(("CGPS2_10CheckMEPatchOnFS:%s",vg_patchFileName));
            //check for valid patch file
            if((strstr((const char *)vg_patchFileName,".")))
            {
                TempCheckSum = (int8_t*)strtok ((char *)TempFsName,".");
                TempCheckSum = (int8_t*)strtok (NULL,".");

                // check for the file size if valid size set to FS upload else static
                FileStatistics vl_FileStat;

                // check for the valid checksum
                if(strlen((const char *)TempCheckSum) == 4)
                {
                    MC_CGPS_TRACE(("CGPS2_10CheckMEPatchOnFS: PatchCheckSum %s",TempCheckSum));
                    vg_FsPatchCheckSum = (MC_CGPS_HEX_TO_DECIMAL(TempCheckSum[0]))<<12|(MC_CGPS_HEX_TO_DECIMAL(TempCheckSum[1]))<<8|(MC_CGPS_HEX_TO_DECIMAL(TempCheckSum[2]))<<4|(MC_CGPS_HEX_TO_DECIMAL(TempCheckSum[3]));

                    if(MC_GFL_STAT((const char *)vg_patchFileName,&vl_FileStat) )
                         MC_CGPS_TRACE(("CGPS2_10CheckMEPatchOnFS:cannot stat %s\n",vg_patchFileName));

                    else
                    {
                        vg_PatchFileSize = vl_FileStat.st_size;
                        MC_CGPS_TRACE(("CGPS2_10CheckMEPatchOnFS: vg_PatchFileSize %d",vg_PatchFileSize));

                        if(vg_PatchFileSize)
                            vg_CGPS_PatchUploadState = K_CGPS_PATCHUPLOAD_FS;
                    }
                    break;
                }

            }
            else
                MC_CGPS_TRACE(("CGPS2_10CheckMEPatchOnFS: missing '.'"));
        }
        else
        {
            if(NoOfPatchFs == 0)
                vg_CGPS_PatchUploadState = K_CGPS_PATCHUPLOAD_STATIC;
        }

    }
    MC_GFL_CLOSEDIR(DirPath);
}


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 11
/**************************************************************************************************/
/* CGPS2_11UploadMEFSPatchCG29xx  :                                            */
/**************************************************************************************************/
void CGPS2_11UploadMEFSPatchCG29xx( int8_t * vp_patchFileName, U4 Max_Num_Patch_Mess, U2 vp_PatchLength)
{
    U2 vl_BytesWritten   = 0;
    U2 vl_PatchLength    = vp_PatchLength;
    U2 vl_ReadCount      = 0;

   // patch file open for only once
   if(gn_Patch_Status == 1)
    {
        MC_CGPS_TRACE(("CGPS2_11UploadMEFSPatchCG29xx: Opening file for PatchUpload %s ",vp_patchFileName));
        vg_gfl_PatchFile = MC_GFL_FOPEN(vp_patchFileName,"r");
        memset(vg_PatchBuffer,0,(K_CGPS_PATCH_SEGMENT_LENGTH+1));
        gn_Patch_Status = 2;
    }

    // patch file already opened do patch uploading
    if(gn_Patch_Status == 2)
    {
        // read the maximum number of patch message from the patch file
        vl_ReadCount=MC_GFL_FREAD(&vg_PatchBuffer,1,Max_Num_Patch_Mess,vg_gfl_PatchFile);

        while( vg_PatchBuffer[vl_ReadCount-1] != 0xA )
        {
            vl_ReadCount--;
        }

        vg_PatchBuffer[vl_ReadCount] = 0x00;
        Max_Num_Patch_Mess           = vl_ReadCount;

        // write the patch buffer read from the file to PE
        vl_BytesWritten = GN_GPS_Write_GNB_Ctrl(Max_Num_Patch_Mess,(CH *)vg_PatchBuffer);
        MC_CGPS_TRACE(("CGPS2_11UploadMEFSPatchCG29xx: Max_Num_Patch_Mess = %d ",  Max_Num_Patch_Mess));

        gn_Cur_Mess[0] += vl_BytesWritten;
        MC_GFL_FSEEK(vg_gfl_PatchFile,gn_Cur_Mess[0],GFL_SEEK_SET);
        MC_CGPS_TRACE(("CGPS2_11UploadMEFSPatchCG29xx: Bytes Written: %d Overall : %d",vl_BytesWritten,gn_Cur_Mess[0]));

        memset(vg_PatchBuffer,0,(K_CGPS_PATCH_SEGMENT_LENGTH+1));

        // check for the patch file upload complete
        if( vl_PatchLength == gn_Cur_Mess[0] )
        {
            gn_Patch_Status = 7;
            vg_FsPatchUploadCount++;
            gn_Cur_Mess[0] = 0;
            MC_CGPS_TRACE(("CGPS2_11UploadMEFSPatchCG29xx: Patch Download Complete. gn_ROM_version=%d, gn_Patch_Status=%d", gn_ROM_version, gn_Patch_Status));
        }
    }

    //  patch file close if file upload complete
    if( gn_Patch_Status == 7 )
    {
        MC_GFL_FCLOSE(vg_gfl_PatchFile);
        MC_CGPS_TRACE(("CGPS2_11UploadMEFSPatchCG29xx: Closing file for PatchUpload %s ",  vp_patchFileName));
    }
}

#undef __CGPS2PATCH_C__
