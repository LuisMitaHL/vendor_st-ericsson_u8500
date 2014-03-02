/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   mixer.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _mixer_h_
#define _mixer_h_

//#define NEW_ENVELOPE_DETECT //more accurate averaging calculation
#define SAMPLE16 0x10000
#define DATA_BLOCK_SIZE_MASK 0xffff
// Memory Preset
#ifndef _NMF_MPC_
#include "aep.h"
#else
#include "audiolibs_common.h"
//#define INVSQROOT2 FORMAT_FLOAT(7.07106781e-01, MAXVAL)
#endif

#define MIXER_DEFAULT_MEM_PRESET 0 // TCM

#ifdef __flexcc2__
#define MIXER_NB_MEM_PRESET 3
#else
#define MIXER_NB_MEM_PRESET 2
#endif

extern MEMORY_PRESET_T const EXTERN mixer_mem_preset_table[MIXER_NB_MEM_PRESET];
#ifdef MMDSP //mmdsp
#define MMshort int
#ifdef __flexcc2__
#define MMlong long
#else
#define MMlong long long
#endif
#define MMlonglong long long
#else
#define MMshort int
#define MMlong double
#define MMlonglong double
#endif





// define used by interface
#define LEFT_CHANNEL  0
#define RIGHT_CHANNEL 1

#define GUARD_BIT 4
#define S
#define MAXCH 6


#ifdef MMDSP
#define invSQROOT2 FORMAT_FLOAT(7.07106781e-01, MAXVAL)
#define THRESHOLD_DEFAULT (0x4e2000 >> GUARD_BIT)

#ifdef NEW_ENVELOPE_DETECT
#define ALPHA_UP_DEFAULT  FORMAT_FLOAT(0.9375,MAXVAL)
#define ALPHA_UP_DEFAULT_SHIFTVAL 4
#define ALPHA_DW_DEFAULT  FORMAT_FLOAT(0.99999952316284,MAXVAL)
#define ALPHA_DW_DEFAULT_SHIFTVAL 21
#else
#define ALPHA_UP_DEFAULT  FORMAT_FLOAT(0.95,MAXVAL)
#define ALPHA_DW_DEFAULT  FORMAT_FLOAT(0.9999995,MAXVAL)
#endif

#define COEF_1 0x0091ab61d92dL
#define COEF_2 0xfa78ef7cf0e9UL
#define COEF_3 0x4254760d3d6eL
#define COEF_4 0x0286d57a8be8L
#else
#define MAXVAL32   2147483647.0
#define MINVAL32 (-2147483648.0)
#define NBITS 32 //EQUIVALENT NBITS FOR INTERNAL CALCULATION
#define SIGNAL_ATTENUATOR 1.0/(pow(2,32-NBITS))  
#define GUARD_ATTENUATOR pow(2.,-GUARD_BIT)
#define invSQROOT2 7.07106781e-01
#define THRESHOLD_DEFAULT (0x4e200000 >> (GUARD_BIT+32-NBITS))

#ifdef NEW_ENVELOPE_DETECT
#define ALPHA_UP_DEFAULT  0.9375 
#define ALPHA_DW_DEFAULT  0.99999952316284
#else
#define ALPHA_UP_DEFAULT  0.95 
#define ALPHA_DW_DEFAULT  0.9999995
#endif

#define COEF_1  0.00889095836165
#define COEF_2 -0.08636868696099
#define COEF_3  1.03640509884801
#define COEF_4  0.03947960822518
#endif
#define COEF_SHIFT 1



/*
  variables needed for envelop detection
*/
typedef struct MIXER_ENVELOP_STRUCT_T
{
#ifdef DSP
  MMshort abs_x0;
  MMshort abs_x1;
  MMshort abs_in;
  MMshort envelop;
#else
  MMlong abs_x0;
  MMlong abs_x1;
  MMlong abs_in;
  MMlong envelop;
#ifdef NEW_ENVELOPE_DETECT
  MMlong envelop_long;
#endif
#endif
} MIXER_ENVELOP_STRUCT_T;


/*
  mixer local structure
*/
typedef struct MIXER_LOCAL_STRUCT_T
{
  MMshort data_block_size;
#ifdef MMDSP
  MIXER_ENVELOP_STRUCT_T env_st[2];
  MMshort alpha_dw;
  MMshort alpha_up;
  MMshort one_minus_alpha_dw;
  MMshort one_minus_alpha_up;
#ifdef NEW_ENVELOPE_DETECT
  MMshort alpha_dw_shift;
  MMshort alpha_up_shift;
#endif
  MMshort threshold;
  MMlong  coef_poly[4];
#else
  MIXER_ENVELOP_STRUCT_T env_st[MAXCH];
  MMlong alpha_dw;
  MMlong alpha_up;
  MMlong one_minus_alpha_dw;
  MMlong one_minus_alpha_up;
  MMlong threshold;
  MMlong coef_poly[4];
#endif
  MMshort nocompression;
  MMshort forcedownmix;
  MMshort normalisepower;
  MMshort nb_input;
}MIXER_LOCAL_STRUCT_T;

/*
 * functions 
 */
#ifndef _NMF_MPC_ 
extern void mixer_nocompression(MMshort **input_list, int nb_input, MMshort *output,int offset, int nb_sample);
#ifndef MMDSP
extern void mixer_nocompression_sample16(MMshort **input_list, int nb_input, MMshort *output,int offset, int nb_sample);
#endif
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern void mixer_stereo(MMshort **input_list, int nb_input, MMshort *output,int nb_sample, MIXER_LOCAL_STRUCT_T *ctx);
extern void mixer_mono(MMshort **input_list, int nb_input, MMshort *output,int nb_sample, int offset, MIXER_LOCAL_STRUCT_T *ctx);
extern void mixer_nch(MMshort **input_list, int nb_input, MMshort *output,int nb_sample_per_channel, int nch,int nocompression, MIXER_LOCAL_STRUCT_T *ctx);

#ifdef __cplusplus
}
#endif // __cplusplus

extern void mixer_stereo_to_dual_mono(MMshort *ptr, MMshort size, MMshort normalize);
extern void mixer_dual_mono_to_stereo(MMshort *ptr, MMshort size, MMshort normalize);
#ifndef MMDSP
extern void mixer_stereo_to_dual_mono_sample16(MMshort *ptr, MMshort size, MMshort normalize);
extern void mixer_dual_mono_to_stereo_sample16(MMshort *ptr, MMshort size, MMshort normalize);
#endif

#endif

#endif //_mixer_h_
