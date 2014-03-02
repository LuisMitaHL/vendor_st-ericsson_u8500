/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 *
 * Description: none.
 *
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _mdrc5b_util_h_
#define _mdrc5b_util_h_

#include "mdrc5b.h"


// functions
void mdrc5b_read (MDRC5B_LOCAL_STRUCT_T *HeapPtr);
void mdrc5b_write(MDRC5B_LOCAL_STRUCT_T *HeapPtr);
void mdrc5b_move (MDRC5B_LOCAL_STRUCT_T *HeapPtr);

#endif /* _mdrc5b_util_h_ */
