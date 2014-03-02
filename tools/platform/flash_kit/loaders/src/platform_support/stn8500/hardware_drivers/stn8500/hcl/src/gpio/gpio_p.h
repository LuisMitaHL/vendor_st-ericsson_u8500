/*****************************************************************************/
/**
*  � ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Private Header file of General Purpose Input/Output (GPIO) module
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef __INC_GPIOP_H
#define __INC_GPIOP_H

#include "gpio.h"
#include "gpio_irqp.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*-----------------------------------------------------------------------------
 * Defines
 *---------------------------------------------------------------------------*/
#define GPIO_SHIFT8     0x08
#define GPIO_SHIFT16    0x10
#define GPIO_SHIFT24    0x18
#define GPIO_8BIT_MASK  0xFF
#define GPIO_12BIT_MASK 0xFFF
#define GPIO_16BIT_MASK 0xFFFF
#define GPIO_32BIT_MASK 0xFFFFFFFF
#define GPIO_8BIT_HIGH  0xFF
#define GPIO_8BIT_LOW   0x00


	/* Peripheral Ids and PCell Ids */
	#define GPIO_PERIPH_ID_0    0x60
	#define GPIO_PERIPH_ID_1    0x00
	#define GPIO_PERIPH_ID_2    0x38
	#define GPIO_PERIPH_ID_3    0x1F
	#define GPIO_PCELL_ID_0     0x0D
	#define GPIO_PCELL_ID_1     0xF0
	#define GPIO_PCELL_ID_2     0x05
	#define GPIO_PCELL_ID_3     0xB1

	/*Alternate function mask */
	#define GPIO_SSP0_MASK              0x00078000
	#define GPIO_SSP1_MASK              0x00007800
	
	#define GPIO_I2C0_MASK              0x00180000
	#define GPIO_I2C2_MASK1             0x00000300
	#define GPIO_I2C2_MASK2             0x00000C00
	#define GPIO_I2C1_MASK1             0x000000C0
	#define GPIO_I2C1_MASK2             0x00030000
	#define GPIO_I2C3_MASK1	   	        0x00000060
    #define GPIO_I2C3_MASK2	   	        0x05000000
    #define GPIO_I2C4_MASK				0x00000030
	
	#define GPIO_MMC0_MASK1	            0xFFFC0000
	#define GPIO_MMC0_MASK2             0x00000001
	#define GPIO_MMC1_MASK1             0x07FF0000
	#define GPIO_MMC2_MASK1             0x000007FF
	#define GPIO_MMC3_MASK1             0xFF800000
	#define GPIO_MMC3_MASK2             0x00000003
	#define GPIO_MMC4_MASK1             0x0000FFE0
	#define GPIO_MMC5_MASK1             0x3FC00000
	#define GPIO_MMC5_MASK2             0x00000003
		
	#define GPIO_MSP0_MASK1             0x0023F000
	#define GPIO_MSP1_MASK              0x0000001E
	#define GPIO_MSP2_MASK              0x0000001F
	
	#define GPIO_UART0_MASK1            0x0000000F
	#define GPIO_UART0_MASK2            0x0000001E
	#define GPIO_UART1_MASK1            0x000000F0
#ifdef STE_AP8540
	#define GPIO_UART2_MASK1            0x03000000
#else
	#define GPIO_UART2_MASK1            0xE0000000
#endif
	#define GPIO_UART2_MASK2            0x00000001

	#define GPIO_IRDA_MASK1				0x00820000
	#define GPIO_IRDA_MASK2				0x00600000
	
	#define GPIO_LCD_PANEL_MASK1		0x003FFFCF
	#define GPIO_LCD_PANEL_MASK2		0xFE800000
	#define GPIO_LCD_PANEL_MASK3		0x00000FFF
	
	#define GPIO_TSP_MASK               0x001F0000
	
	#define GPIO_KEYPAD_MASK1			0x00003FFF
	#define GPIO_KEYPAD_MASK2			0x00001800
	#define GPIO_KEYPAD_MASK3           0x00000C00
	
	#define GPIO_USB_OTG_MASK           0x00000FFF

	#define GPIO_HSIT0_MASK_1			0xC0000000
	#define GPIO_HSIT0_MASK_2			0x00000007
	#define GPIO_HSIR0_MASK 			0x38000000
		
	#define GPIO_NAND_MASK1             0x80000030
	#define GPIO_NAND_MASK2             0x7FC00000
	#define GPIO_NAND_MASK3             0x00000003
	#define GPIO_NAND_MASK4             0x000007FF
	
	#define GPIO_NOR_MASK1              0x00000030
	#define GPIO_NOR_MASK2             	0x7FC00000
	#define GPIO_NOR_MASK3             	0x00000003
	#define GPIO_NOR_MASK4             	0x000007FF
	
	#define GPIO_SPI0_MASK1				0x90000000 
	#define GPIO_SPI0_MASK2				0x00000003 
	#define GPIO_SPI1_MASK              0x00720000 
	#define GPIO_SPI3_MASK1  			0xE0000000 
	#define GPIO_SPI3_MASK2				0x00000001 
	
	#define GPIO_STMAPE_MASK1           0x000007C0  
	#define GPIO_STMAPE_MASK2           0xF8000000  
	

typedef struct
{
    t_gpio_register *p_gpio_register[GPIO_BLOCKS_COUNT];
} t_gpio_system_context;

#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* __INC_GPIOP_H */

/* End of file - gpiop.h */

