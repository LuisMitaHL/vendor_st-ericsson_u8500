
 
#ifndef _mp3_h_
#define _mp3_h_

#include "audiolibs_common.h"
#include "bitstream_lib_proto.h"
#include "common_interface.h"
//relative path provided for mop500_ed-linux platfrorm as vector.h is defined from  both audiolibs as well as /prj/multimedia_shared/common/STLinux/cortex/devkit/arm/bin/../target/usr/include/c++/4.2.3
#include "vector.h" 
#include "polysyn.h"
#include "mp3hybrid.h"
#include "mp3dequan.h"
#include "mpeg_crc.h"
#include "audioutils.h"
#include "mp3_local.h"


#ifdef ARM
#include "cortexa9_common.h"
#endif

#ifndef MP3_API_IMPORT
	#ifdef __SYMBIAN32__
		#define MP3_API_IMPORT IMPORT_C
	#else
		#define MP3_API_IMPORT
	#endif
#endif
#ifndef MP3_API_EXPORT
	#ifdef __SYMBIAN32__
		#define MP3_API_EXPORT EXPORT_C
	#else
		#define MP3_API_EXPORT
	#endif
#endif

typedef struct {
    // Status
    unsigned short bitrate_lsb;
    unsigned short bitrate_msb;
    
    //Other information
    AncillaryData  *ancillary_data;
    //int private_data[_PRIVATE_DATA_SIZE];
    unsigned short memory_preset;    // 0(MP3_ALL_TCM) 1(MP3_MIX_MEM) 2(MP3_NO_TCM)
    unsigned short ErrorConcealment;
} MP3_DECODER_CONFIG_STRUCT_T;

typedef struct {
    unsigned int data_valid; // 0 as long as codec info not available, 1 otherwise
    unsigned int bitrate;    // value in bps/10
} MP3_DECODER_INFO_STRUCT_T;


// Comment this line to use plain MP3
//#define ALLOW_FOR_MP3PRO

/* Comment out this line if sbr static data are allocated outside mp3_heap */
// #define SBR_STATIC_DATA_IN_MP3_HEAP

/* Comment out this line if sbr dynamic data are allocated outside mp3_heap */
//#define SBR_DYNAMIC_DATA_IN_MP3_HEAP


//extern const int mp3_decoder_heap_size;
//extern const int mp3_decoder_struct_size;

/* description of ISO MP3 */
//extern DecoderDescription const DESC_MEM mp3_description;

//extern DecoderDescription const DESC_MEM mp3_2_5_description;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

MP3_API_IMPORT int                   mp3_syncword(int hi, int lo, int *length);
MP3_API_IMPORT int                   mp3_2_5_syncword(int hi,int lo,  int *length);
#ifdef ARM
MP3_API_IMPORT int                   mp3_syncword_32b(int hi, int lo, int *length);
MP3_API_IMPORT int                   mp3_2_5_syncword_32b(int hi,int lo,  int *length);
#endif

MP3_API_IMPORT CODEC_INIT_T          mp3_init(CODEC_INTERFACE_T *itf_cfg);
MP3_API_IMPORT CODEC_INIT_T          mp3_decode_init_malloc(CODEC_INTERFACE_T *itf);
extern MP3_API_IMPORT RETURN_STATUS_LEVEL_T mp3_decode_frame(CODEC_INTERFACE_T *interface_fe);
MP3_API_IMPORT void                  mp3_reset(CODEC_INTERFACE_T *itf);
MP3_API_IMPORT void                  mp3_reset_1(CODEC_INTERFACE_T *itf);
MP3_API_IMPORT void                  mp3_close(CODEC_INTERFACE_T *itf);
MP3_API_IMPORT void                  byte_swap(int *dest, int* src, int size);
MP3_API_IMPORT MP3_DEC_ERROR_ENUM_T  mp3_2_5_parse_header(CODEC_INTERFACE_T *itf);

#ifdef __cplusplus
}
#endif // __cplusplus

typedef enum {
  MP3_ALL_DDR,
  MP3_MIX_ESRAM_DDR,
  MP3_ALL_TCM,
  MP3_BAD_PRESET
}MP3_ALLOC_TYPE_ENUM_T;


/* Definition of mp3PRO base pointers */
#ifdef ALLOW_FOR_MP3PRO
extern char *SBRStatic_heap;
extern char *SBRDynamic_heap;
#endif /* ALLOW_FOR_MP3PRO */

#endif /* Do not edit below this line */







