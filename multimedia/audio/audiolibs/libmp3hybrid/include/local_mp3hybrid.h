/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _local_hybrid_h_
#define _local_hybrid_h_

#ifdef _NMF_MPC_
#include "audiolibs/libmp3hybrid.nmf"
#endif

#include "audiolibs_common.h"
#include "mp3hybrid.h"
#include "btrue.h"
#include "mp3hybrid_common.h"
#include "mp3_hybrid_tables.h"
#include "alias_table.h"
#include "inv_imdct.h"
#include "polysyn.h"
#include "vector.h"

#ifndef SSLIMIT
#define SSLIMIT 18
#endif
#ifndef SBLIMIT
#define SBLIMIT 32
#endif

#ifndef PI
#define         PI                      3.14159265358979
#endif /* PI */

#ifdef ARM
extern int const *const mp3d_hybrid_win_arm[4]; 
extern const int merged_window_arm[10] ;
extern const int Granule_twiddles_short_arm[4];
extern const int scale_tab3_arm[10];
extern const int _9PointIIIDCT_tab_arm[8];
#endif  /* ARM */

#endif /* Do not edit below this line */
