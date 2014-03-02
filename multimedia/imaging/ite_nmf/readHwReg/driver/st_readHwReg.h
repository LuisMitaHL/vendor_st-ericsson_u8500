/*
 * Copyright (C) ST-Ericsson SA 2010
 * Author: Chetan Nanda/chetan.nanda@stericsson.com for ST-Ericsson.
 * License terms: GNU General Public License (GPL), version 2.
 */
 
/*SIA/ISP base address from ARM side core*/ 
#define ISP_REGION_IO                						0xE0000000
#define SIA_ISP_REG_ADDR									(0x521E4)
#define SIA_SIA_BASE_ADDR                                   (0x54000)
#define SIA_ISP_MCU_SYS_ADDR0_OFFSET                        (SIA_SIA_BASE_ADDR + 0x40)
#define SIA_ISP_MCU_SYS_SIZE0_OFFSET                        (SIA_SIA_BASE_ADDR + 0x42)

#define SIA_ISP_MCU_SYS_ADDR1_OFFSET                        (SIA_ISP_MCU_SYS_ADDR0_OFFSET +  0x04)
#define SIA_ISP_MCU_SYS_SIZE1_OFFSET                        (SIA_ISP_MCU_SYS_SIZE0_OFFSET +  0x04)

#define SIA_ISP_MCU_IO_ADDR_0_OFFSET                        (SIA_SIA_BASE_ADDR + 0x60)