/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson.
*
* \file   virtual_surround_delayline.h
* \brief  Room simulation delay line interface
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _virtual_surround_delayline_h_
#define _virtual_surround_delayline_h_

#define DELAYLINE_MAXIMUM_DELAY                 1201
#define DELAYLINE_NUMBER_CROSS_PATH_POINTS      5
#define DELAYLINE_NUMBER_DIRECT_PATH_POINTS     8
#define DELAYLINE_CHECK_ALLOC_SUCCESS(a)        if(NULL == a) { goto DELAY_LINE_LOCAL_ERROR_HANDLING; }


/**
 * DelayLineGainsDirectPathLL
 * Gain for each tap in the delay line for input to output
 * (direct path -> left channel).
 */
static const signed short DelayLineGainsDirectPathLL[] =
{
  0x224D, 0xE320, 0x2016, 0x1137, 0x269A, 0xF464, 0x26AB, 0x1CF4
};


/**
 * DelayLineGainsDirectPathRR
 * Gain for each tap in the delay line for input to output
 * (direct path -> right channel).
 */
static const signed short DelayLineGainsDirectPathRR[] =
{
  0x23DA, 0xE347, 0x1450, 0x1D6D, 0x1219, 0xF252, 0x08E8, 0x0B71
};


/**
 * DelayLineGainsCrossPathRL
 * Gain for each tap in the delay line for second input to
 * output (cross path -> right-left channel).
 */
static const signed short DelayLineGainsCrossPathRL[] =
{
  0x1999, 0x264F, 0xF639, 0xF4FE, 0xEC23
};


/**
 * DelayLineGainsCrossPathLR
 * Gain for each tap in the delay line for second input to
 * output (cross path -> left-right channel)
 */
static const signed short DelayLineGainsCrossPathLR[] =
{
  0x1999, 0x2418, 0xE8DF, 0xF37C, 0xF0E3
};


/**
 * DelayLineDirectLL
 * Delay line tapping points for input to output (direct path).
 * All delays are given in samples at 48kHz.
 */
static const unsigned short DelayLineDirectLL[] =
{
  75, 160, 220, 390, 450, 620, 700, 1050
};


/**
 * DelayLineDirectRR
 * Delay line tapping points for input to output (direct path).
 * All delays are given in samples at 48kHz.
 */
static const unsigned short DelayLineDirectRR[] =
{
  75, 160, 225, 384, 458, 618, 720, 1061
};


/**
 * DelayLineCrossRL
 * Tapping points for second input to output (cross path ->
 * right-left channel). All delays are given in samples at
 * 48kHz.
 */
static const unsigned short DelayLineCrossRL[] =
{
  110, 299, 560, 902, 1198
};


/**
 * DelayLineCrossLR
 * Tapping points for second input to output (cross path ->
 * left-right channel). All delays are given in samples at
 * 48kHz.
 */
static const unsigned short DelayLineCrossLR[] =
{
  110, 300, 550, 900, 1201
};


/**
 * get_delay_line_size
 * Returns the delay line size in samples.
 * \param   delay_line     Delay Line struct.
 */
#define get_delay_line_size(delay_line)                    ((delay_line)->size)


/**
 * Delayline and Cross Coupling definition for Virtual Surround
 */
typedef struct
{
          char     type;                                              /**< Delay line type (left or right). */
   signed short    *delay_line;                                       /**< Delay line array. */
   signed short    *head;                                             /**< First sample in the array. */
   signed short    *tail;                                             /**< Last sample in the array. */
   signed short    *write;                                            /**< Last written sample. */
   signed short    *cross[DELAYLINE_NUMBER_CROSS_PATH_POINTS];        /**< Delay line cross path tapping points. */ 
   signed short    *direct[DELAYLINE_NUMBER_DIRECT_PATH_POINTS];      /**< Delay line direct path tapping points. */
   uint32   size;                                              /**< Delay line size in samples. */
   uint32   frame_size;                                        /**< Block size in samples for every frame queued in the delay line. */
}t_vs_delay_line;


/**
 * The function allocates the delay line object and memory required for the 'delay line' buffer.\n
 * Initializes data members in the delay line struct.
 *
 * \param  delay          Delay line struct.
 * \param  block_size     Number of samples per channel.
 * \retval TRUE           Allocation was successful.
 * \retval FALSE          Out of memory.
 */
t_bool delay_line_allocate(t_vs_delay_line **delay_line, const uint32 block_size, const char* const type);


/**
 * Free memory previously allocated by \ref delay_line_allocate().
 *
 * \param  delay  Delay line struct.
 */
void delay_line_destroy(t_vs_delay_line **delay_line);


/**
 * Reset will clean all internal states and set the audio effect in an initial and predefined state
 * in case the delay line is used for the next stream.
 *
 * \param  delay   Delay line struct.
 */
void delay_line_reset(t_vs_delay_line *delay_line);


/**
 * This function queues a data buffer in the delay line.
 *
 * \param  delay   Delay line struct.
 * \param  frame   Object to store in the delay line array.
 */
void delay_line_queue(t_vs_delay_line *delay_line, const signed short* const frame);



/**
 * Queue delivers the corresponding output after applying gains, direct and crosspath delays
 * for front or surround audio.
 *
 * \param  l_delayline  Delay line struct.
 * \param  l_channel    Object removed from the array.
 * \param  r_delayline  Delay line struct.
 * \param  r_channel    Object removed from the array.
 */
void delay_line_dequeue(t_vs_delay_line *l_delayline, signed short *l_channel, t_vs_delay_line *r_delayline, signed short *r_channel);


#endif /* _virtual_surround_delayline_h_ */
