/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Description: none.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _mdrc5b_limiter_h_
#define _mdrc5b_limiter_h_

#include "mdrc5b.h"


// functions
extern void mdrc5b_reset_limiter        (MDRC5B_LOCAL_STRUCT_T *Heap);
extern void mdrc5b_apply_limiter        (MDRC5B_LOCAL_STRUCT_T *Heap);
extern void mdrc5b_derive_limiter_params(MDRC5B_LOCAL_STRUCT_T *Heap);

#endif /* _mdrc5b_limiter_h_ */
