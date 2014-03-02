
/****************************************************************************
 *                            C STMicroelectronics
 *    Reproduction and Communication of this document is strictly prohibited 
 *      unless specifically authorized in writing by STMicroelectronics.
 *------------------------------------------------------------------------
 * Header files for DIF Panel Services initialization module. 
 * 
 *------------------------------------------------------------------------
 *
 ****************************************************************************/

/*--------------------------------------------------------------------------*
 * Includes																	*
 *--------------------------------------------------------------------------*/
#ifndef _SER_DIF_PANEL_
#define _SER_DIF_PANEL_

#ifdef	__cplusplus
extern "C" {
#endif 
#include "dif.h"
#include "dma.h"
#include "tmr.h" 

/*--------------------------------------------------------------------------*
 * type def and structures
 *--------------------------------------------------------------------------*/


#define DIF_LCD_HEIGHT     208
#define DIF_LCD_WIDTH      176
#define MIN_REFRESH_RATE	1
#define MAX_REFRESH_RATE	100



/*--------------------------------------------------------------------------*
 * private type def and structures
 *--------------------------------------------------------------------------*/


typedef enum{
    SER_DIF_COLOR_8BITS,
    SER_DIF_COLOR_12BITS,
    SER_DIF_COLOR_16BITS,
    SER_DIF_COLOR_24BITS,
    SER_DIF_COLOR_32BITS
}t_ser_dif_color_depth;



typedef enum 
{
	SER_DIF_OK	    =  0,
	SER_DIF_ERROR	= -1
} t_ser_dif_error;




typedef enum {
    SER_DIF_STATE_NOT_CONFIGURED,
    SER_DIF_STATE_CONFIGURED,
    SER_DIF_STATE_RUNNING
} t_ser_dif_status;


typedef struct 
{
	t_ser_dif_status  status;
	t_ser_dif_color_depth  color_depth;
	t_uint8 frame_rate;
	t_tmr_id tmr_id;
#if (defined __STN_8800) || ((defined __EMUL) && (__EMUL >= 300) && (__EMUL < 400))
	t_physical_address frame_buffer_addr;
	t_uint32 tsize;
    t_dma_pipe_id dma_id;
	t_dma_exchange_id exchangeid;
	t_dma_mem_buffer    dma_buff;	
#endif 
	
#if (defined __STN_8810) || (defined __STN_8815) || ((defined __EMUL) && (__EMUL >= 400))
	t_dif_frame_desc frame_desc;
#endif 
} t_idif_internal;

/*---------------------------------------------------------------------------
 *  Functions Prototype                                                   
 *---------------------------------------------------------------------------*/
PUBLIC t_ser_dif_error SER_DIF_Panel_Configure(IN t_ser_dif_color_depth,
                                            IN t_uint8,  IN t_physical_address);
PUBLIC t_ser_dif_error SER_DIF_Panel_Run(void);
PUBLIC void SER_DIF_Panel_Stop(void);

 
#ifdef __cplusplus
} /* allow C++ to use these headers */
#endif	/* __cplusplus */

#endif /* _SER_DIF_PANEL_ */
