/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   volctrl.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _volctrl_h_
#define _volctrl_h_

#include "audiolibs_common.h"


#if !(defined (MMDSP) && !defined(__flexcc2__))
#ifndef _wtype_
#define _wtype_
typedef long MMlong;
typedef int MMshort;
typedef long long MMlonglong;
#endif
#endif


#define GLL 0
#define GLR 1
#define GRL 2
#define GRR 3
#define START_IDX_MCH (GRR+1)
#define MAX_MCH 8
#ifndef MMDSP
#define MAX_CH (MAX_MCH+START_IDX_MCH)
#else
#define MAX_CH START_IDX_MCH
#endif
#define G0	START_IDX_MCH
#define G1	(G0+1)
#define G2	(G1+1)
#define G3	(G2+1)
#define G4	(G3+1)
#define G5	(G4+1)
#define G6	(G5+1)
#define G7	(G6+1)


typedef enum{
  GLL_MASK = (1<<GLL),
  GLR_MASK = (1<<GLR),
  GRL_MASK = (1<<GRL),
  GRR_MASK = (1<<GRR),
  LEFT_MASK        = (GLL_MASK | GRL_MASK),
  RIGHT_MASK       = (GLR_MASK | GRR_MASK),
  RIGHT_GLL        = (RIGHT_MASK | GLL_MASK),
  RIGHT_GRL        = (RIGHT_MASK | GRL_MASK),
  LEFT_GRR         = (LEFT_MASK | GRR_MASK),
  LEFT_GLR         = (LEFT_MASK | GLR_MASK),
  DIRECT_GAIN_MASK = (GRR_MASK | GLL_MASK),
  CROSS_GAIN_MASK  = (GRL_MASK | GLR_MASK),
  ALL_MASK         = (LEFT_MASK | RIGHT_MASK),
  G0_MASK = (1<<G0),
  G1_MASK = (1<<G1),
  G2_MASK = (1<<G2),
  G3_MASK = (1<<G3),
  G4_MASK = (1<<G4),
  G5_MASK = (1<<G5),
  G6_MASK = (1<<G6),
  G7_MASK = (1<<G7)
}volctrl_mask_t;
  
/**/
#define ONE 0x7fffff

#define sixdB_Q8 0x605 // 6.02 in Q8 representation

/* offset of the command receive from aep */
#define VOLCTRL_OFFSET_GAIN 0x8000 /* 128dB in Q8 unsigned representation */
#ifndef MMDSP
#define onedB                (1.0)
#define onedB_Q8             (1<<8)
#define threedB              (3.0)
#define sixdB				 (6.0)
#define VOLCTRL_MINIMUM_GAIN (-114.0)
#define VOLCTRL_MINIMUM_GAIN_LIN (2e-6)
#else
#define onedB                (1<<8)
#define threedB              (3 * onedB)
#define sixdB				 (6 * onedB)
#define VOLCTRL_MINIMUM_GAIN (-114 * onedB) /* minimum gain in Q8 */
#endif


/* default value */
#define ALPHA_DEFAULT 0x7f0000 


/* GUARD_BIT for positive gain, each bit allow to 6db amplification */
#define GUARD_BIT (4) // +24 dB max 


#define VOLCTRL_MAX_CMD 2
#define VOLCTRL_IMMEDIAT_CMD_PENDING (1<<1)

#define VOLCTRL_INPUT_UNDEFINED 0
#define VOLCTRL_INPUT_MONO 1
#define VOLCTRL_INPUT_STEREO 2
#define VOLCTRL_INPUT_DUAL_MONO 3


#define INVSQROOT2 FORMAT_FLOAT(7.07106781e-01, MAXVAL)


/*
 * local struct
 */
#ifndef MMDSP
#define VOLCTRL_EPSILON 0.004
typedef struct VOLCRTL_LOCAL_STRUCT_T
{
     int gain_cmd[VOLCTRL_MAX_CMD][MAX_CH];// command gain in Q8 dB 
     float  gain[MAX_CH];                  // linear command gain 
     double  gain_smoothed[MAX_CH];        // current linear gain 
	 int current_gaindB[MAX_CH];			// current gain in dB (Q8)
     int gain_mask;
     int downmix;
     int upmix;
     int db_ramp;
     int input_mode;
     int new_config;
     int first_time;
     int nb_smooth_iter[MAX_CH];
     double alpha[MAX_CH];
     unsigned int duration[MAX_CH];
	 unsigned int current_duration[MAX_CH];
     int sample_freq;
	 int terminate[MAX_CH];
	 int configL;
	 int configR;
	 int multichannel;
	 int configX[MAX_MCH];
}VOLCTRL_LOCAL_STRUCT_T;
#else
typedef struct VOLCRTL_LOCAL_STRUCT_T
 {
     MMshort gain_cmd[VOLCTRL_MAX_CMD][MAX_CH];// cmd gain in Q8 dB 
     MMlong  gain[MAX_CH];                      	// linear command gain 
     MMlong  gain_smoothed[MAX_CH];             	// current linear gain
	 MMshort current_gaindB[MAX_CH];				// current gain in dB (Q8)
     volctrl_mask_t smooth_gain;
     volctrl_mask_t gain_mask;
     MMshort downmix;
     MMshort db_ramp;
     MMshort input_mode;
     MMshort new_config;
     MMshort first_time;
     MMshort nb_smooth_iter[MAX_CH];
     MMshort alpha[MAX_CH];
     MMshort alpha_shift[MAX_CH];
     unsigned int duration[MAX_CH];
	 unsigned int current_duration[MAX_CH];
     MMshort sample_freq;
	 MMshort sample_freqinv;
	 MMshort terminate[MAX_CH];
	 MMshort configL;
	 MMshort configR;
}VOLCTRL_LOCAL_STRUCT_T;
#endif

/*
 * functions 
 */

#ifndef _NMF_MPC_
#ifndef MMDSP

void process_one_of_nch_no_cross_gain( int *chan, int *out, int chno,
                                        int nb_sample_per_ch, VOLCTRL_LOCAL_STRUCT_T *ctx);


extern void process_one_channel_no_cross_gain( int *chan, int *out, int gain_idx,
                                        int nb_sample, int offset,
                                        VOLCTRL_LOCAL_STRUCT_T *ctx);
extern void process_one_channel_with_cross_gain(
        int *direct_chan, int *cross_chan,
        int *out, int strideout, int nb_sample,
        int direct_gain_idx, int cross_gain_idx,
        VOLCTRL_LOCAL_STRUCT_T *ctx);
extern void process_two_channel_with_cross_gain(   int *chan, int *out, int nb_sample,
                                            VOLCTRL_LOCAL_STRUCT_T *ctx);


//------------------ needed for 16 bit samples -----------------------------------
void process_one_of_nch_no_cross_gain_sample16( short *chan, short *out, int chno,
                                        int nb_sample_per_ch, VOLCTRL_LOCAL_STRUCT_T *ctx);


extern void process_one_channel_no_cross_gain_sample16( short *chan, short *out, int gain_idx,
                                        int nb_sample, int offset,
                                        VOLCTRL_LOCAL_STRUCT_T *ctx);
extern void process_one_channel_with_cross_gain_sample16(
        short *direct_chan, short *cross_chan,
        short *out, int strideout, int nb_sample,
        int direct_gain_idx, int cross_gain_idx,
        VOLCTRL_LOCAL_STRUCT_T *ctx);
extern void process_two_channel_with_cross_gain_sample16(   short *chan, short *out, int nb_sample,
                                            VOLCTRL_LOCAL_STRUCT_T *ctx);

//--------------------------------------------------------------------------------
extern void volctrl_set_new_config(VOLCTRL_LOCAL_STRUCT_T *volctrl);
extern void volctrl_set_new_config_core(VOLCTRL_LOCAL_STRUCT_T *volctrl,int config_index, unsigned int init_current_gain);
extern void volctrl_init(VOLCTRL_LOCAL_STRUCT_T *volctrl,int freq_enum);



#else
extern void process_one_channel_no_cross_gain( MMshort *chan, MMshort *out, MMshort gain_idx,
											   MMshort nb_sample, MMshort offset,
											   VOLCTRL_LOCAL_STRUCT_T *ctx);

extern void process_one_channel_with_cross_gain( 
        MMshort *direct_chan, MMshort *cross_chan,
        MMshort *out, MMshort strideout, MMshort nb_sample,
        MMshort direct_gain_idx, MMshort cross_gain_idx,
        VOLCTRL_LOCAL_STRUCT_T *ctx );

extern void process_two_channel_with_cross_gain( MMshort *chan, MMshort *out, MMshort nb_sample,
												 VOLCTRL_LOCAL_STRUCT_T *ctx );

extern void volctrl_set_new_config(VOLCTRL_LOCAL_STRUCT_T *volctrl);
extern void volctrl_set_new_config_core(VOLCTRL_LOCAL_STRUCT_T *volctrl,int config_index, unsigned int init_current_gain);
extern void volctrl_init(VOLCTRL_LOCAL_STRUCT_T *volctrl,int freq_enum);
#endif
#endif //#ifndef _NMF_MPC_
#endif //_volctrl_h_
