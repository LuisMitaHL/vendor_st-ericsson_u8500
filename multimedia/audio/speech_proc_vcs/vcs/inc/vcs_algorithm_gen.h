#ifndef _VCS_ALGORITHM_GEN_H_
#define _VCS_ALGORITHM_GEN_H_
/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file     vcs_algorithm_gen.h
 * \brief    Header file for generic algorithm plugin
 * \author   ST-Ericsson
 */
/*****************************************************************************/

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct {
	unsigned int interleaved; /**< only non-interleaved supported in speech proc library */
	unsigned int nb_channels; /**< 1 = mono, 2 == stereo on primary input*/
	unsigned int resolution;  /**< Resolution in bits of PCM (16 or 32 bits)
							   Currently only 16 bit are supported */
	unsigned int samplerate; /**< Sample rate 8000 or 16000 */
	unsigned int framesize;  /**< Number of samples in each execution.
								We have samplerate/1000 samples each 1 ms so
								10 ms will in 8kHz be 10*8 and in 16 kHz be 10*16 */
} vcs_algorith_mode_t;

typedef struct vcs_algorithm_config_s {
   int enabled;
   unsigned int len; // actual size of data
   unsigned short data[256]; 
} vcs_algorithm_config_t;

typedef struct vcs_algorithm_s {
    
    /**
    @param algo self
    @param mode mode (sample rate etc)
    @param config contains enable flag and the actual configuration as array of bytes
   */
	void (*set_config)(struct vcs_algorithm_s* algo,
					  const vcs_algorith_mode_t *mode,
                      const vcs_algorithm_config_t* config);
                      
	void (*reset)(struct vcs_algorithm_s* algo, const vcs_algorith_mode_t *mode);
    
    /** 
    If algorithm is running corresponding execute method will be called. The algorithm is running
    if successfully configured and enabled through set_config method
    @param algo self
    @return 0 if algorithms is not running, 1 if running
    */
    int (*is_running)(struct vcs_algorithm_s* algo);
    
    /**
    @param algo self
    @param input input[0] contains mono or left, input[1] contains right if stereo otherwise undefined
    @param nb_input number of in input channels
    @param input output[0] contains mono or left, output[1] contains right if stereo otherwise undefined
    @param nb_output number of output channels
    */
	void (*execute)(struct vcs_algorithm_s* algo,
				   short **input,
				   int nb_input,
				   short **output,
				   int nb_output); 
                   
    void (*destroy)(struct vcs_algorithm_s* algo);              
} vcs_algorithm_t;

vcs_algorithm_t* speech_proc_rx1_factory(void);
vcs_algorithm_t* speech_proc_rx2_factory(void);
vcs_algorithm_t* speech_proc_tx2_factory(void);

#ifdef __cplusplus
}
#endif

#endif // _VCS_ALGORITHM_GEN_H_
