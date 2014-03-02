/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_COMMON_H_
#define _VC1D_COMMON_H_

/*
 * Includes							       
 */
#include "types.h" 
#include <vc1dec/arm_nmf/api/nmftype.idt>

/*------------------------------------------------------------------------
 * Defines                                                             
 *----------------------------------------------------------------------*/
#ifdef USE_ASSERT
#define ASSERT(value) assert(value) /**< \brief use this macro to conditionnaly insert assertions   */
#else
#define ASSERT(value) 
#endif

/** \brief macro to round up division result   */
#define DIV_UP(a,b) ( (a)%(b) == 0 ? ( (a)/(b) ) : ( 1L + (a)/(b) ) )


#define EXTEND_TO_16(value) ( ( (value) + 0xf) & (~0xfU) )
#define EXTEND_TO_8(value) ( ( (value) + 0x7) & (~0x7U) )
#define EXTEND_TO_4(value) ( ( (value) + 0x3) & (~0x3U) )


/* Stringifyer */
#define xstr(s) str(s)
#define str(s) #s

#if !defined(FALSE)
#define FALSE 0
#endif

#if !defined(TRUE)
#define TRUE 1
#endif



/**
 * Description:
 * Bitstream profile enumeration
 */

//typedef enum
//{
//    PROFILE_SIMPLE = 0,  /** Simple profile */
//    PROFILE_MAIN = 1,        /** Main profile */
//    PROFILE_FORBIDEN    /** Reserved */
//} t_vc1_profile;

/**
 * Description:
 * Profile level enumeration
 */

//typedef enum
//{
//    LEVEL_LOW    = 0,    /** Simple/Main profile low level */
//    LEVEL_MEDIUM = 1,    /** Simple/Main profile medium level */
//    LEVEL_HIGH   = 2,    /** Simple/Main profile high level */
//
//    LEVEL_L0     = 0,    /** Advanced profile level 0 */
//    LEVEL_L1     = 1,    /** Advanced profile level 1 */
//    LEVEL_L2     = 2,    /** Advanced profile level 2 */
//    LEVEL_L3     = 3,    /** Advanced profile level 3 */
//    LEVEL_L4     = 4,    /** Advanced profile level 4 */

    /* 5 to 7 reserved */

//    LEVEL_UNKNOWN = 255  /** Unknown profile */
//} t_vc1_level;

typedef enum
{
    VC1_NO_ERROR = 0

} t_vc1_error_code;

/**
 * Description:
 * Quantizer mode enumeration
 */
//typedef enum
//{
//    QUANTIZER_IMPLICIT   = 0,    /** Quantizer implied by quantizer step size */
//    QUANTIZER_EXPLICIT   = 1,    /** Quantizer explicitly signaled */
//    QUANTIZER_NON_UNIFORM = 2,    /** Non-uniform quantizer */
//    QUANTIZER_UNIFORM    = 3     /** Uniform quantizer */
//} t_vc1_quantizer;

typedef enum
{
    MVRANGE_64_32    = 0,    /* x=-64 to 63.f by y=-32 to 31.f */
    MVRANGE_128_64   = 1,
    MVRANGE_512_128  = 2,
    MVRANGE_1024_256 = 3
} t_vc1_mv_range;


/**
 * Description: Scaling to be applied to decoded picture before display
 */
typedef enum
{
    PICTURE_RES_1X1=0,    /** No scaling */
    PICTURE_RES_2X1=1,      /** Scale horizontally */
    PICTURE_RES_1X2=2,      /** SCALE VERTICALLY */
    PICTURE_RES_2X2=3       /** Scale horizontally and vertically */
} t_vc1_picture_resolution;


/**
 * Description:
 * Motion vector mode enumeration
 */

typedef enum
{
    MVMODE_1MV_HALF_PEL_BILINEAR = 0,   /** 1MV     0.50 pel bilinear  */
    MVMODE_1MV_HALF_PEL         = 1,   /** 1MV     0.50 pel bicubic   */
    MVMODE_1MV                = 2,   /** 1MV     0.25 pel bicubic   */
    MVMODE_MIXED_MV            = 3,   /** MixedMV 0.25 pel bicubic   */
    MVMODE_INTENSITY_COMPENSATION     /** VARIABLE LENGTH CODE escape flag */
} t_vc1_mv_mode;


/**
 * Description:
 * Structure for holding a variable length code.
 *
 * Remarks:
 * In an array of these, entry 0 has a special meaning:
 *  - Bits   = index of the last code with size<=16, if 0, max lenght <= 16;
 *  - Length = Number of codes in the array (Length of array - 1)
 *  - Value  = Maximum code length
 */
#define ESCAPE  (0xffe) //escape sequence value in vcl tables

typedef struct
{
    t_uint32     bits;               /**< \brief Bit pattern of the code */
    t_uint16      length;             /**< \brief Length of the code */
//    t_uint32     value;              /**< \brief Value that the code represents */
    t_uint16     value;              /**< \brief Value that the code represents */
} ts_vlc_entry, tps_vlc_entry;



//typedef struct
//{
//    t_vc1_profile       profile;                   /** See standard */
//    t_vc1_level         level;                     /** See standard */
//    t_vc1_quantizer     quantizer;                 /** See standard */
//    t_uint8             dquant;                     /** See standard */
//    t_uint8             max_b_frames;                 /** See standard */
//   t_uint8             q_framerate_for_postproc;      /** See standard */
//    t_uint8             q_bitrate_for_postproc;        /** See standard */

//    t_bool              loop_filter_enabled;                 /** See standard */
//    t_bool              multires_coding_enabled;             /** See standard */
//    t_bool              fast_uvmc_enabled;                   /** See standard */
//    t_bool              extended_mv_enabled;                 /** See standard */
//    t_bool              variable_size_transform_enabled;                /** See standard */
//    t_bool              overlap_transform_enabled;    /** See standard */
//    t_bool              syncmarker_enabled;             /** See standard */
//    t_bool              rangered_enabled;               /** See standard */
//    t_bool              frame_interpolation_enabled;     /** See standard */
//} ts_sequence_parameters, *tps_sequence_parameters;


/**
 * Description:
 * Picture type enumeration
 */
typedef enum
{
    PICTURE_TYPE_I        = 0,    /** I Picture / Field - can be used as a reference */
    PICTURE_TYPE_P        = 1,    /** P Picture / Field - can be used as a reference */
    PICTURE_TYPE_B        = 2,    /** B Picture / Field */
    PICTURE_TYPE_BI       = 3,    /** BI Picture / Field */
    PICTURE_TYPE_SKIPPED  = 4     /** Skipped Frame */
} t_vc1_picture_type;

typedef enum
{
    TABLE_0 =0,
    TABLE_1 =1,
    TABLE_2 =2,
    TABLE_3 =3,
    TABLE_UNKNOWN =-1
} t_vc1_table_id;



/**
 * Description:
 * Structure containing configuration information for the decoder. This structure
 * contains information available to the application from demultiplexing the container format.
 * Typically in our case, those informations are read from the rcv headers (file header)
 *
 */


//typedef struct
//{
//    t_uint16    max_picture_width;  /**  picture width */
//    t_uint16    max_picture_height; /**  picture height */
//    t_uint32    frame_rate; 
//    t_uint32    number_of_frames;        
//    t_uint32    bit_rate;
//} ts_decoder_configuration, *tps_decoder_configuration;


/** \brief struct holding the reconstructed frame in yuv 420 pixel raster   */
typedef struct 
{
    t_uint8 *y;
    t_uint32 y_size; /**< \brief allocated size */
    t_uint8 *cb;
    t_uint32 cb_size; /**< \brief allocated size */
    t_uint8 *cr;
    t_uint32 cr_size; /**< \brief allocated size */
} ts_picture_buffer, *tps_picture_buffer;


/**
 * Description:
 * B Fraction numerator and denominator structure.
 */

typedef struct
{
    t_uint8  numerator;      /** BFraction numerator */
    t_uint8  denominator;    /** BFraction denominator */
    t_uint8  scale_factor;    /** Approximated Numerator*256/Denominator */
} ts_b_fraction;

typedef struct
{
    t_uint16 y_width; /**< \brief indeed the picture width   */
    t_uint16 y_height;
    t_uint16 padded_y_width; /**< \brief picture width extended to the next MB boundary   */
    t_uint16 padded_y_height;

    t_uint16 cbcr_width; /**< \brief indeed the picture width/2   */
    t_uint16 cbcr_height;
    t_uint16 padded_cbcr_width; /**< \brief picture width extended to the next MB boundary for chroma samples   */
    t_uint16 padded_cbcr_height;

} ts_rec_param,*tps_rec_param;

/*------------------------------------------------------------------------
 * Variables                                                             
 *----------------------------------------------------------------------*/
extern t_bool Dbg;

#endif /* _VC1D_COMMON_H_ */
