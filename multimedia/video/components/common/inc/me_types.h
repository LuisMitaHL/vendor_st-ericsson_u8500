/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _T1XHV_ME_TYPES_H_
#define _T1XHV_ME_TYPES_H_

#include "t1xhv_types.h"

/**
 * \brief Sub-macroblock partitions
 */
typedef enum 
{   MB16x16=0           ,
    MB16x8              ,
    MB8x16              ,
    MB8x8
}   t_mb_partition_type;

/**
 * \brief Sub-block partitions
 */
typedef enum 
{
    SUBMB8x8=0          ,
    SUBMB8x4            ,
    SUBMB4x8            ,
    SUBMB4x4
}   t_sub_mb_partition_type;

/**
 * \brief Motion Vector structure
 */
typedef struct {
    t_sint16                 MV[16][2];                /**< Motion vectors within one macroblock */
    t_uint16                 SAD[16];                  /**< SAD values for the partitions within the MB */
    t_uint16                 mb_partition_type;          /**< Type of the macroblock partition */
    t_uint16                 sub_mb_partition_type[4];   /**< sub-block partitions within the four 8x8 blocks */
    t_uint16                 reserved[3]                ;   /**< To ensure correct alignment for MTF operations */
} mv_desc;


/**
 * \brief Spatial-Temporal predictor Motion Vector structure
 */
typedef struct {
    t_sint16  MV[2];                /**< Components of the predictor MV */
} mvp_desc;

typedef struct {
    t_sint16  MV[2];
    t_uint16   SAD;
    t_uint16   reserved;
    t_uint32   reserved1;
    t_uint32   reserved2;
} mvp8815_desc;

/**
 * \brief Interface structure to be used by the main API function
 */
typedef struct {
    t_uint8    *current_frame;     /**< luma component of the current frame buffer */
    t_uint8    *ref_frame;         /**< luma component of the reference frame buffer */
    t_sint16   *dif_frame;         /**< luma component of the difference frame buffer */
    t_uint8    *internal_bufs;     /**< 'b', 'j' and 'h' pre-computed half-pel interpolated buffers */
    mv_desc    *motion_field;      /**< motion field (input/output memory area) */
    mvp_desc   *mv_preds;          /**< temporal predictors used internally by the ME */
    t_uint16    image_width;       /**< width of the frame buffer */
    t_uint16    image_height;      /**< height of the frame buffer */
    t_uint16    interrupt_freq;    /**< interrupt frequency (unused in the ref SW) */
    t_uint16	return_flag;       /**<  0 returns bjh buff: 1 returns dif_frame */
} me_input_params;

typedef struct {
    t_sint16 mv[8][2];
    t_uint16 len;
} list;

typedef struct {
    t_uint32                  cur_ptr;
    t_uint32                  dif_ptr;
    t_uint32                  ref_ptr[4];
    t_uint32                  mot_fld;
    t_uint32                  mv_pr;
    t_sint16                  xdim;
    t_sint16                  ydim;
    t_uint16		      return_flag;
} internal_data;


typedef struct {
    t_uint32                  cur_ptr;
    t_uint32                  ref_ptr;
    t_uint32                  mot_fld;
    t_uint32                  mv_pr;
    t_sint16                  xdim;
    t_sint16                  ydim;
    t_uint16                  mb_start_index;
    t_uint16                  mb_number;
    mvp_desc                  pred_mv;
} internal_data_8815;


typedef struct {
    t_uint8    *current_frame;     /**< luma component of the current frame buffer */
    t_uint8    *ref_frame;         /**< luma component of the reference frame buffer */
    mv_desc    *motion_field;      /**< motion field (input/output memory area) */
    t_uint16    image_width;       /**< width of the frame buffer */
    t_uint16    image_height;      /**< height of the frame buffer */
    mvp_desc    pred_mv;
} hamac_me8815_params;


typedef struct {
    mvp_desc   *mv_preds;          /**< temporal predictors used internally by the ME */
    t_uint16    mb_start_index;
    t_uint16    mb_number;
} host_me8815_input;

#endif /* _T1XHV_ME_TYPES_H_ */


