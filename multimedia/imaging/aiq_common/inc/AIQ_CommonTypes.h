/*****************************************************************************
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*****************************************************************************/

#ifndef _AIQ_COMMON_H_
#define _AIQ_COMMON_H_

#ifdef _MSC_VER
	#define STIN /*static*/ __inline
#else
	#define STIN /*static*/ inline
#endif

/** AIQ_U8 is an 8 bit unsigned quantity that is byte aligned */
typedef unsigned char  AIQ_U8;

/** AIQ_S8 is an 8 bit signed quantity that is byte aligned */
typedef signed char    AIQ_S8;

/** AIQ_U16 is a 16 bit unsigned quantity that is 16 bit word aligned */
typedef unsigned short AIQ_U16;

/** AIQ_S16 is a 16 bit signed quantity that is 16 bit word aligned */
typedef signed short   AIQ_S16;

/** AIQ_U32 is a 32 bit unsigned quantity that is 32 bit word aligned */
typedef unsigned long  AIQ_U32;

/** AIQ_S32 is a 32 bit signed quantity that is 32 bit word aligned */
typedef signed long    AIQ_S32;

/** AIQ_Float is a 32 bit (single precision) floating point value */
typedef float          AIQ_Float;

/** AIQ_Double is a 64 bit (double precision) floating point value */
typedef double         AIQ_Double;

/** Boolean type */
typedef enum AIQ_BOOL {
    AIQ_FALSE = 0,
    AIQ_TRUE = !AIQ_FALSE,
    AIQ_BOOL_MAX = 0x7FFFFFFF
} AIQ_Bool; 
 
/** Image plane */
typedef struct {
    AIQ_S32       s32_Stride;          /**< Number of bytes to add to go from one pixel line to the next line */
    AIQ_U32       u32_Offset ;         /**< Offset of the topleft image defined in bytes */
    AIQ_U8       *pu8_BaseAddress;     /**< Pixel array */
} AIQ_Plane;

typedef struct {
   AIQ_Plane *p_A;     /**< A can be Y or R component*/
   AIQ_Plane *p_B;     /**< B can be U or G component*/
   AIQ_Plane *p_C;     /**< C can be V or B component */
} AIQ_Planar;

typedef struct {
   AIQ_Plane *p_A;     /**< A can be Y or R component*/
   AIQ_Plane *p_BC;    /**< BC can be UV or GB component*/
} AIQ_SemiPlanar;

typedef struct {
   AIQ_Plane *p_ABC;   /**< ABC can be YUV or RGB component*/
} AIQ_Coplanar;

typedef union {
   AIQ_Planar          s_Planar;
   AIQ_SemiPlanar      s_SemiPlanar;
   AIQ_Coplanar        s_Coplanar;
} AIQ_Image;

typedef enum {
    AIQ_COLOR_FormatUnused,
    AIQ_COLOR_FormatMonochrome,
    AIQ_COLOR_Format8bitRGB332,
    AIQ_COLOR_Format12bitRGB444,
    AIQ_COLOR_Format16bitARGB4444,
    AIQ_COLOR_Format16bitARGB1555,
    AIQ_COLOR_Format16bitRGB565,
    AIQ_COLOR_Format16bitBGR565,
    AIQ_COLOR_Format18bitRGB666,
    AIQ_COLOR_Format18bitARGB1665,
    AIQ_COLOR_Format19bitARGB1666, 
    AIQ_COLOR_Format24bitRGB888,
    AIQ_COLOR_Format24bitBGR888,
    AIQ_COLOR_Format24bitARGB1887,
    AIQ_COLOR_Format25bitARGB1888,
    AIQ_COLOR_Format32bitBGRA8888,
    AIQ_COLOR_Format32bitARGB8888,
    AIQ_COLOR_FormatYUV411Planar,
    AIQ_COLOR_FormatYUV411PackedPlanar,
    AIQ_COLOR_FormatYUV420Planar,
    AIQ_COLOR_FormatYUV420PackedPlanar,
    AIQ_COLOR_FormatYUV420SemiPlanar,
    AIQ_COLOR_FormatYUV422Planar,
    AIQ_COLOR_FormatYUV422PackedPlanar,
    AIQ_COLOR_FormatYUV422SemiPlanar,
    AIQ_COLOR_FormatYCbYCr,
    AIQ_COLOR_FormatYCrYCb,
    AIQ_COLOR_FormatCbYCrY,
    AIQ_COLOR_FormatCrYCbY,
    AIQ_COLOR_FormatYUV444Interleaved,
    AIQ_COLOR_FormatRawBayer8bit,
    AIQ_COLOR_FormatRawBayer10bit,
    AIQ_COLOR_FormatRawBayer8bitcompressed,
    AIQ_COLOR_FormatL2, 
    AIQ_COLOR_FormatL4, 
    AIQ_COLOR_FormatL8, 
    AIQ_COLOR_FormatL16, 
    AIQ_COLOR_FormatL24, 
    AIQ_COLOR_FormatL32,
    AIQ_COLOR_FormatYUV420PackedSemiPlanar,
    AIQ_COLOR_FormatYUV422PackedSemiPlanar,
    AIQ_COLOR_Format18BitBGR666,
    AIQ_COLOR_Format24BitARGB6666,
    AIQ_COLOR_Format24BitABGR6666,
    AIQ_COLOR_FormatKhronosExtensions = 0x6F000000, /**< Reserved region for introducing Khronos Standard Extensions */ 
    AIQ_COLOR_FormatVendorStartUnused = 0x7F000000, /**< Reserved region for introducing Vendor Extensions */
    AIQ_COLOR_FormatMax = 0x7FFFFFFF
} AIQ_PixelFormat;

typedef struct {
   AIQ_U32 u32_Width;     /**< Image width */
   AIQ_U32 u32_Height;    /**< Image height */
} AIQ_Dimensions;

typedef struct {
   AIQ_U32 u32_PosX;     /**< Position in X axis */
   AIQ_U32 u32_PosY;     /**< Position in y axis */
} AIQ_Position;

typedef struct {
   AIQ_Position   s_Position;
   AIQ_Dimensions s_Dimensions;
} AIQ_CropParams;



typedef struct {
   AIQ_Image        u_Image;
   AIQ_Dimensions   s_Dimensions;       /**< Dimensions of the frame */  
   AIQ_PixelFormat  u32_PixelFormat;
} AIQ_Frame;

typedef enum
{
   AIQ_NO_ERROR   = 0,
   AIQ_BAD_PARAM,
   AIQ_BAD_STATE,   
   AIQ_OUT_OF_MEM,
   AIQ_UNSUPPORTED_FORMAT,
   AIQ_SYSTEM_ERROR,
   AIQ_ERROR_ERROR_MAX = 0x7FFFFFFF   
} AIQ_Error;

#if (/*defined(__SYMBIAN32__) &&*/ !defined(__cplusplus))
	#define AIQ_NullFunc() (AIQ_Error)NULL
#else
	STIN AIQ_Error AIQ_NullFunc() { return(AIQ_NO_ERROR); }
#endif

#endif //_AIQ_COMMON_H_
