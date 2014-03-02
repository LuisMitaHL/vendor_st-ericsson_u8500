/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HAMAC_TYPES_H
#define HAMAC_TYPES_H

#include "types.h"
#include "settings.h"

#ifdef __ndk5500_a0__
#define Endianess(a)	(t_uint32)(a)
#else
#define Endianess(a)	((((t_uint32)(a) & 0xffff )<<16) + (((t_uint32)(a)>> 16) & 0xffff ))
#endif

/**
 * \brief Structure used as interface from host to hamac for deblocking filter.
 */

typedef struct
{
    /* Subset of sequence parameter set */
    t_uint16 pic_width_in_mbs_minus1;
    t_uint16 pic_height_in_map_units_minus1;
    
    /* Address of buffer for blocks info */
    t_address b_info;

    /* Address of slice headers array */
    t_address sh;

    /* Addresses of current frame buffer components */
    t_address curr_frame[3];

    /* Address of auxiliary frame buffer */
    t_address aux_frame;
    
    /* Address of buffer for deblocking filter parameters */
    t_address p_deblocking_paramv;

     
} t_hamac_deblocking_info;



/**
 * \brief Structure used as interface from host to hamac for slice decoding.
 */

typedef struct
{
    /* Subset of sequence parameter set */
    t_uint16 pic_width_in_mbs_minus1;
    t_uint16 pic_height_in_map_units_minus1;
    t_uint16 PicSizeInMbs;
    
    /* Subset of picture parameter set */
    t_sint16 chroma_qp_index_offset;
    t_uint16 constr_intra_pred_flag;

    /* Subset of slice header parameter */
    t_uint16 first_mb_in_slice;
    t_uint16 slice_qp;         
    t_uint16 slice_type;
    t_uint16 slice_num;

    t_uint16 num_ref_idx_l0_active_minus1;
    
    /* Address of buffer containing MB to slice map */
    t_address mb_slice_map;
    
    /* Addresses of current frame buffer components */
    t_address curr_frame[3];
    
    /* Address of blocks info buffer */
    t_address b_info;
    
    /* Array of addresses for frame buffers components. Used for INTER decoding */
    t_address list0[MAXNUMFRM][3];  

    /* Structs for bitstream handling */
    t_interface_buffer bit_buf;
    t_interface_buffer_pos bit_buf_pos;
    
    /* Counters for decoded macroblock */
    t_uint16 mb_count;
    t_uint16 mb_intra;
        
} t_hamac_slice_info;



/**
 * \brief Structure used as interface from host to hamac for error concealment.
 */

typedef struct
{
    /* Subset of sequence parameter set */
    t_uint16 pic_width_in_mbs_minus1;
    t_uint16 pic_height_in_map_units_minus1;
    t_uint16 PicSizeInMbs;
    
    /* Slices type */
    t_uint16 slice_type;
        
    /* Address of buffer for blocks info */
    t_address b_info;

    /* Addresses of current frame buffer components */
    t_address curr_frame[3];

    /* Addresses of reference frame buffer components */
    t_address ref_frame[3];
    
    /* Intra concealment flag */
    t_uint16 intra_conc;
    
} t_hamac_conc_info;



/*** Structures use to interface hamac layers ***/


/**
 * \brief Structure used by hamac to hold macroblock layer parameters.
 */

typedef struct
{
    /* Subset of sequence parameter set */
    t_uint16 pic_width_in_mbs_minus1;
    t_uint16 pic_height_in_map_units_minus1;
    
    /* Subset of picture parameter set */
    t_sint16 chroma_qp_index_offset;
    t_uint16 constr_intra_pred_flag;

    /* Subset of slice header parameter */
    t_uint16 slice_qp;                
    t_uint16 slice_type;
    
    /* Addresses of current frame buffer components */
    t_address *curr_frame;
    
    /* Pointer to buffer for blocks info */
    t_block_info *b_info;
    
    /* Array of addresses for frame buffers components. Used for INTER decoding */
    t_address list0[MAXNUMFRM][3];  

    t_uint16 num_ref_idx_l0_active_minus1;
        
    /* Macroblock layer */    
    t_uint16 mb_type;
    t_uint16 sub_mb_type[4];
    t_uint16 coded_block_pattern;
    t_uint16 coded_block_pattern_chroma;
    t_uint16 coded_block_pattern_luma;
    t_sint16 mb_qp_delta;
    
    /* Array for I_PCM macroblock */
    t_uint16 pcm_byte[384];  
    
    /* INTRA prediction */
    t_uint16 prev_intra4x4_pred_mode_flag[16];
    t_uint16 rem_intra4x4_pred_mode_flag[16];
    t_uint16 intra_chroma_pred_mode;   
    
    /* INTER prediction */
    t_uint16 ref_idx_l0[4];  /* Only list 0. B prediction not supported in baseline profile */
    t_sint16 mvd_l0[4][4][2]; /* [partition index][sub partition index][component] */ 
    
    /* Residual data */
    t_sint16 intraDC[16];
    t_sint16 luma_level[16][16]; /* Used both for AC levels and Luma levels */
    t_sint16 chromaDC[2][4]; 
    t_sint16 chromaAC[2][4][16];
    
    t_uint16 curr_mb_addr;
    t_uint16 mbx;
    t_uint16 mby;
    
} t_hamac_mb_info;




/**
 * \brief Structure used as interface from residual (CAVLC) decoding.
 */

typedef struct 
{
    /* Subset of sequence parameter set */
    t_uint16 pic_width_in_mbs_minus1;
    
    /* Pointer to buffer for blocks info */
    t_block_info *b_info;
    
    t_uint16 mbx;
    t_uint16 mby;   
    
    t_uint16 curr_block;
    
} t_hamac_residual_info;


#endif
