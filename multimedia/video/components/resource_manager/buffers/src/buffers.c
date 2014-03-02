/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdlib.h>

/* Include automatically built nmf file */
#include "resource_manager/buffers.nmf"

/* Include global "shared w/ host" types and definitions */
#include <t1xhv_common.idt>
#include <t1xhv_vec.idt>
#include <t1xhv_vdc.idt>

#define BUF_MGT_GLOBAL
#include "buffers.h"

#include "macros.h"
#include "dma_api.h"

//#include <trace/mpc_trace.h>
//#include <trace/mpc_trace.c>

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
t_codec_type current_codec_type;
#pragma interface
Iresource_manager_api_signal current_codec_cb;

/*------------------------------------------------------------------------
 * Private functions prototype
 *----------------------------------------------------------------------*/
void dma_link_bbm_programming(
		tps_t1xhv_bitstream_buf_link ps_bitstream_buf_link,
		tps_t1xhv_bitstream_buf      ps_bitstream_buf);

void dma_size_bbm (
		t_uint32      start_size_address,
		t_uint32      end_size_address,
		t_link_status size_type);

t_uint16 dma_circular_bbm_programming(tps_t1xhv_bitstream_buf ps_bitstream_buf);

void signalEndOfLinkList(t_t1xhv_buffer_status buffer_status);

#pragma force_dcumode
void METH(freeResource)(t_t1xhv_resource resource, 
						Iresource_manager_api_inform_resource_status *cb)
{
}

void METH(allocResource)(
		t_t1xhv_resource resource, 
		Iresource_manager_api_inform_resource_status *cb)
{
}

void METH(setDebug)(t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2)
{
    //mpc_trace_init(param1, param2);
} /* End of setDebug() function. */

#pragma force_dcumode
void METH(setTimeout)(t_t1xhv_resource                               resource, 
                      Iresource_manager_api_inform_resource_status   *cb, 
                      t_uint32                                       timeout){}

#pragma force_dcumode
void METH(cancelTimeout)(t_t1xhv_resource                               resource){}



/*------------------------------------------------------------------------
 * Methodes of component interface
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/**
 * \brief	setBufferLink
 * 
 * Set dma transfers for either decode, either encode process and launch it
 * Only link behavior is supported. The link structure has to be complete
 * before the launch of the decode/encode process.  
 * 
 * \brief 	Program DMA for link list buffer mode
 * \param	codec_type (ID_DECODER or ID_ENCODER) 
 * \param	p_bitstream_buf_pos pointer on bitstream buffer position data. It'll
 * 			be mapped on a tps_t1xhv_bitstream_buf_link data structure 
 * \param	p_bitstream_buf_link pointer on bitstream buffer link data. It'll
 * 			be mapped on a tps_t1xhv_bitstream_buf_pos data structure
 * \param	cb call back pointer
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(setBufferLink)(
		t_codec_type codec_type, 
		void * ps_bitstream_buf_pos, 
		void* ps_bitstream_buf_link, 
		Iresource_manager_api_signal *cb)
{
	t_uint32				current_address;				/* For start address calculation  */
	t_uint32				address_to_read;				/* Address for read parameters    */
	t_uint16				sizeof_to_read =0;				/* Size of structure to read      */
	t_uint16				*p_data_to_assign= NULL;		/* Pointer on structure to assign */
	ts_t1xhv_bitstream_buf 	s_bitstream_buf;				/* Structure to program dma 	  */
	tps_task_buffer_mgt		ps_task_def_buffer_mgt;			/* Pointer on task buffer management defined by task_id. */
	t_uint32                start_address;
	t_uint32                end_address;					/* end address of the current link or window */

	current_codec_type = codec_type;
	current_codec_cb = *cb;

	//-printf("\n\nRM:Buffer:: Entered into Func setBufferLink\n");

	iInterrupt.subscribeInterrupt(BUFFER_EOW_IRQ, &iSignal);
	
	/* Assign pointer on global DMA registers definition */
	ps_task_def_buffer_mgt = &Gs_task_buffer_mgt_definition[
		current_codec_type==ID_DECODER ? 0 : 1];

	/* Common data for link buffer */
	sizeof_to_read = sizeof(ts_t1xhv_bitstream_buf_link);
	p_data_to_assign = (t_uint16*)ps_bitstream_buf_link;

	address_to_read = CONVERT_ENDIANNESS(
			((tps_t1xhv_bitstream_buf_pos)(ps_bitstream_buf_pos))->addr_bitstream_buf_struct);

	ps_task_def_buffer_mgt->current_address_buffer_link = (
			(tps_t1xhv_bitstream_buf_pos)(ps_bitstream_buf_pos))->addr_bitstream_buf_struct;

	/* Link mode first part*/
	iMtf.readThroughMTF(address_to_read, sizeof_to_read, p_data_to_assign);

	/* Reset the total amount of resd data. */
	end_address = CONVERT_ENDIANNESS(
			((tps_t1xhv_bitstream_buf_link)(ps_bitstream_buf_link))->addr_buffer_end);
	/* - Buffer Current Address */
	start_address = CONVERT_ENDIANNESS(
			((tps_t1xhv_bitstream_buf_pos)(ps_bitstream_buf_pos))->addr_bitstream_start);

	/* Create a circular buffer with a link buffer */
	/* Program DMA */
	dma_link_bbm_programming(ps_bitstream_buf_link,&s_bitstream_buf);

	/* Program DMA in circular mode */
	dma_circular_bbm_programming(&s_bitstream_buf);

	/* We save the start state so that the scheduler will be able to update it */
	/* when we go other next links */
	/* The start state is referenced to the very first byte read in the bitstream and */
	/* not the start of the bitstream */
	/* in fact we patch the addr_start_current_link evaluated by the function dma_buffer_programming */
	dma_size_bbm (start_address,end_address,SIZE_INIT);

	/* First init of DMA then use ps_bitstream_buf_pos for current address */
	/* Rewrite start address written before */
	current_address = CONVERT_ENDIANNESS(
			((tps_t1xhv_bitstream_buf_pos)(ps_bitstream_buf_pos))->addr_bitstream_start);

	DMA_SET_REG_32_WAIT(ps_task_def_buffer_mgt->dma_bws_lsb,ps_task_def_buffer_mgt->dma_bws_msb,current_address); 
	

	//-printf("RM:Buffer:: Exited from Func setBufferLink\n");
} /* End of setBufferLink() function. */

void METH(setBufferLinkUpdate)(
		t_codec_type codec_type, 
		void * ps_bitstream_buf_pos, 
		void* ps_bitstream_buf_link, 
		Iresource_manager_api_signal *cb)
{
	t_uint32				current_address;				/* For start address calculation  */
	t_uint32				address_to_read,address_to_read1;				/* Address for read parameters    */
	t_uint16				sizeof_to_read =0;				/* Size of structure to read      */
	t_uint16				*p_data_to_assign= NULL;		/* Pointer on structure to assign */
	ts_t1xhv_bitstream_buf 	s_bitstream_buf;				/* Structure to program dma 	  */
	tps_task_buffer_mgt		ps_task_def_buffer_mgt;			/* Pointer on task buffer management defined by task_id. */
	t_uint32                start_address;
	t_uint32                end_address;					/* end address of the current link or window */
	ts_t1xhv_bitstream_buf_link* bitstream_buf_link;

	current_codec_type = codec_type;
	current_codec_cb = *cb;

	//-printf("\n\nRM:Buffer:: Entered into Func setBufferLinkUpdate\n");
	
	iInterrupt.subscribeInterrupt(BUFFER_EOW_IRQ, &iSignal);
	
	/* Assign pointer on global DMA registers definition */
	ps_task_def_buffer_mgt = &Gs_task_buffer_mgt_definition[
		current_codec_type==ID_DECODER ? 0 : 1];

	/* Common data for link buffer */
	sizeof_to_read = sizeof(ts_t1xhv_bitstream_buf_link);
	p_data_to_assign = (t_uint16*)ps_bitstream_buf_link;

	bitstream_buf_link = (ts_t1xhv_bitstream_buf_link*)ps_bitstream_buf_link;

	iMtf.readThroughMTF(ps_task_def_buffer_mgt->current_address_buffer_link,sizeof_to_read,p_data_to_assign);

	if (bitstream_buf_link->addr_next_buf_link != NULL )
	{
		address_to_read = CONVERT_ENDIANNESS(
		     ((ts_t1xhv_bitstream_buf_link*)(bitstream_buf_link))->addr_next_buf_link);

		address_to_read1 = bitstream_buf_link->addr_next_buf_link;          

		ps_task_def_buffer_mgt->current_address_buffer_link = bitstream_buf_link->addr_next_buf_link;
	}
	else
	{
		/* EOW is generated */
		signalEndOfLinkList(BUFFER_EOW);
		return;
	}

	/* Link mode first part*/
	iMtf.readThroughMTF(address_to_read, sizeof_to_read, p_data_to_assign);

	/* Reset the total amount of resd data. */
	end_address = CONVERT_ENDIANNESS(
			((tps_t1xhv_bitstream_buf_link)(bitstream_buf_link))->addr_buffer_end);
	/* - Buffer Current Address */
	start_address = CONVERT_ENDIANNESS(
			((tps_t1xhv_bitstream_buf_pos)(ps_bitstream_buf_pos))->addr_bitstream_start);

	/* Create a circular buffer with a link buffer */
	/* Program DMA */
	dma_link_bbm_programming(bitstream_buf_link,&s_bitstream_buf);

	/* Program DMA in circular mode */
	dma_circular_bbm_programming(&s_bitstream_buf);

	//-printf("RM:Buffer:: Exited from Func setBufferLinkUpdate\n");

} /* End of setBufferLink() function. */

/*****************************************************************************/
/**
 * \brief	signalInterrupt (updateBufferLink in old scheduler)
 * 
 * Set dma transfers for either deocder, either encode process and launch it
 * Only link behavior is supported. The link structure has to be complete
 * before the launch of the decode/encode process.  
 * 
 * \brief 	Program DMA for circular bistream buffer mode
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(signalInterrupt)(t_uint16 interruptSource)
{
	t_uint32                current_address;                /* For start address calculation  */
	t_ahb_address           address_to_read = 0;            /* Address for read parameters    */ 
	t_uint16                sizeof_to_read  = 0;            /* Size of structure to read      */
	t_uint16                *p_data_to_assign= NULL;        /* Pointer on structure to assign */
	ts_t1xhv_bitstream_buf_link* ps_bitstream_buf_link;
	ts_t1xhv_bitstream_buf_link bitstream_buf_link;
	ts_t1xhv_bitstream_buf 	s_bitstream_buf;			          /* Structure to program dma 	  */
	tps_task_buffer_mgt     ps_task_def_buffer_mgt;         /* Pointer on task buffer management defined by task_id. */

	//-printf("RM:BUFFER:: Entered into method signalInterrupt\n"); 

	if (interruptSource == BUFFER_EOW_IRQ) {
		/* Assign pointer on global DMA registers definition */
		ps_task_def_buffer_mgt = &Gs_task_buffer_mgt_definition[
			current_codec_type==ID_DECODER ? 0 : 1];
		ps_bitstream_buf_link  = &bitstream_buf_link;

		/* Common data for link buffer */
		sizeof_to_read = sizeof(ts_t1xhv_bitstream_buf_link);
		p_data_to_assign = (t_uint16*)ps_bitstream_buf_link;

		/* Reload current link buffer to check a new next link */

		/* Link mode first part*/
		iMtf.readThroughMTF(ps_task_def_buffer_mgt->current_address_buffer_link,sizeof_to_read,p_data_to_assign);

		/*******/
		/* EOW */
		/*******/
		/* Check next link */
		if (ps_bitstream_buf_link->addr_next_buf_link != NULL)
		{
			address_to_read = ps_bitstream_buf_link->addr_next_buf_link;          
			ps_task_def_buffer_mgt->current_address_buffer_link = ps_bitstream_buf_link->addr_next_buf_link;
		}
		else
		{
			/* EOW is generated */
			signalEndOfLinkList(BUFFER_EOW);
			return;
		}

		/* Link mode second part*/
		iMtf.readThroughMTF(ps_task_def_buffer_mgt->current_address_buffer_link,sizeof_to_read,p_data_to_assign);

		/* Create a circular buffer with a link buffer */
		/* Program DMA */
		dma_link_bbm_programming(ps_bitstream_buf_link,&s_bitstream_buf);

		/* Program DMA in circular mode */
		dma_circular_bbm_programming(&s_bitstream_buf);

		if (current_codec_type == ID_DECODER){
			DMA_SET_REG(DMA_VCU_IMR, CD_R | REC_W | MECC_R);
			/** \note removed _WAIT for 2.4.5.3 */
			DMA_SET_REG(DMA_VCU_ENR, CD_R | REC_W | MECC_R);
		}
		else {
			DMA_SET_REG(DMA_VCU_IMR, CD_W);
			DMA_SET_REG(DMA_VCU_ENR, CD_W);
		}
	}

	//-printf("RM:BUFFER:: Exited from method signalInterrupt\n"); 
} /* End of updateBufferLink() function. */

#pragma force_dcumode
void METH(signalBuffer)(t_t1xhv_buffer_status bufferStatus) {
	// needed to implement that method for cb interface iSignal
}

/*****************************************************************************/
/**
 * \brief	getBufferLinkStatus
 * 
 * Get the dma transfers status (nb of byte transmitted, current node of the link list, ...)
 * 
 * \brief 	Get DMA for link list buffer mode status
 * \param	p_buffer_link_status pointer buffer link status.
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(getBufferLinkStatus)(ts_buffer_link_status *p_buffer_link_status)
{
	//-printf("RM:BUFFER:: Entered into method getBufferLinkStatus\n");

	p_buffer_link_status->addr_start_current_link      = Gs_task_buffer_size.addr_start_current_link;
	p_buffer_link_status->addr_start_previous_link     = Gs_task_buffer_size.addr_start_previous_link;
	p_buffer_link_status->addr_end_current_link        = Gs_task_buffer_size.addr_end_current_link;

	p_buffer_link_status->nb_of_bytes_to_current_link  = Gs_task_buffer_size.nb_of_bytes_to_current_link;
	p_buffer_link_status->nb_of_bytes_to_previous_link = Gs_task_buffer_size.nb_of_bytes_to_previous_link;
	p_buffer_link_status->nb_of_bytes_in_current_link  = Gs_task_buffer_size.nb_of_bytes_in_current_link;

	p_buffer_link_status->current_address_buffer_link  = Gs_task_buffer_mgt_definition[
		current_codec_type==ID_DECODER ? 0 : 1].current_address_buffer_link;

	//-printf("addr_start_current_link = %x, addr_start_previous_link=%x, addr_end_current_link=%x\n", p_buffer_link_status->addr_start_current_link, p_buffer_link_status->addr_start_previous_link, p_buffer_link_status->addr_end_current_link);
	//-printf("nb_of_bytes_to_current_link = %x, nb_of_bytes_to_previous_link=%x, nb_of_bytes_in_current_link=%x\n", p_buffer_link_status->nb_of_bytes_to_current_link, p_buffer_link_status->nb_of_bytes_to_previous_link, p_buffer_link_status->nb_of_bytes_in_current_link);


} /* End of getBufferLinkStatus() function. */


/*------------------------------------------------------------------------
 * Private functions prototype
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/**
 * \brief 	Program DMA for circular bistream buffer mode
 * \author 	Loic Habrial
 * \param   ps_bitstream_buf pointer on bitstream buffer
 * \param   task_id ID for task to know task type
 *  
 * Write parameters inside DMA for circular buffer mode
 * 
 */
/*****************************************************************************/
t_uint16 dma_circular_bbm_programming(tps_t1xhv_bitstream_buf ps_bitstream_buf)
{
	t_uint32     current_address; /* Temp address calculation             */
	t_uint32     end_address;     /* End address to compute buffer size   */
	t_uint32     start_address;   /* Start address to compute buffer size */

#ifdef SOFT_ERR_BUFFER
	t_uint16 error_type = 0;     /* Error type */
#endif

	/* Pointer on task buffer management defined by task_id. */
	tps_task_buffer_mgt ps_task_def_buffer_mgt;

	/* Assign pointer on global DMA registers definition */
	ps_task_def_buffer_mgt = &Gs_task_buffer_mgt_definition[
		current_codec_type==ID_DECODER ? 0 : 1];
	
	//-printf("RM:BUFFER:: Entered into method dma_circular_bbm_programming\n");


#ifdef SOFT_ERR_BUFFER

	/* addr_buffer_start is not a multiple of 16 */
	if (CHECK_SOFT_ERR(ps_bitstream_buf->addr_buffer_start.lsb,4))
		error_type= ps_task_def_buffer_mgt->error_buffer_start; 

	/* addr_buffer_end is not a multiple of 16 */
	if (CHECK_SOFT_ERR(ps_bitstream_buf->addr_buffer_end.lsb,4))
		error_type= ps_task_def_buffer_mgt->error_buffer_end;

	/* addr_window_start is not a multiple of 16 */
	if (CHECK_SOFT_ERR(ps_bitstream_buf->addr_window_start.lsb,4))
		error_type= ps_task_def_buffer_mgt->error_window_start;

	/* addr_window_end is not a multiple of 16 */
	if (CHECK_SOFT_ERR(ps_bitstream_buf->addr_window_end.lsb,4))
		error_type= ps_task_def_buffer_mgt->error_window_end;

	if (error_type !=0)
		return error_type;

#endif      

	/* - Buffer start */
	current_address = CONVERT_ENDIANNESS(ps_bitstream_buf->addr_buffer_start); 
	/*DMA_SET_REG_32(ps_task_def_buffer_mgt->dma_bbs_lsb,ps_task_def_buffer_mgt->dma_bbs_msb,current_address); */

	/* Save start address for buffer size */
	start_address = current_address;


	/* No window start in encoder */
	//if (current_codec_type == ID_DECODER)
	{    
		/* - Window start */
		current_address = CONVERT_ENDIANNESS(ps_bitstream_buf->addr_window_start); 
		DMA_SET_REG_32(ps_task_def_buffer_mgt->dma_bws_lsb,ps_task_def_buffer_mgt->dma_bws_msb,current_address); 
	}

	/* BCA = BWS or BCA = BBS*/
	/* - Buffer Current Address */
	/*DMA_SET_REG_32_WAIT(ps_task_def_buffer_mgt->dma_bca_lsb,ps_task_def_buffer_mgt->dma_bca_msb,current_address); */

	/* - Buffer End address */
	current_address = CONVERT_ENDIANNESS(ps_bitstream_buf->addr_buffer_end); 
	/*DMA_SET_REG_32(ps_task_def_buffer_mgt->dma_bbe_lsb,ps_task_def_buffer_mgt->dma_bbe_msb,current_address); */

	/* Save end address for buffer size */
	end_address = current_address;

	/* - Window End address */
	current_address = CONVERT_ENDIANNESS(ps_bitstream_buf->addr_window_end); 
	DMA_SET_REG_32(ps_task_def_buffer_mgt->dma_bwe_lsb,ps_task_def_buffer_mgt->dma_bwe_msb,current_address); 

	/* No last address in decoder */
	/*   if (current_codec_type == ID_ENCODER)
		 {
		 DMA_SET_REG_32(ps_task_def_buffer_mgt->dma_bla_lsb,ps_task_def_buffer_mgt->dma_bla_msb,current_address); 
		 }*/

	/* Calculate bitstream size */
	dma_size_bbm(start_address,end_address,SIZE_UPDATE);

	return VDC_ERT_NONE;

} /* End of dma_circular_bbm_programming() function */

/*****************************************************************************/
/**
 * \brief 	Create an artificial circular buffer
 * \author 	Loic Habrial
 * \param   ps_bitstream_buf_link pointer on bitstream buffer link
 * \param   ps_bitstream_buf pointer on bitstream buffer 
 *  
 * Use bistream_buf to create an artificial circular buffer from a link buffer
 *
 */
/*****************************************************************************/
void dma_link_bbm_programming(
		tps_t1xhv_bitstream_buf_link ps_bitstream_buf_link,
		tps_t1xhv_bitstream_buf ps_bitstream_buf)
{
	//-printf("RM:BUFFER:: Entered into method dma_link_bbm_programming\n");
	/* Create an virtual circular buffer and assign buffer limits to window */
	ps_bitstream_buf->addr_buffer_start = ps_bitstream_buf_link->addr_buffer_start;
	ps_bitstream_buf->addr_buffer_end   = ps_bitstream_buf_link->addr_buffer_end;
	ps_bitstream_buf->addr_window_start = ps_bitstream_buf_link->addr_buffer_start;
	ps_bitstream_buf->addr_window_end   = ps_bitstream_buf_link->addr_buffer_end;
	//-printf("RM:BUFFER:: addr_buffer_start =%x, addr_buffer_end=%x\n", ps_bitstream_buf->addr_buffer_start,ps_bitstream_buf->addr_buffer_end);
}

/*****************************************************************************/
/**
 * \brief 	update number of bits read so far and number of bits in current link.
 * \author 	Loic Habrial
 * \param   start_size_address Start buffer address
 * \param   end_size_address End buffer address
 * \param   size_type Init or update calculation
 *
 */
/*****************************************************************************/
void dma_size_bbm (
		t_uint32      start_size_address,
		t_uint32      end_size_address,
		t_link_status size_type)
{
	//-printf("RM:BUFFER:: Entered into method dma_size_bbm\n");
	/* In init case nb_of_bytes_in_current_link is reseted */
	if (size_type == SIZE_INIT)
	{
		Gs_task_buffer_size.nb_of_bytes_to_previous_link = 0;
		Gs_task_buffer_size.nb_of_bytes_to_current_link = 0;
		Gs_task_buffer_size.addr_start_previous_link = 0xFFFFFFFFUL; /* not applicable */

	}
	else
	{
		Gs_task_buffer_size.nb_of_bytes_to_previous_link = Gs_task_buffer_size.nb_of_bytes_to_current_link;
		Gs_task_buffer_size.nb_of_bytes_to_current_link += Gs_task_buffer_size.nb_of_bytes_in_current_link;

		Gs_task_buffer_size.addr_start_previous_link = Gs_task_buffer_size.addr_start_current_link;
	}

	/* Save data to global variable */
	Gs_task_buffer_size.addr_start_current_link = start_size_address;
	Gs_task_buffer_size.addr_end_current_link = end_size_address;
	Gs_task_buffer_size.nb_of_bytes_in_current_link = 
		(t_uint32)((t_uint32)end_size_address - (t_uint32)start_size_address);


	//-printf("size_type = %d, nb_of_bytes_to_previous_link = %ld, nb_of_bytes_to_current_link=%ld , addr_start_previous_link = %x\n", size_type,Gs_task_buffer_size.nb_of_bytes_to_previous_link , Gs_task_buffer_size.nb_of_bytes_to_current_link, Gs_task_buffer_size.addr_start_previous_link  );
	//-printf("addr_start_current_link =%x,addr_end_current_link=%x,nb_of_bytes_in_current_link=%ld \n", start_size_address,end_size_address, Gs_task_buffer_size.nb_of_bytes_in_current_link);
	//-printf("RM:BUFFER:: Exited from method dma_size_bbm\n");
} /* End of dma_size_bbm() function. */


/*****************************************************************************/
/**
 * \brief	signalEndOfLinkList
 * 
 * Signal to codec component the end of the link list has been reached.
 * 
 * \brief 	Program DMA for circular bistream buffer mode
 * \param	codec_type
 * \param	buffer_status
 */
/*****************************************************************************/
void signalEndOfLinkList(t_t1xhv_buffer_status buffer_status)
{
	//-printf("RM:BUFFER:: Entered into method signalEndOfLinkList\n");

	iInterrupt.unsubscribeInterrupt(BUFFER_EOW_IRQ);
	current_codec_cb.signalBuffer(buffer_status);

	//-printf("RM:BUFFER:: Exired from method signalEndOfLinkList\n");
} /* End of signalEndOfLinkList() function. */
