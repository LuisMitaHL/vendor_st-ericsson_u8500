/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   speech_proc_config.h
 * \brief  Contains various buffer size used in speech_proc component
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _speech_proc_config_h_
#define _speech_proc_config_h_

/**
 * \page config Speech Proc Configuration
 * Description of speech_proc variables that can be modified to adapt
 * speech_proc behaviour. \n
 * All the macros described in this page are defined in speech_proc_config.h\n
 * Unless specified, modification of this varaibles doesn't imply recompilation of dsp code.
 */

//////////////////////////////////////////
// Version number
/////////////////////////////////////////
#define SPEECH_PROC_MAJOR    2
#define SPEECH_PROC_MINOR    7
#define SPEECH_PROC_REVISION 5

//////////////////////////////////////////
// Framing
//////////////////////////////////////////
/**
 * \page config
 *
 * <H2> Processing buffer size </H2>
 *
 * The size of the buffer processed by wrappers are defined by : \n
 * \ref PROCESSING_BUFFER_DURATION, that is the size in ms of buffer received/send by ul/dl wrappers \n
 * \ref PROCESSING_COUNT, that is the number of call to processing library for one buffer \n
 * ==> it means that processing lib will process (PROCESSING_BUFFER_DURATION/PROCESSING_COUNT) ms buffer
 *     For now (and will probably remain like this) it MUST be 10 \n
 * so the 2 options are 20/2 or 10/1
 *
 * \note  modification on these values \b DOES imply DSP recompiltaion (for hybrid version)
 */
#define PROCESSING_BUFFER_DURATION 20 /**< NMF buffer size of ul_wrapper output/input ports and dl_wrapper input port, in ms*/
#define PROCESSING_COUNT 2            /**< number of call to processing lib per buffer */

/**
 * \page config
 * <H2> Nmf buffer size on Downlink output port </H2>
 *
 * The size of buffer delivered by nmf component (dl_wrapper) is defined, in ms,
 * by \ref DL_OUTPUT_BUFFER_DURATION.
 * It must be a divider of \ref PROCESSING_BUFFER_DURATION and
 * for now it MUST be equal to \ref PROCESSING_BUFFER_DURATION
 *
 */
#define DL_OUTPUT_BUFFER_DURATION PROCESSING_BUFFER_DURATION /**< NMF buffer size of dl_wrapper output port, in ms */

/**
 * \page config
 * <H2> Nmf buffer size on reference port </H2>
 *
 * The size of nmf buffer, exchanged between reference port and
 * sink's feedback port is defined, in ms, by \ref REF_INPUT_BUFFER_DURATION
 *
 * \note  modification on these values \b DOES imply DSP recompiltaion (for hybrid version)
 */
#define REF_INPUT_BUFFER_DURATION 5 /**< NMF buffer size of time_align reference port, in ms */

/**
 * \page config
 * <H2> Nmf buffer size on uplink port </H2>
 *
 * The size of nmf buffer, exchanged between uplink port and source,
 * is defined, in ms, by \ref UL_INPUT_BUFFER_DURATION
 *
 * \note  modification on these values \b DOES imply DSP recompiltaion (for hybrid version)
 */
#define UL_INPUT_BUFFER_DURATION 5 /**< NMF buffer size of time_align input port, in ms */

/**
 * \page config
 * <H2> Buffer Size at OMX level </H2>
 *
 * The size of buffer required at OMX level (i.e. when a port is not tunneled,
 * or tunneled to a non AFM component using OMX standard communication) is defined
 * for each port, in ms, by : \n
 * - \ref   OMX_DL_INPUT_DURATION     \n
 * - \ref   OMX_DL_OUTPUT_DURATION    \n
 * - \ref   OMX_UL_INPUT_DURATION     \n
 * - \ref   OMX_UL_REFERENCE_DURATION \n
 * - \ref   OMX_UL_OUTPUT_DURATION    \n
 */
#define  OMX_DL_INPUT_DURATION     20 /**< OMX buffer size of DL input port, in ms */
#define  OMX_DL_OUTPUT_DURATION    20 /**< OMX buffer size of DL output port, in ms */
#define  OMX_UL_INPUT_DURATION     20 /**< OMX buffer size of UL input port, in ms */
#define  OMX_UL_REFERENCE_DURATION 20 /**< OMX buffer size of UL reference port, in ms */
#define  OMX_UL_OUTPUT_DURATION    20 /**< OMX buffer size of UL outputput port, in ms */

//////////////////////////////////////////
// Nb exchange between Rx/Tx
//////////////////////////////////////////
/**
 * \page config
 * <H2> Fifo size for information exchange between Rx and TX libs </H2>
 *
 * Rx lib (resp. Tx) may send information to Tx lib (resp. Rx) lib after each
 * processed frame. This information will be read by Tx lib (resp. Rx) only
 * before processing a new frame. Hence if several Rx frames are processed
 * before a Tx frame, Tx wrapper can store up to \ref NB_INFO_EXCHANGE
 * "information". If more information need to be stored, the oldest one
 * is overwritten.
 */
 #define NB_INFO_EXCHANGE 15


//////////////////////////////////////////
//   Nb Channels information
//////////////////////////////////////////
/**
 * \page config
 * <H2> Reference and Uplink port channel infomation </H2>
 *
 * <H3> Reference port </H3>
 * The maximum number of channel supported by OMX reference port is defined by \ref MAX_NB_OMX_REF_CHANNEL.
 * If the port is configured with a higher number of channel, speech_proc component will return an error. \n
 * Now in some cases, if the  speech library does not support (or take advantage) of stereo input we can force
 * to have a mono signal at nmf level (independently of OMX configuration) by setting the macro
 * \ref FORCE_MONO_REFERENCE to 1. In this case, the binding component (pcmadapter or shmpcmin),
 * will downsample the signal before sending it to reference port.\n
 * This allow to save mips by running samplerate conversion on only one channel.
 *
 * <H3> Uplink port </H3>
 * The maximum number of channel supported by OMX uplink input port is defined by \ref MAX_NB_INPUT_CHANNEL.
 * If the port is configured with a higher number of channel, speech_proc component will return an error. \n
 * Adjusting this value to the one needed may save some memory.
 */
#define MAX_NB_OMX_REF_CHANNEL 2 /**< max number of channel on OMX ref port */

#define FORCE_MONO_REFERENCE 1   /**< force mono data on nmf reference port */
#ifdef FORCE_MONO_REFERENCE
#define MAX_NB_REF_CHANNEL 1
#else
#define MAX_NB_REF_CHANNEL MAX_NB_OMX_REF_CHANNEL
#endif

#define MAX_NB_INPUT_CHANNEL 2 /**< max number of channel on uplink inp[ut port */


//////////////////////////////////////////
//   Sample Rate conversion
//////////////////////////////////////////
/**
 * \page config
 * <H2> Samplerate converter </H2>
 *
 * The size of the memory allocated for samplerate converter is defined,
 * in mmdsp words, by \ref SRC_HEAP_SIZE.\n
 * It \b MUST be inline with samplerate used.
 */
#ifdef FORCE_MONO_REFERENCE
#define SRC_HEAP_SIZE 135  /**< number of words allocated for SRC */
#else
#define SRC_HEAP_SIZE 268
#endif

//////////////////////////////////////////
//   ring Buffer for time alignment
//////////////////////////////////////////
/**
 * \page config
 * <H2> Ring buffer on reference port </H2>
 *
 * The size allocated for reference's ring buffer is defined by its
 * duration in ms \ref RING_BUFFER_DURATION, and the maximum sample freq
 * of uplink signal in samples/ms \ref MAX_SAMPLE_FREQ.
 */

#define MAX_SAMPLE_FREQ       16   /**< max sample freq, in sample/ms */
#define RING_BUFFER_DURATION  40   /**< ring buffer duration, in ms   */
#define RING_BUFFER_SIZE      (MAX_SAMPLE_FREQ * MAX_NB_REF_CHANNEL * RING_BUFFER_DURATION)


///////////////////////////////////////////////////////////
// Memory (for hybrid version)
///////////////////////////////////////////////////////////
/**
 * \page config
 * <H2> MMDSP Memory used </H2>
 *
 * All mmdsp memory zones are allocated dynamically by proxy, removing the need to recompile
 * DSP code to change buffer location. \n
 * The memory used for downlink shared (between arm and dsp) buffer is defined by
 * \ref DL_HYBRID_BUFFER_MEMORY, among \ref sp_memory_type_t. The size allocated for this buffer
 * is (DL_OUTPUT_BUFFER_DURATION * MAX_SAMPLE_FREQ) samples plus one buffer_t structure. \n
 *
 * The memory used for uplink shared (between arm and dsp) buffer is defined by
 * \ref UL_HYBRID_BUFFER_MEMORY, inside \ref sp_memory_type_t. The size allocated for this buffer
 * is (PROCESSING_BUFFER_DURATION * MAX_SAMPLE_FREQ * (MAX_NB_REF_CHANNEL + MAX_NB_INPUT_CHANNEL))
 * samples plus one buffer_t structure. \n
 *
 * The memory used for samplerate converter is defined by \ref SRC_MEMORY,
 * among (CM_MM_MPC_TCM24 | CM_MM_MPC_ESRAM24 | CM_MM_MPC_SDRAM24). The size allocated is defined
 * by \ref SRC_HEAP_SIZE.
 *
 * The memory used for reference's ring buffer is defined by \ref RING_BUFFER_MEMORY,
 * among (CM_MM_MPC_TCM24 | CM_MM_MPC_ESRAM24 | CM_MM_MPC_SDRAM24). The size allocated is defined
 * by \ref RING_BUFFER_SIZE.
 */
typedef enum{
  SP_MEM_TCM,
  SP_MEM_ESRAM,
  SP_MEM_DDR
} sp_memory_type_t;


#define DL_HYBRID_BUFFER_MEMORY SP_MEM_TCM /**<  memory to use for downlink hybrid buffer */

#define UL_HYBRID_BUFFER_MEMORY SP_MEM_TCM /**<  memory to use for uplink hybrid buffer */

#define SRC_MEMORY CM_MM_MPC_TCM24 /**<  memory to use for SRC */

#define RING_BUFFER_MEMORY CM_MM_MPC_TCM24 /**<  memory to use for ring buffer */

#endif // _speech_proc_config_h_

