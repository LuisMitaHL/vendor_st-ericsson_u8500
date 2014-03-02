/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _BUFFERS_H_
#define _BUFFERS_H_

/*------------------------------------------------------------------------
 * Include
 *----------------------------------------------------------------------*/
#include "t1xhv_hamac_xbus_regs.h" 	/* Include common HAMAC register file       */

/*
 * Defines							       
 */

#define T1XHV_TASK_NUMBER_BUFFER_MGT 2 /**< \brief Number of task with buffer management Decode,encode */
#define BUFFER_EOW_IRQ 12

/**
 * Types
 */
typedef enum
{
	DMA_INIT        = 1, /**< \brief DMA initialization     */
	LINK_UPDATE_EOW = 0, /**< \brief Update buffer with EOW */
	LINK_UPDATE_BOW = 2  /**< \brief Update buffer with BOW */

} t_dma_status;

/** @{ \name Link size status
 *     \warning DO NOT change enum values
 */
typedef enum
{
	SIZE_UPDATE = 0, /**< \brief Update buffer size   */
	SIZE_INIT   = 1  /**< \brief Init buffer size     */
} t_link_status;

/*****************************************************************************/
/** \brief Buffer management DMA registers
 *
 * Must be inline with DMA registers definition
 */
/*****************************************************************************/
typedef struct ts_task_buffer_mgt 
{
	t_uint16 dma_bws_lsb;         /**< \brief Window start lsb   */
	t_uint16 dma_bws_msb;         /**< \brief Window start msb   */
	t_uint16 error_window_start;  /**< \brief Error window start */

	t_uint16 dma_bwe_lsb;         /**< \brief Window end lsb    */
	t_uint16 dma_bwe_msb;         /**< \brief Window end msb    */
	t_uint16 error_window_end;    /**< \brief Error Window end  */

	t_uint16 dma_bca_lsb;         /**< \brief Current address lsb */
	t_uint16 dma_bca_msb;         /**< \brief Current address msb */

	t_uint16 buf_mgt_mode;        /**< \brief Buffer management mode */

	t_ahb_address current_address_buffer_link; /**< \brief Current address buffer for link mode */

} ts_task_buffer_mgt, *tps_task_buffer_mgt;

/*****************************************************************************/
/** \brief Buffer management size for link mode
 *
 * Contains start and end addresses. Size of current buffer
 * and size of all bistream since init
 */
/*****************************************************************************/
typedef struct ts_size_buffer_mgt 
{
	t_uint32 addr_start_current_link;        /**< \brief Start address of the buffer  of the current link  */
	t_uint32 addr_start_previous_link;        /**< \brief Start address of the buffer of the previous link */
	t_uint32 addr_end_current_link;          /**< \brief End address of the buffer          */

	t_uint32 nb_of_bytes_to_current_link;    /**< \brief Bytes number since start to current link    */
	t_uint32 nb_of_bytes_to_previous_link;    /**< \brief Bytes number since start to previous link  */
	t_uint32 nb_of_bytes_in_current_link;    /**< \brief Bytes number in the current buffer */

} ts_size_buffer_mgt, *tps_size_buffer_mgt;

/*
 * Global Variables							       
 */

/** \brief Task management definition (array of task_def, one for each task). */
EXTMEM ts_task_buffer_mgt Gs_task_buffer_mgt_definition[T1XHV_TASK_NUMBER_BUFFER_MGT] = {
	/**< \brief Must be in line with ID_TASK_DECODE*/
	{
		DMA_CDR_BWS_L,
		DMA_CDR_BWS_H,
		VDC_ERT_BAD_ADD_WINDOW_START,
		DMA_CDR_BWE_L,
		DMA_CDR_BWE_H,
		VDC_ERT_BAD_ADD_WINDOW_END,
		0,
		0,
		0,
		0
	},

	/**< \brief Must be in line with ID_TASK_ENCODE*/
	{
		DMA_CDW_BWS_L,
		DMA_CDW_BWS_H,
		ERR_ENCODE_NOT_FOUND,
		DMA_CDW_BWE_L,
		DMA_CDW_BWE_H,
		ERR_ENCODE_NOT_FOUND,
		0,
		0,
		0,
		0
	}
};

/**
 * Macros							       
 */

/*
 * Global Variables							       
 */

#ifdef BUF_MGT_GLOBAL
volatile EXTMEM ts_size_buffer_mgt Gs_task_buffer_size;
#else
extern volatile EXTMEM ts_size_buffer_mgt Gs_task_buffer_size;
#endif

#endif /* _BUFFERS_H_ */
