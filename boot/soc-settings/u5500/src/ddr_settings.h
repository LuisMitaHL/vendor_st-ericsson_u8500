/*
 *  Copyright (C) 2010 ST-Ericsson AB
 *  Reproduction and Communication of this document is strictly prohibited
 *  unless specifically authorized in writing by ST-Ericsson.
 *
 *
 * File Name     : ddr_settings.h
 * Author        : Mian Yousaf Kaukab <mian.yousaf.kaukab@st-ericsson.com>
 */

#include <prcmu-regs.h>
#include <numerics.h>
#include <xp70_memmap.h>

#define BOOT_OK			0
#define BOOT_INTERNAL_ERROR	0xA0010022
#define SECOND_BOOT_INDICATION	0xF0030002
#define PWR_MNGT_STARTED	0x00000001

#define XP70_IT_EVENT_10 (1 << 0)
#define XP70_IT_EVENT_11 (1 << 1)
#define XP70_IT_EVENT_12 (1 << 2)
#define XP70_IT_EVENT_13 (1 << 3)
#define XP70_IT_EVENT_16 (1 << 4)
#define XP70_IT_EVENT_17 (1 << 5)
#define XP70_IT_EVENT_18 (1 << 6)
#define XP70_IT_EVENT_19 (1 << 7)

/* IO access macro */
#define  IO(addr)  (*((volatile unsigned long *) (addr)))

/* EDB4032B1PB CS defines */
#define EDB4032B1PB_CS0_BC1	0x03   /* CS0 BC-1 : Manufacturer ID */
#define EDB4032B1PB_CS1_BC1	0x03   /* CS1 BC-1 : Manufacturer ID */
#define EDB4032B1PB_CS0_BC4	0x54   /* CS0 BC-4 : I/O width + Density + Type */
#define EDB4032B1PB_CS1_BC4	0x54   /* CS1 BC-4 : I/O width + Density + Type */

/* H8TBR00U0MLR CS defines */
#define H8TBR00U0MLR_CS0_BC1	0x06   /* CS0 BC-1 : Manufacturer ID */
#define H8TBR00U0MLR_CS1_BC1	0x06   /* CS1 BC-1 : Manufacturer ID */
#define H8TBR00U0MLR_CS0_BC4	0x14   /* CS0 BC-4 : I/O width + Density + Type */
#define H8TBR00U0MLR_CS1_BC4	0x14   /* CS1 BC-4 : I/O width + Density + Type */

/* K4P4G324EB_AGC CS defines */
#define K4P4G324EB_CS0_BC1	0x01   /* CS0 BC-1 : Manufacturer ID */
#define K4P4G324EB_CS1_BC1	0x00   /* CS1 BC-1 : Manufacturer ID */
#define K4P4G324EB_CS0_BC4	0x18   /* CS0 BC-4 : I/O width + Density + Type */
#define K4P4G324EB_CS1_BC4	0x00   /* CS1 BC-4 : I/O width + Density + Type */

/* K4P8G304EB_AGC CS defines */
#define K4P8G304EB_CS0_BC1	0x01   /* CS0 BC-1 : Manufacturer ID */
#define K4P8G304EB_CS1_BC1	0x01   /* CS1 BC-1 : Manufacturer ID */
#define K4P8G304EB_CS0_BC4	0x18   /* CS0 BC-4 : I/O width + Density + Type */
#define K4P8G304EB_CS1_BC4	0x18   /* CS1 BC-4 : I/O width + Density + Type */

/* MT42L192M32D3 CS defines */
#define MT42L192M32D3_CS0_BC1	0xff   /* CS0 BC-1 : Manufacturer ID */
#define MT42L192M32D3_CS1_BC1	0xff   /* CS1 BC-1 : Manufacturer ID */
#define MT42L192M32D3_CS0_BC4	0x54   /* CS0 BC-4 : I/O width + Density + Type */
#define MT42L192M32D3_CS1_BC4	0x54   /* CS1 BC-4 : I/O width + Density + Type */

/* Configures DMC and PLL settings */
void hook_dmc_ddr(struct ddr_init *dm_ddr);
#ifdef U5500C
void hook_dmc_333mhz_multiboot(struct ddr_init *dm_ddr);
void hook_dmc_333mhz_twodie(struct ddr_init *dm_ddr);
void hook_dmc_333mhz_onedie(struct ddr_init *dm_ddr);
#else
void hook_dmc_266mhz_multiboot(struct ddr_init *dm_ddr);
void hook_dmc_266mhz_twodie(struct ddr_init *dm_ddr);
void hook_dmc_266mhz_onedie(struct ddr_init *dm_ddr);
#endif
/* Set CPU, DDR, SGA and ICN frequencies */
void set_frequency(struct frequency *set_freq);
/* Set DDR frequency */
void set_memory_speed(struct memory_speed *ddr_speed);
