/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*
#############################################################
File:     ast_types.h

Author:   Daniele BAGNI

e-mail:   daniele.bagni@st.com

Org:      Agrate Advanced System Technologies lab.
          STMicroelectronics S.r.l.

Created:  01.03.99

Modified:

COPYRIGHT:
This program is property of Agrate Advanced System Technology
lab, from STMicroelectronics S.r.l. It should not be
communicated outside STMicroelectronics without authorization.
################################################################
*/

#ifndef H_AST_TYPES_H
#define H_AST_TYPES_H

//#include <limits.h>

/* These type definitions are compiler and platform dependent */

typedef  unsigned char       ui8_t; /* 8-bit */
typedef    signed char       si8_t;

typedef  unsigned short     ui16_t; /* 16-bit container on 32-bits architectures */
typedef    signed short     si16_t;

typedef  unsigned long int  ui32_t; /* 32-bit container on 32-bits architectures */
typedef    signed long int  si32_t;

typedef  float               f32_t; /* 32 bit ("float"  C type) floating point registers */
typedef  double              f64_t; /* 64 bit ("double" C type) floating point registers */

/*
 * MPU Common Types
 */
    /* ### MMTC add (08/12/04)         ### */
    /* ### Avoid collision with FW     ### */
#ifndef _MMTC_TYPES_H_
#define _MMTC_TYPES_H_
typedef unsigned  char  t_uint8;
typedef   signed  char  t_sint8;

typedef unsigned short  t_uint16;
typedef   signed short  t_sint16;

typedef unsigned  long  t_uint32;
typedef   signed  long  t_sint32;
#endif
    /* ### End of MMTC add (08/12/04)  ### */


#ifndef NULL
#  define NULL    0
#endif

#define ON      1
#define OFF     0

#define TRUE    1
#define FALSE   0


/************** restricted pointers  **********/

#  define rstr_ui32_t  ui32_t
#  define rstr_ui8_t    ui8_t
#  define rstr_si32_t  si32_t
#  define rstr_si8_t    si8_t
#  define rstr_si16_t  si16_t
#  define rstr_ui16_t  ui16_t

#  define RESTRICT     __restrict   /* Blank */

/*definitions*/
#define 		    SIMPLE_PROFILE  1
#define 	   SIMPLE_SCALABLE_PROFILE  2
#define 		      CORE_PROFILE  3
#define 		      MAIN_PROFILE  4
#define 		     N_BIT_PROFILE  5
#define  ADVANCED_REAL_TIME_SIMPLE_PROFILE  6
#define 	     ADVANCED_CORE_PROFILE  7
#define 	     CORE_SCALABLE_PROFILE  8
#define ADVANCED_CODING_EFFICIENCY_PROFILE  9
#define            ADVANCED_SIMPLE_PROFILE  10
#define 			NO_PROFILE  0

/* FP: Working modes (masks) */
#define MODE_ENCODER	1	/* MPEG-4 encoder */
#define MODE_IMG_STAB	2	/* Image-stabilization */
#define MODE_PRE_PROC	4	/* pre-processing */

/* FP: menaing of the entries in the mb_refresh_map array */
#define REFR_NONE	0	/* non need to refresh this MB */
#define REFR_MARK	1	/* refresh "marked" for this MB but not done yet */

#define MAX_STR_PARAM      80      /* FP: max number of char (includind the final \0) in a string line-argument */

#define MODE_INTRA 0
#define MODE_INTER 1

#define INTRA_FRAME 0
#define INTER_FRAME 1

#define INTER_MB           0
#define INTER_Q_MB         1
#define INTER_4V_MB        2
#define INTRA_MB           3
#define INTRA_Q_MB         4
#define INTER_4V_MB_Q      5

#define MB_SIZE 16

#define PAL_MB_NUMB        80 /* HDTV Prograssive */ /* 45 */

/* By Filippo Santinello: standard CIF clock slot length in fixed point notation */
#define CLOCK_SLOT      (((1001U<<22)+15000U) / 30000U)



/*struct created to encapsulate the parameters*/
typedef struct mp4_parameterstype {
  int            VOP_WIDTH, VOP_HEIGHT;
  int            VOP_WIDTH_ACTUAL, VOP_HEIGHT_ACTUAL;     /* *FP* store the original VOP size (not rounded to 16) */
  int            pframes;
  int            rc_enable;
  int            rc_method;
  int			 brc_type;
  short          bitrate;
  int            target_dur;
  short          buffer_type;
  short          spat_quality;
  short          min_framerate;
  int            frames;
  int            error_resilience;
  int            reversible_vlc;
  int            hec;
  short          format;
  int            shdr_flag;
  int            quant_type;
  int            framerate;
  int            skipped;
  short          I_quant, P_quant;
  int            motion_estimator;
  //float          rc_loc_coef, rc_int_coef; /*they should be deleted after statistical retrieving*/
  int            rc_skip;
  int            rempeg_flag;
  int            tnr;
  int            tnr_sigma_c;      /*it should be deleted after statistical retrieving*/
  int            tnr_sigma_l;      /*it should be deleted after statistical retrieving*/
  unsigned int   fcode;
  unsigned int   fcode_max;
  int            annexi_flag;
  int            umv;
  int            profile;
  int            level;
  int            ac_prediction_enable;
  int            rc_Qmin;
  ui8_t          rc_Qmax, rc_Errmax; /*they should be deleted after statistical retrieving*/
  int            tm_inc_res;
  int            tm_inc;
  int            delta_tm_inc;
  int            vbv_bufsize;
  int            vbv_occupancy;
  int            panic_flag;
  int            if_flag;
  int            mismatch_corr;
  int            vp_mb_size;
  int            sh_gob_hdr;
  /* segmented mode */
  unsigned int   maxGOBbitsize;
    /* INTRA MB REFRESH */
  int			 ir_period;
  int			 air_enable;
  int			 air_strength;
  int            vbv_forcing;
  int            low_delay;
  int            swis_buf_size;
  int            vop_rounding_type;
  int            rtype_mod;            /* FP: vop rounding type modulation, when 1 toggle vop_rounding_type */
  int            zero_rand_seed;       /* FP: use zero as mp4e_random seed  1=zero  0=time() */
  int            res_strength;         /* FP: resilience strength as from table in MM use cases */
  int			 vol_intra;	       /* FP: 1 = insert a VOL header before each INTRA frame */
  unsigned int   encoder_mode;	       /* FP: bit-mask with enabled modes (pre-proc image-stabilization, encoder) */
  int			 pp_in_x;	       /* FP: Pre-Processing input sequence size */
  int	         pp_in_y;
  int	         pp_crop_x;	       /* FP: Pre-Processing cropping window size */
  int	         pp_crop_y;
  int	         pp_crop_orig_x;       /* FP: Pre-Processing cropping window upper-left position */
  int            pp_crop_orig_y;
  int            pp_res_x;             /* FP: Pre-Processing output sequence size (after re-sizing) */
  int            pp_res_y;
  short			 *mb_refresh_map;      /* FP: in MB scan order 0=no refresh 1=MB refresh needed (e.g. because slice-loss) */
  int			 fake_time_stamps;     /* FP: force time-stamps to recover initial delay */
  int			 bits_per_frame;       /* FP: the number of bits available for the current frame (rate-controller) */
  int			 grab_failure;	       /* FP: 1=grab failure at the current frame    0=grab ok (normal) */
  int			 next_grab_failures;   /* FP: contains the number of consecutive grab-failures from the next frame on */
  char			 time_stamps_fname[MAX_STR_PARAM];  /* FP: filename with time-stamps */
  int			 *time_stamps_vect;			/* FP: points to dynamically allocated array of time-stamps */
  int			 time_stamps_vect_size;			/* FP: its size */
  int            max_vp_size;	       /* FP: the maximum video packet size according to profile-level */
  int 			 prev_time_stamp;	/*EL: timestamp of previous frame */
  int			 curr_time_stamp;
  int            flag_pkt_nodp;
  int            total_bitcount;
  int            last_gob_bitstream_position;
  int			 U_OFFSET;
  int			 V_OFFSET;
  int			 U_OFFSET_ACTUAL;
  int			 V_OFFSET_ACTUAL;
  //for h263_p3
  int			 slice_str_mode;
  int			 mod_quant_mode;
  int			 deblock_filter;
  int			 rtype;
  int			 custom_pcf;
  int			 par;
  int			 aic_dc_only;
  int			 check_iq_values;
  int			 pc_tm_inc_res;
  int			 pc_tm_inc;
  int			 slice_len;
  int			 slice_len_mb;
  int			 adv_intra_coding;
  int			 h263_p3;
//#endif
} mp4_parameters;

typedef struct vlc_table
{
  unsigned short        code;
  unsigned char         length;
} VLCtable;


/* FP: contains one time-stamp for either SH or Simple-Profile */
struct time_stamp_t {
	unsigned int   temp_ref;		       /* Short-Header Only:   Temporal Reference */
	int			   vop_time_increment;	           /* Simple-Profile only: the increment in ticks*/
	unsigned short modulo_time_base_flag;  /* Simple-Profile only: 1=change in modulo_time_base  0=no */

	int modulo;     /* common SH,SP: for the current frame */
	int seconds;    /* common SH,SP: for the current frame */
};


#ifdef STATISTIC
struct statistic_s {
  int vp_previous_bits;              /* contains the bit used in the previous macroblock */
  int vp_hdr1_bits;                  /* contains the Bit used for the   first part of the header of the Video Packet */
  int vp_hdr2_bits;                  /* contains the Bit used for the  second part of the header of the Video Packet */
  int vp_data_bits;                  /* contains the Bit used for the texture part               of the Video Packet */
  int vp_total_bits;                 /* contains the Bit used for the Video Packet */
  int vp_sl_phase;                   /* contains the information whether in pre-analisys  */

  int mb_max_size;                   /* contains the information about the maximum MB size in bits */
  int mb_max_frame;                  /* contains the information about the frame where maximum MB was encountered */
  int mb_max_x_position;             /* contains the information about the x position where maximum MB was encountered */
  int mb_max_y_position;             /* contains the information about the y position where maximum MB was encountered */
  int mb_max_coding_type;            /* contains the information about what coding type was used for the MB */
  int mb_previous_bits;              /* contains the number of bits where the previous macroblock was stopped */

  int pic_size;                      /* contains the number of bits for the current picture */

  int max_pel_vp_length;             /* contains the maximum size of the Video Packet */
  int num_incorrect_vp;              /* contains the number of incorrect generated Video Packets */
};
#endif /* STATISTIC */

typedef ui8_t uint8;
typedef t_sint16 sint16;
typedef ui16_t uint16;
typedef ui32_t uint32;
typedef ui8_t PEL;

#endif  /* H_AST_TYPES_H */



