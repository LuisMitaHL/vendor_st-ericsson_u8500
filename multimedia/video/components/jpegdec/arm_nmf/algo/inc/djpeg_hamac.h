/****************************************************************************
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_hamac.h
 * \brief 	jpeg decoder hamac header file
 * \author  ST-Ericsson
 *  
 *  
 *  This file declares types for hamac jpeg decoder
 * 
 */
/*****************************************************************************/


#ifndef _DJPEG_HAMAC_H_
#define _DJPEG_HAMAC_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "djpeg_common.h"
#include <setjmp.h>
/*------------------------------------------------------------------------
 * Global Variables							       
 *----------------------------------------------------------------------*/
//extern jmp_buf error_jmp;
/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/


/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/



typedef struct huff_elt
{
    struct huff_elt *next ;
    t_uint8 val;
    t_uint8 size;
    t_uint16 code;
} ts_huff_elt, *tps_huff_elt; 



#define HUFF_LOOKAHEAD	8	/* # of bits of lookahead */




#endif /* _DJPEG_HAMAC_H_ */
