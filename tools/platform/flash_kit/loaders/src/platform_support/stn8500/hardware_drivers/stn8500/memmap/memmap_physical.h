
/* Application */
#define MEMMAP_PHY_APP_RAM_START             0xA0000000
#define MEMMAP_PHY_APP_RAM_SIZE              0x01FEF000
#define MEMMAP_PHY_APP_RAM_END               0xA1FEEFFF

/* Audio */
#define MEMMAP_PHY_AUD_RAM_START             MEMMAP_PHY_APP_RAM_START
#define MEMMAP_PHY_AUD_RAM_SIZE              MEMMAP_PHY_APP_RAM_SIZE
#define MEMMAP_PHY_AUD_RAM_END               MEMMAP_PHY_APP_RAM_END

/* Access */
/*
 * A 4 KiByte static region required by the L220 cache controller driver.
 * It is shared between all CPUs.
 */
#define MEMMAP_PHY_L220_DRIVER_REGION_START     0xA1FEF000
#define MEMMAP_PHY_L220_DRIVER_REGION_SIZE      0x00001000
#define MEMMAP_PHY_L220_DRIVER_REGION_END       0xA1FEFFFF

/* Boot Stage Parameters */
#define MEMMAP_PHY_BOOTPARAM0_START          0xA1FF0000
#define MEMMAP_PHY_BOOTPARAM0_SIZE           0x00008000
#define MEMMAP_PHY_BOOTPARAM0_END            0xA1FF7FFF
#define MEMMAP_PHY_BOOTPARAM1_START          0xA1FF8000
#define MEMMAP_PHY_BOOTPARAM1_SIZE           0x00008000
#define MEMMAP_PHY_BOOTPARAM1_END            0xA1FFFFFF

/* - IO: IO area. */
/* TODO: Get from level0-files */
#define MEMMAP_PHY_IO_PERIPH_START           0xE0000000
#define MEMMAP_PHY_IO_PERIPH_SIZE            0x10000000

/* - BOOTROM: Boot ROM. */
#define MEMMAP_PHY_BOOTROM_START             0xFFFF0000
#define MEMMAP_PHY_BOOTROM_SIZE              0x00010000

#define MEMMAP_PHY_IO_XGAM_START              0xE8000000
#define MEMMAP_PHY_IO_XGAM_SIZE               (60 * 1024)
#define MEMMAP_PHY_IO_XGAM_END                (MEMMAP_PHY_IO_XGAM_START + MEMMAP_PHY_IO_XGAM_SIZE - 1)

#define MEMMAP_PHY_IO_AVE4_START              0xF0040000
#define MEMMAP_PHY_IO_AVE4_SIZE               0x38000
#define MEMMAP_PHY_IO_AVE4_END                (MEMMAP_PHY_IO_AVE4_START + MEMMAP_PHY_IO_AVE4_SIZE - 1)


// TEMPORARY workaround since os_listmemories.c expects these constants:

#define MEMMAP_PHY_RAM_START  MEMMAP_PHY_APP_RAM_START
#define MEMMAP_PHY_RAM_SIZE   MEMMAP_PHY_APP_RAM_SIZE

//NOR
#define MEMMAP_PHY_APP_NO_CACHE_NOR_START    0x60000000
#define MEMMAP_PHY_APP_NOR_START             0x80000000
#define MEMMAP_PHY_APP_NOR_SIZE              (512 * 1024 * 1024)
#define MEMMAP_PHY_APP_NOR_END               (MEMMAP_PHY_APP_NOR_START + MEMMAP_PHY_APP_NOR_SIZE - 1)
