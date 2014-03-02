/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _common_interface_h_
#define _common_interface_h_

/* include private defines */
#include "bitstream_lib_proto.h"

#define _ENC_PRIVATE_DATA_SIZE 10

#ifndef MAX_OUTPUT_CHANNELS
#ifdef MC
#define MAX_OUTPUT_CHANNELS  6
#else
#define MAX_OUTPUT_CHANNELS  2
#endif
#endif /* MAX_OUTPUT_CHANNELS */

/* List of supported audio modules */
typedef enum {
    UNKNOWN,
    MP3,         /* ISO compliant */
    MP3_25,      /* ISO + fhg extension */
    AAC_ADTS,
    AAC_ADIF,   
    AAC_RAW,     /* No header or syncword */
    AAC_ENC,     /* MPEG2 */
    ADPCM,       /* speech codecs */
    RIFF,        /* WAVE files */
    BYPASS,
    WMA,
    PAC,
    AC3,
    MPEG2,
    MPEG2_MC,
    MPEG2_DAB,
    DTS,
    PCM,
    SBC,         /* BlueTooth audio codec */
    AMR,
    AMRWB,
    GSMFR,
    MIDIST,
    G723_1,
    REALAUDIO8,
    G711,
    G729,
    QCELP,
    G722,
    EVRC,
    AWP,
    G726,
    DEC_AAC,
    EAAC,
    ENC_G723_1,
    DEC_G723_1,
    ILBC,
    ENC_MP3,
    STMPEG,
    DEC_WMA,
    OGGVORBIS,
    GSMHR,
    DDPLUS,
    FLAC,
    MODE_LAST_IN_LIST
} AUDIO_MODE_T;

/* Sample structure */
/* audio module has to fill all fields */
typedef struct {
    int sample_freq;					// sample frequency (one defined into audiotables.h)
    int sample_size;					// sample size (16 or 24) 
    int headroom;						// number of guard bits
    int chans_nb;						// number of output channels
    int block_len;						// number of sample in the current frame  
    Float *buf_add;	 
    int samples_interleaved;			// decoder status//remove for clean purpose
    int out_pid;

}SAMPLE_STRUCT_T;


/* Bit stream structure */
/* audio module has to fill real_size_frame_in_bit (encoder mode) */
typedef struct {
	int real_size_frame_in_bit;		// number of significant bits (in case of varying frame size)
	BS_STRUCT_T bits_struct;		// set of pointers to read/write bit stream from/to FE fifos
									// (defined into bitstream_lib_proto.h)
}STREAM_STRUCT_T;

/* Audio Module (AM) state structure */
typedef struct {
	/* status flags */
	int first_time;			// set to 1 by UFE, reset by AM in case of specific first task 
	int output_enable;		// set to 1 by AM to indicate when output enable (decoder only to allow copy into fifo)
	int remaining_blocks;	// set to 0 by AM to indicate when all input buffer has been processed (decoder only)
	int status;				// set to error status level
	AUDIO_MODE_T mode; 		// store value found by codec_sync() since last codec_init. Written by UFE, read by AM 
    int bfi;                // if non zero, it means that the current frame is not correct (only updated with SHM)
	int eof;				// Set to TRUE by Codec when reaches end of process
	int sync_lost;			// set to TRUE by FE when synchronization is lost, reset to 0 by codec when decoding next frame
	int in_pid;             // WMA Input Packet Id
	int emit_buffers;       // Set to False if AM does not require the processed buffers to be emitted out at the o/p port.
	
}STATE_STRUCT_T;


/* FE entry point structure */
typedef struct {

	SAMPLE_STRUCT_T	sample_struct; 	// struct used as input for the encoders 
									// and as output for the decoders 
	STREAM_STRUCT_T stream_struct;	// struct used as output for the encoders 
									// and as input for the decoders 
	STATE_STRUCT_T codec_state; 	// audio module state structure

	unsigned int last_input_data;	// Flag to inform codec that no more new data are available
	
	void * codec_local_struct;			// audio module static memory 
	void * codec_config_local_struct;	// Optionnal field: codec may want to duplicate service_zone for its own processing.
	void * codec_info_local_struct;		// codec extra info for the api (bit-rate, modes...)
#ifdef _NMF_MPC_ 
    void * heap_info_table[MEM_BANK_COUNT];
#endif

}CODEC_INTERFACE_T;

/* List of supported header type */
typedef enum
{
    NO_HEADER_FIXED_SIZE,		// audio module with no header and fixed frame size 
	NO_HEADER_VARYING_SIZE,		// audio module with no header but varying frame size (multi rate or dtx enable)
    HEADER_ONCE,				// audio module with a header within first frame 
	HEADER_AT_EACH_FRAME,		// audio module with a header within each frame
	HEADER_PACKET ,              // audio module with packet granularity (wma)
	HEADER_UNKNOWN
} HEADER_TYPE_T;

/* codec return level of return value */
typedef enum {
    RETURN_STATUS_OK,
    RETURN_STATUS_WARNING,
    RETURN_STATUS_ERROR,
    RETURN_STATUS_FATAL_ERROR
} RETURN_STATUS_LEVEL_T;

#endif // _common_interface_h_ 


