/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   encoder.c
* \brief  AMR Decoder NMF Component
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <amr/nmfil/encoder.nmf>
#include <stdlib.h>
#include <assert.h>
#include <verbose.h>
#include <malloc/include/heap.h>
#include <dbc.h>

#include "amr.h"
#include "amr_encoder_local.h"
#include "amr_alloc_ymem.h"
#include "common_interface.h"


/////////////////////////////////////////////////////////////////////
//              Global Variables
/////////////////////////////////////////////////////////////////////

static t_heap_info EXTMEM mHeapInfo[MEM_BANK_COUNT];

t_uint24 EXTMEM ATTR(XMemUsed)     = 0;
t_uint24 EXTMEM ATTR(YMemUsed)     = 0;
t_uint24 EXTMEM ATTR(DDR24MemUsed) = 0;
t_uint24 EXTMEM ATTR(DDR16MemUsed) = 0;
t_uint24 EXTMEM ATTR(ESR24MemUsed) = 0;
t_uint24 EXTMEM ATTR(ESR16MemUsed) = 0;

static EXTMEM AMR_ENCODER_CONFIG_STRUCT_T   amrenc_config_struct;
static EXTMEM AMR_ENCODER_INFO_STRUCT_T     amrenc_info_struct;

/////////////////////////////////////////////////////////////////////


void 
METH(start)() {
	int i;

	for (i=0;i<MEM_BANK_COUNT;i++)
	{
		mHeapInfo[i].Start         = NULL;
		mHeapInfo[i].SizeAvailable = 0;
		mHeapInfo[i].SizeAllocated = 0;
		mHeapInfo[i].SizeTotal     = 0;
	}
}

void 
METH(setHeap)(t_memory_bank memory_bank, void *buf, t_uint32 size){
	t_uint32  i;
	t_sword  *ptr;

	PRECONDITION(memory_bank < MEM_BANK_COUNT);
	PRECONDITION(!((int)buf  & 0x1));

	mHeapInfo[memory_bank].Start = buf;
	mHeapInfo[memory_bank].SizeAvailable = size;
	mHeapInfo[memory_bank].SizeAllocated = 0;
	mHeapInfo[memory_bank].SizeTotal = size;

	// Heap initialization
	ptr = (t_sword *)mHeapInfo[memory_bank].Start;
	for (i=0;i<size;i++)
	{
		*ptr++ = 0;
	}
}

t_uint16
METH(encodeFrame)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	int nbBytes;
	return amr_encode(interface_fe,
					  &interface_fe->stream_struct.bits_struct, &nbBytes);
}

void 
METH(setParameter)(AmrEncParam_t params) {
    amrenc_config_struct.noHeader       = params.bNoHeader;
    amrenc_config_struct.memory_preset  = params.memory_preset;
}

void 
METH(setConfig)(AmrEncConfig_t config) {
	amrenc_config_struct.mode           = config.nBitRate;
	amrenc_config_struct.dtx            = config.bDtxEnable;
    amrenc_config_struct.Payload_Format = config.ePayloadFormat;
    amrenc_config_struct.Efr_on	        = config.bEfr_on;
}


void
METH(open)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	int status,i;
	
	interface_fe->codec_config_local_struct = &amrenc_config_struct;
	interface_fe->codec_info_local_struct   = &amrenc_info_struct;

	for (i=0; i<MEM_BANK_COUNT; i++){
		interface_fe->heap_info_table[i] = &mHeapInfo[i];
	}

	status = amr_encode_open_malloc(interface_fe);
	POSTCONDITION(status == INIT_OK);

	ATTR(XMemUsed)     = mHeapInfo[MEM_XTCM].SizeAllocated;
	ATTR(YMemUsed)     = mHeapInfo[MEM_YTCM].SizeAllocated;
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

void
METH(close)(void *interface) {
    amr_encode_close(interface);
}

void
METH(reset)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	amr_encode_reset(interface_fe);
}

t_uint16
METH(getMaxFrameSize)(void) {
	return AMR_MAX_BITSTREAM_SIZE_IN_BIT;           // 304 bits
}

t_uint16
METH(getMaxSamples)(void) {
	return AMR_SAMPLE_FRAME_SIZE_IN_WORD;           // 160 samples
}

t_uint16
METH(getSampleBitSize)(void) {
	return AMR_BITS_PER_SAMPLE;                     // 16 bits
}


