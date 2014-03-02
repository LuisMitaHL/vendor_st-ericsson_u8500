
#ifndef _mp3_local_h_
#define _mp3_local_h_


typedef enum {

    /* success */
    MP3_OK                               =0x0,

    /* warning */
    MP3_CRC_ERROR                        =0x1,
    MP3_CUTOFF_ERROR                     =0x2,
    MP3_BIG_VALUE_ERROR                  =0x4,
    MP3_HUFFTABLE_ERROR                  =0x8,

    MP3_MOD_BUF_SIZE_ERROR               =0x10,
    MP3_HUFFMAN_DECODE_ERROR             =0x20,
    MP3_GR_LENGTH_ERROR                  =0x40,
    MP3_CH_LENGTH_ERROR                  =0x80,
    MP3_INPUT_BIT_AVAILABLE_ERROR        =0x100,
    MP3_HEAD_FRAMELENGTH_ERROR           =0x200, // detected in mp3_check_header
    MP3_DYNPART_LENGTH_ERROR             =0x400,

    MP3_BLOCK_TYPE_ERROR                 =0x800,
    MP3_HEAD_EMPHASIS_ERROR              =0x1000, // never checked
    MP3_HEAD_LAYER_ERROR                 =0x2000, // checked both in synchro and check_header()
    MP3_HEAD_SAMP_FREQ_ERROR             =0x4000, // checked both in synchro and check_header()
    MP3_DYNPART_PAST_DATA_MISSING	 =0x8000, // means past frame is missing in dynpart buffer and begin_main_data > 0

    /* error */
    MP3_DYNPART_EXCHANGE_ERROR           =0x10000,
    MP3_ILLEGAL_MODE                     =0x20000,// means synchro was not found -> error in header
    MP3_STATIC_PARAMS_CHANGED            =0x40000,
    MP3_BFI_FIRST_FRAME                  =0x80000,
	MP3_NO_ENOUGH_MAIN_DATA_ERROR        =0x100000,// partial frame is sent.
	MP3_NEXT_SYNC_NOT_FOUND              =0x200000,


    MP3_WARNING                          = MP3_CRC_ERROR,
    MP3_ERROR                            = MP3_DYNPART_EXCHANGE_ERROR
    //MP3_FATAL_ERROR                      = MP3_BLOCK_TYPE_ERROR

}MP3_DEC_ERROR_ENUM_T;

typedef enum {

	  MP3_DEC_OK = MP3_OK,
	  MP3_DEC_WARNING =  MP3_WARNING,
	  MP3_DEC_ERROR = MP3_ERROR//,
	  //MP3_DEC_FATAL_ERROR = MP3_FATAL_ERROR
}MP3_DEC_LEVEL_ENUM_T;



#endif /* Do not edit below this line */
