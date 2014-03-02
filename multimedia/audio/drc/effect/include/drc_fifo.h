/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   drc_fifo.h
* \brief  Circular Array implementation in the DRC delay block.
* \author ST-Ericsson
* \version v0.0.1
*/
/*****************************************************************************/
#ifndef _drc_fifo_h_
#define _drc_fifo_h_

/** Macro that checks if heap memory was allocationed or not. */
#define CIRCULAR_ARRAY_CHECK_ALLOC_SUCCESS(a)                     if(NULL == a) { goto CIRCULAR_ARRAY_LOCAL_ERROR_HANDLING; }

/** Macro that returns the circular array size in buffers. */
#define get_circular_array_size(circular_array)                   ((circular_array)->array_size)

/** Macro returns the number of stored buffers in the circular array. */
#define get_circular_array_queued_frames(circular_array)          ((circular_array)->size)


/**
 * Circular Array definition
 */
typedef struct
{
   signed short    *circular_array;      /**< Circular array. */
   signed short    *head;                /**< First sample in the array. */
   signed short    *tail;                /**< Last sample in the array. */
   signed short    *read;                /**< Last read sample to apply gain. */
   signed short    *read_level;          /**< Frame dequeued to measure level. */
   signed short    *write;               /**< Last written sample. */
   unsigned short  read_level_offset;    /**< Circular array offset in buffers to start read_level. */
   unsigned short  size;                 /**< Current size. */
   unsigned short  array_size;           /**< Circular array size in buffers [look ahead time / block size]. */
   unsigned long   frame_size;           /**< Block size in samples for every frame queued in the circular array. */
}t_drc_circular_array;


/**
 * Allocates the circular array struct and memory needed by the array such as 'look ahead' buffer.
 * Initializes data members in the circular array struct.
 *
 * \param  array_p        Double pointer to the circular array struct.
 * \param  req_size       Maximum or required number of buffers the circular array should be able to store.
 * \param  residual       Number of samples remaining when block size is not multiple of the delay (look ahead time).
 * \param  block_size     Number of samples per channel.
 * \param  nof_channels   Number of channels in the input stream.
 * \retval TRUE           Allocation was successful.
 * \retval FALSE          Out of memory.
 */
t_bool circular_array_allocate(t_drc_circular_array **array_p, const unsigned short req_size, const unsigned short residual,
                               const unsigned long block_size, const unsigned short nof_channels);


/**
 * Free memory previously allocated by a call \a circular_array_allocate().
 *
 * \param  array_p   Double pointer to a circular array struct.
 */
void circular_array_destroy(t_drc_circular_array **array_p);


/**
 * Resets previous settings from allocation in case the array will be used for next stream.
 *
 * \param  array   Circular array struct pointer.
 */
void circular_array_reset(t_drc_circular_array *array);


/**
 * Set the circular array struct after the 'look ahead' has been changed by the user.
 *
 * \param  array          Circular array struct pointer.
 * \param  req_size       Maximum or required number of buffers the circular array should be able to store.
 * \param  residual       Number of samples remaining when block size is not multiple of the delay (look ahead time).
 * \param  block_size     Number of samples per channel.
 * \param  nof_channels   Number of channels in the input stream.
 */
void circular_array_set_buffer(t_drc_circular_array *array, const unsigned short req_size, const unsigned short residual,
                               const unsigned long block_size, const unsigned short nof_channels);


/**
 * Adds a buffer to the circular array.
 *
 * \param  array   Circular array struct.
 * \param  frame   Object to store in the array.
 */
void queue(t_drc_circular_array *array, signed short *frame);


/**
 * Removes the oldest object in the circular array.
 *
 * \param  array   Circular array struct.
 * \param  frame   Object removed from the array.
 */
void dequeue(t_drc_circular_array *array, signed short **frame);


/**
 * Removes the oldest-Nofromoldest object in the circular array.
 *
 * \param  array   Circular array struct.
 * \param  frame   Object accessed from the array.
 *                 The number of buffers to step back at the start was calculated in the allocation part.
 *                 If that number is set to zeto, the oldest buffer will be accessed.
 */
void dequeue_latest(t_drc_circular_array *array, signed short **frame);


#endif /* _drc_fifo_h_ */
