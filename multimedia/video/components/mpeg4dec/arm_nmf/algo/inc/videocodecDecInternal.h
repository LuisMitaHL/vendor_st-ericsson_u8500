/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

#ifndef INCLUSION_GUARD_VIDEOCODECDECINTERNAL_H
#define INCLUSION_GUARD_VIDEOCODECDECINTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

	/*************************************************************************
	* Includes
	*************************************************************************/
#include <setjmp.h>
	//#include "vidMalloc.h"
#include <assert.h>
#include "t_basicdefinitions.h"
#include "stdio.h"
#include "types.h"	
#include "t1xhv_common.idt"	
#include "t1xhv_vdc_mpeg4.idt"

	//#define YUV_420_SEMIPLANAR
	//#define YUV_420_SEMIPLANAR_TEST

	/*************************************************************************
	* Types, constants and external variables
	*************************************************************************/

#ifdef DEBUG_BITSTREAMTRACE
#define BITSTREAMTRACE(X)  X
#else
#define BITSTREAMTRACE(X)
#endif

#ifdef DEBUG_PRINT
#define PRINT(X)  X
#else
#define PRINT(X)
#endif


#define UNUSED_ARGUMENT(X)  ((void)(X))

#ifndef _ALGO_DEBUG
#define BIT_ERROR(Label, ErrorCode, String)  longjmp(Label, ErrorCode);
#else
#define BIT_ERROR(Label, ErrorCode, String)  printf("\nVideo Decoder: %s\n", String); longjmp(Label, ErrorCode);
#endif

	// IDCT Algorithm
	//#define IDCT_JPG 0   // Jpeg
	//#define IDCT_INT 1   // Integer implementation
	//#define IDCT_REF 2   // Reference double implementation

	// Concealment interpolation applied on INTRA picture
	//#define NO_INTERPOLATION          0x0
	//#define HORIZONTAL_INTERPOLATION  0x1
	//#define VERTICAL_INTERPOLATION    0x2

#define VIDEODEC_NOERROR            0
#define VIDEODEC_END_OF_BITSTREAM   201

#define VIDEODEC_DEFAULT_INPUTTYPE   VIDEODEC_NULL

#define MSIGN(a)              ((a) < 0 ? -1 : 1)
#define ABS(a)              ((a) < 0 ? -a : a)

#define MMAX(a,b)  (((a)>(b)) ? (a) : (b))

	// Clipping to an interval
#define MCLIP(var, min, max)  ((var) < (min) ? (min) : ((var) > (max) ? (max) : (var)))

	// Clipping for positive values
#define PCLIP(var,max)        ((var)>(max) ? (max) : (var))

#define MABS(X)  (((X)>=0) ? (X) : -(X))

#define TCLIP(var, min, max)    tclip[(var)]

#define Div2Round(X)     (((X) >> 1) | ((X) & 1))

#define MAX_Y_WIDTH               MAX_UINT32       
#define MAX_Y_HEIGHT              MAX_UINT32       

#define MIN_Y_WIDTH                16 
#define MIN_Y_HEIGHT               16

#define MB_HEIGHT                 MAX_UINT32
#define MB_WIDTH                  MAX_UINT32

#define MB_SIZE 16

#define MAX_NUMBER_OF_MB_PER_ROW   MAX_UINT32  
#define MAX_NUMBER_OF_MB           MAX_UINT32  

#define MAX_WARPING_POINTS_SUPPORTED 1

#define CODE_MB 1
#define SKIP_MB 0
#define CODE_NO_BLOCKS 0
#define B_MB_NOT_CODED 2

#define FORMAT_SUB_QCIF 1
#define FORMAT_QCIF     2
#define FORMAT_CIF      3
#define FORMAT_4CIF     4
#define FORMAT_16CIF    5
#define FORMAT_CUSTOM   6
#define FORMAT_16VGA    7

	// Picture and MB types (According to H.263+ Section 5.1.4.3)
#define INTRA                   0
#define INTER                   1
#define BIDIRECTIONAL           2
#define SPRITE                  3
#define STUFFING                6
#define RESERVED2               7
#define UNDETERMINED           21   // Needed in decoder
#define INTERPOLATED           23   // Needed in decoder for interpolation
#define CORRUPT_MC             24   // for prediction from corrupt/undetermined MBs in previous frame
#define INTRA_BEFORE_MARKER    25   // Needed in decoder for concealment in DP-mode
#define INTER_BEFORE_MARKER    26   // Needed in decoder for concealment in DP-mode
#define INTRA_AFTER_MARKER     27   // Needed in decoder for concealment in DP-mode
#define INTER_AFTER_MARKER     28   // Needed in decoder for concealment in DP-mode
#define INTER_CONCEALMENT      29
#define CORRUPT               128   // Needed in decoder

	// Macroblock position in Picture or in Slice
#define MIDDLE         0
#define LEFT           1
#define UPPER          2
#define RIGHT          4
#define LOWER          8
#define UPPER_LEFT     3
#define UPPER_RIGHT    6
#define LOWER_LEFT     9
#define LOWER_RIGHT   12

	// Masks for the above information
#define MB_LEFT_MASK   1
#define MB_UPPER_MASK  2
#define MB_RIGHT_MASK  4
#define MB_LOWER_MASK  8

	//Prediction Modes MC_FORWARD, MC_BACKWARD, MC_INTERPOLATE, MC_DIRECT from MPEG-4 spec, 
	//the last two MC_BIDIRFORWARD, MC_BIDIRBACKWARD is used for direct and interpolate mode when motioncompensation is performed
#define MC_FORWARD      0
#define MC_BACKWARD     1
#define MC_INTERPOLATE  2
#define MC_DIRECT       3
#define MC_BIDIRBACKWARD  4
#define MC_BIDIRFORWARD   5


#define GET_Y_PTR(img, x, y)  ((img)->Y_p  + (x) + (y) * (img)->Y_LineWidth)
#define GET_CB_PTR(img, x, y) ((img)->Cb_p + (x) + (y) * (img)->C_LineWidth)
#define GET_CR_PTR(img, x, y) ((img)->Cr_p + (x) + (y) * (img)->C_LineWidth)

#define INTERLACE_DCT_TYPE_FRAME  0
#define INTERLACE_DCT_TYPE_FIELD  1

#define INTERLACE_TOP_FIELD     0
#define INTERLACE_BOTTOM_FIELD  1

#define MC_BLOCKSIZE_16x16 0
#define MC_BLOCKSIZE_8x8   1

#define ODD(X)   (((X) & 1) == 1)

#define WORD_ALIGNED(X_p)  ((((uint32)(X_p)) & 3) == 0)

#define EQUAL_MV(MV1,MV2)  (MV1.x == MV2.x && MV1.y == MV2.y)

#define DIV2(X,Y)  ( (X) / (Y) + ( ( (X) % (Y) ) > 0 ? 1 : 0 ) )

	/********************************************************************************
	* Typedefs
	*********************************************************************************/
	/******************** ExceptionCode ***************************/
	typedef enum
	{
		NO_ERROR,
		GSC_FOUND,
		VLC_ERROR,
		SYNTAX_ERROR,
		MARKER_BIT_ERROR,
		SEGMENT_ERROR,
		MCBPQ_ERROR,
		MCBPC_ERROR,
		MVD_ERROR,
		DCT_ERROR,
		DQUANT_ERROR,
		INTRA_DC_ERROR,
		GOB_ERROR,
		SLICE_ERROR,
		PIC_HEADER_ERROR,
		PTYPE_ERROR,
		PLUSPTYPE_ERROR,
		OPPTYPE_ERROR,
		MPPTYPE_ERROR,
		CPM_ERROR,
		CHECK_MV_ERROR,
		ALIGNMENT_ERROR,
		DC_MARKER_ERROR,
		ALLOC_LDATA_ERROR,
		VOP_HEADER_ERROR,
		VOL_HEADER_ERROR,
		TCOEFF_ERROR,
		CPF_ERROR,
		PQUANT_ERROR,
		VO_HEADER_ERROR,
		ADV_PRED_MODE_ERROR,
		STARTCODE_ALIGN_ERROR,
		STARTCODE_FOUND_ERROR,
		MB_TYPE_ERROR,
		MB_NUMBER_ERROR,
		MV_LENGTH_ERROR,
		IMG_SIZE_ERROR,
		BARY_START_PNTR_ERROR,
		ALLOC_ERROR,

		UNKNOWN_ERROR
	} ExceptionCode_t;

	typedef uint8 PEL;
	typedef float Float;

	//typedef void* HVIDEODEC;

	typedef enum
	{
		STANDARD_UNKNOWN,
		H263,
		MPEG4,
		H261
	} Standard_t;

	/*
	typedef enum
	{
	VID_STATUS_OK,
	VID_STATUS_CODEC_SPECIFIC_1,
	VID_STATUS_CODEC_SPECIFIC_2,
	VID_STATUS_OUT_OF_MEMORY,
	VID_STATUS_INVALID_STREAM,
	VID_STATUS_END_OF_FILE,
	VID_STATUS_FAILED,
	VID_STATUS_SUSPENDED
	} VID_Status_t;


	// Picture clock frequency struct
	typedef struct
	{
	int Divisor;
	int ConversionCode;
	} PictClockFreq_t;


	typedef enum
	{
	VIDEODEC_NULL,
	VIDEODEC_CONFIGINFO,
	VIDEODEC_FILE,
	VIDEODEC_TCP,
	VIDEODEC_RTP_2190,
	VIDEODEC_RTP_2429,
	VIDEODEC_RTP_MP4,
	VIDEODEC_H324M
	} VideoDecInputType_t;

	typedef enum
	{
	MODE_CRC_DISABLE,
	MODE_CRC_DISCARD,
	} VideoDecCRCMode_t;
	*/
#ifdef ERC_SUPPORT
#define RESYNC_MARKER_LENGTH    16
#define GOV_VOP_SC_LENGTH       24
#define ERROR_CONCEALMENT_THR   90
#define MB_NUMBER               3600

#define MATRIX_COEFFICIENTS   6
#define BLOCK_COUNT           6
#define BLOCK_LUMA_COUNT      4
#define BLOCK_COUNT_1        (BLOCK_COUNT - 1)
#define BLOCK_COUNT_4        (1<<(BLOCK_COUNT_1-4))
#define BLOCK_COUNT_5        (1<<(BLOCK_COUNT_1-5))

#define INT_RVLD_ESCAPE  16384 /* (4*4096 +  0*2048 +  0*32 +  0) */

#define mmax(a, b)      ((a) > (b) ? (a) : (b))
#define mmin(a, b)      ((a) < (b) ? (a) : (b))

	/* OPTIMIZED VLD TABLES */
	/* These 2 tables are formed by a single val,
	and val is so composed:
	first    4 bits for len
	second   1 bit  for last
	third    6 bits for run
	fourth   5 bits for level. */

	static unsigned short rvlc_intra[] =
	{        /*    len      last     run   level */
		0,     /*  0*4096 +  0*2048 +  0*32 +  0 */ /* ERROR */
		0,     /*  0*4096 +  0*2048 +  0*32 +  0 */ /* ERROR */
		12289, /*  3*4096 +  0*2048 +  0*32 +  1 */
		12290, /*  3*4096 +  0*2048 +  0*32 +  2 */
		16384, /*  4*4096 +  0*2048 +  0*32 +  0 */ /* ESCAPE */
		16417, /*  4*4096 +  0*2048 +  1*32 +  1 */
		16387, /*  4*4096 +  0*2048 +  0*32 +  3 */
		18433, /*  4*4096 +  1*2048 +  0*32 +  1 */
		20545, /*  5*4096 +  0*2048 +  2*32 +  1 */
		20577, /*  5*4096 +  0*2048 +  3*32 +  1 */
		20514, /*  5*4096 +  0*2048 +  1*32 +  2 */
		20484, /*  5*4096 +  0*2048 +  0*32 +  4 */
		22561, /*  5*4096 +  1*2048 +  1*32 +  1 */
		22593, /*  5*4096 +  1*2048 +  2*32 +  1 */
		24705, /*  6*4096 +  0*2048 +  4*32 +  1 */
		24737, /*  6*4096 +  0*2048 +  5*32 +  1 */
		24581, /*  6*4096 +  0*2048 +  0*32 +  5 */
		24582, /*  6*4096 +  0*2048 +  0*32 +  6 */
		26721, /*  6*4096 +  1*2048 +  3*32 +  1 */
		26753, /*  6*4096 +  1*2048 +  4*32 +  1 */
		26785, /*  6*4096 +  1*2048 +  5*32 +  1 */
		26817, /*  6*4096 +  1*2048 +  6*32 +  1 */
		28865, /*  7*4096 +  0*2048 +  6*32 +  1 */
		28897, /*  7*4096 +  0*2048 +  7*32 +  1 */
		28738, /*  7*4096 +  0*2048 +  2*32 +  2 */
		28707, /*  7*4096 +  0*2048 +  1*32 +  3 */
		28679, /*  7*4096 +  0*2048 +  0*32 +  7 */
		30945, /*  7*4096 +  1*2048 +  7*32 +  1 */
		30977, /*  7*4096 +  1*2048 +  8*32 +  1 */
		31009, /*  7*4096 +  1*2048 +  9*32 +  1 */
		31041, /*  7*4096 +  1*2048 + 10*32 +  1 */
		31073, /*  7*4096 +  1*2048 + 11*32 +  1 */
		33025, /*  8*4096 +  0*2048 +  8*32 +  1 */
		33057, /*  8*4096 +  0*2048 +  9*32 +  1 */
		32866, /*  8*4096 +  0*2048 +  3*32 +  2 */
		32898, /*  8*4096 +  0*2048 +  4*32 +  2 */
		32804, /*  8*4096 +  0*2048 +  1*32 +  4 */
		32805, /*  8*4096 +  0*2048 +  1*32 +  5 */
		32776, /*  8*4096 +  0*2048 +  0*32 +  8 */
		32777, /*  8*4096 +  0*2048 +  0*32 +  9 */
		34818, /*  8*4096 +  1*2048 +  0*32 +  2 */
		35201, /*  8*4096 +  1*2048 + 12*32 +  1 */
		35233, /*  8*4096 +  1*2048 + 13*32 +  1 */
		35265, /*  8*4096 +  1*2048 + 14*32 +  1 */
		37185, /*  9*4096 +  0*2048 + 10*32 +  1 */
		37026, /*  9*4096 +  0*2048 +  5*32 +  2 */
		36931, /*  9*4096 +  0*2048 +  2*32 +  3 */
		36963, /*  9*4096 +  0*2048 +  3*32 +  3 */
		36902, /*  9*4096 +  0*2048 +  1*32 +  6 */
		36874, /*  9*4096 +  0*2048 +  0*32 + 10 */
		36875, /*  9*4096 +  0*2048 +  0*32 + 11 */
		38946, /*  9*4096 +  1*2048 +  1*32 +  2 */
		39393, /*  9*4096 +  1*2048 + 15*32 +  1 */
		39425, /*  9*4096 +  1*2048 + 16*32 +  1 */
		39457, /*  9*4096 +  1*2048 + 17*32 +  1 */
		39489, /*  9*4096 +  1*2048 + 18*32 +  1 */
		39521, /*  9*4096 +  1*2048 + 19*32 +  1 */
		39553, /*  9*4096 +  1*2048 + 20*32 +  1 */
		41313, /* 10*4096 +  0*2048 + 11*32 +  1 */
		41345, /* 10*4096 +  0*2048 + 12*32 +  1 */
		41154, /* 10*4096 +  0*2048 +  6*32 +  2 */
		41186, /* 10*4096 +  0*2048 +  7*32 +  2 */
		41218, /* 10*4096 +  0*2048 +  8*32 +  2 */
		41091, /* 10*4096 +  0*2048 +  4*32 +  3 */
		41028, /* 10*4096 +  0*2048 +  2*32 +  4 */
		40999, /* 10*4096 +  0*2048 +  1*32 +  7 */
		40972, /* 10*4096 +  0*2048 +  0*32 + 12 */
		40973, /* 10*4096 +  0*2048 +  0*32 + 13 */
		40974, /* 10*4096 +  0*2048 +  0*32 + 14 */
		43681, /* 10*4096 +  1*2048 + 21*32 +  1 */
		43713, /* 10*4096 +  1*2048 + 22*32 +  1 */
		43745, /* 10*4096 +  1*2048 + 23*32 +  1 */
		43777, /* 10*4096 +  1*2048 + 24*32 +  1 */
		43809, /* 10*4096 +  1*2048 + 25*32 +  1 */
		45473, /* 11*4096 +  0*2048 + 13*32 +  1 */
		45346, /* 11*4096 +  0*2048 +  9*32 +  2 */
		45219, /* 11*4096 +  0*2048 +  5*32 +  3 */
		45251, /* 11*4096 +  0*2048 +  6*32 +  3 */
		45283, /* 11*4096 +  0*2048 +  7*32 +  3 */
		45156, /* 11*4096 +  0*2048 +  3*32 +  4 */
		45125, /* 11*4096 +  0*2048 +  2*32 +  5 */
		45126, /* 11*4096 +  0*2048 +  2*32 +  6 */
		45096, /* 11*4096 +  0*2048 +  1*32 +  8 */
		45097, /* 11*4096 +  0*2048 +  1*32 +  9 */
		45071, /* 11*4096 +  0*2048 +  0*32 + 15 */
		45072, /* 11*4096 +  0*2048 +  0*32 + 16 */
		45073, /* 11*4096 +  0*2048 +  0*32 + 17 */
		47107, /* 11*4096 +  1*2048 +  0*32 +  3 */
		47170, /* 11*4096 +  1*2048 +  2*32 +  2 */
		47937, /* 11*4096 +  1*2048 + 26*32 +  1 */
		47969, /* 11*4096 +  1*2048 + 27*32 +  1 */
		48001, /* 11*4096 +  1*2048 + 28*32 +  1 */
		49474, /* 12*4096 +  0*2048 + 10*32 +  2 */
		49284, /* 12*4096 +  0*2048 +  4*32 +  4 */
		49316, /* 12*4096 +  0*2048 +  5*32 +  4 */
		49348, /* 12*4096 +  0*2048 +  6*32 +  4 */
		49253, /* 12*4096 +  0*2048 +  3*32 +  5 */
		49285, /* 12*4096 +  0*2048 +  4*32 +  5 */
		49194, /* 12*4096 +  0*2048 +  1*32 + 10 */
		49170, /* 12*4096 +  0*2048 +  0*32 + 18 */
		49171, /* 12*4096 +  0*2048 +  0*32 + 19 */
		49174, /* 12*4096 +  0*2048 +  0*32 + 22 */
		51235, /* 12*4096 +  1*2048 +  1*32 +  3 */
		51298, /* 12*4096 +  1*2048 +  3*32 +  2 */
		51330, /* 12*4096 +  1*2048 +  4*32 +  2 */
		52129, /* 12*4096 +  1*2048 + 29*32 +  1 */
		52161, /* 12*4096 +  1*2048 + 30*32 +  1 */
		52193, /* 12*4096 +  1*2048 + 31*32 +  1 */
		52225, /* 12*4096 +  1*2048 + 32*32 +  1 */
		52257, /* 12*4096 +  1*2048 + 33*32 +  1 */
		52289, /* 12*4096 +  1*2048 + 34*32 +  1 */
		52321, /* 12*4096 +  1*2048 + 35*32 +  1 */
		53697, /* 13*4096 +  0*2048 + 14*32 +  1 */
		53729, /* 13*4096 +  0*2048 + 15*32 +  1 */
		53602, /* 13*4096 +  0*2048 + 11*32 +  2 */
		53507, /* 13*4096 +  0*2048 +  8*32 +  3 */
		53539, /* 13*4096 +  0*2048 +  9*32 +  3 */
		53476, /* 13*4096 +  0*2048 +  7*32 +  4 */
		53350, /* 13*4096 +  0*2048 +  3*32 +  6 */
		53319, /* 13*4096 +  0*2048 +  2*32 +  7 */
		53320, /* 13*4096 +  0*2048 +  2*32 +  8 */
		53321, /* 13*4096 +  0*2048 +  2*32 +  9 */
		53291, /* 13*4096 +  0*2048 +  1*32 + 11 */
		53268, /* 13*4096 +  0*2048 +  0*32 + 20 */
		53269, /* 13*4096 +  0*2048 +  0*32 + 21 */
		53271, /* 13*4096 +  0*2048 +  0*32 + 23 */
		55300, /* 13*4096 +  1*2048 +  0*32 +  4 */
		55458, /* 13*4096 +  1*2048 +  5*32 +  2 */
		55490, /* 13*4096 +  1*2048 +  6*32 +  2 */
		55522, /* 13*4096 +  1*2048 +  7*32 +  2 */
		55554, /* 13*4096 +  1*2048 +  8*32 +  2 */
		55586, /* 13*4096 +  1*2048 +  9*32 +  2 */
		56449, /* 13*4096 +  1*2048 + 36*32 +  1 */
		56481, /* 13*4096 +  1*2048 + 37*32 +  1 */
		57857, /* 14*4096 +  0*2048 + 16*32 +  1 */
		57889, /* 14*4096 +  0*2048 + 17*32 +  1 */
		57921, /* 14*4096 +  0*2048 + 18*32 +  1 */
		57604, /* 14*4096 +  0*2048 +  8*32 +  4 */
		57509, /* 14*4096 +  0*2048 +  5*32 +  5 */
		57478, /* 14*4096 +  0*2048 +  4*32 +  6 */
		57510, /* 14*4096 +  0*2048 +  5*32 +  6 */
		57447, /* 14*4096 +  0*2048 +  3*32 +  7 */
		57448, /* 14*4096 +  0*2048 +  3*32 +  8 */
		57418, /* 14*4096 +  0*2048 +  2*32 + 10 */
		57419, /* 14*4096 +  0*2048 +  2*32 + 11 */
		57388, /* 14*4096 +  0*2048 +  1*32 + 12 */
		57389, /* 14*4096 +  0*2048 +  1*32 + 13 */
		57368, /* 14*4096 +  0*2048 +  0*32 + 24 */
		57369, /* 14*4096 +  0*2048 +  0*32 + 25 */
		57370, /* 14*4096 +  0*2048 +  0*32 + 26 */
		59397, /* 14*4096 +  1*2048 +  0*32 +  5 */
		59428, /* 14*4096 +  1*2048 +  1*32 +  4 */
		59714, /* 14*4096 +  1*2048 + 10*32 +  2 */
		59746, /* 14*4096 +  1*2048 + 11*32 +  2 */
		59778, /* 14*4096 +  1*2048 + 12*32 +  2 */
		60609, /* 14*4096 +  1*2048 + 38*32 +  1 */
		60641, /* 14*4096 +  1*2048 + 39*32 +  1 */
		60673, /* 14*4096 +  1*2048 + 40*32 +  1 */
		61467, /* 15*4096 +  0*2048 +  0*32 + 27 */
		61545, /* 15*4096 +  0*2048 +  3*32 +  9 */
		61637, /* 15*4096 +  0*2048 +  6*32 +  5 */
		61669, /* 15*4096 +  0*2048 +  7*32 +  5 */
		61732, /* 15*4096 +  0*2048 +  9*32 +  4 */
		61826, /* 15*4096 +  0*2048 + 12*32 +  2 */
		62049, /* 15*4096 +  0*2048 + 19*32 +  1 */
		63525, /* 15*4096 +  1*2048 +  1*32 +  5 */
		63555, /* 15*4096 +  1*2048 +  2*32 +  3 */
		63906, /* 15*4096 +  1*2048 + 13*32 +  2 */
		64801, /* 15*4096 +  1*2048 + 41*32 +  1 */
		64833, /* 15*4096 +  1*2048 + 42*32 +  1 */
		64865, /* 15*4096 +  1*2048 + 43*32 +  1 */
		64897  /* 15*4096 +  1*2048 + 44*32 +  1 */
	};

	static unsigned short rvlc_inter[] =
	{        /*    len      last     run   level */
		0,     /*  0*4096 +  0*2048 +  0*32 +  0 */ /* ERROR */
		0,     /*  0*4096 +  0*2048 +  0*32 +  0 */ /* ERROR */
		12289, /*  3*4096 +  0*2048 +  0*32 +  1 */
		12321, /*  3*4096 +  0*2048 +  1*32 +  1 */
		16384, /*  4*4096 +  0*2048 +  0*32 +  0 */ /* ESCAPE */
		16386, /*  4*4096 +  0*2048 +  0*32 +  2 */
		16449, /*  4*4096 +  0*2048 +  2*32 +  1 */
		18433, /*  4*4096 +  1*2048 +  0*32 +  1 */
		20483, /*  5*4096 +  0*2048 +  0*32 +  3 */
		20577, /*  5*4096 +  0*2048 +  3*32 +  1 */
		20609, /*  5*4096 +  0*2048 +  4*32 +  1 */
		20641, /*  5*4096 +  0*2048 +  5*32 +  1 */
		22561, /*  5*4096 +  1*2048 +  1*32 +  1 */
		22593, /*  5*4096 +  1*2048 +  2*32 +  1 */
		24610, /*  6*4096 +  0*2048 +  1*32 +  2 */
		24769, /*  6*4096 +  0*2048 +  6*32 +  1 */
		24801, /*  6*4096 +  0*2048 +  7*32 +  1 */
		24833, /*  6*4096 +  0*2048 +  8*32 +  1 */
		26721, /*  6*4096 +  1*2048 +  3*32 +  1 */
		26753, /*  6*4096 +  1*2048 +  4*32 +  1 */
		26785, /*  6*4096 +  1*2048 +  5*32 +  1 */
		26817, /*  6*4096 +  1*2048 +  6*32 +  1 */
		28676, /*  7*4096 +  0*2048 +  0*32 +  4 */
		28738, /*  7*4096 +  0*2048 +  2*32 +  2 */
		28961, /*  7*4096 +  0*2048 +  9*32 +  1 */
		28993, /*  7*4096 +  0*2048 + 10*32 +  1 */
		29025, /*  7*4096 +  0*2048 + 11*32 +  1 */
		30945, /*  7*4096 +  1*2048 +  7*32 +  1 */
		30977, /*  7*4096 +  1*2048 +  8*32 +  1 */
		31009, /*  7*4096 +  1*2048 +  9*32 +  1 */
		31041, /*  7*4096 +  1*2048 + 10*32 +  1 */
		31073, /*  7*4096 +  1*2048 + 11*32 +  1 */
		32773, /*  8*4096 +  0*2048 +  0*32 +  5 */
		32774, /*  8*4096 +  0*2048 +  0*32 +  6 */
		32803, /*  8*4096 +  0*2048 +  1*32 +  3 */
		32866, /*  8*4096 +  0*2048 +  3*32 +  2 */
		32898, /*  8*4096 +  0*2048 +  4*32 +  2 */
		33153, /*  8*4096 +  0*2048 + 12*32 +  1 */
		33185, /*  8*4096 +  0*2048 + 13*32 +  1 */
		33217, /*  8*4096 +  0*2048 + 14*32 +  1 */
		34818, /*  8*4096 +  1*2048 +  0*32 +  2 */
		35201, /*  8*4096 +  1*2048 + 12*32 +  1 */
		35233, /*  8*4096 +  1*2048 + 13*32 +  1 */
		35265, /*  8*4096 +  1*2048 + 14*32 +  1 */
		36871, /*  9*4096 +  0*2048 +  0*32 +  7 */
		36900, /*  9*4096 +  0*2048 +  1*32 +  4 */
		36931, /*  9*4096 +  0*2048 +  2*32 +  3 */
		37026, /*  9*4096 +  0*2048 +  5*32 +  2 */
		37345, /*  9*4096 +  0*2048 + 15*32 +  1 */
		37377, /*  9*4096 +  0*2048 + 16*32 +  1 */
		37409, /*  9*4096 +  0*2048 + 17*32 +  1 */
		38946, /*  9*4096 +  1*2048 +  1*32 +  2 */
		39393, /*  9*4096 +  1*2048 + 15*32 +  1 */
		39425, /*  9*4096 +  1*2048 + 16*32 +  1 */
		39457, /*  9*4096 +  1*2048 + 17*32 +  1 */
		39489, /*  9*4096 +  1*2048 + 18*32 +  1 */
		39521, /*  9*4096 +  1*2048 + 19*32 +  1 */
		39553, /*  9*4096 +  1*2048 + 20*32 +  1 */
		40968, /* 10*4096 +  0*2048 +  0*32 +  8 */
		40969, /* 10*4096 +  0*2048 +  0*32 +  9 */
		40997, /* 10*4096 +  0*2048 +  1*32 +  5 */
		41059, /* 10*4096 +  0*2048 +  3*32 +  3 */
		41154, /* 10*4096 +  0*2048 +  6*32 +  2 */
		41186, /* 10*4096 +  0*2048 +  7*32 +  2 */
		41218, /* 10*4096 +  0*2048 +  8*32 +  2 */
		41250, /* 10*4096 +  0*2048 +  9*32 +  2 */
		41537, /* 10*4096 +  0*2048 + 18*32 +  1 */
		41569, /* 10*4096 +  0*2048 + 19*32 +  1 */
		41601, /* 10*4096 +  0*2048 + 20*32 +  1 */
		43681, /* 10*4096 +  1*2048 + 21*32 +  1 */
		43713, /* 10*4096 +  1*2048 + 22*32 +  1 */
		43745, /* 10*4096 +  1*2048 + 23*32 +  1 */
		43777, /* 10*4096 +  1*2048 + 24*32 +  1 */
		43809, /* 10*4096 +  1*2048 + 25*32 +  1 */
		45066, /* 11*4096 +  0*2048 +  0*32 + 10 */
		45067, /* 11*4096 +  0*2048 +  0*32 + 11 */
		45094, /* 11*4096 +  0*2048 +  1*32 +  6 */
		45124, /* 11*4096 +  0*2048 +  2*32 +  4 */
		45187, /* 11*4096 +  0*2048 +  4*32 +  3 */
		45219, /* 11*4096 +  0*2048 +  5*32 +  3 */
		45378, /* 11*4096 +  0*2048 + 10*32 +  2 */
		45729, /* 11*4096 +  0*2048 + 21*32 +  1 */
		45761, /* 11*4096 +  0*2048 + 22*32 +  1 */
		45793, /* 11*4096 +  0*2048 + 23*32 +  1 */
		45825, /* 11*4096 +  0*2048 + 24*32 +  1 */
		45857, /* 11*4096 +  0*2048 + 25*32 +  1 */
		45889, /* 11*4096 +  0*2048 + 26*32 +  1 */
		47107, /* 11*4096 +  1*2048 +  0*32 +  3 */
		47170, /* 11*4096 +  1*2048 +  2*32 +  2 */
		47937, /* 11*4096 +  1*2048 + 26*32 +  1 */
		47969, /* 11*4096 +  1*2048 + 27*32 +  1 */
		48001, /* 11*4096 +  1*2048 + 28*32 +  1 */
		49164, /* 12*4096 +  0*2048 +  0*32 + 12 */
		49191, /* 12*4096 +  0*2048 +  1*32 +  7 */
		49221, /* 12*4096 +  0*2048 +  2*32 +  5 */
		49252, /* 12*4096 +  0*2048 +  3*32 +  4 */
		49347, /* 12*4096 +  0*2048 +  6*32 +  3 */
		49379, /* 12*4096 +  0*2048 +  7*32 +  3 */
		49506, /* 12*4096 +  0*2048 + 11*32 +  2 */
		50017, /* 12*4096 +  0*2048 + 27*32 +  1 */
		50049, /* 12*4096 +  0*2048 + 28*32 +  1 */
		50081, /* 12*4096 +  0*2048 + 29*32 +  1 */
		51235, /* 12*4096 +  1*2048 +  1*32 +  3 */
		51298, /* 12*4096 +  1*2048 +  3*32 +  2 */
		51330, /* 12*4096 +  1*2048 +  4*32 +  2 */
		52129, /* 12*4096 +  1*2048 + 29*32 +  1 */
		52161, /* 12*4096 +  1*2048 + 30*32 +  1 */
		52193, /* 12*4096 +  1*2048 + 31*32 +  1 */
		52225, /* 12*4096 +  1*2048 + 32*32 +  1 */
		52257, /* 12*4096 +  1*2048 + 33*32 +  1 */
		52289, /* 12*4096 +  1*2048 + 34*32 +  1 */
		52321, /* 12*4096 +  1*2048 + 35*32 +  1 */
		53261, /* 13*4096 +  0*2048 +  0*32 + 13 */
		53262, /* 13*4096 +  0*2048 +  0*32 + 14 */
		53263, /* 13*4096 +  0*2048 +  0*32 + 15 */
		53264, /* 13*4096 +  0*2048 +  0*32 + 16 */
		53288, /* 13*4096 +  0*2048 +  1*32 +  8 */
		53349, /* 13*4096 +  0*2048 +  3*32 +  5 */
		53380, /* 13*4096 +  0*2048 +  4*32 +  4 */
		53412, /* 13*4096 +  0*2048 +  5*32 +  4 */
		53507, /* 13*4096 +  0*2048 +  8*32 +  3 */
		53634, /* 13*4096 +  0*2048 + 12*32 +  2 */
		54209, /* 13*4096 +  0*2048 + 30*32 +  1 */
		54241, /* 13*4096 +  0*2048 + 31*32 +  1 */
		54273, /* 13*4096 +  0*2048 + 32*32 +  1 */
		54305, /* 13*4096 +  0*2048 + 33*32 +  1 */
		55300, /* 13*4096 +  1*2048 +  0*32 +  4 */
		55458, /* 13*4096 +  1*2048 +  5*32 +  2 */
		55490, /* 13*4096 +  1*2048 +  6*32 +  2 */
		55522, /* 13*4096 +  1*2048 +  7*32 +  2 */
		55554, /* 13*4096 +  1*2048 +  8*32 +  2 */
		55586, /* 13*4096 +  1*2048 +  9*32 +  2 */
		56449, /* 13*4096 +  1*2048 + 36*32 +  1 */
		56481, /* 13*4096 +  1*2048 + 37*32 +  1 */
		57361, /* 14*4096 +  0*2048 +  0*32 + 17 */
		57362, /* 14*4096 +  0*2048 +  0*32 + 18 */
		57385, /* 14*4096 +  0*2048 +  1*32 +  9 */
		57386, /* 14*4096 +  0*2048 +  1*32 + 10 */
		57414, /* 14*4096 +  0*2048 +  2*32 +  6 */
		57415, /* 14*4096 +  0*2048 +  2*32 +  7 */
		57446, /* 14*4096 +  0*2048 +  3*32 +  6 */
		57540, /* 14*4096 +  0*2048 +  6*32 +  4 */
		57635, /* 14*4096 +  0*2048 +  9*32 +  3 */
		57762, /* 14*4096 +  0*2048 + 13*32 +  2 */
		57794, /* 14*4096 +  0*2048 + 14*32 +  2 */
		57826, /* 14*4096 +  0*2048 + 15*32 +  2 */
		57858, /* 14*4096 +  0*2048 + 16*32 +  2 */
		58433, /* 14*4096 +  0*2048 + 34*32 +  1 */
		58465, /* 14*4096 +  0*2048 + 35*32 +  1 */
		58497, /* 14*4096 +  0*2048 + 36*32 +  1 */
		59397, /* 14*4096 +  1*2048 +  0*32 +  5 */
		59428, /* 14*4096 +  1*2048 +  1*32 +  4 */
		59714, /* 14*4096 +  1*2048 + 10*32 +  2 */
		59746, /* 14*4096 +  1*2048 + 11*32 +  2 */
		59778, /* 14*4096 +  1*2048 + 12*32 +  2 */
		60609, /* 14*4096 +  1*2048 + 38*32 +  1 */
		60641, /* 14*4096 +  1*2048 + 39*32 +  1 */
		60673, /* 14*4096 +  1*2048 + 40*32 +  1 */
		61459, /* 15*4096 +  0*2048 +  0*32 + 19 */
		61543, /* 15*4096 +  0*2048 +  3*32 +  7 */
		61573, /* 15*4096 +  0*2048 +  4*32 +  5 */
		61668, /* 15*4096 +  0*2048 +  7*32 +  4 */
		61986, /* 15*4096 +  0*2048 + 17*32 +  2 */
		62625, /* 15*4096 +  0*2048 + 37*32 +  1 */
		62657, /* 15*4096 +  0*2048 + 38*32 +  1 */
		63525, /* 15*4096 +  1*2048 +  1*32 +  5 */
		63555, /* 15*4096 +  1*2048 +  2*32 +  3 */
		63906, /* 15*4096 +  1*2048 + 13*32 +  2 */
		64801, /* 15*4096 +  1*2048 + 41*32 +  1 */
		64833, /* 15*4096 +  1*2048 + 42*32 +  1 */
		64865, /* 15*4096 +  1*2048 + 43*32 +  1 */
		64897  /* 15*4096 +  1*2048 + 44*32 +  1 */
	};

	/* Reversible Variable Length Code: index table */
	static char rvlc_index[13][14] = {
		{  1,  2,  4,  7, 11, 16, 22, 29, 37, 46, 56, 67, 79,  0},
		{  3,  0,  5,  8, 12, 17, 23, 30, 38, 47, 57, 68, 80,  0},
		{  6,  0,  0,  9, 13, 18, 24, 31, 39, 48, 58, 69, 81,  0},
		{ 10,  0,  0,  0, 14, 19, 25, 32, 40, 49, 59, 70, 82,  0},
		{ 15,  0,  0,  0,  0, 20, 26, 33, 41, 50, 60, 71, 83,  0},
		{ 21,  0,  0,  0,  0,  0, 27, 34, 42, 51, 61, 72, 84,  0},
		{ 28,  0,  0,  0,  0,  0,  0, 35, 43, 52, 62, 73, 85,  0},
		{ 36,  0,  0,  0,  0,  0,  0,  0, 44, 53, 63, 74,  0,  0},
		{ 45,  0,  0,  0,  0,  0,  0,  0,  0, 54, 64, 75,  0,  0},
		{ 55,  0,  0,  0,  0,  0,  0,  0,  0,  0, 65, 76,  0,  0},
		{ 66,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 77,  0,  0},
		{ 78,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0},
		{  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}
	};
#endif
	typedef enum
	{
		None,
		GSC,
		EndSession,           // No more bits in input buffer and callback returned 0 bytes read -> End of decoding
		VOSC,                 // video object start code
		VOLSC,                // video object layer start code
		VisOSSC,              // visual object sequence start code
		UDSC,                 // user data start code
		GVOPSC,               // group of vop start code
		VSEC,                 // video session error code
		VisOSC,               // visual object start code
		PSC,                  // vop / picture start code
		ShortHeader,          // MPEG4 with short headers (H263 baseline)
#ifdef ENABLE_SORENSON
		Sorenson              // Sorenson startcode
#endif
	} ParseEvent_t;

	typedef struct
	{
		sint16 x;   // X Coordinate
		sint16 y;   // Y Coordinate
	} Vector_t;
/*
	typedef struct
	{
		sint32 x;   // X Coordinate 
		sint32 y;   // Y Coordinate
	} Vector32_t;
*/
	typedef struct
	{
		sint16 Number;
		sint16 Start;
		sint16 End;
	} SegData_t;

	//Storage of data for advanced intra coding mode
	typedef struct
	{
		sint16 RCoeff[6][8];
	} RCoeff_t;

	typedef struct
	{
		RCoeff_t* Ap;
		RCoeff_t* Bp;
		sint16    TopLeftDC_3;  // Used in MPEG4 to determine pred. direction
		sint16    TopLeftDC_4;
		sint16    TopLeftDC_5;
	} AIC_Data_t;



	typedef struct
	{
		uint8* Src_p;
		uint8* Dst_p;
		uint16    SrcLineWidth;
		uint16    SrcDispWidth;
		uint16    SrcHeight;
		uint16    DstLineWidth;
		//uint8    RTYPE;
	} MC_Params_t;

	// YUV Image
	typedef struct
	{
		uint8*  Y_p;           // Luminance plane
		uint8*  Cb_p;          // Cb plane
		uint8*  Cr_p;          // Cr plane
		uint16    Y_LineWidth;   // Distance to next row
		uint16    C_LineWidth;
		uint16    Y_DispWidth;   // The number pixels on a line to display
		uint16    C_DispWidth;
		uint16    Y_Height;
		uint16    C_Height;
		//boolean ReadOnly;      // Indication that the buffer is used as a reference and should not be written to
	} YUV_Image_t;

	typedef enum
	{
		Y  = 0,
		CB = 1,
		CR = 2
	} YUV_Component_t;

	typedef struct
	{
		Vector_t    MV[4];    //0 is used when only one motion vectors
		MC_Params_t SourceImage[3];
	} MC_Source_t;

	typedef uint16 QuantMat_t;

	/************************** INSTREAMBUFFER *******************************/
	typedef struct
	{
		Standard_t			Standard;
		uint8*				Buffer_p;
		uint8*				BufferEnd_p;
		uint8*				CurrentBuffer_p;
		uint32				Buffer32;
		sint32				Buffer32_Pos;
		sint32				BytesRead;
		ParseEvent_t			CurrentStartcode;  // ERARDSG - Is this only really used for EOS? If so remove?
		uint8*				PreviousStartcodePosition_p;
		//VideoDecInputType_t InputType;
		//VideoDecCRCMode_t   ModeCRC;
		//boolean             PreviousPacketMissing; // ERARDSG - To use with the DISCARD mode also? Now only used with AWARE
		//sint16              ConcealmentStart;      // ERARDSG - Remove if CRC_AWARE is removed
		sint16				MB_Nr;                 // ERARDSG - Remove if CRC_AWARE is removed
		sint16				dummy1;
		//boolean             PacketMarker;      // ETHONYB - Used to indicate significant events, such as last packet of a frame
		void *				PicLevel;         // EPONEVE - InstreamBuffer_t is the only struct accessable in all functions
		void *				MB_Level;         // EPONEVE - InstreamBuffer_t is the only struct accessable in all functions
		//#ifdef ERC_SUPPORT
		sint32				forward_mb[3600];
		sint32				backward_mb[3600];
		uint8*				Buffer_p_backward;
		sint32				Buffer32_Pos_backward;
		sint32				VP_MBA_init;
		sint32				mb_in_video_packet;
		uint32				start_VP_bit;
		uint32				start_VP_byte;
		uint16				vop_fcode_forward;
		uint16				rvlc;
		uint16				fault;  //fault=1 forward, fault=2 means backward error concealment
		uint16				errorPos;
		uint8					inbfr_backward[12]; /* 16 */ /* internal buffer */
		//#ifdef ERC_SUPPORT

		uint32		    stuffing_check;
		uint8*			tmp_rdptr;      /* temporany input read buffer pointer */
		sint32			tmp_incnt;                 /* temporany internal counter */
		uint32            tmp_buffer;
		uint16            reInitialize;
		//#endif
		//#endif
		uint16				debugFrameCounter;
		uint16				dummy3;
		//uint32				dummy4;

	} InstreamBuffer_t;

	/************************** MB_DATA *******************************/
	typedef struct
	{
		// DATA FOR KEEPING TRACK OF THE POSITION OF THE MACROBLOCK
		Vector_t  Coordinates;            // Pixel coordinates for the upper and leftmost pixel
		sint16    Number;                 // What number does the macroblock have
		//sint16    PFMBNr;                 // MB number when rotated, only used in postfilter
		// MACROBLOCK LAYER CODING INFORMATION
		sint16    SegNr;                  // Segment number, initizalized to -1
		uint8     Place;                  // Where is the macroblock in picture?
		uint8     CodeMB;                 // CODE_MB or SKIP_MB.
		uint8     CodingType;             // Indicate INTRA,INTER,E_INTER_F etc
		uint8     CodeBlock;              // One bit for each block (bitstream order)
		//uint8     PrevCodingType;         // save coding_type from last picture
		uint8     B_CoLocatedNotCoded;      // the co-located macroblock in the future reference VOP is skipped
		uint8     Quant;                  // The quantizer value used for this mb
		sint8     DQuant;                 // The change of quant since previous mb
		uint8     MV_Estimated;           // Signals that MV is alread estimated
		uint8     FourMV;                 // Indicate that this macroblock uses four mv
		uint8     AIC_PredMode;           // (0,1,2) depending on pred. type

		uint8     DecodeIntraDC;          // Decode Intra block even if skipped
		uint8     PredictionMode;          // TYpe of prediction, Always forward for S and P frames but could be Backward, Forward, Interpolate or direct for B frames

		uint16     MODB;
		uint16     MCSEL;                  // GMC stuff
		// Motion vectors and predictors
		Vector_t  BackwardMotionVector[4];           // Motionvector for the whole macroblock
		Vector_t  ForwardMotionVector[4];           // Motionvector for the whole macroblock
		sint16    IntraDC_Array[6];       // MPEG4 + I_VOP + DP -> storage for DC needed
		uint8     LastCoeffs[6];
		uint16     dummyallign;
		Vector_t  Average_Pel;

	} MB_Data_t;

	typedef struct LayerData_t LayerData_t;

	/******************* LAYERDATA *******************************************************************************************/

	typedef void gmc_sample_reconstruction_fp_t(LayerData_t* , sint16);

	struct LayerData_t
	{
		uint8                       NumberOf_GOBS;          // How many GOBs the picture has
		uint16                      NrMB_InGOB;             // The number of macroblocks in gob
		uint16                      NrMB;                   // Total number of macroblocks
		uint8                       NrMB_Horizontal;        // The width in number of macroblocks
		//uint8                       NrMB_Vertical;          // The height in number of macroblocks

		uint8                     HEC_Found;              // MPEG4 specific, indicates if slice has HEC
		//uint16                    MB_Number;
		sint16						MB_Number;				//bug fix

		YUV_Image_t               CurrRecImage __attribute__((aligned(32)));           // The picture being decoded, can be I,P, or B
		YUV_Image_t               PrevRecImage __attribute__((aligned(32)));           // Forward reference, can only be I,P,S frame
		YUV_Image_t               FutureRecImage __attribute__((aligned(32)));         // Backward reference, can only be I,P,S frame
		MC_Source_t               MC_Params __attribute__((aligned(32)));
		uint8                     LastCoeffSymZigZag[6]__attribute__((aligned(32)));  // Lastcoeff according to symmetric zigzag table
		uint8                     Warping_accuracy_log;
		sint32                    gmc_const_c[2];
		gmc_sample_reconstruction_fp_t *gmc_sample_reconstruction_fp;
		InstreamBuffer_t*         Instream_p;

		// Pointers to data used for this layer
		MB_Data_t*                MB_DataArray_p;
		AIC_Data_t*               AIC_Data_p;
		sint16                    DCTArray[384]__attribute__((aligned(32)));          // DCT values for a MB, 4 Y 2 UV = 6*64 = 384 coeffs
		//Standard_t                    Standard;
		//VID_Status_t                  InfoCode;
		//uint8                         InterpolateMode;
		//PictClockFreq_t               PictClockFreq;
		uint16      TRB;                // temporal diff between B-VOP and previous(latest decoded) Ref-VOP
		uint16      TRD;                // temporal diff between two Ref-VOP's previously decoded
		Vector_t    ForwardPredictedMV[2]; // Prediction of mv for the whole macroblock
		Vector_t    BackwardPredictedMV[2];// Prediction of mv for the whole macroblock
		SegData_t   CurrSegData;        // Info about the currently decoded segment
		MB_Data_t*  MB_History_p;       // Macroblocks from the most recently I,P or S(GMC)-VOP
		Vector_t Sprite_Points[MAX_WARPING_POINTS_SUPPORTED];


		ts_t1xhv_vdc_mpeg4_param_in    *Gp_vdc_mpeg4_param_in;              /**< \brief Input parameters from Host                   */
		ts_t1xhv_vdc_mpeg4_param_inout *Gp_vdc_mpeg4_param_inout_in;	          /**< \brief Input/Output parameters                      */
		ts_t1xhv_vdc_mpeg4_param_inout *Gp_vdc_mpeg4_param_inout_out;	          /**< \brief Input/Output parameters                      */
		ts_t1xhv_vdc_mpeg4_param_out   *Gp_vdc_mpeg4_param_out;	          /**< \brief Output parameters from Host                  */
		ts_t1xhv_vdc_frame_buf_in 	  *Gp_vdc_mpeg4_frame_buf_in;  	      /**< \brief Input Frame buffer addresses                 */
		ts_t1xhv_vdc_frame_buf_out 	 *Gp_vdc_mpeg4_frame_buf_out;	      /**< \brief Output Frame buffer addresses                */
		ts_t1xhv_bitstream_buf_link	  *Gp_vdc_mpeg4_bitstream_buf_link;    /**< \brief Bitstream buffer parameters for init         */
		ts_t1xhv_bitstream_buf 		*Gp_vdc_mpeg4_bitstream_buf;         /**< \brief Bitstream buffer parameters for update       */
		ts_t1xhv_vdc_internal_buf      *Gp_vdc_mpeg4_internal_buf;
		ts_t1xhv_bitstream_buf_pos     *Gp_vdc_mpeg4_bitstream_buf_pos;

	};

#ifdef _ALGO_DEBUG
#define DEBUG_FRAME_COUNTER 0
#define DEBUG_MB_COUNTER 0
#endif

#ifdef __cplusplus
}
#endif

#endif //INCLUSION_GUARD_VIDEOCODECDECINTERNAL_H
