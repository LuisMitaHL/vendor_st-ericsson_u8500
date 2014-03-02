/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   updownmix.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _updownmix_h_
#define _updownmix_h_

#include "audiolibs_common.h"
#define MAXPATH 12
#define MAXCH 9
#define INPLACE
//#define USE_TABLE
typedef enum  CHANNELTYPE {
     ChannelNone = 0x0,    //< Unused or empty 
     ChannelLF   = 0x1,    //< Left front 
     ChannelRF   = 0x2,    //< Right front 
     ChannelCF   = 0x3,    //< Center front 
     ChannelLS   = 0x4,    //< Left surround 
     ChannelRS   = 0x5,    //< Right surround 
     ChannelLFE  = 0x6,    //< Low frequency effects 
     ChannelCS   = 0x7,    //< Back surround 
     ChannelLR   = 0x8,    //< Left rear. 
     ChannelRR   = 0x9    //< Right rear. 
}  CHANNELTYPE;

typedef struct UPDOWNMIX_LOCAL_STRUCT_T
{
     float  gain[MAXPATH];
	 int inpath[MAXPATH];
	 int outpath[MAXPATH];
	 int numpath;
	 int nchin;
	 int nchout;
#ifdef USE_TABLE
	 float *gain_table;
#endif
}UPDOWNMIX_LOCAL_STRUCT_T;

/*---------------------------------
Dolby PrologicII does an upmix 2->6
-----------------------------------

INPUT			OUTPUT
-----			------
				effectLF(0)
				
effectLF(0)		effectLR(1)

				effectCF(2)

effectRF(1)		effectRR(3)

				effectRF(4)

				effectLFE(5)


		Path	Gain
		----    ----
LF->LF	0->0	1.0
LF->LR	0->1	.87177979
LF->CF	0->2	.70710678
LF->RR	0->3	.48989795
RF->LR	1->1	.48989795	
RF->CF	1->2	.70710678
RF->RR	1->3	.87177979	
RF->RF	1->4    1.0
RF-LFE	1->5	0.0



-------------------------------*/

/*
 * functions 
 */

void init_updownmix(
#ifdef USE_TABLE
	int nchin,int nchout,UPDOWNMIX_LOCAL_STRUCT_T *ctx,int *channelmapin,int *channelmapout,float *gain_table
#else
	int nchin,int nchout,UPDOWNMIX_LOCAL_STRUCT_T *ctx,int *channelmapin,int *channelmapout
#endif
);


void process_updownmix( int *input,int *output, int nsamples_per_channel,UPDOWNMIX_LOCAL_STRUCT_T *ctx);
void process_updownmix_sample16(short *input,short *output, int nsamples_per_channel,UPDOWNMIX_LOCAL_STRUCT_T *ctx);

#endif //_updownmix_h_
