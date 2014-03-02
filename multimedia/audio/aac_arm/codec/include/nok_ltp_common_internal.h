
/**************************************************************************

This software module was originally developed by
  Mikko Suonio (Nokia)

and edited by
  -

in the course of development of the ISO/IEC 13818-7 and ISO/IEC 14496-3 
standards for reference purposes and its performance may not have been 
optimized. This software module is an implementation of one or more tools as 
specified by the ISO/IEC 13818-7 and ISO/IEC 14496-3 standards.
ISO/IEC gives users free license to this software module or modifications 
thereof for use in products claiming conformance to audiovisual and 
image-coding related ITU Recommendations and/or ISO/IEC International 
Standards. ISO/IEC gives users the same free license to this software module or 
modifications thereof for research purposes and further ISO/IEC standardisation.
Those intending to use this software module in products are advised that its 
use may infringe existing patents. ISO/IEC have no liability for use of this 
software module or modifications thereof. Copyright is not released for 
products that do not conform to audiovisual and image-coding related ITU 
Recommendations and/or ISO/IEC International Standards.
The original developer retains full right to modify and use the code for its 
own purpose, assign or donate the code to a third party and to inhibit third 
parties from using the code for products that do not conform to audiovisual and 
image-coding related ITU Recommendations and/or ISO/IEC International Standards.
This copyright notice must be included in all copies or derivative works.
Copyright (c) ISO/IEC 1997.
 *                                                                           *
 ****************************************************************************/

#ifndef NOK_LTP_COMMON_INTERNAL_H_
#define NOK_LTP_COMMON_INTERNAL_H_


/*
  Purpose:      Number of LTP coefficients. */
#define LPC 1

/*
  Purpose:      Maximum LTP lag.  */
#define DELAY 2048

/*
  Purpose:	Length of the bitstream element ltp_data_present.  */
#define	LEN_LTP_DATA_PRESENT 1

/*
  Purpose:	Length of the bitstream element ltp_lag.  */
#define	LEN_LTP_LAG 11

#ifdef VERSION2
/*
  Purpose:	Length of the bitstream element ltp_lag for LD mode.  */
#define	LEN_LTP_LAG_LD 10

/*
  Purpose:	Length of the bitstream element ltp_lag_update for LD mode.  */
#define	LEN_LTP_LAG_UPDATE 1
#endif

/*
  Purpose:	Length of the bitstream element ltp_coef.  */
#define	LEN_LTP_COEF 3

/*
  Purpose:	Length of the bitstream element ltp_short_used.  */
#define	LEN_LTP_SHORT_USED 1

/*
  Purpose:	Length of the bitstream element ltp_short_lag_present.  */
#define	LEN_LTP_SHORT_LAG_PRESENT 1

/*
  Purpose:	Length of the bitstream element ltp_short_lag.  */
#define	LEN_LTP_SHORT_LAG 5

/*
  Purpose:	Offset of the lags written in the bitstream.  */
#define	NOK_LTP_LAG_OFFSET 16

/*
  Purpose:	Length of the bitstream element ltp_long_used.  */
#define	LEN_LTP_LONG_USED 1

/*
  Purpose:	Upper limit for the number of scalefactor bands
   		which can use lt prediction with long windows.
  Explanation:	Bands 0..NOK_MAX_LT_PRED_SFB-1 can use lt prediction.  */
#define	NOK_MAX_LT_PRED_LONG_SFB 40

/*
  Purpose:	Upper limit for the number of scalefactor bands
   		which can use lt prediction with short windows.
  Explanation:	Bands 0..NOK_MAX_LT_PRED_SFB-1 can use lt prediction.  */
#define	NOK_MAX_LT_PRED_SHORT_SFB 8

/*
   Purpose:      Buffer offset to maintain block alignment.
   Explanation:  This is only used for a short window sequence.  */
#define SHORT_SQ_OFFSET (BLOCK_LEN_LONG-(BLOCK_LEN_SHORT*4+BLOCK_LEN_SHORT/2))

/*
  Purpose:	Number of codes for LTP weight. */
#define CODESIZE 8

/*
   Purpose:      Float type for external data.
   Explanation:  - */
typedef Float float_ext;

#ifndef MMDSP
/* Purpose:	Codebook for LTP weight coefficients.  */
#ifdef ARM
#define FORMAT_FLOAT_LTP_WT(a) (int)((a) * (0x3fffffff)) /* 2Q30 */
static const CODEBOOK_MEM int codebook[CODESIZE] =
{
  FORMAT_FLOAT_LTP_WT(0.570829), 
  FORMAT_FLOAT_LTP_WT(0.696616),
  FORMAT_FLOAT_LTP_WT(0.813004),
  FORMAT_FLOAT_LTP_WT(0.911304),
  FORMAT_FLOAT_LTP_WT(0.984900),
  FORMAT_FLOAT_LTP_WT(1.067894),
  FORMAT_FLOAT_LTP_WT(1.194601),
  FORMAT_FLOAT_LTP_WT(1.369533)
};
#else /* ARM */
static const CODEBOOK_MEM Float codebook[CODESIZE] =
{
  0.570829, 
  0.696616,
  0.813004,
  0.911304,
  0.984900,
  1.067894,
  1.194601,
  1.369533
};
#endif /* ARM */
#else
/* Purpose:	Codebook for LTP weight coefficients * 2^22  */
static const Float CODEBOOK_MEM codebook[CODESIZE] =
{
  2394230,
  2921819,
  3409986,
  3822286,
  4130970,
  4479072,
  5010520,
  5744238
};
#endif

#endif /* NOK_LTP_COMMON_INTERNAL_H_ */
