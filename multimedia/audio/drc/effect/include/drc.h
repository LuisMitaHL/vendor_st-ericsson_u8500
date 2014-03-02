/*****************************************************************************/
/**
 *  (c) ST-Ericsson, 2010 - All rights reserved
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson
 *
 * \file    drc.h
 * \brief   Interface for Dinamic Range Control algoirthm.
 * \author  ST-Ericsson
 * \version v0.0.1
 */
/*****************************************************************************/
#ifndef _drc_h_
#define _drc_h_

#ifdef __cplusplus
extern "C" 
{
#endif

/** Number of specified points on the Static Curve */
#define DRC_NUMBER_OF_STATIC_CURVE_POINTS 8

/** Number of points to generate the Gain and Slope Table */
#define DRC_GAIN_AND_SLOPE_TABLE_SIZE     117

/** Type definition for a boolean/logical value */
typedef unsigned char t_bool;

/** General DRC handle */
typedef void t_drc_handle; 


/**
 * Parameter struct for DRC Allocation
 */
typedef struct 
{
    unsigned short     nof_channels;       /**< Mono or stereo. Set up both input and output to 
                                                the same number of channels. */
    unsigned short     sample_rate;        /**< Sample rate. */
    unsigned long      frame_size;         /**< Number of samples per channel. */
    t_bool             sample_pattern;     /**< PCM sample pattern. True if the stream is interleaved 
                                                and false if the stream is linear. */
}t_drc_allocation_params;


/**
 * DRC Acceptance Level Specifier
 */
typedef enum 
{
  DRC_ACCEPTANCE_LEVEL_90_PERCENT = 1,  /**< 90 percent of the desired gain must be reached within the attack or release time. */
  DRC_ACCEPTANCE_LEVEL_99_PERCENT,      /**< 99 percent of the desired gain must be reached within the attack or release time. */
  DRC_ACCEPTANCE_LEVEL_99_9_PERCENT     /**< 99.9 percent of the desired gain must be reached within the attack or release time. */
}t_drc_acceptance_level;


/**
 * DRC Static Curve Point
 */
typedef struct 
{
  signed short  x;  /**< Point x in Q7.8. Range [-128..0] dB. */
  signed short  y;  /**< Point y in Q7.8. Range [-128..0] dB. y may not introduce a gain outside 
                         the range [-48..48] dB (including OffsetGain).
                         (The Gain is calculated as the difference between x and y). */
}t_drc_static_curve_point;


/**
 * DRC Static Curve
 *
 * DRC Static Curve describes how to dynamically expand or compress the signal.
 * A linear input value is translated to a dB value on the Static Curve using 20 * log10(abs(input)/2^15)
 * if use_peak_measure or 10 * log10(RMS(input)/2^15) if use_peak_measure is false.
 *
 * Saturation Protection is always active. This will decrease the amount of saturation when the signal is rapidly raising.
 * It also protects against saturation when the input is rapidly decreasing and look ahead time is > 0.
 * (If look ahead time is = 0, saturation cannot occur at all during a decreasing signal).
 */
typedef struct 
{
  t_drc_static_curve_point  static_curve_point[DRC_NUMBER_OF_STATIC_CURVE_POINTS];  
                                          /**< The points describes the Static Curve, with an input on the
                                               x-axis and an output on the y-axis.\n
                                               The gain is described as the difference between the "x" 
                                               point and the "y" point.\n
                                               The static_curve_point array describes the Static Curve 
                                               with the x points in an increasing order. */
  signed short              input_gain;   /**< Input gain to the Level Detector, in dB expressed in Q7.8 
                                               (range [-128..128] dB). Measured input level is amplified
                                               with this gain before translated in the Static Curve. This
                                               may be useful when input levels are too low to utilize the
                                               entire range of the Static Curve. */
  signed short              offset_gain;  /**< Offset to the gain calculated Gain Processor, in dB expressed
                                               in Q7.8. Range [48..-48] dB including the gain introduced by 
                                               the Static Curve. The desired output level, specified by the 
                                               Static Curve, is increased by this offset gain. */
}t_drc_static_curve;


/**
 * DRC Time Specifiers
 */
typedef struct 
{
  unsigned short           level_detector_attack_time;    /**< Attack time for the Level Detector, in ms expressed in Q14.1
                                                               (range [0..16000] ms). */
  unsigned short           level_detector_release_time;   /**< Release time for the Level Detector, in ms expressed in Q14.1
                                                               (range [0..16000] ms). */
  unsigned short           gain_processor_attack_time;    /**< Attack time for the Gain Processor, in ms expressed in Q14.1
                                                               (range [0..16000] ms). */
  unsigned short           gain_processor_release_time;   /**< Release time for the Gain Processor, in ms expressed in Q14.1
                                                               (range [0..16000] ms). */
  t_drc_acceptance_level   acceptance_level;              /**< This value tells how much of the specified gain that must have
                                                               been reached before the attack or release time have elapsed.
                                                               The behavior of DRC prior the introduction of this parameters 
                                                               equals \a DRC_ACCEPTANCE_LEVEL_99_9_PERCENT.
                                                               The gain changes is exponentially low pass filtered, thus 99.9
                                                               percent will take about twice as long to reach as 99 percent
                                                               and three times as long to reach as 90%. 90% is the recommended
                                                               setting. */
}t_drc_time_specifiers;


/**
 * Parameter struct for DRC Configuration
 */
typedef struct 
{
  t_bool                   use_peak_measure;  /**< If true, Peak Detector will be used by the algorithm. 
                                                   If not, Root Mean Square (RMS) value is used. RMS is
                                                   recommended. */
  unsigned short           look_ahead_time;   /**< Maximum Look Ahead Time, in ms expressed in Q14.1.
                                                   Range [0..1000] ms.	
                                                   The value set during allocate will be the maximum
                                                   Look Ahead Time that can be used for this instance.
                                                   It will be used by the algorithm until the instance
                                                   is destroyed. If a new instance is allocated, a new
                                                   value can be set.
                                                   \note look_ahead_time introduces a delay in the audio 
                                                   chain consuming memory for each sample to delay.*/
  t_drc_static_curve       static_curve;      /**< DRC Static Curve. */
  t_drc_time_specifiers    time_specifiers;   /**< DRC Time Specifiers. */
}t_drc_configuration_params;


/**
* DRC Parameter Collection Specifier
*
* This specifier is the input to \ref drc_setparam() and \ref drc_getparam().
*/
typedef enum
{
  DRC_PARAMETER_TYPE_GET_NOF_CHANNELS,                   /**< Used when getting number of channels (mono/stereo). */
  DRC_PARAMETER_TYPE_LOOK_AHEAD_TIME,                    /**< Used when setting or getting Look Ahead Time. */
  DRC_PARAMETER_TYPE_USE_PEAK_MEASURE,                   /**< Used when setting or getting measure mode. Peak if true, else RMS. */
  DRC_PARAMETER_TYPE_ALL_CONFIG_PARAMS,                  /**< Used when setting or getting all paramenters Time Specifiers and Static Curve */
  DRC_PARAMETER_TYPE_STATIC_CURVE,                       /**< Used when setting or getting the parameters of the Static Curve. */
  DRC_PARAMETER_TYPE_TIME_SPECIFIERS,                    /**< Used when setting or getting all the Time Specifiers. */
  DRC_PARAMETER_TYPE_TIME_SPECIFIERS_LDATTACK,           /**< Used when setting the time specifier Level Detector Attack Time. */
  DRC_PARAMETER_TYPE_TIME_SPECIFIERS_LDRELEASE,          /**< Used when setting the time specifier Level Detector Release Time. */
  DRC_PARAMETER_TYPE_TIME_SPECIFIERS_GPATTACK,           /**< Used when setting the time specifier Gain Processor Attack Time. */
  DRC_PARAMETER_TYPE_TIME_SPECIFIERS_GPRELEASE,          /**< Used when setting the time specifier Gain Processor Release Time. */
  DRC_PARAMETER_TYPE_TIME_SPECIFIERS_ACCEPTANCELEVEL     /**< Used when setting the time specifier Acceptance Level. */
}t_drc_parameter_type;


/** 
 * Parameter union making possible to set any of the parameter sets using the same data type (t_drc_param).
*/
typedef union 
{
  t_bool                       use_peak_measure;   /**< [Get]     Parameter corresponding to specifier \a DRC_PARAMETER_TYPE_GET_USE_PEAK_MEASURE. */
  unsigned short               look_ahead_time;    /**< [Get]     Parameter corresponding to specifier \a DRC_PARAMETER_TYPE_GET_LOOK_AHEAD_TIME. */
  unsigned short               nof_channels;       /**< [Get]     Parameter corresponding to specifier \a DRC_PARAMETER_TYPE_GET_NBR_OF_CHANNELS. */
  t_drc_static_curve           static_curve;       /**< [Set/Get] Parameter group that contains both Static Curve and Time Specifiers. */
  t_drc_time_specifiers        time_specifiers;    /**< [Set/Get] Parameter corresponding to specifier \a DRC_PARAMETER_TYPE_STATIC_CURVE. */
  t_drc_configuration_params   all_config_params;  /**< Parameter corresponding to specifier \a DRC_PARAMETER_TYPE_TIME_SPECIFIERS. */
} t_drc_params_union;


/** 
* Parameter struct used when setting or getting DRC runtime parameters.
* This parameter struct is the input to \ref drc_setparam() and \ref drc_getparam().
*/
typedef struct
{
  t_drc_parameter_type  type;    /**< Specifier of which of the available structs in the union to use. */
  t_drc_params_union    param;   /**< The parameters, the struct in the union that is specified by type. */
} t_drc_param;


/**
 * Execute return messages from the effect processing function.
*/
typedef enum
{
  DRC_EXECUTE_RESULT_OK_OUTPUT_DELIVERED = 0,  /**< Everything went okay and output was delivered. */
  DRC_EXECUTE_RESULT_OK_DELAY_STATE,           /**< Everything went okay, but no output was delivered 
                                                    due to internal delays. */
  DRC_EXECUTE_RESULT_OK_NEED_MORE_INPUT,       /**< More input is needed to produce output. This result
                                                    must be used also for effects with 0 output channels. */
  DRC_EXECUTE_RESULT_ERROR_INSTANCE_ERROR,     /**< The instance contains incorrect data. */
  DRC_EXECUTE_RESULT_ERROR_UNKNOWN_ERROR       /**< An unknown error occurred. */
} t_drc_execute_result;



/**
 * Execute state for flush functionality.
 *
 * Flush is a state where the component requests the audio effect to flush out all samples stored in the audio effect.
 * Samples may be stored in internal delay buffers, filter states or such. Just before the first flush executes, the 
 * component will set the audio effect in state DRC_FLUSH_STATE_FLUSH_REQUEST. The audio effect will then set the state
 * to state SFXP_FLUSH_STATE_FLUSH_FINISHED when all samples are flushed out.
 *
 * If the audio effect for example has a 127 samples in internal delay buffers, and the component input buffer is 160 
 * samples long, one execute in flush mode is enough to flush. This audio effect can thus directly set the FlushState
 * to DRC_FLUSH_STATE_FLUSH_FINISHED if it detects that it is in the state DRC_FLUSH_STATE_FLUSH_REQUEST during execute.
 */
typedef enum
{
  DRC_FLUSH_STATE_NO_FLUSH,          /**< Normal execution. */
  DRC_FLUSH_STATE_FLUSH_REQUEST,     /**< The component requires the effect to flush and to set state to
                                          DRC_FLUSH_STATE_FLUSH_FINISHED when finished. */
  DRC_FLUSH_STATE_FLUSH_FINISHED     /**< Audio effect is finished flushing, the component may update to
                                          DRC_FLUSH_STATE_NO_FLUSH at any time. Normal execution. */
} t_drc_flush_state;



/**
 * Instantiates the Dynamic Range Control effect.
 *
 * \retval handle   DRC audio effect handler.
 * \retval NULL     Not enough memory to instantiate the audio effect.
 */
extern t_drc_handle* drc_create(void);


/**
 * The instantiate function does a minimal allocation for the instance struct.
 * Allocation of more memory needed by the effect instance is done by the Allocate function.\n
 * The input parameters must be the effect specific allocation parameters and system parameters to be used 
 * when is created.\n
 * This function will not allow to reallocate the effect during runtime. It means if the user wants to change
 * some system parameters, the effect must be destroyed and created with new system parameters.\n
 * The memory allocated must be de-allocated by the destroy function.
 *
 * \param  handle            Handle to a DRC instance.
 * \param  allocate_params   Effect specific allocation parameters. System parameters such as system sample
 *                           rate, number of channels and frame size are sent in this struct.
 * \retval TRUE              Allocation was sucessful.
 * \retval FALSE             Out of memory.
 */
extern t_bool drc_allocate(t_drc_handle **handle, const t_drc_allocation_params*  const allocate_params);


/**
 * This function destroys the effect instance completely, that is, de-allocates all dynamic variables
 * and clears the instance struct to avoid accidental reuse.
 *
 * \param handle  DRC handle.
 */
extern void drc_destroy(t_drc_handle **handle);


/**
 * Reset will reset all internal states and set the audio effect in an initial and predefined state.
 *
 * \note The function must be called after allocating and setting params only the first time. If not,
 * user configuration and algorithm internal parameters will not be reflected during execution.
 *
 * \param handle   DRC handle.
 */
extern void drc_reset(t_drc_handle *handle);


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
 * \param  handle      DRC handle.
 * \param  set_params  Effect specific setup parameters.
 * \retval TRUE        The effect can store setup paramenters correclty and calculate effect specific parameters.
 * \retval FALSE       Wrong parameter and the audio effect was not configured correctly.
 */
extern t_bool drc_setparam(t_drc_handle *handle, const t_drc_param* const set_params);


/**
 * The input parameter is a parameter identifier from a list of available parameter(s) groups 
 * to get, and the return is the current value of the parameter(s). \n
 * The available parameters can be both allocation and runtime parameters.\n
 * The return parameter struct is allocated inside the function and must be deallocated externally
 * using \e free().
 *
 * \param  handle            DRC handle.
 * \param  param_specifier   Specifies what parameter to read out.
 * \param  get_params        Double pointer that gets loaded with specified parameters.
 * \retval TRUE              The instance can retrieve the specified parameter and allocate the get_param struct.
 * \retval FALSE             The struct was not allocated and the parameter/parameters can not be retrieved.
 */
extern t_bool drc_getparam(t_drc_handle *handle, t_drc_parameter_type param_specifier, t_drc_param** const get_params);


/**
 * Prototype for DRC execute function
 *
 * Memory allocation is not allowed in this method. \n
 * The number of samples to process is always the same.\n
 * Input and output buffer does not belong to the audio effect and must be delivered back to the user.\n
 * When executing, the effect check the flush state to deliver stored frames to the render.\n
 * The effect also indicates if was able to produce an output buffer with the data collect up to the
 * current execution.
 *
 * \param handle            DRC handle.
 * \param in_buffer         Handle to input buffer
 * \param out_buffer        Handle to output buffer 
 * \param flush_state       Set DRC in the flush state. EOS arrives and all data kept in the FIFO must be delivered to the output.
 *                          Normally flush_state is set to \a DRC_FLUSH_STATE_NO_FLUSH, however flush is triggered in the execution,
 *                          when EOS is reached. In this case the flush_state is set to  \a DRC_FLUSH_STATE_FLUSH_REQUEST. The FIFO
 *                          finishes to flush to the render the last frame hold. The algorithm set to \a DRC_FLUSH_STATE_FLUSH_FINISHED
 *                          when the last frame was processed and delivered to the sink.
 *                          The algorithm assumes after the last frame was sent to the function. The remaining calls to flush the FIFO,
 *                          the caller must set \a in_buffer parameter NULL.
 *                          \note The flush state is not applicable when the delay (look ahead time) is 0 ms.
 *                                It is set to \a DRC_FLUSH_STATE_NO_FLUSH.
 *
 * \retval DRC_EXECUTE_RESULT_OK_OUTPUT_DELIVERED     Everything went okay and output was delivered.
 * \retval DRC_EXECUTE_RESULT_OK_NEED_MORE_INPUT      More input is needed to produce an output buffer.
 * \retval DRC_EXECUTE_RESULT_ERROR_INSTANCE_ERROR    The instance contains incorrect data such as NULL
 *                                                    input buffer.
 */
extern t_drc_execute_result drc_execute(t_drc_handle *handle, signed short *in_buffer, signed short *out_buffer, t_drc_flush_state *flush_state);


#ifdef __cplusplus
}
#endif

#endif /* _drc_h_ */
