/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <vpp/mpc/algo.nmf>
//Defines							       
#define VPP_GLOBAL             /* used in t1xhv_vpp.h for EXTERN              */
//#define DEBLOCKING
//Includes
#include "t1xhv_vpp.h"         /* .h for this file                            */
//#include "t1xhv_hamac_xbus_regs.h"
#include "macros.h" /* to use intrinsics */
#include "dma_api.h"
#include "vpp_api.h"
#include "ckg_api.h"

//For printf
#ifdef __DEBUG_TRACE_ENABLE
#include <trace/mpc_trace.h>
#endif

/*****************************************************************************/
/**
 * \brief  programmation of the VPP and DMA
 * \author Pankaj Bansal
 * \return Status \n
 *           1 = correct\n
 *           0 = problem\n
 **/
/*****************************************************************************/
t_uint16 vpp_micro_task(t_uint16 d)
{
	
	t_vpp_deb_filter		deblocking_enable = VPP_DEBLOCKING_DISABLE;
	t_vpp_der_filter		deringing_enable  = VPP_DERINGING_DISABLE; 
	t_task_type			    task_type         = TASK_CONV_FROM_420;
	t_vpp_luma_out_fmt		luma_out_fmt      = VPP_LOF_COLUMN;
	t_vpp_chroma_out_fmt	chroma_out_fmt    = VPP_COF_MB;
	t_vpp_luma_in_fmt		luma_in_fmt		  = VPP_LIF_ROW;
	t_vpp_chroma_in_fmt		chroma_in_fmt     = VPP_CIF_RASTER;
	t_vpp_itlv_lc_out		itlvd_lc_out      = VPP_NO_ITLV_LC_OUT;
	t_vpp_chroma_ups		chroma_ups        = VPP_CHROMA_UPS_OFF;
	t_uint16				dma_swap_reg;
	t_uint16				dma_swap_mask;
	t_uint16				tmp;
	t_uint32				addr_source_buffer;       
	t_uint32				chroma_start_address;    
	t_uint32				chroma2_start_address;    
	t_uint32				deblocking_param_start_address; 
	t_uint32				deb_size=0;
	t_uint32				tmp_address_Y, tmp_address_Cb,tmp_address_Cr;
	
#ifdef __ndk8500_a0__ // currently default values OFF are set. 
	t_vpp_prefetch_line  prefetch_line = VPP_PFR_OFF;
	t_vpp_monochrome     monochrome    = VPP_MCR_OFF;
#endif //#ifdef __ndk8500_a0__
	
#ifdef __DEBUG_TRACE_ENABLE
	NmfPrint0(2,"VPP Algo: Entered into vpp_micro_task\n");
#endif

	/** Abort */
	if (Gs_vpp_global_param.abort == 1)
		return(1);
	
	//CKG_ENABLE_CLOCKS_DPL();
	
	VPP_RESET();
	
#if 0
	/* DEB_L_R */
	DMA_SET_REG(DMA_MAX_OPC_SZ4,6);
	DMA_SET_REG(DMA_MAX_MSSG_SZ4,1);
	DMA_SET_REG(DMA_MAX_CHCK_SZ4,0);
	/* DEB_C_R */
	DMA_SET_REG(DMA_MAX_OPC_SZ5,6);
	DMA_SET_REG(DMA_MAX_MSSG_SZ5,0);
	DMA_SET_REG(DMA_MAX_CHCK_SZ5,0);
	/* PARAM_R */
	DMA_SET_REG(DMA_MAX_OPC_SZ6,5);
	DMA_SET_REG(DMA_MAX_MSSG_SZ6,0);
	DMA_SET_REG(DMA_MAX_CHCK_SZ6,0);
#endif	

	/**
	* task type:
	*
	*            === INPUT FMT MacroBlock ===
	*
	*                  0x60 = format conversion from 420  
	*                  0x61 = format conversion from 422
	*                  0x62 = MPEG4 deblocking filter
	*                  0x63 = MPEG4 deblocking and MPEG4 deringing filters
	*                  0x64 = MPEG4 deringing filter
	*                  0x65 = DivX6 deblocking filter
	*                  0x66 = DivX6 deblocking and MPEG4 deringing filters
	*           
	*            === INPUT FMT RASTER ===
	*
	*                  0x67 = format conversion from 420  
	*                  0x68 = format conversion from 422
	*                  0x69 = MPEG4 deblocking filter
	*                  0x6a = MPEG4 deblocking and MPEG4 deringing filters
	*                  0x6b = MPEG4 deringing filter
	*                  0x6c = DivX6 deblocking filter
	*                  0x6d = DivX6 deblocking and MPEG4 deringing filters
	* 
	*/ 
	if(Gs_vpp_global_param.vpp_task_type > 1)
	{
		dma_swap_mask    = PARAM_R | DEB_L_R | DEB_C_R;  /* need to swap input for both raster and MB */
	}
	else
	{
		dma_swap_mask    = DEB_L_R | DEB_C_R;  /* need to swap input for both raster and MB */
	}
		
	switch (Gs_vpp_global_param.task_type) 
	{
	case 0x60:
		luma_in_fmt      = VPP_LIF_COLUMN;
		chroma_in_fmt    = VPP_CIF_MB;
	case 0x67: 
		task_type         = TASK_CONV_FROM_420;  
		deblocking_enable = VPP_DEBLOCKING_DISABLE;
		deringing_enable  = VPP_DERINGING_DISABLE;
		break;
	case 0x61:             
		luma_in_fmt      = VPP_LIF_COLUMN;
		chroma_in_fmt    = VPP_CIF_MB;
	case 0x68:       
		task_type         = TASK_CONV_FROM_422;  
		deblocking_enable = VPP_DEBLOCKING_DISABLE;
		deringing_enable  = VPP_DERINGING_DISABLE;
		break;
	case 0x62:
		luma_in_fmt      = VPP_LIF_COLUMN;
		chroma_in_fmt    = VPP_CIF_MB;
	case 0x69:                    
		task_type         = 5;  
		deblocking_enable = VPP_DEBLOCKING_ENABLE;
		deringing_enable  = VPP_DERINGING_DISABLE;
		break;
	case 0x63:
		luma_in_fmt      = VPP_LIF_COLUMN;
		chroma_in_fmt    = VPP_CIF_MB;
	case 0x6a:                    
		task_type         = 5;  
		deblocking_enable = VPP_DEBLOCKING_ENABLE;
		deringing_enable  = VPP_DERINGING_ENABLE;
		break;
	case 0x64:
		luma_in_fmt      = VPP_LIF_COLUMN;
		chroma_in_fmt    = VPP_CIF_MB;
	case 0x6b:                    
		task_type         = 5;  
		deblocking_enable = VPP_DEBLOCKING_DISABLE;
		deringing_enable  = VPP_DERINGING_ENABLE;
		break;
	case 0x65:
		luma_in_fmt      = VPP_LIF_COLUMN;
		chroma_in_fmt    = VPP_CIF_MB;
	case 0x6c:                    
		task_type         = 2;  
		deblocking_enable = VPP_DEBLOCKING_ENABLE;
		deringing_enable  = VPP_DERINGING_DISABLE;
		break;
	case 0x66:
		luma_in_fmt      = VPP_LIF_COLUMN;
		chroma_in_fmt    = VPP_CIF_MB;
	case 0x6d:                    
		task_type         = 6;  
		deblocking_enable = VPP_DEBLOCKING_ENABLE;
		deringing_enable  = VPP_DERINGING_ENABLE;
		break;
	default:
		break;
	} /* switch */
	
	/*********************************************************************/
	/* information from Gs_vpp_global_param.output_format                    */
	/*  420 MB output    : available for all task types except 422 input */
	/*  420 raster output: available for all task types except 422 input */
	/*  422 itlvd  output: available for all task types                  */
	/*********************************************************************/
	
	dma_swap_mask  |= DEB_L_W | DEB_C_W;
	
	
	switch(Gs_vpp_global_param.output_format)
	{
		
	case VPP_420_MB_OUT:
		luma_out_fmt   = VPP_LOF_COLUMN;
		chroma_out_fmt = VPP_COF_MB;
		itlvd_lc_out   = VPP_NO_ITLV_LC_OUT;
		chroma_ups     = VPP_CHROMA_UPS_OFF;
		/* not allowed */
		if(task_type == TASK_CONV_FROM_422)
			return (0);
		break;
		
	case VPP_420_PLANAR_OUT:
		luma_out_fmt   = VPP_LOF_ROW;
		chroma_out_fmt = VPP_COF_RASTER;
		itlvd_lc_out   = VPP_NO_ITLV_LC_OUT;
		chroma_ups     = VPP_CHROMA_UPS_OFF;
		/* not allowed */
		if(task_type == TASK_CONV_FROM_422)
			return (0);
		break;
		
	case VPP_422_ITLVD_OUT:
		luma_out_fmt   = VPP_LOF_ROW;
		chroma_out_fmt = VPP_COF_MB;
		itlvd_lc_out   = VPP_ITLV_LC_OUT;
		if (task_type == TASK_CONV_FROM_420) {
			chroma_ups     = VPP_CHROMA_UPS_ON;
		} else {
			chroma_ups     = VPP_CHROMA_UPS_OFF;
		}
		break;
	default:
		return (0);
		break;
	}
	
#if 1
	if(((task_type == TASK_CONV_FROM_420) || (task_type == TASK_CONV_FROM_422))
		&&(Gs_vpp_global_param.output_format != VPP_422_ITLVD_OUT))
	{
		/* mode "bypass" */
		/* DEB_L_W */
		DMA_SET_REG(DMA_MAX_OPC_SZ11,6);
		DMA_SET_REG(DMA_MAX_MSSG_SZ11,0);
		DMA_SET_REG(DMA_MAX_CHCK_SZ11,0);	  
	}
	else
	{ /* interleaved or MPEG4 */
		/* DEB_L_W */
		DMA_SET_REG(DMA_MAX_OPC_SZ11,4);
		DMA_SET_REG(DMA_MAX_MSSG_SZ11,0);
		DMA_SET_REG(DMA_MAX_CHCK_SZ11,0);
	}
	
	if((task_type == TASK_CONV_FROM_420) || 
		(task_type == TASK_CONV_FROM_422) || 
		(Gs_vpp_global_param.output_format == VPP_422_ITLVD_OUT))
	{
		/* mode "bypass" or interleaved */
		/* DEB_C_W */
		DMA_SET_REG(DMA_MAX_OPC_SZ12,5);
		DMA_SET_REG(DMA_MAX_MSSG_SZ12,0);
		DMA_SET_REG(DMA_MAX_CHCK_SZ12,0);	  
	}
	else
	{ /* MPEG4 */
		/* DEB_C_W */
		DMA_SET_REG(DMA_MAX_OPC_SZ12,4);
		DMA_SET_REG(DMA_MAX_MSSG_SZ12,0);
		DMA_SET_REG(DMA_MAX_CHCK_SZ12,0);
	}
#endif

	/*----------------------------------------------------------------------------
	----------------------------- DMA PROGRAMMING---------------------------------
	----------------------------------------------------------------------------*/
	
	dma_swap_reg = DMA_GET_REG(DMA_BSM);
	tmp = ~(DEB_L_W | DEB_C_W | DEB_L_R | DEB_C_R);
	dma_swap_reg &= tmp; /* reset VPP swap bits */ 
	dma_swap_reg |= dma_swap_mask;
	DMA_SET_REG(DMA_BSM,dma_swap_reg);
	
	/*----------------------------------------------------------------------------
	Source frame dma programming 
	----------------------------------------------------------------------------*/	
	if(Gs_vpp_global_param.input_format == VPP_420_PLANAR_IN)
	{
		tmp_address_Y = CONVERT_ENDIANNESS(Gs_vpp_global_param.addr_src_frame_buffer);
		tmp_address_Cb   = tmp_address_Y  + (t_uint32) ((t_uint32) Gs_vpp_global_param.in_width * (t_uint32) Gs_vpp_global_param.in_height); 
		tmp_address_Cr   = tmp_address_Cb + (t_uint32) ((t_uint32)(Gs_vpp_global_param.in_width>>1) * (t_uint32)(Gs_vpp_global_param.in_height>>1)); 
#ifdef __DEBUG_TRACE_ENABLE
		NmfPrint2(2,"VPP Algo: Source Base Address: Y h= 0x%x, l = 0x%x\n", tmp_address_Y>>16, tmp_address_Y & 0xffff);
		NmfPrint2(2,"VPP Algo: Source Base Address: Cb h= 0x%x, l = 0x%x\n", tmp_address_Cb>>16, tmp_address_Cb & 0xffff);
		NmfPrint2(2,"VPP Algo: Source Base Address: Cr h= 0x%x, l = 0x%x\n", tmp_address_Cr>>16, tmp_address_Cr & 0xffff);
#endif
		addr_source_buffer = (tmp_address_Y + (t_uint32)((t_uint32)Gs_vpp_global_param.offset_y * (t_uint32)Gs_vpp_global_param.in_width ) + Gs_vpp_global_param.offset_x);

		if(Gs_vpp_global_param.vpp_task_type > 1)
		{
			deblocking_param_start_address = CONVERT_ENDIANNESS(Gs_vpp_global_param.addr_deblock_param_buffer);
		}
		
		chroma_start_address = (tmp_address_Cb + ((t_uint32)((t_uint32)Gs_vpp_global_param.offset_y>>1) * ((t_uint32)Gs_vpp_global_param.in_width>>1) ) + (Gs_vpp_global_param.offset_x>>1));

		chroma2_start_address  = (tmp_address_Cr + ((t_uint32)((t_uint32)Gs_vpp_global_param.offset_y>>1) * ((t_uint32)Gs_vpp_global_param.in_width>>1) ) + (Gs_vpp_global_param.offset_x>>1));
	}
	else  //needs to be modified as per the in/out format
	{
		addr_source_buffer = CONVERT_ENDIANNESS(Gs_vpp_global_param.addr_src_frame_buffer);

		if(Gs_vpp_global_param.vpp_task_type > 1)
		{
			deblocking_param_start_address = CONVERT_ENDIANNESS(Gs_vpp_global_param.addr_deblock_param_buffer);
		}
		
		chroma_start_address   = addr_source_buffer + (t_uint32) ((t_uint32) Gs_vpp_global_param.in_width * (t_uint32) Gs_vpp_global_param.in_height); 
		
		if(task_type == TASK_CONV_FROM_422)
		{
			chroma2_start_address  =  chroma_start_address + 
				(t_uint32) ((t_uint32)(Gs_vpp_global_param.in_width>>1) * (t_uint32)Gs_vpp_global_param.in_height);
		}
		else
		{
			chroma2_start_address  =  chroma_start_address + 
				(t_uint32) ((t_uint32)(Gs_vpp_global_param.in_width>>1) * (t_uint32)(Gs_vpp_global_param.in_height>>1));
		}
	}
	
	/* align to 128 */
	chroma2_start_address = chroma2_start_address + 127;
	chroma2_start_address = chroma2_start_address & 0xFFFFFF80UL;

#ifdef __DEBUG_TRACE_ENABLE
	NmfPrint2(2,"VPP Algo: addr_source_buffer h= 0x%x, l = 0x%x\n", addr_source_buffer>>16, addr_source_buffer & 0xffff);
	NmfPrint2(2,"VPP Algo: chroma_start_address h= 0x%x, l = 0x%x\n", chroma_start_address>>16, chroma_start_address & 0xffff);
	NmfPrint2(2,"VPP Algo: chroma2_start_address h= 0x%x, l = 0x%x\n", chroma2_start_address>>16, chroma2_start_address & 0xffff);
#endif

	DMA_VPP_SET_SOURCE_FRAME_ADDR(addr_source_buffer, chroma_start_address,	chroma2_start_address);
	
	if(Gs_vpp_global_param.output_format == VPP_420_MB_OUT)
	{
		/* frame size for both in and out */
		DMA_VPP_SET_FRAME_SIZE(Gs_vpp_global_param.out_width, Gs_vpp_global_param.out_height);
	}
	else  //needs to be modified as per the in/out format
	{
		/* frame size for both in and out */
		DMA_VPP_SET_FRAME_SIZE(Gs_vpp_global_param.in_width, Gs_vpp_global_param.in_height);
	}

	if(Gs_vpp_global_param.vpp_task_type > 1)
	{	
		deb_size = (Gs_vpp_global_param.in_width>>4)*(Gs_vpp_global_param.in_height>>4)*8;
		DMA_VPP_SET_DEBLOCKING_PARAM_ADDR(deblocking_param_start_address,deb_size);

#ifdef __DEBUG_TRACE_ENABLE
		NmfPrint2(2,"VPP Algo: deblocking_param_start_address l= 0x%x, h= 0x%x \n",
			 deblocking_param_start_address>>16, deblocking_param_start_address & 0xffff);
		NmfPrint2(2,"VPP Algo: Ddeb_size h= %d l=%d\n",deb_size>>16, deb_size & 0xffff);
#endif
	}
	
	/*----------------------------------------------------------------------------
	Dest frame dma programming 
	----------------------------------------------------------------------------*/
	addr_source_buffer = CONVERT_ENDIANNESS(Gs_vpp_global_param.addr_dst_frame_buffer);
	
	chroma_start_address   = addr_source_buffer + 
		(t_uint32) ((t_uint32) Gs_vpp_global_param.out_width * 
		(t_uint32) Gs_vpp_global_param.out_height); 
	
	chroma2_start_address  =  chroma_start_address + 
		(t_uint32) ((t_uint32)(Gs_vpp_global_param.out_width>>1) * 
		(t_uint32)(Gs_vpp_global_param.out_height>>1));
	
	/* align to 128 */
	chroma2_start_address = chroma2_start_address + 127;
	chroma2_start_address = chroma2_start_address & 0xFFFFFF80UL;

#ifdef __DEBUG_TRACE_ENABLE
	NmfPrint2(2,"VPP Algo: Dst : addr_dst_buffer h= 0x%x, l = 0x%x\n", addr_source_buffer>>16, addr_source_buffer & 0xffff);
	NmfPrint2(2,"VPP Algo: chroma_start_address h= 0x%x, l = 0x%x\n", chroma_start_address>>16, chroma_start_address & 0xffff);
	NmfPrint2(2,"VPP Algo: chroma2_start_address h= 0x%x, l = 0x%x\n", chroma2_start_address>>16, chroma2_start_address & 0xffff);
#endif

	DMA_VPP_SET_DEST_FRAME_ADDR(addr_source_buffer,	chroma_start_address, chroma2_start_address);
	
	/*----------------------------------------------------------------------------
		Raster in/out dma prog 
	----------------------------------------------------------------------------*/
	if(chroma_in_fmt == VPP_CIF_RASTER)
	{
#ifdef __DEBUG_TRACE_ENABLE
		NmfPrint0(2,"VPP Algo: DMA_DEB_RASTER_IN = 1\n");
#endif
		DMA_SET_REG(DMA_DEB_RASTER_IN,1);
	}
	else
	{
#ifdef __DEBUG_TRACE_ENABLE
		NmfPrint0(2,"VPP Algo: DMA_DEB_RASTER_IN = 0\n");
#endif
		DMA_SET_REG(DMA_DEB_RASTER_IN,0);	  
	}
	
	if(Gs_vpp_global_param.output_format == VPP_420_MB_OUT)
	{
#ifdef __DEBUG_TRACE_ENABLE
		NmfPrint0(2,"VPP Algo: DMA_DEB_RASTER_OUT = 0\n");
#endif
		DMA_SET_REG(DMA_DEB_RASTER_OUT,0);	  
	}
	else
	{
#ifdef __DEBUG_TRACE_ENABLE
		NmfPrint0(2,"VPP Algo: DMA_DEB_RASTER_OUT = 1\n");
#endif
		DMA_SET_REG(DMA_DEB_RASTER_OUT,1);	  
	}
		
	/*----------------------------------------------------------------------------
	----------------------------- VPP PROGRAMMING---------------------------------
	----------------------------------------------------------------------------*/
#ifdef __DEBUG_TRACE_ENABLE
	NmfPrint0(2,"VPP Algo: VPP PROGRAMMING\n");
	NmfPrint1(2,"VPP Algo: task_type = %d\n", task_type);
	NmfPrint1(2,"VPP Algo: luma_in_fmt = %d\n", luma_in_fmt);
	NmfPrint1(2,"VPP Algo: chroma_in_fmt = %d\n", chroma_in_fmt);
	NmfPrint1(2,"VPP Algo: deblocking_enable = %d\n", deblocking_enable);
	NmfPrint1(2,"VPP Algo: deringing_enable = %d\n", deringing_enable);
	NmfPrint1(2,"VPP Algo: luma_out_fmt = %d\n", luma_out_fmt);
	NmfPrint1(2,"VPP Algo: chroma_out_fmt = %d\n", chroma_out_fmt);
	NmfPrint1(2,"VPP Algo: itlvd_lc_out = %d\n", itlvd_lc_out);
	NmfPrint1(2,"VPP Algo: chroma_ups = %d\n", chroma_ups);
#endif

#ifndef __ndk8500_a0__
	VPP_CONFIGURE(  task_type,
					luma_in_fmt,
					chroma_in_fmt,
					deblocking_enable,
					VPP_DEB_HOR_THEN_VERT,
					deringing_enable,
					luma_out_fmt,
					chroma_out_fmt,
					itlvd_lc_out,
					chroma_ups);
#else
	VPP_CONFIGURE(  task_type,
					luma_in_fmt,
					chroma_in_fmt,
					deblocking_enable,
					VPP_DEB_HOR_THEN_VERT,
					deringing_enable,
					luma_out_fmt,
					chroma_out_fmt,
					itlvd_lc_out,
					chroma_ups, 
					prefetch_line, 
					monochrome);
#endif //#ifndef __ndk8500_a0__
	
	if(Gs_vpp_global_param.output_format == VPP_420_MB_OUT)
	{
#ifdef __DEBUG_TRACE_ENABLE
		NmfPrint0(2,"VPP Algo: VPP_SET_SOURCE_FRAME_SIZE\n");
		NmfPrint1(2,"VPP Algo: out_width = %d\n", Gs_vpp_global_param.out_width);
		NmfPrint1(2,"VPP Algo: out_height = %d\n", Gs_vpp_global_param.out_height);
#endif
		VPP_SET_SOURCE_FRAME_SIZE(Gs_vpp_global_param.out_width, Gs_vpp_global_param.out_height);
		/* This actually disable freeze */
		VPP_SET_FREEZE_OFFSET(0,Gs_vpp_global_param.out_height);
		/* to avoid VPP_SYNC IT */
		VPP_SET_SYNC_OFFSET(0,Gs_vpp_global_param.out_height);
	}
	else  //needs to be modified as per the in/out format
	{
#ifdef __DEBUG_TRACE_ENABLE
		NmfPrint0(2,"VPP Algo: VPP_SET_SOURCE_FRAME_SIZE\n");
		NmfPrint1(2,"VPP Algo: in_width = %d\n", Gs_vpp_global_param.in_width);
		NmfPrint1(2,"VPP Algo: in_height = %d\n", Gs_vpp_global_param.in_height);
#endif
		VPP_SET_SOURCE_FRAME_SIZE(Gs_vpp_global_param.in_width,	Gs_vpp_global_param.in_height);
		/* This actually disable freeze */
		VPP_SET_FREEZE_OFFSET(0,Gs_vpp_global_param.in_height);
		/* to avoid VPP_SYNC IT */
		VPP_SET_SYNC_OFFSET(0,Gs_vpp_global_param.in_height);
	}
	
	/*----------------------------------------------------------------------------
	------------------------------ START HARDWARE---------------------------------
	----------------------------------------------------------------------------*/
	
    /* Write Process enable and interrupt */
    if(deblocking_enable || deringing_enable )
    {
        Gs_vpp_global_param.dma_prog =  PARAM_R | DEB_C_R | DEB_L_R | DEB_L_W | DEB_C_W ;
    } 
    else 
    {
        Gs_vpp_global_param.dma_prog =  DEB_C_R | DEB_L_R | DEB_L_W | DEB_C_W ;
    } 
	
    tmp = DMA_GET_REG(DMA_ENR);
	tmp |= Gs_vpp_global_param.dma_prog;
    DMA_SET_REG(DMA_ENR,tmp);
	
	/* DMA ITs for VPP are masked */
    tmp = DMA_GET_REG(DMA_IMR);

	if(Gs_vpp_global_param.vpp_task_type > 1)
	{
		tmp &= ~(PARAM_R | DEB_C_R | DEB_L_R | DEB_L_W | DEB_C_W);  /* reset mask bits for VPP procs */
	}
	else
	{
		tmp &= ~(DEB_C_R | DEB_L_R | DEB_L_W | DEB_C_W);  /* reset mask bits for VPP procs */
	}

	/*tmp |= Gs_vpp_global_param.dma_prog;*/
    DMA_SET_REG(DMA_IMR,tmp);
	
    //start VPP task
    VPP_START_TASK();
    
#ifdef __DEBUG_TRACE_ENABLE
	NmfPrint0(2,"VPP Algo: Exited from vpp_micro_task\n");
#endif
	return(1);
	
} /* vpp_micro_task */


/*****************************************************************************/
/**
 * \brief 	Stop DMA and VPP for display action
 * \author 	Pankaj Bansal
 */
/*****************************************************************************/
void vpp_stop_hw(void)
{
    t_uint16 dma_word_r;
    t_uint16 dma_word_w;

	/* The ITs programmed for this task */
    dma_word_w = Gs_vpp_global_param.dma_prog;

    /* Abort RAW or Chroma/Luma */
#ifndef __ndk8500_a0__
   dma_stop_wait_process(DMA_DIR,
                          DMA_ENR,
                          dma_word_w,
                          dma_word_w);
#else			  
   dma_stop_wait_process(DMA_DIR_L,
                          DMA_DIR_H,
                          DMA_ENR_L,
                          DMA_ENR_H,
                          dma_word_w,
                          dma_word_w);
#endif //__ndk8500_a0__
    
    /* The current set ITs */
    dma_word_r = DMA_GET_REG(DMA_IMR);
    
    /* The comparison of globaly set and task set */
    dma_word_r &=  ~(dma_word_w);

    /* The reset of the task relevant ones */
    DMA_SET_REG(DMA_IMR,dma_word_r);

    /* Soft VPP reset */
	VPP_RESET();

	//CKG_DISABLE_CLOCKS_DPL();
}

/* END of t1xhv_vpp.c */
