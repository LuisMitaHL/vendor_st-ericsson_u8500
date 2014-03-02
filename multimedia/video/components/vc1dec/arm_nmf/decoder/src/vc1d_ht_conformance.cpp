/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#ifdef NMF_BUILD
#include <vc1dec/arm_nmf/decoder.nmf>
#else
#include <stdio.h>  /* printf */
#include <stdlib.h> /* exit  */
#endif
#include "vc1d_common.h"
#include "vc1d_ht_conformance.h"

/*------------------------------------------------------------------------
 * Defines                                                            
 *----------------------------------------------------------------------*/

#if defined(PRINT_CONFORMANCE) || defined(PRINT_ALL)
#define PRINTF(value) if (Dbg) printf value /**< \brief debug printf function: use double (( when calling   */
#else
#define PRINTF(value)
#endif
/*------------------------------------------------------------------------
 * Constants                                                            
 *----------------------------------------------------------------------*/

const ts_level_limit c_level_limits[2][4] = 
{
    {   /* Simple Profile */
        {   1485,   99,    96,   20, MVRANGE_64_32    }, /* Low    Level */
        {   5940,  396,   384,   77, MVRANGE_64_32    }, /* Medium Level */
        {      0,    0,     0,    0, MVRANGE_64_32    }, /* used to find end of table */
    },
    {   /* Main Profile */
        {   7200,  396,  2000,  306, MVRANGE_128_64   }, /* Low    level */
        {  40500L, 1620, 10000,  611, MVRANGE_512_128  }, /* Medium Level */
        { 245760L, 8192, 20000, 2442, MVRANGE_1024_256 }, /* High   Level */
        {      0,    0,     0,    0, MVRANGE_64_32    },/* used to find end of table */
    }
};

/*------------------------------------------------------------------------
 * Functions (Exported)                                                           
 *----------------------------------------------------------------------*/

t_vc1_level find_level_from_image_size(t_vc1_profile profile, t_uint16 width,t_uint16 height)
{
    t_uint16 width_in_MBs,height_in_MBs;
    t_uint16 mb_in_frame;
    t_sint16 level;
    ts_level_limit const *p_level_limit;

    ASSERT(profile == 0 || profile == 1);

    p_level_limit = &c_level_limits[profile][0];

    width_in_MBs  = (width  + 15) >> 4;
    height_in_MBs = (height + 15) >> 4;
    mb_in_frame = width_in_MBs * height_in_MBs;

    PRINTF(("\tMBf = %u\n",mb_in_frame));

    for (level = 0;level<4;level++)
    {
        if (mb_in_frame <= p_level_limit->MBf) break;
        if (p_level_limit->MBf == 0)
        {
            printf("ERROR: Image too big for the levels of the profile\n");
            return LEVEL_UNKNOWN;
        }
        p_level_limit++;
    }
    
#if defined (PRINT_CONFORMANCE) || defined(PRINT_ALL)
    if (Dbg)
    {
        printf("\tPROFILE/LEVEL ");

        switch (level)
        {
        case 0:
            if (profile == PROFILE_SIMPLE) printf("SP@LL\n");
            else printf("MP@LL\n");
            break;
        
        case 1:
            if (profile == PROFILE_SIMPLE) printf("SP@ML\n");
            else printf("MP@ML\n");
            break;
        case 2:
            if (profile == PROFILE_MAIN) printf("MP@HL\n");
            else ASSERT(FALSE);
            break;
        default: ASSERT(FALSE);
        
        }
    }
#endif

    
    return ((t_vc1_level) level);
}
