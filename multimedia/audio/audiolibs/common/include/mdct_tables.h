/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _mdct_tables_h_ 
#define _mdct_tables_h_ 

#include "audiolibs_common.h"  

 /* mdct tables exclusion */ 
//#define DO_NOT_HAVE_MDCT_TABS_16
//#define DO_NOT_HAVE_MDCT_TABS_32
//#define DO_NOT_HAVE_MDCT_TABS_64
//#define DO_NOT_HAVE_MDCT_TABS_128
//#define DO_NOT_HAVE_MDCT_TABS_256     /* tables requided by aac codecs */
//#define DO_NOT_HAVE_MDCT_TABS_512
//#define DO_NOT_HAVE_MDCT_TABS_1024
//#define DO_NOT_HAVE_MDCT_TABS_2048    /* tables requided by aac codecs */
#define DO_NOT_HAVE_MDCT_TABS_4096 /* not compatible with define FFT_512_PT_SUPPORT (fft.h) */


#define Idx_mdct_tables  4 


/* definition of tables  */


extern Float const MDCT_MEM AudioTables_cos_sinTab_4096[];
extern Float const MDCT_MEM AudioTables_cos_sinTab_2048[];
extern Float const MDCT_MEM AudioTables_cos_sinTab_1024[];
extern Float const MDCT_MEM AudioTables_cos_sinTab_512[];
extern Float const MDCT_MEM AudioTables_cos_sinTab_256[];
extern Float const MDCT_MEM AudioTables_cos_sinTab_128[];
extern Float const MDCT_MEM AudioTables_cos_sinTab_64[];
extern Float const MDCT_MEM AudioTables_cos_sinTab_32[];
extern Float const MDCT_MEM AudioTables_cos_sinTab_16[];




 /* definition of tables pointers */ 
#ifndef MDCT_WAITMEM_TABLES
extern Float MDCT_MEM * audiotables_mdct_init(Float MDCT_MEM * base);
extern Float const MDCT_MEM * const MDCT_MEM AudioTables_mdct_cos_sin[9] ;
#else /* MDCT_WAITMEM_TABLES */
extern Float YMEM * audiotables_mdct_init(Float YMEM  * base);
extern Float YMEM * YMEM AudioTables_mdct_cos_sin[9];
#endif /* MDCT_WAITMEM_TABLES */

Float MDCT_MEM ** fn_AudioTables_mdct_cos_sin(void);

#endif /* Do not edit below this line */
