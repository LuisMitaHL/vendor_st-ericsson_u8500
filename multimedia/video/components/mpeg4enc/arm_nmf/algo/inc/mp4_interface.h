/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef MP4_INTERFACE
#define MP4_INTERFACE


#define MAIN_NAME          "MPEG-4 Encoder"
#define MAIN_OWNER         "Advanced System Technology, Agrate Lab."
#define MAIN_COMPANY       "ST Ericsson"

#define MAIN_VERSION       "4.3"
#define MAIN_DATE          "January 2010"

/* Motion Estimator to use */
#define ESTIMATE_ALGORITHMIC_SLIMPEG      0
#define ESTIMATE_ALGORITHMIC_FULLSEARCH   1
#define ESTIMATE_ARCHITECTURAL            2

#define MAX_PATHNAME_LEN   1024    /* FP: max length for a pathname */ 

struct parameter_value {
  unsigned int   param_id;                              /* internal descriptor of the parameter */
  unsigned int   i_param_value;                         /* integer      value of the parameter      */
  double         df_param_value;                        /* double float value of the parameter  */
  char           str_param_value[MAX_STR_PARAM];        /* FP: the string value of the parameter */
  int            index;                                 /* FP: index in the descr structure (or -1 if not found) */
};

typedef struct  {
  int            activated;         /* FP: in the command line? 1=yes 0=No  */
  unsigned int   param_id;          /* internal descriptor of the parameter */
  unsigned int   param_type;        /* 0=integer, 1=fp, 2=bool, 3=nopar     */
  unsigned int   i_param_default;   /* integer default value of parameter   */
  double         df_param_default;  /* double default value of parameter    */
  unsigned int   i_min_val;         /* integer minimum value                */
  unsigned int   i_max_val;         /* integer maximum value                */
  double         df_min_val;        /* double minimum  value                */
  double         df_max_val;        /* double maximum  value                */
  const char          *c_short_ident;     /* short identifier of the parameter    */
  const char          *c_long_ident;      /* long  identifier of the parameter    */
  const char          *c_description;     /* description of the parameter         */
  struct parameter_value value;     /* store here the parsed value          */
}parameter_descr;


/* option codes */
 
#define FCODE_VAL       1     
#define RLOC_BR         4    
#define RINT_BR         5    
#define RC_SKIPP        6    
#define RC_MAXQ         7    
#define RC_MAXERR       8    
#define RC_BITRATE      9   
#ifndef NOMADIK_VERSION 
#define RC_METH         10   
#endif
#define IMG_FORMAT      11   
#define NUMBER_FR       12   
#define ME_ALGO         13   
#define SHDR_FLAG       14   
#define MP2_QUANT       15   
#define HELP            16   
#define FIX_QP          17   
#define FIX_IQP         18   
#define FIX_PQP         19   
#define P_FRM_NUM       20   
#define FRAMERT         21   
#define SKIP_FRM        22   
#define VP_LNGT         23   
#define RVLC_FLAG       24   
#define HEC_FREQ        25   
#define WDTH_FRM        26   
#define HGTH_FRM        27   
#define REMPEG_ALG      28   
#define UMV_FLAG        29   
#define ANNEXI          30   
#define PROF_IND        31   
#define LEVL_IND        32   
#define AC_ENBL         33   
#define MINQP_VAL       34   
#define TM_INC_RS       35
#define TM_INC          36
#define VBV_BUFSIZE     37
#define VBV_OCCUPANCY   38
#define FCODE_VAL_MAX   39
#define PANIC_MODE      40
#define MISMATCH_CORR   41
#define VP_MB_LNGT      42 
#define INTRA_FORCING   43 
#define SH_GOB_HDR      44
#define INTRA_REFR      45
#define VBV_FORCING     46
#define AD_INTRA_REFR   47
#define SINGLE_I        48
#define VOS_START_CODE  49
#define SWIS_BUF_SIZE   50
#define FILE_SIZE       52
#define TARGET_PSNR     53
#define ZERO_RAND_SEED  54
#define RES_STRENGTH    55      /* FP: Resilience Strength */
#define DYN_OPTIONS     56      /* FP: Specify the text filename with dynamic line-parameters */
#define VOL_INTRA       57      /* FP: Insert a VOL header before each INTRA frame */
#define SLICE_LOSS	58	/* FP: define a slice-loss event */
#define PRE_PROC	59	/* FP: enable pre-processing process */
#define IMG_STAB_PROC	60	/* FP: enable image stabilization process*/
#define ENCODER_PROC	61	/* FP: enable the MPEG-4 encoding process */

#define PRE_SIZE_IN	62      /* FP: pre-processing input sequence size */
#define PRE_CROP_SIZE	63      /* FP: pre-processing enable cropping and specify cropping size */
#define PRE_CROP_POS	64      /* FP: pre-processing enable cropping and specify cropping size */
#define PRE_SIZE_OUT	65      /* FP: pre-processing sequence size after re-sizing */
#define PRE_ACE_ENABLE		66      /* FP: pre-processing ACE Enable */
#define PRE_ACE_STRENGTH	67      /* FP: pre-processing ACE Strength */
#define PRE_ACE_IN_RANGE	68      /* FP: pre-processing ACE Input Range */
#define PRE_ACE_OUT_RANGE	69      /* FP: pre-processing ACE Output Range */
#define PRE_ACE_DECIM		70      /* FP: pre-processing ACE Enable Decimation*/




#define TARGET_DUR		71
#define MIN_FRAMERATE		72
#define SP_QUALITY		73
#define BUFFER_TYPE		74
#define FAKE_TIME_STAMPS	75	/* FP: modify the time-stamps to recover the initial delay */
#define BITS_PER_FRAME		76	/* FP: bit per frame */
#define GRAB_FAILURE		77	/* FP: simulate a grab-failure (only in dynopt file) */
#define TIME_STAMPS_FILE		78	/* FP: the filename that contains the time-stamps */
/* segmented mode */
#define MAXGOBBITSIZE       79 /* max size in bits for a segment (i.e. GOB) in SH */


//#ifdef NOMADIK_VERSION
#define SCENARIO        100
//#endif


                     
#define END_LIST        65534
#define NOT_PRESENT     65535

/*****************************************************************************/
/** 
 * \brief Global Structure for all parameters of current frame. 
 * Received also CBR and VBR global variables that are common to both 
 * algorithms adapted from the same structure in MaSa_ratectrl.c/MMS_ratectrl.c
 * replacing unsigned int   by t_uint32
 *           unsigned long  by t_uint32
 *                    int   by t_sint32
 *           unsigned short by t_uint16
 *                    short by t_sint16
 * except: t_uint16 for Q, Qp, prevQp, Smb, vop_time_increment_resolution
 */
/*****************************************************************************/
typedef struct {

    /** computed values  */
    //t_uint16 frame_number;       /**<\brief Image number in sequence, for debug */
    t_uint16 mb_width;           /**<\brief Width and height in number of MB */
    t_uint16 mb_height;
    t_uint16 mb_number;
    t_uint16 header_size;        /**<\brief Size of the header in bits, used in constant_rate_control */

    /*  in vars */
    t_uint32 brc_frame_target;
    t_uint32 brc_target_min_pred; 
    t_uint32 brc_target_max_pred;

    /*    in-out vars    */
    t_uint32 bitstream_size;      /*  total number of bits to encode the current frame */
    t_uint32 stuffing_bits;
  
    /*    out vars */
    t_uint16 brc_skip_prev;

    /*    stats-global vars    */
    t_uint32 bit_rate;
    t_sint32 delta_target;
    t_uint32 Smax;
    t_uint32 max_buff_level;          /* used in CBR */

    /*    stats-prev vars */
    t_uint32 pictCount;               
    t_uint32 last_I_Size;             
    t_uint32 comp_SUM;               
    t_uint32 comp_count;              
    t_uint32 ts_seconds_old;    
    t_uint32 avgSAD;                  
    t_uint32 seqSAD;                  
    t_uint32 TotSkip;                 /* used in VBR */
    t_uint32 Skip_Current;            /* used in VBR and VBR */
    t_uint32 PictQpSum;               /* used in CBR */
    t_uint32 S_overhead;              /* used in CBR */

    /*    other HAMAC BRC vars    */
    t_uint32 BUFFER_depletion;
    t_uint32 BUFFER_mod;
    t_uint32 PicTarget;
    t_uint32 I_PicTarget;
    t_uint32 P_PicTarget;
    t_uint32 skip_level;
    t_uint32 Tmin;
    t_uint32 Tmin_UF;
    t_uint16 delta_T_stamp;
    t_uint32 last_size;
    t_sint32 buffer_fullness;
    t_uint16 brc_skip;


   //CBR parameters
    t_uint16 marker_bits; 
	/** <\brief added in FW to keep number of bits added
                           * for DM markers so as to remove it from Sm, value
                           * 0, 17 or 19, used in m4e_masa_cbr_overheade() 
                           */
  t_sint32 buffer_fullness_fake_TS;
  
  t_uint32 PictQpSumIntra;
  
  t_uint16 MBnum;

  /* GT: enanched MB layer control for better Qp distribution, 02 Apr 07 */
  t_uint16 BPPmbPrevAct ;
  t_sint32 BPPmbErr	;
  t_uint16 BPPmbPrevEst 	;
  t_sint32 Lprev 	;
  t_uint16 CodedMB 		;
  t_uint16 CodedMB_flag 		;

  //in params
  t_uint16 picture_coding_type;
  t_uint16 frame_width;              /**<\brief Width  in pixels from current frame         */
  t_uint16 frame_height;             /**<\brief Height in pixels from current frame         */
  t_uint16 vp_bit_size;          
  t_uint16 vp_size_max;       
  t_uint16 flag_short_header;        /**<\brief Short header mode if =1                     */
  t_uint16 brc_type;                 /**<\brief the bit rate control (BRC) algorithm       */
  t_uint16 first_I_skipped_flag;  /**<\brief from mainver24d */
  t_sint16  init_ts_modulo_old;      /**<\brief from mainver24d */
  t_uint16 I_Qp;              /**<\brief Initial quantization parameter for intra picture */
  t_uint16 P_Qp;              /**<\brief Initial quantization parameter for inter picture */
  t_uint16 Cprev;             /**<\brief Previous header size in CBR                */
  t_uint16 BPPprev;           /**<\brief Previous bit per pixel parameter in CBR    */
  t_sint16  ts_modulo_old;     /**<\brief Previous vop time increment, signed */

} ts_m4e_global_param;


typedef struct { 
    t_uint16 index;           /**<\brief Index of current MB */
    t_uint16 type;   /**<\brief INTRA, INTER... */
    t_uint16 not_coded;       /**<\brief If decision of not coded MB */
    t_sint16 dquant;          /**<\brief Current dquant */
    t_uint16 QP;              /**<\brief Quantization parameter */
    t_uint16 mad;             /**<\brief MAD if intra */
    t_uint16 sad;             /**<\brief SAD if inter */
    t_sint16 MVx;             /**<\brief X Coordinate of MV */
    t_sint16 MVy;             /**<\brief Y Coordinate of MV */
    t_uint16 index_block;     /**<\brief Current block */
    t_uint16 bx;              /**<\brief bx and by of current block */
    t_uint16 by;              /**<\brief for ACDC Prediction */
    t_uint16 chroma;          /**<\brief If 0=>luma block, if 1=>U, if 2=>V */
    t_uint16 valid;           /**<\brief for MB FIFO */
    t_uint16 DC[6];           /* Saved DC for data partitioning*/
} ts_mb_info, *tps_mb_info;


typedef enum {MB_INTER, MB_INTER_Q,
              MB_INTER_4MV, MB_INTRA,
              MB_INTRA_Q} e_macroblock_type;

/* Types for MPEG4 */
typedef enum {M4E_PICT_I, M4E_PICT_P, M4E_PICT_B} e_m4e_picture_type;

#define UNCONVERT_ENDIANNESS(x,y) y=x
#define CONVERT_ENDIANNESS(x) x
/* option codes */




void mp4e_SetDefault(mp4_parameters * mp4_par);
int  mp4e_ParseParameters(int argc, char **argv, mp4_parameters * mp4_par);
void mp4e_PrintHelp(char* prgname);
int  mp4e_ParseOneParameter(char *string, struct parameter_value *pv, parameter_descr *pd);
int  filesize( char *fname );	/* FP: system-independent file-size utility */

int  mp4e_SetParameter(char *param, mp4_parameters *mp4_par);
int  mp4e_IsActivated(unsigned int param_id);
void mp4e_setResStrength(mp4_parameters * mp4_par);
int  mp4e_ProfileAndLevelConstraints(mp4_parameters * mp4_par);
int  mp4e_EncoderLimitations(mp4_parameters * mp4_par);
void mp4e_dump_options_summary(mp4_parameters * mp4_par);



								  



#endif


