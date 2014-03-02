/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \brief Compile-time settings.
 * \author ST-Ericsson
 */

#ifndef SETTINGS_H
#define SETTINGS_H



/**
 * \brief Set the endianness to little endian if set to 1
 */
 
#define ST_LITTLE_ENDIAN   1


/**
 * \brief Set the size of the fifo (of picture ptr + buffer utilisation info) at the output of the DPB
 */
 
#define DISPLAY_QUEUE_SIZE  4
/* DEF_FTB_SIZE describe the maximum number of FillThisBuffer, 
 * above 16+1+HAMAC_PIPE_SIZE+1+DISPLAY_QUEUE_SIZE
 * that the algo can memorize as available buffers */
#define DEF_FTB_SIZE 40


/**
 * \brief Size of the Hamac pipe
 */
 
#define HAMAC_PIPE_SIZE  3
#define HAMAC_PIPE_FREE_SLOTS 2 /* 1 for current frame, one for a flush cmd */
#define SLICE_LINKED_LIST_SIZE 10

#define NB_BUFFERS_IN_DISPLAY 1;
/**
 * \brief Set the maximum number of storable frames
 */
 
#define MAXNUMFRM  17


/**
 * \brief Set the initial maximum number of slices per frame
 */
 
#define MAXNSLICE   20


/**
 * \brief Set the maximum number of errors in consecutive slice header before forcing IDR resynchronization
 */
 
#define MAX_ERROR   10


/**
 * \brief Toggles on-the-fly insertion/removal of emulation_prevention_three_byte
 *  during the encoding/decoding stage.
 */
 
#define ON_THE_FLY_EMULATION_BYTE   1


/**
 * \brief If set to 1 force reset of frame buffers prior of them reuse
 */
 
#define CLEAN_BUFFER    1


/**
 * \brief If set to 1 temporal error concealment is performed only by copy replacement. 
 * Otherwise motion compensated error concealment is performed. 
 */
 
#define COPY_ONLY   0


/**
 * \brief If set to 1 enables frame_num consistency checking and try to conceal frame numbers in slice headers. 
 * If set to 0 frame_num is not checked and errors are considered slice/frame loss by default. 
 */
 
#define CONC_FRAME_NUM   0


/**
 * \brief If set to 1 hardware model of deblocking filter is used. This define works only if
 * ST_LITTLE_ENDIAN is set to 0. If ST_LITTLE_ENDIAN is set to 1 or this define is set to 0 an equivalent
 * deblocking model is used (faster, no frame buffers conversion)/
 */
 
#define DEBLOCK_HW_MODEL    0


/**
 * \brief If set to 1 enables full user interface (output file, post processing). Use 2 for output file only.
 */
 
#define FULL_GUI   0
 

/**
 * \brief If set to 1 enables VUI messages parsing and decoding
 */

#define ENABLE_VUI  1


/**
 * \brief If set to 1 enables timing informations parsing and decoding
 */

#define ENABLE_TIMING   1


/* Debugging features */

/**
 * \brief Enables generation of on-screen messages for bitstream parsing.
 * 0    no output
 * 1    SPS, PPS, SEI, Slice header
 * 2    Macroblock data
 * 3    Residual
 */
#define VERBOSE_BITSTREAM   0


/**
 * \brief Enables generation of on-screen general messages
 */
#define VERBOSE_STANDARD 0


/**
 * \brief Enables generation of dump of block fetch for INter decoding
 * 0    no output
 * 1    output
 */
 
#define VERBOSE_BLOCK_INTER  0
#define VERBOSE_LIST0_PICTURE  0


/**
 * \brief Enables generation of on-screen messages of motion vector decoding.
 */
 
#define VERB_MV     0


/**
 * \brief Enables generation of on-screen messages for the Decoder Picture Buffer 
 * management operations.
 */
 
#define VERB_DPB    0


/**
 * \brief Enables generation of on-screen messages for error concealment
 */
 
#define VERB_ERR_CONC    0

#define VERBOSE_INPUT_CONTROL   0
//#define VERBOSE_HAMAC_PIPE   0


/**
 * \brief Enables generation of on-screen messages for scene change detection
 */
 
#define VERB_SCD    0

/** to debug HOST HAMAC INTERFACE **/

#define VERBOSE_PARAM 0



/* set it to -1 to avoid to stop after frame n */
#define DEBUG_STOP_FRAME	-1

/* Don't modify below */

#if ENABLE_VUI == 0
    #undef ENABLE_TIMING
    #define ENABLE_TIMING 0
#else
    #define MAX_CBP_CNT 1 /* 32 */
#endif


// FIXME: should be removed after NMF1.8 or 1.9
#if ((!defined __SYMBIAN32__) && (!defined IMPORT_C))
    #define IMPORT_C
#endif
#if ((!defined __SYMBIAN32__) && (!defined EXPORT_C))
    #define EXPORT_C
#endif
#ifdef __cplusplus
extern "C" IMPORT_C void NMF_PANIC(const char* fmt, ...);
extern "C" IMPORT_C void NMF_LOG(const char* fmt, ...);
#else
extern IMPORT_C void NMF_PANIC(const char* fmt, ...);
extern IMPORT_C void NMF_LOG(const char* fmt, ...);
#endif

#endif



