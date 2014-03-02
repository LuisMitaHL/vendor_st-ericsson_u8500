/*****************************************************************************/
/**
*  � ST-Ericsson, 2009 - All rights reserved
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

#if defined(ST_8500ED) || defined(ST_HREFED) || defined(__PEPS_8500)

/* Embedded buffer SRAM (1024KBytes) */
#define EMBEDDED_SRAM_BASE_ADDR              0x40000000
#define EMBEDDED_SRAM_END_ADDR               0x400BFFFF

#elif defined(ST_8500V1) || defined(ST_HREFV1) || defined(__PEPS_8500_V1)|| defined(__PEPS_8500_V2) || defined(ST_8500V2) || defined(ST_HREFV2)

/* Embedded buffer SRAM (640KBytes) */
#define EMBEDDED_SRAM_BASE_ADDR              0x40000000
#define EMBEDDED_SRAM_END_ADDR               0x4009FFFF

#endif


/* Nor Flash Bank0 Address */
#define NOR_FLASH_BANK_0_BASE_ADDR           0x50000000
#define NOR_FLASH_BANK_0_END_ADDR            0x53FFFFFF

/* Nor Flash Bank1 Address */
#define NOR_FLASH_BANK_1_BASE_ADDR           0x54000000
#define NOR_FLASH_BANK_1_END_ADDR            0x57FFFFFF

/* Nand Flash Bank0 Address */
#define NAND_FLASH_BANK_0_BASE_ADDR          0x60000000
#define NAND_FLASH_BANK_0_END_ADDR           0x6FFFFFFF

/* Nand Flash Bank1 Address */
#define NAND_FLASH_BANK_1_BASE_ADDR          0x70000000
#define NAND_FLASH_BANK_1_END_ADDR           0x7FFFFFFF


#define PERIPHERAL_3_BASE_ADDR               0x80000000
#define PERIPHERAL_3_END_ADDR                0x8000FFFF

/* FSMC configuration registers */
#define FSMC_CTRL_REG_BASE_ADDR              (PERIPHERAL_3_BASE_ADDR) + 0x0000
#define FSMC_CTRL_REG_END_ADDR               (PERIPHERAL_3_BASE_ADDR) + 0x0FFF

/* SSP 0 Interface Registers */
#define SSP_0_REG_BASE_ADDR                  (PERIPHERAL_3_BASE_ADDR) + 0x2000
#define SSP_0_REG_END_ADDR                   (PERIPHERAL_3_BASE_ADDR) + 0x2FFF

/* SSP 1 Interface Registers */
#define SSP_1_REG_BASE_ADDR                  (PERIPHERAL_3_BASE_ADDR) + 0x3000
#define SSP_1_REG_END_ADDR                   (PERIPHERAL_3_BASE_ADDR) + 0x3FFF

/* I2C 0 Interface Registers */
#define I2C_0_REG_BASE_ADDR                  (PERIPHERAL_3_BASE_ADDR) + 0x4000
#define I2C_0_REG_END_ADDR                   (PERIPHERAL_3_BASE_ADDR) + 0x4FFF

/* MM-Card/SD-Card2 Interface Registers */
#define SDI_2_REG_BASE_ADDR                  (PERIPHERAL_3_BASE_ADDR) + 0x5000
#define SDI_2_REG_END_ADDR                   (PERIPHERAL_3_BASE_ADDR) + 0x5FFF

/* Scroll Key Encoder Registers */
#define SKE_REG_BASE_ADDR                    (PERIPHERAL_3_BASE_ADDR) + 0x6000
#define SKE_REG_END_ADDR                     (PERIPHERAL_3_BASE_ADDR) + 0x6FFF

/* UART 2 Interface Registers */
#define UART_2_CTRL_REG_BASE_ADDR            (PERIPHERAL_3_BASE_ADDR) + 0x7000
#define UART_2_CTRL_REG_END_ADDR             (PERIPHERAL_3_BASE_ADDR) + 0x7FFF

/* MM-Card/SD-Card5 Interface Registers */
#define SDI_5_REG_BASE_ADDR                  (PERIPHERAL_3_BASE_ADDR) + 0x8000
#define SDI_5_REG_END_ADDR                   (PERIPHERAL_3_BASE_ADDR) + 0x8FFF

/* Peripheral 3 Clock and Reset Control */
#define PRCC_3_CTRL_REG_BASE_ADDR            (PERIPHERAL_3_BASE_ADDR) + 0xF000 
#define PRCC_3_CTRL_REG_END_ADDR             (PERIPHERAL_3_BASE_ADDR) + 0xFFFF

/* GPIO 2 Interface registers  */
#define GPIO_2_REG_BASE_ADDR                 (PERIPHERAL_3_BASE_ADDR) + 0xE000
#define GPIO_2_REG_END_ADDR                  (PERIPHERAL_3_BASE_ADDR) + 0xE07F

/* GPIO 3 Interface registers  */
#define GPIO_3_REG_BASE_ADDR                 (PERIPHERAL_3_BASE_ADDR) + 0xE080
#define GPIO_3_REG_END_ADDR                  (PERIPHERAL_3_BASE_ADDR) + 0xE0FF

/* GPIO 4 Interface registers  */
#define GPIO_4_REG_BASE_ADDR                 (PERIPHERAL_3_BASE_ADDR) + 0xE100
#define GPIO_4_REG_END_ADDR                  (PERIPHERAL_3_BASE_ADDR) + 0xE17F

/* GPIO 5 Interface registers  */
#define GPIO_5_REG_BASE_ADDR                 (PERIPHERAL_3_BASE_ADDR) + 0xE180
#define GPIO_5_REG_END_ADDR                  (PERIPHERAL_3_BASE_ADDR) + 0xEFFF

/* STM  configuration registers */
#define STM_BASE_ADDR                        0x80100000
#define STM_END_ADDR                         0x8010FFFF


#define PERIPHERAL_2_BASE_ADDR               0x80110000
#define PERIPHERAL_2_END_ADDR                0x8011FFFF

/* I2C 3 Interface Registers */
#define I2C_3_REG_BASE_ADDR                  (PERIPHERAL_2_BASE_ADDR) + 0x0000
#define I2C_3_REG_END_ADDR                   (PERIPHERAL_2_BASE_ADDR) + 0x0FFF

/* SPI 2 Interface Registers */
#define SPI_2_REG_BASE_ADDR                  (PERIPHERAL_2_BASE_ADDR) + 0x1000
#define SPI_2_REG_END_ADDR                   (PERIPHERAL_2_BASE_ADDR) + 0x1FFF

/* SPI 1 Interface Registers */
#define SPI_1_REG_BASE_ADDR                  (PERIPHERAL_2_BASE_ADDR) + 0x2000
#define SPI_1_REG_END_ADDR                   (PERIPHERAL_2_BASE_ADDR) + 0x2FFF

/* PWL Interface Registers */
#define PWL_REG_BASE_ADDR                    (PERIPHERAL_2_BASE_ADDR) + 0x3000
#define PWL_REG_END_ADDR                     (PERIPHERAL_2_BASE_ADDR) + 0x3FFF

/* MM-Card/SD-Card4 Interface Registers */
#define SDI_4_REG_BASE_ADDR                  (PERIPHERAL_2_BASE_ADDR) + 0x4000
#define SDI_4_REG_END_ADDR                   (PERIPHERAL_2_BASE_ADDR) + 0x4FFF

/* Slim Bus 0  Interface Registers */
#define SLIMBUS_1_REG_BASE_ADDR              (PERIPHERAL_2_BASE_ADDR) + 0x5000
#define SLIMBUS_1_REG_END_ADDR               (PERIPHERAL_2_BASE_ADDR) + 0x5FFF

/* MSP 2 Interface Registers */
#define MSP_2_REG_BASE_ADDR                  (PERIPHERAL_2_BASE_ADDR) + 0x7000
#define MSP_2_REG_END_ADDR                   (PERIPHERAL_2_BASE_ADDR) + 0x7FFF

/* MM-Card/SD-Card1 Interface Registers */
#define SDI_1_REG_BASE_ADDR                  (PERIPHERAL_2_BASE_ADDR) + 0x8000
#define SDI_1_REG_END_ADDR                   (PERIPHERAL_2_BASE_ADDR) + 0x8FFF

/* MM-Card/SD-Card3 Interface Registers */
#define SDI_3_REG_BASE_ADDR                  (PERIPHERAL_2_BASE_ADDR) + 0x9000
#define SDI_3_REG_END_ADDR                   (PERIPHERAL_2_BASE_ADDR) + 0x9FFF

/* SPI0 Interface Registers */
#define SPI_0_REG_BASE_ADDR                  (PERIPHERAL_2_BASE_ADDR) + 0xA000
#define SPI_0_REG_END_ADDR                   (PERIPHERAL_2_BASE_ADDR) + 0xAFFF

/* HSI 8-ch Receive Interface Registers */
#define HSI_RX_REG_BASE_ADDR                 (PERIPHERAL_2_BASE_ADDR) + 0xB000
#define HSI_RX_REG_END_ADDR                  (PERIPHERAL_2_BASE_ADDR) + 0xBFFF

/* HSI 8-ch Transmit Interface Registers */
#define HSI_TX_REG_BASE_ADDR                 (PERIPHERAL_2_BASE_ADDR) + 0xC000
#define HSI_TX_REG_END_ADDR                  (PERIPHERAL_2_BASE_ADDR) + 0xCFFF

/* GPIO 6 Interface registers  */
#define GPIO_6_REG_BASE_ADDR                 (PERIPHERAL_2_BASE_ADDR) + 0xE000
#define GPIO_6_REG_END_ADDR                  (PERIPHERAL_2_BASE_ADDR) + 0xE07F

/* GPIO 7 Interface registers  */
#define GPIO_7_REG_BASE_ADDR                 (PERIPHERAL_2_BASE_ADDR) + 0xE080
#define GPIO_7_REG_END_ADDR                  (PERIPHERAL_2_BASE_ADDR) + 0xE100

/* Peripheral 2 Clock and Reset Control */
#define PRCC_2_CTRL_REG_BASE_ADDR            (PERIPHERAL_2_BASE_ADDR) + 0xF000 
#define PRCC_2_CTRL_REG_END_ADDR             (PERIPHERAL_2_BASE_ADDR) + 0xFFFF


#define PERIPHERAL_1_BASE_ADDR               0x80120000
#define PERIPHERAL_1_END_ADDR                0x8012FFFF

/* UART 0 Interface Registers */
#define UART_0_CTRL_REG_BASE_ADDR            (PERIPHERAL_1_BASE_ADDR) + 0x0000
#define UART_0_CTRL_REG_END_ADDR             (PERIPHERAL_1_BASE_ADDR) + 0x0FFF

/* UART 1 Interface Registers */
#define UART_1_CTRL_REG_BASE_ADDR            (PERIPHERAL_1_BASE_ADDR) + 0x1000
#define UART_1_CTRL_REG_END_ADDR             (PERIPHERAL_1_BASE_ADDR) + 0x1FFF

/* I2C 1 Interface Registers */
#define I2C_1_REG_BASE_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0x2000
#define I2C_1_REG_END_ADDR                   (PERIPHERAL_1_BASE_ADDR) + 0x2FFF

/* MSP 0 Interface Registers */
#define MSP_0_REG_BASE_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0x3000
#define MSP_0_REG_END_ADDR                   (PERIPHERAL_1_BASE_ADDR) + 0x3FFF

/* MSP 1 Interface Registers */
#define MSP_1_REG_BASE_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0x4000    
#define MSP_1_REG_END_ADDR                   (PERIPHERAL_1_BASE_ADDR) + 0x4FFF

#if defined(ST_8500V2)|| defined(ST_HREFV2)

/* MSP 3 Interface Registers */
#define MSP_3_REG_BASE_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0x5000    
#define MSP_3_REG_END_ADDR                   (PERIPHERAL_1_BASE_ADDR) + 0x5FFF

#endif

/* MM-Card/SD-Card0 Interface Registers */
#define SDI_0_REG_BASE_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0x6000
#define SDI_0_REG_END_ADDR                   (PERIPHERAL_1_BASE_ADDR) + 0x6FFF

/* I2C 2 Interface Registers */
#define I2C_2_REG_BASE_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0x8000
#define I2C_2_REG_END_ADDR                   (PERIPHERAL_1_BASE_ADDR) + 0x8FFF

/* SPI 3 Interface Registers */
#define SPI_3_REG_BASE_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0x9000
#define SPI_3_REG_END_ADDR                   (PERIPHERAL_1_BASE_ADDR) + 0x9FFF


#if defined(ST_8500V1) || defined(ST_HREFV1) || defined(__PEPS_8500_V1)|| defined(__PEPS_8500_V2) || defined(ST_8500V2) || defined(ST_HREFV2)

/* I2C 4 Interface Registers */
#define I2C_4_REG_BASE_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0xA000
#define I2C_4_REG_END_ADDR                   (PERIPHERAL_1_BASE_ADDR) + 0xAFFF

#endif

/* Slim Bus 0  Interface Registers */
#define SLIMBUS_0_REG_BASE_ADDR              (PERIPHERAL_1_BASE_ADDR) + 0xB000
#define SLIMBUS_0_REG_END_ADDR               (PERIPHERAL_1_BASE_ADDR) + 0xBFFF

/* GPIO 0 Interface registers  */
#define GPIO_0_REG_BASE_ADDR                 (PERIPHERAL_1_BASE_ADDR) + 0xE000
#define GPIO_0_REG_END_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0xE07F

/* GPIO 1 Interface registers  */
#define GPIO_1_REG_BASE_ADDR                 (PERIPHERAL_1_BASE_ADDR) + 0xE080
#define GPIO_1_REG_END_ADDR                  (PERIPHERAL_1_BASE_ADDR) + 0xEFFF

/* Peripheral 1 Clock and Reset Control */
#define PRCC_1_CTRL_REG_BASE_ADDR            (PERIPHERAL_1_BASE_ADDR) + 0xF000 
#define PRCC_1_CTRL_REG_END_ADDR             (PERIPHERAL_1_BASE_ADDR) + 0xFFFF

#define PERIPHERAL_4_BASE_ADDR               0x80150000
#define PERIPHERAL_4_END_ADDR                0x8015FFFF

/* Back-up RAM */
#define BACKUP_RAM_BASE_ADDR                 (PERIPHERAL_4_BASE_ADDR)    + 0x0000
#define BACKUP_RAM_END_ADDR                  (PERIPHERAL_4_BASE_ADDR)    + 0x1FFF    

/* Real Time Clock Registers */
#define RTC_REG_BASE_ADDR                    (PERIPHERAL_4_BASE_ADDR)  + 0x4000
#define RTC_REG_END_ADDR                     (PERIPHERAL_4_BASE_ADDR)  + 0x4FFF

/* Dynamic Memory Controller Configuration registers */
#define DMC_CTRL_REG_BASE_ADDR              (PERIPHERAL_4_BASE_ADDR)  + 0x6000
#define DMC_CTRL_REG_END_ADDR               (PERIPHERAL_4_BASE_ADDR)  + 0x6FFF

/* Power, Clock and Reset Management Unit Registers */
#define PRCMU_REG_BASE_ADDR                  (PERIPHERAL_4_BASE_ADDR)  + 0x7000
#define PRCMU_REG_END_ADDR                   (PERIPHERAL_4_BASE_ADDR)  + 0x7FFF

/* Embedded boot ROM (128KBytes) */
#define BOOT_ROM_BASE_ADDR                   0x90000000
#define BOOT_ROM_END_ADDR                    0x9001FFFF

/* DSI-1 link configuration registers */
#define DSI1_LINK_CFG_REG_BASE_ADDR          0xA0351000
#define DSI1_LINK_CFG_REG_END_ADDR           0xA0351FFF

/* DSI-2 link configuration registers */
#define DSI2_LINK_CFG_REG_BASE_ADDR          0xA0352000
#define DSI2_LINK_CFG_REG_END_ADDR           0xA0352000

/* DSI-3 link configuration registers */
#define DSI3_LINK_CFG_REG_BASE_ADDR          0xA0353000
#define DSI3_LINK_CFG_REG_END_ADDR           0xA0353000

/* Display Controller Configuration registers */
#define DISPLAY_CTRL_REG_BASE_ADDR           0xA0350000  
#define DISPLAY_CTRL_REG_END_ADDR            0xA0350FFF           

#if defined(ST_8500ED) || defined(ST_HREFED) || defined(__PEPS_8500)

/* DMA Controller configuration registers */
#define DMA_CTRL_REG_BASE_ADDR               0xA0362000
#define DMA_CTRL_REG_END_ADDR                0xA0362FFF


#elif defined(ST_8500V1) || defined(ST_HREFV1) || defined(__PEPS_8500_V1) || defined(__PEPS_8500_V2) || defined(ST_8500V2) || defined(ST_HREFV2)

/* DMA Controller configuration registers */
#define DMA_CTRL_REG_BASE_ADDR               0x801C0000
#define DMA_CTRL_REG_END_ADDR                0x801C0FFF

#endif


/* SBAG configuration registers */
#define SBAG_REG_BASE_ADDR                   0xA0390000
#define SBAG_REG_END_ADDR                    0xA039FFFF


#define PERIPHERAL_6_BASE_ADDR               0xA03C0000
#define PERIPHERAL_6_END_ADDR                0xA03CFFFF

/* RNG configuration registers */
#define RNG_CFG_REG_BASE_ADDR                (PERIPHERAL_6_BASE_ADDR) + 0x0000
#define RNG_CFG_REG_END_ADDR                 (PERIPHERAL_6_BASE_ADDR) + 0x0FFF

/* HASH 0  Processor registers */
#define HASH_0_BASE_ADDR                     (PERIPHERAL_6_BASE_ADDR) + 0x1000
#define HASH_0_END_ADDR                      (PERIPHERAL_6_BASE_ADDR) + 0x1FFF

/* HASH 1  Processor registers */
#define HASH_1_BASE_ADDR                     (PERIPHERAL_6_BASE_ADDR) + 0x2000
#define HASH_1_END_ADDR                      (PERIPHERAL_6_BASE_ADDR) + 0x2FFF

/* Public Key Accelerator registers */
#define PKA_REG_BASE_ADDR                    (PERIPHERAL_6_BASE_ADDR) + 0x4000
#define PKA_REG_END_ADDR                     (PERIPHERAL_6_BASE_ADDR) + 0x5FFF

/* Public Key Accelerator memory 
#define PKA_MEM_BASE_ADDR                    (PERIPHERAL_6_BASE_ADDR) + 0x5100
#define PKA_MEM_END_ADDR                     (PERIPHERAL_6_BASE_ADDR) + 0x5FFF
*/


/* Crypto 0 processor configuration/data registers */
#define CRYP_0_REG_BASE_ADDR                 (PERIPHERAL_6_BASE_ADDR) + 0xA000
#define CRYP_0_REG_END_ADDR                  (PERIPHERAL_6_BASE_ADDR) + 0xAFFF

/* Crypto 1 processor configuration/data registers */
#define CRYP_1_REG_BASE_ADDR                 (PERIPHERAL_6_BASE_ADDR) + 0xB000
#define CRYP_1_REG_END_ADDR                  (PERIPHERAL_6_BASE_ADDR) + 0xBFFF

/* Peripheral 6 Clock and Reset Control */
#define PRCC_6_CTRL_REG_BASE_ADDR            (PERIPHERAL_6_BASE_ADDR) + 0xF000 
#define PRCC_6_CTRL_REG_END_ADDR             (PERIPHERAL_6_BASE_ADDR) + 0xFFFF

#if defined(ST_8500ED) || defined(ST_HREFED) || defined(__PEPS_8500)


#define PERIPHERAL_7_BASE_ADDR               0xA03D0000
#define PERIPHERAL_7_END_ADDR                0xA03DFFFF

/* Multi Timer Unit 0 registers */
#define MTU_0_REG_BASE_ADDR                  (PERIPHERAL_7_BASE_ADDR) + 0xA000
#define MTU_0_REG_END_ADDR                   (PERIPHERAL_7_BASE_ADDR) + 0xAFFF

/* Multi Timer Unit 1 registers */
#define MTU_1_REG_BASE_ADDR                  (PERIPHERAL_7_BASE_ADDR) + 0xB000
#define MTU_1_REG_END_ADDR                   (PERIPHERAL_7_BASE_ADDR) + 0xBFFF

/* Peripheral 7 Clock and Reset Control */
#define PRCC_7_CTRL_REG_BASE_ADDR            (PERIPHERAL_7_BASE_ADDR) + 0xF000 
#define PRCC_7_CTRL_REG_END_ADDR             (PERIPHERAL_7_BASE_ADDR) + 0xFFFF


#elif defined(ST_8500V1) || defined(ST_HREFV1) || defined(__PEPS_8500_V1)|| defined(__PEPS_8500_V2) || defined(ST_8500V2) || defined(ST_HREFV2)

/* Multi Timer Unit 0 registers */
#define MTU_0_REG_BASE_ADDR                  (PERIPHERAL_6_BASE_ADDR) + 0x6000
#define MTU_0_REG_END_ADDR                   (PERIPHERAL_6_BASE_ADDR) + 0x6FFF


/* Multi Timer Unit 1 registers */
#define MTU_1_REG_BASE_ADDR                  (PERIPHERAL_6_BASE_ADDR) + 0x7000
#define MTU_1_REG_END_ADDR                   (PERIPHERAL_6_BASE_ADDR) + 0x7FFF


#endif


#define PERIPHERAL_5_BASE_ADDR               0xA03E0000
#define PERIPHERAL_5_END_ADDR                0xA03FFFFF

/* USB OTG HS Controller registers */
#define USB_REG_BASE_ADDR                    (PERIPHERAL_5_BASE_ADDR) + 0x00000
#define USB_REG_END_ADDR                     (PERIPHERAL_5_BASE_ADDR) + 0x0FFFF

/* GPIO 8 Interface registers  */
#define GPIO_8_REG_BASE_ADDR                 (PERIPHERAL_5_BASE_ADDR) + 0x1E000
#define GPIO_8_REG_END_ADDR                  (PERIPHERAL_5_BASE_ADDR) + 0x1EFFF

/* Peripheral 5 Clock and Reset Control */
#define PRCC_5_CTRL_REG_BASE_ADDR            (PERIPHERAL_5_BASE_ADDR) + 0x1F000 
#define PRCC_5_CTRL_REG_END_ADDR             (PERIPHERAL_5_BASE_ADDR) + 0x1FFFF

/* GIC Controller configuration registers */
#define GIC_0_CTRL_REG_BASE_ADDR             0xA0410100
#define GIC_0_CTRL_REG_END_ADDR              0xA041FFFF

/* Embedded boot ROM */
#define EMBEDDED_ROM_BASE_ADDR               0xFFFE0000
#define EMBEDDED_ROM_END_ADDR                0xFFFFDBFF

#endif /*__INC_MEMORY_MAPPING_H */
