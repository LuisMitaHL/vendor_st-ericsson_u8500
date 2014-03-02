/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/

#ifndef _VC1D_HT_CONFORMANCE_H_
#define _VC1D_HT_CONFORMANCE_H_

/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"


/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/




/**
 * Description:
 * A structure holding the limits for a given profile and level.
 */

typedef struct
{
    t_uint32         MBs;               /** Maximum macroblocks per second */
    t_uint32         MBf;               /** Maximum macroblocks per frame */
    t_uint32         r_max;             /** Maximum peak transmission rate in kpbs */
    t_uint32         b_max;             /** Maximum buffer size in multiples of 16kbits */
    t_vc1_mv_range   motion_vector_range;   /** Motion vector range allowed */
}   ts_level_limit, *tps_level_limit;

/*------------------------------------------------------------------------
 * Exported functions                                                            
 *----------------------------------------------------------------------*/

t_vc1_level find_level_from_image_size(t_vc1_profile profile, t_uint16 width,t_uint16 height);

#endif /* _VC1D_HT_CONFORMANCE_H_ */
