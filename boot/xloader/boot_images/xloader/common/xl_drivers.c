/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*------------------------------------------------------------------------   */
/*                             Wireless Multimedia                           */
/*                           ROM code and Boot Image Team                    */
/*------------------------------------------------------------------------   */
/* file xl_drivers.c                                                         */
/* author Nicolas Mareau / ST-Ericsson                                       */
/* This module provides Xloaderfile implementation                           */
/* Specification release related to this implementation:                     */
/*   db8500a0 boot image specification.pdf                                   */
/*------------------------------------------------------------------------   */
/*                                                                           */
/*****************************************************************************/

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#include "xloader.h"
#include "issw_printf.h"
#include "xl_drivers.h"

#include "numerics.h"
#include "xp70_memmap.h"

#include "xl_l1_cache_mmu_mgmt.h"
#include "ab8500.h"

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/
#define WAY_NB      8
#define ALL_WAYS    0xFF

#define PRCM_MBOX_CPU_IT17      (1 << 5)

/* SDI2 SD/MMC card interface */
#define SDI2_REG_START_ADDR     0x80005000
#define SDI2_REG_LENGTH         CNTRL_REG_SIZE
#define SDI2_REG_END_ADDR       (SDI2_REG_START_ADDR+SDI2_REG_LENGTH-1)

/* SDI4 SD/MMC card interface */
#define SDI4_REG_START_ADDR     0x80114000
#define SDI4_REG_LENGTH         CNTRL_REG_SIZE
#define SDI4_REG_END_ADDR       (SDI4_REG_START_ADDR+SDI4_REG_LENGTH-1)

#define BOOT_OK             0   /**< SUCCESS, no error.*/
#define MMC_SEND_EXTCSD_SD_IF_COND_8    8
#define MMC_CLEAR_FLAGS                 0x00C007FF
#define MMC_DEFAULT_BLK_SIZE            512
#define MMC_MAX_DLEN_SIZE               0x1FFFFFF
#define MMC_PROG_STATE                  7

/* MMC management related errors */
#define BOOT_ERROR_MMC_TIMEOUT      0xA0010090   /**< MMC error. Timeout while accessing the MMC. */
#define BOOT_ERROR_MMC_CRC_FAIL     0xA0010091   /**< MMC error. CRC error found while reading/sending command. */
#define BOOT_ERROR_MMC_LOCKED       0xA0010092   /**< MMC error. MMC card is locked for reading. */
#define BOOT_ERROR_ADDR_MISALIGNED  0xA0010093   /**< MMC error. Address not block size aligned. */
#define BOOT_ERROR_ILLEGAL_CMD      0xA0010094   /**< MMC error. Command sent not supported. */
#define BOOT_ERROR_MMC_FIFO     0xA0010095   /**< MMC error. SDI Fifo error while reading (overrun...).*/
#define BOOT_ERROR_MMC_INTERNAL_ERR 0xA0010096   /**< MMC error. MMC internal error. */
#define BOOT_ERROR_MMC_BLK_LEN_ERR  0xA0010097   /**< MMC error. Block lenght error.*/
#define BOOT_ERROR_MMC_VOLTAGE_ERR  0xA0010098   /**< MMC error. Voltage error in initialization. */
#define BOOT_ERROR_NOBOOT__PARTITION    0xA0010099   /**< Requested operatrion requires a boot partition but none available */

#define EXT_CSD_PART_CONFIG     179
#define EXT_CSD_REV         192
#define EXT_CSD_BOOT_SIZE_MULTI     226
#define EXT_CSD_PART_CONFIG_ACC_BOOT0   (0x1)
#define EXT_CSD_PART_CONFIG_ACC_BOOT1   (0x2)
#define MASK_BYTE           (0xFFUL)
#define SHIFT_BYTE0         0
#define SHIFT_BYTE1         8
#define SHIFT_BYTE2         16
#define SHIFT_BYTE3         24
#define MASK_BYTE0    (MASK_BYTE << SHIFT_BYTE0)
#define MASK_BYTE1    (MASK_BYTE << SHIFT_BYTE1)
#define MASK_BYTE2    (MASK_BYTE << SHIFT_BYTE2)
#define MASK_BYTE3    (MASK_BYTE << SHIFT_BYTE3)

#define MMC_SEND_CMD(cmdIndex, longResp)        \
    do {                        \
        tmpCmdReg.Reg = 0;          \
        tmpCmdReg.Bit.WaitResp = 1;     \
        tmpCmdReg.Bit.LongResp = (longResp);    \
        tmpCmdReg.Bit.CmdIndex = (cmdIndex);    \
        tmpCmdReg.Bit.CpsmEn = 1;       \
        p_mmcRegister->CMD.Reg = tmpCmdReg.Reg; \
        while (p_mmcRegister->STATUS.CmdAct) {  \
            /* Empty Loop */        \
        }                   \
    } while (0)
/*------------------------------------------------------------------------
 * Private Functions
 *----------------------------------------------------------------------*/

t_uint32 XL_GetLastReset(t_boot_last_reset *p_lastReset);
t_uint8 XL_GetAB8500CutId(void);
void XL_SendToken(t_pub_rom_loader_info *p_loaderInfo, t_uint32 token);
void XL_DisableCachesMmu(void);
PRIVATE t_boot_error MMC_WaitCmdResp(void);
PRIVATE t_boot_error MMC_check_for_error(void);
PRIVATE t_boot_error MMC_CheckR1Resp(t_uint32 cmdIndex);
PRIVATE t_boot_error MMC_GetExtCSD(struct data_collection *data);
PRIVATE t_bool MMC_boot_partitions_supported(struct data_collection *data,
        struct toc_data *toc_data);
/*------------------------------------------------------------------------
 * Public variables declaration
 *----------------------------------------------------------------------*/
static t_MmcType g_MmcType;
static t_MmcType *gp_MmcType;
static t_uint8 mmc_ext_csd[MMC_DEFAULT_BLK_SIZE];
static char tempBlock[MMC_DEFAULT_BLK_SIZE];
volatile t_mmc_register *  p_mmcRegister = NULL;

#ifndef __GNUC__
/* define pragma so code is load at correct location*/
#pragma arm section rodata = ".application"
#endif

t_uint32 XL_GetLastReset(t_boot_last_reset *p_lastReset)
{
    volatile t_prcmu_hw_registers *p_prcmuRegs;
    t_prcmu_reset_status_reg prcmuStatusReg;
    t_uint32 one_reason_printed = 0;

    /* Function shall return an error if no
     * source of reset have been identified.
     */

    *p_lastReset = NO_RESET;
    p_prcmuRegs = (t_prcmu_hw_registers *)PRCMU_REG_START_ADDR;

    prcmuStatusReg.Reg = p_prcmuRegs->prcm_reset_status.Reg;

    XLPUTS("Last reset reason: ");

    /* Test PRCMU status Register values
     * the order is critical since it defines which
     * reset signal has the priority.
     *  Last status read (POR) has the Highest priority
     */
    if (prcmuStatusReg.Bit.apesw_reset) {
        *p_lastReset = APESW_RESET;
        XLPUTS("APESW_RESET");
        one_reason_printed = 1;
    }
    if (prcmuStatusReg.Bit.a9cpu0_wdg_reset) {
        if (one_reason_printed)
            XLPUTS(" + ");
        *p_lastReset = A9CPU0_WDG_RESET;
        XLPUTS("A9CPU0_WDG_RESET");
        one_reason_printed = 1;
    }
    if (prcmuStatusReg.Bit.a9cpu1_wdg_reset) {
        if (one_reason_printed)
            XLPUTS(" + ");
        *p_lastReset = A9CPU1_WDG_RESET;
        XLPUTS("A9CPU1_WDG_RESET");
        one_reason_printed = 1;
    }
    if (prcmuStatusReg.Bit.a9_deep_sleep) {
        if (one_reason_printed)
            XLPUTS(" + ");
        *p_lastReset = A9_DEEP_SLEEP;
        XLPUTS("APE_DEEP_SLEEP");
        one_reason_printed = 1;
    }
    if (prcmuStatusReg.Bit.ape_deep_sleep) {
        if (one_reason_printed)
            XLPUTS(" + ");
        *p_lastReset = APE_DEEP_SLEEP;
        XLPUTS("APE_DEEP_SLEEP");
        one_reason_printed = 1;
    }
    if (prcmuStatusReg.Bit.sec_wdg_reset) {
        if (one_reason_printed)
            XLPUTS(" + ");
        *p_lastReset = SEC_WDG_RESET;
        XLPUTS("SEC_WDG_RESET");
        one_reason_printed = 1;
    }
    if (prcmuStatusReg.Bit.ape_reset) {
        if (one_reason_printed)
            XLPUTS(" + ");
        *p_lastReset = APE_RESET;
        XLPUTS("APE_RESET");
        one_reason_printed = 1;
    }
    if (prcmuStatusReg.Bit.por) {
        if (one_reason_printed)
            XLPUTS(" + ");
        *p_lastReset = POWER_ON_RESET;
        XLPUTS("POWER_ON_RESET");
        one_reason_printed = 1;
    }

    XLPUTS("\n");

    return prcmuStatusReg.Reg;

}

t_uint8 read_hw_i2c(t_uint16 base_addr)
{
    volatile t_prcmu_hw_registers *p_prcmuRegs;

    t_uint8 readValue;

    t_uint8 bank;
    t_uint8 regAdd;

    bank = (base_addr & 0xFF00) >> 8;
    regAdd = base_addr & 0x00FF;

    p_prcmuRegs = (t_prcmu_hw_registers *)PRCMU_REG_START_ADDR;

    TCDMnSec->ReqMb5.un.I2CopType = I2CRead;

    TCDMnSec->ReqMb5.un.SlaveAdd = (bank & 0x1F) | 0x20;

    TCDMnSec->ReqMb5.un.HwGCEn = 0;
    TCDMnSec->ReqMb5.un.StopEn = 1;
    TCDMnSec->ReqMb5.un.RegAdd = regAdd;
    TCDMnSec->ReqMb5.un.RegVal = 0;

    /* Send mb5 interrupt */
    p_prcmuRegs->prcm_mbox_cpu_set = PRCM_MBOX_CPU_IT17;

    /* Check IT for ackMB5 */
    while (((p_prcmuRegs->prcm_arm_it1_val) & PRCM_MBOX_CPU_IT17) !=
            PRCM_MBOX_CPU_IT17)
        ;

    readValue = TCDMnSec->AckMb5.un.RegVal;

    /* Clear IT1 ackmb5 */
    p_prcmuRegs->prcm_arm_it1_clr = PRCM_MBOX_CPU_IT17;

    return readValue;
}

void write_hw_i2c(t_uint16 base_addr, t_uint8 value)
{
    volatile t_prcmu_hw_registers *p_prcmuRegs;

    t_uint8 bank;
    t_uint8 regAdd;

    bank = (base_addr & 0xFF00) >> 8;
    regAdd = base_addr & 0x00FF;

    p_prcmuRegs = (t_prcmu_hw_registers *)PRCMU_REG_START_ADDR;

    TCDMnSec->ReqMb5.un.I2CopType = I2CWrite;

    TCDMnSec->ReqMb5.un.SlaveAdd = (bank & 0x1F) | 0x20;

    TCDMnSec->ReqMb5.un.HwGCEn = 0;
    TCDMnSec->ReqMb5.un.StopEn = 1;
    TCDMnSec->ReqMb5.un.RegAdd = regAdd;
    TCDMnSec->ReqMb5.un.RegVal = value;

    /* Send mb5 interrupt */
    p_prcmuRegs->prcm_mbox_cpu_set = PRCM_MBOX_CPU_IT17;

    /* Check IT for ackMB5 */
    while (((p_prcmuRegs->prcm_arm_it1_val) & PRCM_MBOX_CPU_IT17) !=
            PRCM_MBOX_CPU_IT17)
        ;

    /* Clear IT1 ackmb5 */
    p_prcmuRegs->prcm_arm_it1_clr = PRCM_MBOX_CPU_IT17;
}

/*---------------------------------------------------------------------------*/
/*                         XL_SendToken                                      */
/*---------------------------------------------------------------------------*/
/** \internal
 *
 * Send a TOKEN on active peripheral
 *
 *  \param[out]
 */
/*---------------------------------------------------------------------------*/
void XL_SendToken(t_pub_rom_loader_info *p_loaderInfo, t_uint32 token)
{
    t_secure_rom_boot_info bootInfo;
    t_boot_trace_info *p_bootTrace;
	t_uart_number uartNumber;
    void (**jmpTable) (void);   /* ROM API table */

    /* Get Needed info */
    p_bootTrace = (t_boot_trace_info *) p_loaderInfo->pdebugInfo;
    bootInfo.BootConf = p_loaderInfo->bootInfo;
    jmpTable = (void (**)(void))p_loaderInfo->bootRomJmpTbl;

    switch (p_bootTrace->Bit.DbgBootIf) {
    case IF_UART:
	#ifdef MACH_L8540
		uartNumber = p_loaderInfo->pIfTypes->p_uartType->UartNb;
	#else
		uartNumber = (t_uart_number) (bootInfo.Bit.UartNumber);
	#endif
		ROM_SHARED_FUNC_UART_SEND_DATA(uartNumber,
					       (t_address) & token, (t_size) 4,
					       jmpTable);
        break;
    case IF_USB:
        ROM_SHARED_USB_TRANSMIT_DATA(USB_EP2_NB,
                         (t_address) & (token),
                         4, jmpTable);
        break;
    default:

        break;
    }           /* end switch */
}

/*---------------------------------------------------------------------------*/
/*                         XL_DisableCachesMmu                               */
/*---------------------------------------------------------------------------*/
/** \internal
 *
 * Disable L1 Caches and MMU
 *
 *  \param[out]
 */
/*---------------------------------------------------------------------------*/
void XL_DisableCachesMmu(void)
{
    volatile t_l2cc_registers *const p_L2ccRegs =
        (t_l2cc_registers *)L2CC_REG_START_ADDR;

    /* Disable L1 Caches and MMU */
    XL_DisableDcache();
    XL_DisableIcache();
    /* Invalidates all ways */
    if (p_L2ccRegs->Control.Enable == 1) {
        /* Invalidates all ways */
        p_L2ccRegs->CleanInvByWay.Ways = ALL_WAYS;
        while (p_L2ccRegs->CleanInvByWay.Ways)
            ;
    }

    XL_DisableMmu();
    return;
}

/*---------------------------------------------------------------------------*/
/*                         XL_CleanInvalidateL2                              */
/*---------------------------------------------------------------------------*/
/** \internal
 *
 * Disable L1 Caches and MMU
 *
 *  \param[out]
 */
/*---------------------------------------------------------------------------*/
PUBLIC void XL_CleanInvalidateL2(void)
{
    volatile t_l2cc_registers *const p_L2ccRegs =
        (t_l2cc_registers *)L2CC_REG_START_ADDR;

    if (p_L2ccRegs->Control.Enable == 1) {
        /* Invalidates all ways */
        p_L2ccRegs->CleanInvByWay.Ways = ALL_WAYS;
        while (p_L2ccRegs->CleanInvByWay.Ways)
            ;
    }

    return;
}

PRIVATE t_boot_error MMC_WaitCmdResp(void)
{
    /* Wait until  response or error is received */
    while ((p_mmcRegister->STATUS.CmdRespEnd == 0) &&
            (p_mmcRegister->STATUS.CmdTimeout == 0) &&
            (p_mmcRegister->STATUS.CmdCrcFail == 0)) {
        /* Do Nothing, empty loop */
    }

    /* Check for timeout or CRC errors*/
    if (p_mmcRegister->STATUS.CmdTimeout == 1) {
        p_mmcRegister->ICR.Bit.CmdTimeout = 1;
        return BOOT_ERROR_MMC_TIMEOUT;
    }

    if (p_mmcRegister->STATUS.CmdCrcFail == 1) {
        p_mmcRegister->ICR.Bit.CmdCrcFail = 1;
        return BOOT_ERROR_MMC_CRC_FAIL;
    }

    /* No error, clear static Flags of Status reg */
    p_mmcRegister->ICR.Reg = MMC_CLEAR_FLAGS;

    return BOOT_OK;
}

PRIVATE t_boot_error MMC_CheckR1Resp(t_uint32 cmdIndex)
{
    t_boot_error error;

    do {
        /* wait for the cmd*/
        error = MMC_WaitCmdResp();
        if (error != BOOT_OK)
            return error;

        /* Check if the received response is of desired command */
        if (p_mmcRegister->RESPCMD != cmdIndex)
            XLPRINTF("Expecting a response for CMD %d got response for %d\n",
                    cmdIndex, p_mmcRegister->RESPCMD);
    } while (p_mmcRegister->RESPCMD != cmdIndex);


    /* Analyse R1 resp  */
    if (!(p_mmcRegister->RESP0.Reg & 0xE5F82000)) {
        /* No error bit set*/
        return BOOT_OK;
    }

    /* Find the set bit error */
    if (p_mmcRegister->RESP0.R1Bit.OutRangeAddr == 1)
        return BOOT_ERROR_ADDRESS_OUT_OF_RANGE;

    if (p_mmcRegister->RESP0.R1Bit.MisalignAddr == 1)
        return BOOT_ERROR_ADDR_MISALIGNED;

    if (p_mmcRegister->RESP0.R1Bit.BlkLenErr == 1)
        return BOOT_ERROR_MMC_BLK_LEN_ERR;

    if (p_mmcRegister->RESP0.R1Bit.CmdCrcErr == 1)
        return BOOT_ERROR_MMC_CRC_FAIL;

    if (p_mmcRegister->RESP0.R1Bit.IllegalCmd == 1)
        return BOOT_ERROR_ILLEGAL_CMD;

    if ((p_mmcRegister->RESP0.R1Bit.CardEccFail == 1) ||
            (p_mmcRegister->RESP0.R1Bit.InternalErr == 1) ||
            (p_mmcRegister->RESP0.R1Bit.LastCmdErr == 1))
        return BOOT_ERROR_MMC_INTERNAL_ERR;

    return BOOT_ERROR_MMC_INTERNAL_ERR;
}

/*---------------------------------------------------------------------------*/
/*                          MMC_GetExtCSD                                    */
/*---------------------------------------------------------------------------*/
/** \internal
 *
 * Ensure that the EXT_CSD contained valid information
 *
 *  \param[out]
 */
/*---------------------------------------------------------------------------*/
PRIVATE t_boot_error MMC_GetExtCSD(struct data_collection *data)
{
    t_uint32 *p_tempBuff;
    t_uint32 count;
    t_boot_error error;
    t_mmc_cmd_reg tmpCmdReg;
    t_mmc_data_ctrl_reg tmpDctrlReg;

    p_tempBuff = (t_uint32 *)mmc_ext_csd;

    /* Set the block size to read */
    if (p_mmcRegister->DCTRL.Bit.DataBlkSize != MMC_DEFAULT_BLK_SIZE) {
        tmpDctrlReg.Reg = 0;
        tmpDctrlReg.Bit.DataBlkSize = MMC_DEFAULT_BLK_SIZE;
        p_mmcRegister->DCTRL.Reg = tmpDctrlReg.Reg;

        /* Send cmd16 to set the block length to 512 bytes */
        p_mmcRegister->ARG = MMC_DEFAULT_BLK_SIZE; /* size in bytes */
        MMC_SEND_CMD(16, 0); /* short Response expected */

        /* wait for the response and check if there are errors*/
        error = MMC_CheckR1Resp(16);
        if (error != BOOT_OK)
            return error;
    }

    /*
     * set data timeout and total data length to
     * read in bytes (block multiple)
     */
    p_mmcRegister->DTIMER = 0x000FFFFF;
    p_mmcRegister->DLEN = MMC_DEFAULT_BLK_SIZE; /*EXT-CSD length */

    /* enable data path */
    tmpDctrlReg.Reg = p_mmcRegister->DCTRL.Reg;
    tmpDctrlReg.Bit.DataTransDir = 1; /* read direction */
    tmpDctrlReg.Bit.DataTransMode = 0; /* Block data transfer mode */
    tmpDctrlReg.Bit.DataTransEn = 1;  /* data path enable */
    p_mmcRegister->DCTRL.Reg = tmpDctrlReg.Reg;

    /* Send cmd8 (SEND_EXTCSD) to get ext csd register */
    p_mmcRegister->ARG = 0;
    /* short Response expected */
    MMC_SEND_CMD(MMC_SEND_EXTCSD_SD_IF_COND_8, 0);

    /* wait for the response and check if there are errors*/
    error = MMC_CheckR1Resp(MMC_SEND_EXTCSD_SD_IF_COND_8);
    if (error != BOOT_OK)
        return error;

    /* Retrieve the read register from the fifo */
    while ((p_mmcRegister->STATUS.RxOverrunErr == 0) &&
            (p_mmcRegister->STATUS.DataCrcFail == 0) &&
            (p_mmcRegister->STATUS.DataTimeout == 0) &&
            (p_mmcRegister->STATUS.DataEnd == 0) &&
            (p_mmcRegister->STATUS.StartBitErr == 0)) {
        if (p_mmcRegister->STATUS.RxFifoBurstRable == 1) {
            /* fifo burst to read in word = 8 */
            for (count = 0; count < 8; count++) {
                *(p_tempBuff + count) = p_mmcRegister->FIFO[0];
            }
            p_tempBuff += 8;
        }
    }
    /* Read of ext csd finished or error occured*/

    /* Check for error */
    if (p_mmcRegister->STATUS.DataTimeout == 1) {
        p_mmcRegister->ICR.Bit.DataTimeout = 1;
        return BOOT_ERROR_MMC_TIMEOUT;
    } else if (p_mmcRegister->STATUS.DataCrcFail == 1) {
        p_mmcRegister->ICR.Bit.DataCrcFail = 1;
        return BOOT_ERROR_MMC_CRC_FAIL;
    } else if (p_mmcRegister->STATUS.RxOverrunErr == 1) {
        p_mmcRegister->ICR.Bit.RxOverrunErr = 1;
        return BOOT_ERROR_MMC_FIFO;
    } else if (p_mmcRegister->STATUS.StartBitErr == 1) {
        p_mmcRegister->ICR.Bit.StartBitErr = 1;
        return BOOT_ERROR_MMC_FIFO;
    }

    /*
     * No error occured, retrieve the remaining data
     * from the fifo if needed
     */
    while (p_mmcRegister->STATUS.RxDataAvail) {
        *p_tempBuff = p_mmcRegister->FIFO[0];
        p_tempBuff++;
    }

    /* Clear static Flags of Status reg */
    p_mmcRegister->ICR.Reg = MMC_CLEAR_FLAGS;

    return BOOT_OK;
}

PRIVATE t_bool MMC_boot_partitions_supported(struct data_collection *data,
            struct toc_data *toc_data)
{
    static t_bool first_time = TRUE;
    static t_bool cached_value;
    t_boot_error ext_csd_error;

    if (first_time) {
        first_time = FALSE;
        ext_csd_error =  MMC_GetExtCSD(data);
        if (BOOT_OK != ext_csd_error) {
            XLPRINTF("Could not read MMC ext_csd! error = %d\n",
                    ext_csd_error);
            cached_value = FALSE;
        } else {
            if (mmc_ext_csd[EXT_CSD_REV] >= 3) {
                cached_value = !!mmc_ext_csd[EXT_CSD_BOOT_SIZE_MULTI];
            } else {
                XLPRINTF("MMC boot partitions not supported in "
                        "revision %d\n",
                        mmc_ext_csd[EXT_CSD_REV]);
                cached_value = FALSE;
            }
        }
    }
    return cached_value;
}

t_boot_error MMC_switch_to_boot_partition(struct data_collection *data,
            struct toc_data *toc_data, t_uint32 boot_part)
{
    if (!MMC_boot_partitions_supported(data, toc_data)) {
        return BOOT_ERROR_NOBOOT__PARTITION;
    } else {
        t_mmc_cmd_reg tmpCmdReg;
        t_boot_error ext_csd_error;
        t_mmc_status_reg status;
        t_uint8 resp0Status;
        t_uint32 loop_count;

        /* Send cmd6 to enable access to partition set in boot_part
         ** [31:26] Set to 0,
         ** [25:24] Access:3  Write Byte,
         ** [23:16] Index  [179], EXT_CSD_PART_CONFIG
         ** [15:8]  Value   Boot Partition
         ** [7:3]   Set to 0,
         ** [2:0]   Command Set 0 ignored
         */
        p_mmcRegister->ARG = 0x03000000 |
            (((t_uint32)EXT_CSD_PART_CONFIG << SHIFT_BYTE2) & MASK_BYTE2) |
            (((t_uint32)(0xFF & boot_part) << SHIFT_BYTE1) & MASK_BYTE1);
        MMC_SEND_CMD(6, 0); /* short Response expected for SWITCH cmd */

        ext_csd_error = 0xff;
        /* wait for the response and check if there are errors*/
        ext_csd_error = MMC_CheckR1Resp(6);
        if (ext_csd_error != BOOT_OK) {
            XLPRINTF("Not able to switch to Partition %d "
                    ": error = %d\n",
                    boot_part, ext_csd_error);
            return ext_csd_error;
        }
        /*  For an R1b response need to wait until
         ** the card is no longer busy.
         ** Limit to 1000 retries to prevent lock-ups
         */
        loop_count = 1000;
        do {
            /* Clear the Error flags */
            p_mmcRegister->ICR.Reg = MMC_CLEAR_FLAGS;
            p_mmcRegister->ARG =
                /* rca : [31:16] RCA  [15:0] stuff bits */
                (t_uint32)((gp_MmcType->rca) << SHIFT_BYTE2);
            MMC_SEND_CMD(13, 0); /* short Response expected */
            ext_csd_error = MMC_CheckR1Resp(13);
            loop_count--;
        } while (loop_count &&
                ((BOOT_ERROR_MMC_TIMEOUT == ext_csd_error) ||
                 (p_mmcRegister->RESP0.R1Bit.State == MMC_PROG_STATE) ||
                 (p_mmcRegister->RESP0.R1Bit.ReadyForData != 1)));

        if (ext_csd_error != BOOT_OK) {
            XLPRINTF(
                "switch_to_boot_partition: "
                "CMD13 response not okay!! : error = %d"
                " loop_count = %d\n",
                ext_csd_error, loop_count);
            return ext_csd_error;
        }
        return BOOT_OK;
    }
}

inline t_MmcType *get_MmcType(void)
{
    return gp_MmcType;
}

t_uint8 emmc_voltage_check(void)
{
        /* Voltage Check before accessing the eMMC*/

    t_uint8 vaux = read_hw_i2c(AB8500_VAUX2SEL_REG);

    if ((vaux > AB8500_VAUX2_2V7) && (vaux <= AB8500_VAUX2_MAX))
                XLPRINTF("Proper voltage 0x%x applied to eMMC\n", vaux);
    else {
        write_hw_i2c(AB8500_VAUX2SEL_REG, AB8500_VAUX2_2V9);
        vaux = read_hw_i2c(AB8500_VAUX2SEL_REG);
        XLPRINTF("EMMC:Set to Default 0x0%x as Voltage is not in the Range!\n",
                 vaux);
    }
    return vaux;
}

t_boot_error init_emmc(struct data_collection *data)
{
    t_boot_error ret_code = BOOT_OK;
    t_mmc_init_conf mmcInitConf;
	void * p_MmcType;

    if (data == NULL) {
        XLPUTS(("Mandatory parameter are null\n"));
        ret_code = BOOT_ERROR_NULL_POINTER;
        goto error;
    }
#ifdef MACH_L8540
	p_MmcType = data->loader_info->pIfTypes->p_mmcType;
#else
	p_MmcType = data->loader_info->pMemType;
#endif
    if (data->boot_if == IF_UART || data->boot_if == IF_USB ||
		((p_MmcType != (void *)SDI2_REG_START_ADDR) &&
		(p_MmcType != (void *)SDI4_REG_START_ADDR))) {
        mmcInitConf.mmcBusConf = MMC_1BIT_BUS;
        mmcInitConf.mmcNb = MMC_SDI2;
        mmcInitConf.lowVoltageInit = TRUE;
        mmcInitConf.mmcFreqConf = MMC_CLKDIV_17MHZ_FOR_50MHZ;

        XLPRINTF("Probing for eMMC on SDI2\n");
        /* Test Mmc Detection function */
        ret_code = ROM_SHARED_MMC_DETECT_AND_INIT(
                &mmcInitConf, &g_MmcType,
                data->jmp_table);
        if (ret_code != BOOT_OK) {
            mmcInitConf.mmcNb = MMC_SDI4;
            XLPRINTF("Probing for eMMC on SDI4\n");
            ret_code = ROM_SHARED_MMC_DETECT_AND_INIT(&mmcInitConf,
                    &g_MmcType,
                    data->jmp_table);
            if (ret_code == BOOT_OK)
                p_mmcRegister =
                    (t_mmc_register*)(SDI4_REG_START_ADDR);
            else
                XLPRINTF(
                    "probing failed! Could not find MMC\n");
        } else {
            p_mmcRegister = (t_mmc_register*)(SDI2_REG_START_ADDR);
        }
        if (ret_code != BOOT_OK) {
            XLPRINTF("Initiating emmc failed %d\n", ret_code);
            goto error;
        }

        gp_MmcType = &g_MmcType;
    } else {
        /* If MMC boot then reconfigure for 4-bit if required */
		gp_MmcType = p_MmcType;
    }

error:
    return ret_code;
}
/* end of xl-drivers.c */
