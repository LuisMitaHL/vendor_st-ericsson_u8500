/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_SVA_H_
#define _VC1D_SVA_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_sva_bitplane.h" /* for ts_bitplane */

/*------------------------------------------------------------------------
 * Types (typedef)
 *----------------------------------------------------------------------*/
 typedef     t_uint8  t_nz_coeff_mask; /* type used to ease readability */

/*------------------------------------------------------------------------
 * Types (Enum)
 *----------------------------------------------------------------------*/
/** \brief Subblock pattern enumeration for TTMB and TTBLK  */
typedef enum
{
    /* Block level transform configurations */
    SBP_8X8         = 0,     /**< \brief 8x8 transform, coded */
    SBP_8X4_BOTTOM   = 1,     /**< \brief 8x4 transform, bottom subblock coded */
    SBP_8X4_TOP      = 2,     /**< \brief 8x4 transform, top subblock coded */
    SBP_8X4_BOTH     = 3,     /**< \brief 8x4 transform, both subblocks coded */
    SBP_4X8_RIGHT    = 4,     /**< \brief 4x8 transform, right subblock coded */
    SBP_4X8_LEFT     = 5,     /**< \brief 4x8 transform, left subblock coded */
    SBP_4X8_BOTH     = 6,     /**< \brief 4x8 transform, both subblocks coded */
    SBP_4X4         = 7,     /**< \brief 4x4 transform, subblock pattern separate */

    /* MB level transform configurations */
    SBP_8X8_MB       = 8,     /**< \brief 8x8 transform, coded, whole MB */
    SBP_8X4_BOTTOM_MB = 9,     /**< \brief 8x4 transform, bottom subblock coded, whole MB */
    SBP_8X4_TOP_MB    = 10,    /**< \brief 8x4 transform, top subblock coded, whole MB */
    SBP_8X4_BOTH_MB   = 11,    /**< \brief 8x4 transform, both subblocks coded, whole MB */
    SBP_4X8_RIGHT_MB  = 12,    /**< \brief 4x8 transform, right subblock coded, whole MB */
    SBP_4X8_LEFT_MB   = 13,    /**< \brief 4x8 transform, left subblock coded, whole MB */
    SBP_4X8_BOTH_MB   = 14,    /**< \brief 4x8 transform, both subblocks coded, whole MB */
    SBP_4X4_MB       = 15,    /**< \brief 4x4 transform, subblocks pattern separate, whole MB */

    SBP_MB_LEVEL     = 8      /** MB level threshold */
} t_vc1_sbp;


/**  \brief Inverse transform types */
typedef enum
{
    TRANSFORM_8X8 =0,   /**< \brief inverse transform on a 8x8 samples block   */
    TRANSFORM_8X4 =1,   /**< \brief inverse transform on a 2 8x4 samples blocks   */
    TRANSFORM_4X8 =2,   /**< \brief inverse transform on a 2 4x8 samples blocks   */
    TRANSFORM_4X4 =3,   /**< \brief inverse transform on a 4 4x4 samples blocks   */
    TRANSFORM_ANY =-1   /**< \brief used to flag that the transform time is not known   */
} t_vc1_frame_transform_type;

/** \brief macroblock quantizer step size enumeration */
typedef enum
{
    QUANT_MODE_DEFAULT,           /**< \brief All macroblocks use PQUANT */
    QUANT_MODE_ALL_EDGES,          /**< \brief Edge macroblocks use ALTPQUANT */
    QUANT_MODE_LEFT_TOP,           /**< \brief Left/Top     macroblocks use ALTPQUANT */
    QUANT_MODE_TOP_RIGHT,          /**< \brief Top/Right    macroblocks use ALTPQUANT */
    QUANT_MODE_RIGHT_BOTTOM,       /**< \brief Right/Bottom macroblocks use ALTPQUANT */
    QUANT_MODE_BOTTOM_LEFT,        /**< \brief Bottom/Left  macroblocks use ALTPQUANT */
    QUANT_MODE_LEFT,              /**< \brief Left         macroblocks use ALTPQUANT */
    QUANT_MODE_TOP,               /**< \brief Top          macroblocks use ALTPQUANT */
    QUANT_MODE_RIGHT,             /**< \brief Right        macroblocks use ALTPQUANT */
    QUANT_MODE_BOTTOM,            /**< \brief Bottom       macroblocks use ALTPQUANT */
    QUANT_MODE_MB_DUAL,            /**< \brief PQUANT/ALTPQUANT selected on macroblock basis */
    QUANT_MODE_MB_ANY              /**< \brief Any QUANT selected on a macroblock basis */
} t_vc1_quant_mode;

/** \brief overlap smooth transform kinds */
typedef enum
{
    OVERLAP_TRANS_NONE = 0,       /**< \brief No macroblocks overlap smooth */
    OVERLAP_TRANS_ALL  = 1       /**< \brief All macroblocks overlap smooth */
//    OVERLAP_TRANS_SOME           /* Selected macroblocks overlap smooth */
} t_vc1_overlap_transform;

/** \brief block index in the blocks array   */
typedef enum
{
    BLOCK_Y0 =0,
    BLOCK_Y1 =1,
    BLOCK_Y2 =2,
    BLOCK_Y3 =3,
    BLOCK_CB =4,
    BLOCK_CR =5
} t_vc1_block_id;

/** \brief interpolation mode derived from the mv mode */
typedef enum
{
    INTERP_H_PEL_BILINEAR,
    INTERP_Q_PEL_BILINEAR,
    INTERP_H_PEL_BICUBIC,
    INTERP_Q_PEL_BICUBIC
} t_vc1_interp_mode;


#define SUB_BLOCK_0_NZ_COEFF_MASK (1<<3)
#define SUB_BLOCK_1_NZ_COEFF_MASK (1<<2)
#define SUB_BLOCK_2_NZ_COEFF_MASK (1<<1)
#define SUB_BLOCK_3_NZ_COEFF_MASK (1)

#define SUB_COL_LEFT_NZ_MASK (SUB_BLOCK_0_NZ_COEFF_MASK | SUB_BLOCK_2_NZ_COEFF_MASK)
#define SUB_COL_RIGHT_NZ_MASK (SUB_BLOCK_1_NZ_COEFF_MASK | SUB_BLOCK_3_NZ_COEFF_MASK)

#define SUB_ROW_TOP_NZ_MASK (SUB_BLOCK_0_NZ_COEFF_MASK | SUB_BLOCK_1_NZ_COEFF_MASK)
#define SUB_ROW_BOTTOM_NZ_MASK (SUB_BLOCK_2_NZ_COEFF_MASK | SUB_BLOCK_3_NZ_COEFF_MASK)

#define BLOCK_NZ_COEFF_MASK (SUB_BLOCK_0_NZ_COEFF_MASK |SUB_BLOCK_1_NZ_COEFF_MASK |SUB_BLOCK_2_NZ_COEFF_MASK| SUB_BLOCK_3_NZ_COEFF_MASK)


/* /\** \brief index in the sub-block non_zero_coeff   *\/ */
/* typedef enum */
/* { */
/*     SUB_BLOCK_0 =0, */
/*     SUB_BLOCK_1 =1, */
/*     SUB_BLOCK_2 =2, */
/*     SUB_BLOCK_3 =3 */

/* } t_vc1_sub_block_id; */

/* /\** \brief index in the sub-columns non_zero_coeff   *\/ */
/* typedef enum */
/* { */
/*     SUB_COL_LEFT =0, */
/*     SUB_COL_RIGHT =1 */
/* } t_vc1_sub_col_id; */

/* /\** \brief index in the sub-rows non_zero_coeff   *\/ */
/* typedef enum */
/* { */
/*     SUB_ROW_TOP =0, */
/*     SUB_ROW_BOTTOM =1 */
/* } t_vc1_sub_row_id; */


/** \brief Block Types */
typedef enum
{
    /* Inter blocks */
    BLK_INTER_8X8 = 0,        /**< \brief Inter coded block, 8px wide, 8px high subblocks */
    BLK_INTER_8X4 = 1,        /**< \brief Inter coded block, 8px wide, 4px high subblocks */
    BLK_INTER_4X8 = 2,        /**< \brief Inter coded block, 4px wide, 8px high subblocks */
    BLK_INTER_4X4 = 3,        /**< \brief Inter coded block, 4px wide, 4px high subblocks */
    BLK_INTER_ANY = -1,       /**< \brief Inter coded block, transform not yet chosen */
    /* Intra blocks */
    BLK_INTRA = -0x8000              /**< \brief Intra coded block, no AC prediction */
} t_vc1_block_type;


/** \brief intra prediction direction    */
/** \warning Do NOT edit those values (used as lut for inverse transform selection    */
typedef enum
{
    INTRA_PRED_TOP=8,     /**< \brief Intra coded block, AC prediction of TOP values */
    INTRA_PRED_LEFT=9,    /**< \brief Intra coded block, AC prediction of LEFT values */
    INTRA_PRED_NONE=10     /**< \brief No intra prediction */
} t_vc1_intra_pred_dir;

/** \brief Motion vector prediction direction */
typedef enum
{
    FORWARD_PRED = 0,            /**< \brief Forward MV prediction */
    BACKWARD_PRED =1            /**< \brief backward MV prediction */
} t_vc1_inter_pred_dir;

/** \brief MB types   */
typedef enum
{
    MB_INTRA            = 0,    /**< \brief  Intra (no motion vectors) */
    MB_1MV_FORWARD      = 1,    /**< \brief   One motion vector forward (P/B pictures)*/
    MB_1MV_BACKWARD     = 2,    /**< \brief   One motion vector backward (B pictures)*/ 
    MB_4MV              = 3,     /**< \brief  Four motion vectors (P pictures only) */
    MB_DIRECT           = 4,     /**< \brief Direct mode for B picture   */
    MB_INTERPOLATE      = 5     /**< \brief interpolated mode for B picture   */
} t_vc1_mb_type;



/*------------------------------------------------------------------------
 * Types (Structs)							       
 *----------------------------------------------------------------------*/
/** \brief Motion vector coordinates */
typedef struct
{
    t_sint16 x; /**< \brief x coordinates in qpel units   */
    t_sint16 y; /**< \brief y coordinates in qpel units   */
}  ts_motion_vector, *tps_motion_vector;


/* Inter deblocking decision flags struct */
typedef struct
{
    /* Luma flags */
    t_bool *p_y_l0;
    t_bool *p_y_l1;
    t_bool *p_y_l2;
    t_bool *p_y_l3;
    t_bool *p_y_c0;
    t_bool *p_y_c1;
    t_bool *p_y_c2;
    t_bool *p_y_c3;

    /* Cb/Cr flags */
    t_bool *p_cb_l0;
    t_bool *p_cb_l1;
    t_bool *p_cb_c0;
    t_bool *p_cb_c1;

    t_bool *p_cr_l0;
    t_bool *p_cr_l1;
    t_bool *p_cr_c0;
    t_bool *p_cr_c1;
    
} ts_inter_dbl_flags, *tps_inter_dbl_flags;


/**  \brief struct holding relevant picture dimensions   */
typedef struct
{
    t_uint16 pix_width; /**< \brief picture width in pixels (not MB aligned)   */
    t_uint16 pix_height;/**< \brief picture height in pixels (not MB aligned)   */
    t_uint16 mb_width; /**< \brief width in mbs   */
    t_uint16 mb_height;/**< \brief height in mbs   */
    t_sint32 max_x_qpel; /**< \brief maximum x coordinate value for mv prediction   */
    t_sint32 max_y_qpel; /**< \brief maximum y coordinate value for mv prediction   */
} ts_dimension, *tps_dimension;


/**  \brief struct holding reference picture parameters and content   */
typedef struct
{
    //   t_vc1_picture_type picture_type;    /**< \brief type of the reference picture   */
//    t_vc1_picture_resolution picture_res; /**< \brief resolution of the reference picture   */
    t_bool intensity_compensate_enabled;        /**< \brief flag stating if the intensity compensation mode is enabled   */
    t_bool range_reduction_frame_enabled;       /**< \brief flag stating if the reference picture was encoded with range_reduction   */
    t_uint8 luma_scale;
    t_uint8 luma_shift;
    t_uint8 ic_lut_y[256];      /**< \brief used to store lut value for ic    */
    t_uint8 ic_lut_uv[256];
    ts_picture_buffer pict_buff;        /**< \brief buffer holding reference pciture data   */
} ts_reference_picture, *tps_reference_picture;


/**  \brief picture quantizer parameters */
typedef struct
{
    t_uint8 pq_index;                   /**< \brief pq index value read in the bitstream   */
    t_uint8  pquant;                    /**< \brief Quantizer Step in the range 1 to 31 */
    t_uint8 altpquant;                  /**< \brief alternative pquant for picture   */    
    t_uint8  half_step;                 /**< \brief Quantizer Half Step value 0 or 1 */
    t_bool   is_uniform;                /**< \brief Selects Uniform/NonUniform Quantizer */
    t_bool   dquant_frame_enabled;      /**< \brief DQUANTFRM picture field   */
    t_vc1_quant_mode quant_mode;        /**< \brief quantification mode   */    
} ts_picture_quant, *tps_picture_quant;

/** \brief mb quantizer evaluated from the picture quantizer
 *         and mb position    
 *  \note The MB quantizer is initialized by iq_init_i_mb_quantizer()
 *        or iq_init_pb_mb_quantizer()
*/
typedef struct
{
    t_uint8 quant;      /**< \brief MB quantifier quant or altquan according to MB position and quantification mode*/
    t_bool is_uniform;  /**< \brief flag stating is the quantification is uniform   */
    t_sint16 double_quant;      /**< \brief double quant value   */

} ts_mb_quant, *tps_mb_quant;



/** \brief Coordinates struct */
typedef struct
{
    t_sint16 x; /**< \brief x coordinates in qpel units   */
    t_sint16 y; /**< \brief y coordinates in qpel units   */
}  ts_coordinates, *tps_coordinates;



/** \brief structure holding FLC espace mode 3 parameters */
typedef struct
{
    t_bool is_first_escape_mode3; /**< \brief flag stating that escape mode 3 has been encountered for the current frame   */
    t_uint16 run_code_size;     /**< \brief fixed run code size   */
    t_uint16 level_code_size;   /**< \brief fixed level code size   */
} ts_escape_mode3;


/** \brief intra block specific parameters */
typedef struct
{
    t_sint16 dc;        /**< \brief dc value for future intra prediction   */
    t_sint16 ac_top[7]; /**< \brief top line of ac values (not iqued) for intra pred   */
    t_sint16 ac_left[7];/**< \brief left column of ac values (not iqued) for intra pred   */
    t_vc1_intra_pred_dir pred_dir; /**< \brief prediction direction (used for inverse transform   */

} ts_intra_blk, *tps_intra_blk;

/** \brief inter block specific parameters */
typedef struct
{
    ts_motion_vector mv[2];  /**< \brief motion vector coordinates (FORWARD/BACKWARD)  */
    t_nz_coeff_mask has_nz_coeff_mask;  /**< \brief mask stating which sub part of a 8x8 block has non null coeff   */
} ts_inter_blk, *tps_inter_blk;




/*------------------------------------------------------------------------*/
/* BLOCK  */
/*------------------------------------------------------------------------*/
/** \brief block parameters struct */
typedef struct
{
/** common intra/inter fields */

    /** \brief padding to align data on 32bits word
     *  \warning keep here to ensure that data[64] is aligned on 32 word address */ 
    t_uint32 padding;

    t_sint16 data[64]; /**< \brief    holding current block values from Iqued, to Itrans to smoothed */
    t_uint16 max_xy;/**< \brief max position of x and y in the data sample (from 0 to 7) values 8,9 10 are reserved for intra pred   */
    t_bool is_coded;            /**< \brief flag stating if the block is coded   */
    t_bool is_tobe_overlapped;  /**< \brief flag stating if the block must be overlap smoothed   */
    t_bool is_overlapped;  /**< \brief flag stating that the block has been  overlap smoothed (+128 offset management)  */
    t_vc1_block_type block_type; /**< \brief block type   */

    t_sint16 dc_step_size;      /**< \brief dc step size: placed here to ease processing   */

/** brief intra/inter specific values union   */
    union 
    {
        ts_intra_blk intra;
        ts_inter_blk inter;
    } spec;

} ts_blk, *tps_blk;


/*------------------------------------------------------------------------*/
/* MACRO BLOCK */
/*------------------------------------------------------------------------*/
/** \brief structure holding macro-block relative data    */
typedef struct macro_blk
{
    t_uint16 x; /**< \brief MB x coordinates   */
    t_uint16 y; /**< \brief MB y coordinates   */
    t_uint16 index; /**< \brief mb index in the frame (use for mv history)   */

    struct macro_blk *p_left;                   /**< \brief MB left neighbour   */
    struct macro_blk *p_top;                    /**< \brief MB top neighbour   */
    struct macro_blk *p_top_left;               /**< \brief MB top left neighbour   */
    struct macro_blk *p_top_b;                  /**< \brief for MV prediction: either top-right or top-left   */

/** @{ \name  Parameters parsed from the bitstream:*/
    t_vc1_mb_type mb_type;              /**< \brief type of the MB   */
    t_bool ac_prediction_enabled;       /**< \brief flag stating if ac prediction is enable for the MB   */
    t_bool is_skipped;                  /**< \brief flag stating if the MB is skipped   */
/** @}*/    

    ts_mb_quant quantizer;              /**< \brief Macro block quantizer   */

    /* Blocks data for intra prediction history*/
    ts_blk blocks[6];

} ts_macro_blk,*tps_macro_blk;

/*------------------------------------------------------------------------*/
/* PICTURE */
/*------------------------------------------------------------------------*/
/** \brief Picture parameters */
typedef struct
{
    t_vc1_picture_type picture_type;    /**< \brief type of the picture   */
    t_vc1_mv_range mv_range;            /**< \brief motion vectors range   */
    t_vc1_mv_mode  mv_mode;             /**< \brief motion vectors mode   */
    t_vc1_picture_resolution picture_res;       /**< \brief picure resolution   */
    t_vc1_frame_transform_type frame_transform_type; /**< \brief TTFRM   */
    t_bool overlap_transform_enabled;   /**< \brief set a frame level if enabled a sequence level and VC-1 8.5.1 cdts met  */

    t_bool frame_interpolation_hint_enabled;  /* Not used for decoding */
    t_bool range_reduction_frame_enabled;
    t_bool intensity_compensate_enabled;
    t_bool mb_level_transform_type_disabled;  /**< \brief TTMBF  see 7.1.1.40 */

    t_uint8 luma_scale;
    t_uint8 luma_shift;

    ts_picture_quant quantizer; /**< \brief picture quantizer   */

    t_uint16 b_fraction_numerator;       /**< \brief derived from BFRACTION see VC-1 7.1.1.14  */
    t_uint16 b_fraction_denominator;     /**< \brief derived from BFRACTION see VC-1 7.1.1.14  */
    t_uint16 b_fraction_scale_factor;   /**< \brief derived from BFRACTION see VC-1 7.1.1.14  */
    t_uint8 buffer_fullness;            /**< \brief BF field see VC-1 7.1.1.5   */

    ts_bitplane         motion_vector_type_bp;   
    ts_bitplane         direct_mb_bp;
    ts_bitplane         skip_mb_bp;

    t_vc1_table_id    motion_vector_table_id;
    t_vc1_table_id    coded_block_pattern_table_id;
    
    ts_vlc_entry      *motion_vector_vlc_table;
    t_uint16          *motion_vector_lut_vlc_table;
    ts_vlc_entry      *coded_block_pattern_vlc_table;
    t_uint16          *coded_block_pattern_lut_vlc_table;


    t_vc1_table_id frame_level_transform_ac_table_id1;
    t_vc1_table_id frame_level_transform_ac_table_id2;
    t_vc1_table_id intra_transform_dc_table_id;

    /* Values derived from the parsing */
    ts_dimension dimension;          /**< \brief picture dimension   */
    t_vc1_table_id zz_table_set_id;  /**< \brief id of the zz table set to use   */
    ts_vlc_entry *ttmb_vlc_table;
    ts_vlc_entry *ttblk_vlc_table;
    ts_vlc_entry *sub_block_pattern_4x4_vlc_table;
//    ts_vlc_entry *escape_mode3_vlc_table;
    t_vc1_interp_mode interpolation_mode; /**< \brief interpolation mode derived from the mv_mode   */
    t_sint16 rnd_ctrl; /**< \brief round control value (VC-1 8.3.7)   */

} ts_picture_parameters, *tps_picture_parameters;






/*------------------------------------------------------------------------
 * Macros							       
 *----------------------------------------------------------------------*/
/** macro testing if a block is a luma one */
#define IS_LUMA_BLOCK(blk_id) (blk_id)<=BLOCK_Y3?TRUE:FALSE

/** macro testing if a block is a chroma one */
#define IS_CHROMA_BLOCK(blk_id) (blk_id)>BLOCK_Y3?TRUE:FALSE

/** macro testing if a sub block/column/row has non zero coefficients */
#define SUB_BLOCK_0_HAS_NZ_COEFF(mask) ((mask)&SUB_BLOCK_0_NZ_COEFF_MASK)
#define SUB_BLOCK_1_HAS_NZ_COEFF(mask) ((mask)&SUB_BLOCK_1_NZ_COEFF_MASK)
#define SUB_BLOCK_2_HAS_NZ_COEFF(mask) ((mask)&SUB_BLOCK_2_NZ_COEFF_MASK)
#define SUB_BLOCK_3_HAS_NZ_COEFF(mask) ((mask)&SUB_BLOCK_3_NZ_COEFF_MASK)


#define SUB_COL_LEFT_HAS_NZ_COEFF(mask) ((mask)&SUB_COL_LEFT_NZ_MASK)
#define SUB_COL_RIGHT_HAS_NZ_COEFF(mask) ((mask)&SUB_COL_RIGHT_NZ_MASK)

#define SUB_ROW_TOP_HAS_NZ_COEFF(mask) ((mask)&SUB_ROW_TOP_NZ_MASK)
#define SUB_ROW_BOTTOM_HAS_NZ_COEFF(mask) ((mask)&SUB_ROW_BOTTOM_NZ_MASK)

#define BLOCK_HAS_NZ_COEFF(mask) ((mask)&BLOCK_NZ_COEFF_MASK)

/* #define GET_LAST_X_64(value) (HI_BYTE((value))) */
/* #define GET_LAST_Y_64(value) (LO_BYTE((value))) */
#define SET_LAST_X_64(value,x) (value) &= 0x38; (value) |= (x)
#define SET_LAST_Y_64(value,x) (value) &= 0x07; (value) |= (x<<3)
#define SET_LAST_XY_64(value,x,y) (value) = ((y)<<3)|(x)
#define GET_LAST_X_64(value) ((value) & 0x7) 
#define GET_LAST_Y_64(value) ((value)>>3)

/* #define SET_LAST_XY_64(value,x,y) (value) = MERGE_BYTE((x),(y)) */
 

#endif /* _VC1D_SVA_H_ */
