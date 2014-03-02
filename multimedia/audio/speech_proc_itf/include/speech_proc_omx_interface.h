/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     speech_proc_omx_interface.h
 * \brief    Interface between OMX configuration and the actual processing library.
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#ifndef _speech_proc_omx_interface_h_
#define _speech_proc_omx_interface_h_

#include <OMX_Audio.h>

/**
 * Enum to identify port at OMX level
 */
typedef enum {
  INPUT_UPLINK_PORT,
  INPUT_DOWNLINK_PORT,
  INPUT_REFERENCE_PORT,
  OUTPUT_UPLINK_PORT,
  OUTPUT_DOWNLINK_PORT,
  SPEECH_PROC_NB_AUDIO_PORT,
  OUTPUT_CONTROL_PORT = SPEECH_PROC_NB_AUDIO_PORT,
  SPEECH_PROC_NB_PORT, // must always be the last
} speech_proc_port_name_t;


/**
 * Enum to identify the type of algo at OMX level
 * (inline with speech_proc_algo_t)
 */
typedef enum {
  UPLINK_ALGORITHMS   = 2,
  DOWNLINK_ALGORITHMS = 3
} speech_proc_algo_type_t;



/**
 * Abstract Class
 *
 */
class SpeechProcLibrary {
public:

  SpeechProcLibrary() {
	mUplinkCtx   = (void *)0;
	mDownlinkCtx = (void *)0;
  };
  virtual ~SpeechProcLibrary() {};

  /**
   * Return the number of different config and parameter indexes supported
   * by the processing library. This is not necessarly the same as OMX indexes.
   * This value will be used to size the NMF fifo between proxy and
   * NMF components.
   *
   * \param [in] algo type of Algo (Uplink or Downlink)
   * \return number of indexes (Parameter and Config) supported by the processing library
   */
  virtual OMX_U32 getNbParamAndConfig(speech_proc_algo_type_t algo) = 0;

   /**
   * Return true if bypass mode should be activated for this type of algo.
   * (For debug purpose only)
   *
   * \param [in] algo type of Algo (Uplink or Downlink)
   * \return true to activate bypass mode
   */
  virtual bool isBypass(speech_proc_algo_type_t algo) = 0;

  /**
   * This function is called on each call to OMX_SetParameter interface of the
   * OMX.ST.AFM.speech_proc component.\n
   * - If the index is not related to speech processing library configuration,
   * the function must return OMX_ErrorUnsupportedIndex and the configuration
   * will be handled by above layers.
   * - If the index is related to speech processing configuration, the function
   * should check the content of the structure, return OMX_ErrorUnsupportedSetting
   * if it is not correct or save it locally and return OMX_ErrorNone.
   *
   * \param [in] index OMX index
   * \param [in] param pointer on the configuration structure related to index
   * \return OMX_ErrorNone, OMX_ErrorUnsupportedIndex or OMX_ErrorUnsupportedSetting
   */
  virtual OMX_ERRORTYPE checkParameter(OMX_U32 index, OMX_PTR param) = 0;

  /**
   * Function called to retrieve ALL parameters to set on speech_proc library
   * one by one. This function is called each time the OMX component
   * goes from OMX_LoadedState to OMX_IdleState (i.e. when we create the speech_proc library).
   * The struture returned should always be available (e.g. not allocated in the stack for example).
   *
   *
   * This function will be used like this :
   *
   * \code
   * OMX_U32 previous_index = 0;
   * OMX_U32 current_index = 0;
   * OMX_PTR param = NULL
   *
   * param = getNextParameter(algo,previous_index, &current_index);
   * while(param)
   * {
   *   // Actually this is NOT a direct call to speech_proc library
   *   // but it will end to this.
   *   speech_proc_set_parameter(ctx,current_index,param)
   *   previous_index = current_index;
   *   param = getNextParameter(algo,previous_index, &current_index);
   * }
   * \endcode
   *
   * \param [in]  algo type of Algo (Uplink or Downlink)
   * \param [in]  previous_index last index retrieved (0 on first call)
   * \param [out] current_index value of the current index retreived
   * \return pointer of parameter structure to be passed to speech_proc library. NULL when all parameters
   * have been provided (i.e. when previous_index is the last index supported).
   */
  virtual OMX_PTR getNextParameter(speech_proc_algo_type_t algo, OMX_U32 previous_index, OMX_U32 *current_index) = 0;


  /**
   * This function is called on each call to OMX_SetConfig interface of the
   * OMX.ST.AFM.speech_proc component.\n
   * - If the index is not related to speech processing library configuration,
   * the function must return OMX_ErrorUnsupportedIndex and the configuration
   * will be handled by above layers.
   * - If the index is related to speech processing configuration, the function
   * should check the content of the structure, return OMX_ErrorUnsupportedSetting
   * if it is not correct or save it locally and return OMX_ErrorNone.
   *
   * \param [in] index OMX index
   * \param [in] config pointer on the configuration structure related to index
   * \return OMX_ErrorNone, OMX_ErrorUnsupportedIndex or OMX_ErrorUnsupportedSetting
   */
  virtual OMX_ERRORTYPE checkConfig(OMX_U32 index, OMX_PTR config) = 0;


  /**
   * Function called to retrieve ALL configs to set on speech_proc library
   * one by one. This function is called each time the OMX component
   * goes from OMX_LoadedState to OMX_IdleState (i.e. when we create the speech_proc library).
   * The struture returned should always be available (eg not allocated in the stack for example).
   *
   *
   * This function will be used like this :
   *
   * \code
   * OMX_U32 previous_index = 0;
   * OMX_U32 current_index = 0;
   * OMX_PTR param = NULL
   *
   * param = getNextConfig(algo,previous_index, &current_index);
   * while(param)
   * {
   *   // Actually this is NOT a direct call to speech_proc library
   *   // but it will end to this.
   *   speech_proc_set_config(ctx,current_index,param)
   *   previous_index = current_index;
   *   param = getNextConfig(algo,previous_index, &current_index);
   * }
   * \endcode
   *
   * \param [in]  algo type of Algo (Uplink or Downlink)
   * \param [in]  previous_index last index retrieved (0 on first call)
   * \param [out] current_index value of the current index retreived
   * \return pointer of config structure to be passed to speech_proc library. NULL when all config
   * have been provided (i.e. when previous_index is the last index supported).
   */
  virtual OMX_PTR getNextConfig(speech_proc_algo_type_t algo, OMX_U32 previous_index, OMX_U32 *current_index) = 0;

  /**
   * Function called on each call of the OMX_SetConfig interface of the
   * OMX.ST.AFM.speech_proc component, if the component is not in OMX_LoadedState.\n
   * Because it is not easy to get the result of \ref checkConfig when this function is called
   * NULL should be returned if the index is not related to configuraion of the specify
   * type of algo (downlink or uplink).\n
   * As one OMX index may result in several library configuration, the fuction should return a table
   * of structure and indexes. (Tables have been allocated by proxy and the size is the one returned by
   * \ref getNbParamAndConfig()).
   *
   *
   * \param [in]  algo type of Algo (Uplink or Downlink)
   * \param [in]  index OMX index
   * \param [out] config_table table of configuration structure that will be send to library
   * \param [out] index_table  table of index that will be send to library
   * \param [out] nb_config    number of config to send to library
   * \return OMX_ErrorNone, OMX_ErrorUnsupportedIndex
   */
  virtual OMX_ERRORTYPE getConfig(speech_proc_algo_type_t algo, OMX_U32 index, OMX_PTR* config_table, OMX_U32* index_table, OMX_U32 *nb_config) = 0;

  /**
   * Function called on each call to the OMX_GetParameter/OMX_GetConfig interface of the
   * OMX.ST.AFM.speech_proc component. Function should copy its local config
   * into provided pointer. (This is above layer resposability to allocate enough memory)
   *
   * \param [in]  index OMX index
   * \param [out] config pointer on free memory to be filled with required config.
   * (class should consider that memory size is big enough to write the requested config)
   * \return OMX_ErrorNone, OMX_ErrorUnsupportedIndex
   */
  virtual OMX_ERRORTYPE getOMXParamOrConfig(OMX_U32 index, OMX_PTR config) = 0;

  /**
   * Function called each time the OMX component goes from OMX_LoadedState
   * to OMX_IdleState (i.e. when we create the speech_proc library) and when
   * a port is enabled. As all ports may not be enabled (i.e. not configured)
   * when this function is called, the library should only check port marked
   * as enabled with the \b portenabled bitfield.
   * This function has been added because it is not easy to return an error
   * to the application in \ref speech_proc_set_mode() function.\n
   * NOTE : Parameters pass to this function do NOT take into account the
   * samplerate converter embedded inside time alignment.
   *
   * \param [in] portsettings settings for all audio ports. (port_settings[INPUT_UPLINK_PORT] ... port_settings[OUTPUT_DOWNLINK_PORT])
   * \param [in] portenabled  Bitfield indicating if a port is enabled. (e.g (portenable & (1<<INPUT_UPLINK_PORT)) means that
   *                                                                    INPUT_UPLINK_PORT is enabled)
   * \return OMX_ErrorNone if settings is OK, and OMX_ErrorBadParameter otherwise
   */
  virtual OMX_ERRORTYPE checkPortSettings(OMX_AUDIO_PARAM_PCMMODETYPE *portsettings[SPEECH_PROC_NB_AUDIO_PORT],
										  OMX_U32 portenabled) = 0;

  /**
   * Function called on each call to the OMX_GetExtensionIndex interface of the
   * OMX.ST.AFM.speech_proc component.
   *
   *
   * \param [in] extension String containing extension name
   * \param [out] index OMX index corresponding to the extension
   * \return OMX_ErrorUnsupportedIndex if extension is not supported, OMX_ErrorNone otherwise
   */
  virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING extension, OMX_U32* index) = 0;


  /**
   * Function called by proxy when lib is instanciated
   *
   * \param [in] ctx  Library context pointer
   * \param [in] algo Library instance (Uplink or Downlink)
   */
  void setLibCtx(void *ctx, speech_proc_algo_type_t algo)
  {
	if(algo == UPLINK_ALGORITHMS) mUplinkCtx   = ctx;
	else                          mDownlinkCtx = ctx;
  };

protected:
  /**
   * Function to retrieve library context, if (for very specific case) you
   * need to directly call the library. Typically in function getOMXParamOrConfig
   * you may need to get information directly form the library.
   * \attention Directly calling the library may not be safe: You have not guarantee
   * that it is not currently running, unless you specificallky add some synchro.
   *
   * \param [in] algo Library instance (Uplink or Downlink)
   * \return library context pointer
   */
  void *getLibCtx(speech_proc_algo_type_t algo)
  {
	if (algo == UPLINK_ALGORITHMS) return mUplinkCtx;
	else                           return mDownlinkCtx;
  };

private:
  void * mUplinkCtx;
  void * mDownlinkCtx;
} ;


/**
 * Factory method so that Speech Proc Proxy is completely independant
 * of the processing library used.
 *
 * \return pointer on a class implementing SpeechProcLibrary interface
 */
SpeechProcLibrary *speech_proc_getOMXInterface(void);



#endif // _speech_proc_omx_interface_h_
