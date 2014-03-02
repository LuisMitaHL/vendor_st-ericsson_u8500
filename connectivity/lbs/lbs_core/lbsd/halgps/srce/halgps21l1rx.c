/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */ 

#define __HALGPS21L1RX_C__

/**
* \file halgps20freqaiding.c
* \date 17/07/2009
* \version 1.0
*
* <B>Compiler:</B> ARM ADS\n
*
* <B>Description:</B> This file contains functions that manage L1 status updates for frequency aiding.\n
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

#if defined( GPS_FREQ_AID_FTR ) && (__RTK_E__)

#include "halgpsgsa.h"
#include "halgpsgsainc.h"

#undef  MODULE_NUMBER
#define MODULE_NUMBER   MODULE_HALGPS

#undef  PROCESS_NUMBER
#define PROCESS_NUMBER  PROCESS_HALGPS

#undef  FILE_NUMBER
#define FILE_NUMBER     21


#undef PROCEDURE_NUMBER
#define PROCEDURE_NUMBER 0
/**************************************************************************************************************/
/* GSA_SetAfcUpdate : AFC algo to provide the reliability of the frequency error samples and frequency error. */
/**************************************************************************************************************/
void  GSA_SetAfcUpdate(
   uint8_t vl_Status,      /**<Status of AFC */
   int32_t FreqError_ppb  /**< Uncertainty of the known frequency [ppb] . Set the value to  0 if this value is not known.*/
)
{
    /* Update the previous value in database. */
    vg_HALGPS_AFCStatus.v_PrevFreqError_ppb = vg_HALGPS_AFCStatus.v_FreqError_ppb;

    vg_HALGPS_AFCStatus.v_Status            = vl_Status;

    /* Use absolute value of FreqError_ppb */ 
    vg_HALGPS_AFCStatus.v_FreqError_ppb = (uint32_t)sqrt(( pow( FreqError_ppb,2) + pow(HALGPS_GSA_BTS_FREQ_TOLERANCE, 2) + pow(HALGPS_GSA_DOPPLER_EFFECT, 2) ) /3);
}

#endif

#undef __HALGPS21L1RX_C__
