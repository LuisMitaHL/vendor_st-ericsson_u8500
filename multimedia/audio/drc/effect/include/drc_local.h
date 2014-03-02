/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file    drc_local.h
* \brief   Internal interface for the DRC processing algorithm library.
* \author  ST-Ericsson
* \version 0.0.1
*/
/*****************************************************************************/
#ifndef _drc_local_h_
#define _drc_local_h_

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "drc.h"
#include "drc_fifo.h"


#if defined(DRC_DEBUG) || defined(DRC_MEASURE_DEBUG) || defined(DRC_GAINSLOPETABLE_DEBUG)
#include <stdio.h>
#endif

#ifdef DRC_DEBUG
  #define PRINT  printf("[DRC] "); printf
#else
  #define PRINT(x,...)
#endif

#ifndef TRUE
#define FALSE  0
#define TRUE  (!FALSE)
#endif


#define DRC_CHECK_ALLOC_SUCCESS(a)           if(NULL == a) { goto DRC_ALLOCATE_ERROR_HANDLING; }
#define DRC_ATTACK_RELEASE_TIME_MAX          ((unsigned short)(16000 << 1))
#define DRC_HARD_SATURATION_APPLY_FORWARD    20
#define DRC_LOOK_AHEAD_TIME_MAX              2000
#define DRC_LEVEL_DETECTOR_TIME_DEFAULT      16
#define DRC_GAIN_PROCESSOR_TIME_DEFAULT      400


/**
 * DRC_LOOK_AHEADTIME_IN_SAMPLES
 * Converts the look ahead time value into its equivalent value in samples.
 * \param   SampleRate     Specifies the current sample rate
 * \param   LookAheadTime  The look ahead in Q15.1
*/
#define DRC_LOOK_AHEADTIME_IN_SAMPLES(SampleRate, LookAheadTime)  ( (unsigned short)((unsigned long)(SampleRate)*(LookAheadTime) / 2000) )


/**
 * QuickDivTable
 * Table with 0.875/x values for x = 0.5->1.
 * 
 * To create the table in Matlab, use this:
 * table = dec2hex(floor((0.875*2^23)./linspace(1,2,33)));
 * table = table(1:32); % Last value is 1, no need for that.
 * 
 * Note that if 0.875 is changed, the condition to perform 
 * SoftSaturation must be updated aswell.
 */
static const signed long QuickDivTable[] = 
{
  0x00700000, 0x006C9B26, 0x00696969, 0x00666666, 0x00638E38, 0x0060DD67, 0x005E50D7, 0x005BE5BE,
  0x00599999, 0x00576A25, 0x00555555, 0x0053594D, 0x0051745D, 0x004FA4FA, 0x004DE9BD, 0x004C415C,
  0x004AAAAA, 0x00492492, 0x0047AE14, 0x00464646, 0x0044EC4E, 0x00439F65, 0x00425ED0, 0x004129E4,
  0x00400000, 0x003EE08F, 0x003DCB08, 0x003CBEEA, 0x003BBBBB, 0x003AC10C, 0x0039CE73, 0x0038E38E
};


/**
 * DRC Control Instance struct
 */
typedef struct
{
    t_bool                   use_peak_measure;   /**< If true, Peak Detector will be used by the algorithm. 
                                                      If not, Root Mean Square (RMS) value is used. RMS is 
                                                      recommended. */
    unsigned short           nof_channels;       /**< Number of input channels to the effect. It may not be
                                                      changed during runtime. */
    unsigned short           look_ahead_time;    /**< Look Ahead Time, in ms expressed in Q14.1. */
    t_drc_static_curve       static_curve;       /**< DRC Static Curve sent by the user via drc_setparam. */
    t_drc_time_specifiers    time_specifiers;    /**< DRC Time specifiers. */
}t_drc_control_instance;


/**
 * DRC Gain Table
 */
typedef struct
{
  signed short  slope;    /**< Expressed in Q7.8. */
  signed short  gain;     /**< Expressed in Q8.8. */
}t_drc_gainslope;


/**
 * DRC Algorithm struct
 * Internal parameters used for the DRC algorithm.
 */
typedef struct
{
    t_bool                   use_peak;                /**< Measurement type RMS / Peak in Level Detector. */
    unsigned short           nof_channels;            /**< Number of channels in the audio stream. */
    signed long              ld_level_state;          /**< Expressed in Q30 for RMS, Q31 for Peak. */
    signed long              gp_attack_gamma;         /**< Exponential Weighting Factor, (0 < gamma < 1), +Q31. */
    signed long              gp_release_gamma;        /**< Exponential Weighting Factor, (0 < gamma < 1), +Q31. */
    signed long              ld_attack_gamma;         /**< Exponential Weighting Factor, (0 < gamma < 1), +Q31. */
    signed long              ld_release_gamma;        /**< Exponential Weighting Factor, (0 < gamma < 1), +Q31. */
    unsigned short           measure_interval;        /**< How often to measure input level, 1, 2 or 3. */
    unsigned short           gain_buffer_offset;      /**< Offset to apply in the gain array when look ahead is
                                                           set non-zero. The reson of having this parameter is in
                                                           the OMX standard, delays are not allowed. The DRC needs
                                                           to keep enough data to produce one frame without inserting
                                                           silence at the beginning of the track avoiding synchonization
                                                           problems (video & audio). */
    signed long             *gp_gain_state;           /**< Gain array calculated in Gain Processor component, Q8.23. */
    signed long              quick_div_table[32];     /**< Local copy from the QuivDivTable is kept to speed up the algorithm. */
    t_drc_gainslope          gain_and_slope_table[DRC_GAIN_AND_SLOPE_TABLE_SIZE];   /**< Gain and Slope table. */
}t_drc_execute_params;


/**
 * DRC Processing Instance struct
 */
typedef struct
{
    t_bool                    algo_start;                   /**< The algorithm needs to measure over two different input frames 
                                                                 and applied gain in order to deliver one output frame with the
                                                                 same size as the input. It is due to non silence frames are not
                                                                 allowed when audio starts. It might avoid synchronization issues. */
    unsigned short            sample_rate;                  /**< Sample rate. */
    unsigned long             frame_size;                   /**< Size in samples from incomming audio frames. */
    unsigned long             frame_offset;                 /**< Number of samples to offset the input buffer when handling 
                                                                 interleaved and non-interleaved audio signals. */
    unsigned short            sample_offset;                /**< Number of samples processing output/input frames. It depends if
                                                                 the sample pattern is interleaved or non-interleaved. */
    unsigned short            delay_in_samples;             /**< Residual samples between the look ahead time in samples and frame
                                                                 size. It is used to offset the gain state array. */
    unsigned short            delay_in_buffers;             /**< Number of frames the algorithm needs to hold before to start the
                                                                 execution. */
    unsigned short            look_ahead_time;              /**< Look Ahead Time, in ms expressed in Q14.1. */
    unsigned long             gain_state_buffer_length;     /**< Gain buffer lenght */
    t_drc_circular_array     *look_ahead_time_buffer;       /**< Circular array struct to store samples when look ahead time is non-zero. */
    t_drc_execute_params      algo_params;                  /**< Algorithm specific parameters struct. */
}t_drc_processing_instance;



/**
 * DRC Instance
 */
typedef struct
{
    t_drc_control_instance      *control;       /**< Pointer to control instance struct. */
    t_drc_processing_instance   *processing;    /**< Pointer to processing instance struct. */
}t_drc_instance;



/**
 * Gives the invers of bit_log.
 * \code x = exp(base2)((bit_log(x) / 2^bit_res) + 1 ); \endcode
 *
 * \param   data_in       Number to apply bitexp operation
 * \param   bit_rest      Bit resolution
 * \return  Bitexp value
 */
signed long  bit_exp(signed short data_in, unsigned short bit_res);


/**
 * Gives logarithmic output approximation according to
 * \code bit_log(x) = 2^bit_res * (log(base 2)(x) - 1); \endcode
 *
 * \param[in] data_in        Number to apply bitlog operation
 * \param[in] bit_rest       Bit resolution
 * \param[in] residual       Residual value from the bitlog operation
 * \return    Bitlog value
 */
signed short bit_log(signed long data_in, unsigned short bit_res, unsigned long *residual);


/**
 * Calculates algorithm-specific parameters to execute the DRC audio effect.
 *
 * \param  control      User parameters to setup DRC (static curve, time specifiers, measure interval, etc.)
 * \param  processing   Execution specific parameters to run the algorithm.
 */
void calculate_algorithm_params(const t_drc_control_instance* const control, t_drc_processing_instance* processing);


/**
 * Applies gain to an input buffer.
 *
 * \param  execute_algorithm  Algorithm specific params. Contains gain_state buffer calculated in Measure Peak/RMS.
 * \param  input              Input buffer.
 * \param  output             Output buffer.
 * \param  buffer_length      Buffer length.
 * \param  sample_offset      Offset to read/write in input and output buffer (stereo/mono).
 * \param  gain_offset        IN: Offset to gp_gain_state to get address where to start read the gain.
 *                            OUT: offset to gp_gain_state to get address after last read gain.
 */
void apply_gain(const t_drc_execute_params* const algo_params, signed short *input, signed short *output, 
                unsigned long buffer_length, unsigned short sample_offset, signed long *gain_offset);


/**
 * Measures all gains for a specific input level.
 *
 * \param  execute_algorithm      Algorithm specific params.
 * \param  in_left_channel        Left channel.
 * \param  in_right_channel       Right channel.
 * \param  sample_offset          Offset to read/write in input and output buffer (stereo/mono).
 * \param  buffer_length          Gain state buffer lenght in samples.
 *
 * \note For mono, in_channel_right must point to a memory cell containing a zero. 
 *       Not an entire buffer, but one 32 bit memory cell is sufficient.
 */
void measure(t_drc_execute_params *algo_params, signed short *in_left_channel, signed short *in_right_channel, 
             const unsigned short sample_offset, const unsigned long buffer_length);

#endif /* _drc_local_h_ */

