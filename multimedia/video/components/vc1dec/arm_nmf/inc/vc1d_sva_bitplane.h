/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_SVA_BITPLANE_H_
#define _VC1D_SVA_BITPLANE_H_

/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_target.h"
#include "vc1d_common.h"
#include "vc1d_co_bitstream.h"
#include "vc1d_max.h" /* for MAX_MBF */
/*------------------------------------------------------------------------
 * Defines                                                            
 *----------------------------------------------------------------------*/


/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/
/**
 * Description:
 * Bitplane coding mode.
 */

typedef enum
{
    BITPLANE_CODING_MODE_NORM2 = 0,    /** Normal-2 bitplane coding */
    BITPLANE_CODING_MODE_NORM6 =1 ,        /** Normal-6 bitplane coding */
    BITPLANE_CODING_MODE_ROWSKIP =2,      /** Rowskip bitplane coding */
    BITPLANE_CODING_MODE_COLSKIP =3 ,      /** Colskip bitplane coding */
    BITPLANE_CODING_MODE_DIFF2 =4,        /** Diff-2 bitplane coding */ 
    BITPLANE_CODING_MODE_DIFF6 =5,        /** Diff-6 bitplane coding */
    BITPLANE_CODING_MODE_RAW =6           /** Raw (No) bitplane coding */
} t_vc1_bitplane_coding_mode;

/**
 * Description:
 * Structure to contain bitplane coding information.
 */
typedef struct
{
    t_uint16  position;       /** Index of the next bit in the bitplane */
    t_bool    is_set[MAX_MBF];     /**  bitplane data table. NULL indicates RAW mode */
    t_bool    is_raw_mode;        /** True if raw mode is used in this bitplane */
} ts_bitplane, *tps_bitplane;


/*------------------------------------------------------------------------
 * Functions (exported)                                                            
 *----------------------------------------------------------------------*/
void btpl_read_bitplane( tps_bitstream_buffer p_bitstream,
                                      t_uint16 mb_width,
                                      t_uint16 mb_height,
                                      tps_bitplane p_bitplane);

/****************************************************************************/
/**
 * \brief 	read a biplane value from a given bitplane
 * \author 	jean-marc volle
 * \param 	p_bitplane: bitplane to read from
 * \return 	 bitplane value
 * \note        each access to the bitplane increase the bitplane position index 
 * 
 *
 **/
/****************************************************************************/

INLINE static t_bool btpl_get_bitplane_value(tps_bitplane p_bitplane)
{
    t_bool val;

    /* If the bitplane is a raw one, the value should be read in the bitstream */
    ASSERT(!p_bitplane->is_raw_mode);
    ASSERT(p_bitplane->position < MAX_MBF);

    val = p_bitplane->is_set[p_bitplane->position];

    p_bitplane->position++;

    return (val);

}


#endif /* _VC1D_SVA_BITPLANE_H_ */


