/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson.
*
* \file    virtual_surround_local.h
* \brief   Virtual Surround algorithm interface.
* \author  ST-Ericsson
* \version v0.0.1
*/
/*****************************************************************************/
#ifndef _virtual_surround_local_h_
#define _virtual_surround_local_h_

#ifdef NEON
#include <arm_neon.h>  
//#else
//#include "stm-arm-neon.h"
#endif

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "t_spl.h"
#include "r_spl.h"
#include "virtual_surround.h"
#include "virtual_surround_delayline.h"
/*
#ifdef NEON
#include <arm_neon.h>  
#else
#include "stm-arm-neon.h"
#endif
*/
#ifdef VIRTUAL_SURROUND_DEBUG
  #include <stdio.h>
  #define PRINT  printf("[VSURROUND] "); printf
#else
//  #define PRINT(x,...)
#include <stdio.h>
#define PRINT printf
#endif

#ifndef TRUE
#define FALSE  0
#define TRUE  (!FALSE)
#endif


#define VIRTUAL_SURROUND_MIN_GAIN                   -7800
#define VIRTUAL_SURROUND_MAX_GAIN                    1200
#define VIRTUAL_SURROUND_DEFAULT_GAIN                0
#define VIRTUAL_SURROUND_CF_LFE_GAIN                 0x1A82
#define VIRTUAL_SURROUND_FILTER_SIZE                 24
#define VIRTUAL_SURROUND_ITD_FRONT_DELAY_SIZE        20
#define VIRTUAL_SURROUND_ITD_SURROUND_DELAY_SIZE     30


/**
 * VIRTUAL_SURROUND_CONVERT_MILLIBELLS
 * Converts a specific channel gain in millibel into its equivalent Q value.
 * \param x   Specific gain provided by the user.
 * \param q   Gain in Q2.13.
 */
#define VIRTUAL_SURROUND_CONVERT_MILLIBELLS(q, x) \
do { \
  if ((x) == (-7800)) \
     q = 0; \
  else \
     q = (signed short)(0x2000*powf(10.0,(float)((x)/(2000.0)))); \
}while(0)


/**
 * VIRTUAL_SURROUND_SCALED_MIX_GAIN
 * Scales a specific linear gain into its equivalent Q value.
 * \param  x       Scaled gain expressed in Q2.13.
 * \param  gain    Specific gain provided by the user.
 * \param  value   Scale value expressed in Q2.13.
 */
#define VIRTUAL_SURROUND_SCALED_MIX_GAIN(x, gain, value) \
do { \
    tmp = (sint32)(((sint32)gain * (sint32)value) >> 13); \
    if(tmp > (1<<15)) \
        tmp = 0x7FFF; \
    else if(tmp < -(1<<15)) \
        tmp = 0x8000; \
    x = (signed short)tmp; \
}while(0)


/**
 * StandardDownmixScaleValuesLeft
 * Scale factors for standard dowmix mode used in the multichannel
 * mixer. Values expressed in Q15.
 */
static const signed short StandardDownmixScaleValuesLeft[] =
{ 
  0x257D, 0x0000, 0x1A82, 0x1A82, 0x257D, 0x0000
};


/**
 * StandardDownmixScaleValuesRight
 * Scale factors for standard dowmix mode used in the multichannel
 * mixer. Values expressed in Q15.
 */
static const signed short StandardDownmixScaleValuesRight[] =
{
  0x0000, 0x257D, 0x1A82, 0x1A82, 0x0000, 0x257D
};


/**
 * SurroundDownmixScaleValuesLeft
 * Scale factors for surround dowmix mode used in the multichannel
 * mixer. Values expressed in Q15.
 */
static const signed short SurroundDownmixScaleValuesLeft[] =
{
  0x219C, 0x0000, 0x17C1, 0x17C1, 0xE48F, 0xEC99
};


/**
 * SurroundDownmixScaleValuesRight
 * Scale factors for surround dowmix mode used in the multichannel
 * mixer. Values in Q15.
 */
static const signed short SurroundDownmixScaleValuesRight[] =
{
  0x0000, 0x219C, 0x17C1, 0x17C1, 0x1367, 0x1B71
};


/**
 * FrontCrossPathFilter
 * Filter coefficients for the front cross path channels.
 * Values are normalized and expressed in Q15.
 * HR-filter corresponding to 15 degrees azimuth angle and 0 degree
 * elevation (one filter for left ear and one filter for right ear)
 * Using these two filters it is possible to position the LF signal
 * and the RF signal since the filters are symmetric across azimuth
 * 0 degree.
 */
static const signed short FrontCrossPathFilter[] = 
{
  0xFCA6, 0x051C, 0xFFBC, 0xFA9A, 0xFC80, 0xFDD7, 0x0B64, 0xF689,
  0x0F56, 0xF32E, 0x00AA, 0xF5C7, 0xFCB9, 0x1A1F, 0xDD5A, 0x354A,
  0xC769, 0x4054, 0xA774, 0x4847, 0xC073, 0x3C3A, 0xCF6C, 0x320D
};


/**
 * SurroundCrossPathFilter
 * Filter coefficients for the surround cross path channels.
 * Values are normalized and expressed in Q15.
 * HR-filter corresponding to 15 degrees azimuth angle and 0 degree
 * elevation (one filter for left ear and one filter for right ear)
 * Using these two filters it is possible to position the LS signal
 * and the RS signal since the filters are symmetric across azimuth
 * 0 degree.
 */
static const signed short SurroundCrossPathFilter[] = 
{
  0xFEA8, 0x01EA, 0xFE70, 0x0164, 0xFDE7, 0x03A2, 0x015B, 0x005C,
  0x016F, 0xFC26, 0x010A, 0xFCD5, 0x03D4, 0x03DC, 0xFBFE, 0x08DA,
  0xF52C, 0x0C4D, 0xEDDC, 0x1354, 0xEEE1, 0x1091, 0xF5CC, 0x15AB
};


/**
 * RoomTypeTaps
 * Filter coefficients for the room filter (IIR-Biquad) in the early
 * reverbation. The room filter is a first order IIR filter with
 * transfer function:
 *
 *                 H(z) =      b0
 *                        -------------
 *                         1 - a1 *z^-1
 */
static const SPL_BiQuad_t RoomTypeTaps[] =
{
  // I3DL2_ROOM_DEFAULT
  { {0x0000, 0, 0, 0x0000, 0}, {0, 0}},

  // I3DL2_ROOM_ROOM
  { {0x6809, 0, 0, 0xD7F7, 0}, {0, 0}},

  // I3DL2_ROOM_LIVINGROOM
  { {0x7EB6, 0, 0, 0xC14A, 0}, {0, 0}},

  // I3DL2_ROOM_AUDITORIUM
  { {0x68BE, 0, 0, 0xD742, 0}, {0, 0}},

  // I3DL2_ROOM_CONCERTHALL
  { {0x6979, 0, 0, 0xD687, 0}, {0, 0}},

  // I3DL2_ROOM_ARENA
  { {0x6E73, 0, 0, 0xD18D, 0}, {0, 0}},

  // I3DL2_ROOM_SMALLROOM
  { {0x6C32, 0, 0, 0xD3CE, 0}, {0, 0}},

  // I3DL2_ROOM_MEDIUMROOM
  { {0x6C32, 0, 0, 0xD3CE, 0}, {0, 0}},

  // I3DL2_ROOM_LARGEROOM
  { {0x6C32, 0, 0, 0xD3CE, 0}, {0, 0}},

  // I3DL2_ROOM_MEDIUMHALL
  { {0x6C32, 0, 0, 0xD3CE, 0}, {0, 0}},

  // I3DL2_ROOM_LARGEHALL
  { {0x6C32, 0, 0, 0xD3CE, 0}, {0, 0}},
};


/**
 * Input Frame
 *
 * Internal frame containing every channel enconded in the audio stream. 
 * \note Buffers are only allocated if samples are interleaved. 
 */
typedef struct
{
  signed short         *lf;                                                    /**< Left front buffer. */
  signed short         *rf;                                                    /**< Right front buffer.*/
  signed short         *cf;                                                    /**< Center front buffer.*/
  signed short         *ls;                                                    /**< Left surround buffer.*/
  signed short         *rs;                                                    /**< Right surround buffer.*/
  signed short         *lr;                                                    /**< Left rear buffer. */
  signed short         *rr;                                                    /**< Right rear buffer. */
  signed short         *lfe;                                                   /**< Low frequency effects buffer. */
  signed short         *buffer;                                                /**< Buffer used to allocate all buffers in single memory block. */
  unsigned short       channel_order[VIRTUAL_SURROUND_MAXCHANNELS_SUPPORT];    /**< Sample position in the multichannel raw frame. */
  t_audio_channel_type channel_mapping[VIRTUAL_SURROUND_MAXCHANNELS_SUPPORT];  /**< Audio channel configuration (PCM channel distribution in the frame). */
} t_vs_input_frame;


/**
 * Crosspath settings
 * Filter configuration used in front & surround crosspath.
 */
typedef struct
{
  SPL_vint16Q_t fir_front;                                           /**< FIR filter used in front crosspath. */
  SPL_vint16Q_t fir_surround;                                        /**< FIR filter used in surround crosspath. */
  signed short  *lf_delayline;                                       /**< Left front crosspath delay line. */
  signed short  *rf_delayline;                                       /**< Right front crosspath delay line. */
  signed short  *ls_delayline;                                       /**< Left surround crosspath delay line. */
  signed short  *rs_delayline;                                       /**< Right surround crosspath delay line. */
  signed short  *lf_crosspath;                                       /**< Left front channel crosspath output buffer. */
  signed short  *rf_crosspath;                                       /**< Right front channel crosspath output buffer. */
  signed short  *ls_crosspath;                                       /**< Left surround channel crosspath output buffer. */
  signed short  *rs_crosspath;                                       /**< Right surround channel crosspath output buffer. */
  signed short  lf_itd[VIRTUAL_SURROUND_ITD_FRONT_DELAY_SIZE];       /**< ITD delay for left front crosspath. */
  signed short  rf_itd[VIRTUAL_SURROUND_ITD_FRONT_DELAY_SIZE];       /**< ITD delay for right front crosspath. */
  signed short  ls_itd[VIRTUAL_SURROUND_ITD_SURROUND_DELAY_SIZE];    /**< ITD delay for left surround crosspath. */
  signed short  rs_itd[VIRTUAL_SURROUND_ITD_SURROUND_DELAY_SIZE];    /**< ITD delay for right surround crosspath. */
  signed short  surround_crosspath[VIRTUAL_SURROUND_FILTER_SIZE];    /**< Front crosspath filter coeficients. */
  signed short  front_crosspath[VIRTUAL_SURROUND_FILTER_SIZE];       /**< Surround crosspath filter coeficients. */
} t_vs_crosspath_params;


/**
 * Virtualization Processing output
 * Output buffers delivered to the room simulation and mixer blocks if virtualization mode is enable.
 */
typedef struct
{
  signed short *lf_reverberation;    /**< Left front reverberation (Left Front + Right Front crosspath).  */
  signed short *rf_reverberation;    /**< Right front reverberation (Right Front + Left Front crosspath). */
  signed short *ls_reverberation;    /**< Left surround reverberation (Left Surround + Right Surround crosspath). */
  signed short *rs_reverberation;    /**< Right surround reverberation (Right surround + Left Surround crosspath). */
  signed short *lo_virtualization;   /**< Left output channel (Center Front + LFE + Left Front + Left Surround). */
  signed short *ro_virtualization;   /**< Right output channel (Center Front + LFE + Right Front + Right Surround). */
  signed short *buffer;              /**< Buffer used as heap block for all output buffers in the virtualization processing block.*/
} t_vs_virtualization_output;


/**
 * Virtualization Processing
 */
typedef struct
{
  uint32                     nof_samples;     /**< Number of samples in every channel. */
  unsigned short                    nof_channels;    /**< Number of channels in the input stream. */
  t_vs_mix_gains                    mix_gains;       /**< Mixing gains for each channel being applied before the
                                                          filtering and mixing process.
                                                          Gain values expressed in Q-format (Q2.13) ranging from
                                                          [0..32767]. A milliBell value is translated to a linear
                                                          input on the Mix Gain using 10^(mB / (100*20)). */
  t_vs_crosspath_params             fir_params;      /**< Filter settings in the front & surround crosspath
                                                          components. */
  t_vs_virtualization_output        output;          /**< Outputs sent to the multichannel mixer and the room
                                                          simulation blocks. Observe the multiple output pairs

                                                          Left & right output virtualization buffers are delivered
                                                          to the Multichannel Mixer.
                                                          Left & right front reverberation buffers are sent to the
                                                          Room Simulation.
                                                          Left & right surround reverberation buffers are sent to
                                                          the Room Simulation. */
} t_vs_virtualization_processing;


/**
 * Room Simulation Early Reverberation
 *
 * First order IIR filter (room filter) and delay lines with cross coupling configuration
 * for the 'early reverberation'.
 */
typedef struct
{
  SPL_BiQuad_t     *l_filter;           /**< Left channel IIR filter. */
  SPL_BiQuad_t     *r_filter;           /**< Right channel IIR filter. */
  t_vs_delay_line  *l_delayline;        /**< Left delay line and cross coupling buffer. */
  t_vs_delay_line  *r_delayline;        /**< Right delay line and cross coupling buffer. */
} t_vs_room_simulation_early_reverb;


/**
 * Room Simulation buffer
 *
 * Output buffers in room simulation block and applying the early reverberation for each audio
 * component (front/surround).
 */
typedef struct
{
  signed short *lf_room_simulation;   /**< Left room simulation front. */
  signed short *rf_room_simulation;   /**< Right room simulation front. */
  signed short *ls_room_simulation;   /**< Left room simulation surround. */
  signed short *rs_room_simulation;   /**< Right room simulation surround. */
  signed short *buffer;               /**< Block used as heap block all outputs in the room simulation component. */
} t_vs_room_simulation_output;


/**
 * Room Simulation Processing
 */
typedef struct
{
  uint32                       nof_samples;    /**< Number of samples in every channel. */
  t_vs_room_simulation_output         output;         /**< Front and surround room simulation output buffers. */
  t_vs_room_simulation_early_reverb   front;          /**< Early reverberation unit for front channel components. */
  t_vs_room_simulation_early_reverb   surround;       /**< Early reverberation unit for surround channel components. */
} t_vs_room_simulation_processing;


/**
 * Standard/Surround Downmix Scaled Gains
 *
 * Scaled gains used in the multichannel mixer component if the render mode is Standard or Surround Downmix.
 * The virtualization processing component is disable and the stereo output is the mix between non-processed
 * data and reverberation components.
 */
typedef struct
{
  t_vs_mix_gains  left;     /**< Scaled gains for lefth channel. */
  t_vs_mix_gains  right;    /**< Scaled gains for right channel. */
} t_vs_scale_mix_gains;


/**
 * Room Simulation Gains
 * Gains applied in multichannel mixer when virtualization mode is enable.
 */ 
typedef struct
{
  signed short fr_gain;          /**< Front reverberation gain. */
  signed short sr_gain;          /**< Surround reverberation gain. */
} t_vs_room_simulation_gains;


/**
 * Multichannel Mixer
 */
typedef union
{
  t_vs_scale_mix_gains         downmix;         /**< Scaled gains equal Gxy_user * Scaled_value
                                                     for left and right channels. */
  t_vs_room_simulation_gains   reverberation;   /**< Reverberation gains for front and surround
                                                     reverberation if room simulation is enable. */
} t_vs_multichannel_mixer;


/**
 * Virtual Surround Configuration
 * Settings to enable processing blocks in the effect (Virtualization & Room Simulation components).
 */
typedef struct
{
  t_bool                             room_simulation_front;        /**< Enable/disable room simulation processing
                                                                        for the front speaker pair (LF and RF). */
  t_bool                             room_simulation_surround;     /**< Enable/disable room simulation processing
                                                                        for the surround speaker pair (LS and RS). */
  t_vs_room_simulation_mode          room_simulation_mode;         /**< Room simulation mode. */
  t_vs_room_simulation_room_type     room_simulation_room_type;    /**< Room type where audio is virtualized. */
  t_vs_rendering_mode                rendering_mode;               /**< Virtual surround algorithm type. */
} t_vs_configuration;


/**
 * Virtual Surround instance
 */
typedef struct
{
  unsigned short                     nof_channels;       /**< Number of channels in the input stream. */
  unsigned short                     sample_rate;        /**< Sample rate. */
  uint32                      frame_size;         /**< Number of samples in every channel. */
  t_bool                             interleave;         /**< PCM sample pattern (interleave or linear). */
  t_vs_configuration                 config;             /**< Virtual surround configuration. */
  t_vs_mix_gains                     mix_gains;          /**< User gains. */
  t_vs_input_frame                  *input_frame;        /**< Deinterleaved input frame. */
  t_vs_virtualization_processing    *virtualization;     /**< Virtual processing block. */
  t_vs_room_simulation_processing   *room_simulation;    /**< Room simulation block. */
  t_vs_multichannel_mixer           *mixer;              /**< Multichannel mixer. */
} t_vs_instance;


/**
 * The instantiate function does the allocation in the input frame when interleaving the
 * audio stream.
 *
 * \param  frame    Virtual surround input frame. If the multichannel stream is interleaved
 *                  this struct allocates resources and deinterleaves the stream. If the
 *                  stream is linear, the frame is based on the address for every channel
 *                  encoded in the multichannel stream.
 * \param  config   Virtual surround system configuration.
 * \retval TRUE     Allocation was successful.
 * \retval FALSE    Out of memory.
 */
t_bool frame_allocate(t_vs_input_frame **frame, const t_vs_allocation_params* const config);


/**
 * This function destroys the input frame buffer, that is, de-allocates and clears the buffer
 * to avoid accidental reuse.
 *
 * \param frame   Virtual surround input frame. 
 */
void frame_destroy(t_vs_input_frame **frame);


/**
 * This function configures the input frame used in the algorithm.\n
 * It sets offsets in the frame depending on the channel order, i.e, when samples are interleaved
 * or linear.
 *
 * \param frame            Virtual surround input frame.
 * \param channel_mapping  Audio channel configuration (PCM channel distribution in the frame).
 */
void frame_config(t_vs_input_frame *frame, const t_audio_channel_type channel_mapping[]);


/**
 * Frame deinterleaved is used before executing the algorithm. This function deinterleaves
 * the multichannel stream writting data in the internal frame.
 *
 * \param frame       Internal frame.
 * \param input       Multichannel stream (non/interleaved).
 * \param size        Number of samples every channel.
 * \param interleave  Stream is interleave or noninterleave.
 */
void frame_deinterleave(t_vs_input_frame *frame, const signed short* const input, const uint32 size, const t_bool interleave);


/**
 * The instantiate function does a minimal allocation for the virtualization processing block.
 *
 * \param  virtualization  Virtualization processing instance.
 * \param  nof_samples     Number of samples in every channel.
 * \param  nof_channels    Number of channels encoded in the input stream.
 * \retval TRUE            Allocation was successful.
 * \retval FALSE           Out of memory.
 */
t_bool virtualization_allocate(t_vs_virtualization_processing **virtualization, const uint32 nof_samples, const unsigned short nof_channels);


/**
 * This function destroys the virtual processing block, that is, de-allocates all dynamic variables
 * and clears the instance struct to avoid accidental reuse.
 *
 * \param virtualization  Virtualization Processing instance.
 */
void virtualization_destroy(t_vs_virtualization_processing **virtualization);


/**
 * Reset will restore all internal states to an initial and predefined state in the virtualization
 * processing block.
 *
 * \param virtualization  Virtualization processing instance.
 */
void virtualization_reset(t_vs_virtualization_processing *virtualization);


/**
 * Virtualization processing execution
 * The block will generate the reverberation and virtualizion components used as inputs in the room
 * simulation and mixer.
 *
 * \param instance  Virtualization processing instance.
 * \param frame     Virtual surround internal frame.
 */
void virtualization_execute(t_vs_virtualization_processing *virtualization, t_vs_input_frame *frame);


/**
 * The instantiate function does the allocation in the room simulatioon block.
 *
 * \param  room_simulation  Room simulation processing instance.
 * \param  nof_samples      Number of samples in every channel.
 * \retval TRUE             Allocation was successful.
 * \retval FALSE            Out of memory.
 */
t_bool room_simulation_allocate(t_vs_room_simulation_processing **room_simulation, const uint32 nof_samples);


/**
 * This function destroys the room simulation block, that is, de-allocates all dynamic variables
 * and clears the instance struct to avoid accidental reuse.
 *
 * \param room_simulation  Room simulation instance.
 */
void room_simulation_destroy(t_vs_room_simulation_processing **room_simulation);


/**
 * It will restore internal states to an initial and predefined state in the room simulation
 * block.
 *
 * \param  room_simulation  Room simulation instance.
 */
void room_simulation_reset(t_vs_room_simulation_processing *room_simulation);


/**
 * Room simulation execution
 * The room simulation block will deliver to the mixer the output after applying the reverberation
 * effect based on specific room simulation configuration. It will apply filtering and crosscoupling
 * for the front and sorround paths.
 *
 * \param instance  Virtual Surround instance.
 * \param frame     Internal frame.
 *
 */
void room_simulation_execute(t_vs_instance *instance, const t_vs_input_frame* const frame);


/**
 * The instantiate function does a minimal allocation in the multichannel mixer.
 *
 * \param  mixer  Multichannel mixer instance.
 * \retval TRUE   Allocation was successful.
 * \retval FALSE  Out of memory.
 */
t_bool mixer_allocate(t_vs_multichannel_mixer **mixer);


/**
 * This function destroys the multichannel, de-allocates all dynamic variables and clear the
 * instance struct.
 *
 * \param mixer  Multichannel mixer instance.
 */
void mixer_destroy(t_vs_multichannel_mixer **mixer);


/**
 * Multichannel mixer execution
 * Mix all components to produce either stereo audio signal or a binaural audio signal. The mixing
 * process is according to the rendering mode configured for the current instance. The caller can
 * select different rendering modes during execution time and the mixer will be reconfigured to  
 * process the correspoding audio components from the virtualization and room simulation blocks.
 *
 * \param instance  Virtual Surround instance.
 * \param output    Output signal.
 */
void mixer_execute(t_vs_instance *instance, const t_vs_input_frame* const frame, signed short *output);


#endif /* _virtual_surround_local_h_ */
