/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
  * \file 	djpeg_hv_memory.h
 * \brief   hamac video external memory maping function
 * \author  ST-Ericsson
 */
/*****************************************************************************/


/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "djpeg_common.h"

/*------------------------------------------------------------------------
 * Types							       
 *----------------------------------------------------------------------*/
/** \brief transfert increment values   */
typedef enum
{
    INC_FOUR_BYTES = 0,                         /**< \brief increment of 4 bytes     */
    INC_EIGHT_BYTES,                             /**< \brief increment of 8 bytes     */
    INC_THIRTY_TWO_BYTES,                       /**< \brief increment of 32 bytes   */
    INC_SIXTY_FOUR_BYTES,                       /**< \brief increment of 64 bytes   */
    INC_ONE_HUNDRED_TWENTY_EIGHT_BYTES          /**<  \brief increment of 128 bytes */


} t_transfert_inc;

/*------------------------------------------------------------------------
 * Functions							       
 *----------------------------------------------------------------------*/


void jpegdec_mem_external_write( t_uint32 p_buffer[], t_ulong_value external_add, t_uint32 transfert_byte_size, t_transfert_inc trf_inc);
void jpegdec_mem_external_read( t_uint32 p_buffer[], t_ulong_value external_add, t_uint32 transfert_byte_size, t_transfert_inc trf_inc);
