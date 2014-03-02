/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file of High Speed Serial Interface (HSI) module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _HSI_IRQP_H_
#define _HSI_IRQP_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include "hsi_irq.h"

/*---------------------------------------------------------
HSI_TX registers
---------------------------------------------------------*/
typedef volatile struct
{
    t_uint32    hsit_id;                             /* HSIT Identification Register */                          /*0x000*/
    t_uint32    hsit_mode;                           /* HSIT Mode Register */                                    /*0x004*/
    t_uint32    hsit_tx_state;                       /* HSIT Transmitter State Registerc*/                       /*0x008*/
    t_uint32    hsit_iostate;                        /* HSIT Input Output State Register */                      /*0x00C*/
    t_uint32    hsit_bufstate;                       /* HSIT Buffer State Register */                            /*0x010*/
    t_uint32    hsit_divisor;                        /* HSIT Divisor State Register */                           /*0x014*/
#if((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
	t_uint32	reserved0;							 /*Parity Register has been removed in V2.0							 */
#else
    t_uint32    hsit_parity;                         /* HSIT Parity Register */                                  /*0x018*/
#endif
    t_uint32    hsit_break;                          /* HSIT Break Register  */                                  /*0x01C*/
    t_uint32    hsit_channel;                        /* HSIT Channel Register  */                                /*0x020*/
    t_uint32    hsit_flushbits;                      /* HSIT Flush Bits Register  */                             /*0x024*/
    t_uint32    hsit_priority;                       /* HSIT Priority Register    */                             /*0x028*/
    t_uint32    hsit_burstlen;                       /* HSIT Burst Length Register */                            /*0x02C*/
    t_uint32    hsit_preamble;                       /* HSIT Preamble Register */                                /*0x030*/
    t_uint32    hsit_dataswap;                       /* HSIT Watermark Data Swapping Register */                 /*0x034*/
    t_uint32    reserved1[(0x080 - 0x038) >> 2];     /*reserved*/                                                /*0x038 - 0x07C*/
    t_uint32    hsit_framelen[8];                    /* HSIT Frame Length for all the 8 Channels */              /*0x080 - 0x09C*/
    t_uint32    reserved2[(0x0C0 - 0x0A0) >> 2];     /*reserved*/                                                /*0x0A0 - 0x0BC*/
    t_uint32    hsit_buffer[8];                      /* HSIT Buffer Register */                                  /*0x0C0 - 0x0DC*/
    t_uint32    reserved3[(0x100 - 0x0E0) >> 2];     /*reserved*/                                                /*0x0E0 - 0x0FC*/
    t_uint32    hsit_base[8];                        /* HSIT Base Register */                                    /*0x100 - 0x11C*/
    t_uint32    reserved4[(0x140 - 0x120) >> 2];     /*reserved*/                                                /*0x120 - 0x13C*/
    t_uint32    hsit_span[8];                        /* HSIT Span Register */                                    /*0x140 - 0x15C*/
    t_uint32    reserved5[(0x180 - 0x160) >> 2];     /*reserved*/                                                /*0x160 - 0x17C*/
    t_uint32    hsit_gauge[8];                       /* HSIT Gauge Register */                                   /*0x180 - 0x19C*/
    t_uint32    reserved6[(0x1C0 - 0x1A0) >> 2];     /*reserved*/                                                /*0x1A0 - 0x1BC*/
    t_uint32    hsit_watermark[8];                   /* HSIT Watermark Register */                               /*0x1C0 - 0x1DC*/
    t_uint32    reserved7[(0x200 - 0x1E0) >> 2];     /*reserved*/                                                /*0x1E0 - 0x1FC*/
    t_uint32    hsit_dmaen;                          /* HSIT DMA Enable Register */                              /*0x200*/
    t_uint32    hsit_wmarkmis;                       /* HSIT Watermark Masked Interrupt Status Register */       /*0204*/
    t_uint32    hsit_wmarkim;                        /* HSIT Watermark Interrupt Masked Register */              /*0208*/
    t_uint32    hsit_wmarkic;                        /* HSIT Watermark Interrupt Clear  Register */              /*020C*/
    t_uint32    hsit_wmarid;                         /* HSIT Watermark Interrupt Destination Register */         /*0x210*/
    t_uint32    reserved8[(0xD00 - 0x214) >> 2];     /*reserved*/                                                /*0x214 - 0xCFC*/
    t_uint32    hsit_itcr;                           /* HSIT Test Control Register */                            /*0xD00*/
    t_uint32    hsit_itip;                           /* HSIT Test Input  Register  */                            /*0xD04*/
    t_uint32    hsit_itop1;                          /* HSIT Integration Test Output 1 Register */               /*0xD08*/
    t_uint32    hsit_itop2;                          /* HSIT Integration Test Output 2 Register */               /*0xD0C*/
    t_uint32    reserved9[(0xFE0 - 0xD10) >> 2];     /*reserved*/                                                /*0xD10 - 0xFDC*/
    t_uint32    hsit_periphrl_id0;                   /* HSIT Periphiral Identification Register 0 */             /*0xFE0*/
    t_uint32    hsit_periphrl_id1;                   /* HSIT Periphiral Identification Register 1 */             /*0xFE4*/
    t_uint32    hsit_periphrl_id2;                   /* HSIT Periphiral Identification Register 2 */             /*0xFE8*/
    t_uint32    hsit_periphrl_id3;                   /* HSIT Periphiral Identification Register 3 */             /*0xFEC*/
    t_uint32    hsit_cell_id0;                       /* HSIT Cell Identification Register 0 */                   /*0xFF0*/              
    t_uint32    hsit_cell_id1;                       /* HSIT Cell Identification Register 1 */                   /*0xFF4*/
    t_uint32    hsit_cell_id2;                       /* HSIT Cell Identification Register 2 */                   /*0xFF4*/
    t_uint32    hsit_cell_id3;                       /* HSIT Cell Identification Register 3 */                   /*0xFFC*/
} t_hsi_tx_register;

/*---------------------------------------------------------
HSI_RX registers
---------------------------------------------------------*/
typedef volatile struct
{
    t_uint32    hsir_id;                             /* HSIR Identification Register */                          /*0x000*/
    t_uint32    hsir_mode;                           /* HSIR Mode Register */                                    /*0x004*/
    t_uint32    hsir_rx_state;                       /* HSIR Reciever State Register*/                           /*0x008*/
    t_uint32    hsir_bufstate;                       /* HSIR Buffer State Register */                            /*0x00C*/
    t_uint32    hsir_threshold;                      /* HSIR Threshold Register */                               /*0x010*/
#if ((defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
	t_uint32 	reserved0;							 /* Parity Register has been removed from V2.0						*/
#else
	t_uint32    hsir_parity;                         /* HSIR Parity Register */                                  /*0x014*/
#endif
    t_uint32    hsir_detector;                       /* HSIR Detector Register  */                               /*0x018*/
    t_uint32    hsir_exception;                      /* HSIR Exception Register */                               /*0x01C*/
    t_uint32    hsir_acknowladge;                    /* HSIR Acknowladge Register */                             /*0x020*/
    t_uint32    hsir_channel;                        /* HSIR Channel Register  */                                /*0x024*/
    t_uint32    hsir_realtime;                       /* HSIR Real Time Register */                               /*0x028*/
    t_uint32    hsir_overrun;                        /* HSIR Over Run Register */                                /*0x02C*/
    t_uint32    hsir_overrunack;                     /* HSIR Over Run ACknowladge Register */                    /*0x030*/
    t_uint32    hsir_preamble;                       /* HSIR Preamble Register */                                /*0x034*/
    t_uint32    hsir_pipegauge;                      /* HSIR Pipe Gauge Register */                              /*0x038*/
#if ((defined ST_8500ED) || (defined ST_HREFED) || (defined __PEPS_8500))
    t_uint32    hsir_timeout;                        /* HSIR TimeOut Register */                                 /*0x03C*/
    t_uint32    reserved1[(0x080 - 0x040) >> 2];     /*reserved*/                                                /*0x040 - 0x07C*/
#else
    t_uint32    reserved1[(0x080 - 0x03C) >> 2];     /*reserved*/                                                /*0x03C - 0x07C*/ 
#endif
    t_uint32    hsir_buffer[8];                      /* HSIR Buffer Register */                                  /*0x080 - 0x09C*/
    t_uint32    reserved2[(0x0C0 - 0x0A0) >> 2];     /*reserved*/                                                /*0x0A0 - 0x0BC*/
    t_uint32    hsir_framelen[8];                    /* HSIR Frame Length for all the 8 Channels */              /*0x0C0 - 0x0DC*/
    t_uint32    reserved3[(0x100 - 0x0E0) >> 2];     /*reserved*/                                                /*0x0E0 - 0x0FC*/
    t_uint32    hsir_base[8];                        /* HSIR Base Register */                                    /*0x100 - 0x11C*/
    t_uint32    reserved4[(0x140 - 0x120) >> 2];     /*reserved*/                                                /*0x120 - 0x13C*/
    t_uint32    hsir_span[8];                        /* HSIR Span Register */                                    /*0x140 - 0x15C*/
    t_uint32    reserved5[(0x180 - 0x160) >> 2];     /*reserved*/                                                /*0x160 - 0x17C*/
    t_uint32    hsir_gauge[8];                       /* HSIR Gauge Register */                                   /*0x180 - 0x19C*/
    t_uint32    reserved6[(0x1C0 - 0x1A0) >> 2];     /*reserved*/                                                /*0x1A0 - 0x1BC*/
    t_uint32    hsir_watermark[8];                   /* HSIR Watermark Register */                               /*0x1C0 - 0x1DC*/
#if ((defined ST_8500ED) || (defined ST_HREFED) || (defined __PEPS_8500))
	 t_uint32    reserved7[(0x200 - 0x1E0) >> 2];     /*reserved*/                                                /*0x1E0 - 0x1FC*/
#else
    t_uint32    hsir_frameburstcnt;                  /* HSIR Frame Burst Count  */                               /*0x1E0*/
    t_uint32    reserved7[(0x200 - 0x1E4) >> 2];     /*reserved*/                                                /*0x1E4 - 0x1FC*/        
#endif
    t_uint32    hsir_dmaen;                          /* HSIR DMA Enable Register */                              /*0x200*/
    t_uint32    hsir_wmarkmis;                       /* HSIR Watermark Masked Interrupt Status Register */       /*0204*/
    t_uint32    hsir_wmarkim;                        /* HSIR Watermark Interrupt Masked Register */              /*0208*/
    t_uint32    hsir_wmarkic;                        /* HSIR Watermark Interrupt Clear  Register */              /*020C*/
    t_uint32    hsir_wmarid;                         /* HSIR Watermark Interrupt Destination Register */         /*0x210*/
    t_uint32    hsir_overrunmis;                     /* HSIR OverRun Masked Interrupt Status Register */         /*0x214*/
    t_uint32    hsir_overrunim;                      /* HSIR OverRun Mask Register */                            /*0x218*/
    t_uint32    hsir_excepmis;                       /* HSIR Exception Masked Interrupt Status Register */       /*0x21C*/
    t_uint32    hsir_excepim;                        /* HSIR Exception Masked Register */                        /*0x220*/
    t_uint32    reserved8[(0xD00 - 0x224) >> 2];     /*reserved*/                                                /*0x224 - 0xCFC*/
    t_uint32    hsir_itcr;                           /* HSIR Test Control Register */                            /*0xD00*/
    t_uint32    hsir_itip;                           /* HSIR Test Input  Register  */                            /*0xD04*/
    t_uint32    hsir_itop1;                          /* HSIR Integration Test Output 1 Register */               /*0xD08*/
    t_uint32    hsir_itop2;                          /* HSIR Integration Test Output 2 Register */               /*0xD0C*/
    t_uint32    hsir_itop3;                          /* HSIR Integration Test Output 3 Register */               /*0xD10*/  
    t_uint32    reserved9[(0xFE0 - 0xD14) >> 2];     /*reserved*/                                                /*0xD14 - 0xFDC*/
    t_uint32    hsir_periphrl_id0;                   /* HSIR Periphiral Identification Register 0 */             /*0xFE0*/
    t_uint32    hsir_periphrl_id1;                   /* HSIR Periphiral Identification Register 1 */             /*0xFE4*/
    t_uint32    hsir_periphrl_id2;                   /* HSIR Periphiral Identification Register 2 */             /*0xFE8*/
    t_uint32    hsir_periphrl_id3;                   /* HSIR Periphiral Identification Register 3 */             /*0xFEC*/
    t_uint32    hsir_cell_id0;                       /* HSIR Cell Identification Register 0 */                   /*0xFF0*/              
    t_uint32    hsir_cell_id1;                       /* HSIR Cell Identification Register 1 */                   /*0xFF4*/
    t_uint32    hsir_cell_id2;                       /* HSIR Cell Identification Register 2 */                   /*0xFF4*/
    t_uint32    hsir_cell_id3;                       /* HSIR Cell Identification Register 3 */                   /*0xFFC*/
} t_hsi_rx_register;

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* _HSI_IRQP_H_*/

/* End of file - hsi_irqp.h*/

