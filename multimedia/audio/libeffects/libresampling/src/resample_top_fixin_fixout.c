/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   resample_top_fixin_fixout.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "resample_local.h"
AUDIO_API_EXPORT int
resample_x_process_fixin_fixout(ResampleContext *ctx,
                                int             *in,
                                int             stride,
                                int             *nbSamples, // either no of samples or no of msec
                                int             *output,
                                int             *nbOut,     // either no of samples or no of msec
                                int             *flush)
{
    if(ctx->processing == resample_x_process_fixin_fixout)
    {
        if(resample_choose_processing_routine(ctx) == 1)
        {
            return 1;
        }
    }
    
    return (*ctx->processing) (ctx,
                               in,
                               stride,
                               nbSamples,
                               output,
                               nbOut,
                               flush);
}

