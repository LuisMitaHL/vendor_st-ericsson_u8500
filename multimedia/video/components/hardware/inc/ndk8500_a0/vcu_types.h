/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _VCU_TYPES_H_
#define _VCU_TYPES_H_

/*****************************************************************************/
/*
 * Includes							       
 */
/*****************************************************************************/


/*****************************************************************************/
/*
 * Variables							       
 */
/*****************************************************************************/


/*****************************************************************************/
/*
 * Types							       
 */
/*****************************************************************************/



typedef enum
{
    RECONSTRUCTION_ENABLE  = 0, /**< \brief  */
    RECONSTRUCTION_DISABLE = 1  /**< \brief  */
} t_no_rec;


/** Same type for enc_qtz_idc_clip and enc_qtz_clip */
typedef enum 
{
    CLIP_IN_MINUS_2047_2047 = 0, /**< \brief clip in [-2047,2047] */
    CLIP_IN_MINUS_1023_1023 = 1, /**< \brief clip in [-1023,1023] */
    CLIP_IN_MINUS_511_511   = 2, /**< \brief clip in [-511,511]   */
    CLIP_IN_MINUS_255_255   = 3, /**< \brief clip in [-255,255]   */
    CLIP_IN_MINUS_127_127   = 4  /**< \brief clip in [-127,127]   */
} t_enc_qtz_clip;


typedef enum
{
    INTRA_DC_DIVIDE_BY_Q  = 0,  /**< \brief divide by q  in quantizer function */
    INTRA_DC_DIVIDE_BY_2Q = 1   /**< \brief divide by 2q in quantizer function */
} t_enc_qtz_param_intra_dc_s;

typedef enum
{
    INTRA_DC_TRUNK_ENABLE = 0,  /**< \brief trunc quantizer function result */
    INTRA_DC_ROUND_ENABLE = 1   /**< \brief round quantizer function result */
} t_enc_qtz_param_intra_dc_t;

typedef enum
{
    INTRA_AC_DIVIDE_BY_Q  = 0,  /**< \brief divide by q  in quantizer function */
    INTRA_AC_DIVIDE_BY_2Q = 1   /**< \brief divide by 2q in quantizer function */
} t_enc_qtz_param_intra_ac_s;

typedef enum
{
    INTRA_AC_TRUNK_ENABLE = 0,  /**< \brief trunc quantizer function result */
    INTRA_AC_ROUND_ENABLE = 1   /**< \brief round quantizer function result */
} t_enc_qtz_param_intra_ac_t;

typedef enum
{
    INTER_DIVIDE_BY_Q     = 0,  /**< \brief divide by q  in quantizer function */
    INTER_DIVIDE_BY_2Q    = 1   /**< \brief divide by 2q in quantizer function */
} t_enc_qtz_param_inter_s;

typedef enum
{
    INTER_TRUNK_ENABLE = 0,     /**< \brief trunc quantizer function result */
    INTER_ROUND_ENABLE = 1      /**< \brief round quantizer function result */
} t_enc_qtz_param_inter_t;


typedef enum
{
    IQTZ_THEN_IPRD = 0,   /**< \brief Perform inverse quantization before inverse prediction   */
    IPRD_THEN_IQTZ = 1    /**< \brief Perform inverse prediction before inverse quantization   */

} t_path_iprd;

typedef enum
{
    SCN_DIR_DISABLE = 0, /**< \brief disable coefficient scaning direction   */
    SCN_DIR_ENABLE  = 1  /**< \brief enable coefficient scaning direction   */
} t_scn_mode;


typedef enum
{
    DCT_SHIFT_DISABLE = 0, /**< \brief no level shifting at the output of the IDCT.   */
    DCT_SHIFT_ENABLE  = 1  /**< \brief level shifting of +128 at the output of the IDCT.   */
} t_dct_shift;

typedef enum
{
    PREDICTION_DISABLE = 0,     /**< \brief disable inverse prediction   */
    PREDICTION_ENABLE  = 1      /**< \brief enable inverse prediction   */
} t_dec_prediction;

typedef enum
{
    QTZ_DISABLE = 0,            /**< \brief No table access   */
    QTZ_ENABLE  = 1             /**< \brief Quantizer step is fetched from table for each coeff   */
} t_qtz_table;

typedef enum
{
    QTZ_QP_DISABLE = 0,            /**< \brief Table access but no access to QP FIFO   */
    QTZ_QP_ENABLE  = 1             /**< \brief Table access + QP FIFO access => for MPEG2 */
} t_qtz_table_qp;

typedef enum
{
    IQTZ_CLIP_WARN_DISABLE = 0, /**< \brief do not update PXP_FIFO_CLIP_OUT   */
    IQTZ_CLIP_WARN_ENABLE  = 1  /**< \brief update PXP_FIFO_CLIP_OUT (XIO ready) register in case of clipping   */
} t_iqtz_clip_warn;

typedef enum
{
    IDCT_OUT_DISABLE = 0,        /**< \brief do not update PXP_FIFO_IDCT_OUT  */
    IDCT_OUT_ENABLE  = 1         /**< \brief update PXP_FIFO_IDCT_OUT with results of IDCT   */
} t_read_idct_out;


typedef enum
{
    SEPARATE_INTRA_DC_DISABLE = 0, /**< \brief Do not separate intra dc run/levels at the output of the RLC  */
    SEPARATE_INTRA_DC_ENABLE  = 1  /**< \brief separate intra dc run/levels at the output of the RLC  */
} t_enc_rl_sep;

typedef enum
{
    DECODE_INTER_BLOCK = 0,        /**< \brief decode an inter block   */
    DECODE_INTRA_BLOCK = 1         /**< \brief decode an intra block   */
} t_dec_intra;

typedef enum
{
    ENCODE_INTER_BLOCK = 0,        /**< \brief encode an inter block   */
    ENCODE_INTRA_BLOCK = 1         /**< \brief encode an intra block   */
} t_enc_intra;


typedef enum
{
    INTRA_DC_A_DISABLE = 0, /**< \brief do not use a coeff in inverse quantizer function for dc coeff */
    INTRA_DC_A_ENABLE  = 1  /**< \brief  use a coeff in inverse quantizer function for dc coeff  */
} t_dec_iqtz_param_intra_dc_a;

typedef enum
{
    INTRA_DC_B_DISABLE = 0, /**< \brief do not use b coeff in inverse quantizer function for dc coeff */
    INTRA_DC_B_ENABLE  = 1  /**< \brief  use b coeff in inverse quantizer function for dc coeff  */
} t_dec_iqtz_param_intra_dc_b;

typedef enum
{
    INTRA_DC_C_DISABLE = 0, /**< \brief do not use c coeff in inverse quantizer function for dc coeff */
    INTRA_DC_C_ENABLE  = 1  /**< \brief  use c coeff in inverse quantizer function for dc coeff  */
} t_dec_iqtz_param_intra_dc_c;

typedef enum
{
    INTRA_DC_E_DISABLE = 0, /**< \brief do not use e coeff in inverse quantizer function for dc coeff */
    INTRA_DC_E_ENABLE  = 1  /**< \brief  use e coeff in inverse quantizer function for dc coeff  */
} t_dec_iqtz_param_intra_dc_e;

typedef enum
{
    INTRA_DC_F_DISABLE = 0, /**< \brief do not use f coeff in inverse quantizer function for dc coeff */
    INTRA_DC_F_ENABLE  = 1  /**< \brief  use f coeff in inverse quantizer function for dc coeff  */
} t_dec_iqtz_param_intra_dc_f;

typedef enum
{
    INTRA_AC_A_DISABLE = 0, /**< \brief do not use a coeff in inverse quantizer function for ac coeff */
    INTRA_AC_A_ENABLE  = 1  /**< \brief  use a coeff in inverse quantizer function for ac coeff  */
} t_dec_iqtz_param_intra_ac_a;

typedef enum
{
    INTRA_AC_B_DISABLE = 0, /**< \brief do not use b coeff in inverse quantizer function for ac coeff */
    INTRA_AC_B_ENABLE  = 1  /**< \brief  use b coeff in inverse quantizer function for ac coeff  */
} t_dec_iqtz_param_intra_ac_b;

typedef enum
{
    INTRA_AC_C_DISABLE = 0, /**< \brief do not use c coeff in inverse quantizer function for ac coeff */
    INTRA_AC_C_ENABLE  = 1  /**< \brief  use c coeff in inverse quantizer function for ac coeff  */
} t_dec_iqtz_param_intra_ac_c;

typedef enum
{
    INTRA_AC_E_DISABLE = 0, /**< \brief do not use e coeff in inverse quantizer function for ac coeff */
    INTRA_AC_E_ENABLE  = 1  /**< \brief  use e coeff in inverse quantizer function for ac coeff  */
} t_dec_iqtz_param_intra_ac_e;

typedef enum
{
    INTRA_AC_F_DISABLE = 0, /**< \brief do not use f coeff in inverse quantizer function for ac coeff */
    INTRA_AC_F_ENABLE  = 1  /**< \brief  use f coeff in inverse quantizer function for ac coeff  */
} t_dec_iqtz_param_intra_ac_f;


typedef enum
{
    OTHERS_A_DISABLE = 0,   /**< \brief do not use a coeff in inverse quantizer function for others coeff */
    OTHERS_A_ENABLE  = 1    /**< \brief  use a coeff in inverse quantizer function for others coeff  */
} t_dec_iqtz_param_others_a;

typedef enum
{
    OTHERS_B_DISABLE = 0,   /**< \brief do not use b coeff in inverse quantizer function for others coeff */
    OTHERS_B_ENABLE  = 1    /**< \brief  use b coeff in inverse quantizer function for others coeff  */
} t_dec_iqtz_param_others_b;

typedef enum
{
    OTHERS_C_DISABLE = 0,   /**< \brief do not use c coeff in inverse quantizer function for others coeff */
    OTHERS_C_ENABLE  = 1    /**< \brief  use c coeff in inverse quantizer function for others coeff  */
} t_dec_iqtz_param_others_c;

typedef enum
{
    OTHERS_E_DISABLE = 0,   /**< \brief do not use e coeff in inverse quantizer function for others coeff */
    OTHERS_E_ENABLE  = 1    /**< \brief  use e coeff in inverse quantizer function for others coeff  */
} t_dec_iqtz_param_others_e;

typedef enum
{
    OTHERS_F_DISABLE = 0,   /**< \brief do not use f coeff in inverse quantizer function for others coeff */
    OTHERS_F_ENABLE  = 1    /**< \brief  use f coeff in inverse quantizer function for others coeff  */
} t_dec_iqtz_param_others_f;


typedef enum
{
    ZIG_ZAG_SCAN    = 0,   /**< \brief coefficients scanned in Zig Zig Order   */
    VERTICAL_SCAN   = 1,   /**< \brief coefficients scanned in "vertical" Order   */
    HORIZONTAL_SCAN = 2,   /**< \brief coefficients scanned in "horizontal" Order   */
    ROW_SCAN        = 3    /**< \brief coefficients scanned in rows   */

} t_scn_dir;


typedef enum
{
    QTAB_ZIG_ZAG_SCAN    = 1,   /**< \brief coefficients scanned in Zig Zig Order   */
    QTAB_VERTICAL_SCAN   = 2,   /**< \brief coefficients scanned in "vertical" Order   */
    QTAB_HORIZONTAL_SCAN = 3,   /**< \brief coefficients scanned in "horizontal" Order   */
    QTAB_ROW_SCAN        = 0    /**< \brief coefficients scanned in rows   */

} t_qtab_scn_dir;

typedef enum
{
  QTAB_TAB_0 = 0,            /**< \brief luma table for JPEG, intra table for MPEG1-2 */
  QTAB_TAB_1 = 1,            /**< \brief chroma 1 table for JPEG, inter table for MPEG1-2 */
  QTAB_TAB_2 = 2             /**< \brief chroma 2 table for JPEG, not used for MPEG1-2 */
} t_qtable_index;


typedef enum
{
    DECODE_CHROMA_BLOCK = 0,   /**< \brief current block is a chroma block   */
    DECODE_LUMA_BLOCK   = 1    /**< \brief current block is a luma block   */

} t_dec_rl_luma;


typedef enum
{
    PA_IDCT_IDLE = 0,      /**< \brief the post adder does not read the IDCT output   */
    PA_IDCT_GET  = 1,      /**< \brief the post adder reads the IDCT output and add it  */
    PA_IDCT_SKIP = 2       /**< \brief the post adder reads the IDCT output and ignores it */
} t_pa_idct_cmd;


typedef enum
{
    PA_PRED_IDLE = 0,      /**< \brief the post adder does not read the predictor   */
    PA_PRED_GET  = 1,      /**< \brief the post adder reads the predictor and add it  */
    PA_PRED_SKIP = 2       /**< \brief the post adder reads the predictor and ignores it */
} t_pa_pred_cmd;

typedef enum
{
    PA_RASTER_8x8   = 0,      /**< \brief 64 pixels   */
    PA_RASTER_4x4   = 1,      /**< \brief 16 pixels   */
    PA_RASTER_16x16 = 2       /**< \brief  */
} t_pa_raster;

typedef enum
{
    PA_SHIFT_DISABLE = 0,      /**< \brief shift disable otherwise  */
    PA_SHIFT_ENABLE = 1        /**< \brief shift enble for VC1 + overlap  */
}t_pa_shift_en;

typedef enum
{
    PA_CLIP_DISABLE = 0,      /**< \brief clip disable otherwise  */
    PA_CLIP_ENABLE = 1        /**< \brief clip enble for VC1  */
}t_pa_clip_en;

typedef enum
{
    PA_FIFO_SEL     = 0,      /**< \brief PA with input from FIFO  */
    PA_REG_SEL      = 1      /**< \brief PA with input from REG   */
}t_pa_reg_sel;

typedef enum
{
    INTERLACE_DISABLE = 0, /**< \brief The PA will not interlace its 2 current input blocks */
    INTERLACE_ENABLE  = 1  /**< \brief The PA will  interlace its 2 current input blocks */
} t_interlace;


typedef enum
{
    RASTER_DISABLE = 0, /**< \brief PA: raster planar mode disabled */
    RASTER_ENABLE  = 1  /**< \brief PA: raster planar mode enabled  */
} t_raster;

typedef enum
{
    CLIPPING_DISABLE = 0, /**< \brief clipping after IQ disabled */
    CLIPPING_ENABLE  = 1  /**< \brief clipping after IQ enabled  */
} t_iq_data_clip;

typedef enum
{
    MISMATCH_DISABLE = 0, /**< \brief IDCT mismatch control disabled */
    MISMATCH_ENABLE  = 1  /**< \brief IDCT mismatch control enabled  */
} t_iq_mismatch;


typedef enum
{
  IPRD_DIR_DC   =   0,   /**< \brief DC inverse prediction */
  IPRD_DIR_AC_V =   1,   /**< \brief AC inv pred vertical  */
  IPRD_DIR_AC_H     =   2,   /**< \brief AC inv pred horizontal */
  IPRD_CMD_VC1_DC_I =   4,
  IPRD_CMD_VC1_AC_V_I =  5,
  IPRD_CMD_VC1_AC_H_I =  6,
  IPRD_CMD_VC1_DC_P   =  8,
  IPRD_CMD_VC1_AC_V_P =  9,
  IPRD_CMD_VC1_AC_H_P =  10,
  IPRD_CMD_BYPASS     =  12

} t_iprd_dir;


typedef enum
{
    IPRD_XBUS  =  0,
    IPRD_CUP_LINK  =  1
}t_iprd_cup;



/* defines the size of the square from which the right and bottom pixels 
   are sent to the IPA unit */ 
typedef enum
{
  PA_RB_NO      = 0,  /* no pixels to be sent */
  PA_RB_4x4     = 1,  /* send RB of every/the current 4x4 square */
  PA_RB_8x8     = 2,  /* send RB of the current 8x8 square */
  PA_RB_16x16   = 3   /* send RB of the current 16x16 square */
} t_pa_rb;


typedef enum
{
  SCN_DECODE_MODE = 0, /* scan configured in decode mode */
  SCN_ENCODE_MODE = 1,  /* scan configured in encode mode */
  SCN_ENCODE_CUP_MODE = 3  /* scan configured in encode mode with CUP for H264enc */
} t_scn_encode;


typedef enum
{
  SCN_CMD_ROW            = 0,    /* natural scan order */
  SCN_CMD_ZZ             = 1,    /* zig zag scan */
  SCN_CMD_AV             = 2,    /* alternate vertical scan */
  SCN_CMD_AH             = 3,    /* alternate horizontal scan */
  SCN_CMD_H264_4x4        = 4,
  SCN_CMD_H264_4x4_15     = 14,
  SCN_CMD_H264_8x8       = 5,
  SCN_CMD_H264_8x8_CAVLC = 15,
  SCN_CMD_H264_2x2       = 13,
  SCN_CMD_VC1_INOR       = 6,
  SCN_CMD_VC1_IHOR       = 7,
  SCN_CMD_VC1_IVER       = 8,
  SCN_CMD_VC1_P8x8       = 9,
  SCN_CMD_VC1_P8x4       = 10,
  SCN_CMD_VC1_P4x8       = 11,
  SCN_CMD_VC1_P4x4       = 12
} t_scn_cmd;
#define SCN_CMD_H264_ZZ SCN_CMD_H264_4x4
#define SCN_CMD_H264_ZZ_15 SCN_CMD_H264_4x4_15

typedef enum
{
  REC_CMD_8x8_LUMA          = 0,  /* 8x8 luma MB format */
  REC_CMD_8x8_CHROMA        = 1,  /* 8x8 chroma MB format */
  REC_CMD_8x8_RASTER_LUMA   = 2,  /* 8x8 luma raster format  (vertically itlace means that
								     first line of Y1 follows first line of Y0 */
  REC_CMD_8x8_RASTER_CHROMA = 3,  /* 8x8 chroma raster format */
  REC_CMD_4x4_LUMA          = 4, 
  REC_CMD_4x4_CHROMA        = 5,
  REC_CMD_4x4_RASTER_LUMA   = 6,
  REC_CMD_4X4_RASTER_CHROMA = 7,
  REC_CMD_16x16_LUMA        = 8,  /* needed for H264 PCM */
  REC_CMD_16x16_RASTER_LUMA = 9,
  REC_CMD_8x8_LUMA_FIELD    = 10,
  REC_CMD_8x8_RASTER_LUMA_FIELD = 11
} t_rec_cmd;

typedef enum
{
  REC_FIELD_INTERLACED      =0,  /* default mode, REC does no deinterlacing */
  REC_FIELD_TOP             =2,  /* TOP field */
  REC_FIELD_BOTTOM          =3   /* BOTTOM field */
} t_rec_field;

typedef enum
{
    VC1IT_FIFO_SEL     = 0,      /**< \brief PA with input from FIFO  */
    VC1IT_REG_SEL      = 1      /**< \brief PA with input from REG   */
}t_vc1it_reg_sel;

typedef enum
{
    VC1IT_CMD_8x8 = 0,  /* Inverse transform 8x8 */
    VC1IT_CMD_8x4 = 1,  /* Inverse transform 8x4 */
    VC1IT_CMD_4x8 = 2,  /* Inverse transform 4x8 */
    VC1IT_CMD_4x4 = 3   /* Inverse transform 4x4 */

}t_vc1it_cmd;


typedef enum
{
    VBF_BLOCK_MODE = 0,        /* VBUFF will be used for VC1 between VC1IT and PA */
    VBF_FIFO_MODE = 1        /* VBUFF is used as a regular FIFO module */
}t_vbf_enable_fifo;

typedef enum
{
    VBF_VERTICAL_OV = 0,        /* do vertical overlap */
    VBF_HORIZONTAL_OV = 1       /* do horizontal overlap */
}t_vbf_direction;


typedef enum
{
    VBF_PARTITION_8x8 = 0,
    VBF_PARTITION_8x4 = 1,
    VBF_PARTITION_4x8 = 2,
    VBF_PARTITION_4x4 = 3
}t_vbf_block_partition;


typedef struct t_lrl {
    t_uint16 last;  /**< \brief Last is 1 when last value */
    t_uint16 run;   /**< \brief Number of zeros since last non-zero level */
    t_sint16 level; /**< \brief level value */
} ts_lrl, *tps_lrl;

typedef enum
{
  H264IQ_LUMA4x4    = 0x0,
  H264IQ_CHROMA     = 0x1,
  H264IQ_LUMA16x16  = 0x2,
  H264IQ_CHROMADC   = 0x3,
  H264IQ_LUMADC     = 0x4
} t_h264iq_cmd;

typedef enum
{
  H264Q_LUMA4x4    = 0x0,
  H264Q_CHROMA     = 0x1,
  H264Q_LUMA16x16  = 0x2,
  H264Q_CHROMADC   = 0x3,
  H264Q_LUMADC     = 0x4
} t_h264q_cmd;

typedef enum
{
  H264T_IT_T4x4   = 0x0,
  H264T_IT_H4x4   = 0x1,
  H264T_IT_H2x2   = 0x2,
  H264T_IT_UNDEF  = 0x3,
  H264T_T_T4x4    = 0x4,
  H264T_T_H4x4    = 0x5,
  H264T_T_H2x2    = 0x6,
  H264T_T_UNDEF   = 0x7
} t_h264t_cmd;

typedef enum
{
  H264IT_IT_T4x4   = 0x0,
  H264IT_IT_H4x4   = 0x1,
  H264IT_IT_H2x2   = 0x2,
  H264IT_IT_UNDEF  = 0x3,
  H264IT_T_T4x4    = 0x4,
  H264IT_T_H4x4    = 0x5,
  H264IT_T_H2x2    = 0x6,
  H264IT_T_UNDEF   = 0x7
} t_h264it_cmd;

typedef enum
{
  H264RLC_XBUS_INPUT = 0x0,
  H264RLC_CUP_INPUT = 0x1
} t_h264rlc_reg_cfg;

typedef enum
{
  H264RLC_CAVLC_4x4 = 0x0,
  H264RLC_CAVLC_2x2 = 0x1,
  H264RLC_CAVLC_15  = 0x2
} t_h264rlc_cmd;

typedef enum 
{
  VLC_JPEG_MODE      = 0x0,
  VLC_MPEG4_SH_MODE  = 0x3,
  VLC_MPEG4_SP_MODE  = 0x1
} t_vlc_type;

typedef enum 
{
  VLC_LUMA    = 0x0,
  VLC_CHROMA  = 0x1
} t_vlc_component;

typedef enum 
{
  VLC_INTRA    = 0x0,
  VLC_INTER    = 0x1
} t_vlc_intra;

typedef enum 
{
  VLC_RVLC_OFF    = 0x0,
  VLC_RVLC_ON     = 0x1
} t_vlc_rvlc;


#endif /* _VCU_TYPES_H_ */
