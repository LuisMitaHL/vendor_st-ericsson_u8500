/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef  _CIL_AMRWB_INFO_H_
#define  _CIL_AMRWB_INFO_H_
#include "audiolibs_types.h"

/* amr-wb dec */
typedef struct {
  t_uint16        bitrate;
} t_dec_amrwb_info;

/* amr-wb enc */
typedef struct {
  t_uint16        bitrate;
} t_enc_amrwb_info;

#endif   // _CIL_AMR_INFO_
