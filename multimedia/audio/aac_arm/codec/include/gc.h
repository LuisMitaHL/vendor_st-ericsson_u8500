
/*******************************************************************
"This software module was originally developed by
    Yoshiaki Oikawa (Sony Corporation),
    Mitsuyuki Hatanaka (Sony Corporation)
and edited by
    Yoshiaki Oikawa (Sony Corporation),
    Mitsuyuki Hatanaka (Sony Corporation)
in the course of development of the MPEG-2 NBC/MPEG-4 System/MPEG-4
Video/MPEG-4 Audio standard ISO/IEC 13818-7, 14496-1,2 and 3. This
software module is an implementation of a part of one or more MPEG-2
NBC/MPEG-4 System/MPEG-4 Video/MPEG-4 Audio tools as specified by the
MPEG-2 NBC/MPEG-4 System/MPEG-4 Video/MPEG-4 Audio standard. ISO/IEC
gives users of the MPEG-2 NBC/MPEG-4 System/MPEG-4 Video/MPEG-4 Audio
standards free license to this software module or modifications
thereof for use in hardware or software products claiming conformance
to the MPEG-2 NBC/MPEG-4 System/MPEG-4 Video/MPEG-4 Audio
standards. Those intending to use this software module in hardware or
software products are advised that this use may infringe existing
patents. The original developer of this software module and his/her
company, the subsequent editors and their companies, and ISO/IEC have
no liability for use of this software module or modifications thereof
in an implementation. Copyright is not released for non MPEG-2
NBC/MPEG-4 System/MPEG-4 Video/MPEG-4 Audio conforming products.The
original developer retains full right to use the code for his/her own
purpose, assign or donate the code to a third party and to inhibit
third party from using the code for non MPEG-2 NBC/MPEG-4
System/MPEG-4 Video/MPEG-4 Audio conforming products. This copyright
notice must be included in all copies or derivative works."
Copyright (C) 1996.
*******************************************************************/

/*
 * This header file is for the gain control tool. The common include files,
 * defines and typedefs are declared.
 */

#ifndef _gc_h_
#define _gc_h_

#ifdef SSR

#define NBANDSBITS		2
#define NATKSBITS		3
#define IDGAINBITS		4
#define GAIN_TABLE_SIZE        16
#define ATKLOCBITS		5
#define ATKLOCBITS_START_A	4
#define ATKLOCBITS_START_B	2
#define ATKLOCBITS_SHORT	2
#define ATKLOCBITS_STOP_A	4
#define ATKLOCBITS_STOP_B	5
#define	NBANDS			4
#define NPQFTAPS		96
#define	NPEPARTS		64	/* Num of PreEcho Inhibition Parts */

#define	SHORT_WIN_IN_LONG	8
#define	GC_BLOCKS_LONG	1
#define	GC_BLOCKS_SHORT	SHORT_WIN_IN_LONG
#define	GC_BLOCKS_START	2
#define	GC_BLOCKS_STOP	2

#include "audiolibs_common.h"
#include "defines.h"
#include "filter.h"
#include "vector_gc.h"
#include "ipqf.h"

#ifdef BIT_FIELDS
typedef struct {
    unsigned int    lev:8;
    unsigned int    loc:8;
}               GAIN_DATA;
#else
typedef struct {
    int             lev;
    int             loc;
}               GAIN_DATA;
#endif /* BIT_FIELDS */

typedef struct {
    int             num_gain_data;
    GAIN_DATA       gData[7];
}               GAIN_INF;

typedef struct {
    GAIN_INF        gBlock[8];
}               GAIN_BLOCK;

/* definition of filterbank control & info structure */

typedef struct {
    int             max_band;
    GAIN_BLOCK      gInfo[4];
}               GAIN_CONTROL_DATA;

typedef struct {
    GAIN_INF        gBlock0;
}               GAIN_BLOCK_PREV;

typedef struct {
    int                  max_band;
    GAIN_BLOCK_PREV      gInfo[4];
}               GAIN_CONTROL_DATA_SAVE;

#define	GC_LFFT	256.0
#define	GC_SFFT	32.0

#ifndef PI
#define		PI		(3.14159265359)
#endif


/* Global data for gc_unpack() */
extern int             aac_ssr_decoder_band;

extern void
#ifdef BUFFER_ALLOCATION_IN_SCRATCH_BUFFER
aac_unpac_gaincontrol_data(void *hBs, int ch, int window,GAIN_CONTROL_DATA *aac_g_info[],Float *aac_scratch_buffer);
#else
aac_unpac_gaincontrol_data(void *hBs,int ch, int window,GAIN_CONTROL_DATA *aac_g_info[]);
#endif
extern void
aac_gc_imdct_sub(Float in_data[],
		 int window_sequence,
		 int wnd_shape,
		 int previous_wnd_shape, int band, Float out_data[], int ch
#ifdef IMDCT_RAMXY
			  	 ,Float YMEM *aac_scratch_buffer_y
#endif

		 );
     
extern void
aac_gc_sort_before_imdct(Float * freqSignalCh, Wnd_Shape * wnd_shape,
			 int window_sequence,
			 Float * timeSignalChWithGCandOverlapping, int ch);
extern int
aac_gc_gainc_window(int, GAIN_INF *, GAIN_BLOCK *, int *, int, int);
extern void
aac_gc_reset_gain_data(GAIN_CONTROL_DATA_SAVE *aac_g_info_prev[]);
extern void
aac_gc_compensate(Float input[], int block_size_samples,
	      int window_sequence, int ch,
	      int band, Float *gcOverlapBuffer,Float out[],
		  GAIN_CONTROL_DATA_SAVE *aac_g_info_prev[],
		  GAIN_CONTROL_DATA *aac_g_info[]
#ifdef BUFFER_ALLOCATION_IN_SCRATCH_BUFFER
		 ,Float *aac_scratch_buffer
#endif
		  );
extern void
aac_gc_ipqf_main(Float * bandSignalCh, int block_size_samples,
		 int ch, Float ipqf_fifo[],int *ipqf_fifo_pos,
		 Float timeSignalCh[]);

extern void     aac_gain_control(Float * freqSigCh,
				 Wnd_Shape * wnd_shape,
				 int window_sequenceCh,
				 int ch,
				 Float * gcOverlapBuffer,
				 Float * ipqf_fifo,
				 int   * ipqf_fifo_pos,
				 Float *imdctOut,
		  		 GAIN_CONTROL_DATA_SAVE *aac_g_info_prev[],
		  		 GAIN_CONTROL_DATA *aac_g_info[]
#ifdef BUFFER_ALLOCATION_IN_SCRATCH_BUFFER
				,Float *aac_scratch_buffer
#endif
#ifdef IMDCT_RAMXY
			  	,Float YMEM *aac_scratch_buffer_y
#endif

				 );
#endif /* SSR */

/*******************************************************************
                        End of gc.h
*******************************************************************/
#endif				/* _gc_h_ */



