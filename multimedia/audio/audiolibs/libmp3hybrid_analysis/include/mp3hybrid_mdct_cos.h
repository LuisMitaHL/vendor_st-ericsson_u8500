/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _mp3hybrid_mdct_cos_h
#define _mp3hybrid_mdct_cos_h

extern Float const MP3HYBRID_MEM mp3hybrid_analysis_ca[MP3_MAX_ALIAS];
extern Float const MP3HYBRID_MEM mp3hybrid_coss[6*6];
extern Float const MP3HYBRID_MEM mp3hybrid_cosl[18*18];

extern Float const MP3HYBRID_MEM * AUDIOLIBS_UPLOAD_MEM mp3hybrid_analysis_p_cosl;
extern Float const MP3HYBRID_MEM * AUDIOLIBS_UPLOAD_MEM mp3hybrid_analysis_p_coss;
extern Float const MP3HYBRID_MEM * AUDIOLIBS_UPLOAD_MEM mp3hybrid_analysis_p_ca;  
extern unsigned int AUDIOLIBS_UPLOAD_MEM mp3hybrid_analysis_ref_count;

#endif /* Do not edit below this line */
