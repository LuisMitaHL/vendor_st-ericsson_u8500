/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   audiotest.c
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "audiolibs_common.h"
#include "resample_local.h"

char heap[RESAMPLE_HEAP_SIZE];
ResampleContext ctx;

FILE *in;
FILE *out;

int
main(int argc, char *argv[])
{
	int freq;
	int nb_ch;
	Float fval;
	Float *fval_out[2];
	Float tab1[6];
	Float tab2[6];
	int  nb_out;
	int  i;
	int j;
	int  val;

	if( argc != 5 ) {
		printf("usage : %s filein fileout freqin nbch\n", argv[0]);
		return 1;
	}

	in = fopen(argv[1],"rb");
	if(in == NULL ) {
		printf("could not open file  %s\n",argv[1]);
	}
	
	out = fopen(argv[2],"wb");
	if(out == NULL ) {
		printf("could not open file  %s\n",argv[2]);
	}

	freq = atoi(argv[3]);
	switch(freq) {
	case 8000:
		freq = ESAA_FREQ_8KHZ;
		break;
	case 11025:
		freq = ESAA_FREQ_11_025KHZ;
		break;
	case 12000:
		freq = ESAA_FREQ_12KHZ;
		break;
	case 16000:
		freq = ESAA_FREQ_16KHZ;
		break;
	case 22050:
		freq = ESAA_FREQ_22_05KHZ;
		break;
	case 24000:
		freq = ESAA_FREQ_24KHZ;
		break;
	case 32000:
		freq = ESAA_FREQ_32KHZ;
		break;
	case 48000:
		freq = ESAA_FREQ_48KHZ;
		break;
	default:
		printf("frequency %d not supported \n",freq);
		return 1;
	}

	nb_ch = atoi(argv[4]);
	if( nb_ch != 1 && nb_ch != 2) {
		printf("num channels supported is 1 or 2 \n");
		return 1;
	}

	fval_out[0] = tab1;
	fval_out[1] = tab2;
	
	/* allocate heap */
	if( resample_44_init_context_medium((char*)&heap, RESAMPLE_HEAP_SIZE, &ctx, freq, RESAMPLE_MAX_CHANNELS) ) {
	//if( resample_44_init_context_low_mips((char*)&heap, RESAMPLE_HEAP_SIZE, &ctx, freq, RESAMPLE_MAX_CHANNELS) ) {
		printf(" ohoh init problem \n");
		return 1;
	}
	
	while( feof(in) == 0 ) {

		for(i=0;i<nb_ch;i++) {
			/* read sample */
			val = fgetc(in);
			val <<=8;
			val |= fgetc(in);

			/* get sign extension */
#ifndef __flexcc2__
			val = val << 16;
			val = val >> 16;
#else
			val = val << 8;
			val = val >> 8;
#endif
			
#ifdef MMDSP
			/* scale to 24 bits */
			val <<=8;
			fval = val;
#else
			fval = (float)val/32768.0;
#endif

			resample_44_process(&ctx,
					 freq,
					 &fval,
					 1,
					 1,
					 i,
					 fval_out[i],
					 &nb_out);
		}

		
		/* write output */
		for(j=0;j<nb_out;j++) {

			for(i=0;i<nb_ch;i++) {
				
#ifdef MMDSP				
				val = fval_out[i][j];
				val = waddsat(val, 0x80);
				val = wmsr(val,8);

#else
				if( fval_out[i][j] < 0 ) {
					fval = floor( fval_out[i][j]*32768.0 - 0.5);
					if( fval < -32768 ) {
						printf("negative clip \n");
						fval = -32768;
					}
					val = (int)fval;
				} else {
					fval = floor( fval_out[i][j]*32768.0 + 0.5);
					if( fval > 32767 ) {
						printf("positive clip \n");
						fval = 32767;
					}
					val = (int)fval;
				}
#endif
				
				fputc((val>>8)&0xff, out);
				fputc((val&0xff), out);
			} /* end loop channels */
		} /* end loop nb_out */
	}
	fclose(in); fclose(out);
	return 0;
} /* end main */

