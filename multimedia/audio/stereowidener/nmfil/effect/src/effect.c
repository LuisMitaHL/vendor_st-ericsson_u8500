/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <stereowidener/nmfil/effect.nmf>
#include <stdlib.h>
#include <malloc/include/heap.h>
#include <dbc.h>
#include <stdio.h>
#include <verbose.h>
#include "audiotables.h"
#include "stereoenhancer.h"

#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "audio_stereowidener_nmfil_effect_src_effectTraces.h"
#endif

////////////////////////////////////////////////////////////
//          Global Variables
////////////////////////////////////////////////////////////
/* FE entry point structure */
static STEREOWIDENER_DUMMY_INTERFACE_T interface;

static t_heap_info EXTMEM mHeapInfo[MEM_BANK_COUNT];

t_uint24 EXTMEM ATTR(XMemUsed)     = 0;
t_uint24 EXTMEM ATTR(YMemUsed)     = 0;
t_uint24 EXTMEM ATTR(DDR24MemUsed) = 0;
t_uint24 EXTMEM ATTR(DDR16MemUsed) = 0;
t_uint24 EXTMEM ATTR(ESR24MemUsed) = 0;
t_uint24 EXTMEM ATTR(ESR16MemUsed) = 0;

static t_uint16       mChannelsIn;
static t_sample_freq  mSamplingFreq;
static int AUDIO_CONST_MEM * AudioTables_sampling_freqs_ptr;

static STEREOENHANCER_LOCAL_STRUCT_T       mStereoEnhancerData;
static EXTMEM StereoenhancerAlgoConfig_t   mStereoenhancerConfig;
static void (*processFunction)(t_sword * inbuf, t_sword * outbuf, t_uint16 size);

////////////////////////////////////////////////////////////

void
METH(start)() {
	int i;

	for (i=0;i<MEM_BANK_COUNT;i++)
	{
		mHeapInfo[i].Start          = NULL;
		mHeapInfo[i].SizeAvailable  = 0;
		mHeapInfo[i].SizeAllocated  = 0;
		mHeapInfo[i].SizeTotal      = 0;
	}
}

void
METH(setHeap)(t_memory_bank memory_bank, void *buf, t_uint32 size){
	t_uint32  i;
	t_sword  *ptr;

	PRECONDITION(memory_bank < MEM_BANK_COUNT);
	PRECONDITION(!((int)buf  & 0x1));

	mHeapInfo[memory_bank].Start          = buf;
	mHeapInfo[memory_bank].SizeAvailable  = size;
	mHeapInfo[memory_bank].SizeAllocated  = 0;
	mHeapInfo[memory_bank].SizeTotal      = size;

	// Heap initialization
	ptr = (t_sword *)mHeapInfo[memory_bank].Start;
	for (i=0;i<size;i++){
		*ptr++ = 0;
	}
}

void processBufferMonoToStereo(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
	mStereoEnhancerData.DataIn[0] = inbuf;
	mStereoEnhancerData.InSamples = size;

	mStereoEnhancerData.DataOut[0] = outbuf;
	mStereoEnhancerData.DataOut[1] = outbuf + 1;

	if (mStereoEnhancerData.Enable) {
		m2s_expansion(&mStereoEnhancerData);
	}
	else {
		upmix(&mStereoEnhancerData);
	}
}

void processBufferStereoToStereo(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
	mStereoEnhancerData.DataIn[0] = inbuf;
	mStereoEnhancerData.DataIn[1] = inbuf + 1;
	mStereoEnhancerData.InSamples = size >> 1;

	mStereoEnhancerData.DataOut[0] = outbuf;
	mStereoEnhancerData.DataOut[1] = outbuf + 1;

	if (mStereoEnhancerData.Enable) {
		stereo_enhancement(&mStereoEnhancerData);
	}
	else {
		bypass(&mStereoEnhancerData);
	}
}

void METH(open)()
{
	int i;
	CODEC_INIT_T status;
	STEREOWIDENER_DUMMY_INTERFACE_T *interface_fe = (STEREOWIDENER_DUMMY_INTERFACE_T *) &interface;

	for (i=0; i<MEM_BANK_COUNT; i++){
		interface_fe->heap_info_table[i] = &mHeapInfo[i];
	}

	status = m2s_init(&mStereoEnhancerData,interface_fe);
	POSTCONDITION(status == INIT_OK);

	ATTR(XMemUsed) = mHeapInfo[MEM_XTCM].SizeAllocated;
	ATTR(YMemUsed) = mHeapInfo[MEM_YTCM].SizeAllocated;
	ATTR(DDR24MemUsed) = mHeapInfo[MEM_DDR24].SizeAllocated;
	ATTR(DDR16MemUsed) = mHeapInfo[MEM_DDR16].SizeAllocated;
	ATTR(ESR24MemUsed) = mHeapInfo[MEM_ESR24].SizeAllocated;
	ATTR(ESR16MemUsed) = mHeapInfo[MEM_ESR16].SizeAllocated;

	if (disp_mem.displayMemPreset != NULL)
	{
		disp_mem.displayMemPreset(ATTR(XMemUsed),
								  ATTR(YMemUsed),
								  ATTR(DDR24MemUsed),
								  ATTR(DDR16MemUsed),
								  ATTR(ESR24MemUsed),
								  ATTR(ESR16MemUsed));
	}

}

void METH(reset)()
{
	STEREOWIDENER_DUMMY_INTERFACE_T *interface_fe = (STEREOWIDENER_DUMMY_INTERFACE_T *) &interface;
	// reset the flag to signal a complete reset
	mStereoEnhancerData.strength_flag = 0;
	stereo_reset(&mStereoEnhancerData);
	m2s_reset(&mStereoEnhancerData,interface_fe);
	
}

void METH(processBuffer)(t_sword * inbuf, t_sword * outbuf, t_uint16 size) {
	
	//int error = 0;
	if (mStereoenhancerConfig.mNewConfigAvailable == 1)
    {
        applyEffectConfig(&mStereoenhancerConfig,&mStereoEnhancerData);
        //reset the value
        mStereoenhancerConfig.mNewConfigAvailable = 0;
    }
	processFunction(inbuf, outbuf, size);
	OstTraceInt0(TRACE_DEBUG,"StereowidenerNmf::ProcessFunction reached");
}


////////////////////////////////////////////////////////////
//          Provided Interfaces
////////////////////////////////////////////////////////////

void
METH(setParameter)(StereowidenerParams_t Params) {
	int i = ESAA_FREQ_LAST_IN_LIST;
	mStereoEnhancerData.memory_preset = Params.iMemoryPreset;
	
	//set sampling frequency
	AudioTables_sampling_freqs_ptr = fn_AudioTables_sampling_freqs();
	  
	  for( i = ESAA_FREQ_LAST_IN_LIST; i>= 0; i--)
	  {
		if(AudioTables_sampling_freqs_ptr[i] == Params.iSamplingFreq)
			break;
	  }
	
	  mSamplingFreq = i ;
	mChannelsIn = Params.iChannelsIn;
	if (mChannelsIn == 1) {
		processFunction = processBufferMonoToStereo;

		mStereoEnhancerData.NbInChannels = 1;
		mStereoEnhancerData.DataInOffset = 1;
	} else {
		processFunction = processBufferStereoToStereo;

		mStereoEnhancerData.NbInChannels = 2;
		mStereoEnhancerData.DataInOffset = 2;
	}

	mStereoEnhancerData.NbOutChannels = 2;
	mStereoEnhancerData.DataOutOffset = 2;
	mStereoEnhancerData.sample_freq = mSamplingFreq;

	switch (mSamplingFreq)
	{
	case 7: //ESAA_FREQ_48KHZ
		//mStereoEnhancerData.SamplingRate = FS48K;
		mStereoEnhancerData.Delay_M2S = 102; // (48000 * 0.0021)+1
		mStereoEnhancerData.Delay_LS = 8;
		break;

	case 8: //ESAA_FREQ_44_1KHZ
		//mStereoEnhancerData.SamplingRate = FS44K;
		mStereoEnhancerData.Delay_M2S = 94; // (44100 * 0.0021)+1
		mStereoEnhancerData.Delay_LS = 7;
		break;

	default:
		ASSERT(0);
	}
}

void METH(setConfig)(StereowidenerConfig_t config)
{
		
	/*check if any user changes to the parameters*/
	if (mStereoenhancerConfig.iEnable != config.iEnable)
	{
		mStereoenhancerConfig.iEnable = config.iEnable;
	}

	if (mStereoenhancerConfig.iOutputMode != config.iOutputMode)
	{
		mStereoenhancerConfig.iOutputMode = config.iOutputMode;
	}

	if (mStereoenhancerConfig.iHeadphoneMode != config.iHeadphoneMode)
	{
		mStereoenhancerConfig.iHeadphoneMode = config.iHeadphoneMode;
	}
	
	if (mStereoenhancerConfig.iInternalExternalSpeaker != config.iInternalExternalSpeaker)
	{
		mStereoenhancerConfig.iInternalExternalSpeaker = config.iInternalExternalSpeaker;
	}
	
	if (mStereoenhancerConfig.iStrength != config.iStrength)
	{
		mStereoenhancerConfig.iStrength = config.iStrength;
	}
	//set the mNewConfigAvailable as true
    mStereoenhancerConfig.mNewConfigAvailable = 1;
	
}

void METH(newFormat) (
	t_sample_freq *freq,
	t_uint16 *chans_nb,
	t_uint16 *sample_bitsize)
{
	//PRECONDITION(mSamplingFreq == *freq);
	PRECONDITION(mChannelsIn == *chans_nb);

	*chans_nb = 2; // output always stereo
}

