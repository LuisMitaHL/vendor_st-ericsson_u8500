/************************* MPEG-2 NBC Audio Decoder **************************
 *                                                                           *
"This software module was originally developed by
AT&T, Dolby Laboratories, Fraunhofer Gesellschaft IIS and edited by
Yoshiaki Oikawa (Sony Corporation),
Mitsuyuki Hatanaka (Sony Corporation),
in the course of development of the MPEG-2 NBC/MPEG-4 Audio standard ISO/IEC 13818-7,
14496-1,2 and 3. This software module is an implementation of a part of one or more
MPEG-2 NBC/MPEG-4 Audio tools as specified by the MPEG-2 NBC/MPEG-4
Audio standard. ISO/IEC  gives users of the MPEG-2 NBC/MPEG-4 Audio
standards free license to this software module or modifications thereof for use in
hardware or software products claiming conformance to the MPEG-2 NBC/MPEG-4
Audio  standards. Those intending to use this software module in hardware or
software products are advised that this use may infringe existing patents.
The original developer of this software module and his/her company, the subsequent
editors and their companies, and ISO/IEC have no liability for use of this software
module or modifications thereof in an implementation. Copyright is not released for
non MPEG-2 NBC/MPEG-4 Audio conforming products.The original developer
retains full right to use the code for his/her  own purpose, assign or donate the
code to a third party and to inhibit third party from using the code for non
MPEG-2 NBC/MPEG-4 Audio conforming products. This copyright notice must
be included in all copies or derivative works."
Copyright(c)1996.
 *                                                                           *
 ****************************************************************************/

#ifndef	_aac_local_h_
#define _aac_local_h_

/* include generic header files */
#include "aac.h"

/* include aac specific defines file */
#include "defines.h"

/* include memory specifiers */
#include "aac_memconfig.h"

#ifdef NEW_ADTS
#define MAX_RDB_PER_FRAME	4
#endif /* NEW_ADTS */

#ifndef BLOCK_LEN_LONG	 
#define BLOCK_LEN_LONG	 1024
#endif

#define MONO_CHAN 0
#define MAX_TF_LAYER 10
#define MAX_TIME_CHANNELS 2
#define MIN_CORRECT_ADTS_HEADER 3 // min 3 headers to get fixed header

enum {
    /* block switch windows for single channels or channel pairs */
    Winds = Chans,

    /* max length of DSE, bytes */
    MAX_DBYTES = ((1 << LEN_D_CNT) + (1 << LEN_D_ESC)),

    TEXP = 128,			/* size of exp cache table */
    MAX_IQ_TBL = 128,		/* size of inv quant table */
    MAXFFT = LN4,

    XXXXX
};

struct ArDecoderStr {
  long value;  /* Currently-seen code value           */
  long range;  /* Size of the current code region */
  int cw_len;      /* estimated codeword length */
};

typedef struct ArDecoderStr ArDecoder;

typedef struct 
{
	ArDecoder ArDec[64];
	long Value;
	long Range;
	int Est_cw_len;
} ArDecoder_composite;

typedef struct {
    int             islong;	/* true if long block */
    int             nsbk;	/* sub-blocks (SB) per block */
    int             bins_per_bk;/* coef's per block */
    int             sfb_per_bk;	/* sfb per block */
    int             bins_per_sbk[MAX_SBK];	/* coef's per SB */
    int             sfb_per_sbk[MAX_SBK];	/* sfb per SB */
    int             sectbits[MAX_SBK];
    SHORT   const SR_MEM *sbk_sfb_top[MAX_SBK];	/* top coef per sfb per SB */
    SHORT          *sfb_width_128;	/* sfb width for short blocks */
    SHORT          *bk_sfb_top;	        /* cum version of above */
    int             num_groups;
    SHORT           group_len[8];
    SHORT           group_offs[8];
}               Info;

typedef struct {
    int             pulse_data_present;
    int             number_pulse;
    int             pulse_start_sfb;
    int             pulse_position[NUM_PULSE_LINES];
    int             pulse_offset[NUM_PULSE_LINES];
    int             pulse_amp[NUM_PULSE_LINES];
}               Pulse_Info;

typedef struct {
    int             samp_rate;
    int             nsfb1024;
    SHORT const SR_MEM   *SFbands1024;
    int             nsfb128;
    SHORT const SR_MEM   *SFbands128;
}               SR_Info;

#ifndef BIT_FIELDS
typedef struct {
    int             present;	/* channel present */
    int             tag;	/* element tag */
    int             cpe;	/* 0 if single channel, 1 if channel pair */
    int             common_window;	/* 1 if common window for cpe */
    int             ch_is_left;	/* 1 if left channel of cpe */
    int             paired_ch;	/* index of paired channel in cpe */
    int             widx;	/* window element index for this channel */
    int             is_present;	/* intensity stereo is used */
    int             ncch;	/* number of coupling channels for this ch */

#if (CChans > 0)
    int             cch[CChans];/* coupling channel idx */
    int             cc_dom[CChans];	/* coupling channel domain */
    int             cc_ind[CChans];	/* independently switched coupling
					 									channel flag */
#endif

    char           	*fext;	/* filename extension */
}               Ch_Info;

typedef struct {
    int             nch;	/* total number of audio channels */
    int             nfsce;	/* number of front SCE's pror to first front CPE */
    int             nfch;	/* number of front channels */
    int             nsch;	/* number of side channels */
    int             nbch;	/* number of back channels */
    int             nlch;	/* number of lfe channels */
    int             ncch;	/* number of valid coupling channels */
    int             cc_tag[(1 << LEN_TAG)];	/* tags of valid CCE's */
    int             cc_ind[(1 << LEN_TAG)];	/* independently switched CCE's */
    int             profile;
    int             sampling_rate_idx;
    Ch_Info         ch_info[Chans];
}               MC_Info;


			 
typedef struct {
    int             num_ele;
    int             ele_is_cpe[(1 << LEN_TAG)];
    int             ele_tag[(1 << LEN_TAG)];
}               EleList;

typedef struct {
    int             present;
    int             ele_tag;
    int             pseudo_enab;
}               MIXdown;
#else /* BIT_FIELDS */

typedef struct {
    unsigned int             present:LEN_CH_PRESENT;	/* channel present */
    unsigned int             tag:LEN_TAG;	/* element tag */
    unsigned int             cpe:LEN_IS_CPE;	/* 0 if single channel, 1 if channel pair */
    unsigned int             common_window:LEN_COM_WIN;	/* 1 if common window for cpe */
    unsigned int             ch_is_left:LEN_CH_IS_LEFT;	/* 1 if left channel of cpe */
    unsigned int             paired_ch:LEN_NUM_CH;	/* index of paired channel in cpe */
    unsigned int             widx:LEN_NUM_CH;	/* window element index for this channel */
    unsigned int             is_present:LEN_CH_IS_PRESENT;	/* intensity stereo is used */
    unsigned int             ncch:LEN_NUM_CH;	/* number of coupling channels for this ch */

#if (CChans > 0)
    unsigned int             cch[CChans];/* coupling channel idx */
    unsigned int             cc_dom[CChans];	/* coupling channel domain */
    unsigned int             cc_ind[CChans];	/* independently switched coupling
					 												channel flag */
#endif

    char           			*fext;	/* filename extension */
}               Ch_Info;


typedef struct {
    int             nch;	/* total number of audio channels */
    int             nfsce;	/* number of front SCE's pror to first front CPE */
    int             nfch;	/* number of front channels */
    int             nsch;	/* number of side channels */
    int             nbch;	/* number of back channels */
    int             nlch;	/* number of lfe channels */
    int             ncch;	/* number of valid coupling channels */
    int             cc_tag[SIZE_TAGS];	/* tags of valid CCE's */
    int             cc_ind[SIZE_TAGS];	/* independently switched CCE's */
    int             profile;
    int             sampling_rate_idx;
    Ch_Info         ch_info[Chans];
}               MC_Info;

typedef struct {
    int             num_ele;
    int             ele_is_cpe[SIZE_BOOLEAN_TAGS];
    int             ele_tag[SIZE_TAGS];
}               EleList;

typedef struct {
    unsigned int             present:LEN_MIX_PRES;
    unsigned int             ele_tag:LEN_TAG;
    unsigned int             pseudo_enab:LEN_PSUR_ENAB;
}               MIXdown;
#endif /* BIT_FIELDS */

typedef struct {
    int             profile;
    int             sampling_rate_idx;
    EleList         front;
    EleList         side;
    EleList         back;
    EleList         lfe;
    EleList         data;
    EleList         coupling;
    MIXdown         mono_mix;
    MIXdown         stereo_mix;
    MIXdown         matrix_mix;
    char            comments[SIZE_ANCILLARY];
    long            buffer_fullness;	/* put this transport level info here */
}               ProgConfig;

typedef struct {
    char            adif_id[LEN_ADIF_ID + 1];
    int             copy_id_present;
    char            copy_id[LEN_COPYRT_ID + 1];
    int             original_copy;
    int             home;
    int             bitstream_type;
    long            bitrate;
    int             num_pce;
    int             prog_tags[(1 << LEN_TAG)];
}               ADIF_Header;

#ifdef	ADTS
typedef struct {
#ifdef NEW_ADTS
    int  			syncword;
    int  			frame_length;
    int  			adts_buffer_fullness;
    int  			crc_check;
    int            	id;
    int            	layer;
    int            	protection_abs;
    int            	profile;
    int            	sampling_freq_idx;
    int            	private_bit;
    int            	channel_config;
    int            	original_copy;
    int            	home;
    int            	copyright_id_bit;
    int            	copyright_id_start;
    int            	num_of_rdb;
	int  			rdb_position[MAX_RDB_PER_FRAME - 1];
#ifdef ARM
	int             fixed_header;
	int             header_success;
#endif
#else
    int             syncword;
    int             id;
    int             layer;
    int             protection_abs;
    int             profile;
    int             sampling_freq_idx;
    int             private_bit;
    int             channel_config;
    int             original_copy;
    int             home;
    int             copyright_id_bit;
    int             copyright_id_start;
    int             frame_length;
    int             adts_buffer_fullness;
    int             num_of_rdb;
    int             crc_check;
#endif
}               ADTS_Header;
#endif


#define _WINDOW_TYPE_EXT
typedef UCHAR WINDOW_TYPE_EXT;


enum return_status {
    AAC_OK = 0,
    
    AAC_CRC_ERROR,                     // 1
    AAC_SBR_NOT_INITIALIZED,
    AAC_LC_TYPE_INSTEAD_OF_LTP,
    
    AAC_ILLEGAL_ADIF_HEADER,           // 4
    AAC_ILLEGAL_PROGRAM,               // 5
    AAC_ILLEGAL_TAG,                   // 6
    AAC_ILLEGAL_CHN_CONFIG,            // 7
    AAC_ILLEGAL_SECTION,               // 8 
    AAC_ILLEGAL_SCFACTORS,             // 9
    AAC_ILLEGAL_PULSE_DATA,            // 10
	AAC_GC_NOT_IMPLEMENTED,            // 12
    AAC_MAIN_PROFILE_NOT_IMPLEMENTED,  // 11   
	AAC_INIT_FAILURE_MALLOC,       // 13
    AAC_BSAC_ZERO_LENGTH_FRAME,	       // 14
    AAC_LTP_TYPE_INSTEAD_OF_LC,	       // 15
    AAC_WARNING = AAC_GC_NOT_IMPLEMENTED,
    AAC_ERROR = AAC_MAIN_PROFILE_NOT_IMPLEMENTED,
    AAC_FATAL_ERROR = AAC_ILLEGAL_ADIF_HEADER
};

typedef enum {
	AAC_ALL_TCM,
	AAC_ALL_ESRAM, 
	AAC_MIX_TCM_DDR, 
	AAC_MIX_ESRAM_DDR, 
	AAC_ALL_DDR, 
	AAC_MIX_TCM_DDR_ESRAM,
	AAC_NUMBER_OF_PRESETS
}AAC_ALLOC_MEMPRESET_TYPE_ENUM_T;

#define AAC_BAD_PRESET AAC_NUMBER_OF_PRESETS

/* prototype includes for libraries outside aac project */

#include "vector.h"

#include "mp3dequan.h"
#include "mpeg_crc.h"

#include "arm_porting.h"

/* prototype includes for libraries within aac project */
#include "filter.h" 
#include "vector_gc.h"
#include "gc.h"

/* prototype includes */
#include "getbits.h"
#include "tns.h"
#ifdef MPEG4V1LTP
#include "nok_ltp_common.h"
#endif
#ifdef ERROR_CONCEALMENT
#include "conceal.h"
#endif /* ERROR_CONCEALMENT */
#include "decdata.h"
#include "coupling.h"
#include "huffman_table.h"
#include "decode_book.h"
#include "tns_table.h"
#include "pulse.h"
#include "debug.h"
#include "aac_memory.h"
#include "drc.h"
#include "scale.h"
#include "bitfield.h"
#include "crc.h"
#include "aac_extern_ram.h"
#ifdef SBR /* include sbr structs */
#include "sbrdecoder.h"
#endif
#include "extension.h"

typedef struct {
	Float   *aac_state[Chans];
	UCHAR    aac_hasmask[Winds];
	UCHAR   *aac_mask[Winds];
	UCHAR   *aac_group[Chans];
	UCHAR    aac_wnd[Chans];
	UCHAR    aac_max_sfb[Chans];
	UCHAR   *aac_cb_map[Chans];
	UCHAR    aac_d_bytes[SIZE_ANCILLARY];
	Wnd_Shape aac_wnd_shape[Chans];
	SHORT   *aac_factors[Chans];
	TNS_frame_info *aac_tns[Chans];
	UCHAR    aac_wnd_prev_sequence[Chans-XChans];
	int         aac_toggle_control;

#if (CChans > 0)
	Float   *aac_cc_coef[CChans];
	Float   *aac_cc_gain[CChans][Chans];
	UCHAR    aac_cc_wnd[CChans];
	Wnd_Shape aac_cc_wnd_shape[CChans];
	int     *aac_cc_exp[CChans];
	
#if (ICChans > 0)
	Float   *aac_cc_state[ICChans];
#endif
	
#endif
#ifdef  SSR
	int             aac_ipqf_fifo_pos[Chans - XChans];
	Float          *aac_ipqf_fifo[Chans - XChans];
	GAIN_CONTROL_DATA *aac_g_info[Chans];
	GAIN_CONTROL_DATA_SAVE *aac_g_info_prev[Chans - XChans];
#endif	
#ifndef MPEG4V1LTP

#ifdef MAIN_PROFILE
/* prediction */
	PRED_STATUS *aac_sp_status[Chans];
	int      aac_last_rstgrp_num[Chans];
#endif				/* MAIN_PROFILE */
	
#else
/* long term prediction */
	NOK_LT_PRED_STATUS *aac_nok_lt_status[Chans];
	Float 	   *ltp_scratch_buffer;					
#endif				/* MPEG4V1LTP */


	int             aac_maxfac;                           
	int             aac_default_config;                   


#ifdef   ADTS                                     
	int             aac_adts_header_present;              
	ADTS_Header     aac_adts_header;
#endif                                            

	int             aac_current_program;                  
	ADIF_Header     aac_adif_header;                      
	ProgConfig      aac_prog_config;                      
	MC_Info         aac_mc_info;
	MC_Info         aac_save_mc_info;
	int             aac_debug[256];                       
	Info            aac_only_long_info;                   
	Info            aac_eight_short_info;                 
	SHORT           aac_long_scf_bands[MAXBANDS_LONG];    
	SHORT           aac_short_scf_bands[MAXBANDS];        
	Info           *aac_winmap[NUM_WIN_SEQ];              
	SHORT           aac_sfbwidth128[(1 << LEN_MAX_SFBS)]; 
	Pulse_Info      aac_pulse_info;
#ifdef _NMF_MPC_

Float *			aac_output_buffer;
#else
#ifdef ARM
#ifndef SBR
	Float           aac_output_buffer_aligned[(Chans - XChans) * (LN2+4)];
	Float           *aac_output_buffer;
#else // SBR compiled

	Float           aac_output_buffer_aligned[(Chans - XChans) * (LN+4)];//__attribute__((aligned(16)));
	Float           *aac_output_buffer;//[(Chans - XChans) * (LN+4)];//__attribute__((aligned(16)));
#endif
#else /* ARM */
#ifndef SBR
	Float           aac_output_buffer[(Chans - XChans) * LN2];
#else // SBR compiled

	Float           aac_output_buffer[(Chans - XChans) * LN];
#endif /*  SBR */
#endif /* ARM */
#endif

	Float          *aac_scratch_buffer;
#ifdef MPEG4BSAC
	Float		   *aac_scratch_buffer_aux;
#endif
	Float           YMEM *aac_scratch_buffer_y;
	Float 			YMEM *coefslpc_y;
	int      		num_raw_data_blocks;
#ifdef MC
	int             prev_ch;
#endif
	int				first_cpe;
	unsigned short aac_crc_checksum;
	BS_STRUCT_T savedBs;
	int nblkinh;
#ifdef MPEG4V1LTP
	int				noise_present;/* noise energy */
	int				ltp_present;/* LTP data */
	int				config_ltp;
	int				stream_is_mpeg4;/* is one of PNS and LTP used?*/
#ifdef USELIB_ISOMP4
	int     		mpeg4_header_present;
#endif
#endif
#ifdef MPEG4V1PNS
	long			seed;
#endif
	int				decim_enbl;
	int const HUF_MEM *base;
	int const *plut;
#ifdef MPEG4BSAC
    unsigned short bsac_on;
    unsigned short bsac_nch;
    unsigned short bsac_layer;
	unsigned int  *bs_buf;
	BS_STRUCT_T bsbufs;
	ArDecoder_composite *ARDEC;
	WINDOW_SEQUENCE windowSequence[2]; 
	SAM_WINDOW_SHAPE windowShape[2];
#endif

#ifdef SBR
    /* sbr part */
    unsigned short EnableSBR;
	unsigned short Enable_SBR_Auto_Decimation_for_fs_above24kHz;
	unsigned short bUseLP;
	unsigned short PS_STEREO; 
    unsigned short bitstreamDownmixSBR;
    SBRBITSTREAM *pStreamSbr;
    
    SBR_DECODER_INSTANCE *psbrDecoderInstance; /* pointer to struct */
    SBR_INIT_STATUS sbrInitStatus;

    Float *sbr_scratch_buffer;
#ifdef SBR_MC
	Float *sbr_InterimResult_buffer;
	Float *sbrFrameData_buffer;
#endif

#endif
#ifdef ERROR_CONCEALMENT
	int            concealment_on;
	aacConcealmentInfo *pConcealmentInfo[Chans - XChans];
	 MC_Info         aac_mc_info_conceal;
#endif /* USE_ERROR_CONCEALMENT */
	int samples_interleaved;
	int ch;
	int nch_aac_core;
	int Force_Main_Profile_as_LC;
	int nbadframes;
	int badbitsconsumed;
	int nbgoodframes;
	int avgbits;
	int raw_error;
	unsigned long frames;
	int last_frame_size_in_bit;
	int last_status;
	int last_report_bfi;
	int report_bfi;
	int AudioSpecificConfigChecked;
    int DisableASC;    
	int EnableSBR_CHANNEL;
	int header_channel_no;
#ifdef SBR_MC
    int aac_current_ch_position;
#endif
} AAC_DECODER_STRUCT_T;

extern	int             debug[256];                       

#include "dequan.h"
#include "huffman.h"
#include "monopred.h"
#include "port.h"

#ifdef MPEG4V1LTP
#include "nok_lt_prediction.h"
#endif

#if defined(SSR)&&defined(IMDCT_SIZES_2048_AND_256_ONLY)
#error "IMDCT library compiled with imdct size 2048 and 256 only, SSR profile requires 64 and 512 as well"
#endif

#endif				/* _aac_h_ */





