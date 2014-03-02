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


#include <ctype.h>
#include <debug.h>
#include <stdlib.h>
#include <printf.h>
#include <list.h>
#include <string.h>
#include <arch/ops.h>
#include <platform.h>
#include <platform/debug.h>
#include <platform/timer.h>
#include "mmc_if.h"
#include "target_config.h"
#include "target.h"

#ifdef WITH_LIB_CONSOLE
#include <lib/console.h>

#define FIRST_MMC_DEVICE  0
#define SECOND_MMC_DEVICE 1
#define THIRD_MMC_DEVICE  2

mmc_properties_t *mmc;
bool             mmc_initiated = false;
int              device = FIRST_MMC_DEVICE;

static int cmd_mmc(int argc, const cmd_args *argv);

STATIC_COMMAND_START
#if DEBUGLEVEL > 0
    { "mmc", "MMC handling\n"
                  "Options: init, read, write, list, md, mw and info\n"
                  "    init  : Initiate, [dev]\n"
                  "    read  : Read, syntax dev to_address start_blk nr_of_blk\n"
                  "    write : Write, syntax dev from_address start_blk nr_of_blk\n"
                  "    list  : Lists available MMC/SD devices\n"
                  "    md    : Display data, syntax address\n"
                  "    mw    : Modify word, syntax address value\n"
                  "    info  : Print information, syntax [csd] or [ext_csd]",
                  &cmd_mmc },
#endif
STATIC_COMMAND_END(cmd_mmc);

void print_mmcinfo(void)
{
    uint32_t csd_data;
    uint32_t unit;
    uint32_t multiplier;
    uint32_t frq;

    printf("Manufacturer ID: %x\n", mmc->cid[0] >> 24);
    printf("OEM: %x\n", (mmc->cid[0] >> 8) & 0xffff);
    printf("Name: %c%c%c%c%c \n", mmc->cid[0] & 0xff,
            (mmc->cid[1] >> 24), (mmc->cid[1] >> 16) & 0xff,
            (mmc->cid[1] >> 8) & 0xff, mmc->cid[1] & 0xff);

    csd_data = csd_get_value(TRAN_SPEED, mmc->csd);
    unit = csd_data & 0x7;
    switch (unit) {
        case 0: frq = 10000;    // divided by 10
                break;
        case 1: frq = 100000;   // divided by 10
                break;
        case 2: frq = 1000000;  // divided by 10
                break;
        case 3: frq = 10000000; // divided by 10
                break;
        default: frq = 0;
    }
    multiplier = (csd_data >> 3) & 0xf;
    switch (multiplier) {
        case  0: frq = 0;
                 break;
        case  1: frq *= 10;
                 break;
        case  2: frq *= 12;
                break;
        case  3: frq *= 13;
                 break;
        case  4: frq *= 15;
                 break;
        case  5: frq *= 20;
                 break;
        case  6: frq *= 26;
                 break;
        case  7: frq *= 30;
                 break;
        case  8: frq *= 35;
                 break;
        case  9: frq *= 40;
                 break;
        case 10: frq *= 45;
                 break;
        case 11: frq *= 52;
                 break;
        case 12: frq *= 55;
                 break;
        case 13: frq *= 60;
                 break;
        case 14: frq *= 70;
                 break;
        case 15: frq *= 80;
                 break;
        default: frq = 0;
    }
    printf("Tran Speed: %d\n", frq);
    printf("Read Block Len: %d\n", mmc->read_bl_len);

    printf("%s version %d.%d\n",
           device == THIRD_MMC_DEVICE ? "SD" : "MMC",
            (mmc->spec_vers >> 4) & 0xf, mmc->spec_vers & 0xf);

    csd_data = csd_get_value(C_SIZE, mmc->csd);
    printf("High Capacity: %s\n", csd_data == 0xFFF ? "Yes" : "No");
    printf("Capacity: %lld\n", mmc->device_size);

    printf("Bus Width: %d-bit\n", mmc->bus_width);
}

void print_csd(void)
{
    char *str[MAX_BITS] = {
    "CSD_STRUCTURE      ", /* 2    R    [127:126] */
    "SPEC_VERS          ", /* 4    R    [125:122] */
    "RESERVED_3         ", /* 2    R    [121:120] */
    "TAAC               ", /* 8    R    [119:112] */
    "NSAC               ", /* 8    R    [111:104] */
    "TRAN_SPEED         ", /* 8    R    [103:96] */
    "CCC                ", /*12    R    [95:84] */
    "READ_BL_LEN        ", /* 4    R    [83:80] */
    "READ_BL_PARTIAL    ", /* 1    R    [79:79] */
    "WRITE_BLK_MISALIGN ", /* 1    R    [78:78] */
    "READ_BLK_MISALIGN  ", /* 1    R    [77:77] */
    "DSR_IMP            ", /* 1    R    [76:76] */
    "RESERVED_2         ", /* 2    R    [75:74] */
    "C_SIZE             ", /*12    R    [73:62] */
    "VDD_R_CURR_MIN     ", /* 3    R    [61:59] */
    "VDD_R_CURR_MAX     ", /* 3    R    [58:56] */
    "VDD_W_CURR_MIN     ", /* 3    R    [55:53] */
    "VDD_W_CURR_MAX     ", /* 3    R    [52:50] */
    "C_SIZE_MULT        ", /* 3    R    [49:47] */
    "ERASE_GRP_SIZE     ", /* 5    R    [46:42] */
    "ERASE_GRP_MULT     ", /* 5    R    [41:37] */
    "WP_GRP_SIZE        ", /* 5    R    [36:32] */
    "WP_GRP_ENABLE      ", /* 1    R    [31:31] */
    "DEFAULT_ECC        ", /* 2    R    [30:29] */
    "R2W_FACTOR         ", /* 3    R    [28:26] */
    "WRITE_BL_LEN       ", /* 4    R    [25:22] */
    "WRITE_BL_PARTIAL   ", /* 1    R    [21:21] */
    "RESERVED_1         ", /* 4    R    [20:17] */
    "CONTENT_PROT_APP   ", /* 1    R    [16:16] */
    "FILE_FORMAT_GRP    ", /* 1   R/W   [15:15] */
    "COPY               ", /* 1   R/W   [14:14] */
    "PERM_WRITE_PROTECT ", /* 1   R/W   [13:13] */
    "TMP_WRITE_PROTECT  ", /* 1  R/W/E  [12:12] */
    "FILE_FORMAT        ", /* 2   R/W   [11:10] */
    "ECC                ", /* 2  R/W/E  [9:8] */
    "CRC                ", /* 7  R/W/E  [7:1] */
    "NOT_USED           " /* 1    1    [0:0] ,always 1 */
    };
    int ii;

    for (ii = 0; ii < 4; ii++) {
        printf("CSD[%d]              0x%08x\n", ii, mmc->csd[ii]);
    }
    printf("\n");

    for (ii = CSD_STRUCTURE; ii < MAX_BITS; ii++) {
        printf ("%s 0x%04x\n", str[ii], (uint16_t)csd_get_value(ii, mmc->csd));
    }
}

void print_ext_csd(void)
{
    printf("S_CMD_SET                   0x%02x\n", mmc->ext_csd[504]);
    printf("HPI_FEATURES                0x%02x\n", mmc->ext_csd[503]);
    printf("BKOPS_SUPPORT               0x%02x\n", mmc->ext_csd[502]);
    printf("BKOPS_STATUS                0x%02x\n", mmc->ext_csd[246]);
    printf("CORRECTLY_PRG_SECTORS_NUM   0x%08x\n",
        mmc->ext_csd[245] * (1 << 24) | mmc->ext_csd[244] * (1 << 16) |
        mmc->ext_csd[243] * (1 << 8) | mmc->ext_csd[242]);
    printf("INI_TIMEOUT_AP              0x%02x\n", mmc->ext_csd[241]);
    printf("PWR_CL_DDR_52_360           0x%02x\n", mmc->ext_csd[239]);
    printf("PWR_CL_DDR_52_195           0x%02x\n", mmc->ext_csd[238]);
    printf("MIN_PERF_DDR_W_8_52         0x%02x\n", mmc->ext_csd[235]);
    printf("MIN_PERF_DDR_R_8_52         0x%02x\n", mmc->ext_csd[234]);
    printf("TRIM_MULT                   0x%02x\n", mmc->ext_csd[232]);
    printf("SEC_FEATURE_SUPPORT         0x%02x\n", mmc->ext_csd[231]);
    printf("SEC_ERASE_MULT              0x%02x\n", mmc->ext_csd[230]);
    printf("SEC_TRIM_MULT               0x%02x\n", mmc->ext_csd[229]);
    printf("BOOT_INFO                   0x%02x\n", mmc->ext_csd[228]);
    printf("BOOT_SIZE_MULTI             0x%02x\n", mmc->ext_csd[226]);
    printf("ACC_SIZE                    0x%02x\n", mmc->ext_csd[225]);
    printf("HC_ERASE_GRP_SIZE           0x%02x\n", mmc->ext_csd[224]);
    printf("ERASE_TIMEOUT_MULT          0x%02x\n", mmc->ext_csd[223]);
    printf("REL_WR_SEC_C                0x%02x\n", mmc->ext_csd[222]);
    printf("HC_WP_GRP_SIZE              0x%02x\n", mmc->ext_csd[221]);
    printf("S_C_VCC                     0x%02x\n", mmc->ext_csd[220]);
    printf("S_C_VCCQ                    0x%02x\n", mmc->ext_csd[219]);
    printf("S_A_TIMEOUT                 0x%02x\n", mmc->ext_csd[217]);
    printf("SEC_COUNT                   0x%08x\n",
        mmc->ext_csd[215] * (1 << 24) | mmc->ext_csd[214] * (1 << 16) |
        mmc->ext_csd[213] * (1 << 8) | mmc->ext_csd[212]);
    printf("MIN_PERF_W_8_52             0x%02x\n", mmc->ext_csd[210]);
    printf("MIN_PERF_R_8_52             0x%02x\n", mmc->ext_csd[209]);
    printf("MIN_PERF_W_8_26_4_52        0x%02x\n", mmc->ext_csd[208]);
    printf("MIN_PERF_R_8_26_4_52        0x%02x\n", mmc->ext_csd[207]);
    printf("MIN_PERF_W_4_26             0x%02x\n", mmc->ext_csd[206]);
    printf("MIN_PERF_R_4_26             0x%02x\n", mmc->ext_csd[205]);
    printf("PWR_CL_26_360               0x%02x\n", mmc->ext_csd[203]);
    printf("PWR_CL_52_360               0x%02x\n", mmc->ext_csd[202]);
    printf("PWR_CL_26_195               0x%02x\n", mmc->ext_csd[201]);
    printf("PWR_CL_52_195               0x%02x\n", mmc->ext_csd[200]);
    printf("PARTITION_SWITCH_TIME       0x%02x\n", mmc->ext_csd[199]);
    printf("OUT_OF_INTERRUPT_TIME       0x%02x\n", mmc->ext_csd[198]);
    printf("CARD_TYPE                   0x%02x\n", mmc->ext_csd[196]);
    printf("CSD_STRUCTURE               0x%02x\n", mmc->ext_csd[194]);
    printf("EXT_CSD_REV                 0x%02x\n", mmc->ext_csd[192]);
    printf("CMD_SET                     0x%02x\n", mmc->ext_csd[191]);
    printf("CMD_SET_REV                 0x%02x\n", mmc->ext_csd[189]);
    printf("POWER_CLASS                 0x%02x\n", mmc->ext_csd[187]);
    printf("HS_TIMING                   0x%02x\n", mmc->ext_csd[185]);
    printf("BUS_WIDTH                   0x%02x\n", mmc->ext_csd[183]);
    printf("ERASED_MEM_CONT             0x%02x\n", mmc->ext_csd[181]);
    printf("PARTITION_CONFIG            0x%02x\n", mmc->ext_csd[179]);
    printf("BOOT_CONFIG_PROT            0x%02x\n", mmc->ext_csd[178]);
    printf("BOOT_BUS_WIDTH              0x%02x\n", mmc->ext_csd[177]);
    printf("ERASE_GROUP_DEF             0x%02x\n", mmc->ext_csd[175]);
    printf("BOOT_WP                     0x%02x\n", mmc->ext_csd[173]);
    printf("USER_WP                     0x%02x\n", mmc->ext_csd[171]);
    printf("FW_CONFIG                   0x%02x\n", mmc->ext_csd[169]);
    printf("RPMB_SIZE_MULT              0x%02x\n", mmc->ext_csd[168]);
    printf("WR_REL_SET                  0x%02x\n", mmc->ext_csd[167]);
    printf("WR_REL_PARAM                0x%02x\n", mmc->ext_csd[166]);
    printf("BKOPS_START                 0x%02x\n", mmc->ext_csd[164]);
    printf("BKOPS_EN                    0x%02x\n", mmc->ext_csd[163]);
    printf("RST_n_FUNCTION              0x%02x\n", mmc->ext_csd[162]);
    printf("HPI_MGMT                    0x%02x\n", mmc->ext_csd[161]);
    printf("PARTITIONING_SUPPORT        0x%02x\n", mmc->ext_csd[160]);
    printf("MAX_ENH_SIZE_MULT           0x%08x\n",
        mmc->ext_csd[159] * (1 << 16) |
        mmc->ext_csd[158] * (1 << 8) | mmc->ext_csd[157]);
    printf("PARTITIONS_ATTRIBUTE        0x%02x\n", mmc->ext_csd[156]);
    printf("PARTITION_SETTING_COMPLETED 0x%02x\n", mmc->ext_csd[155]);
    printf("GP_SIZE_MULT_GPP4           0x%08x\n",
        mmc->ext_csd[154] * (1 << 16) |
        mmc->ext_csd[153] * (1 << 8) | mmc->ext_csd[152]);
    printf("GP_SIZE_MULT_GPP3           0x%08x\n",
        mmc->ext_csd[151] * (1 << 16) |
        mmc->ext_csd[150] * (1 << 8) | mmc->ext_csd[149]);
    printf("GP_SIZE_MULT_GPP2           0x%08x\n",
        mmc->ext_csd[148] * (1 << 16) |
        mmc->ext_csd[147] * (1 << 8) | mmc->ext_csd[146]);
    printf("GP_SIZE_MULT_GPP1           0x%08x\n",
        mmc->ext_csd[145] * (1 << 16) |
        mmc->ext_csd[144] * (1 << 8) | mmc->ext_csd[143]);
    printf("ENH_SIZE_MULT               0x%08x\n",
        mmc->ext_csd[142] * (1 << 16) |
        mmc->ext_csd[141] * (1 << 8) | mmc->ext_csd[140]);
    printf("ENH_START_ADDR              0x%08x\n",
        mmc->ext_csd[139] * (1 << 24) | mmc->ext_csd[138] * (1 << 16) |
        mmc->ext_csd[137] * (1 << 8) | mmc->ext_csd[136]);
    printf("SEC_BAD_BLK_MGMNT           0x%02x\n", mmc->ext_csd[134]);
}

int cmd_mmc_init(int argc, const cmd_args *argv)
{
    int status;

    if (argc == 3) {
        if (argv[2].i != FIRST_MMC_DEVICE && argv[2].i != SECOND_MMC_DEVICE &&
            argv[2].i != THIRD_MMC_DEVICE) {
            printf("Unknown device %d\n", argv[2].i);
            return -1;
        } else {
            device = argv[2].i;
        }
    }

    switch (device) {
        case FIRST_MMC_DEVICE :
            mmc = mmc_init (U8500_SDI2_BASE, &status, EXPECT_MMC);
            break;
        case SECOND_MMC_DEVICE :
            mmc = mmc_init (U8500_SDI4_BASE, &status, EXPECT_MMC);
            break;
        case THIRD_MMC_DEVICE :
            if (0 > enable_vaux3()) {
                printf("Failed to init VAUX3 for SD-card\n");
                return -1;
            }
            /* Init internal L/S for SDMMC0 */
            if (mmc_sd_set_startup_iosv() < 0) {
                printf("Failed to init L/S for SD-card\n");
                return -1;
            }
            mmc = mmc_init (U8500_SDI0_BASE, &status, EXPECT_SD);
            break;
    }

    if (MMC_OK == status) {
        printf("Initiated mmc device %d\n", device);
        mmc_initiated = true;
        return 0;
    } else {
        printf("Failed to initiate mmc device %d\n", device);
        return -1;
    }
}

int cmd_mmc_read(int argc, const cmd_args *argv)
{
    int      status;

    if (!mmc_initiated) {
        printf("MMC not initiated\n");
        return -1;
    }

    if (argc < 6) {
        printf("not enough arguments\n");
        return -1;
    }

    if (argv[2].i != device) {
        printf("Device %d is not initiated\n", argv[2].i);
        return -1;
    }

    status = mmc_read(mmc, argv[4].i, argv[5].i, (uint32_t*) argv[3].u);
    if (MMC_OK == status) {
        printf("Read %d blocks at start block %d data stored at 0x%08X OK\n",
               argv[5].i, argv[4].i, argv[3].u);
        return 0;
    } else {
        printf("Could not read at block %d\n", argv[4].i);
        return -1;
    }
}

int cmd_mmc_write(int argc, const cmd_args *argv)
{
    int      status;

    if (!mmc_initiated) {
        printf("MMC not initiated\n");
        return -1;
    }

    if (argc < 6) {
        printf("not enough arguments\n");
        return -1;
    }

    if (argv[2].i != device) {
        printf("Device %d is not initiated\n", argv[2].i);
        return -1;
    }

    status = mmc_write(mmc, argv[4].i, argv[5].i, (uint32_t*) argv[3].u);
    if (MMC_OK == status) {
        printf("Written %d blocks at start block %d from address 0x%08X OK\n",
               argv[5].i, argv[4].i, argv[3].u);
        return 0;
    } else {
        printf("Could not write to block %d\n", argv[4].i);
        return -1;
    }
}

int cmd_mmc_list(int argc, const cmd_args *argv)
{
    mmc_properties_t *mmctst;
    int              status;

    if (mmc_initiated) {
        mmctst = mmc_close(mmc);
        mmc_initiated = false;
    }

    mmctst = mmc_init (U8500_SDI2_BASE, &status, EXPECT_MMC);
    if (MMC_OK == status) {
        printf("mmc device %d available at %08X\n",
               FIRST_MMC_DEVICE, U8500_SDI2_BASE);
    } else {
        printf("mmc device %d not available\n", FIRST_MMC_DEVICE);
    }

    mmctst = mmc_init (U8500_SDI4_BASE, &status, EXPECT_MMC);
    if (MMC_OK == status) {
        printf("mmc device %d available at %08X\n",
               SECOND_MMC_DEVICE, U8500_SDI4_BASE);
    } else {
        printf("mmc device %d not available\n", SECOND_MMC_DEVICE);
    }

    if (0 > enable_vaux3()) {
        printf("Failed to init VAUX3 for SD-card\n");
        return -1;
    }
    /* Init internal L/S for SDMMC0 */
    if (mmc_sd_set_startup_iosv() < 0) {
        printf("Failed to init L/S for SD-card\n");
        return -1;
    }
    mmctst = mmc_init (U8500_SDI0_BASE, &status, EXPECT_SD);
    if (MMC_OK == status) {
        printf("mmc (SD) device %d available at %08X\n",
               THIRD_MMC_DEVICE, U8500_SDI0_BASE);
    } else {
        printf("mmc (SD) device %d not available\n", THIRD_MMC_DEVICE);
    }

    return 0;
}

int cmd_mmc_md(int argc, const cmd_args *argv)
{
    uint32_t start_blk;
    uint32_t address;
    uint32_t *data;
    uint32_t ui;
    int      status;
    char     ascii_s[17];
    int      ks = 0;
    uint8_t  c[4];
    int      i;

    if (!mmc_initiated) {
        printf("MMC not initiated\n");
        return -1;
    }

    if (argc < 3) {
        printf("not enough arguments\n");
        return -1;
    }

    address = argv[2].u;
    start_blk = address / mmc->read_bl_len;
    data = (uint32_t*) malloc(mmc->read_bl_len);
    if (NULL == data) {
        printf("Could not allocate data for read\n");
        return -1;
    }

    status = mmc_read(mmc, start_blk, 1, data);
    if (MMC_OK != status) {
        printf("Could not read address %08X\n", address);
        free(data);
        return -1;
    }

    printf("%08x: ", address);
    memset(ascii_s, 0x00, sizeof(ascii_s));
    for (i = 0, ui = (argv[2].u % mmc->read_bl_len / 4); i < 64; i++, ui++) {
        if (i % 4 == 0 && i > 0) {
            printf("    %s\n", ascii_s);
            address += 16;
            ks = 0;
            printf("%08x: ", address);
        }
        printf("%08x ", data[ui]);
        memcpy(c, (uint8_t*) &data[ui], sizeof(uint32_t));
        for (int i = 0; i < 4; i++) {
            if (c[i] > 0x1F) {
                ascii_s[ks++] = c[i];
            } else {
                ascii_s[ks++] = '.';
            }
        }
    }
    printf("    %s\n", ascii_s);
    free(data);

    return 0;
}

int cmd_mmc_mw(int argc, const cmd_args *argv)
{
    uint32_t start_blk;
    uint32_t address;
    uint32_t *data;
    int      status;

    if (!mmc_initiated) {
        printf("MMC not initiated\n");
        return -1;
    }

    if (argc < 4) {
        printf("not enough arguments\n");
        return -1;
    }

    address = argv[2].u;
    start_blk = address / mmc->read_bl_len;
    data = (uint32_t*) malloc(mmc->read_bl_len);
    if (NULL == data) {
        printf("Could not allocate data for read\n");
        return -1;
    }

    status = mmc_read(mmc, start_blk, 1, data);
    if (MMC_OK != status) {
        printf("Could not read address %08X\n", address);
        free(data);
        return -1;
    }

    data[argv[2].u % mmc->read_bl_len / 4] = argv[3].u;
    status = mmc_write(mmc, start_blk, 1, data);
    free(data);
    if (MMC_OK != status) {
        printf("Could not write address %08X\n", address);
        return -1;
    }

    return 0;
}

int cmd_mmc_info(int argc, const cmd_args *argv)
{
    if (!mmc_initiated) {
        printf("MMC not initiated\n");
        return -1;
    }

    print_mmcinfo();

    if (argc == 3) {
        if (strcmp(argv[2].str, "csd") == 0) {
            print_csd();
        } else if (strcmp(argv[2].str, "ext_csd") == 0) {
            print_ext_csd();
        } else {
            printf("Unknown third option %s\n", argv[2].str);
        }
    }

    return 0;
}

static int cmd_mmc(int argc, const cmd_args *argv)
{

    if (argc < 2) {
        printf("not enough arguments\n");
        return -1;
    }

    if (strcmp(argv[1].str, "init") == 0) {
        return cmd_mmc_init(argc, argv);
    }
    if (strcmp(argv[1].str, "read") == 0) {
        return cmd_mmc_read(argc, argv);
    }
    if (strcmp(argv[1].str, "write") == 0) {
        return cmd_mmc_write(argc, argv);
    }
    if (strcmp(argv[1].str, "list") == 0) {
        return cmd_mmc_list(argc, argv);
    }
    if (strcmp(argv[1].str, "md") == 0) {
        return cmd_mmc_md(argc, argv);
    }
    if (strcmp(argv[1].str, "mw") == 0) {
        return cmd_mmc_mw(argc, argv);
    }
    if (strcmp(argv[1].str, "info") == 0) {
        return cmd_mmc_info(argc, argv);
    }

    printf("Unknown option %s to the mmc command\n", argv[1].str);
    return -1;
}
#endif
