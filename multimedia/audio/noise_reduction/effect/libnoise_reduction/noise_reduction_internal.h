/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief
* \author
*/
/*****************************************************************************/
#ifndef INCLUSION_GUARD_NOISE_REDUCTION_INTERNAL_H
#define INCLUSION_GUARD_NOISE_REDUCTION_H

#include "speech_enhancement.h"

// 16-bit sample = 2 bytes per sample
#define SAMPLE_WIDTH 2

struct noise_reduction_handle_t {
    struct SE_TxComponent_t		*comp_p;
    SE_TxParameters_t   		params;
    SE_SampleRate_t     		sample_rate;
    SE_NumberOfChannels_t 		channels;
    boolean             		initialized;
    boolean             		pending_config;
    SE_NrParameters_t   		pending_nr_config;
    
};


#endif //INCLUSION_GUARD_NOISE_REDUCTION_INTERNAL_H
