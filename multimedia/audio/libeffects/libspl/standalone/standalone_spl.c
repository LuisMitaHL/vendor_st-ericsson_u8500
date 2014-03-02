/****************************************************************/
/* FILE: standalone_spl.c	                                     */
/* AUTHOR: DURNERIN Matthieu                                    */
/* EMAIL: matthieu.durnerin@stericsson.com		 				 	 */
/* DIV: 3GP/3GM/PSA/UCAA                                        */
/* DATE: 2010 March 03                                          */
/* LAST REV: 2010 March 03                                      */
/****************************************************************/
/* GOAL: defines the main function for testing spl              */
/****************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "get_param.h"
#include "spl_defines.h"
#include "audiolibs_common.h"
#ifdef M4
#include "spl_algo_M4.h"
#ifndef CORTEX_M4
#define ARM_CYCLES //for using PMU cycle evaluations
#endif
#else
#include "spl_algo.h"
#endif
#ifdef ARM_CYCLES
#include <pmu_api.h>
#endif
#ifdef CORTEX_M4 
#include "systick.h"
#endif
typedef struct {
	long time;
	int index_buf_update;
} SplUpdate_t;

SPL_LOCAL_STRUCT_T spl_local_struct;

int *inbuf, *outbuf;
short sizebuf=48;
#define MEM_PRESET_DEFAULT 0

int parse_static_parameter(FILE *file, SplParam_t *pSplParam, SplTuning_t *pSplTuning) {
	long value;
	get_param(file, "Type", 'd', (void*)&value, 0);
	pSplParam->Type =(int) value;
	if (pSplParam->Type == SPL_PEAK_LIMITER) {
		get_param(file, "PeakClippingMode", 'd', (void*)&value, 0);
		pSplTuning->PeakClippingMode = (unsigned int)value;
	}
	get_param(file, "PowerAttackTime", 'd', (void*)&value, 0);
	pSplTuning->PowerAttackTime = (unsigned int)value;
	get_param(file, "PowerReleaseTime", 'd', (void*)&value, 0);
	pSplTuning->PowerReleaseTime = (unsigned int)value;
	get_param(file, "GainAttackTime", 'd', (void*)&value, 0);
	pSplTuning->GainAttackTime = (unsigned int)value;
	get_param(file, "GainReleaseTime", 'd', (void*)&value, 0);
	pSplTuning->GainReleaseTime = (unsigned int)value;
	get_param(file, "GainHoldTime", 'd', (void*)&value, 0);
	pSplTuning->GainHoldTime = (unsigned int)value;
	if (pSplParam->Type == SPL_PEAK_LIMITER) {
		get_param(file, "ThreshAttackTime", 'd', (void*)&value, 0);
		pSplTuning->ThreshAttackTime = (unsigned int)value;
		get_param(file, "ThreshReleaseTime", 'd', (void*)&value, 0);
		pSplTuning->ThreshReleaseTime = (unsigned int)value;
		get_param(file, "ThreshHoldTime", 'd', (void*)&value, 0);
		pSplTuning->ThreshHoldTime = (unsigned int)value;
	}
	get_param(file, "BlockSize", 'd', (void*)&value, 0);
	pSplTuning->BlockSize = (unsigned int)value;
	get_param(file, "LookAheadSize", 'd', (void*)&value, 0);
	pSplTuning->LookAheadSize = (unsigned int)value;
	return 0;
}

int update_param (int Type, FILE *file, SplStream_t *pSplStream,
		SplUpdate_t *pUpdate, SplConfig_t *pSplConfig) {
	long cpt_line = 0;
	long value;
#ifdef M4
	long long nb_buffers = 0;
#else
	MMlong nb_buffers = 0;
#endif
	long time_new = 0;

	// read parameters
	cpt_line = get_param(file, "Time", 'd', (void*)&time_new, 1);
	if (cpt_line < 0) /* end of file */
		return 0;
	if (time_new < pUpdate->time) {
		if (time_new != 0) {
			printf ("Error new time %ld < current time %ld\n", 
					time_new, pUpdate->time);
			printf ("Stop updating parameters\n");
		}
		return 0;
	}

	pUpdate->time = time_new;
	get_param(file, "Enable", 'd', (void*)&value, 1);
	pSplConfig->Enable = (int)value;
	get_param(file, "Threshold", 'd', (void*)&value, 1);
	pSplConfig->Threshold = (int)value;
	if (Type == SPL_PEAK_LIMITER) {
		get_param(file, "PeakL_Gain", 'd', (void*)&value, 1);
		pSplConfig->PeakL_Gain = (int)value;
	}
	else {
		get_param(file, "SPL_Mode", 'd', (void*)&value, 1);
		pSplConfig->SPL_Mode = (int)value;
		if (pSplConfig->SPL_Mode == SPL_NORMALIZATION) {
			printf("SPL Normalization mode not supported in this version, limitation mode used\n");
			pSplConfig->SPL_Mode = SPL_LIMITATION;
		}
		get_param(file, "SPL_UserGain", 'd', (void*)&value, 1);
		pSplConfig->SPL_UserGain = (int)value;
	}
	printf("Parameters to update at time %ld ms\n", time_new);
	printf("Enable: %d\n", pSplConfig->Enable);
	printf("Threshold: %d\n", pSplConfig->Threshold);
	if (Type == SPL_PEAK_LIMITER) {
		printf("PeakL_Gain: %d\n", pSplConfig->PeakL_Gain);
	}
	else {
		printf("SPL_Mode: %d\n", pSplConfig->SPL_Mode);
		printf("SPL_UserGain: %d\n", pSplConfig->SPL_UserGain);
	}

	fflush(stdout);
#ifdef M4
	nb_buffers = (((long long)pUpdate->time*(long long)pSplStream->SampleFreq)/1000)/sizebuf; 
	pUpdate->index_buf_update = nb_buffers;
#else
	nb_buffers = (((MMlong)pUpdate->time*(MMlong)pSplStream->SampleFreq)/1000)/sizebuf; 
	pUpdate->index_buf_update = nb_buffers;
#endif   
	return 1;
}


int main(int argc, char **argv) {
	FILE *file_mainin=NULL,*file_mainout=NULL,*file_static=NULL, *file_dyn=NULL;
	SplStream_t mSplStream;
	SplParam_t  mSplParam;
	SplTuning_t mSplTuning;
	SplConfig_t mSplConfig;
	SplUpdate_t mUpdate;
	long niter_min, niter=0;
	int i,j;
#ifdef ARM_CYCLES
	int index=0,arm_cycles=0;
	float arm_mips=0.0;
	long long accu_cycles=0L;
#endif
#ifdef CORTEX_M4
    int sys_cycls =0;
    int sys_cycls_initial =0;
    int cycle_this_frame =0;
    int worst_cycle = 0;
	float M4_mips,M4_avgmips=0,M4_worstmips;
#endif    
#ifdef UNIX
	long cpt=0;
	int k;
	short percent=0;
#endif
	int frmCnt = 0;

	int little_endian = 1;

	short low, high;
	short sample; // must be two bytes, so up-converting to float will preserve the sign

#ifndef UNIX
#ifndef M4
	long max_cycle_count = 0,  max_cycle_frame_nb = 0;
	long start_count, stop_count;	// For locating worst case frame
#endif
#endif

	if(argc != 7 && argc != 8 && argc != 9) {
		printf("\n Usage :");
		printf("\n spl <static_file> <dyn_file> <input_file> <output_file> <freq> <ch_no> [endianness]\n");
		printf("     <static_file> - Text file containing the static and tuning parameters\n");
		printf("     <dyn_file>    - Text file containing the dynamic parameters\n");
		printf("     <input_file>  - PCM file for main channels \n");
		printf("     <output_file> - PCM file, channel mode same as input\n");
		printf("     <freq>        - Sample rate, default 48000\n");
		printf("     <ch_no>       - Number of main channels in main_input_file, must be {1,2}\n");
		printf("     [endianness]  - L for little-endian (default), B for big-endian\n\n");
		printf("     [buffer_size] - Buffer size, default 48 (1 ms at 48 kHz)\n");
		exit(1);
	}

	mSplStream.SampleFreq = atoi(argv[5]);
	if (mSplStream.SampleFreq > 48000 || mSplStream.SampleFreq < 8000) {
		fprintf(stderr, "\n Wrong sample rate %s. Minimum is 8000, maximum is 48000\n", argv[4]);
		exit(3);
	} 

	mSplStream.NumberChannel = atoi(argv[6]);
	if (mSplStream.NumberChannel > 2 || mSplStream.NumberChannel < 1) {
		fprintf(stderr, "\n Number of main channels is incorrect. Minimum is 1, maximum is 2\n");
		exit(3);
	}

	if (argc == 8) {
		if ( argv[7][0] == 'B' || argv[7][0] == 'b') {
			little_endian = 0; 
			printf("Big endian file\n");
		}
		else {
			sizebuf = atoi(argv[7]);
			printf("Buffer size = %d\n", sizebuf);
		}
	}
	else if (argc == 9) {
		if ( argv[7][0] == 'B' || argv[7][0] == 'b') {
			little_endian = 0;
			printf("Big endian file\n");
		}
		sizebuf = atoi(argv[8]);
		printf("Buffer size = %d\n", sizebuf);
	}

	/* open static parameter file */
	if ((file_static = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "\n Can't open static parameter file <%s>\n", argv[1]);
		exit(1);
	}
	else {
		printf("\n static parameter file opened :<%s>\n", argv[1]);
	}

	/* open dynamic parameter file */
	if ((file_dyn = fopen(argv[2], "r")) == NULL) {
		fprintf(stderr, "\n Can't open dynamic parameter file <%s>\n", argv[2]);
		exit(1);
	}
	else {
		printf("\n dynamic parameter file opened :<%s>\n", argv[2]);
	}

	// read parameters
	if (parse_static_parameter(file_static, &mSplParam, &mSplTuning) < 0)
		exit(3);


	// open input files
	if ((file_mainin = fopen(argv[3], "rb")) == NULL) {
		fprintf(stderr, "\n Can't open input file <%s>\n", argv[3]);
		exit(1);
	}
	else {
		printf("\n input file opened :<%s>\n", argv[3]);

		fseek(file_mainin, 0, 2);
		niter_min = ftell(file_mainin) / 2 / mSplStream.NumberChannel;
		rewind(file_mainin);

		niter = niter_min;
	}

	if (niter>=sizebuf) {
		niter -= (niter % sizebuf);
	}
	else {
		fprintf(stderr, "all files must contains at least %d samples \n", sizebuf*mSplStream.NumberChannel);
		exit(1);
	}

	// open output file
	if ((file_mainout = fopen(argv[4], "wb")) == NULL)	{
		fprintf(stderr, "\n Can't open input file <%s>\n", argv[4]);
		exit(0);
	} 
	else {
		printf("\n output file opened :<%s>\n", argv[4]);
	}

	inbuf = (int *)malloc(sizeof(int)*sizebuf*mSplStream.NumberChannel);
	outbuf = (int *)malloc(sizeof(int)*sizebuf*mSplStream.NumberChannel); 

	spl_init(&spl_local_struct, &mSplParam, &mSplStream, &mSplTuning);

	mUpdate.time = -1;
	mUpdate.index_buf_update = 0;

	if (update_param(mSplParam.Type, file_dyn, &mSplStream, 
				&mUpdate, &mSplConfig) == 0) {
		fprintf(stderr, "\n Can't update parameters \n");
		exit(1);
	}
#ifdef ARM_CYCLES
	PMU_init(PMU_INSTRUCTIONS,
			PMU_DATA_MISS,
			PMU_DATA_PENALTY,
			PMU_INST_MISS,
			PMU_INST_PENALTY,
			PMU_BRANCH_MISS
			);
#endif
#ifdef CORTEX_M4
        Systick_Reload();
        //Check_Cycle_count_enable();
        Enable_Systick();
#endif

	for (i=0 ; i<niter/sizebuf; i++) {

		/* Look for an update of parameters */
		if (mUpdate.index_buf_update == i) {
			/* printf("\nUpdate parameters at time %ld ms, nb samples %lld, buffer %d\n",
			   mUpdate.time, (MMlong)i*(MMlong)sizebuf/(MMlong)mSplStream.NumberChannel, i); */
			/* set dynamic params */
			spl_set_config(&spl_local_struct, &mSplConfig);
			/* next update */
			update_param(mSplParam.Type, file_dyn, &mSplStream, 
					&mUpdate, &mSplConfig);
		}
		for (j=0 ; j<sizebuf*mSplStream.NumberChannel; j++) {
#ifdef UNIX
			cpt++;
			if (cpt%(niter*mSplStream.NumberChannel/100)==0) {
				percent++;
				fprintf(stderr," %d %% \r",percent);
			}
#endif
			if (little_endian) 
			{
				low = getc(file_mainin);
				high = getc(file_mainin);
			}
			else
			{
				high = getc(file_mainin);
				low = getc(file_mainin);
			}
			sample = (high<<8) | (low&0xFF);
#ifdef M4
			inbuf[j] = ((int)sample)<<16;        //M4 or CA9
#else
			inbuf[j] = (wsgn16(sample))<<8;      //DSP or X86_MMDSP
#endif
		}
#ifndef UNIX
#ifndef M4
		start_count = get_cycle_count();
#endif
#endif
		// call the processing routine
#ifdef ARM_CYCLES
		PMU_tap(0);
#endif
#ifdef CORTEX_M4 
		sys_cycls_initial=systick_curr_reg_read();
#endif
		spl_main(inbuf, outbuf, sizebuf, &spl_local_struct);
#ifdef ARM_CYCLES
		PMU_tap(1);
		accu_cycles+=(long long)(PMU_tapgetcycles(1,0));
		index++;
#endif
#ifdef CORTEX_M4 
        sys_cycls = systick_curr_reg_read();
		cycle_this_frame=sys_cycls_initial-sys_cycls;
		if (cycle_this_frame<0) cycle_this_frame+=0xffffff; //system cycle counter decrements in circular mode between 0xffffff and 0
		if (cycle_this_frame>worst_cycle) worst_cycle=cycle_this_frame;
		M4_mips=(float)cycle_this_frame/1e6;
		M4_mips=M4_mips*((float)(mSplStream.SampleFreq));
		M4_mips=M4_mips/(float)sizebuf;
		M4_avgmips=.01*M4_mips+.99*M4_avgmips;
#endif
#ifndef UNIX
#ifndef M4
		stop_count = get_cycle_count();

		if ((stop_count - start_count) > max_cycle_count) {
			max_cycle_count = stop_count - start_count;
			max_cycle_frame_nb = frmCnt;
		}
#endif
#endif
		frmCnt++;

		for (j=0 ; j<sizebuf*mSplStream.NumberChannel ; j++) {
			int tmp;
#ifdef M4
			tmp = (outbuf[j]>>8)+0x80;
#else
			tmp = waddsat(outbuf[j],0x80);
#endif
			sample= SAT_16((tmp>>8));
			low = (char)(sample);
			high = (char)(sample>>8);

			if(little_endian) {
				putc(low,file_mainout);
				putc(high,file_mainout);
			}
			else {
				putc(high,file_mainout);
				putc(low,file_mainout);
			}
		}
	}
#ifdef ARM_CYCLES
	arm_cycles=accu_cycles/index;
	arm_mips=(float)arm_cycles/1e6;
	arm_mips=arm_mips*((float)(mSplStream.SampleFreq));
	arm_mips=arm_mips/(float)sizebuf;
	printf("\n\n ---------- avg arm_mips: %f ---------\n\n",arm_mips);	
#endif
#ifdef CORTEX_M4
	M4_worstmips=(float)worst_cycle/1e6;
	M4_worstmips=M4_worstmips*((float)(mSplStream.SampleFreq));
	M4_worstmips=M4_worstmips/(float)sizebuf;
	printf("\n\n -----  avg M4_mips : %f worst M4_mips %f -----\n\n",M4_avgmips,M4_worstmips);
#endif    

	free(inbuf);
	free(outbuf);
#ifndef UNIX
#ifndef M4
//	printf("Worst case MHz is %.1f, occurs in frame no. %ld \n", max_cycle_count*48.0/128000, max_cycle_frame_nb);
	printf("Worst case MHz is %.1f, occurs in frame no. %ld \n", (float)(mSplStream.SampleFreq)*(((float)max_cycle_count/1e6)/(float)sizebuf), max_cycle_frame_nb);

#endif
#endif
	fclose(file_mainin);
	fclose(file_mainout);
	fclose(file_static);
	fclose(file_dyn);
	return 0;
}
