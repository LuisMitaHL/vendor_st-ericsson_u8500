/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   diagnostic.c
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/
#include "resample_local.h"
#include "fidelity.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(ARM) || defined(NEON)
#ifndef MMDSP
#define CHECK16BITS
#endif
#else
#ifdef MMDSP
#define USE_MMDSP_INTRINSICS
#endif
#endif




#ifdef USE_MMDSP_INTRINSICS
#define SHIFTMSB 16
#define SHIFTLSB 8
#define SHIFT3B 0
#else
#define SHIFTMSB 24
#define SHIFTLSB 16
#define SHIFT3B 8
#define SATMAX 2147483647
#endif
int ck_for_msec(int Fin,int Fout,int src_quality);
int inptr[RESAMPLE_MAX_CHANNELS*RESAMPLE_BLOCK_SIZE_MAX];
int outptr[6*RESAMPLE_MAX_CHANNELS*RESAMPLE_BLOCK_SIZE_MAX];
#ifdef CHECK16BITS
short inptrs[RESAMPLE_MAX_CHANNELS*RESAMPLE_BLOCK_SIZE_MAX], outptrs[6*RESAMPLE_MAX_CHANNELS*RESAMPLE_BLOCK_SIZE_MAX];
#endif

int main(int argc, char *argv[])
{
#ifdef CHECK16BITS
	int arm_sample16=0;
#endif
    int RESAMPLE_BLOCK_SIZE_LOCAL=32;
    int nch=RESAMPLE_MAX_CHANNELS,siz,flush=0;
    int iter,nbout=0,nbin,Fin,fin,Fout,fout,outdata,roundval;
    int itemp,bits24=0,src_quality=SRC_STANDARD_QUALITY;
    char *heap;
    ResampleContext ctx;
    FILE *f1,*f2;
    unsigned long nb_sample;
    double temp;
    int freq_lookup[ESAA_FREQ_LAST_IN_LIST];
 
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
    if (argc<5)
    {
#ifdef CHECK16BITS
        printf("\n\nuse: SrcMultiChannel INFILE OUTFILE freqin freqout [[src_quality] arm_sample16]\n");
#else
        printf("\n\nuse: SrcMultiChannel INFILE OUTFILE freqin freqout [src_quality] \n");
#endif
        printf("-------------------------------------------------------------------------------------------------\n");
        printf("Run the multi channel sample rate converter\n\n");
        printf("where:\n");
        printf("        INFILE= 8-channel interleaved 16-bit input binary file name\n");
        printf("        OUTFILE=8-channel interleaved 16-bit output binary file name\n");
        printf("        freqin=input sampling frequency in kHz\n");
        printf("        freqout=output sampling frequency in kHz\n");
#if defined(hifi) || defined(hifi_locoefs)
        printf("        src_quality=N,Y,L,l,H for STANDARD(def),LOW_RIPPLE,LOW_LATENCY,LOW_LATENCY_IN_MSEC,Hifi resp.\n");
#else
        printf("        src_quality=N,Y,L,l for STANDARD(def),LOW_RIPPLE,LOW_LATENCY,LOW_LATENCY_IN_MSEC\n");
#endif
#ifdef CHECK16BITS
        printf("        arm_sample16 (def N) for ARM target, samples coming from framework are on 16 bits, not 32\n");
#endif
        exit(1);
    }
    temp=atof(argv[3]);
    Fin=(int)temp;
    temp=atof(argv[4]);
    if (argc>5)
    {
        if (argv[5][0]=='N') src_quality=SRC_STANDARD_QUALITY;
        else if (argv[5][0]=='Y') src_quality=SRC_LOW_RIPPLE;
        else if (argv[5][0]=='L') src_quality=SRC_LOW_LATENCY;
        else if (argv[5][0]=='l') src_quality=SRC_LOW_LATENCY_IN_MSEC;
#if defined(hifi) || defined(hifi_locoefs)
        else if (argv[5][0]=='H')
        {
            src_quality=SRC_HIFI;
            bits24=1;
        }
#endif
    }
#ifdef CHECK16BITS
	if ((argc>6)&&(argv[6][0]!='N'))
	{
		arm_sample16=1;
	}
#endif
    Fout=(int)temp;
    fin=0;
    fout=0;
    for (iter=0;iter<ESAA_FREQ_LAST_IN_LIST;iter++)
    {
        if (freq_lookup[iter]==Fin)
        {
            fin=iter;
        }
        if (freq_lookup[iter]==Fout)
        {
            fout=iter;
        }
    }
    if ((fin==0)||(fout==0))
    {
        printf("******** Invalid Parameter **********\n");
        exit(1);
    }
    if ((f1=fopen(argv[1],"rb"))==0)
    {
        printf("\n\n********** BAD INPUT FILE NAME  **********\n\n");
        exit(1);
    }
    f2=fopen(argv[2],"wb");
    if (ck_for_msec(Fin,Fout,src_quality)) RESAMPLE_BLOCK_SIZE_LOCAL=48;
    if (resample_calc_heap_size_fixin_fixout    (fin,fout,src_quality,&siz,RESAMPLE_MAX_CHANNELS*RESAMPLE_BLOCK_SIZE_LOCAL,RESAMPLE_MAX_CHANNELS))
	{
		printf(" \n\n\n@@@@@@@@@@@@ ohoh calc_heap_size problem @@@@@@@@@@@@@@@ \n\n\n");
		exit(1);
	}
    heap=(char *)malloc(siz);
#ifdef CHECK16BITS
	if (arm_sample16)
	{
    	if( resample_x_init_ctx_low_mips_fixin_fixout_sample16((char*)heap,siz ,&ctx,fin,fout,src_quality,RESAMPLE_MAX_CHANNELS,RESAMPLE_MAX_CHANNELS*RESAMPLE_BLOCK_SIZE_LOCAL) ) 
		{
			printf(" \n\n\n@@@@@@@@@@@@ ohoh init problem @@@@@@@@@@@@@@@ \n\n\n");
        	free(heap);
        	exit(1);
    	}
	}
	else
#endif
	{
    	if( resample_x_init_ctx_low_mips_fixin_fixout((char*)heap,siz ,&ctx,fin,fout,src_quality,RESAMPLE_MAX_CHANNELS,RESAMPLE_MAX_CHANNELS*RESAMPLE_BLOCK_SIZE_LOCAL) ) {
			printf(" \n\n\n@@@@@@@@@@@@ ohoh init problem @@@@@@@@@@@@@@@ \n\n\n");
        	free(heap);
        	exit(1);
    	}
	}
    nbin=RESAMPLE_BLOCK_SIZE_LOCAL;
#ifdef USE_MMDSP_INTRINSICS
    roundval=0x00000080;
#else
    roundval=0x00008000;
#endif
    /* get input file size*/
    fseek(f1, 0, SEEK_END);
    if (bits24==0)
        nb_sample = ftell(f1) / 2;
    else
        nb_sample = ftell(f1) / 3;
    rewind(f1);
    while (1)
    {
        if (nbin!=0)
        {
            for (iter=0;iter<nbin*nch;iter++)
            {
                unsigned int tmp;
                if (nb_sample!=0)
                {
                    tmp=fgetc(f1);
#ifdef USE_MMDSP_INTRINSICS
                    itemp = wmsl(wsgn8((int)tmp),16);
                    tmp=fgetc(f1);
                    itemp += (tmp<<8);
                    if (bits24!=0)
                    {
                        tmp=fgetc(f1);
                        itemp += (tmp);
                    }
#else
                    itemp = tmp<<24;
                    tmp=fgetc(f1);
                    itemp += (tmp<<16);
                    if (bits24!=0)
                    {
                        tmp=fgetc(f1);
                        itemp += (tmp<<8);
                    }
#endif
                    nb_sample--;
                    inptr[iter]=((int)itemp);
#ifdef CHECK16BITS
					if (arm_sample16)
					{
						inptrs[iter]=(short)((inptr[iter])>>16);
					}
#endif
                }
                else
                {
                    break;
                }
            }
        }
        if (nb_sample==0) break;
        nbin=RESAMPLE_BLOCK_SIZE_LOCAL;
        nbout=RESAMPLE_BLOCK_SIZE_LOCAL;
		if (ck_for_msec(Fin,Fout,src_quality))
		{
			nbin/=Fin;
			nbout/=Fout;
		}
#ifdef CHECK16BITS
		if (arm_sample16) 
		{
			if((*ctx.processing) (&ctx, (int*)inptrs, nch, &nbin, (int*)outptrs, &nbout, &flush) != 0)
			{
				printf("Error in SRC processing routine\n");
				exit(1);
			}
		}
		else
#endif
		{
			if((*ctx.processing) (&ctx, inptr, nch, &nbin, outptr, &nbout, &flush) != 0)
			{
				printf("Error in SRC processing routine\n");
				exit(1);
			}
		}
		if (ck_for_msec(Fin,Fout,src_quality))
		{
			nbin*=Fin;
			nbout*=Fout;
		}
        if (nbout!=0)
        {
            for (iter=0;iter<nbout*nch;iter++)
            {
                char byte;
                outdata=outptr[iter];
#ifdef CHECK16BITS
				if (arm_sample16) outdata=outptrs[iter]<<16;
				if ((bits24==0)&&(arm_sample16==0))
#else
                if (bits24==0)
#endif
                {
#ifndef USE_MMDSP_INTRINSICS
                    if (outdata<(SATMAX-roundval)) outdata+=roundval;
#else
                    outdata=waddsat(outdata,roundval);
#endif
                }
                byte=(char)(outdata>>SHIFTMSB);
                putc(byte,f2);
                byte=(char)(outdata>>SHIFTLSB);
                putc(byte,f2);
                if (bits24!=0)
                {
                    byte=(char)(outdata>>SHIFT3B);
                    putc(byte,f2);
                }
            }
        }
    }
    fclose(f1);
    fclose(f2);
    free(heap);

    return 0;
} /* end main() */
int ck_for_msec(int Fin,int Fout,int src_quality)
{
    int temp=0;
    if (src_quality==SRC_LOW_LATENCY_IN_MSEC)
    {
        if      ((Fin==48)&&(Fout==8)) temp=1;
        else if ((Fin==48)&&(Fout==16)) temp=1;
        else if ((Fin==16)&&(Fout==8)) temp=1;
        else if ((Fin==8)&&(Fout==48)) temp=1;
        else if ((Fin==16)&&(Fout==48)) temp=1;
        else if ((Fin==8)&&(Fout==16)) temp=1;
    }
    return temp;
}







