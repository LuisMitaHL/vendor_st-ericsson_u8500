/*
 * Copyright (C) ST-Ericsson SA 2011
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#ifndef _U5500_I2C_H_
#define _U5500_I2C_H_

#include <asm/types.h>
#include <asm/io.h>
#include <asm/errno.h>

#include <asm/arch/common.h>

/* Generic macros */

#define I2C_WRITE_FIELD(reg_name,mask,shift,value) \
                  (reg_name = ((reg_name & ~mask) | (value << shift)))
#define I2C_READ_FIELD(reg_name,mask,shift)    ((reg_name & mask) >> shift )

struct u5500_i2c_regs {
	u32 cr;			/* Control Register                      0x00 */
	u32 scr;		/* Slave Address Register                0x04 */
	u32 hsmcr;		/* HS Master code Register               0x08 */
	u32 mcr;		/* Master Control Register               0x0C */
	u32 tfr;		/* Transmit Fifo Register                0x10 */
	u32 sr;			/* Status Register                       0x14 */
	u32 rfr;		/* Receiver Fifo Register                0x18 */
	u32 tftr;		/* Transmit Fifo Threshold Register      0x1C */
	u32 rftr;		/* Receiver Fifo Threshold Register      0x20 */
	u32 dmar;		/* DMA register                          0x24 */
	u32 brcr;		/* Baud Rate Counter Register            0x28 */
	u32 imscr;		/* Interrupt Mask Set and Clear Register 0x2C */
	u32 risr;		/* Raw interrupt status register         0x30 */
	u32 misr;		/* Masked interrupt status register      0x34 */
	u32 icr;		/* Interrupt Set and Clear Register      0x38 */
	u32 reserved_1[(0xFE0 - 0x3c) >> 2];	/* Reserved 0x03C to 0xFE0 */
	u32 periph_id_0;	/* peripheral ID 0	0xFE0 */
	u32 periph_id_1;	/* peripheral ID 1	0xFE4 */
	u32 periph_id_2;	/* peripheral ID 2	0xFE8 */
	u32 periph_id_3;	/* peripheral ID 3	0xFEC */
	u32 cell_id_0;		/* I2C cell   ID 0	0xFF0 */
	u32 cell_id_1;		/* I2C cell   ID 1	0xFF4 */
	u32 cell_id_2;		/* I2C cell   ID 2	0xFF8 */
	u32 cell_id_3;		/* I2C cell   ID 3	0xFFC */
};


/* Control Register */

/* Mask values for control register mask */
#define I2C_CR_PE          MASK_BIT0	/* Peripheral enable */
#define I2C_CR_OM          0x6		/* Operation mode */
#define I2C_CR_SAM         MASK_BIT3	/* Slave Addressing mode */
#define I2C_CR_SM          0x30		/* Speed mode */
#define I2C_CR_SGCM        MASK_BIT6	/* Slave General call mode */
#define I2C_CR_FTX         MASK_BIT7	/* Flush Transmit */
#define I2C_CR_FRX         MASK_BIT8	/* Flush Receive */
#define I2C_CR_DMA_TX_EN   MASK_BIT9	/* DMA TX Enable */
#define I2C_CR_DMA_RX_EN   MASK_BIT10	/* DMA Rx Enable */
#define I2C_CR_DMA_SLE     MASK_BIT11	/* DMA Synchronization Logic enable */
#define I2C_CR_LM          MASK_BIT12	/* Loop back mode */
#define I2C_CR_FON         0x6000	/* Filtering On */

/* shift valus for control register bit fields */
#define I2C_CR_SHIFT_PE		0	/* Peripheral enable */
#define I2C_CR_SHIFT_OM		1	/* Operation mode */
#define I2C_CR_SHIFT_SAM	3	/* Slave Addressing mode */
#define I2C_CR_SHIFT_SM		4	/* Speed mode */
#define I2C_CR_SHIFT_SGCM	6	/* Slave General call mode */
#define I2C_CR_SHIFT_FTX	7	/* Flush Transmit */
#define I2C_CR_SHIFT_FRX	8	/* Flush Receive */
#define I2C_CR_SHIFT_DMA_TX_EN	9	/* DMA TX Enable */
#define I2C_CR_SHIFT_DMA_RX_EN	10	/* DMA Rx Enable */
#define I2C_CR_SHIFT_DMA_SLE	11	/* DMA Synch Logic enable */
#define I2C_CR_SHIFT_LM		12	/* Loop back mode */
#define I2C_CR_SHIFT_FON	13	/* Filtering On */

/* bus operation modes */
#define I2C_BUS_SLAVE_MODE		0
#define I2C_BUS_MASTER_MODE		1
#define I2C_BUS_MASTER_SLAVE_MODE	2


/* Slave control register*/

/* Mask values slave control register */
#define I2C_SCR_ADDR                   0x3FF
#define I2C_SCR_DATA_SETUP_TIME        0xFFFF0000

/* Shift values for Slave control register */
#define I2C_SCR_SHIFT_ADDR               0
#define I2C_SCR_SHIFT_DATA_SETUP_TIME    16


/* Master Control Register */

/* Mask values for Master control register */
#define I2C_MCR_OP	MASK_BIT0	/* Operation */
#define I2C_MCR_A7	0xFE		/* LSB bits of the Address(7-bit ) */
#define I2C_MCR_EA10	0x700		/* Extended Address */
#define I2C_MCR_SB	MASK_BIT11	/* Start byte procedure */
#define I2C_MCR_AM	0x3000		/* Address type */
#define I2C_MCR_STOP	MASK_BIT14	/* stop condition */
#define I2C_MCR_LENGTH	0x3FF8000	/* Frame length */
#define I2C_MCR_A10	0x7FE		/* Define to set the 10 bit address */
/* mask for length field,stop and operation  */
#define I2C_MCR_LENGTH_STOP_OP	0x3FFC001

/* Shift values for Master control values */
#define I2C_MCR_SHIFT_OP	0	/* Operation */
#define I2C_MCR_SHIFT_A7	1	/* LSB bits of the Address(7-bit ) */
#define I2C_MCR_SHIFT_EA10	8	/* Extended Address */
#define I2C_MCR_SHIFT_SB	11	/* Start byte procedure */
#define I2C_MCR_SHIFT_AM	12	/* Address type */
#define I2C_MCR_SHIFT_STOP	14	/* stop condition */
#define I2C_MCR_SHIFT_LENGTH	15	/* Frame length */
#define I2C_MCR_SHIFT_A10	1	/* define to set the 10 bit addres */

#define I2C_MCR_SHIFT_LENGTH_STOP_OP	0


/* Status Register */

/* Mask values for Status register */
#define I2C_SR_OP		0x3	/* Operation */
#define I2C_SR_STATUS		0xC	/* Controller Status */
#define I2C_SR_CAUSE		0x70	/* Abort Cause */
#define I2C_SR_TYPE		0x180	/* Receive Type */
#define I2C_SR_LENGTH		0xFF700	/* Transfer length */

/* Shift values for Status register */
#define I2C_SR_SHIFT_OP		0	/* Operation */
#define I2C_SR_SHIFT_STATUS	2	/* Controller Status */
#define I2C_SR_SHIFT_CAUSE	4	/* Abort Cause */
#define I2C_SR_SHIFT_TYPE	7	/* Receive Type */
#define I2C_SR_SHIFT_LENGTH	9	/* Transfer length */

/* abort cause */
enum i2c_abort_cause {
	I2C_NACK_ADDR	= 0,
	I2C_NACK_DATA	= 1,
	I2C_ACK_MCODE	= 2,
	I2C_ARB_LOST	= 3,
	I2C_BERR_START	= 4,
	I2C_BERR_STOP	= 5,
	I2C_OVFL	= 6
};


/* Baud rate counter registers */

/* Mask values for Baud rate counter register */
#define I2C_BRCR_BRCNT2	0xFFFF		/* Baud Rate Counter for HS mode */
#define I2C_BRCR_BRCNT1	0xFFFF0000	/* counter for Standard and Fast mode */

/* Shift values for the Baud rate counter register */
#define I2C_BRCR_SHIFT_BRCNT2	0
#define I2C_BRCR_SHIFT_BRCNT1	16


/* Interrupt Register  */

/* Mask values for Interrupt registers */
#define I2C_INT_TXFE	MASK_BIT0	/* Tx fifo empty */
#define I2C_INT_TXFNE	MASK_BIT1	/* Tx Fifo nearly empty */
#define I2C_INT_TXFF	MASK_BIT2	/* Tx Fifo Full */
#define I2C_INT_TXFOVR	MASK_BIT3	/* Tx Fifo over run */
#define I2C_INT_RXFE	MASK_BIT4	/* Rx Fifo Empty */
#define I2C_INT_RXFNF	MASK_BIT5	/* Rx Fifo nearly empty */
#define I2C_INT_RXFF	MASK_BIT6	/* Rx Fifo Full  */
#define I2C_INT_RFSR	MASK_BIT16	/* Read From slave request */
#define I2C_INT_RFSE	MASK_BIT17	/* Read from slave empty */
#define I2C_INT_WTSR	MASK_BIT18	/* Write to Slave request */
#define I2C_INT_MTD	MASK_BIT19	/* Master Transcation Done*/
#define I2C_INT_STD	MASK_BIT20	/* Slave Transaction Done */
#define I2C_INT_MAL	MASK_BIT24	/* Master Arbitation Lost */
#define I2C_INT_BERR	MASK_BIT25	/* Bus Error */
#define I2C_INT_MTDWS	MASK_BIT28	/* Master Transaction Done wo/ Stop */

/* Shift values for Interrupt registers */
#define I2C_INT_SHIFT_TXFE	0	/* Tx fifo empty */
#define I2C_INT_SHIFT_TXFNE	1	/* Tx Fifo nearly empty */
#define I2C_INT_SHIFT_TXFF	2	/* Tx Fifo Full */
#define I2C_INT_SHIFT_TXFOVR	3	/* Tx Fifo over run */
#define I2C_INT_SHIFT_RXFE	4	/* Rx Fifo Empty */
#define I2C_INT_SHIFT_RXFNF	5	/* Rx Fifo nearly empty */
#define I2C_INT_SHIFT_RXFF	6	/* Rx Fifo Full  */
#define I2C_INT_SHIFT_RFSR	16	/* Read From slave request */
#define I2C_INT_SHIFT_RFSE	17	/* Read from slave empty */
#define I2C_INT_SHIFT_WTSR	18	/* Write to Slave request */
#define I2C_INT_SHIFT_MTD	19	/* Master Transcation Done */
#define I2C_INT_SHIFT_STD	20	/* Slave Transaction Done */
#define I2C_INT_SHIFT_MAL	24	/* Master Arbitation Lost */
#define I2C_INT_SHIFT_BERR	25	/* Bus Error */
#define I2C_INT_SHIFT_MTDWS	28	/* Master Transcation Done wo/ Stop */


/* Misc defines */
#define I2C_MAX_STANDARD_SCL	100000	/* Max clock (Hz) for Standard Mode */
#define I2C_MAX_FAST_SCL	400000	/* Max clock (Hz) for Fast Mode */
#define I2C_MAX_HIGH_SPEED_SCL	3400000	/* Max clock (Hz) for HS Mode */

#endif	/* _U5500_I2C_H_ */
