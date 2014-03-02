#ifndef _vector_gc_h_
#define _vector_gc_h_

#include "audiolibs_common.h"
#include "defines.h"

#ifdef SSR
extern void vector_mpy_copy_gc(Float *p,Float *q,int *gc_win,int len,int incr);
extern void vector_mac_gc(Float *p,Float *q,int *gc_win,int len,int incr);
extern void vector_mac_gc_copy(Float *res,Float *p,Float *q,int *gc_win,int len,int incr);
#endif /* SSR */

#endif /* Do not edit below this line */
