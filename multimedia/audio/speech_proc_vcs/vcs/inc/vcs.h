#ifndef _VCS_H
#define _VCS_H
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs.h
 * \brief    Interface for voice shell
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

  /**
	 Enumeration for kind of processing
   */
  typedef enum {
	VCS_UPLINK, /**< Uplink processing (TX)*/
	VCS_DOWNLINK, /**< Downlink processing (RX)*/
	VCS_BYPASS  /**< Bypass mode only for test */
  } vcs_processing_kind_t;
  
  /**
	 Errors used in when logging
   */
  typedef enum {
	VCS_FATAL, /**< Fatal error */
	VCS_ERROR, /**< Error */
	VCS_WARNING, /**< Warning */
	VCS_INFO, /**< Information  */
	VCS_DEBUG, /**< Debug prints */
	VCS_VERBOSE /**< Verbose prints */
  } vcs_log_kind_t;
  
  /**
	 List of probes
   */
  typedef enum {
	
	VCS_PROBE_TX_INPUT_LEFT,
	VCS_PROBE_TX_INPUT_RIGHT,
	VCS_PROBE_TX_INPUT_MIC_REF1,
	VCS_PROBE_TX_EC_FEEDBACK_LEFT,
	VCS_PROBE_TX_EC_FEEDBACK_RIGHT,
	VCS_PROBE_TX_EC_FILTERTAP_LEFT,
	VCS_PROBE_TX_EC_FILTERTAP_RIGHT,
	VCS_PROBE_TX_SE_LINEAR_LEFT,
	VCS_PROBE_TX_SE_LINEAR_RIGHT,
	VCS_PROBE_TX_SE_NONLINEAR_LEFT,
	VCS_PROBE_TX_SE_NONLINEAR_RIGHT,
	VCS_PROBE_TX_SE_OUT,
	VCS_PROBE_TX_TREQ,
	VCS_PROBE_TX_CTM,
	VCS_PROBE_TX_DRC,
	VCS_PROBE_TX_COMFORTNOISE,
    VCS_PROBE_TX_CUSTOMER_EFFECT,
    
	VCS_PROBE_RX_INPUT,
	VCS_PROBE_RX_CTM,
	VCS_PROBE_RX_SE,
	VCS_PROBE_RX_DRC,
    VCS_PROBE_RX_COMFORTNOISE,
    VCS_PROBE_RX_CUSTOMER_EFFECT,

	VCS_PROBE_ADEQ_TX_CALIBRATION_DATA,
    VCS_PROBE_ADEQ_RX_CALIBRATION_DATA,
    
	VCS_PROBE_TX_SE_CONFIG,
	VCS_PROBE_TX_TREQ_CONFIG,
	VCS_PROBE_TX_CTM_CONFIG,
	VCS_PROBE_TX_DRC_CONFIG,
    VCS_PROBE_TX_COMFORTNOISE_CONFIG,
	
	VCS_PROBE_RX_CTM_CONFIG,
	VCS_PROBE_RX_SE_CONFIG,
	VCS_PROBE_RX_DRC_CONFIG,
    VCS_PROBE_RX_COMFORTNOISE_CONFIG,
    VCS_PROBE_RX_CUSTOMER_FIRST_CONFIG,
    VCS_PROBE_RX_CUSTOMER_LAST_CONFIG,
    VCS_PROBE_TX_CUSTOMER_LAST_CONFIG,

	VCS_PROBE_NUMBEROF /**<total number of probes */
  } vcs_probe_points_t;

  /**
	 Result codes
   */
  typedef enum {VCS_OK = 0,  /**< OK */
				VCS_ERROR_PARAM,  /**< Parameter error */
				VCS_ERROR_MEMORY,  /**< Memory error (out of memory) */
				VCS_ERROR_ALG_NOT_SUPPORTED,  /**< Algorithm is not supported */
				VCS_ERROR_ALG,  /**< Error allocating or configuring an algorithm */
				VCS_ERROR_NOT_IMPL,  /**< Sorry, functionality not implemented */
				VCS_ERROR_OTHER  /**< Some other error */
  } vcs_result_t;


  typedef struct vcs_portdef_s {
	unsigned int nb_channels;
	unsigned int samplerate;
	unsigned int interleaved;
  } vcs_portdef_t;

  typedef struct vcs_portsdef_s {
	
	struct {
	  unsigned int nb_channels;
	  unsigned int samplerate;
	  unsigned int interleaved;
	} port[2];
	unsigned int nb_ports;
  } vcs_portsdef_t;

  void vcs_portsdef_init(vcs_portsdef_t *ports,
						unsigned int pos,
						unsigned int nb_channels,
						unsigned int samplerate,
						unsigned int interleaved);
  

  
  /**
	 Mode
   */
  typedef struct {
	unsigned int resolution;  /**< Resolution in bits of PCM (16 or 32 bits)
							   Currently only 16 bit are supported */
	unsigned int samplerate; /**< Sample rate 8000 or 16000 */
	unsigned int framesize;  /**< Number of samples in each execution.
								We have samplerate/1000 samples each 1 ms so
								10 ms will in 8kHz be 10*8 and in 16 kHz be 10*16 */
  } vcs_mode_t;

  typedef struct vcs_log_s {
	 /**
		Function to be used for loggin errors, warning etc
	 @param reason reason for the log
	 @param s string to write to the log
   */
	void (*log)(struct vcs_log_s* hndl,
				vcs_log_kind_t reason,
				const char* s, ...);
	
  /**
	 Method to be used for probing data at specific probe
	 points
	 @param point The probe point
	 @param dir Direction (uplink or downlink)
	 @param data pointer to data block
	 @param nb_channels, number of channels in input
	 @param len_of_channel length in bytes of each channels
   */
	void (*probe)(struct vcs_log_s* hndl,
				  vcs_probe_points_t point,
				  const void* data,
				  int len);
	int (*probe_is_active)(struct vcs_log_s* hndl,
				        vcs_probe_points_t point);
  } vcs_log_t;
  
  typedef struct vcs_export_s vcs_export_t;

  /**
	 Voice call context object containing all supported methods
   */
  
  typedef struct vcs_ctx_s
  {
	/**
	   process data on input buffer and puts result in
	   output buffer
	   @param self context
	   @param inbuf pointer to input buffers
	   @param outbuf pointer to output buffers
	   @return 0 on success otherwise error code
	   
	 */
	int (*process)(struct vcs_ctx_s*self,
				   short** input, int nb_input,
				   short** output, int nb_output);
	

	/**
	   Sets parameters required for process. 
	   @param self context
	   @param resulution resulution of PCM, 16 or 32
	   @param sample_rate Sample rate, either 8000 or 16000
	   @param nb_channels Number of channels, either 1 (mono) or
	   2 (stereo)
	   @param interleaved 0 if not interleaved, otherwise 1
	   @param framesize No of samples in each
	   @return 0 on success otherwise error code
	 */
	int (*set_mode)(struct vcs_ctx_s*self,
					const vcs_mode_t* mode,
					const vcs_portsdef_t* input,
					const vcs_portsdef_t* output);
	/**
	   Sets parametersfor an algorithm 
	   @param self context
	   @param param parameter to set
	   @param data pointer to an array of bytes containing configuration
	   data for algorithm
	   @param len length of data in bytes
	   @return 0 on success otherwise error code
	 */
	int (*set_param)(struct vcs_ctx_s*self,
					 int param,
					 const void *data,
					 int len);
	/**
	   Sets configuration for an algorithm
	   @param self context
	   @param param parameter to set
	   @param data pointer to an array of bytes containing configuration
	   data for algorithm
	   @param len length of data in bytes
	   @return 0 on success otherwise error code
	 */	
	int (*set_config)(struct vcs_ctx_s*self,
					  int param,
					  const void* data,
					  int len);
	/**
	   Returns parameter setting for an algorithm
	   @param self context
	   @param param parameter to get
	   @param data pointer to an array of bytes containing configuration
	   data for algorithm
	   @param len length of data in bytes as in, actual len as out
	   @return 0 on success otherwise error code
	 */	
	int (*get_param)(struct vcs_ctx_s*self,
					 int param,
					 void* data,
					 int *len);
	/**
	   Returns configuration setting for an algorithm
	   @param self context
	   @param param parameter to set
	   @param data pointer to an array of bytes containing configuration
	   data for algorithm
	   @param len length of data in bytes as in, actual len as out
	   @return 0 on success otherwise error code
	 */	
	int (*get_config)(struct vcs_ctx_s*self,
					  int param,
					  void* data,
					  int *len);
	/**
	   Returns a export structure
	   @param self context
	   @param data export data structure
	   @return 0 on success otherwise error code
	*/
	int (*exportdata) (struct vcs_ctx_s*self,
				   vcs_export_t**data);
	/**
	   Imports data that has been exported through export function
	   @param self context
	   @param data export data structure to bee imported. The receiver
	   takes ownership of the structure and at return data will point on NULL
	   @return 0 on success otherwise error code
	*/
	int (*importdata) (struct vcs_ctx_s*self,
				   vcs_export_t**data);
	/**
	   Returns if self is for uplink or downlink processing
	   @param self context
	   @return uplink or downlink
	*/

	
				   
	vcs_processing_kind_t (*get_kind)(const struct vcs_ctx_s*self);
	/**
	   Destroys the object and deallocate all allocated
	   memory. This should be the last usage of the object since
	   after this it does not exist any longer
	   @param self Context
	   @return 0 if OK otherwise error code

	 */
	int (*destroy)(struct vcs_ctx_s*self);
	
	vcs_processing_kind_t  mykind;
		 
  } vcs_ctx_t;
  
 

  void vcs_deinterleave(const short *in, int ilen, short *out[], int nb_ch, int len);
  
  void vcs_interleave(short *in[], int nb_ch, int ilen, short *out, int olen);
  		  

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* _VCS_H */
