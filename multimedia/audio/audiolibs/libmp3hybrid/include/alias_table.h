/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _alias_table_h_
#define _alias_table_h_

#ifdef ARM
extern int const YMEM mp3_hybrid_cs_arm[MP3_MAX_ALIAS+1]; 
extern int const YMEM mp3_hybrid_ca_arm[MP3_MAX_ALIAS];
#endif

extern Float const YMEM mp3_hybrid_cs[MP3_MAX_ALIAS+1]; 
extern Float const YMEM mp3_hybrid_ca[MP3_MAX_ALIAS];


#endif /* Do not edit below this line */
