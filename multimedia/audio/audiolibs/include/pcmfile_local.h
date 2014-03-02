/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef _pcmfile_local_h_
#define _pcmfile_local_h_

#include "pcmfile.h"
#include "riff_decoder.h"

#ifndef USE_AUDIO_CLASS

#define MAX_BLOCK_SIZE   256
#define MAX_CHANNELS     2

#define WAVE_FORMAT_PCM         0x0001
#define WAVE_FORMAT_MPEG_LAYER3 0x0055
#define WAVE_FORMAT_MPEG_AAC    0x0180

enum {
    ID_RIFF,
    ID_FMT,
    ID_FACT,
    ID_DATA
};

enum {
    PCMFILE_OK = 0,
    PCMFILE_FATAL_ERROR
};


#include "pcm.h"
#include "pcm_local.h"
#include "riff.h"
#include "pcm_parser.h"

extern struct AudioInfo *pInfo;

#endif /* USE_AUDIO_CLASS */

#endif /* Do not edit below this line */





