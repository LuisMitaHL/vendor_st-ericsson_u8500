/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   get_maxheapsz.c
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
#include "audiolibs_common.h"
#include "resample_local.h"
#include <stdio.h>
#include <stdlib.h>
#define MAX_FREQ_IN ESAA_FREQ_96KHZ
#define MAX_FREQ_OUT ESAA_FREQ_48KHZ
#define MIN_FREQ ESAA_FREQ_7_2KHZ

int main(void)
{
/*--------------------------- Calc max heapsiz for upsampling and downsampling -------------*/
	int siz,fin,fout;
	int freq_lookup[17];
	int max_sizu,max_sizd,min_sizu,min_sizd,max_finu,min_finu,max_find,min_find,max_foutu,min_foutu,max_foutd,min_foutd;
	freq_lookup[ESAA_FREQ_UNKNOWNKHZ]=0;
	freq_lookup[ESAA_FREQ_192KHZ]=0;
	freq_lookup[ESAA_FREQ_176_4KHZ]=0;
	freq_lookup[ESAA_FREQ_128KHZ]=0;
	freq_lookup[ESAA_FREQ_96KHZ]=96;
	freq_lookup[ESAA_FREQ_88_2KHZ]=88;
	freq_lookup[ESAA_FREQ_64KHZ]=64;
	freq_lookup[ESAA_FREQ_48KHZ]=48;
	freq_lookup[ESAA_FREQ_44_1KHZ]=44;
	freq_lookup[ESAA_FREQ_32KHZ]=32;
	freq_lookup[ESAA_FREQ_24KHZ]=24;
	freq_lookup[ESAA_FREQ_22_05KHZ]=22;
	freq_lookup[ESAA_FREQ_16KHZ]=16;
	freq_lookup[ESAA_FREQ_12KHZ]=12;
	freq_lookup[ESAA_FREQ_11_025KHZ]=11;
	freq_lookup[ESAA_FREQ_8KHZ]=8;
	freq_lookup[ESAA_FREQ_7_2KHZ]=7;
	max_finu=0;
	min_finu=0;
	max_foutu=0;
	min_foutu=0;
	max_sizu=0;
	min_sizu=65536;
	max_find=0;
	min_sizu=65536;
	max_foutd=0;
	max_sizd=0;
	for (fin=MIN_FREQ;fin>=MAX_FREQ_IN;fin--)
	{
		for (fout=MIN_FREQ;fout>=MAX_FREQ_OUT;fout--)
		{
			if (resample_calc_heap_size_fixin_fixout(fin,fout,0,&siz,RESAMPLE_BLOCK_SIZE,1)==0)
			{
				if (freq_lookup[fin]<freq_lookup[fout])
				{
					if (siz>max_sizu) 
					{
						max_sizu=siz;
						max_finu=fin;
						max_foutu=fout;
					}
					if (siz<min_sizu) 
					{
						min_sizu=siz;
						min_finu=fin;
						min_foutu=fout;
					}
				}
				else if (freq_lookup[fin]>freq_lookup[fout])
				{
					if (siz>max_sizd) 
					{
						max_sizd=siz;
						max_find=fin;
						max_foutd=fout;
					}
					if (siz<min_sizd) 
					{
						min_sizd=siz;
						min_find=fin;
						min_foutd=fout;
					}

				}
			}
		}
	}
	printf("frequencies for upsampling max heap size: %d fin: %d kHz fout: %d kHz\n",
			max_sizu, freq_lookup[max_finu],freq_lookup[max_foutu]);
	printf("frequencies for dnsampling max heap size: %d fin: %d kHz fout: %d kHz\n",
			max_sizd,freq_lookup[max_find],freq_lookup[max_foutd]);
	printf("frequencies for upsampling min heap size: %d fin: %d kHz fout: %d kHz\n",
			min_sizu, freq_lookup[min_finu],freq_lookup[min_foutu]);
	printf("frequencies for dnsampling min heap size: %d fin: %d kHz fout: %d kHz\n",
			min_sizd,freq_lookup[min_find],freq_lookup[min_foutd]);
	printf("-----------------------------------------------------------------------\n");
/*
resample_calc_max_heap_size_fixin_fixout(Inrate,Outrate,src_quality,*heapsiz,blocksiz,channel_nb,conversion_type)
conversion_type: 0 unknown
				 1 up
				 2 down
if Inrate or Outrate is 0 then it is unknown
*/	
	resample_calc_max_heap_size_fixin_fixout(0,max_foutu,0,&siz,RESAMPLE_BLOCK_SIZE,1,1);
	printf("heap size from calc_max_heap_size upsampling fout=%d kHz: %d\n",freq_lookup[max_foutu],siz);
	resample_calc_max_heap_size_fixin_fixout(0,max_foutd,0,&siz,RESAMPLE_BLOCK_SIZE,1,2);
	printf("heap size from calc_max_heap_size downsampling fout=%d kHz: %d\n",freq_lookup[max_foutd],siz);
	resample_calc_max_heap_size_fixin_fixout(0,0,0,&siz,RESAMPLE_BLOCK_SIZE,1,0);
	printf("heap size from calc_max_heap_size conversion type unknown, frequencies unknown: %d\n",siz);
	resample_calc_max_heap_size_fixin_fixout(0,0,0,&siz,RESAMPLE_BLOCK_SIZE,1,1);
	printf("heap size from calc_max_heap_size conversion type upsampling, frequencies unknown: %d\n",siz);
	resample_calc_max_heap_size_fixin_fixout(0,0,0,&siz,RESAMPLE_BLOCK_SIZE,1,2);
	printf("heap size from calc_max_heap_size conversion type downsampling, frequencies unknown: %d\n",siz);
	resample_calc_max_heap_size_fixin_fixout(max_finu,0,0,&siz,RESAMPLE_BLOCK_SIZE,1,1);
	printf("heap size from calc_max_heap_size conversion type upsampling, in freq at maxup, out frequency unknown: %d\n",siz);
	resample_calc_max_heap_size_fixin_fixout(max_find,0,0,&siz,RESAMPLE_BLOCK_SIZE,1,2);
	printf("heap size from calc_max_heap_size conversion type downsampling, in freq at maxdn, out frequency unknown: %d\n",siz);



/*-----------------------------------------------------------------------------------------------------*/
	return 0;
} /* end main() */

