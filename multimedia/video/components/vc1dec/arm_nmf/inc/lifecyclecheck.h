/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



extern	t_uint16	decoder_state;
#define		PRE_CONDITION(a)	if ((a)==0) NMF_PANIC("Error on expected component state %s %d\n",__FILE__,__LINE__)
#define 	NONE		1
#define 	CONSTRUCT	2
#define 	RUNNING		3
#define 	STOPPED		4

