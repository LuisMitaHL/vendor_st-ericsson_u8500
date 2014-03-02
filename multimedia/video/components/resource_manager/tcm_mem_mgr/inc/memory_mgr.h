/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __MEMORY_MGR_H__
#define __MEMORY_MGR_H__

/* uncomment this define if you want to activate internal checks in memory mgr */
//#define _DEBUG_MEMMGR

/* this is the internal linked lists maximum size, it limits the max number of 
   outstanding mallocs that can be done */ 
#define LST_MAX_ELM_NBR 64            

/* defines the alignment (expressed in nbr of 16-bits words) of the allocated addresses */
#define MM_ALGN         2

/* defines the heap size, expressed in amount of 16-bits words */
#define TCM_HEAP_SIZE       2

#endif /* __MEMORY_MGR_H__ */
