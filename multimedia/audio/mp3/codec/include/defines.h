
 
#ifndef _defines_h_
#define _defines_h_


#ifdef NO_BUFFER
#define mp3_read_bit(a,b) drb_getbit(b);
#else
#define mp3_read_bit(a,b) mp3bs_read_bit(a,b);
#endif

// Comment out this line if you want the same scaling as Fhg for floating-point model
//#define SAME_SCALING_AS_FHG_REFERENCE

#define SS sizeof(short)
#define SF sizeof(float)



/********************* common layer definitions *****************************/

/***    SYNCH-WORD                                                        ***/


#define FhG_SYNCHLENGTH    11
#define ISO_SYNCHLENGTH    12

/***    Constants                                                         ***/

#define LAYER 4
#define BITRATE_INDEX 16
#define SAMPFREQ_INDEX 4
#define MPEG_INDEX 2
#define MPEG_ID_EX 2
#define SBLIMIT 32
/*** definition of LAYER-IDs ***/

#define LAYER_III 1

/*** definition of MPEG-Standard ***/

#define MPEG1_ID   1
#define MPEG2_ID   0

/*** definition of output channel id ***/


/*** definition of system and synchronization constants ***/

#define MAX_FRAMELENGTH      1932
//1932 = 1440+512( max maindatabegin)-17(side info for mono)-4(header)+1extra

//1728	/* The highest allowed framelength       */
/*  calculated from the highest allowed  */
/*  bitrate for Layer II/III, which both */
/*  have 1152 bits/frame, at 32 kHz.     */
/*  384 kbit/second at 32 kHz               */
/*  -> 12 bit/sample -> 13824 bits/frame */
#ifdef ARM
#define MAX_FRAMELENGTH_24   MAX_FRAMELENGTH/4    /* MAX_DYNPART_LENGT/24 */
#define MOD_BUF_SIZE         (MAX_FRAMELENGTH_24*4)
#else
#define MAX_FRAMELENGTH_24   MAX_FRAMELENGTH/3    /* MAX_DYNPART_LENGT/24 */
#define MOD_BUF_SIZE         (MAX_FRAMELENGTH_24*3)
#endif



#define MAX_GRAN_LENGTH       7680	/* maximum allowed dynamic part length   */
/* of a granule                          */
#define MAX_DYNPART_LENGTH   11352+4096	/* maximum allowed dynamic part length   */
/* 320kBit, 32kHz = 11520 bits/frame     */
/* - header (36bits) - SI mono (136bits) */

/*** return codes of the 'synchronize' function ***/




/*** definition of modes ***/

#define MP3_BUFSIZE 2000// 2000 bytes as max framelen ~1932 bytes
#define MONO_CH     1
#define STEREO_CH   2

#define MP1_FRAMELEN 384

#define STEREO         0
#define JOINT_STEREO   1
#define DUAL_CHANNEL   2
#define SINGLE_CHANNEL 3

#define NORMAL_STEREO  0
#define INTENSITY_ONLY 1
#define MS_ONLY        2
#define INTENSITY_MS   3

#define ILLEGAL_IS_POS 31

#define MPEG2_CHAN  5
#define ML_CHAN         7

#define NTSC    0

#define CHAN 2
#define GRAN 2
#define TRANS_FAK 3
#define REGIONS_S 2
#define REGIONS_L 3
#define HYB_LENGTH 576

#define SCF_BANDS_L       21
#define SCF_BANDS_S       (TRANS_FAK*12)
#define SCF_BANDS_L_SPLIT 8
#define SCF_BANDS_L_SPLIT_M2 6
#define SCF_BANDS_S_SPLIT TRANS_FAK*9
#define MAX_SCF_BANDS     (SCF_BANDS_S+TRANS_FAK)

#define NB_BANDS_S 13
#define NB_BANDS_L 22

#define SCFSI_BANDS 4
#define SCF_MPEG2_PART 4

#define BLOCK_MODES 3

#define NORM_TYPE 0		/* Normal MDCT Block */
#define START_TYPE 1		/* MDCT Start Block */
#define SHORT_TYPE 2		/* MDCT Short Block */
#define STOP_TYPE 3		/* MDCT Stop Block */

#define IBLEN HYB_LENGTH	/* Block Size */

#define CHAN_L2 2
#define GRAN_L2 12

#define SCF_CNT 3
#define CODE_CNT 3
#define SUBBAND 32
#define POLYSTEP 18

/*** structure for bitstream SI-definition                                  */

typedef struct {
    unsigned short  field_length;	/* length of entry in bit           */
    unsigned short  repeat;	/* #times to repeat (for vectors)   */
    unsigned short *flag;	/* decoding-condition flag          */
    unsigned short  invert;	/* invert condition-flag, if = 1    */
    unsigned short *pdest;	/* pointer to entry-variable        */



    /* pointer to a function, which is  */
    /* performed after decoding. Its    */
    /* parameter ist the entry-variable- */
    /* pointer                          */
} BITSTREAMDEF;


#ifndef NULL
#define NULL 0
#endif

typedef Float UNION_STORE[HYB_LENGTH];

typedef struct {
    unsigned int *byte_ptr;
    int nb_bits;
} STRUCT_24BIT;


#define BITSTREAMDEF_SIZE 16
#define DATA_BUF_SIZE (MAX_FRAMELENGTH)
#define DATA_BUF_SIZE_24 (MAX_FRAMELENGTH_24)


typedef struct {
    int            data[DATA_BUF_SIZE_24];	/* data buffer                  */
    unsigned int *byte_ptr_read;
    unsigned int *byte_ptr_write;
    int nb_octet_read;
    int nb_octet_write;
    unsigned short  data_valid;	/* #valid bytes in buffer       */

    unsigned short  act_data_length;	/* length of actual dynpart     */
    unsigned int* begin_ptr;
    unsigned int* end_ptr;

} DYNAMIC_PART_BUFFER_24;



typedef struct {
    int  valid_bits;	/* number of valid bits in byte     */
    STRUCT_24BIT struct24;	/* pointer to buffer where to write */
    int  bit_count;	/* counts encoded bits              */
    unsigned int *begin_ptr;
    unsigned int *end_ptr;

} BIT_BUF_24;

/*** structure for bitstream buffer (modulo)                                */


typedef struct {
    unsigned int  *byte_ptr;
    int  nb_octet;
} STRUCT_MOD_24BIT;


typedef struct {
    unsigned short  valid_bits;	/* number of valid bits in byte     */
    STRUCT_24BIT  struct24;	/* pointer to buffer where to write */
    unsigned short  bit_count;	/* counts encoded bits              */
    unsigned int  *begin_ptr;	/* pointer to first buffer byte     */
    unsigned int  *end_ptr;	/* pointer to last buffer byte      */


} MOD_BUF_24;


typedef struct {
    BS_STRUCT_T bs_buf_write;
    BS_STRUCT_T bs_buf_read;
    unsigned int    data[DATA_BUF_SIZE_24];
    unsigned int    bit_count;
} DATA_STRUCT;


/*** definition of the header struct ***/

typedef struct {
    unsigned short  synch;	/*   1  */
    unsigned short  FhG_id;	/*   2  */
    unsigned short  id;		/*   3  */
    unsigned short  layer;	/*   4  */
    unsigned short  protection_bit;	/*   5  */
    unsigned short  bitrate_index;	/*   6  */
    unsigned short  sampling_frequency;	/*   7  */
    unsigned short  padding_bit;	/*   8  */
    unsigned short  private_bit;	/*   9  */
    unsigned short  mode;	/*  10  */
    unsigned short  mode_extension;	/*  11  */
    unsigned short  copyright;	/*  12  */
    unsigned short  original_home;	/*  13  */
    unsigned short  emphasis;	/*  14  */

    unsigned short  crc_check;	/*  15  */
    unsigned short  next_main_data_begin;	/*  16  */
    unsigned short  private_bits;	/*  17  */

    unsigned short  header_bitlength;	/*  18  */
    unsigned int    bits_in_frame;	/*  19/20  */
    unsigned int    frame_cnt;	/*  21/22  */
    unsigned long   synch_file_pos;	/*  23  */

} HEADER;



/************************* layer II definitions ***************************/


typedef	struct SI_L3_CHGR {

    unsigned short  part2_3_length;
    unsigned short  big_values;
    unsigned short  global_gain;
    unsigned short  scalefac_compress;
    unsigned short  window_switching_flag;
    unsigned short  block_type;
    unsigned short  mixed_block_flag;
    unsigned short  table_select[REGIONS_L];
    unsigned short  subblock_gain[TRANS_FAK];
    unsigned short  region0_count;
    unsigned short  region1_count;
    unsigned short  preflag;
    unsigned short  scalefac_scale;
    unsigned short  count1table_select;

    unsigned short  block_mode;
    unsigned short  region_pairs[REGIONS_L];
    unsigned short  cutoff;

    unsigned char  *band_width_ptr;
    short           no_of_bands;
    short           no_of_short_tri;
    short           slen[SCF_MPEG2_PART];	/* # of bits for scf for each region */
    short           nr_of_sfb[SCF_MPEG2_PART];	/* # of scalefactors for each region */
    short           intensity_scale;


} SI_L3_CHGR;

typedef struct SI_L3_CH {
    unsigned short  scfsi[SCFSI_BANDS];
    SI_L3_CHGR gr[GRAN];
} SI_L3_CH;

typedef struct SI_L3 {
    unsigned short  main_data_begin;
    unsigned short  private_bits;
    SI_L3_CH ch[CHAN + NTSC];
} SI_L3;


typedef union {
    SI_L3           l3;
} SI;


/***    scalefactorband structure table struct                              */

typedef struct {
    short           scf_slen1_l;	/* #long block scalef. with length slen1 */
    short           scf_slen2_l;	/* #long block scalef. with length slen2 */
    short           scf_slen1_s;	/* #short block scalef. with length slen1 */
    short           scf_slen2_s;	/* #short block scalef. with length slen2 */
    /* #scalefactors in each scfsi-band      */
    short           scfsi_band_width[SCFSI_BANDS];

} SCF_DEC_INFO;



/***   scalefactor bandwidth table structure                                 */


typedef struct {
    short           no_of_scf_bands;	/* #scalefactor bands                */
    short           no_of_short_tri;	/* #of short block triples           */
    unsigned char   width[MAX_SCF_BANDS];	/* width of scalefactor band         */
} SCF_BAND_INFO;


typedef short SCF_TYPE[GRAN][MAX_SCF_BANDS];

typedef Float GRAN_TYPE[CHAN][HYB_LENGTH];
typedef Float  IMDCT_PREV_BLK_TYPE[HYB_LENGTH];


typedef unsigned char SCF_BAND_INFO_TAB[MAX_SCF_BANDS];

typedef struct {

    SCF_TYPE *mp3_scalefacs;
    GRAN_TYPE *mp3_granule_buffer;

    int      mp3_polysyn_offset[2];
    Float   *mp3_polysyn_fifo[2];

    IMDCT_PREV_BLK_TYPE *mp3_prevblck;

    SI      *mp3_si;
    int      mp3_upsampling;
    int data_valid;
    unsigned short    mp3_max_ch;
    unsigned short    mp3_max_gr;
    DATA_STRUCT *data_struct;

    UNION_STORE    *mp3_ustore;

    SCF_BAND_INFO_TAB *mp3_scf_band_info_tab;
    unsigned short  mp3_FhG_synch_flag;
    int             mp3_toggle_buffer;
    HEADER          mp3_Head;		/* for def_header definition */
    unsigned short  mp3_new_mpeg2_detect;	/* for def_header definition */
    SI_L3_CHGR mp3_tmp_si_chgr_l3;
    unsigned short static_FhG_id;
    unsigned short static_id;
    unsigned short static_sampling_freq;
    short prev_gr_mute_flag;
    short prev_gr_data_avail;
#ifdef PARTIAL_FRAME_HANDLING
	int partial_frame;
	unsigned int bitstream_save[MP3_BUFSIZE>>2];
	int saved_bufsize;
	int bytes_notcopied;
	int offset;
#endif
#ifndef MMDSP
	int gsblimitj;
	int	gsblimitr;
	int gJSBound;
	int gindex1;
	int gindex2;
    int gsblimit;	
	int gBitAlloc[SBLIMIT * 2];
	int gDScaleIndex[SBLIMIT * 2 *3 ];// 2 channels
	int *gQuanLevel[SBLIMIT];
	int gScfSi[SBLIMIT * 2];
	BITSTREAMDEF mp3_header[BITSTREAMDEF_SIZE];
	BITSTREAMDEF mp3_tmp_si_chgr_BD[BITSTREAMDEF_SIZE];
#endif
} MP3_DECODER_STRUCT_T;

#endif  /* Do not edit below this line */













