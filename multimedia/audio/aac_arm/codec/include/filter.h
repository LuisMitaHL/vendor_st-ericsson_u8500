#ifndef _filter_h_
#define _filter_h_

#include "audiolibs_common.h"
#include "imdct.h"
#include "mdct.h"
#include "mdct_tables.h"
#include "audiowindows.h"
#include "defines.h"


#include "vector.h"

#define		npow2(x)	(1L << (x))	/* 2^x */


void
aac_unfold_state_buffer(Float *overlap, 
						Float *out,
						int previous_wnd_shape, int previous_wnd_sequence); 

void
aac_save_state_buffer(Float *overlap, Float *in, int wnd_sequence, 
					  int wnd_shape);
void
aac_perform_overlap(Float *in, 
					Float *out,
					int previous_wnd_shape, int wnd_shape, int wnd_sequence); 


void
aac_local_imdct_sub(Float in_data[], int window_sequence, Float out_data[]
#ifdef IMDCT_RAMXY
					,Float  YMEM *aac_scratch_buffer_y
#endif
				   );



void
aac_overlap_add(
		Float input[],
		int block_size_samples,
		int window_sequence,
		Float * OverlapBuffer,
		Float out[]);

  

#ifdef ARM

void lib_DCTIV(int* out_fft, int* rgiCoef,  int cSBn,  int last_stage_shift, int zi);
extern void aac_imdct_sub(int fi_shz1, Float *in_data,
				  int window_sequence,
				  int wnd_shape, int previous_wnd_shape, Float *out,
                  int z1
);

extern void
aac_st_freq2time_compress_state(UCHAR blockType,
					   Wnd_Shape * wnd_shape,
					   UCHAR * prev_sequence,
					   int *ifreqInPtr,int *statePtr,
					   int *tmp
					   );

extern void aac_st_time2freq_adapt (
								 UCHAR window_sequence,
								 Wnd_Shape *wnd_shape,			                              
								 int *in_time_data, 		
								 int *out_freq_data, 
								 int last_stage_shift
								 );

extern void	aac_st_freq2time_adapt(	UCHAR blockType,
					   Wnd_Shape * wnd_shape,
					   UCHAR * prev_sequence,
					   int *ifreqInPtr,int *statePtr,
					   int *tmp
				);	

void
vector_set_value_neon(int *p, int len, int value);

#else /* ARM */

void
aac_imdct_sub(Float in_data[],
	      int window_sequence,
	      int wnd_shape,
	      int previous_wnd_shape,
	      Float out_data[]
#ifdef IMDCT_RAMXY
		 ,Float YMEM *aac_scratch_buffer_y
#endif
);

extern void
aac_st_freq2time_compress_state(
		       UCHAR blockType,
		       Wnd_Shape * wnd_shape,
		       UCHAR *prev_sequence,
		       Float * freqInPtr,
		       Float * statePtr,
		       Float *tmp
#ifdef IMDCT_RAMXY
		 	  ,Float YMEM *aac_scratch_buffer_y
#endif
			   );

void
aac_st_freq2time_adapt(
		       UCHAR blockType,
		       Wnd_Shape * wnd_shape,
		       UCHAR *prev_sequence,
		       Float * freqInPtr,
		       Float * statePtr,
		       Float *tmp
#ifdef IMDCT_RAMXY
		 	  ,Float YMEM *aac_scratch_buffer_y
#endif
			   ); 

void 
aac_st_time2freq_adapt (
			   UCHAR window_sequence,
			   Wnd_Shape *wnd_shape,			
			   Float *in_time_data, 		
			   Float *out_freq_data,
			   int   *out_exp,
			   Float *fft_buff_real,
			   Float YMEM *fft_buff_imag_y
			);
#endif

		 

#endif /* Do not edit below this point */
