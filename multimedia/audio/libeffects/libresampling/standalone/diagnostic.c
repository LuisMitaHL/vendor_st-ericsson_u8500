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
#ifdef ARM
//#define TESTING //set to run without cmdline arguments for rvdebug
//#define MEASURE_CYCLES
#endif
//#define ARM_CYCLES //for using PMU cycle evaluations
//#define ARM_BOARDTEST_PROFILE //for using profile measuring execution in usec

#ifdef ARM_CYCLES
#include <pmu_api.h>
#endif
#ifdef ARM_BOARDTEST_PROFILE
#include "Profile.h"
#endif


#include "resample_local.h"

#include <stdio.h>
#include <stdlib.h>
#ifndef M4_LL_MSEC
#define CHECK_CPY_CTX
#endif
#if defined(ARM) || defined(NEON)
#ifndef MMDSP
#define CHECK16BITS
#endif
#else
#ifdef MMDSP
#define USE_MMDSP_INTRINSICS
#endif
#endif
#ifdef ARM_BOARDTEST_PROFILE
#define CHECK16BITS
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
#ifdef TESTING

//FILE *db;
#ifdef CHECK16BITS
void setargs(char* inp1, char* inp2, char* inp3, char* inp4, char* inp5, char* inp6, char* inp7, char* inp8, char* inp9)
#else
void setargs(char* inp1, char* inp2, char* inp3, char* inp4, char* inp5, char* inp6, char* inp7, char*inp8)
#endif
{
    FILE* inpfile;
    
    inpfile=fopen("args.txt","r+");
    if(inpfile == NULL){
        printf("\n cannot open file from setargs\n");
        exit(0);
    }
    fscanf(inpfile, "%s\n", inp1);
    fscanf(inpfile, "%s\n", inp2);
    fscanf(inpfile, "%s\n", inp3);
    fscanf(inpfile, "%s", inp4);
    fscanf(inpfile, "%s", inp5);
    fscanf(inpfile, "%s", inp6);
    fscanf(inpfile, "%s", inp7);
    fscanf(inpfile, "%s", inp8);
#ifdef CHECK16BITS
    fscanf(inpfile, "%s", inp9);
#endif
//	printf("command line: %s %s %s %s %s %s %s %s %s\n",inp1,inp2,inp3,inp4,inp5,inp6,inp7,inp8,inp9);
    fclose(inpfile);
}
#endif

int ck_for_msec(int Fin,int Fout,int src_quality);

int main(int argc, char *argv[])
{
	int i;
#ifdef CHECK16BITS
	int arm_sample16=0;
#endif
    int RESAMPLE_BLOCK_SIZE_LOCAL=32;
#ifdef CHECK_CPY_CTX
    int half_size,ctx_copied=0;
#endif
#ifdef __flexcc2__
    unsigned long input_count,output_count,total_count=0L;
#endif
    int siz,flush,flush_in=0,flush_out;
    int   iter,nbout,nbin,Fin,fin,Fout,fout,outdata,roundval,src_quality;
    int itemp,bits24=0;
    int inptr[RESAMPLE_BLOCK_SIZE_MAX], outptr[6*RESAMPLE_BLOCK_SIZE_MAX];
#ifdef CHECK16BITS
    short inptrs[RESAMPLE_BLOCK_SIZE_MAX], outptrs[6*RESAMPLE_BLOCK_SIZE_MAX];
#endif
//#ifdef CHECK16BITS
//	short arm16_samples[6*RESAMPLE_BLOCK_SIZE_MAX];
//#endif
    char *heap;
    ResampleContext ctx;
    FILE *f1,*f2;
    unsigned long nb_sample;
    double temp;
#ifdef MEASURE_CYCLES
    int cortex_cycles=0;
    unsigned long total_cortex_cycles=0L;
    int cortex_count=0;
#endif
#ifdef ARM_BOARDTEST_PROFILE
	int timeval,nblocks_tested=1000000;
	float frq,MIPS,MHZclockfreq=800.;
#endif
#ifdef TESTING
    char myargv1[20];
    char myargv2[20];
    char myargv3[20];
    char myargv4[20];
    char myargv5[20];
    char myargv6[20];
	char myargv7[20];
	char myargv8[20];
	char myargv9[20];
#endif

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
    src_quality=SRC_STANDARD_QUALITY;

#ifdef TESTING
//  db = fopen("dump.txt", "w+");
#ifdef CHECK16BITS
	argc = 10;
    setargs(myargv1, myargv2 , myargv3, myargv4, myargv5, myargv6, myargv7, myargv8,myargv9);
#else
	argc = 9;
    setargs(myargv1, myargv2 , myargv3, myargv4, myargv5, myargv6, myargv7,myargv8);
#endif
#endif

    if (argc<5)
    {
#ifdef CHECK16BITS
        printf("\n\nuse: diagnostic INFILE OUTFILE freqin freqout [src_quality] [verbose] [flush_eof] [24_bit_samples] [arm_sample16]\n");
#else
        printf("\n\nuse: diagnostic INFILE OUTFILE freqin freqout [src_quality] [verbose] [flush_eof] [24_bit_samples]\n");
#endif
        printf("-------------------------------------------------------------------------------------------------\n");
        printf("Run the sample rate converter\n\n");
        printf("where:\n");
        printf("        INFILE=mono 16-bit input binary file name\n");
        printf("        OUTFILE=mono 16-bit output binary file name\n");
        printf("        freqin=input sampling frequency in kHz\n");
        printf("        freqout=output sampling frequency in kHz\n");
#if defined(hifi) || defined(hifi_locoefs)
        printf("        src_quality=H (def N) for hifi quality (thd=120dB) \n");
#endif
        printf("        src_quality=Y (def N) for special low ripple \n");
        printf("        src_quality=L (def N) for special lo latency cases \n");
        printf("        src_quality=l (def N) for special lo latency cases with input/output smpl quantities in msec\n");
        printf("        verbose=N (def Y) for no printout of conversion progress \n");
        printf("        flush_eof=Y (def N) flush src after end of file detected\n");
        printf("        24_bit_samples (def N) input and output files have 24-bit(Y) or 16-bit samples(N)\n");
#ifdef CHECK16BITS
        printf("        arm_sample16 (def N) for ARM target, samples coming from framework are on 16 bits, not 32\n");
#endif
        exit(1);
    }
#ifdef TESTING
    temp=atof(myargv3);
#else
    temp=atof(argv[3]);
#endif
    Fin=(int)temp;
#ifdef TESTING
    temp=atof(myargv4);
#else
    temp=atof(argv[4]);
#endif
    if (argc>=6)
    {
#ifdef TESTING
        if (myargv5[0]=='Y')
#else
        if (argv[5][0]=='Y')
#endif
            src_quality=SRC_LOW_RIPPLE;
#ifdef TESTING
        if (myargv5[0]=='L')
#else
        if (argv[5][0]=='L')
#endif
            src_quality=SRC_LOW_LATENCY;
#ifdef TESTING
        if (myargv5[0]=='l')
#else
        if (argv[5][0]=='l')
#endif
            src_quality=SRC_LOW_LATENCY_IN_MSEC;
#if defined(hifi) || defined(hifi_locoefs)
#ifdef TESTING
        if (myargv5[0]=='H')
#else
        if (argv[5][0]=='H')
#endif
            src_quality=SRC_HIFI;
#endif



    }
#ifdef TESTING
    if ((argc>=9)&&(myargv8[0]!='N')) bits24=1;
#else
    if ((argc>=9)&&(argv[8][0]!='N')) bits24=1;
#endif
#ifdef TESTING
    if ((argc>=8)&&(myargv7[0]!='N')) bits24=1;
#else
    if ((argc>=8)&&(argv[7][0]!='N')) bits24=1;
#endif
#ifdef CHECK16BITS
//	if (bits24==0)
//	{
#ifdef TESTING
    if ((argc>=10)&&(myargv9[0]!='N')) arm_sample16=1;
#else
	if ((argc>=10)&&(argv[9][0]!='N')) arm_sample16=1;
#endif
//	}
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
#ifdef TESTING
    if ((f1=fopen(myargv1,"rb"))==0)
#else
    if ((f1=fopen(argv[1],"rb"))==0)
#endif
    {
        printf("\n\n********** BAD INPUT FILE NAME  **********\n\n");
        exit(1);
    }
#ifdef TESTING
    f2=fopen(myargv2,"wb");
#else
    f2=fopen(argv[2],"wb");
#endif
    if (ck_for_msec(Fin,Fout,src_quality))
    {
        RESAMPLE_BLOCK_SIZE_LOCAL=48;
    }
//  resample_calc_max_heap_size_fixin_fixout(0,     0,src_quality,&siz,RESAMPLE_MAX_CHANNELS*RESAMPLE_BLOCK_SIZE_LOCAL,RESAMPLE_MAX_CHANNELS,0/*conversion_type*/);
//  printf("max_heapsiz: %d\n",siz);
    if (resample_calc_heap_size_fixin_fixout    (fin,fout,src_quality,&siz,RESAMPLE_MAX_CHANNELS*RESAMPLE_BLOCK_SIZE_LOCAL,RESAMPLE_MAX_CHANNELS))
	{
        	printf(" \n\n\n@@@@@@@@@@@@ ohoh calc_heap_size problem @@@@@@@@@@@@@@@ \n\n\n");
			exit(1);
	}
//  printf("actual_heapsiz: %d\n",siz);
    heap=(char *)malloc(siz);
#ifdef CHECK16BITS
	if (arm_sample16)
	{
    	if( resample_x_init_ctx_low_mips_fixin_fixout_sample16((char*)heap,siz ,&ctx,fin,fout,src_quality,RESAMPLE_MAX_CHANNELS,RESAMPLE_MAX_CHANNELS*RESAMPLE_BLOCK_SIZE_LOCAL) ) 
		{
        	printf(" \n\n\n@@@@@@@@@@@@ ohoh init problem @@@@@@@@@@@@@@ \n\n\n");
        	free(heap);
        	exit(1);
    	}
	}
	else
#endif
	{
    	if( resample_x_init_ctx_low_mips_fixin_fixout((char*)heap,siz ,&ctx,fin,fout,src_quality,RESAMPLE_MAX_CHANNELS,RESAMPLE_MAX_CHANNELS*RESAMPLE_BLOCK_SIZE_LOCAL) ) 
		{
        	printf(" \n\n\n@@@@@@@@@@ ohoh init problem @@@@@@@@@@@@@@@@ \n\n\n");
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
#ifdef MEASURE_CYCLES
    nb_sample=256;
#endif
#ifdef CHECK_CPY_CTX
    half_size = nb_sample/2;
#endif
    rewind(f1);
#ifdef ARM_BOARDTEST_PROFILE
//	printf("Init perf...\n");
    init_perf(1);
#endif
#ifdef ARM_CYCLES
	PMU_init(PMU_INSTRUCTIONS,
			PMU_DATA_MISS,
			PMU_DATA_PENALTY,
			PMU_INST_MISS,
			PMU_INST_PENALTY,
			PMU_BRANCH_MISS
			);
#endif
#ifndef ARM_BOARDTEST_PROFILE //else execute only once
    while (1)
#endif
    {
        if (nbin!=0)
        {
            for (iter=0;iter<nbin;iter++)
            {
                unsigned int tmp;
#ifdef TESTING
                if ((argc<7)||((argc>=7)&&(myargv6[0]!='N')))
#else
                if ((argc<7)||((argc>=7)&&(argv[6][0]!='N')))
#endif
                    printf("end in %6u samples\r",(unsigned int)nb_sample);
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
                }
                else
                {
                    flush_in=1;
                    itemp=0;
                }
                inptr[iter]=((int)itemp);
            }
#ifdef CHECK16BITS
			if (arm_sample16)
			{
				for (i=0;i<nbin;i++) inptrs[i]=(short)((inptr[i])>>16);
			}
#endif
        }
#ifndef ARM_BOARDTEST_PROFILE
#ifdef TESTING
        if (((argc<8)||(myargv7[0]=='N'))&&(nb_sample==0))
#else
        if (((argc<8)||(argv[7][0]=='N'))&&(nb_sample==0))
#endif
        {
            break;
        }
#endif
        nbin=RESAMPLE_BLOCK_SIZE_LOCAL;
        nbout=RESAMPLE_BLOCK_SIZE_LOCAL;
        flush=flush_in;


#ifdef CHECK_CPY_CTX
#ifdef TESTING
        if ((argc<8)||(myargv7[0]=='N')) // only do the context copy test if we're not testing the flush test
#else
        if ((argc<8)||(argv[7][0]=='N'))
#endif
        {
            if (nb_sample<half_size)
            {
                if (ctx_copied==0)
                {
                    resample_x_cpy_0_1_ctx_low_mips_fixin_fixout(&ctx,fin,fout,src_quality,RESAMPLE_MAX_CHANNELS*RESAMPLE_BLOCK_SIZE_LOCAL);
                    ctx_copied=1;
                }
            }
        }
#endif //ifdef CHECK_CPY_CTX



#ifdef __flexcc2__
        input_count=get_cycle_count();
#endif
        //      startperf();
#ifdef MEASURE_CYCLES
        PerfEnableDisable(0);
        PerfSetTickRate(0);
        PerfSelectEvent(1,0x68);//Instr exec
        PerfSelectEvent(2,0x03);//DMiss
        PerfSelectEvent(3,0x01);//IMiss
        PerfEnableDisable(1);
        PerfReset(0);
        PerfReset(1);
#endif
        if (ck_for_msec(Fin,Fout,src_quality))
        {
            nbin/=Fin;
            nbout/=Fout;
        }
#ifdef ARM_CYCLES
		PMU_tap(0);
#endif
#ifdef ARM_BOARDTEST_PROFILE
		start_perf(0);
		for (iter=0;iter<nblocks_tested;iter++) //execute 1000 times to minimize Profile time measurement overhead
		{
			nbin=RESAMPLE_BLOCK_SIZE_LOCAL;
			nbout=RESAMPLE_BLOCK_SIZE_LOCAL;
			flush=0;
#endif
//---------------------- call to SRC library is here ---------------------------------
#ifdef CHECK16BITS
			if (arm_sample16) 
			{
        		if((*ctx.processing) (&ctx, (int*)inptrs, 1, &nbin, (int*)outptrs, &nbout, &flush) != 0)
        		{
            		printf("Error in SRC processing routine\n");
            		exit(1);
        		}
			}
			else
#endif
			{
        		if((*ctx.processing) (&ctx, inptr, 1, &nbin, outptr, &nbout, &flush) != 0)
        		{
            		printf("Error in SRC processing routine\n");
            		exit(1);
        		}
			}
//-------------------------------------------------------------------------------------
#ifdef ARM_BOARDTEST_PROFILE
		}
       	end_perf(0);
#endif
#ifdef ARM_CYCLES
		PMU_tap(1);
		PMU_verbose(1,0);
#endif

//convert from msec for this mode
        if (ck_for_msec(Fin,Fout,src_quality))
        {
            nbin*=Fin;
            nbout*=Fout;
        }
#ifdef MEASURE_CYCLES
        cortex_cycles=PerfGetCount(0xff);
        total_cortex_cycles+=cortex_cycles;
        cortex_count++;
        printf("\n Cycles : %d \n", cortex_cycles);
        printf("\n Instructions : %d \n", PerfGetCount(1));
        printf("\n D Miss : %d \n", PerfGetCount(2));
        printf("\n I Miss : %d \n", PerfGetCount(3));
#endif
//      endperf();
#ifdef __flexcc2__
        output_count=get_cycle_count();
        total_count+=(output_count-input_count);
#endif
        flush_out=flush;
#ifndef ARM_BOARDTEST_PROFILE
        if (flush_out!=0) break;
#endif
        if (nbout!=0)
        {
#ifdef CHECK16BITS
			if ((bits24==0)&&(arm_sample16==0))
#else
			if (bits24==0)
#endif
			{
				for (i=0;i<nbout;i++)
				{
#ifndef USE_MMDSP_INTRINSICS
					if (outptr[i]<(SATMAX-roundval)) outptr[i]+=roundval;
#else
					outptr[i]=waddsat(outptr[i],roundval);
#endif
				}
			}
#ifdef CHECK16BITS
			if (arm_sample16) 
			{
				for (i=0;i<nbout;i++)
				{
					outptr[i]=((int)(outptrs[i]))<<16;
				}
			}
#endif
            for (iter=0;iter<nbout;iter++)
            {
                char byte;
                outdata=outptr[iter];
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
#ifdef ARM_BOARDTEST_PROFILE
//	printf("Here is the conclusion:\n");
//    get_total_times();
    timeval=get_mean_times ();
	frq=(float)Fin;
	if ((float)Fout>frq) frq=(float)Fout;
	frq=frq*1000.;
	MIPS=(MHZclockfreq*(float)timeval*frq)/((float)nblocks_tested*(float)RESAMPLE_BLOCK_SIZE_LOCAL*(float)(1e6));
	printf("MIPS: %f\n",MIPS);
#endif

#ifdef MEASURE_CYCLES
    printf("avg cortex cycles: %d\n",total_cortex_cycles/cortex_count);
#endif
#ifdef __flexcc2__
    printf("total cycles: %ld\n",total_count);
#endif
#ifdef TESTING
//  fclose(db);
#endif
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
#ifdef M4_LL_MSEC
		else if ((Fin==Fout)&&(Fin==48||Fin==16||Fin==8)) temp=1;
#endif
    }
    return temp;
}


