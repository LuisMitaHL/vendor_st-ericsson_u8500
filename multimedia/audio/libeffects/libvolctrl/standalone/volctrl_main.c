/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   volctrl_main.c
* \brief  
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

#include "volctrl.h"
#include "get_param.h"

//#ifdef ARM
//#define MEASURE_CYCLES
//#endif

#define Q8_ONE (1<<8)
//#define CKTERMINATE
void read_input(FILE *file_in, MMshort* buff, int nb_sample, int sample_size) {
    int i,j;
    MMshort sample;
    int nb_read = sample_size / 8;


    for (j=0 ; j<nb_sample ;  j++)
    {
        sample = 0;
        for (i=nb_read;i>0;i--) {
            sample <<=8;
            sample += (int)fgetc(file_in);
        }
#ifndef __flexcc2__
        sample <<= (32-sample_size);
#ifdef MMDSP //x86_mmdsp
        sample >>= 8;
#endif
#else
        sample <<= (24-sample_size);
#endif
        buff[j] = sample;
    }
}

void write_output(MMshort *buff, FILE *file_out, int nb_sample, int sample_size) {
    int i,j;
    int nb_write = sample_size / 8;
    int sample;
    int tmp;

    for (j=0; j<nb_sample;j++) {
        sample = buff[j];
#ifndef MMDSP
        // Rounding if no saturation
        if (sample < 0x7fff0000) {
            sample += (1 << (32-sample_size-1));
        }
        sample >>= (32-sample_size);
#else
        sample= waddsat(sample,0x80);
        sample >>= (24-sample_size);
#endif
        for (i=nb_write;i>0;i--) {
            tmp = (sample>>(8*(i-1)))&0xff;
            fputc(tmp, file_out);
        }
    }
}

void RAZChannel(MMshort * buff, int size, int stride) {
    int i;
    for (i=0; i < size; i+= stride) {
        buff[i] = 0;
    }
}


// Copy from process_buffer method from NFM
void process_buffer(VOLCTRL_LOCAL_STRUCT_T *mVolData, MMshort *buff, int size) {
    MMshort *left_in  = buff;
    MMshort *right_in = buff + 1;
    MMshort *left_out = buff;
    MMshort *right_out= buff + 1;
#ifndef MMDSP
	int i;
	int gain_idx;
#endif
    while (mVolData->new_config) {
        volctrl_set_new_config(mVolData);
    }
#ifndef MMDSP
	if (mVolData->multichannel!=0)
	{
		for (i=0;i<mVolData->input_mode;i++)
		{
			gain_idx=i+START_IDX_MCH;
//			if (mVolData->gain_mask & (1<<gain_idx))
				process_one_of_nch_no_cross_gain(buff,buff, i,size/mVolData->input_mode,mVolData);
		}
	}
	else
	{
#endif
		switch (mVolData->input_mode) {
			case 1:
				// mono mode
				process_one_channel_no_cross_gain(
						left_in, left_out, GLL, size, 1, mVolData);
				break;

			case 2:
				// stereo mode
				if (mVolData->downmix) {
					process_one_channel_with_cross_gain(
							left_in, right_in, left_out, 1, size, GLL, GRL, mVolData);
				}
				else
				{
					if ((mVolData->gain_mask & GRL_MASK) && (mVolData->gain_mask & GLR_MASK)) 
					{
						// both cross gains not zero
						process_two_channel_with_cross_gain(
								left_in, left_out, size, mVolData);
					} 
					else
						if (mVolData->gain_mask & GRL_MASK) {
							// left channel from L & R
							process_one_channel_with_cross_gain(
									left_in, right_in, left_out, 2, size, GLL, GRL, mVolData);

							if (mVolData->gain_mask & GRR_MASK) {
								// right channel from R
								process_one_channel_no_cross_gain(
										right_in, right_out, GRR, size, 2, mVolData);
							}
							else
							{
								RAZChannel(right_in, size, 2);
							}
						} 
						else
							if (mVolData->gain_mask & GLR_MASK) {
								if (mVolData->gain_mask & GRR_MASK) {
									// right channel from L & R
									process_one_channel_with_cross_gain(
											right_in, left_in, right_out, 2, size, GRR, GLR, mVolData);
								}
								else
								{
									// right channel from L
									process_one_channel_no_cross_gain(
											left_in, right_out, GLR, size, 2, mVolData);
								}

								// left channel from L
								process_one_channel_no_cross_gain(
										left_in, left_out, GLL, size, 2, mVolData);
							}
							else
							{
								// no cross gain

								// left channel from L
								process_one_channel_no_cross_gain(
										left_in, left_out, GLL, size, 2, mVolData);

								if (mVolData->gain_mask & GRR_MASK) {
									// right channel from R
									process_one_channel_no_cross_gain(
											right_in, right_out, GRR, size, 2, mVolData);
								}
								else
								{
									RAZChannel(right_out, size, 2);
								}
							}
				}
				break;

			default:
				printf("\rUnsupported number of input channels\nExiting\n");
				exit(3);
		}
#ifndef MMDSP
	} //if (multichannel!=0)
#endif
}

int main(int argc, char **argv)
{
    VOLCTRL_LOCAL_STRUCT_T volctrl;
    MMshort nb_bit_in, nb_channel_in, tmp, block_size_in, block_size_out;
    long longvar;
    double doublevar;
    FILE *file_in, *file_out, *file_param;

    MMshort *buff;

    int filesize, niter;
    int processed = 0;
    int arg;

#ifndef MMDSP
	int i;
#endif

#ifdef MEASURE_CYCLES
	int cortex_cycles=0;
	unsigned long total_cortex_cycles=0L;
	int cortex_count=0;
#endif

    if (argc < 4)
    {		/* if not enough arguments */
        printf("\n Usage :");
        printf
            ("\n volctrl <input file> <output file> <parameters file> \n\n");
        exit(-1);
    }

    /**************/
    /* open files */
    /**************/

    arg = 1;	/* jump program name */
    if ((file_in = fopen(argv[arg++], "rb")) == NULL)
    {
        printf("\n Can't open input file <%s>\n", argv[arg - 1]);
        exit(0);
    } else
        printf("\n input file openned :<%s>\n", argv[arg - 1]);

    if ((file_out = fopen(argv[arg++], "wb")) == NULL)
    {
        printf("\n Can't open input file <%s>\n", argv[arg - 1]);
        exit(0);
    } else
        printf("\n output file openned :<%s>\n", argv[arg - 1]);

    if ((file_param = fopen(argv[arg], "rb")) == NULL)
    {
        printf("\n Can't open parameter file <%s>\n",argv[arg] );
        exit(0);
    } else 
        printf("\n parameter file openned :<%s>\n", argv[arg]);


    /************************/
    /* Read parameters file */
    /************************/
#ifndef MMDSP
    get_param(file_param, "multichannel", 'd', &longvar);
    volctrl.multichannel = (MMshort) longvar;
#endif
    get_param(file_param, "nb_bit_in", 'd', &longvar);
    nb_bit_in = (MMshort) longvar;

    get_param(file_param, "nb_channel_in", 'd', &longvar);
    nb_channel_in = (MMshort) longvar;
    volctrl.input_mode = nb_channel_in;

	longvar=0;
    get_param(file_param, "downmix", 'd', &longvar);
    volctrl.downmix = (MMshort) longvar;

    get_param(file_param, "db_ramp", 'd', &longvar);
    volctrl.db_ramp =(MMshort)longvar;

	get_param(file_param, "gll", 'f', &doublevar);
    tmp = (MMshort)(doublevar * Q8_ONE);
    volctrl.gain_cmd[0][GLL] = tmp ;

	get_param(file_param, "glr", 'f', &doublevar);
    tmp = (MMshort)(doublevar * Q8_ONE);
    volctrl.gain_cmd[0][GLR] = tmp;

	get_param(file_param, "grl", 'f', &doublevar);
    tmp = (MMshort)(doublevar * Q8_ONE);
    volctrl.gain_cmd[0][GRL] = tmp;

	get_param(file_param, "grr", 'f', &doublevar);
    tmp = (MMshort)(doublevar * Q8_ONE);
    volctrl.gain_cmd[0][GRR] = tmp;

    get_param(file_param, "alphal", 'd', &longvar);
    volctrl.duration[0] = (MMshort) longvar;
	volctrl.duration[1] = (MMshort) longvar;

	get_param(file_param, "alphar", 'd', &longvar);
    volctrl.duration[2] = (MMshort) longvar;
	volctrl.duration[3] = (MMshort) longvar;

	volctrl.terminate[0] = 0;
	volctrl.terminate[1] = 0;
	volctrl.terminate[2] = 0;
	volctrl.terminate[3] = 0;

    get_param(file_param, "block_size", 'd', &longvar);
    block_size_in = (MMshort)longvar;

#ifndef MMDSP
	if (volctrl.multichannel!=0)
	{
		tmp=0;
		get_param(file_param, "g0", 'f', &doublevar);
		tmp = (MMshort)(doublevar * Q8_ONE);
		volctrl.gain_cmd[0][G0] = tmp ;

		tmp=0;
		get_param(file_param, "g1", 'f', &doublevar);
		tmp = (MMshort)(doublevar * Q8_ONE);
		volctrl.gain_cmd[0][G1] = tmp ;

		tmp=0;
		get_param(file_param, "g2", 'f', &doublevar);
		tmp = (MMshort)(doublevar * Q8_ONE);
		volctrl.gain_cmd[0][G2] = tmp ;

		tmp=0;
		get_param(file_param, "g3", 'f', &doublevar);
		tmp = (MMshort)(doublevar * Q8_ONE);
		volctrl.gain_cmd[0][G3] = tmp ;

		tmp=0;
		get_param(file_param, "g4", 'f', &doublevar);
		tmp = (MMshort)(doublevar * Q8_ONE);
		volctrl.gain_cmd[0][G4] = tmp ;

		tmp=0;
		get_param(file_param, "g5", 'f', &doublevar);
		tmp = (MMshort)(doublevar * Q8_ONE);
		volctrl.gain_cmd[0][G5] = tmp ;

		tmp=0;
		get_param(file_param, "g6", 'f', &doublevar);
		tmp = (MMshort)(doublevar * Q8_ONE);
		volctrl.gain_cmd[0][G6] = tmp ;

		tmp=0;
		get_param(file_param, "g7", 'f', &doublevar);
		tmp = (MMshort)(doublevar * Q8_ONE);
		volctrl.gain_cmd[0][G7] = tmp ;

		tmp=0;
		get_param(file_param, "alpha0", 'd', &longvar);
		volctrl.duration[G0] = (MMshort) longvar;

		tmp=0;
		get_param(file_param, "alpha1", 'd', &longvar);
		volctrl.duration[G1] = (MMshort) longvar;

		tmp=0;
		get_param(file_param, "alpha2", 'd', &longvar);
		volctrl.duration[G2] = (MMshort) longvar;

		tmp=0;
		get_param(file_param, "alpha3", 'd', &longvar);
		volctrl.duration[G3] = (MMshort) longvar;

		tmp=0;
		get_param(file_param, "alpha4", 'd', &longvar);
		volctrl.duration[G4] = (MMshort) longvar;

		tmp=0;
		get_param(file_param, "alpha5", 'd', &longvar);
		volctrl.duration[G5] = (MMshort) longvar;

		tmp=0;
		get_param(file_param, "alpha6", 'd', &longvar);
		volctrl.duration[G6] = (MMshort) longvar;

		tmp=0;
		get_param(file_param, "alpha7", 'd', &longvar);
		volctrl.duration[G7] = (MMshort) longvar;

		for (i=0;i<MAX_MCH;i++)
			volctrl.terminate[START_IDX_MCH+i] = 0;

	} //if (volctrl.multichannel!=0)
#endif //ifndef MMDSP

    fclose(file_param);

    /**************************/
    /* Init and reset volctrl */
    /**************************/
    volctrl.gain_mask=0;
    volctrl.gain_cmd[1][GLL] = 0;
    volctrl.gain_cmd[1][GLR] = VOLCTRL_MINIMUM_GAIN * Q8_ONE;
    volctrl.gain_cmd[1][GRL] = VOLCTRL_MINIMUM_GAIN * Q8_ONE;
    volctrl.gain_cmd[1][GRR] = 0;
	volctrl.configL=volctrl.configR=2; //bit 0 for nmf bit 1 for standalone
#ifndef MMDSP
	if (volctrl.multichannel!=0)
	{
		for (i=0;i<MAX_MCH;i++)
		{
			volctrl.gain_cmd[1][START_IDX_MCH+i]=0;
			volctrl.configX[i]=2; //bit 0 for nmf bit 1 for standalone
		}
	}
#endif
    volctrl.first_time = 0;
    volctrl.new_config = VOLCTRL_IMMEDIAT_CMD_PENDING+1;

    volctrl_init(&volctrl, 8); //44.1 kHz

    volctrl_set_new_config(&volctrl);

    /***************/
    /* Launch test */
    /***************/
    printf("\nLaunch test...\n");

    buff = (MMshort*) malloc(block_size_in*sizeof(MMshort));
    if (buff == NULL)
    {
        printf("\tCan't allocate buffer\n");
        exit(0);
    }

    fseek(file_in, 0, 2);
    filesize = ftell(file_in) / (nb_bit_in/8);
    rewind(file_in);

    niter = (filesize / block_size_in);

    block_size_out = block_size_in;
    if ((volctrl.input_mode == 2) && volctrl.downmix) {
        block_size_out /= 2;
    }

    printf("0 sample processed");

    for(; niter > 0; niter--) {
        read_input(file_in, buff, block_size_in, nb_bit_in);
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
#ifdef CKTERMINATE
		if (niter<3*(filesize / block_size_in)/4)
		{
			volctrl.terminate[0]=1;
			volctrl.terminate[1]=1;
			volctrl.terminate[2]=1;
			volctrl.terminate[3]=1;
		}
		if (niter<(filesize / block_size_in)/2)
		{
			volctrl.terminate[0]=0;
			volctrl.terminate[1]=0;
			volctrl.terminate[2]=0;
			volctrl.terminate[3]=0;
		}
#endif
        process_buffer(&volctrl, buff, block_size_in);
#ifdef MEASURE_CYCLES
		cortex_cycles=PerfGetCount(0xff);
		total_cortex_cycles+=cortex_cycles;
		cortex_count++;
//        printf("\n Cycles : %d \n", cortex_cycles);
//        printf("\n Instructions : %d \n", PerfGetCount(1));
//        printf("\n D Miss : %d \n", PerfGetCount(2));
//        printf("\n I Miss : %d \n", PerfGetCount(3));
#endif
        write_output(buff, file_out, block_size_out, nb_bit_in);
        processed += block_size_in;
        printf("\r%d samples processed", processed);
    }

    niter = filesize % block_size_in;

    if (niter) {
        read_input(file_in, buff, niter, nb_bit_in);
        process_buffer(&volctrl, buff, niter);
        write_output(buff, file_out, ((volctrl.input_mode == 2) && volctrl.downmix)? niter/2:niter, nb_bit_in);
        processed += niter;
        printf("\r%d samples processed", processed);
    }

    printf("\nDone, ");
    free (buff);
    fclose(file_in);
    fclose(file_out);

#ifdef MEASURE_CYCLES
	printf("avg cortex cycles: %d\n",total_cortex_cycles/cortex_count);
#endif

    printf("exiting.\n");
    return 0;
}
