/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson.
*
* \file    virtual_surround.h
* \brief   Virtual Surround algorithm interface.
* \author  ST-Ericsson
* \version v0.0.1
*/
/*****************************************************************************/
#ifndef _virtual_surround_h_
#define _virtual_surround_h_

#ifdef __cplusplus
extern "C" {
#endif


/** Maximum number of channels supported in the effect */
#define VIRTUAL_SURROUND_MAXCHANNELS_SUPPORT  8

/** Type definition for a boolean/logical value */
typedef unsigned char t_bool;

/** General Virtual Surround handle */
typedef void t_vs_handle;


/**
 * Audio Channel Type enum
 *
 * The enumeration provides channel types supported in the audio effect. Even though the
 * current implementation only supports up to 8 channels, "center surround (CS)" channel
 * is incorporated for future support. The user will set the channel distribution in the
 * stream. It is recommended the user should use the default channel mapping (standard
 * RIFF/WAV mapping as present in standard multi-channel WAV files):
 *
 *      [ FRONT_LEFT FRONT_RIGHT FRONT_CENTER LOW_FREQUENCY BACK_LEFT BACK_RIGHT ]
 */
typedef enum 
{
    AUDIO_CHANNEL_NONE = 0x0,              /**< Unused or empty */
    AUDIO_CHANNEL_LF   = 0x1,              /**< Left front */
    AUDIO_CHANNEL_RF   = 0x2,              /**< Right front */
    AUDIO_CHANNEL_CF   = 0x3,              /**< Center front */
    AUDIO_CHANNEL_LS   = 0x4,              /**< Left surround */
    AUDIO_CHANNEL_RS   = 0x5,              /**< Right surround */
    AUDIO_CHANNEL_LFE  = 0x6,              /**< Low frequency effects */
    AUDIO_CHANNEL_CS   = 0x7,              /**< Center Surround or Back Surround */
    AUDIO_CHANNEL_LR   = 0x8,              /**< Left rear */
    AUDIO_CHANNEL_RR   = 0x9,              /**< Right rear */
    AUDIO_CHANNEL_MAX  = 0x7FFFFFFF        /**< Maximum value unused */
}t_audio_channel_type;


/**
 * Virtual Surround allocation struct
 */
typedef struct 
{
    unsigned short         nof_channels;                                            /**< Number of channels in the stream. */
    unsigned short         sample_rate;                                             /**< Sample rate. */
    int          		frame_size;                                              /**< Number of samples available per channel. */
    t_bool                 interleave;                                              /**< PCM sample pattern. True if the stream is interleaved 
                                                                                         and false if the stream is noninterleave (linear). */
    t_audio_channel_type   channel_mapping[VIRTUAL_SURROUND_MAXCHANNELS_SUPPORT];   /**< Audio channel configuration (PCM channel distribution) */
}t_vs_allocation_params;


/**
 * Virtual Surround Rendering Mode.
 *
 * Rendering mode is the selection if the Virtual Surround effect should process the multichannel 
 * audio to produce either stereo audio signal or a binaural audio signal. The choice of rendenring
 * mode will affect the user experience and the computational load.
 */
typedef enum
{
    VIRTUAL_SURROUND_STANDARD_DOWN_MIX = 1,     /**< Matrix Downmix without left and right surround mixing. */
    VIRTUAL_SURROUND_SURROUND_DOWN_MIX,         /**< Matrix Downmix with left and right surround mixing. */
    VIRTUAL_SURROUND_SURROUND_VIRTUALIZATION    /**< 5.1 channel virtualization. */
} t_vs_rendering_mode;


/**
 * Virtual Surround Room Simulation Mode.
 *
 * Room simulation mode controls the type of audio signals (non-processed or virtualized audio signals)
 * the room simulation block will process.
 */
typedef enum
{
    VIRTUAL_SURROUND_ROOM_SIMULATION_MIXED = 1,      /**< Room Simulation is based on non-processed front/surround audio signals.
                                                          In this mode the unprocessed front and surround channels are inputs to
                                                          the reverberation processing. */
    VIRTUAL_SURROUND_ROOM_SIMULATION_VIRTUALIZED     /**< Room Simulation is based on virtualized front/surround audio signals.
                                                          In this mode the processed front and surround channels are inputs to
                                                          the reverberation processing. */
} t_vs_room_simulation_mode;


/**
 * Virtual Surround Room Simulation Room Type.
 * The room type will completely configure the reverberation block.
 */
typedef enum
{
    VIRTUAL_SURROUND_ROOM_TYPE_DEFAULT = 1,      /**< Default room simulation environment*/
    VIRTUAL_SURROUND_ROOM_TYPE_ROOM,             /**< Room type*/
    VIRTUAL_SURROUND_ROOM_TYPE_LIVING_ROOM,      /**< Living room */
    VIRTUAL_SURROUND_ROOM_TYPE_AUDITORIUM,       /**< Audiotorium */
    VIRTUAL_SURROUND_ROOM_TYPE_CONCERT_HALL,     /**< Concert hall*/
    VIRTUAL_SURROUND_ROOM_TYPE_ARENA,            /**< Arena */
    VIRTUAL_SURROUND_ROOM_TYPE_SMALL_ROOM,       /**< Small room */
    VIRTUAL_SURROUND_ROOM_TYPE_MEDIUM_ROOM,      /**< Medium room */
    VIRTUAL_SURROUND_ROOM_TYPE_LARGE_ROOM,       /**< Large room */
    VIRTUAL_SURROUND_ROOM_TYPE_MEDIUM_HALL,      /**< Medium hall */
    VIRTUAL_SURROUND_ROOM_TYPE_LARGE_HALL        /**< Large hall */
} t_vs_room_simulation_room_type;


/**
 * Virtual Surround Mix Gains
 *
 * The mixing gain for each channel (after processing) can be adjusted before final mixing into left and
 * right channel. Gain value is given in milliBels.
 * \note Millibel: 1 mB = 0.01 dB
 */
typedef struct
{
  signed short lf_gain;      /**< Left front gain. Gain in mB ranging from [-7800..1200] where -7800 is mute. */
  signed short rf_gain;      /**< Right front gain. Gain in mB ranging from [-7800..1200] where -7800 is mute. */
  signed short cf_gain;      /**< Center front gain. Gain in mB ranging from [-7800..1200] where -7800 is mute. */
  signed short ls_gain;      /**< Left surround gain. Gain in mB ranging from [-7800..1200] where -7800 is mute. */
  signed short rs_gain;      /**< Right surround gain. Gain in mB ranging from [-7800..1200] where -7800 is mute. */
  signed short lfe_gain;     /**< Low frequency effects gain. Gain in mB ranging from [-7800..1200] where -7800 is mute. */
  signed short fr_gain;      /**< Front reverberation gain. Gain in mB ranging from [-7800..1200] where -7800 is mute. */
  signed short sr_gain;      /**< Surround reverberation gain. Gain in mB ranging from [-7800..1200] where -7800 is mute. */
} t_vs_mix_gains;


/**
 * Virtual Surround configuration
 */
typedef struct 
{
  t_bool                             room_simulation_front;       /**< Enable/disable room simulation for the Front speaker pair (LF and RF). */
  t_bool                             room_simulation_surround;    /**< Enable/disable room simulation for the Surround speaker pair (LS and RS). */
  t_vs_room_simulation_mode          room_simulation_mode;        /**< Virtual surround algorithm type. */
  t_vs_room_simulation_room_type     room_simulation_room_type;   /**< Room simulation mode.*/
  t_vs_rendering_mode                rendering_mode;              /**< Room type where audio is virtualized.*/
  t_vs_mix_gains                     gains;                       /**< Mixing gains */
} t_vs_configuration_params;


/**
 * Virtual Surround parameter collection specifier.
 *
 * This specifier is the input to \ref virtual_surround_setparams().
 */
typedef enum
{
  VIRTUAL_SURROUND_PARAMETER_TYPE_GET_NOF_CHANNELS,                  /**< Used when getting number of channels */
  VIRTUAL_SURROUND_PARAMETER_TYPE_RENDERING_MODE,                    /**< Used when setting or getting rendering mode. */
  VIRTUAL_SURROUND_PARAMETER_TYPE_ROOM_SIMULATION_MODE,              /**< Used when setting or getting room simulation mode. */
  VIRTUAL_SURROUND_PARAMETER_TYPE_ROOM_SIMULATION_FRONT,             /**< Used when setting or getting room simulation for front speakers.*/
  VIRTUAL_SURROUND_PARAMETER_TYPE_ROOM_SIMULATION_SURROUND,          /**< Used when setting or getting room simulation for surround speakers. */
  VIRTUAL_SURROUND_PARAMETER_TYPE_ROOM_SIMULATION_ROOM_TYPE,         /**< Used when setting or getting the room type used in the room simulation. */
  VIRTUAL_SURROUND_PARAMETER_TYPE_ALL_CONFIG_PARAMS,                 /**< Used when setting or getting all algorithm configuration parameters.*/
  VIRTUAL_SURROUND_PARAMETER_TYPE_LEFT_FRONT_GAIN,                   /**< Used when setting or getting left front channel gain. */
  VIRTUAL_SURROUND_PARAMETER_TYPE_RIGHT_FRONT_GAIN,                  /**< Used when setting or getting right front channel gain. */
  VIRTUAL_SURROUND_PARAMETER_TYPE_CENTER_FRONT_GAIN,                 /**< Used when setting or getting center front channel gain. */
  VIRTUAL_SURROUND_PARAMETER_TYPE_LEFT_SURROUND_GAIN,                /**< Used when setting or getting left surround channel gain. */
  VIRTUAL_SURROUND_PARAMETER_TYPE_RIGHT_SURROUND_GAIN,               /**< Used when setting or getting right surround channel gain. */
  VIRTUAL_SURROUND_PARAMETER_TYPE_LOW_FREQUENCY_EFFECTS_GAIN,        /**< Used when setting or getting low frequency effects channel gain. */
  VIRTUAL_SURROUND_PARAMETER_TYPE_FRONT_ROOM_SIMULATION_GAIN,        /**< Used when setting or getting front reverberation channel gain. */
  VIRTUAL_SURROUND_PARAMETER_TYPE_SURROUND_ROOM_SIMULATION_GAIN,     /**< Used when setting or getting surround reverberation channel gain. */
  VIRTUAL_SURROUND_PARAMETER_TYPE_MIX_GAINS                          /**< Used when setting or getting mixing gains in the virtualization block */
}t_vs_parameter_type;


/** 
 * Parameter union making possible to set any of the parameter sets using the same data type (t_vs_params).
 */
typedef union 
{
  unsigned short                   nof_channels;               /**< [Get]     Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_GET_NOF_CHANNELS. */
  t_bool                           room_simulation_front;      /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_ROOM_SIMULATION_FRONT. */
  t_bool                           room_simulation_surround;   /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_ROOM_SIMULATION_SURROUND. */
  t_vs_rendering_mode              rendering_mode;             /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_RENDERING_MODE. */
  t_vs_room_simulation_mode        room_simulation_mode;       /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_ROOM_SIMULATION_MODE. */
  t_vs_room_simulation_room_type   room_simulation_room_type;  /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_ROOM_SIMULATION_ROOM_TYPE. */
  signed short                     lf_gain;                    /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_LEFT_FRONT_GAIN. */
  signed short                     rf_gain;                    /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_RIGHT_FRONT_GAIN. */
  signed short                     cf_gain;                    /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_CENTER_FRONT_GAIN. */
  signed short                     ls_gain;                    /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_LEFT_SURROUND_GAIN. */
  signed short                     rs_gain;                    /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_RIGHT_SURROUND_GAIN. */
  signed short                     lfe_gain;                   /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_LOW_FREQUENCY_EFFECTS_GAIN. */
  signed short                     fr_gain;                    /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_FRONT_ROOM_SIMULATION_GAIN. */
  signed short                     sr_gain;                    /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_SURROUND_ROOM_SIMULATION_GAIN. */
  t_vs_mix_gains                   gains;                      /**< [Set/Get] Parameter corresponding to specifier \a VIRTUAL_SURROUND_PARAMETER_TYPE_MIX_GAINS. */
  t_vs_configuration_params        all_config_params;          /**< [Set/Get] Parameter group that contains all parameters required to set the effect. */
} t_vs_params_union;


/** 
 * Parameter struct used when setting or getting effect configuration.
 * This struct is the input to \ref virtual_surround_setparam() and \ref virtual_surround_getparam().
 */
typedef struct
{
  t_vs_parameter_type  type;    /**< Specifier of which of the available structs in the union to use. */
  t_vs_params_union    param;   /**< The parameters, the struct in the union that is specified by type. */ 
} t_vs_param;


/**
 * The fuction creates a virtual surround instance.
 *
 * \retval handle  Virtual Surround handle.
 * \retval NULL    Not enough memory to instantiate the audio effect.
 */
extern t_vs_handle* virtual_surround_create(void);


/**
 * The instantiate function does a minimal allocation for the instance struct.
 * Allocation of more memory needed by the effect instance is done by the allocate function.\n
 * Input parameters must be system and effect specific parameters used when that is created.\n
 * This function will not allow to reallocate the effect during runtime. It means if the caller
 * wants to change some system parameter, the effect must be destroyed and created with new 
 * system settings. Memory allocated must be de-allocated by the destroy function.
 *
 * \param  handle            Virtual Surround handle.
 * \param  allocate_params   System and effect specific allocation parameters(such as sample rate,
 *                           number of channels, frame size, etc).
 * \retval TRUE              Allocation was sucessful.
 * \retval FALSE             Out of memory.
 */
extern t_bool virtual_surround_allocate(t_vs_handle **handle, const t_vs_allocation_params*  const allocate_params);


/**
 * This function destroys the effect instance completely, that is, de-allocates all dynamic variables
 * and clears the instance struct to avoid accidental reuse.
 *
 * \param  handle  Virtual Surround handle.
 */
extern void virtual_surround_destroy(t_vs_handle **handle);


/**
 * Reset will clean all internal states and set the audio effect in an initial and predefined state.
 *
 * \param  handle  Virtual Surround handle.
 */
extern void virtual_surround_reset(t_vs_handle *handle);


/**
 * The host will be provided a list of parameter groups that can be set.
 * Sometimes the caller can set all input parameters in one group, while others have a 
 * number of groups that are set separately.
 * 
 * The input parameters to the function are an element from this list, and the desired values
 * of the corresponding parameters. The effect control will now, depending on effect type, 
 * process the parameters, save them, and finally calculate algorithm specific parameters 
 * used in the algorithm execution.
 *
 * \param  handle       Virtual Surround handle.
 * \param  set_params   Effect specific setup parameters.
 *
 * \retval TRUE         The effect stored configuration parameter(s) correctly.
 * \retval FALSE        Wrong parameter, the effect was not configured correctly.
 */
extern t_bool virtual_surround_setparam(t_vs_handle *handle, const t_vs_param* const set_params);


/**
 * The input parameter is a parameter identifier from a list of available parameter(s)
 * groups to get, and the return is the current value of those parameter(s).\n
 * The available parameters can be both allocation and runtime parameters.\n
 *
 * The return parameter struct is allocated inside the function and must be deallocated 
 * externally using \a free().
 *
 * \param  handle           Virtual Surround handle.
 * \param  param_specifier  Specifies what parameter to read out.
 * \param  get_params       Double pointer that gets loaded with specified parameters.
 *
 * \retval TRUE             The instance can retrieve the specified parameter and allocate the struct.
 * \retval FALSE            The struct was not allocated and any parameter can be retrieved.
 */
extern t_bool virtual_surround_getparam(t_vs_handle *handle, t_vs_parameter_type param_specifier, t_vs_param** const get_params);


/**
 * Prototype for Virtual Surround execute function
 *
 * Memory allocation is not allowed in this method.\n
 * The number of samples to process is always the same.\n
 * Input and output buffers do not belong to the audio effect and must be returned to the user.\n
 * The effect is inplace, always gets an input frame and delivers an output frame stereo.
 *
 * \param handle   Virtual Surround handle.
 * \param input    Multichannel input stream.
 * \param output   Output buffer.
 */
extern void virtual_surround_execute(t_vs_handle *handle, signed short *input, signed short *output);


#ifdef __cplusplus
}
#endif

#endif /* _virtual_surround_h_ */
