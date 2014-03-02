/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#if 0
static void check_mem(void)
{
	 LOS_Free(LOS_Alloc(1,0,LOS_MAPPING_DEFAULT));

}
#define		PRE_CONDITION(a)	check_mem();NMF_LOG(">> %s %d\n",__FILE__,__LINE__);if ((a)==0) NMF_PANIC("Error on expected component state %s %d\n",__FILE__,__LINE__) 
#else
#define		PRE_CONDITION(a)	if ((a)==0) NMF_PANIC("Error on expected component state %s %d\n",__FILE__,__LINE__) 
#endif


#define 	NONE		1
#define 	CONSTRUCT	2
#define 	RUNNING		3
#define 	STOPPED		4



