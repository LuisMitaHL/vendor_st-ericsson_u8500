
#ifndef	_defines_h_
#define _defines_h_


/* defines for EAACPlus */
#define GPP_AACPLUS_v2 
//#define AACPLUS_ALPHA
//#define GPP_AACPLUS_v2
//#define MPEG4_AACPLUS_v2

#ifdef GPP_AACPLUS_v2
#define GPP_AACPLUS_v1
#define PARAMETRIC_STEREO
#endif

#ifdef GPP_AACPLUS_v1
#define LP_NEVER
#define SBR
#endif

#define NON_BE_BUGFIX

/* define for BSAC */
//#define MPEG4BSAC

/* define for Error Concealment */
#define ERROR_CONCEALMENT

/* Define compilation constants */

/* Use hardware block for bitstream parsing */
//#define USE_FRAMEBUFFER

/* Use AAC ch. position(C,L,R,Ls,Rs,lfe) */
//#define AAC_DEFAULT_CHANNEL_CONFIG 

/* Generate two channel decoder */
#define STEREO_ONLY

#ifdef ARM
/* Generate multi channel decoder */
#ifndef MC
#define MC
#endif
//#define STD_PARSE
#endif


/* Allow for MPEG4 LTP support */
#define MPEG4V1LTP


/* #if !defined(MMDSP) */ 
/* Allow for PNS support in float mode for now*/
#define MPEG4V1PNS 
/* #endif */

/* Allow for debug concerning necessary precision of 1/sqrt(nrg) in PNS */
#ifndef __flexcc2__ /* to debug PNS only in float and unix modes */
#define DEBUG_PNS_NRG
#define  PRINT   fprintf
#ifndef SE
#define  SE      stdout
#endif // SE
#endif

/* selection of one of the different implementations of 1/sqrt(nrg) in PNS */
/* #define PNS_IMP1 /\* input tab on 10 bits + 0 Ralston's iteration *\/ */
#ifndef PNS_IMP1
#define PNS_IMP2 /* input tab on 5 bits + 1 Ralston's iteration */
#endif

/* Allow for SSR support */
//#define SSR

/* reduce size of imdct state buffer from 1024 to 512 */
//#ifndef SSR 
#define COMPRESS_STATE_BUFFER 

#ifdef SSR
#ifndef DO_NOT_HAVE_MDCT_TABS_32
#error "SSR cannot be enabled if IMDCT tables are not present for this profile, check audiolibs/common/include/mdct_tables.h"
#endif
#endif /* SSR */

/* Reuse scratch buffer instead of the stack for temporary buffers */
#define BUFFER_ALLOCATION_IN_SCRATCH_BUFFER

/* Compress scale factors and cb map into a single array */
#define COMPRESS_CB_MAP_AND_SC_FACTORS

/* Compress mask array */
#define COMPRESS_MASK

/* Use ram in x and y for filter bank and fft */
#define IMDCT_RAMXY

/* Use static allocation instead of malloc()'s */
#define NOMALLOC

/* Support unsigned long types */
#define HAS_ULONG 

/* Allow for ADTS support */
#define ADTS
/* Allow for new ADTS support (new CRC : M9021 - october 2002) */
#define NEW_ADTS

/* Allow for DRC support */
//#define DRC

/* trick for SSR IPQF filter */
#define INTERLEAVE_SSR_WINDOW

/* Concatenate huffman data into a single 24-bit word */
#define HUFF_CONCAT

/* Concatenate unpack data into 24-bits */
#define UNPACK_NEW

/* Use PLB's magic normalization to pass conformance */
#define PLB_NORM

/* concatenate RAM as much as possible */
#define BIT_FIELDS   

/* Define location of ROM *//* memory allocations defined in aac_memconfig.h */
/* #define SR_MEM  YMEM  /\* Sample rate dependent info is in YMEM *\/ */
/* #define FT_MEM  YMEM  /\* Filter related tables are in YMEM     *\/ */
/* #define HUF_MEM YMEM  /\* Huffman tables are in YMEM            *\/ */

/* Define number of coupling channels */
/* #define is required in order to use these args in #if () directive */
#define ICChans	0 /*3*/		/* independently switched coupling channels */
#define DCChans	0 /*3*/		/* dependently switched coupling channels */
#define XCChans	0 /*2*/		/* scratch space index for parsing unused
				 * coupling channels */
#define CChans  (ICChans + DCChans + XCChans)

/* Define dynamic range and normalization constants */
#define  NORM_BITS         15
#define  NORMVAL           (1<<NORM_BITS)
#define  SAFETY_BIT_MARGIN (4)
#define  SAFETY_MARGIN     (1<<SAFETY_BIT_MARGIN)

#define  FAT_BIT_MARGIN    (6)
#define  FAT_MARGIN        (1<<FAT_BIT_MARGIN)
#define  SSR_COMPENSATE    1

#ifdef ARM
#define WORDSIZE   32
#else /* ARM */
#define WORDSIZE   24
#endif /* ARM */

typedef struct {
    UCHAR            this_bk;
    UCHAR            prev_bk;
#ifdef COMPRESS_STATE_BUFFER
    UCHAR            prev_sequence;
#endif /* COMPRESS_STATE_BUFFER */
}               Wnd_Shape;

typedef enum {
    ONLY_LONG_SEQUENCE = 0x0,
    LONG_START_SEQUENCE,
    EIGHT_SHORT_SEQUENCE,
    LONG_STOP_SEQUENCE,
    MAX_SEQUENCE
} WINDOW_SEQUENCE;

typedef enum {
	SAM_WS_FHG, SAM_WS_DOLBY, SAM_N_WINDOW_SHAPES
} SAM_Window_shape;
typedef SAM_Window_shape SAM_WINDOW_SHAPE;


#include "interface.h"
#include "chandefs.h"

#endif				/* _defines_h_ */



