#ifndef _VCS_ALGO_H_
#define _VCS_ALGO_H_
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_algo.h
 * \brief    Interface file for algorithm definitions
 * \author   ST-Ericsson
 */
/*****************************************************************************/
#include "vcs.h"
#ifdef DRC
#include "drc.h"
#endif

#ifdef VCSSE
#include "speech_enhancement.h"
#endif

#ifdef VCSCTM
#include "tty.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

  /**
	 Enumeration of each supported algorithm
   */
  typedef enum {
	VCS_CTM,                    /**< Ctm  */
	VCS_RXSPEECHENHANCEMENT,    /**< RX speech enhancement */
	VCS_TXSPEECHENHANCEMENT,    /**< TX speech enhancement */
	VCS_DRC,                    /**< DRC  */
	VCS_TREQ,                   /**< Transduse equalizer  */
    VCS_COMFORTNOISE,           /**< Comfort noise */
    VCS_CUSTOMER_PLUGIN_FIRST,  /**< Customer first plugin */
    VCS_CUSTOMER_PLUGIN_LAST,   /**< Customer last plugin */
	VCS_ALGO_LAST               /**< Last */
  } vcs_algo_kind_t;

  typedef struct {
	unsigned int interleaved;
	unsigned int nb_channels; /**< 1 = mono, 2 == stereo on primary input*/
	unsigned int nb_channels2; /**< 1 = mono, 2 == stereo on secondary source*/
	unsigned int resolution;  /**< Resolution in bits of PCM (16 or 32 bits)
							   Currently only 16 bit are supported */
	unsigned int samplerate; /**< Sample rate 8000 or 16000 */
	unsigned int framesize;  /**< Number of samples in each execution.
								We have samplerate/1000 samples each 1 ms so
								10 ms will in 8kHz be 10*8 and in 16 kHz be 10*16 */
  } vcs_algo_mode_t;

  
  void vcs_to_algo_mode(vcs_algo_mode_t *algomode,
						const vcs_mode_t *mode,
						unsigned int nb_channels,
						unsigned int interleaved);
   void vcs_to_algo_mode2(vcs_algo_mode_t *algomode,
						  const vcs_mode_t *mode,
						  unsigned int nb_channels,
						  unsigned int nb_channels2,
						  unsigned int interleaved);
  

#ifdef DRC
  /**
	 Configuration for DRC
   */
  typedef struct {
	int enabled; 
	t_drc_configuration_params config_param;
  } vcs_drc_config_t;
  
  /**
	 Parameter setting for DRC.
   */
  
#else
   typedef struct {
	int enabled; 
   } vcs_drc_config_t;
  
  typedef struct {
  } vcs_drc_param_t;
  
#endif


typedef struct vcs_se_probedata_s {
  vcs_probe_points_t kind;
  unsigned int datalen;
  unsigned int alloclen;
  short data[1];
} vcs_se_probedata_t; 
  
#ifdef VCSSE

  typedef struct {
	int enabled;
	SE_TxParameters_t txparam;
  } vcs_setx_config_t;

  typedef struct {
	int enabled;
	SE_RxParameters_t rxparam;
  } vcs_serx_config_t;

  typedef struct SE_TxExportContainer_t vcs_se_export_t;
  
#else
  typedef struct {
	int enabled;
  } vcs_setx_config_t;
  
  
  typedef struct {
	int enabled;
  } vcs_serx_config_t;
  
  typedef struct vcs_se_export_s {int dummy; } vcs_se_export_t;
  
#endif

#ifdef VCSTREQ
   typedef struct {
	int enabled;
  } vcs_treq_config_t;
#else
   typedef struct {
	int enabled;
  } vcs_treq_config_t;
#endif
  
  typedef struct {
	int enabled;
  } vcs_ctm_config_t;
  
#ifdef VCSCTM

  typedef struct {
	TTY_RxSyncState_t rxsync;
	int handover_detected;
  } vcs_ctmrx_export_t;
  typedef struct {
	short dummy;
  } vcs_ctmtx_export_t;

  
#else
   typedef struct {
	int dummy;
  } vcs_ctmrx_export_t;
  typedef struct {
	int dummy;
  } vcs_ctmtx_export_t;
#endif

  typedef struct {
    int enabled;
    short level;
  } vcs_comfortnoise_config_t;
  

  typedef struct vcs_export_rx_s {
	vcs_se_export_t*se;
  } vcs_export_rx_t;
  
  typedef struct vcs_export_tx_s {
	vcs_se_export_t*se;
	vcs_ctmrx_export_t ctmrx;
  } vcs_export_tx_t;

  struct vcs_export_s {
	vcs_export_tx_t totx;
	vcs_export_rx_t torx;
  };
  
  typedef enum {
    VCS_DRC_RX,
    VCS_DRC_TX,
    VCS_SPEECHENHANCEMENT_TX,
    VCS_SPEECHENHANCEMENT_RX,
    VCS_CTM_TX,
    VCS_CTM_RX
  } vcs_set_config_type_t;
  
  struct vcs_probe_set_config_s {
    vcs_set_config_type_t type;
	unsigned int len;
    union {
	  vcs_serx_config_t serx;
	  vcs_setx_config_t setx;
	  vcs_drc_config_t drc;
	  vcs_ctm_config_t ctm;
	} data;
  } ;
  
#ifdef __cplusplus
}
#endif 

#endif // _VCS_ALGO_H_
