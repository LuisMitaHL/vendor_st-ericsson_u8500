/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "ite_init.h"
#include <cm/inc/cm.h>

#include <los/api/los_api.h>

#include <stdlib.h>
#include <stdio.h>
#include "ite_main.h"
#include "cli.h"

/*--------------------------------------------------------------------------*
 * Defines                                                                  *
 *--------------------------------------------------------------------------*/
/* Component Manager needs (to find elf files) */
/* Set defines according to our needs of SVA only */
/* (VIDEO_FW_PATHNAME and VIDEO_FW_FILENAME have been set by makefile) */
/* (IMAGING_FW_PATHNAME and IMAGING_FW_FILENAME have been set by makefile) */
#define AUDIO_FW_PATHNAME ""
#define AUDIO_FW_FILENAME NULL
/* Constants for NMF env. */
/* MPC Data and Code Segment alignment constraints : at least 8*64bits = 0x40 bytes */
//#define SEG_ALIGNMENT     0x0000003F
#define SEG_ALIGNMENT     0x000003FF
#define MALLOC_ALIGNMENT  0x0000000F


/*--------------------------------------------------------------------------*
 * Prototypes for internal functions                                        *
 *--------------------------------------------------------------------------*/
void ITE_LogFunction( const char *ap_format ,
                             int         a_param1  ,
                             int         a_param2  ,
                             int         a_param3  ,
                             int         a_param4  ,
                             int         a_param5  ,
                             int         a_param6  );

void ITE_TraceWrite32( t_uint32 a_channel       ,
                              t_uint8             a_isTimestamped ,
                              t_uint32            a_value         );

/* ------------------------------------------------------------------------
   FUNCTION : VAL_RemovePageFromCache
   PURPOSE  : Function that remove some page from ARM cache
   ------------------------------------------------------------------------ */
/*#ifndef __NMF_THINK
 void VAL_RemovePageFromCache(t_8820nmf_idx_memory_page idx_page_to_remove)
{

   switch(idx_page_to_remove)
   {
      case IDX_SDRAM_SHARE_BANK1:

         SER_MMU_RemovePagesFromCache(
             (t_uint32) &Image$$ER_SHARE_AHB_MEM$$Base                 ,
             (t_uint32) &Image$$ER_SHARE_AHB_MEM$$ZI$$Length 
                        + (t_uint32) &Image$$ER_SHARE_AHB_MEM$$Length );
      break;

      case IDX_ESRAM_BANKS:
         SER_MMU_RemovePagesFromCache(
             (t_uint32) &Image$$ER_ESRAM_BANKS$$Base                ,
             (t_uint32) &Image$$ER_ESRAM_BANKS$$ZI$$Length 
                        + (t_uint32) &Image$$ER_ESRAM_BANKS$$Length );
      break;

      case IDX_SDRAM_SHARE_BANK2:
         SER_MMU_RemovePagesFromCache(
             (t_uint32) &Image$$ER_SHARE_AHB_MEM2$$Base                ,
             (t_uint32) &Image$$ER_SHARE_AHB_MEM2$$ZI$$Length
                        + (t_uint32) &Image$$ER_SHARE_AHB_MEM2$$Length );
      break;

      default:
      break;
   }
} 
#endif

*/



/* ------------------------------------------------------------------------
   FUNCTION : Val8820nmf_LogFunction
   PURPOSE  : Log function to be called by Component Manager to log its events
   ------------------------------------------------------------------------ */
void ITE_LogFunction(const char *ap_format ,
                     int         a_param1  ,
                     int         a_param2  ,
                     int         a_param3  ,
                     int         a_param4  ,
                     int         a_param5  ,
                     int         a_param6  )
{
   /* Redirect to printf() function to display messages 
      in debugger console window */
   LOS_Log(ap_format, a_param1, a_param2, a_param3, a_param4, a_param5, a_param6);
}



/* ------------------------------------------------------------------------
   FUNCTION : Val8820nmf_TraceWrite32
   PURPOSE  : Trace32 function to be called by Component Manager to trace
              its events
   ------------------------------------------------------------------------ */
   
void ITE_TraceWrite32(  t_uint32 a_channel       ,
                        t_uint8             a_isTimestamped ,
                        t_uint32            a_value         )
{
 UNUSED(a_channel);
 UNUSED(a_isTimestamped);
 UNUSED(a_value);
 
   /* NOT IMPLEMENTED YET */
}
