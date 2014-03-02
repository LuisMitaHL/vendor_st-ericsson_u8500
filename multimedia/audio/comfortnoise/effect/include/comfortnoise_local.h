/*****************************************************************************/
/**
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 *
 * \file   comfortnoise_local.h
 * \brief  Interface for Comfort Noise Generation algoirthm.
 * \author ST-Ericsson
 * \version v0.0.1
 */
/*****************************************************************************/
#ifndef _comfortnoise_local_h
#define _comfortnoise_local_h

#ifndef CNG_MMDSP
  #include <math.h>
  #include <string.h>
  #include <stdlib.h>
#endif

#include "comfortnoise.h"

#ifdef CNG_DEBUG
  #include <stdio.h>
  #define PRINT  printf("[CNG] "); printf
#else
#ifdef CNG_MMDSP
  #define PRINT(x)
  #define NULL  0
#else
  #define PRINT(x,...)
#endif
#endif

#ifndef TRUE
#define FALSE  0
#define TRUE  (!FALSE)
#endif


/** Maximum noise generation RMS level calculated in dBovl */
#define CNG_NOISE_CALCULATED_LEVEL_MAX     -70

/** Maximum block size used in the effect (optimization: 10ms @ 48kHz) */
#define CNG_MAXIMUM_BLOCK_SIZE              480

/** Error handling macro */
#define COMFORT_NOISE_CHECK_ALLOC_SUCCESS(a)           if(NULL == a) { goto COMFORT_NOISE_ALLOCATE_ERROR_HANDLING; }


/**
 * Comfort Noise Generation variables
 */
typedef struct
{
	unsigned short seed;                                               /**< noise generation seed */
	unsigned short prev;                                               /**< noise generation previous value */
} t_vars_comfort_noise_generation;


/**
 * Comfort Noise Generation instance struct
 */
typedef struct
{
    bool_t                            enable;                           /**< Enable/disable the algorithm */
    signed short                      level;                            /**< Noise generation level */
    signed short                      dBRMS_level;                      /**< RMS noise generation level */
    unsigned short                    volume;                           /**< Amplification if level is not in [-90, -70] dBovl*/
    unsigned short                    nof_channels;                     /**< Mono or stereo. Set up both input and output to the same number of channels. */
    unsigned int                      frame_size;                       /**< Number of samples per channel. */
    unsigned int                      frame_offset;                     /**< Number of samples to offset the input buffer when handling 
                                                                             interleaved and non-interleaved audio signals. */
    unsigned short                    sample_offset;                    /**< Number of samples processing output/input frames. It depends if
                                                                             the sample pattern is interleaved or non-interleaved. */
    t_vars_comfort_noise_generation   comfort_noise_variables_left;     /**< Comfort noise generation variables struct for left channel */
    t_vars_comfort_noise_generation   comfort_noise_variables_right;    /**< Comfort noise generation variables struct for right channel */
} t_comfort_noise_state;


#ifdef CNG_MMDSP
/**
 * Comfort Noise Generation MMDSP instance
 */
extern t_comfort_noise_state   state;
#endif

#endif /* comfortnoise_local.h */
