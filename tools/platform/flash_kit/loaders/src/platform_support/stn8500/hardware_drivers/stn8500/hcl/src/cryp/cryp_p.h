/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
*  This module provides prototype for CRYP Private functions and structrue
*  AUTHOR :  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _CRYP_P_H_
#define _CRYP_P_H_

#include "cryp.h"
#include "cryp_irqp.h"

#ifdef __cplusplus
extern "C"
{	/* In case C++ needs to use this header.*/
#endif

/*-----------------------------------------------------------------------------
	Generic Macros
-----------------------------------------------------------------------------*/
#define CRYP_SET_BITS(reg_name,mask)          HCL_SET_BITS(reg_name,mask)
#define CRYP_CLR_BITS(reg_name,mask)          HCL_CLEAR_BITS(reg_name,mask)
#define CRYP_WRITE_BIT(reg_name,val,mask)     HCL_WRITE_BITS(reg_name,val,mask)
#define CRYP_TEST_BITS(reg_name,val)          HCL_READ_BITS(reg_name,val)
#define CRYP_WRITE_REG(reg_name,val)          HCL_WRITE_REG(reg_name,val)
#define CRYP_READ_REG(reg_name)               HCL_READ_REG(reg_name)
#define CRYP_CLEAR                            MASK_NULL32

#define CRYP_PUT_BITS(reg,val,shift,mask)   ((reg) =   (((reg) & ~(mask)) | (((t_uint32)val << shift) & (mask))))
/*-----------------------------------------------------------------------------
	CRYP specific Macros
-----------------------------------------------------------------------------*/
#define CRYP_PERIPHERAL_ID0     0xE3
#define CRYP_PERIPHERAL_ID1     0x05
#define CRYP_PERIPHERAL_ID2     0x28
#define CRYP_PERIPHERAL_ID3     0x00

#define CRYP_PCELL_ID0      0x0D
#define CRYP_PCELL_ID1      0xF0
#define CRYP_PCELL_ID2      0x05
#define CRYP_PCELL_ID3      0xB1


/*------------------------------------------------------------------------
	CRYP register defalut
------------------------------------------------------------------------*/
#define MAX_DEVICE_SUPPORT      2
//#define CRYP_CR_DEFAULT         0x0
//#define CRYP_CR_DEFAULT         0x0001
#define CRYP_CR_DEFAULT         0x0003
#define CRYP_DMACR_DEFAULT		0x0
#define CRYP_IMSC_DEFAULT		0x0
#define CRYP_DIN_DEFAULT        0x0
#define CRYP_DOUT_DEFAULT       0x0

#define CRYP_KEY_DEFAULT		0x0
#define CRYP_INIT_VECT_DEFAULT  0x0

/*------------------------------------------------------------------------
	CRYP Control register specific mask 	
------------------------------------------------------------------------*/
#if ((defined ST_HREFV2) || (defined ST_8500V2) || (defined __PEPS_8500_V2))
#define CRYP_KEY_LOCK_MASK		MASK_BIT24
#endif

#define CRYP_STEALING_MASK		MASK_BIT20
#define CRYP_SECOND_LAST_MASK	MASK_BIT16
#define CRYP_ACTIVITY_MASK		MASK_BIT15
#define CRYP_INIT_MASK		    MASK_BIT13
#define CRYP_START_MASK		    MASK_BIT12	
#define CRYP_FIFO_FLUSH_MASK    MASK_BIT14	

#define CRYP_KEY_ACCESS_MASK    MASK_BIT10	
#define CRYP_KEY_SIZE_MASK      (MASK_BIT9 | MASK_BIT8)	
#define CRYP_DATA_TYPE_MASK	    (MASK_BIT7 | MASK_BIT6)	  
#define CRYP_ALGO_MASK	        (MASK_BIT5 | MASK_BIT4 |MASK_BIT3)	
#define CRYP_ENC_DEC_MASK	    MASK_BIT2	
#define CRYP_KSE_MASK           MASK_BIT11
#define CRYP_SR_BUSY_MASK	    MASK_BIT4
#define CRYP_PRLG_MASK          MASK_BIT1
#define CRYP_SECURE_MASK        MASK_BIT0

/*------------------------------------------------------------------------
   BIT POSITION USED while setting bits in register
------------------------------------------------------------------------*/
#if ((defined ST_HREFV2) || (defined ST_8500V2) || (defined __PEPS_8500_V2))
#define CRYP_KEY_LOCK_POS   	24
#endif

#define CRYP_STEALING_POS   	20
#define CRYP_SECOND_LAST_POS   	16
#define CRYP_ACTIVITY_POS   	15
#define CRYP_INIT_POS       	13
#define CRYP_START_POS      	12
#define CRYP_KEY_ACCESS_POS 	10
#define CRYP_KEY_SIZE_POS   	8
#define CRYP_DATA_TYPE_POS  	6
#define CRYP_ALGO_POS       	3
#define CRYP_ENC_DEC_POS    	2
#define CRYP_PRLG_POS       	1
#define CRYP_KSE_POS        	11
#define CRYP_SR_BUSY_POS    	4

/*-----------------------------------------------------------------------------
	CRYP Status register
-----------------------------------------------------------------------------*/
#define CRYP_INFIFO_EMPTY_MASK  	MASK_BIT0
#define CRYP_INFIFO_NOTFULL_MASK  	MASK_BIT1
#define CRYP_OUTFIFO_NOTEMPTY_MASK  MASK_BIT2
#define CRYP_OUTFIFO_FULL_MASK  	MASK_BIT3
#define CRYP_BUSY_STATUS_MASK  		MASK_BIT4

/*-----------------------------------------------------------------------------
	CRYP PCRs------PC_NAND control register
	BIT_MASK 
-----------------------------------------------------------------------------*/

#define CRYP_DMA_REQ_MASK       (MASK_BIT1 | MASK_BIT0)	
#define CRYP_DMA_REQ_MASK_POS 	0


/*-----------------------------------------------------------------------------
	CRYP power management specifc structure.
-----------------------------------------------------------------------------*/
typedef struct
{	
    t_uint32 cnfg;		
    t_uint32 dmacr;		
    t_uint32 imsc;      
    
    t_uint32 key_1_l;    
    t_uint32 key_1_r;    
    t_uint32 key_2_l;    
    t_uint32 key_2_r;    
    t_uint32 key_3_l;    
    t_uint32 key_3_r;    
    t_uint32 key_4_l;    
    t_uint32 key_4_r;    
    
    t_uint32 init_vect_0_l; 
    t_uint32 init_vect_0_r; 
    t_uint32 init_vect_1_l; 
    t_uint32 init_vect_1_r; 
    
}t_cryp_device_context;

typedef struct
{
    t_cryp_register			*p_cryp_reg[MAX_DEVICE_SUPPORT]; 	    /*CRYP Register structure*/
    t_cryp_device_context 	device_context[MAX_DEVICE_SUPPORT];		/*Device context*/
}t_cryp_system_context;

#ifdef __cplusplus
} /*Allow C++ to use this header */
#endif  /* __cplusplus   */
#endif	

