/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _riff_h_
#define _riff_h_

extern int
riff_match_32_bits(int hi, int lo, int himask, int lomask);

extern int
riff_sampling_frequency(int f);

extern int
riff_parse_header(struct AudioInfo * info);

extern int
riff_syncword(int hi, int lo, int *length);

#endif /* Do not edit below this line */
