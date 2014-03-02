/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public Header file of I2C Controller  module containing interrupts APIs
* \author  ST-Ericsson
*/
/*****************************************************************************/


#ifndef _I2C_IRQ_H_
#define _I2C_IRQ_H_


#include "hcl_defs.h"


#ifdef __cplusplus
extern "C"
{	/* In case C++ needs to use this header.*/
#endif


/*-----------------------------------------------------------------------------
	Typedefs
-----------------------------------------------------------------------------*/
#if((defined ST_8500ED)||(defined ST_HREFED)||(defined __PEPS_8500))
    typedef enum{
	I2C0,						
	I2C1,                       
	I2C2,						
	I2C3
}t_i2c_device_id;	
#else
    typedef enum{
	I2C0,						
	I2C1,                       
	I2C2,						
	I2C3,
	I2C4                        
}t_i2c_device_id;

#endif

typedef enum
{
    
    
    I2C_IRQ_SRC_TRANSMIT_FIFO_EMPTY	        		 =  MASK_BIT0,
    I2C_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY           =  MASK_BIT1,
    I2C_IRQ_SRC_TRANSMIT_FIFO_FULL                   =  MASK_BIT2,
    I2C_IRQ_SRC_TRANSMIT_FIFO_OVERRUN                =  MASK_BIT3,
    I2C_IRQ_SRC_RECEIVE_FIFO_EMPTY                   =  MASK_BIT4,
    I2C_IRQ_SRC_RECEIVE_FIFO_NEARLY_FULL             =  MASK_BIT5,
    I2C_IRQ_SRC_RECEIVE_FIFO_FULL                    =  MASK_BIT6,
    I2C_IRQ_SRC_READ_FROM_SLAVE_REQUEST              =  MASK_BIT16,
    I2C_IRQ_SRC_READ_FROM_SLAVE_EMPTY                =  MASK_BIT17,
    I2C_IRQ_SRC_WRITE_TO_SLAVE_REQUEST               =  MASK_BIT18,
    I2C_IRQ_SRC_MASTER_TRANSACTION_DONE              =  MASK_BIT19,
    I2C_IRQ_SRC_SLAVE_TRANSACTION_DONE               =  MASK_BIT20,
    I2C_IRQ_SRC_MASTER_ARBITRATION_LOST              =  MASK_BIT24,
    I2C_IRQ_SRC_BUS_ERROR                            =  MASK_BIT25,
    I2C_IRQ_SRC_MASTER_TRANSACTION_DONE_WITHOUT_STOP =  MASK_BIT28,
    I2C_IRQ_SRC_ALL                                  =  0x131F007F,
   
} t_i2c_irq_src_id;



/* Macros for handling the device id */
#define I2CID_SHIFT							7
#define GETDEVICE(irqsrc)					((t_i2c_device_id)((irqsrc >>I2CID_SHIFT ) & 0x7))


/* a macro for masking all interrupts */

typedef t_uint32 t_i2c_irq_src; /*Combination of various interrupt sources
                                     described by t_i2c_irq_src_id*/

/*-----------------------------------------------------------------------------
	Events and interrupts management functions
-----------------------------------------------------------------------------*/
PUBLIC void             I2C_SetBaseAddress  (t_i2c_device_id id, t_logical_address address );
PUBLIC void             I2C_EnableIRQSrc    (t_i2c_irq_src_id id);
PUBLIC void             I2C_DisableIRQSrc   (t_i2c_irq_src_id id);
PUBLIC t_i2c_irq_src_id I2C_GetIRQSrc       (t_i2c_device_id id);
PUBLIC void 			I2C_ClearIRQSrc     (t_i2c_irq_src_id id);
PUBLIC t_bool           I2C_IsPendingIRQSrc (t_i2c_irq_src_id id);
PUBLIC t_i2c_device_id  I2C_GetDeviceID     (t_i2c_irq_src_id id);

#ifdef __cplusplus
}   /* Allow C++ to use this header */
#endif  /* __cplusplus              */

#endif	/* _I2C_IRQ_H_               */
