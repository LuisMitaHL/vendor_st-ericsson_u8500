/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file of I2C Controller module
* \author  ST-Ericsson
*/
/*****************************************************************************/



#ifndef _I2C_IRQP_H_
#define _I2C_IRQP_H_

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "hcl_defs.h"

#ifdef __cplusplus
extern "C"
{	/* In case C++ needs to use this header.*/
#endif


/*-----------------------------------------------------------------------------

	Generic Macros

-----------------------------------------------------------------------------*/


#define I2C_CLEAR                           MASK_NULL32


#define I2C_WRITE_FIELD(reg_name,mask,shift,value) \
                  (reg_name = ((reg_name & ~mask) | (value << shift)))
                  
                  
#define I2C_READ_FIELD(reg_name,mask,shift)    ((reg_name & mask) >> shift )

typedef volatile struct  
{
   	

    t_uint32 cr;                                 /* Control Register                      0x00 */
    t_uint32 scr;                                /* Slave Address Register                0x04 */
    t_uint32 hsmcr;                              /* HS Master code Register               0x08 */
    t_uint32 mcr;                                /* Master Control Register               0x0C */
    t_uint32 tfr;                                /* Transmit Fifo Register                0x10 */
    t_uint32 sr;                                 /* Status Register                       0x14 */
    t_uint32 rfr;                                /* Receiver Fifo Register                0x18 */
    t_uint32 tftr;                               /* Transmit Fifo Threshold Register      0x1C */
    t_uint32 rftr;                               /* Receiver Fifo Threshold Register      0x20 */
    t_uint32 dmar;                               /* DMA register                          0x24 */
    t_uint32 brcr;                               /* Baud Rate Counter Register            0x28 */
    t_uint32 imscr;                              /* Interrupt Mask Set and Clear Register 0x2C */
    t_uint32 risr;                               /* Raw interrupt status register         0x30 */
    t_uint32 misr;                               /* Masked interrupt status register      0x34 */
    t_uint32 icr;                                /* Interrupt Set and Clear Register      0x38 */
	t_uint32 itcr;								 /* Integration Test Control Register	  0x3C */
	t_uint32 itip;								 /* Integration Test Input Register		  0x40 */
	t_uint32 itop;								 /* Integration Test Output Register	  0x44 */
	t_uint32 tdr;								 /* Test Data Register 					  0x48 */
	t_uint32 thddat;							 /* Hold Time Data						  0x4C */
	t_uint32 thdsta_fst_std;					 /* Hold Time Start Condition Register	  0x50 */
	t_uint32 thdsta_fmp_hs;						 /* Hold Time Start Condition HS Register 0x54 */
	t_uint32 tsusta_fst_std;					 /* Setup Time Start Condition Register	  0x58 */
	t_uint32 tsusta_fmp_hs;						 /* Setup Time Start Condition Register	  0x5C */
    t_uint32 reserved_1[(0xFE0 - 0x60) >> 2];    /* Reserved	                          0x060 to 0xFE0*/
    t_uint32 periph_id_0;                        /*peripheral ID 0                        0xFE0  */
	t_uint32 periph_id_1;                        /*peripheral ID 1                        0xFE4  */
	t_uint32 periph_id_2;                        /*peripheral ID 2                        0xFE8  */ 
	t_uint32 periph_id_3;                        /*peripheral ID 3                        0xFEC  */
	t_uint32 cell_id_0;                          /*I2C cell   ID 0                        0xFF0  */
	t_uint32 cell_id_1;                          /*I2C cell   ID 1                        0xFF4  */
 	t_uint32 cell_id_2;                          /*I2C cell   ID 2                        0xFF8  */
  	t_uint32 cell_id_3;                          /*I2C cell   ID 3                        0xFFC  */
    

}t_i2c_registers;


/* Control Register */ 
   /* Mask values for control register mask */
#define I2C_CR_PE          MASK_BIT0       /* Peripheral enable*/
#define I2C_CR_OM          0x6             /* Operation mode  */  
#define I2C_CR_SAM         MASK_BIT3       /* Slave Addressing mode */
#define I2C_CR_SM          0x30            /* Speed mode  */ 
#define I2C_CR_SGCM        MASK_BIT6       /* Slave General call mode */
#define I2C_CR_FTX         MASK_BIT7       /* Flush Transmit     */
#define I2C_CR_FRX         MASK_BIT8       /* Flush Receive */
#define I2C_CR_DMA_TX_EN   MASK_BIT9       /* DMA TX Enable */
#define I2C_CR_DMA_RX_EN   MASK_BIT10      /* DMA Rx Enable */
#define I2C_CR_DMA_SLE     MASK_BIT11      /* DMA Synchronization Logic enable */
#define I2C_CR_LM          MASK_BIT12      /* Loop back mode */
#define I2C_CR_FON         0x6000          /* Filtering On */
#if((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1) ||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
#define I2C_CR_FS          MASK_BIT15      /* Force Stop */
#endif
   /*  shift valus for control register bit fields */
#define I2C_CR_SHIFT_PE          0         /* Peripheral enable*/
#define I2C_CR_SHIFT_OM          1         /* Operation mode  */  
#define I2C_CR_SHIFT_SAM         3         /* Slave Addressing mode */
#define I2C_CR_SHIFT_SM          4         /* Speed mode  */ 
#define I2C_CR_SHIFT_SGCM        6         /* Slave General call mode */
#define I2C_CR_SHIFT_FTX         7         /* Flush Transmit     */
#define I2C_CR_SHIFT_FRX         8         /* Flush Receive */
#define I2C_CR_SHIFT_DMA_TX_EN   9         /* DMA TX Enable */
#define I2C_CR_SHIFT_DMA_RX_EN   10        /* DMA Rx Enable */
#define I2C_CR_SHIFT_DMA_SLE     11        /* DMA Synchronization Logic enable */
#define I2C_CR_SHIFT_LM          12        /* Loop back mode */
#define I2C_CR_SHIFT_FON         13        /* Filtering On */
#if((defined ST_8500V1)||(defined ST_HREFV1)||(defined __PEPS_8500_V1)||(defined __PEPS_8500_V2)||(defined ST_8500V2)||(defined ST_HREFV2))
#define I2C_CR_SHIFT_FS          15        /* Force Stop */
#endif

/* Slave control register*/
  /* Mask values slave control register */
#define I2C_SCR_ADDR                   0x3FF
#define I2C_SCR_DATA_SETUP_TIME        0xFFFF0000

   /* Shift values for Slave control register */
#define I2C_SCR_SHIFT_ADDR               0
#define I2C_SCR_SHIFT_DATA_SETUP_TIME    16 
  
  

/* Master Control Register */
   /* Mask values for Master control register */
#define I2C_MCR_OP      MASK_BIT0          /* Operation */
#define I2C_MCR_A7      0xFE               /* LSB bits of the Address(7-bit ) */
#define I2C_MCR_EA10    0x700             /* Extended Address */
#define I2C_MCR_SB      MASK_BIT11         /* Start byte procedure */
#define I2C_MCR_AM      0x3000             /* Address type */
#define I2C_MCR_STOP    MASK_BIT14         /* stop condition */
#define I2C_MCR_LENGTH  0x3FF8000           /* Frame length */
#define I2C_MCR_A10     0x7FE              /* Define to set the 10 bit address */

#define I2C_MCR_LENGTH_STOP_OP    0x3FFC001  /*mask for length field,stop and operation  */
  /* Shift values for Master control values */

#define I2C_MCR_SHIFT_OP      0            /* Operation */
#define I2C_MCR_SHIFT_A7      1            /* LSB bits of the Address(7-bit ) */
#define I2C_MCR_SHIFT_EA10    8            /* Extended Address */
#define I2C_MCR_SHIFT_SB      11           /* Start byte procedure */
#define I2C_MCR_SHIFT_AM      12           /* Address type */
#define I2C_MCR_SHIFT_STOP    14           /* stop condition */
#define I2C_MCR_SHIFT_LENGTH  15           /* Frame length */
#define I2C_MCR_SHIFT_A10     1            /* define to set the 10 bit addres */
  
#define I2C_MCR_SHIFT_LENGTH_STOP_OP   0

/*  Status Register */
  /* Mask values for Status register */
#define I2C_SR_OP       0x3                /* Operation */
#define I2C_SR_STATUS   0xC                /* Controller Status */
#define I2C_SR_CAUSE    0x70               /* Abort Cause */
#define I2C_SR_TYPE     0x180               /* Receive Type */
#define I2C_SR_LENGTH   0xFF700            /* Transfer length */

/* Shift values for Status register */
#define I2C_SR_SHIFT_OP       0            /* Operation */
#define I2C_SR_SHIFT_STATUS   2            /* Controller Status */
#define I2C_SR_SHIFT_CAUSE    4            /* Abort Cause */
#define I2C_SR_SHIFT_TYPE     7            /* Receive Type */
#define I2C_SR_SHIFT_LENGTH   9            /* Transfer length */
#define I2C_NACK_ADDR		  0
#define I2C_NACK_DATA		  1
#define I2C_ACK_MCODE		  2
#define I2C_ARB_LOST		  3
#define I2C_BERR_START		  4
#define I2C_BERR_STOP		  5
#define I2C_OVFL 			  6

/* DMA Register */
  /* Mask values for DMA register */ 
#define I2C_DMA_SBSIZE_RX  0x7            /* Source Burst Size Rx */
#define I2C_DMA_BURST_RX   MASK_BIT3      /* Burst Rx */
#define I2C_DMA_DBSIZE_TX  0x700          /* Destination Burst Size Tx */
#define I2C_DMA_BURST_TX   MASK_BIT11     /* Burst Tx */

  /* Shift values for DMA register */
#define I2C_DMA_SHIFT_SBSIZE_RX  0        /* Source Burst Size Rx */
#define I2C_DMA_SHIFT_BURST_RX   3        /* Burst Rx */
#define I2C_DMA_SHIFT_DBSIZE_TX  8        /* Destination Burst Size Tx */
#define I2C_DMA_SHIFT_BURST_TX   11       /* Burst Tx */

/* Baud rate counter registers */
  /* Mask values for Baud rate counter register */
#define I2C_BRCR_BRCNT2  0xFFFF      /* Baud rate counter value for HS mode  */ 
#define I2C_BRCR_BRCNT1  0xFFFF0000  /* Baud rate counter value for Standard and Fast mode*/
#define I2C_FAST_MODE_BRCR_VAL 0x28	 /*Clock to use to work with High Speed Mode*/	

/* Shift values for the Baud rate counter register */
#define I2C_BRCR_SHIFT_BRCNT2    0
#define I2C_BRCR_SHIFT_BRCNT1    16    



/* Interrupt Register  */
   /* Mask values for Interrupt registers */
#define I2C_INT_TXFE      MASK_BIT0       /* Tx fifo empty */
#define I2C_INT_TXFNE     MASK_BIT1       /* Tx Fifo nearly empty */
#define I2C_INT_TXFF      MASK_BIT2       /* Tx Fifo Full */
#define I2C_INT_TXFOVR    MASK_BIT3       /* Tx Fifo over run */
#define I2C_INT_RXFE      MASK_BIT4       /* Rx Fifo Empty */
#define I2C_INT_RXFNF     MASK_BIT5       /* Rx Fifo nearly empty */
#define I2C_INT_RXFF      MASK_BIT6       /* Rx Fifo Full  */
#define I2C_INT_RFSR      MASK_BIT16      /* Read From slave request */
#define I2C_INT_RFSE      MASK_BIT17      /* Read from slave empty */
#define I2C_INT_WTSR      MASK_BIT18      /* Write to Slave request */
#define I2C_INT_MTD       MASK_BIT19      /* Master Transcation Done*/
#define I2C_INT_STD       MASK_BIT20      /* Slave Transaction Done */
#define I2C_INT_MAL       MASK_BIT24      /* Master Arbitation Lost */
#define I2C_INT_BERR      MASK_BIT25      /* Bus Error */
#define I2C_INT_MTDWS     MASK_BIT28      /* Master Transcation Done Without Stop*/
   /* Shift values for Interrupt registers */
#define I2C_INT_SHIFT_TXFE      0               /* Tx fifo empty */
#define I2C_INT_SHIFT_TXFNE     1               /* Tx Fifo nearly empty */
#define I2C_INT_SHIFT_TXFF      2               /* Tx Fifo Full */
#define I2C_INT_SHIFT_TXFOVR    3               /* Tx Fifo over run */
#define I2C_INT_SHIFT_RXFE      4               /* Rx Fifo Empty */
#define I2C_INT_SHIFT_RXFNF     5               /* Rx Fifo nearly empty */
#define I2C_INT_SHIFT_RXFF      6               /* Rx Fifo Full  */
#define I2C_INT_SHIFT_RFSR      16              /* Read From slave request */
#define I2C_INT_SHIFT_RFSE      17              /* Read from slave empty */
#define I2C_INT_SHIFT_WTSR      18              /* Write to Slave request */
#define I2C_INT_SHIFT_MTD       19              /* Master Transcation Done */
#define I2C_INT_SHIFT_STD       20              /* Slave Transaction Done */
#define I2C_INT_SHIFT_MAL       24              /* Master Arbitation Lost */
#define I2C_INT_SHIFT_BERR      25              /* Bus Error */
#define I2C_INT_SHIFT_MTDWS     28              /* Master Transcation Done Without Stop*/

/*Shift Values for the Setup and Hold Times for I2C*/
#define I2C_MIN_DATA_HOLD_TIME          300
#define I2C_THDDAT_MASK                 0x1FF
#define I2C_THDDAT_SHIFT                0
#define I2C_MIN_HOLD_TIME_START_STD     4000
#define I2C_MIN_HOLD_TIME_START_FST     600
#define I2C_MIN_HOLD_TIME_START_FMP     260
#define I2C_MIN_HOLD_TIME_START_HS      160
#define I2C_THDSTA_STD_MASK				0x1FF
#define I2C_THDSTA_FST_MASK				0x1FF0000
#define I2C_THDSTA_FMP_MASK				0x1FF
#define I2C_THDSTA_HS_MASK				0x1FF0000
#define I2C_THDSTA_STD_SHIFT			0
#define I2C_THDSTA_FST_SHIFT			16
#define I2C_THDSTA_FMP_SHIFT			0
#define I2C_THDSTA_HS_SHIFT				16
#define I2C_MIN_SETUP_TIME_START_STD    4700
#define I2C_MIN_SETUP_TIME_START_FST    600
#define I2C_MIN_SETUP_TIME_START_FMP    260
#define I2C_MIN_SETUP_TIME_START_HS     160
#define I2C_TSUSTA_STD_MASK				0x1FF
#define I2C_TSUSTA_FST_MASK				0x1FF0000
#define I2C_TSUSTA_FMP_MASK				0x1FF
#define I2C_TSUSTA_HS_MASK				0x1FF0000
#define I2C_TSUSTA_STD_SHIFT			0
#define I2C_TSUSTA_FST_SHIFT			16
#define I2C_TSUSTA_FMP_SHIFT			0
#define I2C_TSUSTA_HS_SHIFT				16

/*I2C Maximum device id check*/
#if((defined ST_8500ED)||(defined ST_HREFED)||(defined __PEPS_8500))
#define I2C_MAX_ID 				3
#else
#define I2C_MAX_ID				4
#endif

#define I2C_MAX_10_BIT_ADDRESS 		1023
#define I2C_MAX_7_BIT_ADDRESS		127
#define I2C_RESERVED_ADDRESS		8
#define I2C_FUTURE_RESERVED_ADDRESS 119
#define I2C_SW_RESET_MASK			0xFFFFFFFE
#define I2C_MAX_THRESHOLD_VALUE 	15
#define I2C_BUS_ERROR_STATUS		3

#define I2C_MULTIPLE_WRITE 				((t_uint32)I2C_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY |\
                                        (t_uint32) I2C_IRQ_SRC_TRANSMIT_FIFO_OVERRUN |\
                                        (t_uint32) I2C_IRQ_SRC_RECEIVE_FIFO_NEARLY_FULL |\
                                        (t_uint32) I2C_IRQ_SRC_RECEIVE_FIFO_FULL |\
                                        (t_uint32) I2C_IRQ_SRC_READ_FROM_SLAVE_REQUEST |\
                                        (t_uint32) I2C_IRQ_SRC_WRITE_TO_SLAVE_REQUEST |\
                                        (t_uint32) I2C_IRQ_SRC_MASTER_TRANSACTION_DONE |\
                                        (t_uint32) I2C_IRQ_SRC_SLAVE_TRANSACTION_DONE |\
                                        (t_uint32) I2C_IRQ_SRC_MASTER_ARBITRATION_LOST |\
                                        (t_uint32) I2C_IRQ_SRC_BUS_ERROR |\
                                        (t_uint32) I2C_IRQ_SRC_MASTER_TRANSACTION_DONE_WITHOUT_STOP)
										
/*Read interrupt Enable Flags*/
#define I2C_MULTIPLE_READ				((t_uint32)I2C_IRQ_SRC_RECEIVE_FIFO_NEARLY_FULL |\
										(t_uint32) I2C_IRQ_SRC_RECEIVE_FIFO_FULL |\
										(t_uint32) I2C_IRQ_SRC_READ_FROM_SLAVE_REQUEST |\
										(t_uint32) I2C_IRQ_SRC_READ_FROM_SLAVE_EMPTY |\
										(t_uint32) I2C_IRQ_SRC_WRITE_TO_SLAVE_REQUEST |\
										(t_uint32) I2C_IRQ_SRC_MASTER_TRANSACTION_DONE |\
										(t_uint32) I2C_IRQ_SRC_SLAVE_TRANSACTION_DONE |\
										(t_uint32) I2C_IRQ_SRC_MASTER_ARBITRATION_LOST |\
										(t_uint32) I2C_IRQ_SRC_BUS_ERROR |\
										(t_uint32) I2C_IRQ_SRC_MASTER_TRANSACTION_DONE_WITHOUT_STOP)
										
#define I2C_MTD_INTR					((t_uint32)I2C_IRQ_SRC_TRANSMIT_FIFO_EMPTY |\
										(t_uint32) I2C_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY |\
										(t_uint32) I2C_IRQ_SRC_TRANSMIT_FIFO_FULL |\
										(t_uint32) I2C_IRQ_SRC_TRANSMIT_FIFO_OVERRUN)

#define I2C_STD_INTR					((t_uint32) I2C_IRQ_SRC_TRANSMIT_FIFO_EMPTY |\
										(t_uint32) I2C_IRQ_SRC_TRANSMIT_FIFO_NEARLY_EMPTY |\
										(t_uint32) I2C_IRQ_SRC_TRANSMIT_FIFO_FULL |\
										(t_uint32) I2C_IRQ_SRC_TRANSMIT_FIFO_OVERRUN |\
										(t_uint32) I2C_IRQ_SRC_RECEIVE_FIFO_EMPTY |\
										(t_uint32) I2C_IRQ_SRC_RECEIVE_FIFO_NEARLY_FULL |\
										(t_uint32) I2C_IRQ_SRC_RECEIVE_FIFO_FULL)

#define I2C_MTDWS_INTR					I2C_MTD_INTR					
												

#endif /* _I2C_IRQP_H_ */

/* End of file i2c_irqp.h */

