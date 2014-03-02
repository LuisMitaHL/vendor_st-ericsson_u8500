/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _audiowindows_h_
#define _audiowindows_h_

#include "audiolibs_common.h"

#ifdef __flexcc2__
#define EXTERN __EXTERN
#else
#define EXTERN
#endif

/* Tables required by 2048/128 imdct overlap-add, only half of the
 *  window is saved (symetry)
 */

#ifdef ARM
extern int const WINDOW_MEM AudioTables_dol_long_arm[1024];
extern int const WINDOW_MEM AudioTables_dol_short_arm[128];
extern int const WINDOW_MEM AudioTables_sin_long_arm[1024];
extern int const WINDOW_MEM AudioTables_sin_short_arm[128];


int WINDOW_MEM *fn_AudioTables_sin_long_arm(void);
int WINDOW_MEM *fn_AudioTables_sin_short_arm(void);
int WINDOW_MEM *fn_AudioTables_dol_long_arm(void);
int WINDOW_MEM *fn_AudioTables_dol_short_arm(void);

#endif /* ARM */

extern Float const WINDOW_MEM AudioTables_dol_long[1024];
extern Float const WINDOW_MEM AudioTables_dol_short[128];
extern Float const WINDOW_MEM AudioTables_sin_long[1024];
extern Float const WINDOW_MEM AudioTables_sin_short[128];

/* Tables required by 256/32 imdct overlap-add, only half of the
 *  window is saved (symetry). AAC SSR profile
 */
extern Float const WINDOW_MEM AudioTables_dol_long_ssr[256];
extern Float const WINDOW_MEM AudioTables_dol_short_ssr[32];
extern Float const WINDOW_MEM AudioTables_sin_long_ssr[256];
extern Float const WINDOW_MEM AudioTables_sin_short_ssr[32];

Float WINDOW_MEM *fn_AudioTables_sin_long(void);
Float WINDOW_MEM *fn_AudioTables_sin_short(void);
Float WINDOW_MEM *fn_AudioTables_dol_long(void);
Float WINDOW_MEM *fn_AudioTables_dol_short(void);

Float WINDOW_MEM *fn_AudioTables_sin_long_ssr(void);
Float WINDOW_MEM *fn_AudioTables_sin_short_ssr(void);
Float WINDOW_MEM *fn_AudioTables_dol_long_ssr(void);
Float WINDOW_MEM *fn_AudioTables_dol_short_ssr(void);
#endif /* Do not edit below this line */
