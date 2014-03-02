/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   decoder.c
* \brief  AMR Decoder NMF Component
* \author ST-Ericsson
*/
/*****************************************************************************/
#include <amr/nmfil/decoder.nmf>
#include <stdlib.h>
#include <malloc/include/heap.h>
#include <dbc.h>
#include "amr.h"
#include "amr_decoder_local.h"
#include "amr_alloc_ymem.h"

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

static EXTMEM AMR_DECODER_INFO_STRUCT_T    amrdec_info_struct;
static EXTMEM AMR_DECODER_CONFIG_STRUCT_T  amrdec_config_struct;

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

	mHeapInfo[memory_bank].Start         = buf;
	mHeapInfo[memory_bank].SizeAvailable = size;
	mHeapInfo[memory_bank].SizeAllocated = 0;
	mHeapInfo[memory_bank].SizeTotal     = size;

	// Heap initialization
	ptr = (t_sword *)mHeapInfo[memory_bank].Start;
	for (i=0;i<size;i++)
	{
		*ptr++ = 0;
	}
}

t_uint16
METH(checkSynchro)(t_uint24 msp, t_uint24 lsp, t_uint16 *framesize_p) {
	return amr_syncword(msp, lsp, framesize_p);
}

t_uint16
METH(decodeFrame)(void *interface) {
    t_uint16 status;

    status = amr_decode_frame(interface);

    return status;
}

void
METH(setParameter)(AmrDecParams_t Params) {
	amrdec_config_struct.noHeader       = (unsigned short)Params.bNoHeader;
	amrdec_config_struct.concealment_on = (unsigned short)Params.bErrorConcealment;
	amrdec_config_struct.memory_preset  = (unsigned short)Params.memory_preset;
}

void
METH(setConfig)(AmrDecConfig_t config) {
	amrdec_config_struct.Payload_Format = (unsigned short)config.ePayloadFormat;
    amrdec_config_struct.Efr_on 	    = (unsigned short)config.bEfr_on;
}

void
METH(open)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	int     i;

	interface_fe->codec_config_local_struct = &amrdec_config_struct;
	interface_fe->codec_info_local_struct   = &amrdec_info_struct;

	for (i=0; i<MEM_BANK_COUNT; i++)
	{
		interface_fe->heap_info_table[i] = &mHeapInfo[i];
	}

	amr_init_decode_malloc(interface_fe);

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
METH(reset)(void *interface) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	amr_decode_reset(interface_fe);
}

void
METH(setOutputBuf)(void *interface, void *buf) {
	CODEC_INTERFACE_T *interface_fe = (CODEC_INTERFACE_T *) interface;
	AMR_DECODER_STRUCT_T *amr_decoder_struct = (AMR_DECODER_STRUCT_T *) interface_fe->codec_local_struct;

	amr_decoder_struct->synth = buf;
}

t_uint16
METH(getMaxFrameSize)(void) {
	return AMR_MAX_BITSTREAM_SIZE_IN_BIT;           // 304 bits
}

void
METH(getOutputGrain)(t_uint16 *blocksize, t_uint16 *maxchans) {
	*blocksize  = AMR_SAMPLE_FRAME_SIZE_IN_WORD;    // 160 samples
	*maxchans   = 1;
}

t_uint16
METH(getHeaderType)(void) {
	return NO_HEADER_VARYING_SIZE;
}

t_uint16
METH(getSampleBitSize)(void) {
	return AMR_BITS_PER_SAMPLE;                     // 16 bits
}

t_uint16
METH(getBlockSize)(void) {
	return 0;
}
