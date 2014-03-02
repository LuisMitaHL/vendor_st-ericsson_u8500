/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AFM_Utils.cpp
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include "AFM_Utils.h"
#include "ENS_DBC.h"

AFM_API_EXPORT OMX_U32 AFM::OMX_SamplingRate(t_sample_freq eFreq) {
    switch (eFreq) {
        case FREQ_192KHZ:
            return 192000;
        case FREQ_176_4KHZ:
            return 176400;
        case FREQ_128KHZ:
            return 128000;
        case FREQ_96KHZ:
            return 96000;
        case FREQ_88_2KHZ:
            return 88200;
        case FREQ_64KHZ:
            return 64000;
        case FREQ_48KHZ:
            return 48000;
        case FREQ_44_1KHZ:
            return 44100;
        case FREQ_32KHZ:
            return 32000;
        case FREQ_24KHZ:
            return 24000;
        case FREQ_22_05KHZ:
            return 22050;
        case FREQ_16KHZ:
            return 16000;
        case FREQ_12KHZ:
            return 12000;
        case FREQ_11_025KHZ:
            return 11025;
        case FREQ_8KHZ:
            return 8000;
        case FREQ_7_2KHZ:
            return 7200;
        case FREQ_UNKNOWN:
            return 0;

        default:
            DBC_ASSERT(0);
            return 0;
    }
}

AFM_API_EXPORT t_sample_freq 
AFM::sampleFreq(OMX_U32 nSamplingRate) {
    switch (nSamplingRate) {
        case 192000:
            return FREQ_192KHZ;
        case 176400:
            return  FREQ_176_4KHZ;
        case 128000:
            return  FREQ_128KHZ;
        case 96000:
            return  FREQ_96KHZ;
        case 88200:
            return  FREQ_88_2KHZ;
        case 64000:
            return  FREQ_64KHZ;
        case 48000:
            return  FREQ_48KHZ;
        case 44100:
            return  FREQ_44_1KHZ;
        case 32000:
            return  FREQ_32KHZ;
        case 24000:
            return  FREQ_24KHZ;
        case 22050:
            return  FREQ_22_05KHZ;
        case 16000:
            return  FREQ_16KHZ;
        case 12000:
            return  FREQ_12KHZ;
        case 11025:
            return  FREQ_11_025KHZ;
        case 8000:
            return  FREQ_8KHZ;
        case 7200:
            return  FREQ_7_2KHZ;
        case 0:
            return FREQ_UNKNOWN;

        default:
            DBC_ASSERT(0);
            return FREQ_UNKNOWN;
    }
}
