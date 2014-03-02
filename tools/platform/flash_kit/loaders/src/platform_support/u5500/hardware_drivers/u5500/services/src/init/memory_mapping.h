/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides the base address of the component
* \author  ST-Ericsson
*/
/*****************************************************************************/


#ifndef __INC_MEMORY_MAPPING_H
#define __INC_MEMORY_MAPPING_H

/*--------------------------------------------------------------------------*/

/* SDRAM bank 0 */
#define SDRAM_BANK_0_BASE_ADDR               0x00000000
#define SDRAM_BANK_0_END_ADDR                0x1FFFFFFF

/* SDRAM bank 1 */
#define SDRAM_BANK_1_BASE_ADDR               0x20000000
#define SDRAM_BANK_1_END_ADDR                0x3FFFFFFF


/* Embedded buffer SRAM (384KBytes) */
#define EMBEDDED_SRAM_BASE_ADDR              0x40000000
#define EMBEDDED_SRAM_END_ADDR               0x4005FFFF



/* Nor Flash Bank0 Address */
#define NOR_FLASH_BANK_0_BASE_ADDR           0x50000000
#define NOR_FLASH_BANK_0_END_ADDR            0x53FFFFFF

/* Nor Flash Bank1 Address */
#define NOR_FLASH_BANK_1_BASE_ADDR           0x54000000
#define NOR_FLASH_BANK_1_END_ADDR            0x57FFFFFF

/* Nor Flash Bank2 Address */
#define NOR_FLASH_BANK_2_BASE_ADDR           0x58000000
#define NOR_FLASH_BANK_2_END_ADDR            0x5BFFFFFF

/* Nor Flash Bank3 Address */
#define NOR_FLASH_BANK_3_BASE_ADDR           0x5C000000
#define NOR_FLASH_BANK_3_END_ADDR            0x5FFFFFFF


/* Nand Flash Bank0 Address */
#define NAND_FLASH_BANK_0_BASE_ADDR          0x60000000
#define NAND_FLASH_BANK_0_END_ADDR           0x6FFFFFFF


/*******************************************************************************/

#define PERIPHERAL_5_BASE_ADDR               0x80100000
#define PERIPHERAL_5_END_ADDR                0x8011FFFF


/* Peripheral 5 Clock and Reset Control */
#define PRCC_5_CTRL_REG_BASE_ADDR            (PERIPHERAL_5_BASE_ADDR) + 0x1F000 
#define PRCC_5_CTRL_REG_END_ADDR             (PERIPHERAL_5_BASE_ADDR) + 0x1FFFF

/* GPIO 5 Interface registers  */
#define GPIO_5_REG_BASE_ADDR                 (PERIPHERAL_5_BASE_ADDR) + 0x1E000
#define GPIO_5_REG_END_ADDR                  (PERIPHERAL_5_BASE_ADDR) + 0x1E080

/* GPIO 6 Interface registers  */
#define GPIO_6_REG_BASE_ADDR                 (PERIPHERAL_5_BASE_ADDR) + 0x1E080
#define GPIO_6_REG_END_ADDR                  (PERIPHERAL_5_BASE_ADDR) + 0x1E100

/* GPIO 7 Interface registers  */
#define GPIO_7_REG_BASE_ADDR                 (PERIPHERAL_5_BASE_ADDR) + 0x1E100
#define GPIO_7_REG_END_ADDR                  (PERIPHERAL_5_BASE_ADDR) + 0x1E180


/* IrRC Interface registers  */
#define IrRC_REG_BASE_ADDR                   (PERIPHERAL_5_BASE_ADDR) + 0x10000
#define IrRC_REG_END_ADDR                    (PERIPHERAL_5_BASE_ADDR) + 0x1EFFF

/* IrDA Interface registers  */
#define IrDA_REG_BASE_ADDR                   (PERIPHERAL_5_BASE_ADDR) + 0xD000
#define IrDA_REG_END_ADDR                    (PERIPHERAL_5_BASE_ADDR) + 0xFFFF


/* MSP 2 Interface Registers */
#define MSP_2_REG_BASE_ADDR                  (PERIPHERAL_5_BASE_ADDR) + 0xC000
#define MSP_2_REG_END_ADDR                   (PERIPHERAL_5_BASE_ADDR) + 0xCFFF


/* I2C 3 Interface Registers */
#define I2C_3_REG_BASE_ADDR                  (PERIPHERAL_5_BASE_ADDR) + 0xB000
#define I2C_3_REG_END_ADDR                   (PERIPHERAL_5_BASE_ADDR) + 0xBFFF

/* I2C 2 Interface Registers */
#define I2C_2_REG_BASE_ADDR                  (PERIPHERAL_5_BASE_ADDR) + 0xA000
#define I2C_2_REG_END_ADDR                   (PERIPHERAL_5_BASE_ADDR) + 0xAFFF

/* MM-Card/SD-Card4 Interface Registers */
#define SDI_4_REG_BASE_ADDR                  (PERIPHERAL_5_BASE_ADDR) + 0x9000
#define SDI_4_REG_END_ADDR                   (PERIPHERAL_5_BASE_ADDR) + 0x9FFF


/* MM-Card/SD-Card3 Interface Registers */
#define SDI_3_REG_BASE_ADDR                  (PERIPHERAL_5_BASE_ADDR) + 0x8000
#define SDI_3_REG_END_ADDR                   (PERIPHERAL_5_BASE_ADDR) + 0x8FFF


/* MM-Card/SD-Card1 Interface Registers */
#define SDI_1_REG_BASE_ADDR                  (PERIPHERAL_5_BASE_ADDR) + 0x7000
#define SDI_1_REG_END_ADDR                   (PERIPHERAL_5_BASE_ADDR) + 0x7FFF


/* UART 3 Interface Registers */
#define UART_3_CTRL_REG_BASE_ADDR            (PERIPHERAL_5_BASE_ADDR) + 0x6000
#define UART_3_CTRL_REG_END_ADDR             (PERIPHERAL_5_BASE_ADDR) + 0x6FFF


/* UART 2 Interface Registers */
#define UART_2_CTRL_REG_BASE_ADDR            (PERIPHERAL_5_BASE_ADDR) + 0x5000
#define UART_2_CTRL_REG_END_ADDR             (PERIPHERAL_5_BASE_ADDR) + 0x5FFF


/* UART 1 Interface Registers */
#define UART_1_CTRL_REG_BASE_ADDR            (PERIPHERAL_5_BASE_ADDR) + 0x4000
#define UART_1_CTRL_REG_END_ADDR             (PERIPHERAL_5_BASE_ADDR) + 0x4FFF

/* SPI 3 Interface Registers */
#define SPI_3_CTRL_REG_BASE_ADDR             (PERIPHERAL_5_BASE_ADDR) + 0x3000
#define SPI_3_CTRL_REG_END_ADDR              (PERIPHERAL_5_BASE_ADDR) + 0x3FFF

/* SPI 2 Interface Registers */
#define SPI_2_CTRL_REG_BASE_ADDR             (PERIPHERAL_5_BASE_ADDR) + 0x2000
#define SPI_2_CTRL_REG_END_ADDR              (PERIPHERAL_5_BASE_ADDR) + 0x2FFF

/* SPI 1 Interface Registers */
#define SPI_1_CTRL_REG_BASE_ADDR             (PERIPHERAL_5_BASE_ADDR) + 0x1000
#define SPI_1_CTRL_REG_END_ADDR              (PERIPHERAL_5_BASE_ADDR) + 0x1FFF

/* SPI 0 Interface Registers */
#define SPI_0_CTRL_REG_BASE_ADDR             (PERIPHERAL_5_BASE_ADDR) + 0x0000
#define SPI_0_CTRL_REG_END_ADDR              (PERIPHERAL_5_BASE_ADDR) + 0x0FFF


/*********************************************************************************/



#define PERIPHERAL_6_BASE_ADDR               0x80120000
#define PERIPHERAL_6_END_ADDR                0x8012FFFF

/* Peripheral 6 Clock and Reset Control */
#define PRCC_6_CTRL_REG_BASE_ADDR            (PERIPHERAL_6_BASE_ADDR) + 0xF000 
#define PRCC_6_CTRL_REG_END_ADDR             (PERIPHERAL_6_BASE_ADDR) + 0xFFFF


/* Crypto 1 processor configuration/data registers */
#define CRYP_1_REG_BASE_ADDR                 (PERIPHERAL_6_BASE_ADDR) + 0xB000
#define CRYP_1_REG_END_ADDR                  (PERIPHERAL_6_BASE_ADDR) + 0xBFFF


/* Crypto 0 processor configuration/data registers */
#define CRYP_0_REG_BASE_ADDR                 (PERIPHERAL_6_BASE_ADDR) + 0xA000
#define CRYP_0_REG_END_ADDR                  (PERIPHERAL_6_BASE_ADDR) + 0xAFFF

/* Multi Timer Unit 1 registers */
#define MTU_1_REG_BASE_ADDR                  (PERIPHERAL_6_BASE_ADDR) + 0x7000
#define MTU_1_REG_END_ADDR                   (PERIPHERAL_6_BASE_ADDR) + 0x7FFF


/* Multi Timer Unit 0 registers */
#define MTU_0_REG_BASE_ADDR                  (PERIPHERAL_6_BASE_ADDR) + 0x6000
#define MTU_0_REG_END_ADDR                   (PERIPHERAL_6_BASE_ADDR) + 0x6FFF


/* Public Key Accelerator registers */
#define PKA_MEM_BASE_ADDR                    (PERIPHERAL_6_BASE_ADDR) + 0x5100
#define PKA_MEM_END_ADDR                     (PERIPHERAL_6_BASE_ADDR) + 0x51FF

/* Public Key Accelerator registers */
#define PKA_REG_BASE_ADDR                    (PERIPHERAL_6_BASE_ADDR) + 0x4000
#define PKA_REG_END_ADDR                     (PERIPHERAL_6_BASE_ADDR) + 0x5FFF

/* HASH 1  Processor registers */
#define HASH_1_BASE_ADDR                     (PERIPHERAL_6_BASE_ADDR) + 0x2000
#define HASH_1_END_ADDR                      (PERIPHERAL_6_BASE_ADDR) + 0x2FFF

/* HASH 0  Processor registers */
#define HASH_0_BASE_ADDR                     (PERIPHERAL_6_BASE_ADDR) + 0x1000
#define HASH_0_END_ADDR                      (PERIPHERAL_6_BASE_ADDR) + 0x1FFF


/* RNG configuration registers */
#define RNG_CFG_REG_BASE_ADDR                (PERIPHERAL_6_BASE_ADDR) + 0x0000
#define RNG_CFG_REG_END_ADDR                 (PERIPHERAL_6_BASE_ADDR) + 0x0FFF


/***********************************************************************************/

#define PERIPHERAL_3_BASE_ADDR               0x80140000
#define PERIPHERAL_3_END_ADDR                0x8014FFFF

/* Peripheral 3 Clock and Reset Control */
#define PRCC_3_CTRL_REG_BASE_ADDR            (PERIPHERAL_3_BASE_ADDR) + 0xF000 
#define PRCC_3_CTRL_REG_END_ADDR             (PERIPHERAL_3_BASE_ADDR) + 0xFFFF

/* GPIO 4 Interface registers  */
#define GPIO_4_REG_BASE_ADDR                 (PERIPHERAL_3_BASE_ADDR) + 0xE000
#define GPIO_4_REG_END_ADDR                  (PERIPHERAL_3_BASE_ADDR) + 0xE080


/* GPIO 2 Interface registers  */
#define KEYPAD_REG_BASE_ADDR                 (PERIPHERAL_3_BASE_ADDR) + 0x0000
#define KEYPAD_REG_END_ADDR                  (PERIPHERAL_3_BASE_ADDR) + 0xFFFF



/************************************************************************************/


#define PERIPHERAL_4_BASE_ADDR               0x80150000
#define PERIPHERAL_4_END_ADDR                0x801CFFFF

/* Back-up RAM */
#define BACKUP_RAM_BASE_ADDR                 (PERIPHERAL_4_BASE_ADDR)    + 0x0000
#define BACKUP_RAM_END_ADDR                  (PERIPHERAL_4_BASE_ADDR)    + 0x1FFF    

/* Real Time timer Registers */
#define RTT0_REG_BASE_ADDR                    (PERIPHERAL_4_BASE_ADDR)  + 0x2000
#define RTT0_REG_END_ADDR                     (PERIPHERAL_4_BASE_ADDR)  + 0x2FFF

/* Real Time timer Registers */
#define RTT1_REG_BASE_ADDR                    (PERIPHERAL_4_BASE_ADDR)  + 0x3000
#define RTT1_REG_END_ADDR                     (PERIPHERAL_4_BASE_ADDR)  + 0x3FFF

/* Real Time Clock Registers */
#define RTC_REG_BASE_ADDR                    (PERIPHERAL_4_BASE_ADDR)  + 0x4000
#define RTC_REG_END_ADDR                     (PERIPHERAL_4_BASE_ADDR)  + 0x4FFF

/* SCR Registers */
#define SCR_REG_BASE_ADDR                    (PERIPHERAL_4_BASE_ADDR)  + 0x5000
#define SCR_REG_END_ADDR                     (PERIPHERAL_4_BASE_ADDR)  + 0x5FFF


/* Dynamic Memory Controller Configuration registers */
#define DMC_CTRL_REG_BASE_ADDR             	    (PERIPHERAL_4_BASE_ADDR)  + 0x6000
#define DMC_CTRL_REG_END_ADDR              	    (PERIPHERAL_4_BASE_ADDR)  + 0x6FFF

/* Power, Clock and Reset Management Unit Registers */
#define PRCMU_REG_BASE_ADDR                 	 (PERIPHERAL_4_BASE_ADDR)  + 0x7000
#define PRCMU_REG_END_ADDR                  	 (PERIPHERAL_4_BASE_ADDR)  + 0x7FFF


/* Power, Clock and Reset Management Unit Secure Registers */
#define PRCMU_SEC_REG_BASE_ADDR                  (PERIPHERAL_4_BASE_ADDR)  + 0x8000
#define PRCMU_SEC_REG_END_ADDR                   (PERIPHERAL_4_BASE_ADDR)  + 0x8FFF


/* MSP 1 Interface Registers */
#define MSP_1_REG_BASE_ADDR               		   (PERIPHERAL_4_BASE_ADDR) + 0x9000    
#define MSP_1_REG_END_ADDR                		   (PERIPHERAL_4_BASE_ADDR) + 0x9FFF


/* GPIO 3 Interface registers  */
#define GPIO_3_REG_BASE_ADDR               		  (PERIPHERAL_4_BASE_ADDR) + 0xA000
#define GPIO_3_REG_END_ADDR                		  (PERIPHERAL_4_BASE_ADDR) + 0xAFFF


/* ICN_PMU Interface Registers */
#define ICN_PMU_REG_BASE_ADDR               	   (PERIPHERAL_4_BASE_ADDR) + 0xBE00    
#define ICN_PMU_REG_END_ADDR                	   (PERIPHERAL_4_BASE_ADDR) + 0xBEFF

/* Cable detect Interface Registers */
#define CDETECT_REG_BASE_ADDR               	   (PERIPHERAL_4_BASE_ADDR) + 0xF000   
#define CDETECT_REG_END_ADDR                	   (PERIPHERAL_4_BASE_ADDR) + 0xFFFF

/* XP70 Program Interface Registers */
#define XP70_PROG_REG_BASE_ADDR               	   (PERIPHERAL_4_BASE_ADDR) + 0x10000   
#define XP70_PROG_REG_END_ADDR                	   (PERIPHERAL_4_BASE_ADDR) + 0x17FFF

/* XP70 Data Interface Registers */
#define XP70_DATA_REG_BASE_ADDR               	   (PERIPHERAL_4_BASE_ADDR) + 0x18000   
#define XP70_DATA_REG_END_ADDR                	   (PERIPHERAL_4_BASE_ADDR) + 0x1CFFF



/********************************************************************************/


#define PERIPHERAL_2_BASE_ADDR               0xA0010000
#define PERIPHERAL_2_END_ADDR                0xA001FFFF

/* Peripheral 2 Clock and Reset Control */
#define PRCC_2_CTRL_REG_BASE_ADDR            (PERIPHERAL_2_BASE_ADDR) + 0xF000 
#define PRCC_2_CTRL_REG_END_ADDR             (PERIPHERAL_2_BASE_ADDR) + 0xFFFF

/* GPIO 2 Interface registers  */
#define GPIO_2_REG_BASE_ADDR                 (PERIPHERAL_2_BASE_ADDR) + 0xE000
#define GPIO_2_REG_END_ADDR                  (PERIPHERAL_2_BASE_ADDR) + 0xE080


/* USB OTG HS Controller registers */
#define USB_REG_BASE_ADDR                    (PERIPHERAL_2_BASE_ADDR) + 0x00000
#define USB_REG_END_ADDR                     (PERIPHERAL_2_BASE_ADDR) + 0x0FFFF

/**************************************************************************************/

#define PERIPHERAL_1_BASE_ADDR               0xA0020000
#define PERIPHERAL_1_END_ADDR                0xA002FFFF



/* Peripheral 2 Clock and Reset Control */
#define PRCC_1_CTRL_REG_BASE_ADDR            (PERIPHERAL_1_BASE_ADDR) + 0xF000 
#define PRCC_1_CTRL_REG_END_ADDR             (PERIPHERAL_1_BASE_ADDR) + 0xFFFF

/* GPIO 0 Interface registers  */
#define GPIO_0_REG_BASE_ADDR                 (PERIPHERAL_1_BASE_ADDR) + 0xE000
#define GPIO_0_REG_END_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0xE080

/* GPIO 1 Interface registers  */
#define GPIO_1_REG_BASE_ADDR                 (PERIPHERAL_1_BASE_ADDR) + 0xE000
#define GPIO_1_REG_END_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0xE100


/* MSP 0 Interface Registers */
#define MSP_0_REG_BASE_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0x5000
#define MSP_0_REG_END_ADDR                   (PERIPHERAL_1_BASE_ADDR) + 0x5FFF


/* I2C 1 Interface Registers */
#define I2C_1_REG_BASE_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0x4000
#define I2C_1_REG_END_ADDR                   (PERIPHERAL_1_BASE_ADDR) + 0x4FFF

/* UART 0 Interface Registers */
#define UART_0_CTRL_REG_BASE_ADDR            (PERIPHERAL_1_BASE_ADDR) + 0x3000
#define UART_0_CTRL_REG_END_ADDR             (PERIPHERAL_1_BASE_ADDR) + 0x3FFF


/* MM-Card/SD-Card2 Interface Registers */
#define SDI_2_REG_BASE_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0x2000
#define SDI_2_REG_END_ADDR                   (PERIPHERAL_1_BASE_ADDR) + 0x2FFF


/* MM-Card/SD-Card0 Interface Registers */
#define SDI_0_REG_BASE_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0x1000
#define SDI_0_REG_END_ADDR                   (PERIPHERAL_1_BASE_ADDR) + 0x1FFF

/* FSMC configuration registers */
#define FSMC_CTRL_REG_BASE_ADDR              (PERIPHERAL_1_BASE_ADDR) + 0x0000
#define FSMC_CTRL_REG_END_ADDR               (PERIPHERAL_1_BASE_ADDR) + 0x0FFF

/************************************************************************************/


/* GIC Controller configuration registers */
#define GIC_0_CTRL_REG_BASE_ADDR             0xA0410100
#define GIC_0_CTRL_REG_END_ADDR              0xA041FFFF


/* Embedded boot ROM (128KBytes) */
#define BOOT_ROM_BASE_ADDR                   0x90000000
#define BOOT_ROM_END_ADDR                    0x9001FFFF


/* DMA Controller configuration registers */
#define DMA_CTRL_REG_BASE_ADDR               0x90030000
#define DMA_CTRL_REG_END_ADDR                0x90030FFF

/* GIC IT distributor register */
#define GIC_DIST_REG_BASE_ADDR               0xA0411000
#define GIC_DIST_REG_END_ADDR                0xA0411FFF



#endif /*__INC_MEMORY_MAPPING_H */
