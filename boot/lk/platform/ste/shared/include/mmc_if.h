#ifndef _MMC_IF_H
#define _MMC_IF_H
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

#include "mmc_types.h"

#define uS1000              1000
#define uS2000              2000
#define OP_COND_RETRIES     1000

/* JESD84-A441 8.3 */
#define SPEC_VERS_UNKNOWN   0xffff
#define SPEC_VERS_MMCA_0    0
#define SPEC_VERS_MMCA_1    1
#define SPEC_VERS_MMCA_2    2
#define SPEC_VERS_MMCA_3    3
#define SPEC_VERS_MMC_4     4


/* SD card types,
 * SD Host Controller Simplified Specification Version 3.00 */
#define SDSC_V1x            5
#define SDSC_V2x            6
#define SDSC_SDxC           7
#define SDSC_SDIO           8


/* Data transfer direction */
#define DATA_READ   0
#define DATA_WRITE  1


/* Command id's, JESD84-A441 7.10.4 */
#define MC_GO_IDLE_STATE           0
#define MC_SEND_OP_COND            1
#define MC_ALL_SEND_CID            2
#define MC_SET_RELATIVE_ADDR       3
#define MC_SET_DSR                 4
#define MC_SWITCH                  6
#define MC_SELECT_CARD             7
#define MC_SEND_EXT_CSD            8
#define MC_SEND_CSD                9
#define MC_SEND_CID                10
#define MC_STOP_TRANSMISSION       12
#define MC_SEND_STATUS             13
#define MC_SET_BLOCKLEN            16
#define MC_READ_SINGLE_BLOCK       17
#define MC_READ_MULTIPLE_BLOCK     18
#define MC_SET_BLOCK_COUNT         23
#define MC_WRITE_SINGLE_BLOCK      24
#define MC_WRITE_MULTIPLE_BLOCK    25
#define MC_ERASE_GROUP_START       35
#define MC_ERASE_GROUP_END         36
#define MC_ERASE                   38
#define MC_APP_CMD                 55

/* SD specific commands */
#define SD_SEND_OCR                5
#define SD_SET_BUS_WIDTH           6
#define SD_SEND_IF_COND            8
#define SD_SEND_OP_COND            41
#define SD_SEND_SCR                51


#define RESP_SIZE       4

typedef enum {
    MC_RESP_NONE = 0,
    MC_RESP_R1,
    MC_RESP_R1b,
    MC_RESP_R2,
    MC_RESP_R3,
    MC_RESP_R4,
    MC_RESP_R5,
    MC_RESP_R6,
    MC_RESP_R7
} mc_response_type_e;


#define OCR_BUSY_BIT        0x80000000
#define OCR_SECTOR_MODE     0x40000000


#define MMC_BLOCK_SIZE_512      512
#define ECSD_SIZE               512
#define SCR_SIZE                2
#define CSD_SIZE                4
#define CID_SIZE                4


#define ECSD_WR_REL_PARAM_HS_CTRL_REL	(1 << 0)
#define ECSD_WR_REL_PARAM_EN_REL_WR		(1 << 2)


/* eMMC extended CSD register indexes, JESD84-A441 8.4 */
#define ECSD_WR_REL_PARAM               166
#define ECSD_WR_REL_SET                 167
#define ECSD_BUS_WIDTH                  183
#define ECSD_HS_TIMING                  185
#define ECSD_POWER_CLASS                187
#define ECSD_CARD_TYPE                  196
#define ECSD_SEC_COUNT                  212
#define ECSD_REL_WR_SEC_C               222
#define ECSD_SEC_FEATURE_SUPPORT        231

/* Extended CSD access modes, i.e. operations on ECSD, JESD84-A441 7.6.1 */
#define ECSD_ACCESS_MODE_COMMAND        0x00
#define ECSD_ACCESS_MODE_SET_BITS       0x01
#define ECSD_ACCESS_MODE_CLEAR_BITS     0x02
#define ECSD_ACCESS_MODE_WRITE_BYTE     0x03


/* WR_REL_PARAM bits */
#define HS_CTRL_REL_SET                 0x01
#define EN_REL_WR_SET                   0x04


/* WR_REL_SET bits */
#define WR_DATA_REL_USR_SET             0x01
#define WR_DATA_REL_1_SET               0x02
#define WR_DATA_REL_2_SET               0x04
#define WR_DATA_REL_3_SET               0x08
#define WR_DATA_REL_4_SET               0x80

/* Extended CSD Bus width values */
typedef enum {
    ECSD_BW_1 = 0,
    ECSD_BW_4 = 1,
    ECSD_BW_8 = 2,
    ECSD_BW_4_DDR = 5,
    ECSD_BW_8_DDR = 6,
} ecsd_bus_width_e;


/* SEC_FEATURE_SUPPORT bits*/
#define SEC_FEATURE_ARG_SECURE          0x80000000
#define SEC_FEATURE_ARG_GARBAGE_COLLECT 0x00008000
#define SEC_FEATURE_ARG_ID_WRITE_BLOCK  0x00000001

#define SEC_ER_EN                       0x00000001
#define SEC_GB_CL_EN                    0x00000010

#define BUS_WIDTH_NONE      0x00
#define BUS_WIDTH_1         0x01
#define BUS_WIDTH_4         0x04
#define BUS_WIDTH_8         0x08

#define DDR_DISABLED        0
#define DDR_ENABLED         1

/* HS_TIMING value */
#define HS_TIMING_SET       1

/* CARD_TYPE bits */
#define CARD_TYPE_26        0x01
#define CARD_TYPE_52        0x02
#define CARD_TYPE_52_DD_HV  0x04
#define CARD_TYPE_52_DD_LV  0x08

/* CCS-bit, to indicate if the host supports SDHC / SDXC cards */
#define OCR_HCS             0x40000000

#define REL_WRITE_FALSE     0x00000000
#define REL_WRITE_TRUE      0x80000000


#define INIT_CLOCK          400000
#define MAX_CLOCK           50000000


#define EXPECT_SD           1
#define EXPECT_MMC          0


/*
  MMC status in R1, for native mode (SPI bits are different)
  Type
	e : error bit
	s : status bit
	r : detected and set for the actual command response
	x : detected and set during command execution. the host must poll
            the card by sending status command in order to read these bits.
  Clear condition
	a : according to the card state
	b : always related to the previous command. Reception of
            a valid command will clear it (with a delay of one command)
	c : clear by read
 */

#define R1_CURRENT_STATE(x)	((x & 0x00001E00) >> 9)	/* sx, b (4 bits) */

#define R1_STATE_IDLE	0
#define R1_STATE_READY	1
#define R1_STATE_IDENT	2
#define R1_STATE_STBY	3
#define R1_STATE_TRAN	4
#define R1_STATE_DATA	5
#define R1_STATE_RCV	6
#define R1_STATE_PRG	7
#define R1_STATE_DIS	8


/* Type describing CSD entries */
typedef struct {
    uint32_t  hi;
    uint32_t  lo;
} bit_range_t;


/* eMMC Card Specific Data according to Jedec Standard no. 84-A44 */
typedef enum {
    CSD_STRUCTURE   = 0, /* 2    R    [127:126] */
    SPEC_VERS          , /* 4    R    [125:122] */
    RESERVED_3         , /* 2    R    [121:120] */
    TAAC               , /* 8    R    [119:112] */
    NSAC               , /* 8    R    [111:104] */
    TRAN_SPEED         , /* 8    R    [103:96] */
    CCC                , /*12    R    [95:84] */
    READ_BL_LEN        , /* 4    R    [83:80] */
    READ_BL_PARTIAL    , /* 1    R    [79:79] */
    WRITE_BLK_MISALIGN , /* 1    R    [78:78] */
    READ_BLK_MISALIGN  , /* 1    R    [77:77] */
    DSR_IMP            , /* 1    R    [76:76] */
    RESERVED_2         , /* 2    R    [75:74] */
    C_SIZE             , /*12    R    [73:62] */
    VDD_R_CURR_MIN     , /* 3    R    [61:59] */
    VDD_R_CURR_MAX     , /* 3    R    [58:56] */
    VDD_W_CURR_MIN     , /* 3    R    [55:53] */
    VDD_W_CURR_MAX     , /* 3    R    [52:50] */
    C_SIZE_MULT        , /* 3    R    [49:47] */
    ERASE_GRP_SIZE     , /* 5    R    [46:42] */
    ERASE_GRP_MULT     , /* 5    R    [41:37] */
    WP_GRP_SIZE        , /* 5    R    [36:32] */
    WP_GRP_ENABLE      , /* 1    R    [31:31] */
    DEFAULT_ECC        , /* 2    R    [30:29] */
    R2W_FACTOR         , /* 3    R    [28:26] */
    WRITE_BL_LEN       , /* 4    R    [25:22] */
    WRITE_BL_PARTIAL   , /* 1    R    [21:21] */
    RESERVED_1         , /* 4    R    [20:17] */
    CONTENT_PROT_APP   , /* 1    R    [16:16] */
    FILE_FORMAT_GRP    , /* 1   R/W   [15:15] */
    COPY               , /* 1   R/W   [14:14] */
    PERM_WRITE_PROTECT , /* 1   R/W   [13:13] */
    TMP_WRITE_PROTECT  , /* 1  R/W/E  [12:12] */
    FILE_FORMAT        , /* 2   R/W   [11:10] */
    ECC                , /* 2  R/W/E  [9:8] */
    CRC                , /* 7  R/W/E  [7:1] */
    NOT_USED           , /* 1    1    [0:0] ,always 1 */
    MAX_BITS
} csd_emmc_bits_t;


/* Host specific parameters */
typedef struct {
    struct sdi_registers   *base;
    uint8_t                 host_hs_timing;
    uint32_t                host_clock;
    uint32_t                host_blocksize;
    uint32_t                host_bus_width;
} mmc_host_t;


/* MMC device parameters */
typedef struct {
    mmc_host_t     *host;
    uint32_t        voltage_window;
    uint32_t        spec_vers;
    uint8_t         sector_mode;
    uint32_t        bus_width;
    uint8_t         ddr;
    uint32_t        rel_write;
    uint8_t         device_hs_timing;
    uint32_t        device_clock;
    uint32_t        ocr;
    uint32_t        scr[SCR_SIZE];
    uint32_t        csd[CSD_SIZE];
    uint8_t         ext_csd[ECSD_SIZE];
    uint32_t        cid[CID_SIZE];
    uint16_t        rca;
    uint32_t        tran_speed;
    uint32_t        read_bl_len;
    uint32_t        erase_group_size;
    uint32_t        write_bl_len;
    uint32_t        data_timeout;
    uint32_t        wr_rel_param;
    uint32_t        rel_wr_sec_c;
    uint64_t        device_size;
    uint8_t         is_sd_card;
} mmc_properties_t;


/* MMC command parameters */
typedef struct {
    uint16_t            id;                     /* CMD ID */
    mc_response_type_e  resp;                   /* Response type */
    uint8_t             rsp_long;               /* 1 = long respnse */
    uint8_t             rsp_crc;                /* 1 = response CRC */
    uint32_t            arg;                    /* CMD argument */
    uint32_t            resp_data[RESP_SIZE];   /* Response data */
    uint32_t            flags;                  /* Misc flags */
} mmc_cmd_t;


/* Macro to populate mmc_cmd_t */
#define FILL_CMD(m, i,r,l,c,a,f) \
                 m.id = i;\
                 m.resp = r; \
                 m.rsp_long = l;\
                 m.rsp_crc = c;\
                 m.arg = a;\
                 m.flags = f;


/* MMC data transfer type */
typedef struct {
    uint8_t     rd_wr;                      /* Transfer direction */
    uint32_t    n_blocks;                   /* N blocks to transfer */
    uint32_t    block_size;                 /* Size of one block */
    uint32_t    *data;                      /* Transfer buffer */
} mmc_data_t;


/*---------------------------------------------------------------------
 * MMC inteface functions
 *-------------------------------------------------------------------*/

/*
 * mmc_init
 *
 * Initialize eMMC device
 *
 * Parameters:
 * uint32 sdi_base_address : Base adress of control registerl
 * int    *status          : Updated with status
 * int    expect_sd        : EXPECT_SD if SD-card expected, EXPECT_MMC for mmc
 *                           Host is initated differently for SD/MMC, hence
 *                           this is needed.
 *
 * Returns: pointer to created mmc_properties_t. NULL on failure.
 *          Call mmc_close to destroy struct
 */
mmc_properties_t* mmc_init (uint32_t sdi_base_address,
                            int *status,
                            int expect_sd);


/*
 * mmc_close
 *
 * Close eMMC device
 *
 * Parameters:
 * mmc_properties_t *device : device to close
 *
 * Returns: NULL
 */
mmc_properties_t *mmc_close (mmc_properties_t *device);


/*
 * mmc_read
 *
 * Read n_blocks of data to dest, starting at start_block.
 *
 * Parameters:
 * mmc_properties_t   *device
 * uint32        start_block
 * uint32        n_blocks
 * uint32       *dest
 *
 * Returns: mmc_result
 */
int mmc_read (mmc_properties_t *device,
              uint32_t    start_block,
              uint32_t    n_blocks,
              uint32_t   *dest);


/*
 * mmc_write
 *
 * Write n_blocks of data to eMMC from source, starting at start_block.
 *
 * Parameters:
 * mmc_properties_t   *device
 * uint32        start_block
 * uint32        n_blocks
 * uint32       *source
 *
 * Returns: mmc_result
 */
int mmc_write (mmc_properties_t    *device,
               uint32_t       start_block,
               uint32_t       n_blocks,
               uint32_t      *source);


/*
 * mmc_erase
 *
 * Erase n_blocks, starting at start_block
 *
 * Parameters:
 * mmc_properties_t   *device
 * uint32        start_block
 * uint32        n_blocks
 *
 * Returns: mmc_result
 */
int mmc_erase (mmc_properties_t    *device,
               uint32_t       start_block,
               uint32_t       n_blocks);

/*
 * mmc_sd_detected
 *
 * Detect SD card
 *
 * Parameters: -
 *
 * Returns: 0 if no card is detected, 1 otherwise
 */
int mmc_sd_detected (void);

/*
 * csd_get_value
 *
 * Return the value of the requested csd bits
 *
 * Parameters:
 * csd_emmc_bits_t bits
 * uint32          *csd
 *
 * Returns: Value of the bits
 */
uint32_t csd_get_value(csd_emmc_bits_t bits, uint32_t *csd);

#endif /* _MMC_IF_H */
