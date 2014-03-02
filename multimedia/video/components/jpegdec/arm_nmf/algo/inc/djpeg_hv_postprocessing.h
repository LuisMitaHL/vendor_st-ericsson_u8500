/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file    djpeg_hv_postprocessing.h
 * \brief   Post-processing functions declaration
 * \author  ST-Ericsson
 *  This file declares the functions that implement the conversion to
 *  YUV 4:2:0 of the decoded image.
 */
/*****************************************************************************/

#ifndef _DJPEG_HV_POSTPROCESSING_H_
#define _DJPEG_HV_POSTPROCESSING_H_
/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "djpeg_hamac.h"

  #ifdef __NMF
#include <t1xhv_vdc_jpeg.idt>
#include <jpegdec.idt>
#else
#include "djpeg_interface.h"
#include "djpeg_global.h"
#endif



/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/
/** \brief struct information used by the post processing functions */



 /*------------------------------------------------------------------------
 * Global variables
 *----------------------------------------------------------------------*/
/****************************************************************************/
/**
 * \brief 	create a dense char array of 32 bytes from a mmdsp char array
 * \author 	Aroua Ben Daryoug
 * \param 	char_array_in : mmdsp char array ie 1 char on 2 bytes
 * \param 	array_in_size : size of the input array
 * \param 	compact_char_array_out compacted char array ie 4 chars on every ui32
 * \note        This function assume little endianess.
 * 
 **/
/****************************************************************************/
/* #pragma inline */
/* static void compact_mmdsp_char(t_uint8 char_array_in[], t_uint16 array_in_size,t_uint16 compact_char_array_out[]) */
/* { */
/*     t_uint8 *p_char; */
/*     t_uint16 *p_compact_char; */
/*     t_uint16 cnt; */
    
/*     p_char = char_array_in; */
/*     p_compact_char = compact_char_array_out; */
/* #pragma loop maxitercount(16) */
/*     for (cnt = 0;cnt<array_in_size/2;cnt++) */
/*     { */
/* //      *p_compact_char = (t_uint16)(*p_char)<< 8 |(t_uint16)(*(p_char+1)) ; */
/*         *p_compact_char = wmerge((*p_char),(*(p_char+1))); */
/* /\*         printf("char1=/n%x/n",*p_char);  *\/ */
/* /\*         printf("char2=/n%x/n",*(p_char+1));  *\/ */
/* /\*         printf("compressed char=/n%x/n", *p_compact_char);  *\/ */
/*         p_compact_char ++; */
/*         p_char +=2; */
/*     } */
/* }/\* end of compact_mmdsp_char *\/ */

 
/*------------------------------------------------------------------------
 * Functions
 *----------------------------------------------------------------------*/



void jpegdec_pp_init(tps_hamac_info p_hamac_info,          
             tps_post_process_info p_pp_info);


#endif /* _DJPEG_HV_POSTPROCESSING_H_ */


