

#ifndef _aac_h_
#define _aac_h_

#include "audiolibs_common.h"
#include "audiotables.h"
#include "audiowindows.h"
#include "common_interface.h"
#include "close.h"
#include "defines.h"
#ifdef ARM
#include "cortexa9_common.h"
#endif

#define BUFSIZE 5000 /*(6144*6+3000 )/8  */
#define AAC_MAX_BITSTREAM_SIZE_IN_BIT                      (15544)  // theory is 12288*3 but we need to increase size to pass some corrupted stream
#define AAC_SAMPLE_FRAME_SIZE_IN_WORD                      1024*2
#define EAACPLUS_SAMPLE_FRAME_SIZE_IN_WORD                 2048*2


enum {
	EAAC_ERROR_CONCEALMENT_OFF = 0,
	EAAC_ERROR_CONCEALMENT_MUTE,
	EAAC_ERROR_CONCEALMENT_REPEAT,
	EAAC_ERROR_CONCEALMENT_ADAPTIVE,
	EAAC_ERROR_CONCEALMENT_3GPP
};
	


extern RETURN_STATUS_LEVEL_T aac_decode_frame(CODEC_INTERFACE_T *interface_fe);


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

extern CODEC_INIT_T aac_init_memory(CODEC_INTERFACE_T *AAC_Interface);
extern int aac_adif_syncword(int hi, int lo, int *length);
extern int aac_adts_syncword(int hi, int lo, int *length);
extern int aac_adif_syncword_b24(int hi, int lo, int *length);
extern int aac_adts_syncword_b24(int hi, int lo, int *length);
extern int aac_adts_lc_syncword(int hi, int lo, int *length);
extern int aac_raw_syncword(int hi, int lo, int *length);
extern void eaacPlus_reset(CODEC_INTERFACE_T *interface_fe);
extern CODEC_INIT_T eaacPlus_open(CODEC_INTERFACE_T *eaacPlus_Interface);
extern RETURN_STATUS_LEVEL_T eaacPlus_decode_frame(CODEC_INTERFACE_T *interface_fe);
extern void eaacPlus_close(CODEC_INTERFACE_T *interface_fe);
extern int aac_getAudioSpecificConfig(void *hBs, void *p_global, unsigned short ASC_header_size);
extern void  aac_parse(CODEC_INTERFACE_T *interface_fe);

#ifdef __cplusplus
}
#endif // __cplusplus

/*--------------------- NEW --------------------------------------------------*/

// only syntax, profile, sample-freq and downSample are handled at this point 
typedef struct
{
	unsigned short syntax;				//  STATIC range 0(ADTS),1(ADIF),2(RAW), 3(RAW_STREAMING), 4(Autdetect ADIF or ADTS) 5(AutoDetect ADIF/ADTS/RAW) default 0(ADTS)
	unsigned short objectType;          //  STATIC (needed for RAW syntax only) values 2(LC),4(LTP), default 2(LC)
	unsigned short sample_freq;			//  STATIC (needed for RAW syntax only) 8->96kHz, default 48kHz
//	unsigned short max_input_channels;	  STATIC range [1,6], default 2(stereo) -> to reserve more or less memory
	unsigned short memory_preset;		// 0(MEM_DEFAULT)  1(MEM_ALL_DDR) 2(MEM_ALL_TCM)
										// 8(MEM_ALL_ESRAM) 9(MEM_MIX_ESRAM_DDR) 11(MEM_MIX_ESRAM_OTHER_1)
//	unsigned short max_blocks_per_frame; STATIC (needed for ADTS syntax only) range [1,4], default 1 -> to reserve more or less memory 
	unsigned short crc_ignore;			//  STATIC no action taken if crc error and crc_ignore=1 (def 1)
	unsigned short downSample;			//  STATIC range [0,1], default 0, down_sample if needed for freqs > 48kHz
	unsigned short EnableSBR;			//  STATIC range [0,1,2,3]
										// ---------------------------------------------------------------------------------------
  										// 0(default)=sbr not decoded with fs_out=fs_aac_core if iDownSample=0  
  										// else fs_out=fs_aac_core/2 if iDownSample=1;
										// ---------------------------------------------------------------------------------------
										// 1=sbr decoded with fs_out=2*fs_aac_core if iDownSample=0 else 1*fs_aac_core if 
										// iDownSample=1
										// ---------------------------------------------------------------------------------------
										// 2=same as 0 above
										// ---------------------------------------------------------------------------------------
										// 3=sbr decoded with fs_out=2*fs_aac_core only if fs_aac_core<=24kHz or iDownSample ==0
										// ---------------------------------------------------------------------------------------
	unsigned short currentProgram;      // for test ADIF only, not handled in hamaca
	unsigned short bsac_on;     		// STATIC: BSAC field - LC sub-group - range [0,1] - default 0 
	unsigned short bsac_nch;     		// STATIC: BSAC number of channels - valid in bsac_type only - range [1,2] - default 2
    unsigned short bsac_layer;     		// STATIC: BSAC bitrate decoding - valid if bsac_on and !bsac_usemaxlayer-range [0,48]-default 0
	unsigned short bsac_usemaxlayer;	// STATIC: BSAC bitrate decoding - valid if bsac_on - range [0,1] - default 1
	unsigned short concealment_on;      // STATIC: turns error concealment on - range [0-1] - default 0
	unsigned short fer;                 // for test only - frame error rate - range [0-100] - default 0
	unsigned short bad_bit_location_in_frame; // for test only, force bitstream errors starting at this location
	unsigned short badFrame;            // DYNAMIC: signals bad frame, applies error concealment - range [0-1] - default 0
	unsigned short shm;					// for test only, decodes shared memory files in standalone mode
    unsigned short DisableASC;          // disable ASC autodetection and keep raw data instead
//#ifdef MC
	unsigned int aac_config_map[MAX_OUTPUT_CHANNELS];
	unsigned int aac_ch_nb;
//#endif
    unsigned short ASC_header_size;      // size of ASC header if any
	int xx;
} AAC_DECODER_CONFIG_STRUCT_T;

/*----------------------------------------------------------------------------*/

typedef struct
{
    unsigned short data_valid;	// 0 as long as codec info not available, 1 otherwise
    unsigned short bitrate;	// value in bps/10 - NA for the moment
    unsigned short tns;		// Always 1
    unsigned short objectType;  
    unsigned short bsac_on;
    unsigned short sbr;
//#ifdef MC
	unsigned int ch_map[MAX_OUTPUT_CHANNELS];	
//#endif
} AAC_DECODER_INFO_STRUCT_T;

enum {
    AAC_ADTS_SYNTAX,
    AAC_ADIF_SYNTAX,
    AAC_RAW_SYNTAX,
    AAC_RAW_STREAMING_SYNTAX,
    AAC_ADIF_OR_ADTS_SYNTAX,
    AAC_AUTODETECT_SYNTAX
};

#define AAC_PROCESSING_BITSIZE 	24
#define AAC_RTIL_OK 0

#endif /* Do not edit below this point */
     
