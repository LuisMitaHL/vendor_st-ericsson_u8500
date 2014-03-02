/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   bindings_optim.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _bindings_optim_h_
#define _bindings_optim_h_

#include "AFM_macros.h"


#define SHMPCM_ROUNDING         // use rounding for shmpcm     routines (for downmixing and/or 32->16 bits samples conversion)
#define PCMADAPTER_ROUNDING     // use rounding for pcmadapter routines (for downmixing and/or 32->16 bits samples conversion)


// Thresholds above which CA9 optimization will be applied
// (below these values, NEON instrinsics won't be used, so NEON won't be awakened)

// for pcmadapter
#define STEREO2MONO_16_NEON_SIZE_THRESHOLD                   16
#define STEREO2MONO_32_NEON_SIZE_THRESHOLD                   8
#define MONO2STEREO_16_NEON_SIZE_THRESHOLD                   16
#define MONO2STEREO_32_NEON_SIZE_THRESHOLD                   8
#define C16TO32_NEON_SIZE_THRESHOLD                          8
#define C32TO16_NEON_SIZE_THRESHOLD                          8
#define MOVE_CHUNK_NEON_SIZE_THRESHOLD                       32

// for shmin & shmout
#define SWAP_BYTES_NEON_SIZE_THRESHOLD                       32

// for shmpcmin & shmpcmout
#define COPY_DOWNMIX_IN16B_OUT16B_SWAP_NEON_SIZE_THRESHOLD   16
#define COPY_DOWNMIX_IN16B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD 16
#define COPY_DOWNMIX_IN16B_OUT32B_SWAPIN_NEON_SIZE_THRESHOLD 8
#define COPY_DOWNMIX_IN16B_OUT32B_NOSWAP_NEON_SIZE_THRESHOLD 8
#define COPY_DOWNMIX_IN32B_OUT16B_SWAPIN_NEON_SIZE_THRESHOLD 8
#define COPY_DOWNMIX_IN32B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD 8
#define COPY_DOWNMIX_IN32B_OUT32B_SWAP_NEON_SIZE_THRESHOLD   8
#define COPY_DOWNMIX_IN32B_OUT32B_NOSWAP_NEON_SIZE_THRESHOLD 8
#define COPY_UPMIX_IN16B_OUT16B_SWAP_NEON_SIZE_THRESHOLD     16
#define COPY_UPMIX_IN16B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD   16
#define COPY_UPMIX_IN16B_OUT32B_SWAPIN_NEON_SIZE_THRESHOLD   8
#define COPY_UPMIX_IN16B_OUT32B_NOSWAP_NEON_SIZE_THRESHOLD   8
#define COPY_UPMIX_IN32B_OUT16B_SWAPIN_NEON_SIZE_THRESHOLD   8
#define COPY_UPMIX_IN32B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD   8
#define COPY_UPMIX_IN32B_OUT32B_SWAP_NEON_SIZE_THRESHOLD     8
#define COPY_UPMIX_IN32B_OUT32B_NOSWAP_NEON_SIZE_THRESHOLD   8
#define COPY_IN16B_OUT16B_SWAP_NEON_SIZE_THRESHOLD           16
#define COPY_IN16B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD         16
#define COPY_IN16B_OUT32B_SWAPIN_NEON_SIZE_THRESHOLD         8
#define COPY_IN16B_OUT32B_SWAPOUT_NEON_SIZE_THRESHOLD        8
#define COPY_IN16B_OUT32B_NOSWAP_NEON_SIZE_THRESHOLD         8
#define COPY_IN32B_OUT16B_SWAPIN_NEON_SIZE_THRESHOLD         8
#define COPY_IN32B_OUT16B_SWAPOUT_NEON_SIZE_THRESHOLD        8
#define COPY_IN32B_OUT16B_NOSWAP_NEON_SIZE_THRESHOLD         8
#define COPY_IN32B_OUT32B_SWAP_NEON_SIZE_THRESHOLD           8
#define COPY_IN32B_OUT32B_NOSWAP_NEON_SIZE_THRESHOLD         8



#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// for pcmadapter
AFM_API_IMPORT void _stereo2mono_16_optim    (void *pBuffer, unsigned int size);
AFM_API_IMPORT void _stereo2mono_16_rnd_optim(void *pBuffer, unsigned int size);
AFM_API_IMPORT void _stereo2mono_32_optim    (void *pBuffer, unsigned int size);
AFM_API_IMPORT void _stereo2mono_32_rnd_optim(void *pBuffer, unsigned int size);
AFM_API_IMPORT void _mono2stereo_16_optim    (void *pBuffer, unsigned int size);
AFM_API_IMPORT void _mono2stereo_32_optim    (void *pBuffer, unsigned int size);
AFM_API_IMPORT void _c16to32_optim           (void *pBuffer, unsigned int size);
AFM_API_IMPORT void _c32to16_optim           (void *pBuffer, unsigned int size);
AFM_API_IMPORT void _c32to16_rnd_optim       (void *pBuffer, unsigned int size);
AFM_API_IMPORT void _move_chunk_optim        (void *pIn, void *pOut, unsigned int size);
#ifdef PCMADAPTER_ROUNDING
#define stereo2mono_16_optim _stereo2mono_16_rnd_optim
#define stereo2mono_32_optim _stereo2mono_32_rnd_optim
#define mono2stereo_16_optim _mono2stereo_16_optim
#define mono2stereo_32_optim _mono2stereo_32_optim
#define c16to32_optim        _c16to32_optim
#define c32to16_optim        _c32to16_rnd_optim
#define move_chunk_optim     _move_chunk_optim
#else // PCMADAPTER_ROUNDING
#define stereo2mono_16_optim _stereo2mono_16_optim
#define stereo2mono_32_optim _stereo2mono_32_optim
#define mono2stereo_16_optim _mono2stereo_16_optim
#define mono2stereo_32_optim _mono2stereo_32_optim
#define c16to32_optim        _c16to32_optim
#define c32to16_optim        _c32to16_optim
#define move_chunk_optim     _move_chunk_optim
#endif // PCMADAPTER_ROUNDING

// for shmin & shmout
AFM_API_IMPORT void _swap_bytes_optim(void *buffer, unsigned int size, int swap_mode);
#define swap_bytes_optim _swap_bytes_optim

// for shmpcmin & shmpcmout
AFM_API_IMPORT void _copy_downmix_in16b_out16b_swap_optim      (void *in, void *out, unsigned int size); // AABB     CCDD     -> (BBAA+DDCC)/2
AFM_API_IMPORT void _copy_downmix_in16b_out16b_swap_rnd_optim  (void *in, void *out, unsigned int size); // AABB     CCDD     -> (BBAA+DDCC+1)/2
AFM_API_IMPORT void _copy_downmix_in16b_out16b_noswap_optim    (void *in, void *out, unsigned int size); // AABB     CCDD     -> (AABB+CCDD)/2
AFM_API_IMPORT void _copy_downmix_in16b_out16b_noswap_rnd_optim(void *in, void *out, unsigned int size); // AABB     CCDD     -> (AABB+CCDD+1)/2
AFM_API_IMPORT void _copy_downmix_in16b_out32b_swapin_optim    (void *in, void *out, unsigned int size); // AABB     CCDD     -> (BBAA0000+DDCC0000)/2
AFM_API_IMPORT void _copy_downmix_in16b_out32b_noswap_optim    (void *in, void *out, unsigned int size); // AABB     CCDD     -> (AABB0000+CCDD0000)/2
AFM_API_IMPORT void _copy_downmix_in32b_out16b_swapin_optim    (void *in, void *out, unsigned int size); // AABBCCDD EEFFGGHH -> ((DDCCBBAA+HHGGFFEE)/2)>>16
AFM_API_IMPORT void _copy_downmix_in32b_out16b_swapin_rnd_optim(void *in, void *out, unsigned int size); // AABBCCDD EEFFGGHH -> ((DDCCBBAA+HHGGFFEE)/2+2^15)>>16
AFM_API_IMPORT void _copy_downmix_in32b_out16b_noswap_optim    (void *in, void *out, unsigned int size); // AABBCCDD EEFFGGHH -> ((AABBCCDD+EEFFGGHH)/2)>>16
AFM_API_IMPORT void _copy_downmix_in32b_out16b_noswap_rnd_optim(void *in, void *out, unsigned int size); // AABBCCDD EEFFGGHH -> ((AABBCCDD+EEFFGGHH)/2+2^15)>>16
AFM_API_IMPORT void _copy_downmix_in32b_out32b_swap_optim      (void *in, void *out, unsigned int size); // AABBCCDD EEFFGGHH -> (DDCCBBAA+HHGGFFEE)/2
AFM_API_IMPORT void _copy_downmix_in32b_out32b_swap_rnd_optim  (void *in, void *out, unsigned int size); // AABBCCDD EEFFGGHH -> (DDCCBBAA+HHGGFFEE+1)/2
AFM_API_IMPORT void _copy_downmix_in32b_out32b_noswap_optim    (void *in, void *out, unsigned int size); // AABBCCDD EEFFGGHH -> (AABBCCDD+EEFFGGHH)/2
AFM_API_IMPORT void _copy_downmix_in32b_out32b_noswap_rnd_optim(void *in, void *out, unsigned int size); // AABBCCDD EEFFGGHH -> (AABBCCDD+EEFFGGHH+1)/2
AFM_API_IMPORT void _copy_upmix_in16b_out16b_swap_optim        (void *in, void *out, unsigned int size); // AABB              -> BBAA                BBAA
AFM_API_IMPORT void _copy_upmix_in16b_out16b_noswap_optim      (void *in, void *out, unsigned int size); // AABB              -> AABB                AABB
AFM_API_IMPORT void _copy_upmix_in16b_out32b_swapin_optim      (void *in, void *out, unsigned int size); // AABB              -> BBAA0000            BBAA0000
AFM_API_IMPORT void _copy_upmix_in16b_out32b_noswap_optim      (void *in, void *out, unsigned int size); // AABB              -> AABB0000            AABB0000
AFM_API_IMPORT void _copy_upmix_in32b_out16b_swapin_optim      (void *in, void *out, unsigned int size); // AABBCCDD          -> DDCC                DDCC
AFM_API_IMPORT void _copy_upmix_in32b_out16b_swapin_rnd_optim  (void *in, void *out, unsigned int size); // AABBCCDD          -> (DDCCBBAA+2^15)>>16 (DDCCBBAA+2^15)>>16
AFM_API_IMPORT void _copy_upmix_in32b_out16b_noswap_optim      (void *in, void *out, unsigned int size); // AABBCCDD          -> AABB                AABB
AFM_API_IMPORT void _copy_upmix_in32b_out16b_noswap_rnd_optim  (void *in, void *out, unsigned int size); // AABBCCDD          -> (AABBCCDD+2^15)>>16 (AABBCCDD+2^15)>>16
AFM_API_IMPORT void _copy_upmix_in32b_out32b_swap_optim        (void *in, void *out, unsigned int size); // AABBCCDD          -> DDCCBBAA            DDCCBBAA
AFM_API_IMPORT void _copy_upmix_in32b_out32b_noswap_optim      (void *in, void *out, unsigned int size); // AABBCCDD          -> AABBCCDD            AABBCCDD
AFM_API_IMPORT void _copy_in16b_out16b_swap_optim              (void *in, void *out, unsigned int size); // AABB              -> BBAA
AFM_API_IMPORT void _copy_in16b_out16b_noswap_optim            (void *in, void *out, unsigned int size); // AABB              -> AABB
AFM_API_IMPORT void _copy_in16b_out32b_swapin_optim            (void *in, void *out, unsigned int size); // AABB              -> BBAA0000
AFM_API_IMPORT void _copy_in16b_out32b_swapout_optim           (void *in, void *out, unsigned int size); // AABB              -> 0000BBAA
AFM_API_IMPORT void _copy_in16b_out32b_noswap_optim            (void *in, void *out, unsigned int size); // AABB              -> AABB0000
AFM_API_IMPORT void _copy_in32b_out16b_swapin_optim            (void *in, void *out, unsigned int size); // AABBCCDD          -> DDCC
AFM_API_IMPORT void _copy_in32b_out16b_swapin_rnd_optim        (void *in, void *out, unsigned int size); // AABBCCDD          -> (DDCCBBAA+2^15)>>16
AFM_API_IMPORT void _copy_in32b_out16b_swapout_optim           (void *in, void *out, unsigned int size); // AABBCCDD          -> BBAA
AFM_API_IMPORT void _copy_in32b_out16b_swapout_rnd_optim       (void *in, void *out, unsigned int size); // AABBCCDD          -> FFEE with EEFF=(AABBCCDD+2^15)>>16
AFM_API_IMPORT void _copy_in32b_out16b_noswap_optim            (void *in, void *out, unsigned int size); // AABBCCDD          -> AABB
AFM_API_IMPORT void _copy_in32b_out16b_noswap_rnd_optim        (void *in, void *out, unsigned int size); // AABBCCDD          -> (AABBCCDD+2^15)>>16
AFM_API_IMPORT void _copy_in32b_out32b_swap_optim              (void *in, void *out, unsigned int size); // AABBCCDD          -> DDCCBBAA
AFM_API_IMPORT void _copy_in32b_out32b_noswap_optim            (void *in, void *out, unsigned int size); // AABBCCDD          -> AABBCCDD
#ifdef SHMPCM_ROUNDING
#define copy_downmix_in16b_out16b_swap_optim   _copy_downmix_in16b_out16b_swap_rnd_optim
#define copy_downmix_in16b_out16b_noswap_optim _copy_downmix_in16b_out16b_noswap_rnd_optim
#define copy_downmix_in16b_out32b_swapin_optim _copy_downmix_in16b_out32b_swapin_optim
#define copy_downmix_in16b_out32b_noswap_optim _copy_downmix_in16b_out32b_noswap_optim
#define copy_downmix_in32b_out16b_swapin_optim _copy_downmix_in32b_out16b_swapin_rnd_optim
#define copy_downmix_in32b_out16b_noswap_optim _copy_downmix_in32b_out16b_noswap_rnd_optim
#define copy_downmix_in32b_out32b_swap_optim   _copy_downmix_in32b_out32b_swap_rnd_optim
#define copy_downmix_in32b_out32b_noswap_optim _copy_downmix_in32b_out32b_noswap_rnd_optim
#define copy_upmix_in16b_out16b_swap_optim     _copy_upmix_in16b_out16b_swap_optim
#define copy_upmix_in16b_out16b_noswap_optim   _copy_upmix_in16b_out16b_noswap_optim
#define copy_upmix_in16b_out32b_swapin_optim   _copy_upmix_in16b_out32b_swapin_optim
#define copy_upmix_in16b_out32b_noswap_optim   _copy_upmix_in16b_out32b_noswap_optim
#define copy_upmix_in32b_out16b_swapin_optim   _copy_upmix_in32b_out16b_swapin_rnd_optim
#define copy_upmix_in32b_out16b_noswap_optim   _copy_upmix_in32b_out16b_noswap_rnd_optim
#define copy_upmix_in32b_out32b_swap_optim     _copy_upmix_in32b_out32b_swap_optim
#define copy_upmix_in32b_out32b_noswap_optim   _copy_upmix_in32b_out32b_noswap_optim
#define copy_in16b_out16b_swap_optim           _copy_in16b_out16b_swap_optim
#define copy_in16b_out16b_noswap_optim         _copy_in16b_out16b_noswap_optim
#define copy_in16b_out32b_swapin_optim         _copy_in16b_out32b_swapin_optim
#define copy_in16b_out32b_swapout_optim        _copy_in16b_out32b_swapout_optim
#define copy_in16b_out32b_noswap_optim         _copy_in16b_out32b_noswap_optim
#define copy_in32b_out16b_swapin_optim         _copy_in32b_out16b_swapin_rnd_optim
#define copy_in32b_out16b_swapout_optim        _copy_in32b_out16b_swapout_rnd_optim
#define copy_in32b_out16b_noswap_optim         _copy_in32b_out16b_noswap_rnd_optim
#define copy_in32b_out32b_swap_optim           _copy_in32b_out32b_swap_optim
#define copy_in32b_out32b_noswap_optim         _copy_in32b_out32b_noswap_optim
#else // SHMPCM_ROUNDING
#define copy_downmix_in16b_out16b_swap_optim   _copy_downmix_in16b_out16b_swap_optim
#define copy_downmix_in16b_out16b_noswap_optim _copy_downmix_in16b_out16b_noswap_optim
#define copy_downmix_in16b_out32b_swapin_optim _copy_downmix_in16b_out32b_swapin_optim
#define copy_downmix_in16b_out32b_noswap_optim _copy_downmix_in16b_out32b_noswap_optim
#define copy_downmix_in32b_out16b_swapin_optim _copy_downmix_in32b_out16b_swapin_optim
#define copy_downmix_in32b_out16b_noswap_optim _copy_downmix_in32b_out16b_noswap_optim
#define copy_downmix_in32b_out32b_swap_optim   _copy_downmix_in32b_out32b_swap_optim
#define copy_downmix_in32b_out32b_noswap_optim _copy_downmix_in32b_out32b_noswap_optim
#define copy_upmix_in16b_out16b_swap_optim     _copy_upmix_in16b_out16b_swap_optim
#define copy_upmix_in16b_out16b_noswap_optim   _copy_upmix_in16b_out16b_noswap_optim
#define copy_upmix_in16b_out32b_swapin_optim   _copy_upmix_in16b_out32b_swapin_optim
#define copy_upmix_in16b_out32b_noswap_optim   _copy_upmix_in16b_out32b_noswap_optim
#define copy_upmix_in32b_out16b_swapin_optim   _copy_upmix_in32b_out16b_swapin_optim
#define copy_upmix_in32b_out16b_noswap_optim   _copy_upmix_in32b_out16b_noswap_optim
#define copy_upmix_in32b_out32b_swap_optim     _copy_upmix_in32b_out32b_swap_optim
#define copy_upmix_in32b_out32b_noswap_optim   _copy_upmix_in32b_out32b_noswap_optim
#define copy_in16b_out16b_swap_optim           _copy_in16b_out16b_swap_optim
#define copy_in16b_out16b_noswap_optim         _copy_in16b_out16b_noswap_optim
#define copy_in16b_out32b_swapin_optim         _copy_in16b_out32b_swapin_optim
#define copy_in16b_out32b_swapout_optim        _copy_in16b_out32b_swapout_optim
#define copy_in16b_out32b_noswap_optim         _copy_in16b_out32b_noswap_optim
#define copy_in32b_out16b_swapin_optim         _copy_in32b_out16b_swapin_optim
#define copy_in32b_out16b_swapout_optim        _copy_in32b_out16b_swapout_optim
#define copy_in32b_out16b_noswap_optim         _copy_in32b_out16b_noswap_optim
#define copy_in32b_out32b_swap_optim           _copy_in32b_out32b_swap_optim
#define copy_in32b_out32b_noswap_optim         _copy_in32b_out32b_noswap_optim
#endif // SHMPCM_ROUNDING

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _bindings_optim_h_
