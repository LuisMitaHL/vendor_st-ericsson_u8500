/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     speech_proc_interface.h
 * \brief    Interface between NMF wrappers of OMX Speechproc
 *           component and the actual processing library.
 * \author   ST-Ericsson
 * \version  v1.0.0
 */
/*****************************************************************************/
#ifndef _speech_proc_interface_h_
#define _speech_proc_interface_h_

#ifdef __cplusplus
extern "C"
{
#endif

  /**
   * Used at init to tell which kind of algorithm the
   * instantiation of the library will have to handle
   */
  typedef enum {
    SP_UPLINK_NO_ALGO,      /**< Bypass Mode uplink   */
    SP_DOWNLINK_NO_ALGO,    /**< Bypass Mode downlink */
    SP_UPLINK_ALGO,         /**< Uplink Algorithms    */
    SP_DOWNLINK_ALGO        /**< Downlink Algorithms  */
  } speech_proc_algo_t;


  /**
   * Index used to distinguish various input buffers
   */
  typedef enum {
    SP_MAIN_PORT,       /**< input port for all kind of algorithms */
    SP_REFERENCE_PORT,  /**< observation port, only for uplink algorithms */
  } speech_proc_port_index_t;


  /**
   * Used to describe speech data for downlink.
   <TABLE border=1>
   <TR><TD>              </TD><TD> Bad Frame                                        </TD><TD> Speech                                         </TD></TR>
   <TR><TD> AMR-WB    3G </TD><TD> FQI (0=Bad, 1=Good)                              </TD><TD> Rate 1-8(speech) 9,15(no speech), 14 see below </TD></TR>
   <TR><TD> AMR-NB    3G </TD><TD> FQI (0=Bad, 1=Good)                              </TD><TD> Rate 1-7(speech) 8,15(no speech)               </TD></TR>
   <TR><TD> AMR-WB    2G </TD><TD> RX type 1,3,6 (Bad), 0,4,5,7 (Good), 2 see below </TD><TD> Rate 1-8(speech) 9,15(no speech)               </TD></TR>
   <TR><TD> AMR-NB    2G </TD><TD> RX type 1,3,6 (Bad), 0,4,5,7 (Good), 2 see below </TD><TD> Rate 1-7(speech) 8,15(no speech)               </TD></TR>
   <TR><TD> FR,HR,EFR 2G </TD><TD> BFI (0=Good, 1=Bad)                              </TD><TD> SID 0 (speech), 2 (no speech)                  </TD></TR>
   </TABLE>

   Speech Lost (2) for 2G AMR (NB and WB) we should idealy use previous frame. The same is true for AMR-WB speech lost in 3G

   <TABLE border=1>
   <TR><TD> Previous frame </TD><TD> Bad Frame </TD><TD> Speech </TD></TR>
   <TR><TD> Speech         </TD><TD> true      </TD><TD> true   </TD></TR>
   <TR><TD> No speech      </TD><TD> false     </TD><TD> false  </TD></TR>
   </TABLE>
  */
  typedef enum {
    SPEECH_PROC_INFO_ELEMENT_SYSTEM,         /**<Current system, 1 = 2G, 2 = 3G */
    SPEECH_PROC_INFO_ELEMENT_SPEECH_CODEC,   /**<Which speech codec used to decode downlink frame. Half rate (1), Full Rate (2),
                                                Enhanced Full Rate (3), AMR-NB (4), AMR-WB (5)  */
    SPEECH_PROC_INFO_ELEMENT_RXTYPE_OR_SID,  /**<For AMR-NB the field contains RX type as
                                                found in downlink message. For Half Rate, Full Rate, and Enhanced Full Rate the field
                                                contains silence descriptor. Both field only valid for 2G */
    SPEECH_PROC_INFO_ELEMENT_BITRATE_OR_TAF, /**<For AMR-NB the field contains bit rate as
                                                defined in downlink message. For Half Rate,
                                                Full Rate, and Enhanced Full Rate the field
                                                contains TAF flag as defined in downlink message*/
    SPEECH_PROC_INFO_ELEMENT_FQI_OR_BFI,     /**<BFI is valid only for Half Rate, Full Rate, and Enhanced Full Rate and is based on BFI flag
                                                in downlink message. For Half Rate it is True if either BFI or UFI is true, otherwise False.
                                                For Full Rate it is True if either BFI or DFI is true, otherwise False. */
    SPEECH_PROC_INFO_ELEMENT_EXTRA,          /**<Additional information that can be used for other purpose*/
    SPEECH_PROC_INFO_ELEMENT_NUMBER_OF
  } speech_proc_frame_info_element_t;


  /**
   * Frame information
   */
  typedef struct speech_proc_frame_info_t {
    unsigned char FrameInfo[SPEECH_PROC_INFO_ELEMENT_NUMBER_OF]; /**< Sequence of setting */
  } speech_proc_frame_info_t;


  /**
   * Error type
   */
  typedef enum {
    SP_ERROR_NONE,                    /**< No error */
    SP_ERROR_NO_MORE_MEMORY,          /**< Not enough memory available */
    SP_ERROR_UNSUPPORTED,             /**< Feature not supported  */
    SP_ERROR,                         /**< Any other Kind of error, not specified */
    SP_ERROR_SPECIFIC_START = 0x1000, /**< Library can define all necessary
                                           error code starting from this point */
  } speech_proc_error_t;


  /**
   *  Define buffer size, sample size and sample rate
   */
  typedef struct speech_proc_setting_t {
    unsigned int resolution; /**< bit per sample, only 16 currently supported */
    unsigned int framesize;  /**< Number of samples in each execution.
                                May be 1,2,4,5,10 or 20 ms (only 10ms for currently)
                                Each 1 ms contains samplerate/1000 so 10 ms in 8kHz is 80 and in 16kHz 160 */
  } speech_proc_settings_t;


  /**
   * Define settings
   */
  typedef struct speech_proc_portsetting_t {
    struct {
      unsigned int enabled;     /**< If not set, then library should not take into account next info*/
      unsigned int interleaved; /**< 0 means flat, 1 means interleaved. (Only flat is currently supported) */
      unsigned int nb_channels; /**< Number of channels */
      unsigned int samplerate;  /**< 8000 or 16000*/
    } port[2];
    unsigned int nb_ports;
  } speech_proc_port_settings_t;


  /**
   * Called once to initialize the library. \n
   * This function should allocate and initialize all the necessary data structure,
   * and then return a pointer on its context. This context will be passed as first argument
   * in all other interface functions.
   * In case of error *ctx \b MUST also be set to NULL.
   *
   * \param algo     specify the kind of algorithms this instance will process
   * \param ctx      pointer of pointer to return address of the context
   * \param version  pointer to be filled with library version (if any)
   * \return SP_ERROR_NONE if no error \n
   *         SP_ERROR_XXX otherwise
   */
  speech_proc_error_t speech_proc_init(speech_proc_algo_t algo,  void **ctx, unsigned int *version);


  /**
   * Called once to close the library.\n
   * This function should free all memory used by the library.
   *
   * \param ctx pointer on local context
   * \return SP_ERROR_NONE if no error \n
   *         SP_ERROR otherwise
   */
  speech_proc_error_t speech_proc_close(void *ctx);


  /**
   * Called before \ref speech_proc_open.\n
   * This function is called to set static parameters.
   * Use OMX configuration prototype.
   * Index filtering will be done in above layers and then library will
   * only received indexes that it is supposed to support.
   *
   *
   * \param ctx           pointer on local context
   * \param index         specifies which algo is to be configured
   * \param config_struct configuration structure for this index (to be casted into right structure)
   *
   * \return SP_ERROR_NONE if no error \n
   *         SP_ERROR_XXX otherwise
   */
  speech_proc_error_t speech_proc_set_parameter(void *ctx, int index, void *config_struct);


  /**
   * Called at any time before or during process.\n
   * This function is called to set dynamic parameters.
   * Use OMX configuration prototype.
   * Index filtering will be done in above layers and then library will
   * only received indexes that it is supposed to support.
   *
   *
   *
   * \param ctx           pointer on local context
   * \param index         specifies which algo is to be configured
   * \param config_struct configuration structure for this index (to be casted into right structure)
   *
   * \return SP_ERROR_NONE if no error \n
   *         SP_ERROR_XXX otherwise
   */
  speech_proc_error_t speech_proc_set_config(void *ctx, int index, void *config_struct);

  /**
   * This function is NOT called by NMF wrapper.
   * Nevertheless, the OMX layer may need to retrieve information directly from the library
   * (like internal algo variable values for example). If so, this function will be called from
   * SpeechProcLibrary derived class, and then if you don't call it you don't need to implement it.
   *
   * \param ctx           pointer on local context
   * \param index         specifies which algo is to be retrieved
   * \param config_struct configuration structure for this index (to be casted into right structure)
   *
   * \return SP_ERROR_NONE if no error \n
   *         SP_ERROR_XXX otherwise
   */
  speech_proc_error_t speech_proc_get_config(void *ctx, int index, void *config_struct);


  /**
   * Called to set the working sample frequency.\n
   * Necessarily called once before first process but can be re-called during process.
   * Called when both input and ouput port are enabled (but not necessarily reference port
   * for uplink. It will be recalled if reference port is enabled after)
   *
   * \param ctx        pointer on local context
   * \param mode       global settings
   * \param inports    settings of input ports
   * \param outports   settings of output ports
   *
   * \return SP_ERROR_NONE if no error \n
   *         SP_ERROR_XXX otherwise (for example if settings asked are not supported)
   */
  speech_proc_error_t speech_proc_set_mode(void *ctx,
                                           const speech_proc_settings_t* mode,
                                           const speech_proc_port_settings_t* inports,
                                           const speech_proc_port_settings_t* outports);


  /**
   * Called once before first call to \ref speech_proc_process and after call to
   * \ref speech_proc_set_parameter function.\n
   * This function finalize memory allocation (if depending of static parameters),
   * and library initialization.
   *
   * \param ctx           pointer on local context
   *
   * \return SP_ERROR_NONE if no error\n
   *         SP_ERROR_XXX otherwise
   */
  speech_proc_error_t speech_proc_open(void *ctx);


  /**
   * Called each time a new input buffer is available.\n
   * This function should always generate nb_output (always 1 currently) output buffers.
   * Size, resolution, sample rate and channels layout of input/output buffers
   * have already been set with \ref speech_proc_set_mode function. \n
   * Additional frame info can also be given as parameter. For downlink it contains information
   * received on downlink with speech codec specific information such as BFI. If no information
   * exists then parameter is set to NULL.
   * NOTICE : What we call input/output is one channel and not all channels of one port.
   * For example, uplink has two input ports (\ref SP_MAIN_PORT and \ref SP_REFERENCE_PORT)
   * but MAIN_PORT may be mono and reference port may be stereo.
   * In this case nb_input will be 3. \n
   * Layout of input/output buffers will be organized so that all channels of
   * one port will be in one continuous buffer. \n
   *
   * Example for 2 input ports, both stereo and not interleaved:
   * \code
   * input_port[0]    input_port[1]        input_port[2]    input_port[3]
   * |                |                    |                |
   * v                v                    v                v
   * +----------------+----------------+   +----------------+----------------+
   * | main port left | main port right|   | ref port left  | ref port right |
   * +----------------+----------------+   +----------------+----------------+
   * \endcode
   *
   * Example for 2 input ports, one mono one stereo and interleaved:
   * \code
   * input_port[0]         input_port[1]
   * |                     |input_port[2] (one sample offset)
   * |                     ||
   * v                     vv
   * +-----------------+   +-----------------------------------------+
   * | main port (mono)|   | reference port (2 channels interleaved) |
   * +-----------------+   +-----------------------------------------+
   * \endcode
   *
   *
   * \param ctx           pointer on local context
   * \param input         table of input buffers
   * \param nb_input      number of input channels
   * \param output        table of output buffers
   * \param nb_output     number of output channels
   * \param frame_info    contains frame information. Can be NULL in which case no frame info exists.

   *
   * \return SP_ERROR_NONE if no error \n
   *         SP_ERROR_XXX otherwise
   */
  speech_proc_error_t speech_proc_process(void *ctx,
                                          short** input, int nb_input,
                                          short** output, int nb_output,
                                          const speech_proc_frame_info_t *frame_info);


  /**
   * Called after each call to process.\n
   * This function should return usefull information for the other algorithms group.
   * The buffer returned should have be allocated by the library itself.
   *
   * \param ctx           pointer on local context
   *
   * \return pointer on information area\n
   *         NULL if no information is to be shared
   */
  void * speech_proc_get_processing_info(void *ctx);


  /**
   * Called by the other algorithms group (i.e. downlink for uplink,
   * and uplink for downlink) wrapper after processing one frame.\n
   * This function provides information about the other path
   *
   * \param ctx  pointer on local context
   * \param info pointer on information from other path
   *
   * \return SP_ERROR_NONE if no error \n
   *         SP_ERROR_XXX otherwise
   */
  speech_proc_error_t speech_proc_set_processing_info(void *ctx, void *info);


  /**
   * Called after each call to process.\n
   * This function should return the gain to apply for sidetone.
   * The value should be specify in mB (millibels = 1/100 dB).
   * Although it is called after each frame, sidetone gain is only "used" to update
   * sinks & sources gain if updated flag is set.
   *
   * \param ctx      pointer on local context
   * \param gain     pointer on gain.
   * \param updated  pointer on flag to be set if sidetone gain value has been modified
   *                 compared to previous call
   *
   * \return SP_ERROR_NONE if the gain pointer has been updated \n
   *         SP_ERROR_UNSUPPORTED if sidetone gain is not supported by the lib
   *                             (it will prevent further calls to this function) \n
   */
  speech_proc_error_t speech_proc_get_sidetone_gain(void *ctx, int *gain, int *updated);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _speech_proc_interface_h_ */
