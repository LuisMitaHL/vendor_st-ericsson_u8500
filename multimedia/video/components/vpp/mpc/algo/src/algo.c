/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <vpp/mpc/algo.nmf>

//common component file
#include "macros.h"

//#include "pxp_api.h"
#include "itc_api.h"
#include "ckg_api.h"
//#include "stplug_api.h"
//#include "dfi_api.h"

#include "vpp_api.h"
#include <t1xhv_vpp.h>

#define VPP_CHECK_PARAMS

#ifndef VPP_IRQ_EOT
	#define VPP_IRQ_EOT 21
#endif

//For printf
#ifdef __DEBUG_TRACE_ENABLE
#include <trace/mpc_trace.h>
#include <trace/mpc_trace.c>
#endif
/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
static t_t1xhv_debug_mode	vppDebugMode;
static t_uint16				vppChannelNumber;
static t_uint32				vppStartTime;

volatile unsigned int vpp_irq_eot_ack;

/*------------------------------------------------------------------------
 * Private functions prototype
 *----------------------------------------------------------------------*/
void notifyEndAlgo (t_uint16 channelNumber, t_t1xhv_status status, t_uint32 info);

#pragma interface
Ivpp_mpc_api_end_algo	algoCb; /* will be a ptr when cb NMF bug solved */

/*------------------------------------------------------------------------
 * Methodes of component interface
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/**
 * \brief  init
 *
 * Init VPP Algo component. Implicitly called while instanciating
 * 	this component.
 * 	Unmask wanted internal interrupts.
 */
/*****************************************************************************/
#pragma force_dcumode
t_nmf_error METH(construct)(void)
{
  vppDebugMode = DBG_MODE_NORMAL;
  return NMF_OK;

} /* End of init() function. */

/*****************************************************************************/
/**
 * \brief  controlAlgo
 *
 * Control VPP Algo component. 
 *
 * \param   sCommand
 * \param   ChannelNumber (i.e. instance number) the algo is linked to
 * \param   param (n/a)
  */
/*****************************************************************************/
#pragma force_dcumode
void METH(controlAlgo)(t_t1xhv_command Command, t_uint16 channelNumber, t_uint16 param)
{
	t_uint16 errorOrInfo = VPP_ERT_NONE;
	t_uint16 dma_read;
	t_uint16 dma_prog;
    volatile t_uint16 EMU_unit_maskit_prev; /*  current value of EMU_unit_maskit */
	
#ifdef __DEBUG_TRACE_ENABLE
	NmfPrint0(2,"VPP Algo: Entered into controlalgo\n");
#endif

	if ((vppDebugMode == DBG_MODE_NO_HW) && (Command == CMD_START))
	{
		notifyEndAlgo (channelNumber, STATUS_JOB_COMPLETE, VPP_ERT_NONE);
		return;
	}
	
	switch (Command)
	{
    case CMD_RESET:
    case CMD_ABORT:
		/* (Not yet implemented) */
		break;
		
    case CMD_START:
		
#ifdef VPP_CHECK_PARAMS	
		if( (Gs_vpp_global_param.vpp_task_type != YUV420RasterPlaner_YUV420MB) &&
			(Gs_vpp_global_param.vpp_task_type != YUV420MB_YUV420MB) &&
			(Gs_vpp_global_param.vpp_task_type != TASK_H263_INLOOP_SW) &&
			(Gs_vpp_global_param.vpp_task_type != TASK_DIVX_POST_FLT_DRG_SW) &&
			(Gs_vpp_global_param.vpp_task_type != TASK_DIVX_POST_FLT_DRG_HW) &&
			(Gs_vpp_global_param.vpp_task_type != TASK_MPEG4_POST_FLT_HW) &&
			(Gs_vpp_global_param.vpp_task_type != TASK_MPEG4_POST_DRG_HW) &&
			(Gs_vpp_global_param.vpp_task_type != TASK_MPEG4_POST_FLT_DRG_HW) &&
			(Gs_vpp_global_param.vpp_task_type != TASK_MPEG4_POST_FLT_DRG_SW))
		{
			errorOrInfo= VPP_ERT_BAD_TASK_TYPE;
		}
		/* addr_src_frame_buffer is not a multiple of 256 */
		if (CHECK_SOFT_ERR(Gs_vpp_global_param.addr_src_frame_buffer,8))
			errorOrInfo= VPP_ERT_BAD_ADD_SRC_BUFFER;
		
		/* addr_dst_frame_buffer is not a multiple of 256 */
		if (CHECK_SOFT_ERR(Gs_vpp_global_param.addr_dst_frame_buffer,8))
			errorOrInfo= VPP_ERT_BAD_ADD_DEST_BUFFER;

		if(Gs_vpp_global_param.vpp_task_type >= 2)
		{
			/* addr_dst_frame_buffer is not a multiple of 256 */
			if (CHECK_SOFT_ERR(Gs_vpp_global_param.addr_deblock_param_buffer,3))
				errorOrInfo= VPP_ERT_BAD_ADD_DBLK_PARAM_BUFFER;
		}
		else if(Gs_vpp_global_param.vpp_task_type == 0)
		{
			/* format_conv_type is not 0 */
			if ((Gs_vpp_global_param.task_type != 0x67) && (Gs_vpp_global_param.task_type != 0x60))
				errorOrInfo= VPP_ERT_BAD_FORMAT_CONV_TYPE;

			/* offset_x is not a multiple of 16 */
			if (CHECK_SOFT_ERR(Gs_vpp_global_param.offset_x,4))
				errorOrInfo= VPP_ERT_BAD_OFFSET_X;
			
			/* offset_y is not a multiple of 16 */
			if (CHECK_SOFT_ERR(Gs_vpp_global_param.offset_y,4))
				errorOrInfo= VPP_ERT_BAD_OFFSET_Y;

			if(Gs_vpp_global_param.task_type == 0x67)
			{
				/* in_width or in_height is not a multiple of 32 */
				if (CHECK_SOFT_ERR(Gs_vpp_global_param.in_width,5) && CHECK_SOFT_ERR(Gs_vpp_global_param.in_height,5))
					errorOrInfo= VPP_ERT_NOT_SUPPORTED;

				/* out_width or out_height is not a multiple of 32 */
				if (CHECK_SOFT_ERR(Gs_vpp_global_param.out_width,5) && CHECK_SOFT_ERR(Gs_vpp_global_param.out_height,5))
					errorOrInfo= VPP_ERT_NOT_SUPPORTED;
			}
		}
	
		/* in_width is not a multiple of 16 */
		if (CHECK_SOFT_ERR(Gs_vpp_global_param.in_width,4))
			errorOrInfo= VPP_ERT_BAD_INPUT_WIDTH;
		
		/* in_height is not a multiple of 16 */
		if (CHECK_SOFT_ERR(Gs_vpp_global_param.in_height,4))
			errorOrInfo= VPP_ERT_BAD_INPUT_HEIGHT;
		
		/* out_width is not a multiple of 16 */
		if (CHECK_SOFT_ERR(Gs_vpp_global_param.out_width,4))
			errorOrInfo= VPP_ERT_BAD_OUTPUT_WIDTH;
		
		/* out_height is not a multiple of 16 */
		if (CHECK_SOFT_ERR(Gs_vpp_global_param.out_height,4))
			errorOrInfo= VPP_ERT_BAD_OUTPUT_HEIGHT;
#endif
#if 1		
		/* start of  critical code section (after the BDU access)*/
		EMU_unit_maskit_prev = EMU_unit_maskit;
		MMDSP_EMU_MASKIT(INTERRUPT_DISABLE);
		
		CKG_ENABLE_CLOCKS_VPP_NMF();
		
		/* end  of critical code section*/
		MMDSP_EMU_MASKIT(EMU_unit_maskit_prev);
#endif
		if (errorOrInfo == 0)
		{
			/* Memorize the current channel number. */
			vppChannelNumber = channelNumber;
			
			/* Get current timer value (for algo duration calculation) */
			vppStartTime = ITC_GET_TIMER_32();
			
			vpp_irq_eot_ack = 0;
			
			iInterrupt.subscribeInterrupt(VPP_IRQ_EOT, &iSignal);

			VPP_START_TASK();
			VPP_ABORT_TASK();
			while(vpp_irq_eot_ack==0);
			
			// programmation of the VPP and DMA

			vpp_irq_eot_ack = 0;
			iInterrupt.subscribeInterrupt(VPP_IRQ_EOT, &iSignal);

#ifdef __DEBUG_TRACE_ENABLE
	NmfPrint0(2,"VPP Algo: Before VPP task start\n");
#endif
			vpp_micro_task(0);

#ifdef __DEBUG_TRACE_ENABLE
			NmfPrint0(2,"VPP Algo: After VPP task and waiting for it's completion\n");
#endif			
			//check VPP task is completed or not before calling notifyEndAlgo
			while(vpp_irq_eot_ack == 0);
			
#ifdef __DEBUG_TRACE_ENABLE
			NmfPrint0(2,"VPP Algo: After VPP task finished\n");
#endif
			/* Stop VPP Hardware */ 
			vpp_stop_hw();
		}
		
#ifdef __DEBUG_TRACE_ENABLE
		NmfPrint0(2,"VPP Algo: exited from controlalgo\n");
#endif

		vpp_irq_eot_ack = 0;
		
		iInterrupt.subscribeInterrupt(VPP_IRQ_EOT, &iSignal);
		
		VPP_START_TASK();
		VPP_ABORT_TASK();
		while(vpp_irq_eot_ack==0);

#if 1
		/* start of  critical code section (after the BDU access)*/
		EMU_unit_maskit_prev = EMU_unit_maskit;
		MMDSP_EMU_MASKIT(INTERRUPT_DISABLE);
		
		CKG_DISABLE_CLOCKS_VPP_NMF();
		
		/* end  of critical code section*/
		MMDSP_EMU_MASKIT(EMU_unit_maskit_prev);
#endif
		notifyEndAlgo (channelNumber,
			errorOrInfo == 0 ? STATUS_JOB_COMPLETE : STATUS_JOB_ABORTED,
			errorOrInfo);
		break;
		
    default:
		break;
	} /* switch (Command) */
} /* end of controlAlgo() function */

/*****************************************************************************/
/**
 * \brief  configureAlgoFormatConv
 *
 * Configure VPP Algo component adresses of all required data
 *  structures
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(configureAlgoFormatConv)(Ivpp_mpc_api_end_algo *cb,
						t_uint32 format_conv_type,	
						t_uint32 addr_src_frame_buffer,
						t_uint32 addr_dst_frame_buffer,
						t_uint16 offset_x,
						t_uint16 offset_y,
						t_uint16 in_width,
						t_uint16 in_height,
						t_uint16 out_width,
						t_uint16 out_height)
{
	algoCb = *cb;
	
	if (vppDebugMode == DBG_MODE_NORMAL)
	{
		switch(format_conv_type)
		{
		case YUV420RasterPlaner_YUV420MB:
				Gs_vpp_global_param.task_type		= 0x67;
				Gs_vpp_global_param.vpp_task_type	= 0;
				Gs_vpp_global_param.output_format	= VPP_420_MB_OUT;
				Gs_vpp_global_param.input_format	= VPP_420_PLANAR_IN;
				break;
		case YUV420MB_YUV420MB:
				Gs_vpp_global_param.task_type		= 0x60;
				Gs_vpp_global_param.vpp_task_type	= 0;
				Gs_vpp_global_param.output_format	= VPP_420_MB_OUT;
				Gs_vpp_global_param.input_format	= VPP_420_MB_IN;
				break;
		default:
				//NmfPrint0(2,"Warning: Unsupported format_conv_type, Default YUV420RasterPlaner_YUV420MB is used\n");
				Gs_vpp_global_param.task_type		= 0x67;
				Gs_vpp_global_param.vpp_task_type	= 0;
				Gs_vpp_global_param.output_format	= VPP_420_MB_OUT;
				Gs_vpp_global_param.input_format	= VPP_420_PLANAR_IN;
				break;
		}

#ifdef __DEBUG_TRACE_ENABLE
		NmfPrint1(2,"VPP Algo: format_conv_type = %x\n", format_conv_type);
		NmfPrint1(2,"VPP Algo: vpp_task_type = %x\n", Gs_vpp_global_param.vpp_task_type);
		NmfPrint1(2,"VPP Algo: input_format = %d\n", Gs_vpp_global_param.input_format);
		NmfPrint1(2,"VPP Algo: output_format = %d\n", Gs_vpp_global_param.output_format);
		NmfPrint2(2,"VPP Algo: addr_src_frame_buffer h= %x, l=0x%x\n",addr_src_frame_buffer>>16, addr_src_frame_buffer & 0xffff);
		NmfPrint2(2,"VPP Algo: addr_dst_frame_buffer h= %x, l=0x%x\n",addr_dst_frame_buffer>>16, addr_dst_frame_buffer & 0xffff);
		NmfPrint1(2,"VPP Algo: offset_x = %d\n",offset_x);
		NmfPrint1(2,"VPP Algo: offset_y = %d\n",offset_y);
		NmfPrint1(2,"VPP Algo: in_width = %d\n",in_width);
		NmfPrint1(2,"VPP Algo: in_height = %d\n",in_height);
		NmfPrint1(2,"VPP Algo: out_width = %d\n",out_width);
		NmfPrint1(2,"VPP Algo: out_height = %d\n",out_height);
#endif

		Gs_vpp_global_param.addr_src_frame_buffer = addr_src_frame_buffer;
		Gs_vpp_global_param.addr_dst_frame_buffer = addr_dst_frame_buffer;
		Gs_vpp_global_param.offset_x = offset_x;
		Gs_vpp_global_param.offset_y = offset_y;
		Gs_vpp_global_param.in_width = in_width;
		Gs_vpp_global_param.in_height = in_height;
		Gs_vpp_global_param.out_width = out_width;
		Gs_vpp_global_param.out_height = out_height;	
	} 
} /* end of configureAlgoFormatConv() function. */

/*****************************************************************************/
/**
 * \brief  configureAlgoDeblock
 *
 * Configure VPP Algo component adresses of all required data
 *  structures
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(configureAlgoDeblock)(Ivpp_mpc_api_end_algo *cb,
								t_uint32 vpp_task_type,	
								t_uint32 addr_src_frame_buffer,
								t_uint32 addr_dst_frame_buffer,
								t_uint32 addr_deblock_param_buffer,
  								t_uint16 width,
								t_uint16 height
)
{
	algoCb = *cb;
	
#ifdef __DEBUG_TRACE_ENABLE
		NmfPrint1(2,"VPP Algo: vpp_task_type = %x\n", vpp_task_type);
#endif

	if (vppDebugMode == DBG_MODE_NORMAL)
	{
		switch(vpp_task_type)
		{
		case TASK_H263_INLOOP_SW:
				Gs_vpp_global_param.task_type		= 0x6C;
				Gs_vpp_global_param.vpp_task_type	= TASK_H263_INLOOP_SW;
				Gs_vpp_global_param.output_format	= VPP_420_PLANAR_OUT;
				Gs_vpp_global_param.input_format	= VPP_420_PLANAR_IN;
				break;
		case TASK_MPEG4_POST_FLT_HW:
				Gs_vpp_global_param.task_type		= 0x62;
				Gs_vpp_global_param.vpp_task_type	= TASK_MPEG4_POST_FLT_HW;
				Gs_vpp_global_param.output_format	= VPP_420_MB_OUT;
				Gs_vpp_global_param.input_format	= VPP_420_MB_IN;
				break;
		case TASK_MPEG4_POST_DRG_HW:
				Gs_vpp_global_param.task_type		= 0x64;
				Gs_vpp_global_param.vpp_task_type	= TASK_MPEG4_POST_DRG_HW;
				Gs_vpp_global_param.output_format	= VPP_420_MB_OUT;
				Gs_vpp_global_param.input_format	= VPP_420_MB_IN;
				break;
		case TASK_MPEG4_POST_FLT_DRG_HW:
				Gs_vpp_global_param.task_type		= 0x63;
				Gs_vpp_global_param.vpp_task_type	= TASK_MPEG4_POST_FLT_DRG_HW;
				Gs_vpp_global_param.output_format	= VPP_420_MB_OUT;
				Gs_vpp_global_param.input_format	= VPP_420_MB_IN;
				break;
		case TASK_MPEG4_POST_FLT_DRG_SW:
				Gs_vpp_global_param.task_type		= 0x6A;
				Gs_vpp_global_param.vpp_task_type	= TASK_MPEG4_POST_FLT_DRG_SW;
				Gs_vpp_global_param.output_format	= VPP_420_PLANAR_OUT;
				Gs_vpp_global_param.input_format	= VPP_420_PLANAR_IN;
				break;
		case TASK_DIVX_POST_FLT_DRG_SW:
				Gs_vpp_global_param.task_type		= 0x6D;
				Gs_vpp_global_param.vpp_task_type	= TASK_DIVX_POST_FLT_DRG_SW;
				Gs_vpp_global_param.output_format	= VPP_420_PLANAR_OUT;
				Gs_vpp_global_param.input_format	= VPP_420_PLANAR_IN;
				break;
		case TASK_DIVX_POST_FLT_DRG_HW:
				Gs_vpp_global_param.task_type		= 0x66;
				Gs_vpp_global_param.vpp_task_type	= TASK_DIVX_POST_FLT_DRG_HW;
				Gs_vpp_global_param.output_format	= VPP_420_MB_OUT;
				Gs_vpp_global_param.input_format	= VPP_420_MB_IN;
				break;
		default:
				NmfPrint0(2,"Warning: Unsupported vpp_task_type, Default MPEG4 Deblocking is used\n");
				Gs_vpp_global_param.task_type		= 0x62;
				Gs_vpp_global_param.vpp_task_type	= TASK_MPEG4_POST_FLT_HW;
				Gs_vpp_global_param.output_format	= VPP_420_MB_OUT;
				Gs_vpp_global_param.input_format	= VPP_420_MB_IN;
				break;
		}

#ifdef __DEBUG_TRACE_ENABLE
		NmfPrint1(2,"VPP Algo: task_type = %x\n", Gs_vpp_global_param.task_type);
		NmfPrint1(2,"VPP Algo: input_format = %d\n", Gs_vpp_global_param.input_format);
		NmfPrint1(2,"VPP Algo: output_format = %d\n", Gs_vpp_global_param.output_format);
		NmfPrint2(2,"VPP Algo: addr_src_frame_buffer h= %x, l=0x%x\n",addr_src_frame_buffer>>16, addr_src_frame_buffer & 0xffff);
		NmfPrint2(2,"VPP Algo: addr_dst_frame_buffer h= %x, l=0x%x\n",addr_dst_frame_buffer>>16, addr_dst_frame_buffer & 0xffff);
		NmfPrint2(2,"VPP Algo: addr_deblock_param_buffer h= %x, l=0x%x\n",addr_deblock_param_buffer>>16, addr_deblock_param_buffer & 0xffff);
		NmfPrint1(2,"VPP Algo: width = %d\n",width);
		NmfPrint1(2,"VPP Algo: height = %d\n",height);
#endif

		Gs_vpp_global_param.addr_src_frame_buffer = addr_src_frame_buffer;
		Gs_vpp_global_param.addr_dst_frame_buffer = addr_dst_frame_buffer;
		Gs_vpp_global_param.addr_deblock_param_buffer = addr_deblock_param_buffer;
		Gs_vpp_global_param.offset_x = 0;
		Gs_vpp_global_param.offset_y = 0;
		Gs_vpp_global_param.in_width = width;
		Gs_vpp_global_param.in_height = height;
		Gs_vpp_global_param.out_width = width;
		Gs_vpp_global_param.out_height = height;	
	} 
} /* end of configureAlgoDeblock() function. */


/*****************************************************************************/
/**
 * \brief  signalBuffer
 *
 * Signal the abnormal completion of a buffer link list process.
 *
 * \param	to be defined.
  */
/*****************************************************************************/
#pragma force_dcumode
void METH(signalBuffer)(t_t1xhv_buffer_status bufferStatus)
{
  notifyEndAlgo (vppChannelNumber, STATUS_BUFFER_NEEDED, 0);
} /* End of signalBuffer() fucntion. */


/*****************************************************************************/
/**
 * \brief  signalInterrupt
 *
 * Signal an interrupt linked to the decode process occured.
 *
 * \param	to be defined.
  */
/*****************************************************************************/
#pragma force_dcumode
void METH(signalInterrupt)(t_uint16 interruptSource)
{
#ifdef __DEBUG_TRACE_ENABLE
	 NmfPrint0(2,"VPP Algo: Entered into signalInterrupt\n");
#endif

	if (interruptSource == VPP_IRQ_EOT) 
	{
		 iInterrupt.unsubscribeInterrupt(VPP_IRQ_EOT);
		 vpp_irq_eot_ack = 1;
	}
} /* End of signalInterrupt() function. */

/*****************************************************************************/
/**
 * \brief  setDebug
 *
 * Set the debug mode of the VPP NMF component
 *
 * \param	mode.
 * \param	param1.
 * \param	param2.
  */
/*****************************************************************************/
#pragma force_dcumode
void METH(setDebug)(t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2)
{
  if ((mode == DBG_MODE_NORMAL) || (mode == DBG_MODE_NO_HW) || (mode == DBG_MODE_PATTERN))
  {
    vppDebugMode = mode;
#ifdef __DEBUG_TRACE_ENABLE
    mpc_trace_init(param1, param2);
#endif
  }
} /* End of setDebug() fucntion. */

/*------------------------------------------------------------------------
 * Internal Functions
 *----------------------------------------------------------------------*/
 /*****************************************************************************/
/**
 * \brief  notifyEndAlgo
 *
 * Private function to call the interface endAlgo according to the channelNumber, i.e.
 *  instance number
 *
  */
/*****************************************************************************/
void notifyEndAlgo (t_uint16 channelNumber, t_t1xhv_status status, t_uint32 info)
{
	t_uint32  currentTimeInTicks;
	t_uint32  durationInTicks;
	
	// Get current time.
	currentTimeInTicks = ITC_GET_TIMER_32();
	durationInTicks = currentTimeInTicks - vppStartTime;
	
#ifdef __DEBUG_TRACE_ENABLE
	NmfPrint2(2,"VPP Algo: Returing to Adapter with status = %d and info = %d\n", status, info);
#endif

	algoCb.endAlgo(status, (t_t1xhv_vpp_info)info, durationInTicks); 
} /* end of notifyEndAlgo() function. */


