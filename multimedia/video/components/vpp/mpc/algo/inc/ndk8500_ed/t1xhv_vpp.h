/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

 
#ifndef _T1XHV_VPP_H_
#define _T1XHV_VPP_H_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#include "t1xhv_types.h"           /* All t_ types in common_inc        */
//#include "host_interface_common.h"  /* Subtask parameters               */
#include "vpp_api.h"

/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/
#ifdef VPP_GLOBAL
#define EXTERN
#else  /* not VPP_GLOBAL */
#define EXTERN extern
#endif /* not VPP_GLOBAL */

/* output_format */
#define VPP_420_MB_OUT     0
#define VPP_420_PLANAR_OUT 1
#define VPP_422_ITLVD_OUT  2


/* input_format */
#define VPP_420_MB_IN     0
#define VPP_420_PLANAR_IN 1
#define VPP_422_ITLVD_IN  2

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/
/** \brief Exchange struture between Display task and scheduler */
typedef struct t1xhv_vpp_global_param 
{
    t_uint16 dma_prog;            /**< \brief DMA_ENR register programmed value (from display to scheduler )   */
    t_uint16 abort;               /**< \brief Abort flag from scheduler   */
	t_uint32 addr_src_frame_buffer;
	t_uint32 addr_dst_frame_buffer;
	t_uint32 addr_deblock_param_buffer;
	t_uint16 offset_x;
	t_uint16 offset_y;
	t_uint16 in_width;
	t_uint16 in_height;
	t_uint16 out_width;
	t_uint16 out_height;
	t_uint16 task_type;	          
	t_uint16 output_format;	
	t_uint16 input_format;	
	t_task_type vpp_task_type;	
} ts_t1xhv_vpp_global_param, *tps_t1xhv_vpp_global_param;

/*------------------------------------------------------------------------
 * Functions Declarations							       
 *----------------------------------------------------------------------*/
/** @{ \name PUBLIC FUNCTIONS  */
/*---------------------------- */
t_uint16  vpp_micro_task(t_uint16 d);
void      vpp_stop_hw(void);


/** @} end of public functions*/

/*------------------------------------------------------------------------
 * Variables
 *----------------------------------------------------------------------*/

 /** Parameters exchanged between scheduler and display task   */
EXTERN ts_t1xhv_vpp_global_param     Gs_vpp_global_param;

#undef EXTERN

#ifndef __DEBUG_TRACE_ENABLE
	#define printf
	#define PRINT_VAR(x)
#endif


#endif /* _T1XHV_DPL_H_ */


