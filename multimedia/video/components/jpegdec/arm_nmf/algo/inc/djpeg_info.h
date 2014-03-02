/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_info.h
 * \brief 	decoder enum
 * \author  ST-Ericsson
 *  
 */
/*****************************************************************************/

#ifndef _DJPEG_INFO_H_
#define _DJPEG_INFO_H_

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/
typedef enum
{
    DECODE_OK=0, /**< \brief the decoding was performed well   */
    DECODE_SUSPENDED, /**< \brief the decoding was suspended because of bitstream data shortage   */
    INVALID_BITSTREAM /**< \brief the bitstream is corrupted   */

} t_decoder_state;

#endif

