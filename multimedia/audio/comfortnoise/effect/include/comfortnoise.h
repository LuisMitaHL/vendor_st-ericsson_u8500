/*****************************************************************************/
/**
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 *
 * \file   comfortnoise.h
 * \brief  Interface for Comfort Noise Generation algoirthm.
 * \author ST-Ericsson
 * \version v0.0.1
 */
/*****************************************************************************/
#ifndef _comfortnoise_h
#define _comfortnoise_h

#ifdef __cplusplus
extern "C" 
{
#endif

/** Minimum noise generation RMS level in dBolv */
#define CNG_NOISE_LEVEL_MIN     -91

/** Maximum noise generation RMS level in dBovl */
#define CNG_NOISE_LEVEL_MAX     -50

/** Type definition for a boolean/logical value */
typedef unsigned char bool_t;

/** General Comfort Noise Generaton (CNG) handle */
typedef void t_comfort_noise_handle; 


/**
 * Parameter struct for Comfort Noise Generation allocation
 */
 typedef struct 
{
    unsigned short  nof_channels;       /**< Mono or stereo. Set up both input and output to 
                                             the same number of channels. */
    unsigned int    frame_size;         /**< Number of samples per channel. */
    bool_t          sample_pattern;     /**< PCM sample pattern. True if the stream is interleaved 
                                             and false if the stream is linear. */
}t_comfort_noise_allocation_params;


/**
 * Parameter struct for Comfort Noise Generation configuration
 */
typedef struct 
{
    bool_t          enable;            /**< Enable/disable the algorithm */
    signed short    level;             /**< RMS noise generation level */
}t_comfort_noise_configuration_params;
 
 
/**
 * Instantiates the Comfort Noise Generation effect.
 *
 * \retval handle   Comfort Noise Generation audio effect handler.
 * \retval NULL     Not enough memory to instantiate the audio effect.
 */
extern t_comfort_noise_handle* comfort_noise_create(void);
 

 /**
 * The instantiate function does a minimal allocation for the instance struct.
 * Allocation of more memory needed by the effect instance is done by the allocate function.\n
 * The input parameters must be the effect specific allocation parameters and system parameters to be used 
 * when is created.\n
 * This function will not allow to reallocate the effect during runtime. It means if the user wants to change
 * some system parameters, the effect must be destroyed and created with new system parameters.\n
 * The memory allocated must be de-allocated by the destroy function.
 *
 * \param  handle            Handle to a CNG instance.
 * \param  allocate_params   Effect specific allocation parameters. System parameters such as system sample
 *                           rate, number of channels and frame size are sent in this struct.
 * \retval TRUE              Allocation was sucessful.
 * \retval FALSE             Out of memory.
 */
extern bool_t comfort_noise_allocate(t_comfort_noise_handle **handle, const t_comfort_noise_allocation_params* allocate_params);


/**
 * This function destroys the effect instance completely, that is, de-allocates all dynamic variables
 * and clears the instance struct to avoid accidental reuse.
 *
 * \param handle  Comfort Noise Generation handle.
 */
extern void comfort_noise_destroy(t_comfort_noise_handle **handle);


/**
 * Reset will reset all internal states and set the audio effect in an initial and predefined state.
 *
 * \note The function must be called after allocating and setting params only the first time. If not,
 * user configuration and algorithm internal parameters will not be reflected during execution.
 *
 * \param handle   Comfort Noise Generation handle.
 */
extern void comfort_noise_reset(t_comfort_noise_handle *handle);


/**
 * The host will be provided a list of parameter groups that can be set.
 * Sometimes the user can set all input parameters in one group, while others have a number of groups 
 * that are set separately.
 *
 * The input parameters to the function are an element from this list, and the desired values
 * of the corresponding parameters. The effect control will now, depending on effect type, process 
 * the parameters, save them, and so on, and finally calculate algorithm specific parameters used in
 * the algorithm execution.
 *
 * \param  handle      Comfort Noise Generation handle.
 * \param  set_params  Effect specific setup parameters.
 * \retval TRUE        The effect can store setup paramenters correclty and calculate effect specific parameters.
 * \retval FALSE       Wrong parameter and the audio effect was not configured correctly.
 */
extern bool_t comfort_noise_setconfigure(t_comfort_noise_handle *handle, const t_comfort_noise_configuration_params* set_params);


/**
 * The input parameter is a parameter identifier from a list of available parameter(s) groups 
 * to get, and the return is the current value of the parameter(s). \n
 * The available parameters can be both allocation and runtime parameters.\n
 * The return parameter struct is allocated inside the function and must be deallocated externally
 * using \e free().
 *
 * \param  handle            Comfort Noise Generation handle.
 * \param  param_specifier   Specifies what parameter to read out.
 * \param  get_params        Double pointer that gets loaded with specified parameters.
 * \retval TRUE              The instance can retrieve the specified parameter and allocate the get_param struct.
 * \retval FALSE             The struct was not allocated and the parameter/parameters can not be retrieved.
 */
extern bool_t comfort_noise_getconfigure(t_comfort_noise_handle *handle, t_comfort_noise_configuration_params** const get_params);


/**
 * Prototype for Comfort Noise Generation process function
 *
 * Memory allocation is not allowed in this method. \n
 * The number of samples to process is always the same.\n
 * Input and output buffer does not belong to the audio effect and must be delivered back to the user.\n
 *
 * \param handle            Comfort Noise Generation handle.
 * \param in_buffer         Handle to input buffer
 * \param out_buffer        Handle to output buffer
 * \param size              Number of samples per channel to be processed (frame size)
 */
extern void comfort_noise_process(t_comfort_noise_handle *handle, signed short *in_buffer, signed short *out_buffer, signed int size);


#ifdef __cplusplus
}
#endif

#endif /* comfortnoise.h */
