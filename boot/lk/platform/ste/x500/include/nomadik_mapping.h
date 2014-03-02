/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/*------------------------------------------------------------------------
 * 	                             TPA / MPU
 *                   MultiMedia Technology Center (MMTC)
 *------------------------------------------------------------------------
 * Declaration of all available addresses for each hardware platform used.
 *
 * !!! WARNING, IMPORTANT ...
 * The limits contained within this file are real used addresses: These are more
 * restrictive than those from the memory map given in the nomadik full documentation.
 * ... WARNING, IMPORTANT !!!
 *------------------------------------------------------------------------
 *
 ******************************************************************************/


#ifndef _NOMADIK_MAPPING_H
#define _NOMADIK_MAPPING_H


#define CNTRL_REG_SIZE					0x00001000
#define GPIO_REG_SIZE					0x00000080
#ifndef PAGE_SIZE
#define PAGE_SIZE                       0x00001000
#endif

/* ------------------------------------------------------------ */
/* STN_8500 address mapping                                     */
/* ------------------------------------------------------------ */

	// SDRAM chip select 0
	#define SDRAM0_START_ADDR			0x00000000
	#define SDRAM0_LENGTH				0x20000000
	#define SDRAM0_END_ADDR				(SDRAM0_START_ADDR+SDRAM0_LENGTH-1)

	// SDRAM chip select 1
	#define SDRAM1_START_ADDR			0x20000000
	#define SDRAM1_LENGTH				0x20000000
	#define SDRAM1_END_ADDR				(SDRAM1_START_ADDR+SDRAM1_LENGTH-1)

	// SDRAM chip select 0
	#define SDRAM0_ALIAS_START_ADDR		0xC0000000
	#define SDRAM0_ALIAS_LENGTH			0x20000000
	#define SDRAM0_ALIAS_END_ADDR		(SDRAM0_ALIAS_START_ADDR+SDRAM0_ALIAS_LENGTH-1)

	// SDRAM chip select 1
	#define SDRAM1_ALIAS_START_ADDR		0xE0000000
	#define SDRAM1_ALIAS_LENGTH			0x20000000
	#define SDRAM1_ALIAS_END_ADDR		(SDRAM1_ALIAS_START_ADDR+SDRAM1_ALIAS_LENGTH-1)

	/*************************/
	/* Embedded buffer eSRAM */
	#define BOOT_RAM_START_ADDR			0x40000000
	#define BOOT_RAM_LENGTH				0x000A0000
	#define	BOOT_RAM_END_ADDR			(BOOT_RAM_START_ADDR+BOOT_RAM_LENGTH-1)

	/* Embedded buffer eSRAM */
	/*************************/

    // Embedded RAM bank 4
	#define BOOT_RAM_BANK4_START_ADDR	0x40080000
	#define BOOT_RAM_BANK_LENGTH		0x00020000
	#define BOOT_RAM_BANK4_END_ADDR		(BOOT_RAM_BANK4_START_ADDR+BOOT_RAM_BANK_LENGTH-1)

    // Embedded RAM bank 3
	#define BOOT_RAM_BANK3_START_ADDR	0x40060000
	#define BOOT_RAM_BANK_LENGTH		0x00020000
	#define BOOT_RAM_BANK3_END_ADDR		(BOOT_RAM_BANK3_START_ADDR+BOOT_RAM_BANK_LENGTH-1)

    // Embedded RAM bank 2
	#define BOOT_RAM_BANK2_START_ADDR	0x40040000
	#define BOOT_RAM_BANK_LENGTH		0x00020000
	#define BOOT_RAM_BANK2_END_ADDR		(BOOT_RAM_BANK2_START_ADDR+BOOT_RAM_BANK_LENGTH-1)

    // Embedded RAM bank 1
	#define BOOT_RAM_BANK1_START_ADDR	0x40020000
	#define BOOT_RAM_BANK_LENGTH		0x00020000
	#define BOOT_RAM_BANK1_END_ADDR		(BOOT_RAM_BANK1_START_ADDR+BOOT_RAM_BANK_LENGTH-1)

    // Embedded RAM bank 0
	#define BOOT_RAM_BANK0_START_ADDR	0x40000000
	#define BOOT_RAM_BANK_LENGTH		0x00020000
	#define BOOT_RAM_BANK0_END_ADDR		(BOOT_RAM_BANK0_START_ADDR+BOOT_RAM_BANK_LENGTH-1)

	/**************/
	/* Secure RAM */
	#define SEC_RAM_START_ADDR          BOOT_RAM_BANK0_START_ADDR
	#define SEC_RAM_LENGTH				0x00010000
	#define	SEC_RAM_END_ADDR			(SEC_RAM_START_ADDR+SEC_RAM_LENGTH-1)


    //0x400C0000 Undefined

	// NOR Flash Chip Select 0
	#define NOR_FLASH_CS0_START_ADDR	0x50000000
	#define NOR_FLASH_CS0_LENGTH		0x04000000
	#define NOR_FLASH_CS0_END_ADDR		(NOR_FLASH_CS0_START_ADDR+NOR_FLASH_CS0_LENGTH-1)

	// NOR Flash Chip Select 1
	#define NOR_FLASH_CS1_START_ADDR	0x54000000
	#define NOR_FLASH_CS1_LENGTH		0x04000000
	#define NOR_FLASH_CS1_END_ADDR		(NOR_FLASH_CS1_START_ADDR+NOR_FLASH_CS1_LENGTH-1)

    // 0x58000000 Undefined

    // NAND Flash Chip Select 0
	#define NAND_FLASH_CS0_START_ADDR	0x60000000
	#define NAND_FLASH_CS0_LENGTH		0x10000000
	#define NAND_FLASH_CS0_END_ADDR		(NAND_FLASH_CS0_START_ADDR+NAND_FLASH_CS0_LENGTH-1)

    // NAND Flash Chip Select 1
	#define NAND_FLASH_CS1_START_ADDR	0x70000000
	#define NAND_FLASH_CS1_LENGTH		0x10000000
	#define NAND_FLASH_CS1_END_ADDR		(NAND_FLASH_CS1_START_ADDR+NAND_FLASH_CS1_LENGTH-1)

//--------------------------------------------------------------------
// Global STBus peripheral definitions
//--------------------------------------------------------------------
    // STM
	#define STM_REG_START_ADDR		    0x80100000
	#define STM_REG_LENGTH			    0x00010000
	#define STM_REG_END_ADDR		    (STM_REG_START_ADDR+STM_REG_LENGTH-1)

	// B2R2 configuration registers
	#define B2R2_REG_START_ADDR			0x80130000
	#define B2R2_REG_LENGTH				0x00010000
	#define B2R2_REG_END_ADDR			(B2R2_REG_START_ADDR+B2R2_REG_LENGTH-1)

    // Hardware semaphore
	#define HSEM_REG_START_ADDR			0x80140000
	#define HSEM_REG_LENGTH				CNTRL_REG_SIZE
	#define HSEM_REG_END_ADDR			(HSEM_REG_START_ADDR+HSEM_REG_LENGTH-1)

    // Main ICN Crossbar configuration registers
	#define ICN_REG_START_ADDR			0x81000000
	#define ICN_REG_LENGTH				0x00000430
	#define ICN_REG_END_ADDR			(ICN_REG_START_ADDR+ICN_REG_LENGTH-1)

	// SVA Memory space
	#define SVA_MEM_START_ADDR			0xA0100000
	#define SVA_MEM_LENGTH				(256 * PAGE_SIZE)
	#define SVA_MEM_END_ADDR			(SVA_MEM_START_ADDR+SVA_MEM_LENGTH-1)

    // SIA Memory space
	#define SIA_MEM_START_ADDR			0xA0200000
	#define SIA_MEM_LENGTH				(256 * PAGE_SIZE)
	#define SIA_MEM_END_ADDR			(SIA_MEM_START_ADDR+SIA_MEM_LENGTH-1)

	// SGA configuration registers
	#define SGA_REG_START_ADDR			0xA0300000
	#define SGA_REG_LENGTH				(32 * PAGE_SIZE)
	#define SGA_REG_END_ADDR			(SGA_REG_START_ADDR+SGA_REG_LENGTH-1)

	// UNIPRO
	#define UNIPRO_START_ADDR			0xA0320000
	#define UNIPRO_LENGTH				(32 * PAGE_SIZE)
	#define UNIPRO_END_ADDR 			(UNIPRO_START_ADDR+UNIPRO_LENGTH-1)

    //G12 ??
	#define G12_REG_START_ADDR			0xA0320000
	#define G12_REG_LENGTH				CNTRL_REG_SIZE
	#define G12_REG_END_ADDR			(G12_REG_START_ADDR+G12_REG_LENGTH-1)

	// DISPLAY controller
    #define DISPLAY_REG_START_ADDR			0xA0350000
	#define DISPLAY_REG_LENGTH				CNTRL_REG_SIZE
	#define DISPLAY_REG_END_ADDR			(DISPLAY_REG_START_ADDR+DISPLAY_REG_LENGTH-1)

	//DSI1
	#define DSI1_REG_START_ADDR			0xA0351000
	#define DSI1_REG_LENGTH				CNTRL_REG_SIZE
	#define DSI1_REG_END_ADDR			(DSI1_REG_START_ADDR+DSI1_REG_LENGTH-1)

    //DSI2
	#define DSI2_REG_START_ADDR			0xA0352000
	#define DSI2_REG_LENGTH				CNTRL_REG_SIZE
	#define DSI2_REG_END_ADDR			(DSI2_REG_START_ADDR+DSI2_REG_LENGTH-1)

    //DSI3
	#define DSI3_REG_START_ADDR			0xA0353000
	#define DSI3_REG_LENGTH				CNTRL_REG_SIZE
	#define DSI3_REG_END_ADDR			(DSI3_REG_START_ADDR+DSI3_REG_LENGTH-1)

	// DMA controller
	#define DMA_REG_START_ADDR			0x801C0000
	#define DMA_REG_LENGTH				CNTRL_REG_SIZE
	#define DMA_REG_END_ADDR			(DMA_REG_START_ADDR+DMA_REG_LENGTH-1)

    // SBAG configuration registers
	#define SBAG_REG_START_ADDR			0xA0390000
	#define SBAG_REG_LENGTH				0x00010000
	#define SBAG_REG_END_ADDR			(SBAG_REG_START_ADDR+SBAG_REG_LENGTH-1)

    //SCU
    #define SCU_REG_START_ADDR          0xA0410000
    #define SCU_REG_LENGTH				0x00000100
	#define SCU_REG_END_ADDR			(SCU_REG_START_ADDR+SCU_REG_LENGTH-1)

    // GIC controller
    #define GIC_REG_START_ADDR          0xA0410100
    #define GIC_REG_LENGTH				0x00001F00
	#define GIC_REG_END_ADDR			(GIC_REG_START_ADDR+GIC_REG_LENGTH-1)

    // A9 priv periph
    #define A9_PRIV_PERIPH_START_ADDR          0xA0410600
    #define A9_PRIV_PERIPH_REG_LENGTH		   0x0000100
	#define A9_PRIV_PERIPH_END_ADDR			(A9_PRIV_PERIPH_START_ADDR+A9_PRIV_PERIPH_REG_LENGTH-1)

    // Level 2 Cache configuration registers (L220)//To become L310 0xA0414000 or 0xA0422000 ??
	#define L2CC_REG_START_ADDR			0xA0412000
	#define L2CC_REG_LENGTH				CNTRL_REG_SIZE
	#define L2CC_REG_END_ADDR			(L2CC_REG_START_ADDR+L2CC_REG_LENGTH-1)


//--------------------------------------------------------------------
// DMA periph1 peripheral definitions
//--------------------------------------------------------------------

    // UART 0
	#define UART0_REG_START_ADDR		0x80120000
	#define UART0_REG_LENGTH			CNTRL_REG_SIZE
	#define UART0_REG_END_ADDR			(UART0_REG_START_ADDR+UART0_REG_LENGTH-1)

	// UART 1
	#define UART1_REG_START_ADDR		0x80121000
	#define UART1_REG_LENGTH			CNTRL_REG_SIZE
	#define UART1_REG_END_ADDR			(UART1_REG_START_ADDR+UART1_REG_LENGTH-1)

	// I2C1
	#define I2C1_REG_START_ADDR			0x80122000
	#define I2C1_REG_LENGTH				CNTRL_REG_SIZE
	#define I2C1_REG_END_ADDR			(I2C1_REG_START_ADDR+I2C1_REG_LENGTH-1)

	// MSP0
	#define MSP0_REG_START_ADDR			0x80123000
	#define MSP0_REG_LENGTH				CNTRL_REG_SIZE
	#define MSP0_REG_END_ADDR			(MSP0_REG_START_ADDR+MSP0_REG_LENGTH-1)

	// MSP1
	#define MSP1_REG_START_ADDR			0x80124000
	#define MSP1_REG_LENGTH				CNTRL_REG_SIZE
	#define MSP1_REG_END_ADDR			(MSP1_REG_START_ADDR+MSP1_REG_LENGTH-1)

    // SDI0 SD/MMC card interface
	#define SDI0_REG_START_ADDR			0x80126000
	#define SDI0_REG_LENGTH				CNTRL_REG_SIZE
	#define SDI0_REG_END_ADDR			(SDI0_REG_START_ADDR+SDI0_REG_LENGTH-1)

	// I2C2
	#define I2C2_REG_START_ADDR			0x80128000
	#define I2C2_REG_LENGTH				CNTRL_REG_SIZE
	#define I2C2_REG_END_ADDR			(I2C2_REG_START_ADDR+I2C2_REG_LENGTH-1)

    // SPI3
	#define SPI3_REG_START_ADDR			0x80129000
	#define SPI3_REG_LENGTH				CNTRL_REG_SIZE
	#define SPI3_REG_END_ADDR			(SPI3_REG_START_ADDR+SPI3_REG_LENGTH-1)

    //0x8012A000

    // SLIM0
	#define SLIM0_REG_START_ADDR		0x8012B000
	#define SLIM0_REG_LENGTH			CNTRL_REG_SIZE
	#define SLIM0_REG_END_ADDR			(SLIM0_REG_START_ADDR+SLIM0_REG_LENGTH-1)

	//0x8012C000

	// Memory stick host controller
	#define MSHC_REG_START_ADDR			0x8012D000
	#define MSHC_REG_LENGTH				CNTRL_REG_SIZE
	#define MSHC_REG_END_ADDR			(MSHC_REG_START_ADDR+MSHC_REG_LENGTH-1)

	// General Purpose I/Os 0 to 31
	#define GPIO0_REG_START_ADDR		0x8012E000 //GPIO cluster 0 and 1
	#define GPIO0_REG_LENGTH			GPIO_REG_SIZE
	#define GPIO0_REG_END_ADDR			(GPIO0_REG_START_ADDR+GPIO0_REG_LENGTH-1)

	// General Purpose I/Os 32 to 63
	#define GPIO1_REG_START_ADDR		0x8012E080 //0x8012E062
	#define GPIO1_REG_LENGTH			GPIO_REG_SIZE
	#define GPIO1_REG_END_ADDR			(GPIO1_REG_START_ADDR+GPIO1_REG_LENGTH-1)

    // CLK_RST1
	#define CLK_RST1_REG_START_ADDR			0x8012F000
	#define CLK_RST1_REG_LENGTH				CNTRL_REG_SIZE
	#define CLK_RST1_REG_END_ADDR			(CLK_RST1_REG_START_ADDR+CLK_RST1_REG_LENGTH-1)

//--------------------------------------------------------------------
// DMA periph2 peripheral definitions
//--------------------------------------------------------------------

	// I2C3
	#define I2C3_REG_START_ADDR			0x80110000
	#define I2C3_REG_LENGTH				CNTRL_REG_SIZE
	#define I2C3_REG_END_ADDR			(I2C3_REG_START_ADDR+I2C3_REG_LENGTH-1)

    // SPI2
	#define SPI2_REG_START_ADDR			0x80111000
	#define SPI2_REG_LENGTH				CNTRL_REG_SIZE
	#define SPI2_REG_END_ADDR			(SPI2_REG_START_ADDR+SPI2_REG_LENGTH-1)

    // SPI1
	#define SPI1_REG_START_ADDR			0x80112000
	#define SPI1_REG_LENGTH				CNTRL_REG_SIZE
	#define SPI1_REG_END_ADDR			(SPI1_REG_START_ADDR+SPI1_REG_LENGTH-1)

    // PWL
	#define PWL_REG_START_ADDR			0x80113000
	#define PWL_REG_LENGTH				CNTRL_REG_SIZE
	#define PWL_REG_END_ADDR			(PWL_REG_START_ADDR+PWL_REG_LENGTH-1)

    // SDI4 SD/MMC card interface
	#define SDI4_REG_START_ADDR			0x80114000
	#define SDI4_REG_LENGTH				CNTRL_REG_SIZE
	#define SDI4_REG_END_ADDR			(SDI4_REG_START_ADDR+SDI4_REG_LENGTH-1)

    // MSP2
	#define MSP2_REG_START_ADDR			0x80117000
	#define MSP2_REG_LENGTH				CNTRL_REG_SIZE
	#define MSP2_REG_END_ADDR			(MSP2_REG_START_ADDR+MSP2_REG_LENGTH-1)

    // SDI1 SD/MMC card interface
	#define SDI1_REG_START_ADDR			0x80118000
	#define SDI1_REG_LENGTH				CNTRL_REG_SIZE
	#define SDI1_REG_END_ADDR			(SDI1_REG_START_ADDR+SDI1_REG_LENGTH-1)

    // SDI3 SD/MMC card interface
	#define SDI3_REG_START_ADDR			0x80119000
	#define SDI3_REG_LENGTH				CNTRL_REG_SIZE
	#define SDI3_REG_END_ADDR			(SDI3_REG_START_ADDR+SDI3_REG_LENGTH-1)

    // SPI0
	#define SPI0_REG_START_ADDR			0x8011A000
	#define SPI0_REG_LENGTH				CNTRL_REG_SIZE
	#define SPI0_REG_END_ADDR			(SPI0_REG_START_ADDR+SPI0_REG_LENGTH-1)

	// SSI Receive 0 & 1
	#define SSIRX_REG_START_ADDR		0x8011B000
	#define SSIRX_REG_LENGTH			CNTRL_REG_SIZE
	#define SSIRX_REG_END_ADDR			(SSIRX_REG_START_ADDR+SSIRX_REG_LENGTH-1)

	// SSI transmit 0 & 1
	#define SSITX_REG_START_ADDR		0x8011C000
	#define SSITX_REG_LENGTH			CNTRL_REG_SIZE
	#define SSITX_REG_END_ADDR			(SSITX_REG_START_ADDR+SSITX_REG_LENGTH-1)

    // General Purpose I/Os 192 to 223
	#define GPIO6_REG_START_ADDR		0x8011E000
	#define GPIO6_REG_LENGTH			GPIO_REG_SIZE
	#define GPIO6_REG_END_ADDR			(GPIO6_REG_START_ADDR+GPIO6_REG_LENGTH-1)

    // General Purpose I/Os 224 to 255
	#define GPIO7_REG_START_ADDR		0x8011E080
	#define GPIO7_REG_LENGTH			GPIO_REG_SIZE
	#define GPIO7_REG_END_ADDR			(GPIO7_REG_START_ADDR+GPIO7_REG_LENGTH-1)

    // CLK_RST2
	#define CLK_RST2_REG_START_ADDR			0x8011F000
	#define CLK_RST2_REG_LENGTH				CNTRL_REG_SIZE
	#define CLK_RST2_REG_END_ADDR			(CLK_RST2_REG_START_ADDR+CLK_RST2_REG_LENGTH-1)

//--------------------------------------------------------------------
// DMA periph3 peripheral definitions
//--------------------------------------------------------------------

	// FSMC controller
	#define FSMC_REG_START_ADDR			0x80000000
	#define FSMC_REG_LENGTH				CNTRL_REG_SIZE
	#define FSMC_REG_END_ADDR			(FSMC_REG_START_ADDR+FSMC_REG_LENGTH-1)

	// SSP0
	#define SSP0_REG_START_ADDR			0x80002000
	#define SSP0_REG_LENGTH				CNTRL_REG_SIZE
	#define SSP0_REG_END_ADDR			(SSP0_REG_START_ADDR+SSP0_REG_LENGTH-1)

	// SSP1
	#define SSP1_REG_START_ADDR			0x80003000
	#define SSP1_REG_LENGTH				CNTRL_REG_SIZE
	#define SSP1_REG_END_ADDR			(SSP1_REG_START_ADDR+SSP1_REG_LENGTH-1)

	// I2C0
	#define I2C0_REG_START_ADDR			0x80004000
	#define I2C0_REG_LENGTH				CNTRL_REG_SIZE
	#define I2C0_REG_END_ADDR			(I2C0_REG_START_ADDR+I2C0_REG_LENGTH-1)

    // SDI2 SD/MMC card interface
	#define SDI2_REG_START_ADDR			0x80005000
	#define SDI2_REG_LENGTH				CNTRL_REG_SIZE
	#define SDI2_REG_END_ADDR			(SDI2_REG_START_ADDR+SDI2_REG_LENGTH-1)

    // KBD+SKE Scroll key and key board encoder
	#define SKE_REG_START_ADDR			0x80006000
	#define SKE_REG_LENGTH				CNTRL_REG_SIZE
	#define SKE_REG_END_ADDR			(SKE_REG_START_ADDR+SKE_REG_LENGTH-1)

    // UART 2
	#define UART2_REG_START_ADDR		0x80007000
	#define UART2_REG_LENGTH			CNTRL_REG_SIZE
	#define UART2_REG_END_ADDR			(UART2_REG_START_ADDR+UART2_REG_LENGTH-1)

    // SDI5 SD/MMC card interface
	#define SDI5_REG_START_ADDR			0x80008000
	#define SDI5_REG_LENGTH				CNTRL_REG_SIZE
	#define SDI5_REG_END_ADDR			(SDI5_REG_START_ADDR+SDI5_REG_LENGTH-1)

	// General Purpose I/Os 64 to 95
	#define GPIO2_REG_START_ADDR		0x8000E000
	#define GPIO2_REG_LENGTH			GPIO_REG_SIZE
	#define GPIO2_REG_END_ADDR			(GPIO2_REG_START_ADDR+GPIO2_REG_LENGTH-1)

	// General Purpose I/Os 96 to 127
	#define GPIO3_REG_START_ADDR		0x8000E080
	#define GPIO3_REG_LENGTH			GPIO_REG_SIZE
	#define GPIO3_REG_END_ADDR			(GPIO3_REG_START_ADDR+GPIO3_REG_LENGTH-1)

	// General Purpose I/Os 128 to 159
	#define GPIO4_REG_START_ADDR		0x8000E100
	#define GPIO4_REG_LENGTH			GPIO_REG_SIZE
	#define GPIO4_REG_END_ADDR			(GPIO4_REG_START_ADDR+GPIO4_REG_LENGTH-1)

	// General Purpose I/Os 160 to 191
	#define GPIO5_REG_START_ADDR		0x8000E180
	#define GPIO5_REG_LENGTH			GPIO_REG_SIZE
	#define GPIO5_REG_END_ADDR			(GPIO5_REG_START_ADDR+GPIO5_REG_LENGTH-1)

    // CLK_RST3
	#define CLK_RST3_REG_START_ADDR		0x8000F000
	#define CLK_RST3_REG_LENGTH			CNTRL_REG_SIZE
	#define CLK_RST3_REG_END_ADDR		(CLK_RST3_REG_START_ADDR+CLK_RST3_REG_LENGTH-1)

//--------------------------------------------------------------------
// DMA periph4 Private peripheral definitions
//--------------------------------------------------------------------

	// Secure Back up SRAM
	#define SEC_BACKUP_RAM_START_ADDR	0x80150000
	#define SEC_BACKUP_RAM_LENGTH		0x00001C00
	#define SEC_BACKUP_RAM_END_ADDR		(SEC_BACKUP_RAM_START_ADDR+SEC_BACKUP_RAM_LENGTH-1)

	// Back up SRAM
	#define BACKUP_RAM_START_ADDR		0x80151C00
	#define BACKUP_RAM_LENGTH			0x00000400
	#define BACKUP_RAM_END_ADDR			(BACKUP_RAM_START_ADDR+BACKUP_RAM_LENGTH-1)

	// Real time timer 0
	#define RTT0_REG_START_ADDR			0x80152000
	#define RTT0_REG_LENGTH				CNTRL_REG_SIZE
	#define RTT0_REG_END_ADDR			(RTT0_REG_START_ADDR+RTT0_REG_LENGTH-1)

	// Real time timer 1
	#define RTT1_REG_START_ADDR			0x80153000
	#define RTT1_REG_LENGTH				CNTRL_REG_SIZE
	#define RTT1_REG_END_ADDR			(RTT1_REG_START_ADDR+RTT1_REG_LENGTH-1)

    // Real time clock
	#define RTC_REG_START_ADDR			0x80154000
	#define RTC_REG_LENGTH				CNTRL_REG_SIZE
	#define RTC_REG_END_ADDR			(RTC_REG_START_ADDR+RTC_REG_LENGTH-1)

	// SECURE CONTROL REGISTERS (SCR) / eFUSE
	#define SEC_REG_START_ADDR			0x80155000
	#define SEC_REG_LENGTH				CNTRL_REG_SIZE
	#define SEC_REG_END_ADDR			(SEC_REG_START_ADDR+SEC_REG_LENGTH-1)

	// DBAHN Dynamic memory controller (DMC base)
	#define SDMC_REG_START_ADDR			0x80156000
	#define SDMC_REG_LENGTH				CNTRL_REG_SIZE
	#define SDMC_REG_END_ADDR			(SDMC_REG_START_ADDR+SDMC_REG_LENGTH-1)

	// Power management unit PRCMU
	#define PRCMU_REG_START_ADDR		0x80157000
	#define PRCMU_REG_LENGTH			CNTRL_REG_SIZE
	#define PRCMU_REG_END_ADDR			(PRCMU_REG_START_ADDR+PRCMU_REG_LENGTH-1)

    // XP70 programm
	#define XP70P_REG_START_ADDR		0x801B0000
	#define XP70P_REG_LENGTH			CNTRL_REG_SIZE
	#define XP70P_REG_END_ADDR			(XP70P_REG_START_ADDR+XP70P_REG_LENGTH-1)

    // XP70 data 0xA03AD000  or 0xA03AE000 ??
	#define XP70D_REG_START_ADDR		0x801B8000
	#define XP70D_REG_LENGTH			CNTRL_REG_SIZE
	#define XP70D_REG_END_ADDR			(XP70D_REG_START_ADDR+XP70D_REG_LENGTH-1)

    // APE and A9-SS Debug Peripheral mapping in Periph4
    // ---------------------------------------------------------------------

    // SoC APE DAP
	#define DAP_REG_START_ADDR          0x80160000
	#define DAP_REG_LENGTH              (16 * PAGE_SIZE)
	#define DAP_REG_END_ADDR            (DAP_REG_START_ADDR+DAP_REG_LENGTH-1)

    // SoC ROM table
	#define DAP_ROM_TABLE_START_ADDR    0x80170000
	#define DAP_ROM_TABLE_LENGTH        (16 * PAGE_SIZE)
	#define DAP_ROM_TABLE_END_ADDR     ( DAP_ROM_TABLE_START_ADDR+DAP_ROM_TABLE_LENGTH-1)

    // SoC CTI
	#define CTI_START_ADDR              0x80180000
	#define CTI_LENGTH                  (16 * PAGE_SIZE)
	#define CTI_END_ADDR               ( CTI_START_ADDR+CTI_LENGTH-1)

    // SoC Trace port
	#define TPIU_START_ADDR             0x80190000
	#define TPIU_LENGTH                 (16 * PAGE_SIZE)
	#define TPIU_END_ADDR               (TPIU_START_ADDR+TPIU_LENGTH-1)

    // A9 Debug ROM table
	#define DBG_ROM_BASE_START_ADDR     0x801A0000
	#define DBG_ROM_BASE_LENGTH         PAGE_SIZE
	#define DBG_ROM_BASE_END_ADDR       (DBG_ROM_BASE_START_ADDR+DBG_ROM_BASE_LENGTH-1)

    // Embedded Trace Buffer
	#define ETB_START_ADDR              0x801A4000
	#define ETB_LENGTH                  PAGE_SIZE
	#define ETB_END_ADDR               (ETB_START_ADDR+ETB_LENGTH-1)

    // Embedded Trace Buffer CTI
	#define ETB_CTI_START_ADDR          0x801A5000
	#define ETB_CTI_LENGTH              PAGE_SIZE
	#define ETB_CTI_END_ADDR            (ETB_CTI_START_ADDR+ETB_CTI_LENGTH-1)

    // A9 Coresigth Funnel
	#define A9_FUNNEL_START_ADDR        0x801A6000
	#define A9_FUNNEL_LENGTH            PAGE_SIZE
	#define A9_FUNNEL_END_ADDR          (A9_FUNNEL_START_ADDR+A9_FUNNEL_LENGTH-1)

    // Core0 Debug
	#define CORE0_DBG_START_ADDR        0x801A8000
	#define CORE0_DBG_LENGTH            PAGE_SIZE
	#define CORE0_DBG_END_ADDR          (CORE0_DBG_START_ADDR+CORE0_DBG_LENGTH-1)

    // Core0 PMU (Performnace Monitor Unit)
	#define CORE0_PMU_START_ADDR        0x801A9000
	#define CORE0_PMU_LENGTH            PAGE_SIZE
	#define CORE0_PMU_END_ADDR          (CORE0_PMU_START_ADDR+CORE0_PMU_LENGTH-1)

    // Core1 Debug
	#define CORE1_DBG_START_ADDR        0x801AA000
	#define CORE1_DBG_LENGTH            PAGE_SIZE
	#define CORE1_DBG_END_ADDR          (CORE1_DBG_START_ADDR+CORE1_DBG_LENGTH-1)

    // Core1 PMU (Performnace Monitor Unit)
	#define CORE1_PMU_START_ADDR        0x801AB000
	#define CORE1_PMU_LENGTH            PAGE_SIZE
	#define CORE1_PMU_END_ADDR          (CORE1_PMU_START_ADDR+CORE1_PMU_LENGTH-1)

    // CTI0
	#define CTI0_START_ADDR             0x801AC000
	#define CTI0_LENGTH                 PAGE_SIZE
	#define CTI0_END_ADDR               (CTI0_START_ADDR+CTI0_PMU_LENGTH-1)

    // CTI1
	#define CTI1_START_ADDR             0x801AD000
	#define CTI1_LENGTH                 PAGE_SIZE
	#define CTI1_END_ADDR               (CTI1_START_ADDR+CTI1_PMU_LENGTH-1)

    // PTM0 (Program Trace Module 0)
	#define PTM0_START_ADDR             0x801AE000
	#define PTM0_LENGTH                 PAGE_SIZE
	#define PTM0_END_ADDR               (PTM0_START_ADDR+PTM0_LENGTH-1)

    // PTM1 (Program Trace Module 1)
	#define PTM1_START_ADDR             0x801AF000
	#define PTM1_LENGTH                 PAGE_SIZE
	#define PTM1_END_ADDR               (PTM1_START_ADDR+PTM1_LENGTH-1)

//--------------------------------------------------------------------
// DMA periph5 peripheral definitions
//--------------------------------------------------------------------

    // USB OTG configuration registers + data
	#define USBOTG_REG_START_ADDR		0xA03E0000
	#define USBOTG_REG_LENGTH			0x00010000
	#define USBOTG_REG_END_ADDR			(USBOTG_REG_START_ADDR+USBOTG_REG_LENGTH-1)

    // General Purpose I/Os 256 to 287
	#define GPIO8_REG_START_ADDR		0xA03FE000
	#define GPIO8_REG_LENGTH			GPIO_REG_SIZE
	#define GPIO8_REG_END_ADDR			(GPIO8_REG_START_ADDR+GPIO8_REG_LENGTH-1)

   // CLK_RST5
	#define CLK_RST5_REG_START_ADDR     0xA03FF000
	#define CLK_RST5_REG_LENGTH         CNTRL_REG_SIZE
	#define CLK_RST5_REG_END_ADDR       (CLK_RST5_REG_START_ADDR+CLK_RST5_REG_LENGTH-1)

//--------------------------------------------------------------------
// DMA periph6 peripheral definitions
//--------------------------------------------------------------------
    // Random number generator
	#define RNG_REG_START_ADDR			0xA03C0000
	#define RNG_REG_LENGTH				CNTRL_REG_SIZE
	#define RNG_REG_END_ADDR			(RNG_REG_START_ADDR+RNG_REG_LENGTH-1)

    // HASH0
	#define HASH_REG_START_ADDR			0xA03C1000
	#define HASH_REG_LENGTH				CNTRL_REG_SIZE
	#define HASH_REG_END_ADDR			(HASH_REG_START_ADDR+HASH_REG_LENGTH-1)

    // HASH1
	#define HASH1_REG_START_ADDR			0xA03C2000
	#define HASH1_REG_LENGTH				CNTRL_REG_SIZE
	#define HASH1_REG_END_ADDR			(HASH1_REG_START_ADDR+HASH1_REG_LENGTH-1)

	// PKA base registers
	#define PKA_REG_START_ADDR			0xA03C4000
	#define PKA_REG_LENGTH				CNTRL_REG_SIZE
	#define PKA_REG_END_ADDR			(PKA_REG_START_ADDR+PKA_REG_LENGTH-1)

	// PKA base memory
	#define PKA_MEM_START_ADDR			0xA03C5000
	#define PKA_MEM_LENGTH				CNTRL_REG_SIZE
	#define PKA_MEM_END_ADDR			(PKA_MEM_START_ADDR+PKA_MEM_LENGTH-1)

	// Multiple timer unit 0
	#define MTU0_REG_START_ADDR         0xA03C6000
	#define MTU0_REG_LENGTH             CNTRL_REG_SIZE
	#define MTU0_REG_END_ADDR           (MTU0_REG_START_ADDR+MTU0_REG_LENGTH-1)

	// Multiple timer unit 1
	#define MTU1_REG_START_ADDR         0xA03C7000
	#define MTU1_REG_LENGTH             CNTRL_REG_SIZE
	#define MTU1_REG_END_ADDR           (MTU1_REG_START_ADDR+MTU1_REG_LENGTH-1)

    // CR control register
	#define CR_REG_START_ADDR           0xA03C8000
	#define CR_REG_LENGTH               CNTRL_REG_SIZE
	#define CR_REG_END_ADDR             (CR_REG_START_ADDR+CR_REG_LENGTH-1)

	// CRYPTO processor (DES, TDES & AES)
	#define CRYP_REG_START_ADDR         0xA03CA000
	#define CRYP_REG_LENGTH             CNTRL_REG_SIZE
	#define CRYP_REG_END_ADDR           (CRYP_REG_START_ADDR+CRYP_REG_LENGTH-1)

	// CRYPT1 processor (DES, TDES & AES)
	#define CRYP1_REG_START_ADDR        0xA03CB000
	#define CRYP1_REG_LENGTH            CNTRL_REG_SIZE
	#define CRYP1_REG_END_ADDR          (CRYP1_REG_START_ADDR+CRYP1_REG_LENGTH-1)

	// UNIPRO registers
	#define UNIPRO_REG_START_ADDR       0xA03CD000
	#define UNIPRO_REG_LENGTH           CNTRL_REG_SIZE
	#define UNIPRO_REG_END_ADDR         (UNIPRO_REG_START_ADDR+UNIPRO_REG_LENGTH-1)

   // CLK_RST6
	#define CLK_RST6_REG_START_ADDR			0xA03CF000
	#define CLK_RST6_REG_LENGTH				CNTRL_REG_SIZE
	#define CLK_RST6_REG_END_ADDR			(CLK_RST6_REG_START_ADDR+CLK_RST6_REG_LENGTH-1)

//--------------------------------------------------------------------
// Peripheral test block definitions
//--------------------------------------------------------------------

	// ETM configuration registers ??
	#define ETM_REG_START_ADDR			0x63280000
	#define ETM_REG_LENGTH				0x00010000
	#define ETM_REG_END_ADDR			(ETM_REG_START_ADDR+ETM_REG_LENGTH-1)

    // System and reset controller
	#define SRC_REG_START_ADDR			0x903BF000
	#define SRC_REG_LENGTH				CNTRL_REG_SIZE
	#define SRC_REG_END_ADDR			(SRC_REG_START_ADDR+SRC_REG_LENGTH-1)

	// FIRDA
	#define FIRDA_REG_START_ADDR		0x7011F000
	#define FIRDA_REG_LENGTH			CNTRL_REG_SIZE
	#define FIRDA_REG_END_ADDR			(FIRDA_REG_START_ADDR+FIRDA_REG_LENGTH-1)

	// TZIC1
	#define TZIC1_REG_START_ADDR		0x90413000
	#define TZIC1_REG_LENGTH			CNTRL_REG_SIZE
	#define TZIC1_REG_END_ADDR			(TZIC1_REG_START_ADDR+TZIC1_REG_LENGTH-1)

	// TZIC0
	#define TZIC0_REG_START_ADDR		0x90412000
	#define TZIC0_REG_LENGTH			CNTRL_REG_SIZE
	#define TZIC0_REG_END_ADDR			(TZIC0_REG_START_ADDR+TZIC0_REG_LENGTH-1)

//--------------------------------------------------------------------
// Modem sub-system
//--------------------------------------------------------------------
	// ModemSS
	#define MODEM_REG_START_ADDR		0xB0000000
	#define MODEM_REG_LENGTH			0x10000000
	#define MODEM_REG_END_ADDR			(MODEM_REG_START_ADDR+MODEM_REG_LENGTH-1)

//////////////////////////

#endif // _NOMADIK_MAPPING_H

// End of file - nomadik_mapping.h
